/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PasteCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PASTE_COMMAND_H
#define PASTE_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QSharedPointer>
#include <QString>

#include <ld_element_cursor.h>
#include <ld_element.h>

#include "app_common.h"
#include "selection_data.h"
#include "cursor.h"
#include "command_base_with_undo.h"

namespace Ld {
    class CursorStateCollection;
}

class Clipboard;
class CommandContainer;

/**
 * Command that can be issued to perform a paste operation from the clipboard.
 */
class APP_PUBLIC_API PasteCommand:public CommandBaseWithUndo {
    Q_DECLARE_TR_FUNCTIONS(PasteCommand)

    public:
        /**
         * Constructor
         *
         * \param[in] clipboard Pointer to the clipboard to be used for this operation.  The clipboard is expected to
         *                      remain valid during the entire lifetime of this object.
         */
        PasteCommand(Clipboard* clipboard = nullptr);

        /**
         * Constructor
         *
         * \param[in] newCursor Cursor being used to track the position in the document.  Note that the command will
         *                      update the cursor, if needed.
         *
         * \param[in] clipboard Pointer to the clipboard to be used for this operation.  The clipboard is expected to
         *                      remain valid during the entire lifetime of this object.
         */
        PasteCommand(QSharedPointer<Cursor> newCursor, Clipboard* clipboard = nullptr);

        ~PasteCommand() override;

        /**
         * Method that returns the command type.
         *
         * \return Returns the command type value.
         */
        CommandType commandType() const final;

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
         * The selection data to be pasted.  This data is captured when this class is instantiated.
         */
        SelectionDataPointer capturedSelectionData;
};

#endif
