/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_table_frame_element.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "command_container.h"
#include "command.h"
#include "modify_table_command.h"

ModifyTableCommand::ModifyTableCommand() {
    currentTableElementCursor.setInvalid();
}


ModifyTableCommand::ModifyTableCommand(QSharedPointer<Ld::TableFrameElement> tableElement) {
    currentTableElementCursor.setElement(tableElement);
    currentTableElementCursor.setWholeElement();
}


ModifyTableCommand::~ModifyTableCommand() {}


Command::CommandType ModifyTableCommand::commandType() const {
    return Command::CommandType::MODIFY_TABLE;
}


bool ModifyTableCommand::merge(const CommandContainer&) {
    return false;
}


QString ModifyTableCommand::description() const {
    return tr("modify table");
}


void ModifyTableCommand::setTableElement(QSharedPointer<Ld::TableFrameElement> newTableElement) {
    currentTableElementCursor.setElement(newTableElement);
    currentTableElementCursor.setWholeElement();
}


QSharedPointer<Ld::TableFrameElement> ModifyTableCommand::tableElement() const {
    return currentTableElementCursor.element().dynamicCast<Ld::TableFrameElement>();
}

