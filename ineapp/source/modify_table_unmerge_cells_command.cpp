/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableUnmergeCellsCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>
#include <ld_paragraph_element.h>
#include <ld_text_element.h>
#include <ld_paragraph_format.h>
#include <ld_character_format.h>
#include <ld_table_frame_element.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "command_container.h"
#include "command.h"
#include "modify_table_command.h"
#include "modify_table_unmerge_cells_command.h"

ModifyTableUnmergeCellsCommand::ModifyTableUnmergeCellsCommand() {
    currentRowIndex    = Ld::TableFrameElement::invalidRow;
    currentColumnIndex = Ld::TableFrameElement::invalidColumn;
}


ModifyTableUnmergeCellsCommand::ModifyTableUnmergeCellsCommand(
        QSharedPointer<Ld::TableFrameElement> tableElement,
        unsigned                              rowIndex,
        unsigned                              columnIndex
    ):ModifyTableCommand(
        tableElement
    ) {
    currentRowIndex    = rowIndex;
    currentColumnIndex = columnIndex;
}


ModifyTableUnmergeCellsCommand::~ModifyTableUnmergeCellsCommand() {}


ModifyTableCommand::Subcommand ModifyTableUnmergeCellsCommand::subCommandType() const {
    return ModifyTableCommand::Subcommand::MERGE_CELLS;
}


bool ModifyTableUnmergeCellsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    assert(originalTableFrameElement.isNull());
    originalTableFrameElement = tableElement()->clone(true).dynamicCast<Ld::TableFrameElement>();

    QSharedPointer<Ld::ParagraphElement> paragraphElement = Ld::Element::create(Ld::ParagraphElement::elementName)
                                                            .dynamicCast<Ld::ParagraphElement>();
    QSharedPointer<Ld::ParagraphFormat> paragraphFormat = Ld::Format::create(Ld::ParagraphFormat::formatName)
                                                          .dynamicCast<Ld::ParagraphFormat>();
    paragraphElement->setFormat(paragraphFormat);

    QSharedPointer<Ld::TextElement> textElement = Ld::Element::create(Ld::TextElement::elementName)
                                                  .dynamicCast<Ld::TextElement>();
    QSharedPointer<Ld::CharacterFormat> characterFormat = Ld::Format::create(Ld::CharacterFormat::formatName)
                                                          .dynamicCast<Ld::CharacterFormat>();
    textElement->setFormat(characterFormat);

    paragraphElement->append(textElement, nullptr);

    Ld::ElementPointerList children;
    children << paragraphElement;

    bool success = tableElement()->unmergeCells(currentRowIndex, currentColumnIndex, children);

    cursorStateCollection->updateCursorState(true);

    return success;
}


bool ModifyTableUnmergeCellsCommand::undo(Ld::CursorStateCollection*) {
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


QString ModifyTableUnmergeCellsCommand::description() const {
    return tr("modify table: merge cells");
}


void ModifyTableUnmergeCellsCommand::setRowIndex(unsigned newRowIndex) {
    currentRowIndex = newRowIndex;
}


unsigned ModifyTableUnmergeCellsCommand::rowIndex() const {
    return currentRowIndex;
}


void ModifyTableUnmergeCellsCommand::setColumnIndex(unsigned newColumnIndex) {
    currentColumnIndex = newColumnIndex;
}


unsigned ModifyTableUnmergeCellsCommand::columnIndex() const {
    return currentColumnIndex;
}

