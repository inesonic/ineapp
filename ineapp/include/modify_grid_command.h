/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyGridCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_GRID_COMMAND_H
#define MODIFY_GRID_COMMAND_H

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
    class ElementWithGridChildren;
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
 */
class APP_PUBLIC_API ModifyGridCommand:public CommandBase {
    Q_DECLARE_TR_FUNCTIONS(ModifyGridCommand)

    public:
        /**
         * Enumeration indicating the grid modification operation to be performed.
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
             * Indicates we're adding one or more columns to the table.
             */
            ADD_COLUMNS,

            /**
             * Indicates we're removing a row from the table.
             */
            REMOVE_ROW,

            /**
             * Indicates we're removing a row from a table.
             */
            REMOVE_COLUMNS
        };

        ModifyGridCommand();

        /**
         * Constructor
         *
         * \param[in] gridElement The grid element to be modified.
         */
        ModifyGridCommand(QSharedPointer<Ld::ElementWithGridChildren> gridElement);

        ~ModifyGridCommand() override;

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
         * Method you can use to set the grid element to be modified.
         *
         * \param[in] newGridElement The new grid element to be modified.
         */
        void setGridElement(QSharedPointer<Ld::ElementWithGridChildren> newGridElement);

        /**
         * Method that returns the grid element that is going to be modified.
         *
         * \return Returns a shared pointer to the grid element to be modified.
         */
        QSharedPointer<Ld::ElementWithGridChildren> gridElement() const;

    private:
        /**
         * Cursor used to locate the grid element this command should operate on.
         */
        Ld::ElementCursor currentGridElementCursor;
};

#endif
