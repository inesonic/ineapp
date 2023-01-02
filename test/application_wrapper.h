/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides a wrapper for tests pulling the QtTest framework more cleanly into the signal/slot mechanism.
***********************************************************************************************************************/

#ifndef APPLICATION_WRAPPER_H
#define APPLICATION_WRAPPER_H

#include <QtGlobal>
#include <QObject>
#include <QList>

#include <eqt_programmatic_application.h>

/**
 * Class that pulls the QtTest functions under an instance of \ref UniqueApplication.  The class allows tests to be
 * run under the application framework expected by the various classes.
 *
 * This approach was inspired by comments on the web page:
 *
 *     http://stackoverflow.com/questions/1524390/what-unit-testing-framework-should-i-use-for-qt
 */
class ApplicationWrapper:public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] argumentCount The number of command line arguments.
         *
         * \param[in] argumentValues The command line arguments.
         *
         * \param[in] parent Pointer to the parent object.
         */
        ApplicationWrapper(
            int&     argumentCount,
            char**   argumentValues,
            QObject* parent = Q_NULLPTR
        );

        ~ApplicationWrapper() override;

        /**
         * Adds a new test class to the test framework.
         *
         * \param[in] testInstance The test instance to be added.
         */
        void includeTest(QObject* testInstance);

        /**
         * Executes all registered tests for the application.
         *
         * \return Returns 0 on success, non-zero on error.
         */
        int exec();

    private slots:
        /**
         * Signal that is emitted when the application instance's main loop is running and the \ref UniqueApplication
         * instance has determined that we are the primary instance.
         *
         * \param[out] startupCondition Indicates the state of the application when it starts up.
         */
        void startUserInterface(EQt::UniqueApplication::StartupCondition const startupCondition);

        /**
         * Starts the tests, in succession.  Triggered by the signal/slot event loop.
         */
        void runNextTest();

    private:
        void startNextTest();

        EQt::ProgrammaticApplication* applicationInstance;
        QList<QObject*>               registeredTests;
        QList<QObject*>::iterator     nextTest;
        int                           currentStatus;
};

#endif
