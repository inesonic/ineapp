/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableAddRowsCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_paragraph_element.h>
#include <ld_paragraph_format.h>
#include <ld_text_element.h>
#include <ld_character_format.h>
#include <ld_table_frame_element.h>
#include <ld_table_frame_format.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "command_container.h"
#include "command.h"
#include "modify_table_command.h"
#include "modify_table_add_rows_command.h"

ModifyTableAddRowsCommand::ModifyTableAddRowsCommand() {
    currentRowIndex   = Ld::TableFrameElement::invalidRow;
    currentNumberRows = Ld::TableFrameElement::invalidRow;
}


ModifyTableAddRowsCommand::ModifyTableAddRowsCommand(
        QSharedPointer<Ld::TableFrameElement> tableElement,
        unsigned                              rowIndex,
        unsigned                              numberRows
    ):ModifyTableCommand(
        tableElement
    ) {
    currentRowIndex   = rowIndex;
    currentNumberRows = numberRows;
}


ModifyTableAddRowsCommand::~ModifyTableAddRowsCommand() {}


ModifyTableCommand::Subcommand ModifyTableAddRowsCommand::subCommandType() const {
    return ModifyTableCommand::Subcommand::ADD_ROWS;
}


bool ModifyTableAddRowsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    QSharedPointer<Ld::ParagraphElement> paragraph       = Ld::Element::create(Ld::ParagraphElement::elementName)
                                                           .dynamicCast<Ld::ParagraphElement>();
    QSharedPointer<Ld::ParagraphFormat>  paragraphFormat = Ld::Format::create(Ld::ParagraphFormat::formatName)
                                                           .dynamicCast<Ld::ParagraphFormat>();
    paragraph->setFormat(paragraphFormat);

    Ld::ElementPointer textElement     = Ld::Element::create(Ld::TextElement::elementName);
    Ld::FormatPointer  characterFormat = Ld::Format::create(Ld::CharacterFormat::formatName);
    textElement->setFormat(characterFormat);

    paragraph->append(textElement, nullptr);
    Ld::ElementPointerList children;
    children << paragraph;

    oldFormat = tableElement()->format()->clone().dynamicCast<Ld::TableFrameFormat>();
    tableElement()->insertRowsBefore(currentRowIndex, currentNumberRows, true, children);

    cursorStateCollection->updateCursorState(true);

    return true;
}


bool ModifyTableAddRowsCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    QSharedPointer<Ld::TableFrameElement> tableElement = ModifyTableAddRowsCommand::tableElement();
    for (unsigned remainingRows=0 ; remainingRows<currentNumberRows ; ++remainingRows) {
        tableElement->removeRow(currentRowIndex, false, cursorStateCollection);
    }

    tableElement->setFormat(oldFormat);
    oldFormat.reset();

    cursorStateCollection->updateCursorState(false);

    return true;
}


QString ModifyTableAddRowsCommand::description() const {
    QString result;
    if (currentNumberRows == 1) {
        result = tr("modify table: add row");
    } else {
        result = tr("modify table: add %1 rows").arg(currentNumberRows);
    }

    return result;
}


void ModifyTableAddRowsCommand::setRowIndex(unsigned newRowIndex) {
    currentRowIndex = newRowIndex;
}


unsigned ModifyTableAddRowsCommand::rowIndex() const {
    return currentRowIndex;
}


void ModifyTableAddRowsCommand::setNumberRows(unsigned newNumberRows) {
    currentNumberRows = newNumberRows;
}


unsigned ModifyTableAddRowsCommand::numberRows() const {
    return currentNumberRows;
}
