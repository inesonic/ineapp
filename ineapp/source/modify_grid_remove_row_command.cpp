/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyGridRemoveRowCommand class.
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
#include "modify_grid_remove_row_command.h"

ModifyGridRemoveRowCommand::ModifyGridRemoveRowCommand() {
    currentRowIndex   = static_cast<unsigned long>(-1);
}


ModifyGridRemoveRowCommand::ModifyGridRemoveRowCommand(
        QSharedPointer<Ld::ElementWithGridChildren> gridElement,
        unsigned long                               rowIndex
    ):ModifyGridCommand(
        gridElement
    ),currentRowIndex(
        rowIndex
    ) {}


ModifyGridRemoveRowCommand::~ModifyGridRemoveRowCommand() {}


ModifyGridCommand::Subcommand ModifyGridRemoveRowCommand::subCommandType() const {
    return Subcommand::REMOVE_ROW;
}


bool ModifyGridRemoveRowCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridRemoveRowCommand::gridElement();
    if (!gridElement.isNull()) {
        unsigned long numberRows = gridElement->numberRows();

        if (numberRows > 1 && currentRowIndex < numberRows) {
            Q_ASSERT(removedElements.isEmpty());

            const Fixer* fixer = Fixer::fixer(gridElement->typeName());
            if (fixer != Q_NULLPTR) {
                fixer->preRemoveRow(gridElement, currentRowIndex);
            }

            unsigned long numberColumns = gridElement->numberColumns();

            if (cursorStateCollection != Q_NULLPTR) {
                unsigned long replacementRow;
                if (currentRowIndex == (numberRows - 1)) {
                    replacementRow = currentRowIndex - 1;
                } else {
                    replacementRow = currentRowIndex + 1;
                }

                for (unsigned long columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
                    Ld::ElementPointer childToRemove = gridElement->child(currentRowIndex, columnIndex);
                    removedElements.append(childToRemove);

                    for (  Ld::CursorStateCollection::iterator it  = cursorStateCollection->begin(),
                                                               end = cursorStateCollection->end()
                         ; it != end
                         ; ++it
                        ) {
                        Ld::ElementPointer cursorElement     = it->element();
                        bool               updateCursorEntry = false;
                        if (!cursorElement.isNull()) {
                            if (cursorElement == childToRemove) {
                                updateCursorEntry = true;
                            }
                        } else {
                            unsigned long childIndex = currentRowIndex * numberColumns + columnIndex;

                            if (it->parent() == gridElement && it->childIndex() == childIndex) {
                                updateCursorEntry = true;
                            }
                        }

                        if (updateCursorEntry) {
                            Ld::ElementPointer replacement = gridElement->child(replacementRow, columnIndex);
                            if (!replacement.isNull()) {
                                it->updateToFirstPositionIn(replacement, true);
                            } else {
                                unsigned long childIndex = replacementRow * numberColumns + columnIndex;
                                it->update(gridElement, childIndex);
                            }
                        }
                    }

                    if (!childToRemove.isNull()) {
                        gridElement->removeChild(childToRemove, Q_NULLPTR);
                    }
                }
            } else {
                for (unsigned long columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
                    Ld::ElementPointer childToRemove = gridElement->child(currentRowIndex, columnIndex);
                    removedElements.append(childToRemove);

                    if (!childToRemove.isNull()) {
                        gridElement->removeChild(childToRemove, Q_NULLPTR);
                    }
                }
            }

            gridElement->removeRow(currentRowIndex, Q_NULLPTR);
            cursorStateCollection->updateCursorState(false);

            result = true;
        } else {
            result = false;
        }
    } else {
        result = false;
    }

    return result;
}


bool ModifyGridRemoveRowCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridRemoveRowCommand::gridElement();
    if (!gridElement.isNull()) {
        unsigned long numberColumns = gridElement->numberColumns();

        Q_ASSERT(static_cast<unsigned long>(removedElements.size()) == numberColumns);

        gridElement->insertRowBefore(currentRowIndex, cursorStateCollection);

        for (unsigned long columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            Ld::ElementPointer oldElement = removedElements.at(columnIndex);
            gridElement->setChild(currentRowIndex, columnIndex, oldElement, cursorStateCollection);
        }

        removedElements.clear();

        result = true;
    } else {
        result = false;
    }

    return result;
}


QString ModifyGridRemoveRowCommand::description() const {
    return tr("remove row");
}


void ModifyGridRemoveRowCommand::setRowIndex(unsigned long oldRowIndex) {
    currentRowIndex = oldRowIndex;
}


unsigned long ModifyGridRemoveRowCommand::rowIndex() const {
    return currentRowIndex;
}
