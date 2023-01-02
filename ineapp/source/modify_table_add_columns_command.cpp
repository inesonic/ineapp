/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableAddColumnsCommand class.
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
#include "modify_table_add_columns_command.h"

ModifyTableAddColumnsCommand::ModifyTableAddColumnsCommand() {
    currentColumnIndex   = Ld::TableFrameElement::invalidColumn;
    currentNumberColumns = Ld::TableFrameElement::invalidColumn;
}


ModifyTableAddColumnsCommand::ModifyTableAddColumnsCommand(
        QSharedPointer<Ld::TableFrameElement> tableElement,
        unsigned                              columnIndex,
        unsigned                              numberColumns
    ):ModifyTableCommand(
        tableElement
    ) {
    currentColumnIndex   = columnIndex;
    currentNumberColumns = numberColumns;
}


ModifyTableAddColumnsCommand::~ModifyTableAddColumnsCommand() {}


ModifyTableCommand::Subcommand ModifyTableAddColumnsCommand::subCommandType() const {
    return ModifyTableCommand::Subcommand::ADD_COLUMNS;
}


bool ModifyTableAddColumnsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
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
    tableElement()->insertColumnsBefore(currentColumnIndex, currentNumberColumns, true, children);

    cursorStateCollection->updateCursorState(true);

    return true;
}


bool ModifyTableAddColumnsCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    QSharedPointer<Ld::TableFrameElement> tableElement = ModifyTableAddColumnsCommand::tableElement();

    for (unsigned remainingColumns=0 ; remainingColumns<currentNumberColumns; ++remainingColumns) {
        tableElement->removeColumn(currentColumnIndex, false, cursorStateCollection);
    }

    tableElement->setFormat(oldFormat);
    oldFormat.reset();

    cursorStateCollection->updateCursorState(false);

    return true;
}


QString ModifyTableAddColumnsCommand::description() const {
    QString result;
    if (currentNumberColumns == 1) {
        result = tr("modify table: add column");
    } else {
        result = tr("modify table: add %1 columns").arg(currentNumberColumns);
    }

    return result;
}


void ModifyTableAddColumnsCommand::setColumnIndex(unsigned newColumnIndex) {
    currentColumnIndex = newColumnIndex;
}


unsigned ModifyTableAddColumnsCommand::columnIndex() const {
    return currentColumnIndex;
}


void ModifyTableAddColumnsCommand::setNumberColumns(unsigned newNumberColumns) {
    currentNumberColumns = newNumberColumns;
}


unsigned ModifyTableAddColumnsCommand::numberColumns() const {
    return currentNumberColumns;
}
