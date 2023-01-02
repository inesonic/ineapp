/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyGridAddRowsCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_GRID_ADD_ROWS_COMMAND_H
#define MODIFY_GRID_ADD_ROWS_COMMAND_H

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
 * Command class that adds rows to a grid element.
 */
class APP_PUBLIC_API ModifyGridAddRowsCommand:public ModifyGridCommand {
    Q_DECLARE_TR_FUNCTIONS(ModifyGridAddRowsCommand)

    public:
        ModifyGridAddRowsCommand();

        /**
         * Constructor
         *
         * \param[in] gridElement The table element to be modified.
         *
         * \param[in] rowIndex    The row index of the first row to be inserted.
         *
         * \param[in] numberRows  The number of rows to be inserted.
         */
        ModifyGridAddRowsCommand(
            QSharedPointer<Ld::ElementWithGridChildren> gridElement,
            unsigned long                               rowIndex,
            unsigned long                               numberRows
        );

        ~ModifyGridAddRowsCommand() override;

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
         * Method you can use to set the row index for the newly added rows.
         *
         * \param[in] newRowIndex The new row index.
         */
        void setRowIndex(unsigned long newRowIndex);

        /**
         * Method you can use to obtain the row index for the newly added row.
         *
         * \return Returns the row index for the newly added row.
         */
        unsigned long rowIndex() const;

        /**
         * Method you can use to set the number of rows.
         *
         * \param[in] newNumberRows The new number of rows.
         */
        void setNumberRows(unsigned long newNumberRows);

        /**
         * Method you can use to obtain the number of rows.
         *
         * \return Returns the number of rows.
         */
        unsigned long numberRows() const;

    private:
        /**
         * The current first row index.
         */
        unsigned long currentRowIndex;

        /**
         * The current number of rows.
         */
        unsigned long currentNumberRows;
};

#endif
