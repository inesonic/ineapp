/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Command class.
***********************************************************************************************************************/

#include <QPointer>
#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_cursor.h>
#include <ld_cursor_state_collection.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_element_with_positional_children.h>
#include <ld_element_with_grouped_children.h>

#include "cursor.h"
#include "command_container.h"
#include "command.h"

Command::Command() {}


Command::Command(CursorPointer newCursor) {
    currentCursor        = newCursor;
    currentCursorAtIssue = *newCursor;
}


Command::~Command() {}


bool Command::merge(const CommandContainer&) {
    return false;
}


bool Command::redo(Ld::CursorStateCollection* cursorStateCollection) {
    return execute(cursorStateCollection);
}


QString Command::detailedDescription() const {
    return description();
}


void Command::setCursor(CursorPointer newCursor) {
    currentCursor        = newCursor.toWeakRef();
    currentCursorAtIssue = *newCursor;
}


CursorPointer Command::cursor() const {
    return currentCursor.toStrongRef();
}


void Command::setCursorAtIssue(const Cursor& newCursor) {
    currentCursorAtIssue = newCursor;
}


const Cursor& Command::cursorAtIssue() const {
    return currentCursorAtIssue;
}
