/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BuildExecuteStateMachine class.
***********************************************************************************************************************/

#include <QObject>
#include <QCoreApplication>
#include <QString>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <QStandardPaths>

#include <cstring>

#include <util_system.h>

#include <cbe_dynamic_library_loader.h>

#include <ld_element_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_variable_element.h>
#include <ld_code_generator.h>
#include <ld_cpp_code_generator.h>
#include <ld_code_generator_visual.h>
#include <ld_cpp_code_generator_diagnostic.h>
#include <ld_diagnostic.h>
#include <ld_root_element.h>
#include <ld_operation.h>
#include <ld_operation_database.h>
#include <ld_identifier.h>
#include <ld_identifier_container.h>
#include <ld_identifier_database.h>
#include <ld_data_type.h>
#include <ld_calculated_value.h>
#include <ld_data_type_decoder.h>

#include <model_status.h>
#include <model_rng.h>
#include <model_api.h>
#include <model_identifier_data.h>
#include <model_identifier_database.h>
#include <m_per_thread.h>
#include <m_api.h>
#include <model_variant.h>
#include <m_console.h>

#include <mat_api.h>
#include <mkl_api.h>

#include "application.h"
#include "metatypes.h"
#include "console_device.h"
#include "cpp_code_generator_visual.h"
#include "loaded_model_status.h"
#include "runtime_diagnostic.h"
#include "build_execute_state_machine.h"

BuildExecuteStateMachine::BuildExecuteStateMachine(QObject* parent):QObject(parent) {
    currentStatusInstance = new LoadedModelStatus(this);

    currentState               = State::IDLE;
    targetState                = State::IDLE;
    currentDeallocatorFunction = Q_NULLPTR;
    currentModelApi            = Q_NULLPTR;
    currentDebugMode           = false;
    newDebugMode               = false;
    currentSingleStep          = false;

    connect(
        currentStatusInstance,
        &LoadedModelStatus::modelStarted,
        this,
        &BuildExecuteStateMachine::modelStarted
    );
    connect(
        currentStatusInstance,
        &LoadedModelStatus::modelFinished,
        this,
        &BuildExecuteStateMachine::modelFinished
    );
    connect(
        currentStatusInstance,
        &LoadedModelStatus::modelAborted,
        this,
        &BuildExecuteStateMachine::modelAborted
    );
    connect(
        currentStatusInstance,
        &LoadedModelStatus::modelPausedOnUserRequest,
        this,
        &BuildExecuteStateMachine::modelPausedOnUserRequest
    );
    connect(
        currentStatusInstance,
        &LoadedModelStatus::modelPausedAtOperation,
        this,
        &BuildExecuteStateMachine::modelPausedAtOperation
    );
    connect(
        currentStatusInstance,
        &LoadedModelStatus::modelPausedOnVariableUpdate,
        this,
        &BuildExecuteStateMachine::modelPausedOnVariableUpdate
    );
    connect(
        currentStatusInstance,
        &LoadedModelStatus::modelResumed,
        this,
        &BuildExecuteStateMachine::modelResumed
    );

    connect(
        currentStatusInstance,
        &LoadedModelStatus::sendOutputToDevice,
        this,
        &BuildExecuteStateMachine::sendOutputToDevice,
        Qt::BlockingQueuedConnection
    );

    loader = new Cbe::DynamicLibraryLoader;
}


BuildExecuteStateMachine::~BuildExecuteStateMachine() {
    unloadIdentifierValues();
    delete loader;
}


BuildExecuteStateMachine::State BuildExecuteStateMachine::state() const {
    return currentState;
}


bool BuildExecuteStateMachine::isReady() const {
    return (currentState == State::IDLE || currentState == State::BUILD_READY);
}


bool BuildExecuteStateMachine::isActive() const {
    return !isReady();
}


bool BuildExecuteStateMachine::isRunning() const {
    return currentState == State::RUNNING;
}


bool BuildExecuteStateMachine::isRunning(QSharedPointer<Ld::RootElement> rootElement) const {
    bool result = false;

    if (currentState == State::RUNNING) {
        QSharedPointer<Ld::RootElement>
            currentRootElement = BuildExecuteStateMachine::currentRootElement.toStrongRef();

        if (!currentRootElement.isNull()) {
            if (currentRootElement == rootElement) {
                result = true;
            } else if (currentRootElement->allDependencies().contains(rootElement)) {
                result = true;
            }
        }
    }

    return result;
}


Model::Variant BuildExecuteStateMachine::valueForVariable(QSharedPointer<Ld::VariableElement> variableElement) const {
    Model::Variant result;

    Ld::IdentifierContainer identifier = variableElement->identifier();
    if (identifier.isValid()) {
        Ld::Identifier::Handle handle = identifier.handle();

        const Model::IdentifierData&
            identifierData = currentModelIdentifierDatabase->identifierDataByHandle(handle);

        if (identifierData.isValid()) {
            result = identifierData.value();
        }
    }

    return result;
}


QSharedPointer<Ld::RootElement> BuildExecuteStateMachine::rootElement() const {
    return currentRootElement.toStrongRef();
}


bool BuildExecuteStateMachine::updateVariable(const Ld::CalculatedValue& calculatedValue) {
    bool success;

    Model::IdentifierData identifierData = currentModelIdentifierDatabase->identifierDataByName(
        calculatedValue.name1().toLocal8Bit().data(),
        calculatedValue.name2().toLocal8Bit().data()
    );

    if (identifierData.isValid() && identifierData.isVariable()) {
        success = identifierData.setValue(calculatedValue.variant());

        if (success) {
            Ld::Identifier::Handle   handle    = identifierData.identifierHandle();
            Ld::IdentifierContainer identifier = currentIdentifierDatabase.entryByHandle(handle);

            if (identifier.isValid()) {
                reportSingleIdentifierChanged(identifier, identifierData.value());
            }
        }
    } else {
        success = false;
    }

    return success;
}


void BuildExecuteStateMachine::rootElementChanged(QSharedPointer<Ld::RootElement> rootElement) {
    if (currentState != State::IDLE) {
        Ld::ElementPointer currentRootElement = BuildExecuteStateMachine::rootElement();

        if (currentRootElement == rootElement) {
            targetState = State::IDLE;
            transitionToNewState();
        }
    }
}


void BuildExecuteStateMachine::run(
        QSharedPointer<Ld::RootElement> rootElement,
        bool                            debugMode,
        Ld::ElementPointer              runToElement
    ) {
    newRootElement      = rootElement;
    newDebugMode        = debugMode;
    currentRunToElement = runToElement.toWeakRef();

    targetState = State::RUNNING;
    transitionToNewState();
}


void BuildExecuteStateMachine::step(QSharedPointer<Ld::RootElement> rootElement) {
    if (targetState == State::PAUSED && rootElement == currentRootElement && currentDebugMode) {
        currentSingleStep = true;
        targetState       = State::RUNNING;
    } else {
        newRootElement = rootElement;
        newDebugMode   = true;
        targetState    = State::PAUSED;
    }

    currentRunToElement.clear();
    transitionToNewState();
}


void BuildExecuteStateMachine::pause() {
    if (targetState == State::RUNNING) {
        targetState = State::PAUSED;
        transitionToNewState();
    }
}


void BuildExecuteStateMachine::resume(Ld::ElementPointer runToElement) {
    if (targetState == State::PAUSED) {
        currentRunToElement = runToElement.toWeakRef();
        currentSingleStep   = false;
        targetState         = State::RUNNING;
        transitionToNewState();
    }
}


void BuildExecuteStateMachine::pauseOrResume() {
    if (targetState == State::PAUSED) {
        resume();
    } else {
        pause();
    }
}


void BuildExecuteStateMachine::abort() {
    if (targetState == State::RUNNING || targetState == State::PAUSED) {
        targetState = State::BUILD_READY;
        transitionToNewState();
    }
}


void BuildExecuteStateMachine::shutdownNow() {
    if (targetState == State::IDLE || targetState == State::BUILD_READY) {
        emit nowShutdown();
    } else {
        currentShutdownForced = true;

        if (targetState == State::WAITING_TO_BUILD || targetState == State::BUILDING) {
            targetState = State::IDLE;
        } else {
            targetState = State::BUILD_READY;
        }

        transitionToNewState();
    }
}


bool BuildExecuteStateMachine::setInstructionBreakpoint(Ld::ElementPointer element, bool nowSet) {
    bool success = false;

    if (currentModelApi != Q_NULLPTR) {
        Ld::ElementPointer elementRoot = element->root();
        if (elementRoot == currentRootElement) {
            Ld::Operation operation = currentOperationDatabase.fromElement(element);
            if (operation.isValid()) {
                Ld::Operation::Handle operationHandle = operation.handle();
                if (operationHandle < currentModelApi->numberOperationHandles()) {
                    currentModelApi->setBreakAtOperation(operationHandle, nowSet);
                    success = true;
                }
            }
        }
    }

    return success;
}


bool BuildExecuteStateMachine::clearInstructionBreakpoint(Ld::ElementPointer element, bool nowClear) {
    return setInstructionBreakpoint(element, !nowClear);
}


void BuildExecuteStateMachine::clearAllInstructionBreakpoints() {
    if (currentModelApi != nullptr) {
        Ld::Operation::Handle numberOperationHandles = currentModelApi->numberOperationHandles();
        for (Ld::Operation::Handle operationHandle=0 ; operationHandle<numberOperationHandles ; ++operationHandle) {
            currentModelApi->setBreakAtOperation(operationHandle, false);
        }
    }
}


void BuildExecuteStateMachine::generatorIdle() {
    if (currentState == State::WAITING_TO_BUILD) {
        bool nowOwnGenerator = tryToOwn();
        if (nowOwnGenerator) {
            performBuild();
        }
    }
}


void BuildExecuteStateMachine::translationCompleted(
        QSharedPointer<Ld::RootElement> /* rootElement */,
        bool                            buildSuccessful
    ) {
    // State Transitions:
    // EXPORTING -> IDLE
    // BUILDING  -> BUILD_READY
    // BUILDING  -> RUNNING
    // BUILDING  -> RUNNING -> PAUSED
    // BUILDING  -> IDLE
    // BUILDING  -> BUILDING

    releaseOwnership();

    bool loadSuccessful;
    if (buildSuccessful) {
        loadSuccessful = loadModel();
        if (!loadSuccessful) {
            // TODO: Report load failed
        }
    } else {
        loadSuccessful = false;
    }

    if (!buildSuccessful || !loadSuccessful) {
        targetState = State::IDLE;
    }

    switch (targetState) {
        case State::IDLE: {
            if (QFile(currentFilename).exists()) {
                deleteModelFile();
            }

            transitionToIdle();
            break;
        }

        case State::WAITING_TO_BUILD: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILDING: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILD_READY: {
            updateCurrentState(State::BUILD_READY);
            break;
        }

        case State::RUNNING:
        case State::PAUSED: {
            startExecution();
            break;
        }

        case State::PAUSING: {
            Q_ASSERT(false);
            break;
        }

        case State::ABORTING: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void BuildExecuteStateMachine::translationErrorDetected(
        QSharedPointer<Ld::RootElement> /* rootElement */,
        Ld::DiagnosticPointer           diagnostic
    ) {
    if (Application::debugModeEnabled()) {
        if (Application::debugModeEnabled()) {
            M::Console::registerCallback(Application::consoleDevice());
        }

        M::Console::MessageType messageType = M::Console::MessageType::INVALID;
        switch (diagnostic->diagnosticType()) {
            case Ld::Diagnostic::Type::NONE:           { messageType = M::Console::MessageType::INFORMATION;   break; }
            case Ld::Diagnostic::Type::INFORMATION:    { messageType = M::Console::MessageType::INFORMATION;   break; }
            case Ld::Diagnostic::Type::PAUSED:         { messageType = M::Console::MessageType::INFORMATION;   break; }
            case Ld::Diagnostic::Type::WARNING:        { messageType = M::Console::MessageType::BUILD_WARNING; break; }
            case Ld::Diagnostic::Type::FATAL_ERROR:    { messageType = M::Console::MessageType::BUILD_ERROR;   break; }
            case Ld::Diagnostic::Type::INTERNAL_ERROR: { messageType = M::Console::MessageType::BUILD_ERROR;   break; }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        ConsoleDevice* console = Application::consoleDevice();
        console->reportImmediate(messageType, diagnostic->diagnosticMessage());

        if (diagnostic->codeGeneratorName() == Ld::CppCodeGenerator::codeGeneratorName) {
            QSharedPointer<Ld::CppCodeGeneratorDiagnostic>
                cppDiagnostic = diagnostic.dynamicCast<Ld::CppCodeGeneratorDiagnostic>();

            if (!cppDiagnostic.isNull()) {
                QString backendMessage = cppDiagnostic->backendMessage();
                QStringList backendMessages = backendMessage.split("\n");
                backendMessages.prepend(
                    tr("Line %1 Column %2 Offset %3 Backend Code %4 Phase %5:")
                        .arg(cppDiagnostic->lineNumber())
                        .arg(cppDiagnostic->columnNumber())
                        .arg(cppDiagnostic->byteOffset())
                        .arg(static_cast<unsigned long>(cppDiagnostic->compilerCode()))
                        .arg(cppDiagnostic->translationPhase().currentPhaseName())
                );

                for (  QStringList::const_iterator messageIterator    = backendMessages.constBegin(),
                                                   messageEndIterator = backendMessages.constEnd()
                     ; messageIterator != messageEndIterator
                     ; ++messageIterator
                    ) {
                    console->reportImmediate(messageType, tr("    %1").arg(*messageIterator));
                }
            }
        }
    }
}


void BuildExecuteStateMachine::modelStarted(Model::Api* modelApi) {
    Q_ASSERT(modelApi == currentModelApi);
    clearIdentifierChanges();
    emit running(currentRootElement, currentDebugMode);
}


void BuildExecuteStateMachine::modelFinished(Model::Api* modelApi) {
    Q_ASSERT(modelApi == currentModelApi);
    currentModelApi->setRunToLocation(Model::invalidOperationHandle);

    Ld::ElementPointer pausedElement = currentPausedElement.toStrongRef();
    if (!pausedElement.isNull()) {
        pausedElement->clearDiagnostic();
    }

    reportIdentifierChanges();
    emit stopped(currentRootElement);

    if (Application::debugModeEnabled()) {
        M::Console::report(M::Console::MessageType::INFORMATION, "Finished.");
    }

    switch (targetState) {
        case State::IDLE: {
            expungeLoadedModel();
            deleteModelFile();
            transitionToIdle();

            break;
        }

        case State::WAITING_TO_BUILD: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILDING: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILD_READY: {
            updateCurrentState(State::BUILD_READY);
            break;
        }

        case State::RUNNING: {
            if (newBuildNeeded()) {
                expungeLoadedModel();
                deleteModelFile();
                startBuild();
            } else {
                updateCurrentState(State::BUILD_READY);
            }

            break;
        }

        case State::PAUSED: {
            if (newBuildNeeded()) {
                expungeLoadedModel();
                deleteModelFile();
                startBuild();
            } else {
                startExecution();
            }

            break;
        }

        case State::PAUSING: {
            Q_ASSERT(false);
            break;
        }

        case State::ABORTING: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void BuildExecuteStateMachine::modelAborted(
        Model::Api*        modelApi,
        Model::AbortReason abortReason,
        unsigned long long operationHandle
    ) {
    Q_ASSERT(modelApi == currentModelApi);
    currentModelApi->setRunToLocation(Model::invalidOperationHandle);

    Ld::ElementPointer pausedElement = currentPausedElement.toStrongRef();
    if (!pausedElement.isNull()) {
        pausedElement->clearDiagnostic();
    }

    QSharedPointer<Ld::RootElement> rootElement = currentRootElement.toStrongRef();
    Q_ASSERT(!rootElement.isNull());

    Ld::Operation      operation = currentOperationDatabase.fromHandle(operationHandle);
    Ld::ElementPointer element   = operation.element();

    Ld::DiagnosticPointer runtimeDiagnostic(
        new RuntimeDiagnostic(
            element,
            RuntimeDiagnostic::Type::RUNTIME_ERROR,
            static_cast<RuntimeDiagnostic::Code>(abortReason)
        )
    );

    currentPausedElement = element.toWeakRef();
    if (!currentPausedElement.isNull()) {
        element->flagDiagnostic(runtimeDiagnostic);
    }

    reportIdentifierChanges();
    emit aborted(currentRootElement);

    if (Application::debugModeEnabled()) {
        M::Console::report(
            M::Console::MessageType::INFORMATION,
            "Aborted: ",
            runtimeDiagnostic->diagnosticMessage().toLocal8Bit().constData()
        );
    }

    switch (targetState) {
        case State::IDLE: {
            expungeLoadedModel();
            deleteModelFile();
            transitionToIdle();

            break;
        }

        case State::WAITING_TO_BUILD: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILDING: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILD_READY: {
            if (newBuildNeeded()) {
                expungeLoadedModel();
                deleteModelFile();
                startBuild();
            } else {
                updateCurrentState(State::BUILD_READY);
            }

            break;
        }

        case State::RUNNING:
        case State::PAUSED: {
            if (newBuildNeeded()) {
                expungeLoadedModel();
                deleteModelFile();
                startBuild();
            } else {
                updateCurrentState(State::BUILD_READY);
            }

            break;
        }

        case State::PAUSING: {
            Q_ASSERT(false);
            break;
        }

        case State::ABORTING: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void BuildExecuteStateMachine::modelPausedOnUserRequest(Model::Api* modelApi, unsigned long long operationHandle) {
    Q_ASSERT(modelApi == currentModelApi);
    currentModelApi->setRunToLocation(Model::invalidOperationHandle);

    modelPaused();

    QSharedPointer<Ld::RootElement> rootElement = currentRootElement.toStrongRef();
    Q_ASSERT(!rootElement.isNull());

    Ld::Operation      operation = currentOperationDatabase.fromHandle(operationHandle);
    Ld::ElementPointer element   = operation.element();

    Ld::DiagnosticPointer runtimeDiagnostic(
        new RuntimeDiagnostic(
            element,
            RuntimeDiagnostic::Type::PAUSED,
            RuntimeDiagnostic::Code::PAUSED_ON_USER_REQUEST
        )
    );

    currentPausedElement = element.toWeakRef();
    element->flagDiagnostic(runtimeDiagnostic);

    reportIdentifierChanges();
    emit pausedOnUserRequest(currentRootElement, element);
}


void BuildExecuteStateMachine::modelPausedAtOperation(Model::Api* modelApi, unsigned long long operationHandle) {
    Q_ASSERT(modelApi == currentModelApi);
    currentModelApi->setRunToLocation(Model::invalidOperationHandle);

    modelPaused();

    QSharedPointer<Ld::RootElement> rootElement = currentRootElement.toStrongRef();
    Q_ASSERT(!rootElement.isNull());

    Ld::Operation      operation = currentOperationDatabase.fromHandle(operationHandle);
    Ld::ElementPointer element   = operation.element();

    Ld::DiagnosticPointer runtimeDiagnostic(
        new RuntimeDiagnostic(
            element,
            RuntimeDiagnostic::Type::PAUSED,
            RuntimeDiagnostic::Code::PAUSED_AT_INSTRUCTION_BREAKPOINT
        )
    );

    currentPausedElement = element.toWeakRef();
    element->flagDiagnostic(runtimeDiagnostic);

    reportIdentifierChanges();
    emit pausedAtInstructionBreakpoint(currentRootElement, element);
}


void BuildExecuteStateMachine::modelPausedOnVariableUpdate(
        Model::Api*        modelApi,
        unsigned long long operationHandle,
        unsigned long long identifierHandle
    ) {
    Q_ASSERT(modelApi == currentModelApi);
    currentModelApi->setRunToLocation(Model::invalidOperationHandle);

    modelPaused();

    QSharedPointer<Ld::RootElement> rootElement = currentRootElement.toStrongRef();
    Q_ASSERT(!rootElement.isNull());

    Ld::Operation      operation = currentOperationDatabase.fromHandle(operationHandle);
    Ld::ElementPointer element   = operation.element();

    Ld::IdentifierContainer identifierData = currentIdentifierDatabase.entryByHandle(identifierHandle);

    Ld::DiagnosticPointer runtimeDiagnostic(
        new RuntimeDiagnostic(
            element,
            RuntimeDiagnostic::Type::PAUSED,
            RuntimeDiagnostic::Code::PAUSED_ON_VARIABLE_UPDATE
        )
    );

    currentPausedElement = element.toWeakRef();
    element->flagDiagnostic(runtimeDiagnostic);

    reportIdentifierChanges();
    emit pausedOnVariableUpdate(currentRootElement, element, identifierData);
}


void BuildExecuteStateMachine::modelResumed(Model::Api* modelApi) {
    Q_ASSERT(modelApi == currentModelApi);

    Ld::ElementPointer pausedElement = currentPausedElement.toStrongRef();
    if (!pausedElement.isNull()) {
        pausedElement->clearDiagnostic();
    }

    clearIdentifierChanges();
    emit resumed(currentRootElement);

    switch (targetState) {
        case State::IDLE:
        case State::BUILD_READY: {
            abortExecution();
            break;
        }

        case State::WAITING_TO_BUILD: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILDING: {
            Q_ASSERT(false);
            break;
        }

        case State::RUNNING: {
            if (newBuildNeeded()) {
                abortExecution();
            } else {
                updateCurrentState(State::RUNNING);
            }

            break;
        }

        case State::PAUSED: {
            if (newBuildNeeded()) {
                abortExecution();
            } else {
                pauseExecution();
            }

            break;
        }

        case State::PAUSING: {
            Q_ASSERT(false);
            break;
        }

        case State::ABORTING: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


bool BuildExecuteStateMachine::newBuildNeeded() const {
    return (currentRootElement != newRootElement || currentDebugMode != newDebugMode);
}


void BuildExecuteStateMachine::transitionToNewState() {
    switch (currentState) {
        case State::IDLE: {
            // IDLE:        *** No Action Needed ***
            // BUILD_READY: IDLE -> BUILDING -> BUILD_READY
            // RUNNING:     IDLE -> BUILDING -> RUNNING
            // PAUSED:      IDLE -> BUILDING -> RUNNING -> PAUSED

            switch (targetState) {
                case State::IDLE: {
                    break;
                }

                case State::WAITING_TO_BUILD: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILDING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILD_READY:
                case State::RUNNING:
                case State::PAUSED: {
                    startBuild();
                    break;
                }

                case State::PAUSING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::ABORTING: {
                    Q_ASSERT(false);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case State::WAITING_TO_BUILD: {
            // IDLE:           WAITING_TO_BUILDING -> IDLE
            // BUILDING:       *** No Action Needed ***
            // BUILD_READY:    *** No Action Needed ***
            // RUNNING:        *** No Action Needed ***
            // PAUSED:         *** No Action Needed ***

            switch (targetState) {
                case State::IDLE: {
                    transitionToIdle();
                    break;
                }

                case State::WAITING_TO_BUILD: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILDING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILD_READY:
                case State::RUNNING:
                case State::PAUSED: {
                    // We never need to interupt this action if we're moving to a later state.
                    break;
                }

                case State::PAUSING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::ABORTING: {
                    Q_ASSERT(false);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case State::BUILDING: {
            // IDLE:              BUILDING -> IDLE
            // BUILDING:          *** No Action Needed ***
            //                 or BUILDING -> BUILDING
            // BUILD_READY:       *** No Action Needed ***
            //                 or BUILDING -> BUILDING -> BUILD_READY
            // RUNNING:           *** No Action Needed ***
            //                 or BUILDING -> BUILDING -> RUNNING
            // PAUSED:         BUILDING -> RUNNING -> PAUSED

            switch (targetState) {
                case State::IDLE: {
                    stopBuild();
                    break;
                }

                case State::WAITING_TO_BUILD: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILDING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILD_READY:
                case State::RUNNING:
                case State::PAUSED: {
                    if (newBuildNeeded()) {
                        stopBuild();
                    }

                    break;
                }

                case State::PAUSING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::ABORTING: {
                    Q_ASSERT(false);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case State::BUILD_READY: {
            // IDLE:        BUILD_READY -> IDLE
            // BUIDING:     BUILD_READY -> BUILDING
            // BUILD_READY:    *** No Action Needed **
            //              or BUILD_READY -> BUILDING -> BUILD_READY
            // RUNNING:        BUILD_READY -> RUNNING
            //              or BUILD_READY -> BUILDING -> RUNNING
            // PAUSED:         BUILD_READY -> RUNNING -> PAUSED
            //              or BUILD_READY -> BUILDING -> RUNNING -> PAUSED

            switch (targetState) {
                case State::IDLE: {
                    if (currentModelApi != Q_NULLPTR) {
                        expungeLoadedModel();
                    }

                    deleteModelFile();
                    transitionToIdle();

                    break;
                }

                case State::WAITING_TO_BUILD: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILDING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILD_READY: {
                    if (newBuildNeeded()) {
                        if (currentModelApi != Q_NULLPTR) {
                            expungeLoadedModel();
                        }

                        deleteModelFile();
                        startBuild();
                    }

                    break;
                }

                case State::RUNNING:
                case State::PAUSED: {
                    if (newBuildNeeded()) {
                        if (currentModelApi != Q_NULLPTR) {
                            expungeLoadedModel();
                        }

                        deleteModelFile();
                        startBuild();
                    } else {
                        startExecution();
                    }

                    break;
                }

                case State::PAUSING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::ABORTING: {
                    Q_ASSERT(false);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case State::RUNNING: {
            // IDLE:        RUNNING -> ABORTING -> IDLE
            // BUILDING:    RUNNING -> ABORTING -> BUILDING
            // BUILD_READY:    RUNNING -> ABORTING -> BUILD_READY
            //              or RUNNING -> ABORTING -> BUILDING -> BUILD_READY
            // RUNNING:        *** No Action Needed ***
            //              or RUNNING -> ABORTING -> BUILDING -> RUNNING
            // ABORTING:       RUNNING -> ABORTING -> BUILD_READY
            //              or RUNNING -> ABORTING -> BUILDING -> BUILD_READY
            // PAUSED:         RUNNING -> PAUSED
            //              or RUNNING -> ABORTING -> BUILDING -> RUNNING -> PAUSED

            switch (targetState) {
                case State::IDLE:
                case State::BUILD_READY:
                case State::ABORTING: {
                    abortExecution();
                    break;
                }

                case State::WAITING_TO_BUILD: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILDING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::RUNNING: {
                    if (newBuildNeeded()) {
                        abortExecution();
                    }

                    break;
                }

                case State::PAUSED: {
                    if (newBuildNeeded()) {
                        abortExecution();
                    } else {
                        pauseExecution();
                    }

                    break;
                }

                case State::PAUSING: {
                    Q_ASSERT(false);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case State::ABORTING: {
            // We always let an abort complete
            break;
        }

        case State::PAUSING: {
            // For simplicity, we always wait for pausing to complete before any other action.
            break;
        }

        case State::PAUSED: {
            // IDLE:        PAUSED -> ABORTING -> IDLE
            // EXPORTING:   PAUSED -> ABORTING -> EXPORTING
            // BUILDING:    PAUSED -> ABORTING -> BUILDING
            // BUILD_READY:    PAUSED -> ABORTING -> BUILD_READY
            //              or PAUSED -> ABORTING -> BUILDING -> BUILD_READY
            // RUNNING:        PAUSED -> RUNNING
            //              or PAUSED -> ABORTING -> BUILDING -> RUNNING
            // ABORTING:    PAUSED -> ABORTING
            // PAUSED:         *** No Action Needed ***
            //              or PAUSED -> ABORTING -> BUILDING -> RUNNING -> PAUSED

            switch (targetState) {
                case State::IDLE: {
                    abortExecution();
                    break;
                }

                case State::BUILD_READY:
                case State::ABORTING: {
                    abortExecution();
                    break;
                }

                case State::WAITING_TO_BUILD: {
                    Q_ASSERT(false);
                    break;
                }

                case State::BUILDING: {
                    Q_ASSERT(false);
                    break;
                }

                case State::RUNNING: {
                    if (newBuildNeeded()) {
                        abortExecution();
                    } else {
                        resumeExecution();
                    }

                    break;
                }

                case State::PAUSED: {
                    if (newBuildNeeded()) {
                        abortExecution();
                    }

                    break;
                }

                case State::PAUSING: {
                    Q_ASSERT(false);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void BuildExecuteStateMachine::modelPaused() {
    switch (targetState) {
        case State::IDLE:
        case State::BUILD_READY: {
            abortExecution();
            break;
        }

        case State::WAITING_TO_BUILD: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILDING: {
            Q_ASSERT(false);
            break;
        }

        case State::RUNNING: {
            if (newBuildNeeded()) {
                abortExecution();
            } else {
                targetState = State::PAUSED;
                updateCurrentState(State::PAUSED);
            }

            break;
        }

        case State::PAUSED: {
            if (newBuildNeeded()) {
                abortExecution();
            } else {
                updateCurrentState(State::PAUSED);
            }

            break;
        }

        case State::PAUSING: {
            Q_ASSERT(false);
            break;
        }

        case State::ABORTING: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void BuildExecuteStateMachine::startBuild() {
    bool nowOwnGenerator = tryToOwn();
    if (nowOwnGenerator) {
        performBuild();
    } else {
        updateCurrentState(State::WAITING_TO_BUILD);
    }
}


void BuildExecuteStateMachine::performBuild() {
    QSharedPointer<Ld::RootElement> rootElement = newRootElement.toStrongRef();
    if (!rootElement.isNull()) {
        currentFilename = filenameForRootElement(rootElement);
        QFileInfo directoryInfo(QFileInfo(currentFilename).absolutePath());
        bool      isWritable = directoryInfo.isWritable();
        if (isWritable) {
            currentRootElement = rootElement.toWeakRef();
            currentDebugMode   = newDebugMode;

            updateCurrentState(State::BUILDING);
            translate(rootElement, currentFilename);
        } else {
            releaseOwnership();

            emit directoryIsReadOnly(directoryInfo.absoluteFilePath());

            targetState = State::IDLE;
            transitionToIdle();
        }
    } else {
        releaseOwnership();

        targetState  = State::IDLE;
        transitionToIdle();
    }
}


void BuildExecuteStateMachine::stopBuild() {
    abort();
    releaseOwnership();

    switch (targetState) {
        case State::IDLE: {
            transitionToIdle();
            break;
        }

        case State::WAITING_TO_BUILD: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILDING: {
            Q_ASSERT(false);
            break;
        }

        case State::BUILD_READY:
        case State::RUNNING:
        case State::PAUSED: {
            currentRootElement.clear();
            currentDebugMode = false;

            startBuild();

            break;
        }

        case State::ABORTING: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void BuildExecuteStateMachine::startExecution() {
    Q_ASSERT(currentModelApi != Q_NULLPTR);

    updateCurrentState(State::RUNNING);

    QSharedPointer<Ld::RootElement> rootElement = currentRootElement.toStrongRef();
    currentIdentifierDatabase = rootElement->identifierDatabase();
    currentOperationDatabase  = rootElement->operationDatabase();

    Model::Rng::RngType rngType         = rootElement->rngType();
    bool                useSeedAsPreset = rootElement->rngSeedIsPreset();

    Model::Rng::RngSeed rngSeed;
    if (useSeedAsPreset) {
        std::memcpy(rngSeed, rootElement->rngSeed(), sizeof(Ld::RootElement::RngSeed));
    } else {
        M::PerThread perThread(0, rngType, rngSeed, Q_NULLPTR);

        rngSeed[0] = static_cast<std::uint64_t>(perThread.trng()) << 32 | perThread.trng();
        rngSeed[1] = static_cast<std::uint64_t>(perThread.trng()) << 32 | perThread.trng();
        rngSeed[2] = static_cast<std::uint64_t>(perThread.trng()) << 32 | perThread.trng();
        rngSeed[3] = static_cast<std::uint64_t>(perThread.trng()) << 32 | perThread.trng();

        rootElement->setRngSeed(rngSeed);
    }

    configureInstructionBreakpoints();

    if (targetState == State::PAUSED) {
        currentModelApi->pause();
    } else {
        setRunToOperation(currentRunToElement.toStrongRef());
    }

    if (Application::debugModeEnabled()) {
        unsigned numberParallelThreads = currentModelApi->numberThreads();
        M::Console::initialize(numberParallelThreads);
        M::Console::report(M::Console::MessageType::INFORMATION, "Starting.");
    }

    QFileInfo runDirectory;
    bool      useStandardDirectory = true;
    if (rootElement->hasFilename()) {
        QFileInfo programFileInformation(rootElement->filename());
        runDirectory = QFileInfo(programFileInformation.canonicalPath());

        useStandardDirectory = (!runDirectory.exists() || !runDirectory.isDir());
    }

    if (useStandardDirectory) {
        QStringList documentsPath = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        runDirectory = QFileInfo(documentsPath.first());
        if (!runDirectory.exists() || !runDirectory.isDir()) {
            QStringList desktopPath = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
            runDirectory = QFileInfo(desktopPath.first());
            if (!runDirectory.exists() || !runDirectory.isDir()) {
                QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
                runDirectory = QFileInfo(homePath.first());
            }
        }
    }

    if (runDirectory.exists() && runDirectory.isDir()) {
        QDir::setCurrent(runDirectory.canonicalFilePath());
    }

    currentModelApi->start(rngType, rngSeed, currentStatusInstance);
}


void BuildExecuteStateMachine::abortExecution() {
    Q_ASSERT(currentModelApi != Q_NULLPTR);

    updateCurrentState(State::ABORTING);
    currentModelApi->abort();
}


void BuildExecuteStateMachine::pauseExecution() {
    Q_ASSERT(currentModelApi != Q_NULLPTR);

    updateCurrentState(State::PAUSING);
    currentModelApi->pause();
}


void BuildExecuteStateMachine::resumeExecution() {
    Q_ASSERT(currentModelApi != Q_NULLPTR);
    Q_ASSERT(currentState == State::PAUSED);

    targetState = State::RUNNING;
    updateCurrentState(State::RUNNING);

    bool success;
    if (currentSingleStep) {
        currentSingleStep = false;
        success = currentModelApi->singleStep();
    } else {
        setRunToOperation(currentRunToElement.toStrongRef());
        success = currentModelApi->resume();
    }

    Q_ASSERT(success);
}


void BuildExecuteStateMachine::transitionToIdle() {
    currentRootElement.clear();
    newRootElement.clear();

    currentDebugMode = false;
    newDebugMode     = false;

    updateCurrentState(State::IDLE);

    if (currentShutdownForced) {
        currentShutdownForced = false;
        emit nowShutdown();
    }
}


bool BuildExecuteStateMachine::loadModel() {
    bool success;

    Q_ASSERT(loader->isNotLoaded());
    Q_ASSERT(currentDeallocatorFunction == Q_NULLPTR);
    Q_ASSERT(currentModelApi == Q_NULLPTR);

    success = loader->load(currentFilename);
    if (success) {
        Model::AllocatorFunction allocatorFunction = reinterpret_cast<Model::AllocatorFunction>(
            loader->resolve(Model::allocatorFunctionName)
        );

        if (allocatorFunction != Q_NULLPTR) {
            Model::DeallocatorFunction deallocatorFunction = reinterpret_cast<Model::DeallocatorFunction>(
                loader->resolve(Model::deallocatorFunctionName)
            );

            if (deallocatorFunction != Q_NULLPTR) {
                Model::Api* newModelApi = (*allocatorFunction)(&matApi);
                if (newModelApi != Q_NULLPTR) {
                    // TODO: Validate proper debug mode settings.

                    currentModelApi = static_cast<M::Api*>(newModelApi);
                    Q_ASSERT(currentModelApi != Q_NULLPTR);

                    currentDeallocatorFunction     = deallocatorFunction;
                    currentModelIdentifierDatabase = new Model::IdentifierDatabase(newModelApi->identifierDatabase());

                    if (Application::debugModeEnabled()) {
                        currentModelApi->registerConsoleCallback(Application::consoleDevice());
                    }
                } else {
                    success = false;
                }
            } else {
                success = false;
            }
        } else {
            success = false;
        }

        if (!success) {
            bool successfullyUnloaded = loader->unload();
            Q_ASSERT(successfullyUnloaded);
        }
    }

    return success;
}


void BuildExecuteStateMachine::expungeLoadedModel() {
    Q_ASSERT(loader->isLoaded());
    Q_ASSERT(currentDeallocatorFunction != Q_NULLPTR);
    Q_ASSERT(currentModelApi != Q_NULLPTR);

    currentModelApi->releaseConsoleCallback();
    unloadIdentifierValues();

    delete currentModelIdentifierDatabase;
    (*currentDeallocatorFunction)(currentModelApi);

    bool successfullyUnloaded = loader->unload();
    Q_ASSERT(successfullyUnloaded);

    currentDeallocatorFunction     = Q_NULLPTR;
    currentModelApi                = Q_NULLPTR;
    currentModelIdentifierDatabase = Q_NULLPTR;
}


void BuildExecuteStateMachine::deleteModelFile() {
    Q_ASSERT(!currentFilename.isEmpty());

    QFile currentLibraryFile(currentFilename);
    if (currentLibraryFile.exists()) {
        bool successfullyRemovedFile = currentLibraryFile.remove();
        Q_ASSERT(successfullyRemovedFile);
    }

    currentFilename.clear();
}


void BuildExecuteStateMachine::updateCurrentState(State newState) {
    bool  wasReady = isReady();

    currentState = newState;
    bool nowReady = isReady();

    if (wasReady != nowReady) {
        emit ready(nowReady);
        emit active(!nowReady);
    }

    if (newState == State::BUILD_READY && currentShutdownForced) {
        currentShutdownForced = false;
        emit nowShutdown();
    }
}


void BuildExecuteStateMachine::configureInstructionBreakpoints() {
    Q_ASSERT(currentModelApi != Q_NULLPTR);

    Ld::Operation::Handle numberOperationHandles = currentModelApi->numberOperationHandles();
    for (Ld::Operation::Handle operationHandle=0 ; operationHandle<numberOperationHandles ; ++operationHandle) {
        Ld::Operation      operation = currentOperationDatabase.fromHandle(operationHandle);
        Ld::ElementPointer element   = operation.element();

        if (!element.isNull()) {
            currentModelApi->setBreakAtOperation(operationHandle, element->instructionBreakpointSet());
        }
    }
}


QString BuildExecuteStateMachine::filenameForRootElement(QSharedPointer<Ld::RootElement> rootElement) {
    QString result;

    #if (defined(Q_OS_DARWIN))

        QString extension = tr("dylib");

    #elif (defined(Q_OS_LINUX))

        QString extension = tr("so");

    #elif (defined(Q_OS_WIN))

        QString extension = tr("dll");

    #else

        #error Unknown platform

    #endif

    if (rootElement->hasFilename()) {
        QFileInfo fileInformation(rootElement->filename());
        QString   path     = fileInformation.absolutePath();
        QString   baseName = fileInformation.fileName();

        result = tr("%1/.~binary-%2.%3").arg(path).arg(baseName).arg(extension);
    } else {
        QString userName = Util::username();
        result = QString("%1/%2-%3-%4.tmp.%5").arg(QDir::tempPath())
                 .arg(QCoreApplication::applicationName())
                 .arg(userName)
                 .arg(QDateTime::currentMSecsSinceEpoch())
                 .arg(extension);
    }

    return result;
}


void BuildExecuteStateMachine::setRunToOperation(Ld::ElementPointer runToElement) {
    while (!runToElement.isNull() && !currentOperationDatabase.contains(runToElement)) {
        runToElement = runToElement->parent();
    }

    if (!runToElement.isNull()) {
        Ld::Operation operation = currentOperationDatabase.fromElement(runToElement);
        if (operation.isValid()) {
            Ld::Operation::Handle operationHandle = operation.handle();
            currentModelApi->setRunToLocation(operationHandle);
        }
    }
}


void BuildExecuteStateMachine::reportIdentifierChanges() {
    Q_ASSERT(currentModelApi != Q_NULLPTR);

    for (  Model::IdentifierDatabase::ConstIterator
               identifierIterator    = currentModelIdentifierDatabase->constBegin(),
               identifierEndIterator = currentModelIdentifierDatabase->constEnd()
         ; identifierIterator != identifierEndIterator
         ; ++identifierIterator
        ) {
        const Model::IdentifierData& identifierData = *identifierIterator;
        if (identifierData.isVariable()) {
            Ld::Identifier::Handle  handle     = identifierData.identifierHandle();
            Ld::IdentifierContainer identifier = currentIdentifierDatabase.entryByHandle(handle);
            if (identifier.isValid()) {
                reportSingleIdentifierChanged(identifier, identifierData.value());
            }
        }
    }
}


void BuildExecuteStateMachine::clearIdentifierChanges() {
    Ld::ElementPointerSet processedElements;

    for (  Model::IdentifierDatabase::ConstIterator
               identifierIterator    = currentModelIdentifierDatabase->constBegin(),
               identifierEndIterator = currentModelIdentifierDatabase->constEnd()
         ; identifierIterator != identifierEndIterator
         ; ++identifierIterator
        ) {
        const Model::IdentifierData identifier = *identifierIterator;
        Ld::IdentifierContainer     container  = currentIdentifierDatabase.entryByHandle(identifier.identifierHandle());
        if (container.isValid()) {
            for (  Ld::Identifier::ElementIterator elementIterator    = container.begin(),
                                                   elementEndIterator = container.end()
                 ; elementIterator != elementEndIterator
                 ; ++elementIterator
                ) {
                Ld::ElementPointer element = elementIterator->element();
                if (!processedElements.contains(element)) {
                    element->clearCalculatedValue();
                    processedElements.insert(element);
                }
            }
        }
    }
}


void BuildExecuteStateMachine::reportSingleIdentifierChanged(
        const Ld::IdentifierContainer& identifier,
        const Model::Variant&          value
    ) {
    Ld::CalculatedValue         calculatedValue(identifier.variableName(), value);
    QList<Ld::ElementValueData> elementDataList = identifier.elementValueList();
    for (  QList<Ld::ElementValueData>::const_iterator
               elementValueIterator    = elementDataList.constBegin(),
               elementValueEndIterator = elementDataList.constEnd()
         ; elementValueIterator != elementValueEndIterator
         ; ++elementValueIterator
        ) {
        const Ld::ElementValueData& elementData = *elementValueIterator;
        Ld::ElementPointer          element     = elementData.element();
        unsigned                    valueIndex  = elementData.valueIndex();
        if (!element.isNull()) {
            element->setCalculatedValue(valueIndex, calculatedValue);
        }
    }
}


void BuildExecuteStateMachine::unloadIdentifierValues() {
    if (!currentRootElement.toStrongRef().isNull()) {
        for (  Model::IdentifierDatabase::ConstIterator
                   identifierIterator    = currentModelIdentifierDatabase->constBegin(),
                   identifierEndIterator = currentModelIdentifierDatabase->constEnd()
             ; identifierIterator != identifierEndIterator
             ; ++identifierIterator
            ) {
            const Model::IdentifierData& identifierData = *identifierIterator;
            if (identifierData.isVariable()) {
                Ld::Identifier::Handle  handle     = identifierData.identifierHandle();
                Ld::IdentifierContainer identifier = currentIdentifierDatabase.entryByHandle(handle);
                if (identifier.isValid()) {
                    reportSingleIdentifierChanged(identifier, Model::Variant());
                }
            }
        }
    }
}
