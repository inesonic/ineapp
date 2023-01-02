/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyTableCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_TABLE_COMMAND_H
#define MODIFY_TABLE_COMMAND_H

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
#include "command_base_with_undo.h"

namespace Ld {
    class Element;
    class TableFrameElement;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Base class for commands that can be issued to modify a table.  You can use this command to perform a number of
 * specifc types of modifications:
 *
 *     * Add rows
 *     * Remove rows
 *     * Add columns
 *     * Remove columns
 *     * Delete table, remove contents
 *     * Delete table, preserve contents
 *     * Merge cells
 *     * Split cells
 */
class APP_PUBLIC_API ModifyTableCommand:public CommandBase{
    Q_DECLARE_TR_FUNCTIONS(ModifyTableCommand)

    public:
        /**
         * Enumeration indicating the table operation to be performed.
         */
        enum class Subcommand {
            /**
             * Indicates no operation.
             */
            INVALID,

            /**
             * Indicates we're adding one or more rows to a table.
             */
            ADD_ROWS,

            /**
             * Indicates we're removing one or more rows from a table.
             */
            REMOVE_ROWS,

            /**
             * Indicates we're adding one or more columns to a table.
             */
            ADD_COLUMNS,

            /**
             * Indicates we're removing one or more columns from a table.
             */
            REMOVE_COLUMNS,

            /**
             * Indicates we're deleting the entire table with contents.
             */
            DELETE_TABLE_AND_CONTENTS,

            /**
             * Indicates we're deleting the entire table, replacing it with it's contents.
             */
            DELETE_TABLE_PRESERVE_CONTENTS,

            /**
             * Indicates we're merging cells.
             */
            MERGE_CELLS,

            /**
             * Indicates we're splitting merged cells.
             */
            SPLIT_CELLS
        };

        ModifyTableCommand();

        /**
         * Constructor
         *
         * \param[in] tableElement The table element to be modified.
         */
        ModifyTableCommand(QSharedPointer<Ld::TableFrameElement> tableElement);

        ~ModifyTableCommand() override;

        /**
         * Method that returns the command type.
         *
         * \return Returns the command type value.
         */
        CommandType commandType() const final;

        /**
         * Method that returns the sub-command.
         *
         * \return Returns the sub-command for this operation.
         */
        virtual Subcommand subCommandType() const = 0;

        /**
         * Method that attempts to merge another command with this command.  If the merge is successful, the merged
         * command can be successfully destroyed, information will be included into this object instance.
         *
         * \param[in] commandToMerge Shared pointer to the command to be merged into this command.
         *
         * \return Returns true on success, returns false if the commands could not be merged.  The default
         *         implementation always returns false.
         */
        bool merge(const CommandContainer& commandToMerge) final;

        /**
         * Method that returns a description of the command suitable for displaying in an undo/redo stack window.
         *
         * \return Returns a description of the command.
         */
        QString description() const override;

        /**
         * Method you can use to set the table element to be modified.
         *
         * \param[in] newTableElement The new table element to be modified.
         */
        void setTableElement(QSharedPointer<Ld::TableFrameElement> newTableElement);

        /**
         * Method that returns the table element that is going to be modified.
         *
         * \return Returns a shared pointer to the table element to be modified.
         */
        QSharedPointer<Ld::TableFrameElement> tableElement() const;

    private:
        /**
         * Cursor used to locate the table element this command should operate on.
         */
        Ld::ElementCursor currentTableElementCursor;
};

#endif
