/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Command class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QSharedPointer>

#include <ld_element_structures.h>

#include "app_common.h"
#include "cursor.h"

namespace Ld {
    class ElementCursor;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Pure virtual base class for commands issued by the GUI to update the underlying program.  This class exists largely
 * to support the concept of a command queue with undo/redo capabilities.
 *
 * One area of surprising complexity is management of the cursors.  This class provides several mechanisms you can use
 * for this purpose.
 *
 *     * The class maintains a weak pointer to the cursor that invoked this command.  You can use this cursor when a
 *       command needs to update the cursor where the user issued to command.
 *
 *     * The class maintains a copy of the cursor state at the point that the cursor was set for the command.  You
 *       should always use this cursor to determine where a change was made as a user may have manually updated a
 *       cursor or the cursor may have been destroyed prior to an undo or redo operation making the active cursor
 *       position invalid.  Note that the command queue will manually update the current cursor for each issued command
 *       during undo/redo operations if the cursor is still defined when the undo/redo is requested.
 *
 *     * The \ref Command::execute and \ref Command::undo methods include a parameter that is a container for all the
 *       cursors and their state.  The container includes methods you can use to manage cursor updates during common
 *       operations.
 */
class APP_PUBLIC_API Command {
    public:
        /**
         * Enumerated value indicating the type of command.  There will be a derived class available for each of these
         * command types.
         */
        enum class CommandType {
            /**
             * Indicates an invalid command type value.
             */
            INVALID,

            /**
             * Value used to indicate the \ref InsertStringCommand command type.
             */
            INSERT_STRING,

            /**
             * Value used to indicate the \ref DeleteCommand command type.
             */
            DELETE_CONTENT,

            /**
             * Value used to indicate the \ref InsertElementCommand command type.
             */
            INSERT_ELEMENT,

            /**
             * Value used to indcate the \ref ModifyTableCommand command type.
             */
            MODIFY_TABLE,

            /**
             * Value used to indicate the \ref ModifyGridCommand command type.
             */
            MODIFY_GRID,

            /**
             * Value used to indicate the \ref UpdatePageFormatCommand command type.
             */
            UPDATE_PAGE_FORMAT,

            /**
             * Value used to indicate the \ref UpdateFormatCommand command type.
             */
            UPDATE_FORMAT,

            /**
             * Value used to indicate the \ref CutCopyCommand command type.
             */
            CUT_COPY,

            /**
             * Value used to indicate the \ref PasteCommand command type.
             */
            PASTE
        };

        Command();

        /**
         * Constructor
         *
         * \param[in] newCursor The cursor who's position that should be updated in response to this command.
         */
        Command(CursorPointer newCursor);

        virtual ~Command();

        /**
         * Method that returns the command type.
         *
         * \return Returns the command type value.
         */
        virtual CommandType commandType() const = 0;

        /**
         * Method that attempts to merge another command with this command.  If the merge is successful, the merged
         * command can be successfully destroyed, information will be included into this object instance.
         *
         * \param[in] commandToMerge Shared pointer to the command to be merged into this command.
         *
         * \return Returns true on success, returns false if the commands could not be merged.  The default
         *         implementation always returns false.
         */
        virtual bool merge(const CommandContainer& commandToMerge);

        /**
         * Method that executes the command.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool execute(Ld::CursorStateCollection* cursorStateCollection) = 0;

        /**
         * Method that performs an undo of the command using stored information.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool undo(Ld::CursorStateCollection* cursorStateCollection) = 0;

        /**
         * Method that performs a redo of the command using stored information.  The default implementation simply
         * ca;;s the \ref Command::execute method.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool redo(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that returns a description of the command suitable for displaying in an undo/redo stack window.
         *
         * \return Returns a description of the command.
         */
        virtual QString description() const = 0;

        /**
         * Method that returns a detailed description of the command suitable for displaying in an undo/redo stack
         * window.  This detailed description is suitable for use in fly-over text and other more verbose
         * presentations.
         *
         * \return Returns a detailed description of the command.  The default implementation returns the value
         *         reported by the \ref Command::description method.
         */
        virtual QString detailedDescription() const;

        /**
         * Method you can use to change the cursor.  This method will also update the value of the cursor at issue.
         *
         * \param[in] newCursor The cursor being used to track the element position.
         */
        void setCursor(CursorPointer newCursor);

        /**
         * Method that returns the cursor instance being used to track the current position.
         *
         * \return Returns the cursor indicating the point of insertion.  Note that a null pointer may be returned if
         *         the cursor was destroyed.
         */
        CursorPointer cursor() const;

        /**
         * Method you can use to change the value of the cursor reported when the command was issued.
         *
         * \param[in] newCursor The cursor state when the command was issued.
         */
        void setCursorAtIssue(const Cursor& newCursor);

        /**
         * Method you can call to obtain a copy of the cursor when the command was issued.
         *
         * \return Returns a copy of the cursor at the point that the cursor was set.
         */
        const Cursor& cursorAtIssue() const;

    private:
        /**
         * The cursor being used to track the position in the document.  We store as a weak pointer so that the UI can
         * destroy the cursor state.
         */
        CursorWeakPointer currentCursor;

        /**
         * The value of the cursor at the point that the cursor was set.
         */
        Cursor currentCursorAtIssue;
};

#endif
