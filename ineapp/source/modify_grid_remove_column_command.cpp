/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyGridRemoveColumnCommand class.
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
#include "modify_grid_remove_column_command.h"

ModifyGridRemoveColumnCommand::ModifyGridRemoveColumnCommand() {
    currentColumnIndex = static_cast<unsigned long>(-1);
}


ModifyGridRemoveColumnCommand::ModifyGridRemoveColumnCommand(
        QSharedPointer<Ld::ElementWithGridChildren> gridElement,
        unsigned long                               columnIndex
    ):ModifyGridCommand(
        gridElement
    ),currentColumnIndex(
        columnIndex
    ) {}


ModifyGridRemoveColumnCommand::~ModifyGridRemoveColumnCommand() {}


ModifyGridCommand::Subcommand ModifyGridRemoveColumnCommand::subCommandType() const {
    return Subcommand::REMOVE_ROW;
}


bool ModifyGridRemoveColumnCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridRemoveColumnCommand::gridElement();
    if (!gridElement.isNull()) {
        unsigned long numberColumns = gridElement->numberColumns();

        if (numberColumns > 1 && currentColumnIndex < numberColumns) {
            Q_ASSERT(removedElements.isEmpty());

            const Fixer* fixer = Fixer::fixer(gridElement->typeName());
            if (fixer != Q_NULLPTR) {
                fixer->preRemoveColumn(gridElement, currentColumnIndex);
            }

            unsigned long numberRows = gridElement->numberRows();

            if (cursorStateCollection != Q_NULLPTR) {
                unsigned long replacementColumn;
                if (currentColumnIndex == (numberColumns - 1)) {
                    replacementColumn = currentColumnIndex - 1;
                } else {
                    replacementColumn = currentColumnIndex + 1;
                }

                for (unsigned long rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                    Ld::ElementPointer childToRemove = gridElement->child(rowIndex, currentColumnIndex);
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
                            unsigned long childIndex = rowIndex * numberColumns + currentColumnIndex;

                            if (it->parent() == gridElement && it->childIndex() == childIndex) {
                                updateCursorEntry = true;
                            }
                        }

                        if (updateCursorEntry) {
                            Ld::ElementPointer replacement = gridElement->child(rowIndex, replacementColumn);
                            if (!replacement.isNull()) {
                                it->updateToFirstPositionIn(replacement, true);
                            } else {
                                unsigned long childIndex = rowIndex * numberColumns + replacementColumn;
                                it->update(gridElement, childIndex);
                            }
                        }
                    }

                    if (!childToRemove.isNull()) {
                        gridElement->removeChild(childToRemove, Q_NULLPTR);
                    }
                }
            } else {
                for (unsigned long rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                    Ld::ElementPointer childToRemove = gridElement->child(rowIndex, currentColumnIndex);
                    removedElements.append(childToRemove);

                    if (!childToRemove.isNull()) {
                        gridElement->removeChild(childToRemove, Q_NULLPTR);
                    }
                }
            }

            gridElement->removeColumn(currentColumnIndex, Q_NULLPTR);
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


bool ModifyGridRemoveColumnCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    QSharedPointer<Ld::ElementWithGridChildren> gridElement = ModifyGridRemoveColumnCommand::gridElement();
    if (!gridElement.isNull()) {
        unsigned long numberRows = gridElement->numberRows();

        Q_ASSERT(static_cast<unsigned long>(removedElements.size()) == numberRows);

        gridElement->insertColumnBefore(currentColumnIndex, cursorStateCollection);

        for (unsigned long rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
            Ld::ElementPointer oldElement = removedElements.at(rowIndex);
            gridElement->setChild(rowIndex, currentColumnIndex, oldElement, cursorStateCollection);
        }

        removedElements.clear();

        result = true;
    } else {
        result = false;
    }

    return result;
}


QString ModifyGridRemoveColumnCommand::description() const {
    return tr("remove column");
}


void ModifyGridRemoveColumnCommand::setColumnIndex(unsigned long oldColumnIndex) {
    currentColumnIndex = oldColumnIndex;
}


unsigned long ModifyGridRemoveColumnCommand::columnIndex() const {
    return currentColumnIndex;
}
