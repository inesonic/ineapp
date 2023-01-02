/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyTableAddColumnsCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_TABLE_ADD_COLUMNS_COMMAND_H
#define MODIFY_TABLE_ADD_COLUMNS_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include <ld_element_structures.h>
#include <ld_element_cursor.h>
#include <ld_element.h>
#include "ld_cursor.h"

#include "app_common.h"
#include "modify_table_command.h"

namespace Ld {
    class Element;
    class TableFrameElement;
    class TableFrameFormat;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command you can use to add one or more columns to a table.
 */
class APP_PUBLIC_API ModifyTableAddColumnsCommand:public ModifyTableCommand {
    Q_DECLARE_TR_FUNCTIONS(ModifyTableAddColumnsCommand)

    public:
        ModifyTableAddColumnsCommand();

        /**
         * Constructor
         *
         * \param[in] tableElement The table element to be modified.
         *
         * \param[in] columnIndex   The column index where the first column is to be added.  An invalid column index
         *                          will cause columns to be appended to the table.
         *
         * \param[in] numberColumns The number of columns to be added to the table.
         */
        ModifyTableAddColumnsCommand(
            QSharedPointer<Ld::TableFrameElement> tableElement,
            unsigned                              columnIndex,
            unsigned                              numberColumns
        );

        ~ModifyTableAddColumnsCommand() override;

        /**
         * Method that returns the sub-command.
         *
         * \return Returns the sub-command for this operation.
         */
        Subcommand subCommandType() const final;

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
        QString description() const final;

        /**
         * Method you can use to set the column index where the new columns are to be added.
         *
         * \param[in] newColumnIndex The new column index value.
         */
        void setColumnIndex(unsigned newColumnIndex);

        /**
         * Method you can use to obtain the column index where the new columns are to be added.
         *
         * \return Returns the column index of the first newly added column.
         */
        unsigned columnIndex() const;

        /**
         * Method you can use to set the number of columns to be added.
         *
         * \param[in] newNumberColumns The new number of columns to be added.
         */
        void setNumberColumns(unsigned newNumberColumns);

        /**
         * Method you can use to obtain the number of columns to be added.
         *
         * \return Returns the number of columns to be added.
         */
        unsigned numberColumns() const;

    private:
        /**
         * The zero based column index of the first newly added column.
         */
        unsigned currentColumnIndex;

        /**
         * The number of columns to be added.
         */
        unsigned currentNumberColumns;

        /**
         * The table frame format at the point that the command was executed.  The format is stored to support undo
         * operations.
         */
        QSharedPointer<Ld::TableFrameFormat> oldFormat;
};

#endif
