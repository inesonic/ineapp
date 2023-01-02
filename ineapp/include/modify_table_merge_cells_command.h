/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyTableMergeCellsCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_TABLE_MERGE_CELLS_COMMAND_H
#define MODIFY_TABLE_MERGE_CELLS_COMMAND_H

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
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command you can use to merge two or more cells together.
 */
class APP_PUBLIC_API ModifyTableMergeCellsCommand:public ModifyTableCommand {
    Q_DECLARE_TR_FUNCTIONS(ModifyTableMergeCellsCommand)

    public:
        ModifyTableMergeCellsCommand();

        /**
         * Constructor
         *
         * \param[in] tableElement The table element to be modified.
         *
         * \param[in] rowIndex     The row index of the top row in the range of cells to merge.
         *
         * \param[in] columnIndex  The column index of the left column in the range of cells to merge.
         *
         * \param[in] rowSpan      The number of rows to be merged.
         *
         * \param[in] columnSpan   The number of columns to be merged.
         */
        ModifyTableMergeCellsCommand(
            QSharedPointer<Ld::TableFrameElement> tableElement,
            unsigned                              rowIndex,
            unsigned                              columnIndex,
            unsigned                              rowSpan,
            unsigned                              columnSpan
        );

        ~ModifyTableMergeCellsCommand() override;

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
         * Method you can use to set the row index of the top row in the merge.
         *
         * \param[in] newRowIndex The new row index value.
         */
        void setRowIndex(unsigned newRowIndex);

        /**
         * Method you can use to obtain the row index of the top row in the merge.
         *
         * \return Returns the row index of the top row in the merged range.
         */
        unsigned rowIndex() const;

        /**
         * Method you can use to set the column index of the left column in the merge.
         *
         * \param[in] newColumnIndex The new column index value.
         */
        void setColumnIndex(unsigned newColumnIndex);

        /**
         * Method you can use to obtain the column index of the left column in the merge.
         *
         * \return Returns the column index of the top column in the merged range.
         */
        unsigned columnIndex() const;

        /**
         * Method you can use to set the row span of the merged range.
         *
         * \param[in] newRowSpan The new row span value.
         */
        void setRowSpan(unsigned newRowSpan);

        /**
         * Method you can use to obtain the row span of the merged span.
         *
         * \return Returns the row span of the merged range.
         */
        unsigned rowSpan() const;

        /**
         * Method you can use to set the column span of the merged range.
         *
         * \param[in] newColumnSpan The new column span value.
         */
        void setColumnSpan(unsigned newColumnSpan);

        /**
         * Method you can use to obtain the column span of the merged span.
         *
         * \return Returns the column span of the merged range.
         */
        unsigned columnSpan() const;

    private:
        /**
         * The zero based row index of the merged cell range.
         */
        unsigned currentRowIndex;

        /**
         * The zero based column index of the merged cell range.
         */
        unsigned currentColumnIndex;

        /**
         * The row span of the merged cells.
         */
        unsigned currentRowSpan;

        /**
         * The column span of the merged cells.
         */
        unsigned currentColumnSpan;

        /**
         * Clone of the table frame element prior to having the row removed.
         */
        QSharedPointer<Ld::TableFrameElement> originalTableFrameElement;
};

#endif
