/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyGridAddColumnsCommand class.
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
#include "fixer.h"
#include "modify_grid_command.h"
#include "modify_grid_add_columns_command.h"

ModifyGridAddColumnsCommand::ModifyGridAddColumnsCommand() {
    currentColumnIndex   = static_cast<unsigned long>(-1);
    currentNumberColumns = 0;
}


ModifyGridAddColumnsCommand::ModifyGridAddColumnsCommand(
        QSharedPointer<Ld::ElementWithGridChildren> gridElement,
        unsigned long                               columnIndex,
        unsigned long                               numberColumns
    ):ModifyGridCommand(
        gridElement
    ),currentColumnIndex(
        columnIndex
    ),currentNumberColumns(
        numberColumns
    ) {}


ModifyGridAddColumnsCommand::~ModifyGridAddColumnsCommand() {}


ModifyGridCommand::Subcommand ModifyGridAddColumnsCommand::subCommandType() const {
    return Subcommand::ADD_ROWS;
}


bool ModifyGridAddColumnsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridAddColumnsCommand::gridElement();
    if (!gridElement.isNull()) {
        const Fixer* fixer = Fixer::fixer(gridElement->typeName());

        for (unsigned i=0 ; i<currentNumberColumns ; ++i) {
            Ld::ElementPointer defaultContent;
            if (fixer != Q_NULLPTR) {
                defaultContent = fixer->preInsertColumn(gridElement, currentColumnIndex);
            }

            gridElement->insertColumnBefore(currentColumnIndex, cursorStateCollection, defaultContent);

            if (fixer != Q_NULLPTR) {
                fixer->postInsertColumn(gridElement, currentColumnIndex);
            }

            if (cursorStateCollection != Q_NULLPTR) {
                cursorStateCollection->updateCursorState(false);
            }
        }

        result = true;
    } else {
        result = false;
    }

    return result;
}


bool ModifyGridAddColumnsCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridAddColumnsCommand::gridElement();
    if (!gridElement.isNull()) {
        const Fixer* fixer = Fixer::fixer(gridElement->typeName());

        for (unsigned i=0 ; i<currentNumberColumns ; ++i) {
            if (fixer != Q_NULLPTR) {
                fixer->preRemoveColumn(gridElement, currentColumnIndex);
            }

            gridElement->removeColumn(currentColumnIndex, cursorStateCollection);
        }

        result = true;
    } else {
        result = false;
    }

    return result;
}


QString ModifyGridAddColumnsCommand::description() const {
    QString result;

    if (currentNumberColumns == 1) {
        result = tr("add column");
    } else {
        result = tr("add %1 columns").arg(currentNumberColumns);
    }

    return result;
}


void ModifyGridAddColumnsCommand::setColumnIndex(unsigned long newColumnIndex) {
    currentColumnIndex = newColumnIndex;
}


unsigned long ModifyGridAddColumnsCommand::columnIndex() const {
    return currentColumnIndex;
}


void ModifyGridAddColumnsCommand::setNumberColumns(unsigned long newNumberColumns) {
    currentNumberColumns = newNumberColumns;
}


unsigned long ModifyGridAddColumnsCommand::numberColumns() const {
    return currentNumberColumns;
}
