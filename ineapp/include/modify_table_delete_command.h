/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ModifyTableDeleteCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MODIFY_TABLE_DELETE_COMMAND_H
#define MODIFY_TABLE_DELETE_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include <ld_element_cursor.h>
#include <ld_element.h>

#include "ld_element_structures.h"
#include "ld_cursor.h"

#include "app_common.h"
#include "modify_table_command.h"

namespace Ld {
    class Element;
    class TableFrameElement;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command you can use to delete an entire table.
 */
class APP_PUBLIC_API ModifyTableDeleteCommand:public ModifyTableCommand {
    Q_DECLARE_TR_FUNCTIONS(ModifyTableDeleteCommand)

    public:
        ModifyTableDeleteCommand();

        /**
         * Constructor
         *
         * \param[in] tableElement The table element to be deleted.
         */
        ModifyTableDeleteCommand(QSharedPointer<Ld::TableFrameElement> tableElement);

        ~ModifyTableDeleteCommand() override;

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

    private:
        /**
         * Index under the root where the table was previously located.
         */
        unsigned long previousTableIndex;

        /**
         * The previously removed table.
         */
        QSharedPointer<Ld::TableFrameElement> previousTableElement;
};

#endif
