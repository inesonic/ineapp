/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref CommandContainer class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>
#include <QString>
#include <QSharedPointer>
#include <QSet>

#include <ld_cursor.h>
#include <ld_cursor_state_collection.h>

#include <command.h>
#include <command_container.h>

#include "test_command_container.h"

/***********************************************************************************************************************
 * TestCommand
 */

class TestCommand:public Command {
    public:
        TestCommand(
            Command::CommandType newCommandType,
            bool                 newMergeStatus,
            bool                 newExecuteStatus,
            bool                 newUndoStatus,
            const QString&       newDescription
        );

        ~TestCommand() override;

        Command::CommandType commandType() const final;

        bool merge(const CommandContainer& commandToMerge) final;

        bool execute(Ld::CursorStateCollection* cursorStateCollection) final;

        bool undo(Ld::CursorStateCollection* cursorStateCollection) final;

        QString description() const final;

        static unsigned numberInstances();

        static void clearNumberInstances();

    private:
        static unsigned currentNumberInstances;

        Command::CommandType currentCommandType;
        bool                 currentMergeStatus;
        bool                 currentExecuteStatus;
        bool                 currentUndoStatus;
        QString              currentDescription;
};


unsigned TestCommand::currentNumberInstances = 0;

TestCommand::TestCommand(
        Command::CommandType newCommandType,
        bool                 newMergeStatus,
        bool                 newExecuteStatus,
        bool                 newUndoStatus,
        const QString&       newDescription
    ) {
    currentCommandType   = newCommandType;
    currentMergeStatus   = newMergeStatus;
    currentExecuteStatus = newExecuteStatus;
    currentUndoStatus    = newUndoStatus;
    currentDescription   = newDescription;

    ++currentNumberInstances;
}


TestCommand::~TestCommand() {
    --currentNumberInstances;
}


Command::CommandType TestCommand::commandType() const {
    return currentCommandType;
}


bool TestCommand::merge(const CommandContainer&) {
    return currentMergeStatus;
}


bool TestCommand::execute(Ld::CursorStateCollection*) {
    return currentExecuteStatus;
}


bool TestCommand::undo(Ld::CursorStateCollection*) {
    return currentUndoStatus;
}


QString TestCommand::description() const {
    return currentDescription;
}


unsigned TestCommand::numberInstances() {
    return currentNumberInstances;
}


void TestCommand::clearNumberInstances() {
    currentNumberInstances = 0;
}

/***********************************************************************************************************************
 * TestCommandContainer
 */

TestCommandContainer::TestCommandContainer() {}


TestCommandContainer::~TestCommandContainer() {}


void TestCommandContainer::initTestCase() {}


void TestCommandContainer::testConstructorsAndDestructors() {
    TestCommand::clearNumberInstances();

    {
        CommandContainer container1;
        QCOMPARE(container1.isValid(), false);

        CommandContainer container2(new TestCommand(
            Command::CommandType::INSERT_STRING,
            true,
            true,
            true,
            "description1"
        ));

        QCOMPARE(container2.isValid(), true);
        QCOMPARE(container2.commandType(), Command::CommandType::INSERT_STRING);

        QSharedPointer<Command> command2(new TestCommand(
            Command::CommandType::INSERT_ELEMENT,
            true,
            true,
            true,
            "description2"
        ));

        CommandContainer container3(command2);

        QCOMPARE(container3.isValid(), true);
        QCOMPARE(container3.commandType(), Command::CommandType::INSERT_ELEMENT);

        CommandContainer container4 = container3;
        QCOMPARE(container4.isValid(), true);
        QCOMPARE(container4.commandType(), Command::CommandType::INSERT_ELEMENT);

        QCOMPARE(TestCommand::numberInstances(), 2U);
    }

    QCOMPARE(TestCommand::numberInstances(), 0U);
}


void TestCommandContainer::testAccessors() {
    Ld::CursorStateCollection cursorStateCollection;

    QSharedPointer<TestCommand> commandPointer(new TestCommand(
        Command::CommandType::UPDATE_FORMAT,
        false,
        false,
        false,
        "descriptionx"
    ));

    CommandContainer container1(new TestCommand(
        Command::CommandType::INSERT_ELEMENT,
        true,
        false,
        true,
        "description1"
    ));

    QCOMPARE(container1.isValid(), true);
    QCOMPARE(container1.isInvalid(), false);

    QCOMPARE(container1.commandType(), Command::CommandType::INSERT_ELEMENT);
    QCOMPARE(container1.merge(commandPointer), true);
    QCOMPARE(container1.execute(&cursorStateCollection), false);
    QCOMPARE(container1.undo(&cursorStateCollection), true);
    QCOMPARE(container1.description(), QString("description1"));

    CommandContainer container2(new TestCommand(
        Command::CommandType::DELETE_CONTENT,
        false,
        true,
        false,
        "description2"
    ));

    QCOMPARE(container2.isValid(), true);
    QCOMPARE(container2.isInvalid(), false);

    QCOMPARE(container2.commandType(), Command::CommandType::DELETE_CONTENT);
    QCOMPARE(container2.merge(container1), false);
    QCOMPARE(container2.execute(&cursorStateCollection), true);
    QCOMPARE(container2.undo(&cursorStateCollection), false);
    QCOMPARE(container2.description(), QString("description2"));

    CommandContainer container3(new TestCommand(
        Command::CommandType::CUT_COPY,
        true,
        true,
        false,
        "description3"
    ));

    QCOMPARE(container3.isValid(), true);
    QCOMPARE(container3.isInvalid(), false);

    QCOMPARE(container3.commandType(), Command::CommandType::CUT_COPY);
    QCOMPARE(container3.merge(container2), true);
    QCOMPARE(container3.execute(&cursorStateCollection), true);
    QCOMPARE(container3.undo(&cursorStateCollection), false);
    QCOMPARE(container3.description(), QString("description3"));
}


void TestCommandContainer::testPointerReferenceOperator() {
    CommandContainer container(new TestCommand(
        Command::CommandType::CUT_COPY,
        true,
        false,
        true,
        "description1"
    ));
    QCOMPARE(container->commandType(), Command::CommandType::CUT_COPY);
    QCOMPARE(
        (*const_cast<const CommandContainer*>(&container))->commandType(),
        Command::CommandType::CUT_COPY
    );
}


void TestCommandContainer::testAssignmentOperators() {
    CommandContainer container1(new TestCommand(
        Command::CommandType::PASTE,
        true,
        false,
        true,
        "description1"
    ));
    QCOMPARE(container1.isValid(), true);
    QCOMPARE(container1.commandType(), Command::CommandType::PASTE);

    CommandContainer container2;
    QCOMPARE(container2.isValid(), false);

    container2 = container1;
    QCOMPARE(container2.isValid(), true);
    QCOMPARE(container2.commandType(), Command::CommandType::PASTE);
}
