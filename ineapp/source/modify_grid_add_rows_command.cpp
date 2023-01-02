/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyGridAddRowsCommand class.
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
#include "modify_grid_add_rows_command.h"

ModifyGridAddRowsCommand::ModifyGridAddRowsCommand() {
    currentRowIndex   = static_cast<unsigned long>(-1);
    currentNumberRows = 0;
}


ModifyGridAddRowsCommand::ModifyGridAddRowsCommand(
        QSharedPointer<Ld::ElementWithGridChildren> gridElement,
        unsigned long                               rowIndex,
        unsigned long                               numberRows
    ):ModifyGridCommand(
        gridElement
    ),currentRowIndex(
        rowIndex
    ),currentNumberRows(
        numberRows
    ) {}


ModifyGridAddRowsCommand::~ModifyGridAddRowsCommand() {}


ModifyGridCommand::Subcommand ModifyGridAddRowsCommand::subCommandType() const {
    return Subcommand::ADD_ROWS;
}


bool ModifyGridAddRowsCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridAddRowsCommand::gridElement();
    if (!gridElement.isNull()) {
        const Fixer* fixer = Fixer::fixer(gridElement->typeName());

        for (unsigned i=0 ; i<currentNumberRows ; ++i) {
            Ld::ElementPointer defaultContent;
            if (fixer != Q_NULLPTR) {
                defaultContent = fixer->preInsertRow(gridElement, currentRowIndex);
            }

            gridElement->insertRowBefore(currentRowIndex, cursorStateCollection, defaultContent);

            if (fixer != Q_NULLPTR) {
                fixer->postInsertRow(gridElement, currentRowIndex);
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


bool ModifyGridAddRowsCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridAddRowsCommand::gridElement();
    if (!gridElement.isNull()) {
        const Fixer* fixer = Fixer::fixer(gridElement->typeName());

        for (unsigned i=0 ; i<currentNumberRows ; ++i) {
            if (fixer != Q_NULLPTR) {
                fixer->preRemoveRow(gridElement, currentRowIndex);
            }

            gridElement->removeRow(currentRowIndex, cursorStateCollection);
        }

        result = true;
    } else {
        result = false;
    }

    return result;
}


QString ModifyGridAddRowsCommand::description() const {
    QString result;

    if (currentNumberRows == 1) {
        result = tr("add row");
    } else {
        result = tr("add %1 rows").arg(currentNumberRows);
    }

    return result;
}


void ModifyGridAddRowsCommand::setRowIndex(unsigned long newRowIndex) {
    currentRowIndex = newRowIndex;
}


unsigned long ModifyGridAddRowsCommand::rowIndex() const {
    return currentRowIndex;
}


void ModifyGridAddRowsCommand::setNumberRows(unsigned long newNumberRows) {
    currentNumberRows = newNumberRows;
}


unsigned long ModifyGridAddRowsCommand::numberRows() const {
    return currentNumberRows;
}
