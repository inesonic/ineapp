/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CutCopyCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CUT_COPY_COMMAND_H
#define CUT_COPY_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QSharedPointer>
#include <QString>

#include <ld_element_cursor.h>
#include <ld_element.h>

#include "app_common.h"
#include "cursor.h"
#include "selection_data.h"
#include "command_base_with_undo.h"

namespace Ld {
    class CursorStateCollection;
}

class Clipboard;
class CommandContainer;

/**
 * Command that can be issued to perform a cut or copy operation to a provided clipboard.
 */
class APP_PUBLIC_API CutCopyCommand:public CommandBaseWithUndo {
    Q_DECLARE_TR_FUNCTIONS(CutCopyCommand)

    public:
        /**
         * Enumeration of supported modes.
         */
        enum class Mode {
            /**
             * Indicates cut mode.
             */
            CUT,

            /**
             * Indicates copy mode.
             */
            COPY
        };

        /**
         * Constructor
         *
         * \param[in] mode      The requested mode.
         *
         * \param[in] clipboard Pointer to the clipboard to be used for this operation.  The clipboard is expected to
         *                      remain valid during the entire lifetime of this object.
         */
        CutCopyCommand(Mode mode = Mode::CUT, Clipboard* clipboard = nullptr);

        /**
         * Constructor
         *
         * \param[in] newCursor Cursor being used to track the position in the document.  Note that the command will
         *                      update the cursor, if needed.
         *
         * \param[in] mode      The requested mode.
         *
         * \param[in] clipboard Pointer to the clipboard to be used for this operation.  The clipboard is expected to
         *                      remain valid during the entire lifetime of this object.
         */
        CutCopyCommand(CursorPointer newCursor, Mode mode = Mode::CUT, Clipboard* clipboard = nullptr);

        ~CutCopyCommand() override;

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

        /**
         * Method you can use to set the requested command mode.
         *
         * \param[in] newMode The new command mode.
         */
        void setMode(Mode newMode);

        /**
         * Method you can use to obtain the requested command mode.
         *
         * \return Returns the requested command mode.
         */
        Mode mode() const;

        /**
         * Method you can use to set the clipboard.
         *
         * \param[in] newClipboard The clipboard to be used for the requested operation.  The clipboard is expected to
         *                         live for the entire lifetime of this object.
         */
        void setClipboard(Clipboard* newClipboard);

        /**
         * Method you can use to obtain the clipboard being used for this operation.
         *
         * \return Returns a pointer to the requested clipboard.
         */
        Clipboard* clipboard() const;

    private:
        /**
         * The operation mode.
         */
        Mode currentMode;

        /**
         * The clipboard being used for the operation.
         */
        Clipboard* currentClipboard;

        /**
         * The captured selection data.
         */
        SelectionDataPointer currentSelectionData;
};

#endif
