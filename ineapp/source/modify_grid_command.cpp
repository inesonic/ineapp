/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyGridCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_element_with_grid_children.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "command_container.h"
#include "command.h"
#include "modify_grid_command.h"

ModifyGridCommand::ModifyGridCommand() {
    currentGridElementCursor.setInvalid();
}


ModifyGridCommand::ModifyGridCommand(QSharedPointer<Ld::ElementWithGridChildren> gridElement) {
    currentGridElementCursor.setElement(gridElement);
    currentGridElementCursor.setWholeElement();
}


ModifyGridCommand::~ModifyGridCommand() {}


Command::CommandType ModifyGridCommand::commandType() const {
    return Command::CommandType::MODIFY_GRID;
}


bool ModifyGridCommand::merge(const CommandContainer&) {
    return false;
}


void ModifyGridCommand::setGridElement(QSharedPointer<Ld::ElementWithGridChildren> newGridElement) {
    currentGridElementCursor.setElement(newGridElement);
    currentGridElementCursor.setWholeElement();
}


QSharedPointer<Ld::ElementWithGridChildren> ModifyGridCommand::gridElement() const {
    return currentGridElementCursor.element().dynamicCast<Ld::ElementWithGridChildren>();
}

