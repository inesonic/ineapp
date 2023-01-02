/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ApplicationWrapper class that pulls the QtTest framework more cleanly into the Qt
* signal/slot mechanisms of Qt.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QList>
#include <QtTest/QtTest>
#include <QTimer>

#include <eqt_programmatic_application.h>

#include "application_wrapper.h"

ApplicationWrapper::ApplicationWrapper(
        int&     argumentCount,
        char**   argumentValues,
        QObject* parent
    ):QObject(
         parent
    ) {
    applicationInstance = new EQt::ProgrammaticApplication(
        argumentCount,
        argumentValues,
        tr("test_ineapp"),
        tr("1.00"),
        tr("Inesonic, LLC"),
        tr("inesonic.com")
    );

    connect(
        applicationInstance,
        &EQt::ProgrammaticApplication::startUserInterface,
        this,
        &ApplicationWrapper::startUserInterface
    );
}

ApplicationWrapper::~ApplicationWrapper() {
    delete applicationInstance;
}


void ApplicationWrapper::includeTest(QObject* testInstance) {
    testInstance->setParent(this);
    registeredTests.append(testInstance);
}


int ApplicationWrapper::exec() {
    int exitStatus = applicationInstance->exec();
    return exitStatus | currentStatus;
}


void ApplicationWrapper::startUserInterface(EQt::UniqueApplication::StartupCondition const) {
    nextTest = registeredTests.begin();
    startNextTest();
}


void ApplicationWrapper::runNextTest() {
    currentStatus |= QTest::qExec(*nextTest);

    ++nextTest;
    startNextTest();
}


void ApplicationWrapper::startNextTest() {
    if (nextTest == registeredTests.end()) {
        applicationInstance->quit();
    } else {
        QTimer::singleShot(1, this, SLOT(runNextTest()));
    }
}
