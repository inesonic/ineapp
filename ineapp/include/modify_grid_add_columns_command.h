/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyGridAddColumnsCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_GRID_ADD_COLUMNS_COMMAND_H
#define MODIFY_GRID_ADD_COLUMNS_COMMAND_H

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
 * Command class that adds columns to a grid element.
 */
class APP_PUBLIC_API ModifyGridAddColumnsCommand:public ModifyGridCommand {
    Q_DECLARE_TR_FUNCTIONS(ModifyGridAddColumnsCommand)

    public:
        ModifyGridAddColumnsCommand();

        /**
         * Constructor
         *
         * \param[in] gridElement   The table element to be modified.
         *
         * \param[in] columnIndex   The column index of the first column to be inserted.
         *
         * \param[in] numberColumns The number of columns to be inserted.
         */
        ModifyGridAddColumnsCommand(
            QSharedPointer<Ld::ElementWithGridChildren> gridElement,
            unsigned long                               columnIndex,
            unsigned long                               numberColumns
        );

        ~ModifyGridAddColumnsCommand() override;

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
         * Method you can use to set the column index for the newly added columns.
         *
         * \param[in] newColumnIndex The new column index.
         */
        void setColumnIndex(unsigned long newColumnIndex);

        /**
         * Method you can use to obtain the column index for the newly added column.
         *
         * \return Returns the column index for the newly added column.
         */
        unsigned long columnIndex() const;

        /**
         * Method you can use to set the number of columns.
         *
         * \param[in] newNumberColumns The new number of columns.
         */
        void setNumberColumns(unsigned long newNumberColumns);

        /**
         * Method you can use to obtain the number of columns.
         *
         * \return Returns the number of columns.
         */
        unsigned long numberColumns() const;

    private:
        /**
         * The current first column index.
         */
        unsigned long currentColumnIndex;

        /**
         * The current number of columns.
         */
        unsigned long currentNumberColumns;
};

#endif
