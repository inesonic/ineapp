/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableRemoveRowsCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>
#include <ld_table_frame_element.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "command_container.h"
#include "command.h"
#include "modify_table_command.h"
#include "modify_table_remove_rows_command.h"

ModifyTableRemoveRowsCommand::ModifyTableRemoveRowsCommand() {
    currentRowIndex   = Ld::TableFrameElement::invalidRow;
    currentNumberRows = Ld::TableFrameElement::invalidRow;
}


ModifyTableRemoveRowsCommand::ModifyTableRemoveRowsCommand(
        QSharedPointer<Ld::TableFrameElement> tableElement,
        unsigned                              rowIndex,
        unsigned                              numberRows
    ):ModifyTableCommand(
        tableElement
    ) {
    currentRowIndex   = rowIndex;
    currentNumberRows = numberRows;
}


ModifyTableRemoveRowsCommand::~ModifyTableRemoveRowsCommand() {}


ModifyTableCommand::Subcommand ModifyTableRemoveRowsCommand::subCommandType() const {
    return ModifyTableCommand::Subcommand::REMOVE_ROWS;
}


bool ModifyTableRemoveRowsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    assert(originalTableFrameElement.isNull());
    originalTableFrameElement = tableElement()->clone(true).dynamicCast<Ld::TableFrameElement>();

    for (unsigned rowNumber=0 ; rowNumber<currentNumberRows ; ++rowNumber) {
        tableElement()->removeRow(currentRowIndex, true, cursorStateCollection);
    }

    cursorStateCollection->updateCursorState(true);

    return true;
}


bool ModifyTableRemoveRowsCommand::undo(Ld::CursorStateCollection*) {
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


QString ModifyTableRemoveRowsCommand::description() const {
    QString result;
    if (currentNumberRows == 1) {
        result = tr("modify table: remove row");
    } else {
        result = tr("modify table: remove %1 rows").arg(currentNumberRows);
    }

    return result;
}


void ModifyTableRemoveRowsCommand::setRowIndex(unsigned newRowIndex) {
    currentRowIndex = newRowIndex;
}


unsigned ModifyTableRemoveRowsCommand::rowIndex() const {
    return currentRowIndex;
}


void ModifyTableRemoveRowsCommand::setNumberRows(unsigned newNumberRows) {
    currentNumberRows = newNumberRows;
}


unsigned ModifyTableRemoveRowsCommand::numberRows() const {
    return currentNumberRows;
}
