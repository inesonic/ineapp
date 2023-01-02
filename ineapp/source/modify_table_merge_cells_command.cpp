/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableMergeCellsCommand class.
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
#include "modify_table_merge_cells_command.h"

ModifyTableMergeCellsCommand::ModifyTableMergeCellsCommand() {
    currentRowIndex    = Ld::TableFrameElement::invalidRow;
    currentColumnIndex = Ld::TableFrameElement::invalidColumn;
    currentRowSpan     = Ld::TableFrameElement::invalidRow;
    currentColumnSpan  = Ld::TableFrameElement::invalidColumn;
}


ModifyTableMergeCellsCommand::ModifyTableMergeCellsCommand(
        QSharedPointer<Ld::TableFrameElement> tableElement,
        unsigned                              rowIndex,
        unsigned                              columnIndex,
        unsigned                              rowSpan,
        unsigned                              columnSpan
    ):ModifyTableCommand(
        tableElement
    ) {
    currentRowIndex    = rowIndex;
    currentColumnIndex = columnIndex;
    currentRowSpan     = rowSpan;
    currentColumnSpan  = columnSpan;
}


ModifyTableMergeCellsCommand::~ModifyTableMergeCellsCommand() {}


ModifyTableCommand::Subcommand ModifyTableMergeCellsCommand::subCommandType() const {
    return ModifyTableCommand::Subcommand::MERGE_CELLS;
}


bool ModifyTableMergeCellsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    assert(originalTableFrameElement.isNull());
    originalTableFrameElement = tableElement()->clone(true).dynamicCast<Ld::TableFrameElement>();

    tableElement()->mergeCells(
        currentRowIndex,
        currentColumnIndex,
        currentColumnSpan - 1,
        currentRowSpan - 1,
        true,
        cursorStateCollection
    );

    cursorStateCollection->updateCursorState(true);

    return true;
}


bool ModifyTableMergeCellsCommand::undo(Ld::CursorStateCollection*) {
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


QString ModifyTableMergeCellsCommand::description() const {
    return tr("modify table: merge cells");
}


void ModifyTableMergeCellsCommand::setRowIndex(unsigned newRowIndex) {
    currentRowIndex = newRowIndex;
}


unsigned ModifyTableMergeCellsCommand::rowIndex() const {
    return currentRowIndex;
}


void ModifyTableMergeCellsCommand::setColumnIndex(unsigned newColumnIndex) {
    currentColumnIndex = newColumnIndex;
}


unsigned ModifyTableMergeCellsCommand::columnIndex() const {
    return currentColumnIndex;
}


void ModifyTableMergeCellsCommand::setRowSpan(unsigned newRowSpan) {
    currentRowSpan = newRowSpan;
}


unsigned ModifyTableMergeCellsCommand::rowSpan() const {
    return currentRowSpan;
}


void ModifyTableMergeCellsCommand::setColumnSpan(unsigned newColumnSpan) {
    currentColumnSpan = newColumnSpan;
}


unsigned ModifyTableMergeCellsCommand::columnSpan() const {
    return currentColumnSpan;
}

