/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CppCodeGeneratorVisual class.
***********************************************************************************************************************/

#include <QObject>
#include <QSharedPointer>
#include <QSet>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>

#include <cstdint>

#include <m_console.h>

#include <ld_element_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_code_generator.h>
#include <ld_cpp_code_generator.h>
#include <ld_code_generator_output_type_container.h>
#include <ld_translation_phase.h>
#include <ld_cpp_code_generator_diagnostic.h>
#include <ld_code_generator_visual.h>
#include <ld_environment.h>

#include "metatypes.h"
#include "application.h"
#include "console_device.h"
#include "cpp_code_generator_visual.h"

/***********************************************************************************************************************
 * CppCodeGeneratorVisual::Client
 */

CppCodeGeneratorVisual::Client::Client(bool alwaysReceiveStatusUpdates) {
    currentAlwaysReceiveUpdates = alwaysReceiveStatusUpdates;

    QSharedPointer<Ld::CppCodeGenerator>
        codeGenerator = Ld::CodeGenerator::codeGenerator(Ld::CppCodeGenerator::codeGeneratorName)
                        .dynamicCast<Ld::CppCodeGenerator>();

    if (!codeGenerator.isNull()) {
        CppCodeGeneratorVisual* visual = dynamic_cast<CppCodeGeneratorVisual*>(codeGenerator->visual());
        if (visual != Q_NULLPTR) {
            visual->addClient(this);
        }
    }
}


CppCodeGeneratorVisual::Client::~Client() {
    if (currentVisual != Q_NULLPTR) {
        currentVisual->removeClient(this);
        currentVisual = Q_NULLPTR;
    }
}


bool CppCodeGeneratorVisual::Client::alwaysReceivesStatusUpdates() const {
    return currentAlwaysReceiveUpdates;
}


bool CppCodeGeneratorVisual::Client::tryToOwn() {
    bool result;

    if (currentVisual != Q_NULLPTR) {
        result = currentVisual->tryToOwn(this);
    } else {
        result = false;
    }

    return result;
}


void CppCodeGeneratorVisual::Client::waitForOwnership() {
    if (currentVisual != Q_NULLPTR) {
        currentVisual->waitForOwnership(this);
    }
}


bool CppCodeGeneratorVisual::Client::releaseOwnership() {
    bool result;

    if (currentVisual != Q_NULLPTR) {
        result = currentVisual->releaseOwnership(this);
    } else {
        result = false;
    }

    return result;
}


bool CppCodeGeneratorVisual::Client::translate(
        QSharedPointer<Ld::RootElement>             rootElement,
        const QString&                              outputFile,
        const Ld::CodeGeneratorOutputTypeContainer& outputType,
        CppCodeGeneratorVisual::Client::ExportMode  exportMode,
        const QString&                              runtimeLibrary
    ) {
    bool result;

    if (currentVisual != Q_NULLPTR) {
        result = currentVisual->translate(this, rootElement, outputFile, outputType, exportMode, runtimeLibrary);
    } else {
        result = false;
    }

    return result;
}


bool CppCodeGeneratorVisual::Client::translate(
        QSharedPointer<Ld::RootElement>            rootElement,
        const QString&                             outputFile,
        CppCodeGeneratorVisual::Client::ExportMode exportMode,
        const QString&                             runtimeLibrary
    ) {
    bool result;

    if (currentVisual != Q_NULLPTR) {
        result = currentVisual->translate(this, rootElement, outputFile, exportMode, runtimeLibrary);
    } else {
        result = false;
    }

    return result;
}


QString CppCodeGeneratorVisual::Client::intermediateRepresentation() const {
    QString result;

    if (currentVisual != Q_NULLPTR) {
        result = currentVisual->intermediateRepresentation();
    }

    return result;
}


bool CppCodeGeneratorVisual::Client::active() const {
    bool result;

    if (currentVisual != Q_NULLPTR) {
        result = currentVisual->active();
    } else {
        result = false;
    }

    return result;
}


bool CppCodeGeneratorVisual::Client::abort() {
    bool result;

    if (currentVisual != Q_NULLPTR) {
        result = currentVisual->abort(this);
    } else {
        result = false;
    }

    return result;
}


void CppCodeGeneratorVisual::Client::clientOwnsGenerator() {}


void CppCodeGeneratorVisual::Client::generatorBusy() {}


void CppCodeGeneratorVisual::Client::generatorIdle() {}


void CppCodeGeneratorVisual::Client::translationStarted(QSharedPointer<Ld::RootElement>) {}


void CppCodeGeneratorVisual::Client::translationPhaseStarted(
        QSharedPointer<Ld::RootElement>,
        unsigned,
        unsigned,
        const QString&
    ) {}


void CppCodeGeneratorVisual::Client::translationCompleted(QSharedPointer<Ld::RootElement>, bool) {}


void CppCodeGeneratorVisual::Client::translationAborted(QSharedPointer<Ld::RootElement>) {}


void CppCodeGeneratorVisual::Client::translationErrorDetected(
        QSharedPointer<Ld::RootElement>,
        Ld::DiagnosticPointer
    ) {}

/***********************************************************************************************************************
 * CppCodeGeneratorVisual
 */

CppCodeGeneratorVisual::CppCodeGeneratorVisual(QObject* parent):QObject(parent) {
    connect(
        this,
        &CppCodeGeneratorVisual::translationHasStarted,
        this,
        &CppCodeGeneratorVisual::processTranslationStarted,
        Qt::BlockingQueuedConnection
    );

    connect(
        this,
        &CppCodeGeneratorVisual::translationPhaseHasStarted,
        this,
        &CppCodeGeneratorVisual::processTranslationPhaseStarted,
        Qt::BlockingQueuedConnection
    );

    connect(
        this,
        &CppCodeGeneratorVisual::translationHasCompleted,
        this,
        &CppCodeGeneratorVisual::processTranslationCompleted,
        Qt::BlockingQueuedConnection
    );

    connect(
        this,
        &CppCodeGeneratorVisual::translationWasAborted,
        this,
        &CppCodeGeneratorVisual::processTranslationAborted,
        Qt::BlockingQueuedConnection
    );

    connect(
        this,
        &CppCodeGeneratorVisual::translationErrorWasDetected,
        this,
        &CppCodeGeneratorVisual::processTranslationErrorDetected,
        Qt::BlockingQueuedConnection
    );

    currentClient = Q_NULLPTR;
}


CppCodeGeneratorVisual::~CppCodeGeneratorVisual() {
    for (QSet<Client*>::iterator it=currentClients.begin(),end=currentClients.end() ; it!=end ; ++it) {
        Client* client = *it;
        client->currentVisual = Q_NULLPTR;
    }
}


void CppCodeGeneratorVisual::addClient(CppCodeGeneratorVisual::Client* newClient) {
    QMutexLocker locker(&accessMutex);

    currentClients.insert(newClient);
    newClient->currentVisual = this;
}


void CppCodeGeneratorVisual::removeClient(CppCodeGeneratorVisual::Client* client) {
    QMutexLocker locker(&accessMutex);

    currentClients.remove(client);
    client->currentVisual = Q_NULLPTR;

    if (client == currentClient) {
        currentClient = Q_NULLPTR;
        updateOwnerStatus();

        Ld::CppCodeGenerator* generator = dynamic_cast<Ld::CppCodeGenerator*>(codeGenerator());
        generator->abort();
    }
}


void CppCodeGeneratorVisual::processTranslationStarted(
        QSharedPointer<Ld::RootElement>,
        const Ld::CodeGeneratorOutputTypeContainer&,
        unsigned long
    ) {
    if (currentClient != Q_NULLPTR) {
        currentClient->translationStarted(currentRootElement);
    }

    for (QSet<Client*>::const_iterator it=currentClients.constBegin(),end=currentClients.constEnd() ; it!=end ; ++it) {
        Client* client = *it;
        if (client->alwaysReceivesStatusUpdates() && client != currentClient) {
            client->translationStarted(currentRootElement);
        }
    }
}


void CppCodeGeneratorVisual::processTranslationPhaseStarted(
        QSharedPointer<Ld::RootElement>             rootElement,
        const Ld::CodeGeneratorOutputTypeContainer&,
        const Ld::TranslationPhase&                 phase
    ) {
    unsigned phaseNumber  = phase.phaseNumber();
    unsigned numberPhases = phase.numberPhases();

    if (currentClient != Q_NULLPTR) {
        currentClient->translationPhaseStarted(rootElement, phaseNumber, numberPhases, phase.currentPhaseName());
    }

    for (QSet<Client*>::const_iterator it=currentClients.constBegin(),end=currentClients.constEnd() ; it!=end ; ++it) {
        Client* client = *it;
        if (client->alwaysReceivesStatusUpdates() && client != currentClient) {
            client->translationPhaseStarted(rootElement, phaseNumber, numberPhases, phase.currentPhaseName());
        }
    }
}


void CppCodeGeneratorVisual::processTranslationCompleted(
        bool                                        success,
        QSharedPointer<Ld::RootElement>             rootElement,
        const Ld::CodeGeneratorOutputTypeContainer&
    ) {
    if (currentClient != Q_NULLPTR) {
        currentClient->translationCompleted(rootElement, success);
    }

    for (QSet<Client*>::const_iterator it=currentClients.constBegin(),end=currentClients.constEnd() ; it!=end ; ++it) {
        Client* client = *it;
        if (client->alwaysReceivesStatusUpdates() && client != currentClient) {
            client->translationCompleted(rootElement, success);
        }
    }
}


void CppCodeGeneratorVisual::processTranslationAborted(
        QSharedPointer<Ld::RootElement>             rootElement,
        const Ld::CodeGeneratorOutputTypeContainer&
    ) {
    if (currentClient != Q_NULLPTR) {
        currentClient->translationAborted(rootElement);
    }

    for (QSet<Client*>::const_iterator it=currentClients.constBegin(),end=currentClients.constEnd() ; it!=end ; ++it) {
        Client* client = *it;
        if (client->alwaysReceivesStatusUpdates() && client != currentClient) {
            client->translationAborted(rootElement);
        }
    }
}


void CppCodeGeneratorVisual::processTranslationErrorDetected(Ld::DiagnosticPointer diagnostic) {
    if (currentClient != Q_NULLPTR) {
        currentClient->translationErrorDetected(currentRootElement, diagnostic);
    }

    for (QSet<Client*>::const_iterator it=currentClients.constBegin(),end=currentClients.constEnd() ; it!=end ; ++it) {
        Client* client = *it;
        if (client->alwaysReceivesStatusUpdates() && client != currentClient) {
            client->translationErrorDetected(currentRootElement, diagnostic);
        }
    }
}


bool CppCodeGeneratorVisual::tryToOwn(Client* client) {
    bool success = accessMutex.tryLock();
    if (success) {
        currentClient = client;
        updateOwnerStatus();
    }

    return success;
}


void CppCodeGeneratorVisual::waitForOwnership(Client* client) {
    accessMutex.lock();
    currentClient = client;
    updateOwnerStatus();
}


bool CppCodeGeneratorVisual::releaseOwnership(Client* client) {
    bool success = (currentClient == client);
    if (success) {
        currentClient = Q_NULLPTR;
        accessMutex.unlock();
    }

    return success;
}


QString CppCodeGeneratorVisual::intermediateRepresentation() const {
    return codeGenerator()->intermediateRepresentation();
}


void CppCodeGeneratorVisual::translationStarted(
        QSharedPointer<Ld::RootElement>             rootElement,
        const Ld::CodeGeneratorOutputTypeContainer& outputType,
        unsigned long                               numberPerElementTranslationSteps
    ) {
    if (Application::debugModeEnabled()) {
        translateStartTime      = QDateTime::currentMSecsSinceEpoch();
        translatePhaseStartTime = translateStartTime;
        ConsoleDevice* console = Application::consoleDevice();
        console->reportImmediate(
            M::Console::MessageType::INFORMATION,
            QString("Translate started")
        );
    }

    emit translationHasStarted(rootElement, outputType, numberPerElementTranslationSteps);
}


void CppCodeGeneratorVisual::translationPhaseStarted(
        QSharedPointer<Ld::RootElement>             rootElement,
        const Ld::CodeGeneratorOutputTypeContainer& outputType,
        const Ld::TranslationPhase&                 phase
    ) {
    if (Application::debugModeEnabled()) {
        std::uint64_t  currentTime = QDateTime::currentMSecsSinceEpoch();
        ConsoleDevice* console     = Application::consoleDevice();

        if (phase.phaseNumber() != 0) {
            console->reportImmediate(
                M::Console::MessageType::INFORMATION,
                QString("    -- Required %1 mSec").arg(currentTime - translatePhaseStartTime)
            );
        }

        translatePhaseStartTime = currentTime;
        console->reportImmediate(
            M::Console::MessageType::INFORMATION,
            QString("  Started phase %1").arg(phase.currentPhaseName())
        );
    }

    emit translationPhaseHasStarted(rootElement, outputType, phase);
}


void CppCodeGeneratorVisual::translationCompleted(
        bool                                        success,
        QSharedPointer<Ld::RootElement>             rootElement,
        const Ld::CodeGeneratorOutputTypeContainer& outputType
    ) {
    if (Application::debugModeEnabled()) {
        std::uint64_t  translateEndTime = QDateTime::currentMSecsSinceEpoch();
        ConsoleDevice* console          = Application::consoleDevice();
        console->reportImmediate(
            M::Console::MessageType::INFORMATION,
            QString("    -- Required %1 mSec").arg(translateEndTime - translatePhaseStartTime)
        );
        console->reportImmediate(
            M::Console::MessageType::INFORMATION,
            QString("    Translate finished, operation required %1 mSec").arg(translateEndTime - translateStartTime)
        );
    }

    emit translationHasCompleted(success, rootElement, outputType);
}


void CppCodeGeneratorVisual::translationAborted(
        QSharedPointer<Ld::RootElement>             rootElement,
        const Ld::CodeGeneratorOutputTypeContainer& outputType
    ) {
    if (Application::debugModeEnabled()) {
        ConsoleDevice* console = Application::consoleDevice();
        console->reportImmediate(
            M::Console::MessageType::INFORMATION,
            QString("    Translate aborted")
        );
    }

    emit translationWasAborted(rootElement, outputType);
}


void CppCodeGeneratorVisual::translationErrorDetected(Ld::DiagnosticPointer diagnostic) {
    emit translationErrorWasDetected(diagnostic);
}


bool CppCodeGeneratorVisual::translate(
        CppCodeGeneratorVisual::Client*             client,
        QSharedPointer<Ld::RootElement>             rootElement,
        const QString&                              outputFile,
        const Ld::CodeGeneratorOutputTypeContainer& outputType,
        CppCodeGeneratorVisual::Client::ExportMode  exportMode,
        const QString&                              runtimeLibrary
    ) {
    bool success         = false;
    bool releaseWhenDone = false;

    if (currentClient == Q_NULLPTR) {
        success = tryToOwn(client);
        releaseWhenDone = success;
    } else if (currentClient == client) {
        success = true;
    }

    if (success) {
        currentRootElement = rootElement;

        Ld::CppCodeGenerator* generator = dynamic_cast<Ld::CppCodeGenerator*>(codeGenerator());

        #if (defined(Q_OS_WIN))

            generator->setStaticLibraries(Ld::Environment::standardStaticLibraries(runtimeLibrary));

            QList<QString> librarySearchPaths = Ld::Environment::librarySearchPaths(runtimeLibrary);
            librarySearchPaths += Ld::Environment::plugInLibrarySearchPaths();
            generator->setLibrarySearchPaths(librarySearchPaths);

        #elif (defined(Q_OS_LINUX) || defined(Q_OS_DARWIN))

            (void) runtimeLibrary;

        #else

            #error Undefined platform.

        #endif

        success = generator->translate(rootElement, outputFile, outputType, exportMode);
    }

    if (releaseWhenDone) {
        success = releaseOwnership(client) && success;
    }

    return success;
}


bool CppCodeGeneratorVisual::translate(
        CppCodeGeneratorVisual::Client*            client,
        QSharedPointer<Ld::RootElement>            rootElement,
        const QString&                             outputFile,
        CppCodeGeneratorVisual::Client::ExportMode exportMode,
        const QString&                             runtimeLibrary
    ) {
    bool success         = false;
    bool releaseWhenDone = false;

    if (currentClient == Q_NULLPTR) {
        success = tryToOwn(client);
        releaseWhenDone = success;
    } else if (currentClient == client) {
        success = true;
    }

    if (success) {
        currentRootElement = rootElement;

        Ld::CppCodeGenerator* generator = dynamic_cast<Ld::CppCodeGenerator*>(codeGenerator());

        #if (defined(Q_OS_WIN))

            generator->setStaticLibraries(Ld::Environment::standardStaticLibraries(runtimeLibrary));

            QList<QString> librarySearchPaths = Ld::Environment::librarySearchPaths(runtimeLibrary);
            librarySearchPaths += Ld::Environment::plugInLibrarySearchPaths();
            generator->setLibrarySearchPaths(librarySearchPaths);

        #elif (defined(Q_OS_LINUX) || defined(Q_OS_DARWIN))

            (void) runtimeLibrary;

        #else

            #error Undefined platform.

        #endif

        success = generator->translate(rootElement, outputFile, exportMode);
    }

    if (releaseWhenDone) {
        success = releaseOwnership(client) && success;
    }

    return success;
}


bool CppCodeGeneratorVisual::active() const {
    return currentClient != Q_NULLPTR;
}


bool CppCodeGeneratorVisual::abort(CppCodeGeneratorVisual::Client* client) {
    bool result;

    if (currentClient == client) {
        Ld::CppCodeGenerator* generator = dynamic_cast<Ld::CppCodeGenerator*>(codeGenerator());
        generator->abort();

        result = true;
    } else {
        result = false;
    }

    return result;
}


void CppCodeGeneratorVisual::updateOwnerStatus() {
    for (QSet<Client*>::iterator it=currentClients.begin(),end=currentClients.end() ; it!=end ; ++it) {
        Client* client = *it;
        if (currentClient == Q_NULLPTR) {
            client->generatorIdle();
        } else {
            if (currentClient == client) {
                client->clientOwnsGenerator();
            } else {
                client->generatorBusy();
            }
        }
    }
}
