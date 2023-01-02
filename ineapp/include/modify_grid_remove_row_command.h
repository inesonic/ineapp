/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyGridRemoveRowCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_GRID_REMOVE_ROW_COMMAND_H
#define MODIFY_GRID_REMOVE_ROW_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include <ld_element_structures.h>
#include <ld_element_cursor.h>
#include <ld_element.h>

#include "app_common.h"
#include "cursor.h"
#include "modify_grid_command.h"

namespace Ld {
    class Element;
    class ElementWithGridChildren;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command class that removes a single row from from a grid element.
 */
class APP_PUBLIC_API ModifyGridRemoveRowCommand:public ModifyGridCommand {
    Q_DECLARE_TR_FUNCTIONS(ModifyGridRemoveRowCommand)

    public:
        ModifyGridRemoveRowCommand();

        /**
         * Constructor
         *
         * \param[in] gridElement The table element to be modified.
         *
         * \param[in] rowIndex    The row index of the row to be removed.
         */
        ModifyGridRemoveRowCommand(
            QSharedPointer<Ld::ElementWithGridChildren> gridElement,
            unsigned long                               rowIndex
        );

        ~ModifyGridRemoveRowCommand() override;

        /**
         * Method that returns the sub-command.
         *
         * \return Returns the sub-command for this operation.
         */
        Subcommand subCommandType() const override;

        /**
         * Method that executes the command.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool execute(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that performs an undo of the command using stored information.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undo(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that returns a description of the command suitable for displaying in an undo/redo stack window.
         *
         * \return Returns a description of the command.
         */
        QString description() const override;

        /**
         * Method you can use to set the row index for the row to remove.
         *
         * \param[in] oldRowIndex The row index.
         */
        void setRowIndex(unsigned long oldRowIndex);

        /**
         * Method you can use to obtain the row index for the row to remove.
         *
         * \return Returns the row index for the row to be removed.
         */
        unsigned long rowIndex() const;

    private:
        /**
         * The current row index.
         */
        unsigned long currentRowIndex;

        /**
         * List of elements that were removed.  Required to support undo.
         */
        Ld::ElementPointerList removedElements;
};

#endif
