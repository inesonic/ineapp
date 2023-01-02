/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableRemoveColumnsCommand class.
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
#include "modify_table_remove_columns_command.h"

ModifyTableRemoveColumnsCommand::ModifyTableRemoveColumnsCommand() {
    currentColumnIndex   = Ld::TableFrameElement::invalidColumn;
    currentNumberColumns = Ld::TableFrameElement::invalidColumn;
}


ModifyTableRemoveColumnsCommand::ModifyTableRemoveColumnsCommand(
        QSharedPointer<Ld::TableFrameElement> tableElement,
        unsigned                              columnIndex,
        unsigned                              numberColumns
    ):ModifyTableCommand(
        tableElement
    ) {
    currentColumnIndex   = columnIndex;
    currentNumberColumns = numberColumns;
}


ModifyTableRemoveColumnsCommand::~ModifyTableRemoveColumnsCommand() {}


ModifyTableCommand::Subcommand ModifyTableRemoveColumnsCommand::subCommandType() const {
    return ModifyTableCommand::Subcommand::ADD_COLUMNS;
}


bool ModifyTableRemoveColumnsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    assert(originalTableFrameElement.isNull());
    originalTableFrameElement = tableElement()->clone(true).dynamicCast<Ld::TableFrameElement>();

    for (unsigned columnNumber=0 ; columnNumber<currentNumberColumns ; ++columnNumber) {
        tableElement()->removeColumn(currentColumnIndex, true, cursorStateCollection);
    }

    cursorStateCollection->updateCursorState(true);

    return true;
}


bool ModifyTableRemoveColumnsCommand::undo(Ld::CursorStateCollection*) {
    assert(!originalTableFrameElement.isNull());

    Ld::ElementPointer element = cursorAtIssue().elementCursor().element();
    while (!element.isNull() && element->typeName() != Ld::TableFrameElement::elementName) {
        element = element->parent();
    }

    assert(!element.isNull());
    QSharedPointer<Ld::TableFrameElement> table  = element.dynamicCast<Ld::TableFrameElement>();
    Ld::ElementPointer                    parent = table->parent();

    assert(parent->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL);
    QSharedPointer<Ld::ElementWithPositionalChildren>
        positionalParent = parent.dynamicCast<Ld::ElementWithPositionalChildren>();

    unsigned tableIndex = positionalParent->indexOfChild(table);
    positionalParent->removeChild(table, nullptr);
    positionalParent->insertBefore(tableIndex, originalTableFrameElement, nullptr);

    originalTableFrameElement.reset();

    *cursor() = cursorAtIssue();

    return true;
}


QString ModifyTableRemoveColumnsCommand::description() const {
    QString result;
    if (currentNumberColumns == 1) {
        result = tr("modify table: remove column");
    } else {
        result = tr("modify table: remove %1 columns").arg(currentNumberColumns);
    }

    return result;
}


void ModifyTableRemoveColumnsCommand::setColumnIndex(unsigned newColumnIndex) {
    currentColumnIndex = newColumnIndex;
}


unsigned ModifyTableRemoveColumnsCommand::columnIndex() const {
    return currentColumnIndex;
}


void ModifyTableRemoveColumnsCommand::setNumberColumns(unsigned newNumberColumns) {
    currentNumberColumns = newNumberColumns;
}


unsigned ModifyTableRemoveColumnsCommand::numberColumns() const {
    return currentNumberColumns;
}
