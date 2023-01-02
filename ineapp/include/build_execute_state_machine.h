/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BuildExecuteStateMachine class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BUILD_EXECUTE_STATE_MACHINE_H
#define BUILD_EXECUTE_STATE_MACHINE_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QWeakPointer>

#include <model_api.h>
#include <model_status.h>

#include <model_variant.h>
#include <model_identifier_database.h>

#include <ld_element_structures.h>
#include <ld_variable_name.h>
#include <ld_data_type.h>
#include <ld_identifier_container.h>
#include <ld_identifier_database.h>
#include <ld_operation_database.h>

#include "app_common.h"
#include "data_type_presentation_generator.h"
#include "runtime_diagnostic.h"
#include "cpp_code_generator_visual.h"

namespace Model {
    class Api;
    class IdentifierDatabase;
    class IdentifierData;
    class Variant;
}

namespace M {
    class Api;
}

namespace Cbe {
    class DynamicLibraryLoader;
}

namespace Ld {
    class RootElement;
    class CppCodeGenerator;
    class VariableElement;
}

class LoadedModelStatus;

/**
 * The \ref BuildExecuteStateMachine class provides an engine you can use from multiple points of control to manage
 * the build, run, pause, abort, tasks.
 */
class APP_PUBLIC_API BuildExecuteStateMachine:public QObject, private CppCodeGeneratorVisual::Client {
    Q_OBJECT

    public:
        /**
         * Enumeration of the various operating states.
         */
       enum class State {
            /**
             * State indicating that the build/execute state machine is idle with no build image ready for use.
             */
            IDLE,

            /**
             * State indicating that the C++ translator is busy and we are waiting for it to become available.
             */
            WAITING_TO_BUILD,

            /**
             * State indicating that the build/execute state machine is actively building an image.
             */
            BUILDING,

            /**
             * State indicating that the build/execute state machine has a built image ready for use.
             */
            BUILD_READY,

            /**
             * State indicating that the build/execute state machine is actively running an image.
             */
            RUNNING,

            /**
             * State indicating that the build/execute state machine is aborting execution.
             */
            ABORTING,

            /**
             * State indicating that the build/execute state machine is waiting for the model to pause.
             */
            PAUSING,

            /**
             * State indicating that the build/execute state machine has paused execution.
             */
            PAUSED
        };

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        BuildExecuteStateMachine(QObject* parent = Q_NULLPTR);

        ~BuildExecuteStateMachine() final;

        /**
         * Method you can use to obtain the current operating state.
         *
         * \return Returns the current operating state.
         */
        State state() const;

        /**
         * Method you can use to determine if the state machine is ready for a build or for program execution.
         *
         * \return Returns true if the state machine is in the \ref BuildExecuteStateMachine::State::BUILD_READY state
         *         ir the \ref BuildExecuteStateMachine::State::IDLE state.  Returns false if the state machine is in
         *         any other state.
         */
        bool isReady() const;

        /**
         * Method you can use to determine if the state machine is transitioning between states or is actively running
         * a model.
         *
         * \return Returns true if the state machine is not in either the
         *         \ref BuildExecuteStateMachine::State::BUILD_READY state or the
         *         \ref BuildExecuteStateMachine::State::IDLE state.
         */
        bool isActive() const;

        /**
         * Method you can use to determine if the state machine is running.
         *
         * \return Returns true if the state machine is in the running state.  Returns false if the state machine is
         *         not in the running state.
         */
        bool isRunning() const;

        /**
         * Method you can use to determine if the state machine is running a specific user program.
         *
         * \param[in] rootElement The root element representing the program, or referenced section to check.
         *
         * \return Returns true if the state machine is in the running state and running the indicated program or a
         *         program that references it.  Returns false if the state machine is not in the running state.
         */
        bool isRunning(QSharedPointer<Ld::RootElement> rootElement) const;

        /**
         * Method you can use to obtain the root element currently tied to the state machine.
         *
         * \return Returns a pointer to the root element that is currently tied to the state machine.  A null pointer
         *         is returned if the state machine is idle.
         */
        QSharedPointer<Ld::RootElement> rootElement() const;

        /**
         * Method that obtains the variant associated with a given variable.
         *
         * \param[in] variableElement The variable element to obtain the variant for.
         *
         * \return Returns the variant for the variable.  An invalid variant is returned if the variable does not
         *         exist or a value has not been determined.
         */
        Model::Variant valueForVariable(QSharedPointer<Ld::VariableElement> variableElement) const;

    signals:
        /**
         * Signal that is emitted when the state machine is idle or has a build available.  This signal can be used to
         * enable run/pause controls.
         *
         * \param[out] nowReady Holds true if the state machine is ready for new user run/pause commands.  Holds false
         *                      if the state machine is in a transitional state or is actively running a model.
         */
        void ready(bool nowReady);

        /**
         * Signal that is emitted when the state machine is transitioning between states or is actively running a
         * user's program.
         *
         * \param[out] nowActive Holds true if the state machine is transitioning between states or is actively running
         *                       a program.
         */
        void active(bool nowActive);

        /**
         * Signal that is emitted when the state machine begins executing the current image.
         *
         * \param[out] rootElement The root element associated with the library being executed.
         *
         * \param[out] debugMode   If true, the state machine started executing in debug mode.  If false, the state
         *                         machine started executing in release mode.
         */
        void running(QSharedPointer<Ld::RootElement> rootElement, bool debugMode);

        /**
         * Signal that is emitted when the running executable terminates.
         *
         * \param[out] rootElement The root element associated with the library being executed.
         */
        void stopped(QSharedPointer<Ld::RootElement> rootElement);

        /**
         * Signal that is emitted when the running executable is aborted.
         *
         * \param[out] rootElement The root element associated with the library being executed.
         */
        void aborted(QSharedPointer<Ld::RootElement> rootElement);

        /**
         * Signal that is emitted when the running executable pauses due to a user request.
         *
         * \param[out] rootElement The root element associated with the library being executed.
         *
         * \param[out] pausedAt    The element that we are paused at.
         */
        void pausedOnUserRequest(QSharedPointer<Ld::RootElement> rootElement, Ld::ElementPointer pausedAt);

        /**
         * Signal that is emitted when the running executable pauses on an instruction breakpoint.
         *
         * \param[out] rootElement The root element associated with the library being executed.
         *
         * \param[out] pausedAt    The element that we are paused at.
         */
        void pausedAtInstructionBreakpoint(QSharedPointer<Ld::RootElement> rootElement, Ld::ElementPointer pausedAt);

        /**
         * Signal that is emitted when the running executable pauses on a variable update.
         *
         * \param[out] rootElement   The root element associated with the library being executed.
         *
         * \param[out] pausedAt      The element that we are paused at.
         *
         * \param[out] identifierData Data describing the identifier that triggered the pause condition.
         */
        void pausedOnVariableUpdate(
            QSharedPointer<Ld::RootElement> rootElement,
            Ld::ElementPointer              pausedAt,
            const Ld::IdentifierContainer&  identifierData
        );

        /**
         * Signal that is emitted when the running executable resumes execution.
         *
         * \param[out] rootElement The root element associated with the library being executed.
         */
        void resumed(QSharedPointer<Ld::RootElement> rootElement);

        /**
         * Signal that is emitted when the state machine has been forcibly shutdown.
         */
        void nowShutdown();

        /**
         * Signal that is emitted when the model reports immediate output.  You can use this to stream output from
         * the model to a given device and channel.  This method is called from the same thread(s) that the model
         * is running in and will block the thread.
         *
         * \param[in] device  The device to receive the data.
         *
         * \param[in] value   The reported value.
         */
        void sendOutputToDevice(Model::Device device, const Model::Variant& value);

        /**
         * Signal that is emitted when a model is attemted to be built in a read-only directory.
         *
         * \param[out] directoryPath The path to the directory in questin.
         */
        void directoryIsReadOnly(const QString& directoryPath);

    public slots:
        /**
         * Slot you can trigger to update a variable.
         *
         * \param[in] calculatedValue The updated calculated value.
         *
         * \return Returns true on success, returns false on error.
         */
        bool updateVariable(const Ld::CalculatedValue& calculatedValue);

        /**
         * Slot you can trigger to inform the state machine that a root element has changed.
         *
         * \param[in] rootElement The just changed root element.
         */
        void rootElementChanged(QSharedPointer<Ld::RootElement> rootElement);

        /**
         * Slot you can trigger to request a root element be run, building the root element, if needed.
         *
         * \param[in] rootElement  The root element to be executed.
         *
         * \param[in] debugMode    If true, the library will be built with debugging support.  If false, the library
         *                         will be built without debugging support.
         *
         * \param[in] runToElement The element we want the program to pause execution at.  A null pointer will cause
         *                         execution to run to completion.
         */
        void run(
            QSharedPointer<Ld::RootElement> rootElement,
            bool                            debugMode,
            Ld::ElementPointer              runToElement = Ld::ElementPointer()
        );

        /**
         * Slot you can trigger to request the root element step by a single operation.  If the root element is not
         * active, the root element will step into the first operation, building the root element, if needed.
         *
         * Note that this operation implies a debug build if a new build is needed.
         *
         * \param[in] rootElement The root element to be executed.
         */
        void step(QSharedPointer<Ld::RootElement> rootElement);

        /**
         * Slot you can trigger to request an executing model to pause immediately.
         */
        void pause();

        /**
         * Slot you can trigger to request an executing model resume.
         *
         * \param[in] runToElement The element we want the program to pause execution at.  A null pointer will cause
         *                         execution to run to completion.
         */
        void resume(Ld::ElementPointer runToElement = Ld::ElementPointer());

        /**
         * Slot you can trigger to request an executing model pause or resume depending in its current state.
         */
        void pauseOrResume();

        /**
         * Slot you can trigger to abort execution.
         */
        void abort();

        /**
         * Slot you can trigger to shutdown all operations as soon as possible.
         */
        void shutdownNow();

        /**
         * Slot you can trigger to set or clear an instruction breakpoint.
         *
         * \param[in] element The element to receive the instruction breakpoint.
         *
         * \param[in] nowSet  If true, we are setting an instruction breakpoint.  If clear, we are clearing an
         *                    instruction breakpoint.
         *
         * \return Returns true on success, returns false on error.
         */
        bool setInstructionBreakpoint(Ld::ElementPointer element, bool nowSet = true);

        /**
         * Slot you can trigger to clear or set an instruction breakpoint.
         *
         * \param[in] element  The element to receive the instruction breakpoint.
         *
         * \param[in] nowClear If true, we are clearing an instruction breakpoint.  If clear, we are setting an
         *                     instruction breakpoint.
         *
         * \return Returns true on success, returns false on error.
         */
        bool clearInstructionBreakpoint(Ld::ElementPointer element, bool nowClear = true);

        /**
         * Slot you can trigger to clear all instruction breakpoint.
         */
        void clearAllInstructionBreakpoints();

    private slots:
        /**
         * Slot that is triggered when the model has started execution.
         *
         * \param[in] modelApi A pointer to the model API class.
         */
        void modelStarted(Model::Api* modelApi);

        /**
         * Slot that is triggered when the model has completed execution.
         *
         * \param[in] modelApi A pointer to the model API class.
         */
        void modelFinished(Model::Api* modelApi);

        /**
         * Slot that is triggered when the model has been aborted.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] abortReason     The reason the model was aborted.
         *
         * \param[in] operationHandle Handle to the operation in the model that was executing when the abort was
         *                            triggered.
         */
        void modelAborted(
            Model::Api*        modelApi,
            Model::AbortReason abortReason,
            unsigned long long operationHandle
        );

        /**
         * Slot that is triggered when a thread has paused due to a call to Model::Api::pause.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] operationHandle The operation handle where the model paused.
         */
        void modelPausedOnUserRequest(Model::Api* modelApi, unsigned long long operationHandle);

        /**
         * Slot that is triggered when a thread has paused due to an instruction breakpoint.  Note that this method
         * may be called from a thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] operationHandle The operation handle where the model paused.
         */
        void modelPausedAtOperation(Model::Api* modelApi, unsigned long long operationHandle);

        /**
         * Slot that is triggered when a thread has paused due to a data breakpoint.  Note that this method may be
         * called from a thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi         A pointer to the model API class.
         *
         * \param[in] operationHandle  The operation handle where the model paused.
         *
         * \param[in] identifierHandle The identifier handle that triggered the pause condition.
         */
        void modelPausedOnVariableUpdate(
            Model::Api*        modelApi,
            unsigned long long operationHandle,
            unsigned long long identifierHandle
        );

        /**
         * Slot that is triggered when the model resumes operation.
         *
         * \param[in] modelApi A pointer to the model API class.
         */
        void modelResumed(Model::Api* modelApi);

    private:
        /**
         * Method that is called when the generator becomes idle and can be claimed.
         */
        void generatorIdle() final;

        /**
         * Method that is called when the code generator's has completed the last translation phase.
         *
         * \param[in] rootElement The root element the generator is operating on.
         *
         * \param[in] successful  A flag holding true if the translation was successful.
         */
        void translationCompleted(QSharedPointer<Ld::RootElement> rootElement, bool successful) final;

        /**
         * Method that is called each time the code generator uncovers an error.
         *
         * \param[in] rootElement The root element the generator is operating on.
         *
         * \param[out] diagnostic The diagnostic tied to the error, warning, etc.
         */
        void translationErrorDetected(
            QSharedPointer<Ld::RootElement> rootElement,
            Ld::DiagnosticPointer           diagnostic
        ) final;

        /**
         * Method that determines if the root element or library name has been changed.
         */
        bool newBuildNeeded() const;

        /**
         * Method that is called when external stimulus requests a transition to a new state.
         */
        void transitionToNewState();

        /**
         * Method that is called when the model pauses.
         */
        void modelPaused();

        /**
         * Method that is called to start an build.  This method will attempt to obtain the generator and will either
         * call BuildExecuteStateMachine::performBuild or will change the state to
         * BuildExecuteStateMachine::State::WAITING_TO_BUILD.
         */
        void startBuild();

        /**
         * Method that is called once we own the generator to actually perform the build.
         */
        void performBuild();

        /**
         * Method that is called to trigger a build to be stopped.
         */
        void stopBuild();

        /**
         * Method that is called to start model execution.
         */
        void startExecution();

        /**
         * Method that is called to abort model execution.
         */
        void abortExecution();

        /**
         * Method that is called to pause model execution.
         */
        void pauseExecution();

        /**
         * Method that is called to resume model execution.
         */
        void resumeExecution();

        /**
         * Method that is called to start a transition to idle.
         */
        void transitionToIdle();

        /**
         * Method that is called to load the model.
         *
         * \return Returns true on success, returns false if the model could not be loaded.
         */
        bool loadModel();

        /**
         * Method that is called to delete any currently built model.
         */
        void deleteModelFile();

        /**
         * Method that is called to expunge any currently loaded model and delete the associated library.
         */
        void expungeLoadedModel();

        /**
         * Method that is called to update the state and emit state transition signals.
         *
         * \param[in] newState The new state.
         */
        void updateCurrentState(State newState);

        /**
         * Method that is called to update all instruction breakpoints prior to program execution.
         */
        void configureInstructionBreakpoints();

        /**
         * Method that obtains the default build output filename to use for a root element.
         *
         * \param[in] rootElement The root element to obtain the filename for.
         *
         * \return Returns the build output filename.
         */
        static QString filenameForRootElement(QSharedPointer<Ld::RootElement> rootElement);

        /**
         * Method that is called to setup the run-to operation handle.
         *
         * \param[in] runToElement The run-to element we want execution to pause at.
         */
        void setRunToOperation(Ld::ElementPointer runToElement);

        /**
         * Method that is called to update all element tied to identifiers.
         */
        void reportIdentifierChanges();

        /**
         * Method that is called to clear any reported identifier values.
         */
        void clearIdentifierChanges();

        /**
         * Method that is called to update changes to a single variable.
         *
         * \param[in] identifier The variable identifier.
         *
         * \param[in] value      The value to be reported.
         */
        void reportSingleIdentifierChanged(const Ld::IdentifierContainer& identifier, const Model::Variant& value);

        /**
         * Method that is called to clear all reported identifier values.
         */
        void unloadIdentifierValues();

        /**
         * The library loader we use to load, unload, and execute models.
         */
        Cbe::DynamicLibraryLoader* loader;

        /**
         * Weak pointer to the current root element.
         */
        QWeakPointer<Ld::RootElement> currentRootElement;

        /**
         * Identifier database captured when the model was started.
         */
        Ld::IdentifierDatabase currentIdentifierDatabase;

        /**
         * Operation database captured when the model was started.
         */
        Ld::OperationDatabase currentOperationDatabase;

        /**
         * Weak pointer to the new root element.
         */
        QWeakPointer<Ld::RootElement> newRootElement;

        /**
         * Weak pointer to the element that the program counter is currently paused at.  This value is used to provide
         * and remove run-time diagnostics to element.
         */
        Ld::ElementWeakPointer currentPausedElement;

        /**
         * The current state.
         */
        State currentState;

        /**
         * The desired target state.
         */
        State targetState;

        /**
         * Pointer to the deallocator function we should use to unload the model.
         */
        Model::DeallocatorFunction currentDeallocatorFunction;

        /**
         * The current model API.
         */
        M::Api* currentModelApi;

        /**
         * Pointer to the identifier database being tracked by this model.
         */
        Model::IdentifierDatabase* currentModelIdentifierDatabase;

        /**
         * The loaded model status used to track our model.
         */
        LoadedModelStatus* currentStatusInstance;

        /**
         * The current filename assigned to the binary object.
         */
        QString currentFilename;

        /**
         * Flag indicating if the current build is a debug build.
         */
        bool currentDebugMode;

        /**
         * Flag indicating if the next build should be a debug build.
         */
        bool newDebugMode;

        /**
         * Element indicating that the program should run-to or resume to this location, then step.
         */
        Ld::ElementWeakPointer currentRunToElement;

        /**
         * Flag that indicates we should single step on resume.
         */
        bool currentSingleStep;

        /**
         * Flag that indicates that we're forcing a shutdown.
         */
        bool currentShutdownForced;
};

#endif
