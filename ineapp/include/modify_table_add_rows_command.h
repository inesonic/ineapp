/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyTableAddRowsCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_TABLE_ADD_ROWS_COMMAND_H
#define MODIFY_TABLE_ADD_ROWS_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include <ld_element_structures.h>
#include <ld_element_cursor.h>
#include <ld_element.h>
#include <ld_cursor.h>

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
 * Command you can use to add one or more rows to a table.
 */
class APP_PUBLIC_API ModifyTableAddRowsCommand:public ModifyTableCommand {
    Q_DECLARE_TR_FUNCTIONS(ModifyTableAddRowsCommand)

    public:
        ModifyTableAddRowsCommand();

        /**
         * Constructor
         *
         * \param[in] tableElement The table element to be modified.
         *
         * \param[in] rowIndex     The row index where the first row is to be added.  An invalid row index will cause
         *                         rows to be appended to the table.
         *
         * \param[in] numberRows   The number of rows to be added to the table.
         */
        ModifyTableAddRowsCommand(
            QSharedPointer<Ld::TableFrameElement> tableElement,
            unsigned                              rowIndex,
            unsigned                              numberRows
        );

        ~ModifyTableAddRowsCommand() override;

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
         * Method you can use to set the row index where the new rows are to be added.
         *
         * \param[in] newRowIndex The new row index value.
         */
        void setRowIndex(unsigned newRowIndex);

        /**
         * Method you can use to obtain the row index where the new rows are to be added.
         *
         * \return Returns the row index of the first newly added row.
         */
        unsigned rowIndex() const;

        /**
         * Method you can use to set the number of rows to be added.
         *
         * \param[in] newNumberRows The new number of rows to be added.
         */
        void setNumberRows(unsigned newNumberRows);

        /**
         * Method you can use to obtain the number of rows to be added.
         *
         * \return Returns the number of rows to be added.
         */
        unsigned numberRows() const;

    private:
        /**
         * The zero based row index of the first newly added row.
         */
        unsigned currentRowIndex;

        /**
         * The number of rows to be added.
         */
        unsigned currentNumberRows;

        /**
         * The table frame format at the point that the command was executed.  The format is stored to support undo
         * operations.
         */
        QSharedPointer<Ld::TableFrameFormat> oldFormat;
};

#endif
