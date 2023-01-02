/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CommandContainer class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_cursor_state_collection.h>

#include "command.h"
#include "cursor.h"
#include "command_container.h"

const Cursor CommandContainer::invalidCursor;

CommandContainer::CommandContainer() {}


CommandContainer::CommandContainer(QSharedPointer<Command> command):currentCommand(command) {}


CommandContainer::CommandContainer(Command* command):currentCommand(command) {}


CommandContainer::CommandContainer(const CommandContainer& other):currentCommand(other.currentCommand) {}


CommandContainer::~CommandContainer() {}


bool CommandContainer::isValid() const {
    return !currentCommand.isNull();
}


bool CommandContainer::isInvalid() const {
    return currentCommand.isNull();
}


Command& CommandContainer::command() {
    return *currentCommand;
}


Command::CommandType CommandContainer::commandType() const {
    Command::CommandType result;

    if (!currentCommand.isNull()) {
        result = currentCommand->commandType();
    } else {
        result = Command::CommandType::INVALID;
    }

    return result;
}


bool CommandContainer::merge(QSharedPointer<Command> commandToMerge) {
    bool result;

    if (!currentCommand.isNull()) {
        result = currentCommand->merge(commandToMerge);
    } else {
        result = false;
    }

    return result;
}


bool CommandContainer::merge(const CommandContainer& commandToMerge) {
    bool result;

    if (!currentCommand.isNull()) {
        result = currentCommand->merge(commandToMerge.currentCommand);
    } else {
        result = false;
    }

    return result;
}


bool CommandContainer::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    if (!currentCommand.isNull()) {
        result = currentCommand->execute(cursorStateCollection);
    } else {
        result = false;
    }

    return result;
}


bool CommandContainer::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    if (!currentCommand.isNull()) {
        result = currentCommand->undo(cursorStateCollection);
    } else {
        result = false;
    }

    return result;
}


bool CommandContainer::redo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    if (!currentCommand.isNull()) {
        result = currentCommand->redo(cursorStateCollection);
    } else {
        result = false;
    }

    return result;
}


QString CommandContainer::description() const {
    QString result;

    if (!currentCommand.isNull()) {
        result = currentCommand->description();
    }

    return result;
}


QString CommandContainer::detailedDescription() const {
    QString result;

    if (!currentCommand.isNull()) {
        result = currentCommand->detailedDescription();
    }

    return result;
}


CursorPointer CommandContainer::cursor() const {
    CursorPointer result;

    if (!currentCommand.isNull()) {
        result = currentCommand->cursor();
    } else {
        result = Q_NULLPTR;
    }

    return result;
}


const Cursor& CommandContainer::cursorAtIssue() const {
    return currentCommand.isNull() ? invalidCursor : currentCommand->cursorAtIssue();
}


Command& CommandContainer::operator*() const {
    return *currentCommand.data();
}


Command* CommandContainer::operator->() const {
    return currentCommand.data();
}


CommandContainer& CommandContainer::operator=(const CommandContainer& other) {
    currentCommand = other.currentCommand;
    return *this;
}


CommandContainer& CommandContainer::operator=(QSharedPointer<Command> other) {
    currentCommand = other;
    return *this;
}
