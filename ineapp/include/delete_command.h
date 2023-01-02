/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DeleteCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DELETE_COMMAND_H
#define DELETE_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include <cstdint>

#include "app_common.h"
#include "cursor.h"
#include "command_base_with_undo.h"

namespace Ld {
    class ElementPosition;
    class ElementCursor;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command that can be issued to delete one or more characters, element(s), or a selection.  This class primarily acts
 * as a container for information used to perform the delete operation.
 *
 *     * Holding information required by the command.
 *     * Holding information needed to undo the delete operation.
 *     * Merging this command with other commands.
 */
class APP_PUBLIC_API DeleteCommand:public CommandBaseWithUndo {
    Q_DECLARE_TR_FUNCTIONS(DeleteCommand)

    public:
        /**
         * Type used to represent unicode character values.
         */
        typedef std::uint16_t UnicodeValue;

        /**
         * Constructor
         *
         * \param[in] deleteBehindCursor If true, the delete operation should target the text or element before the
         *                               current cursor position.  If false, the delete operation should target the
         *                               text or element after the cursor position.  This value is ignored for
         *                               selections.
         */
        DeleteCommand(bool deleteBehindCursor);

        /**
         * Constructor
         *
         *
         * \param[in] deleteBehindCursor If true, the delete operation should target the text or element before the
         *                               current cursor position.  If false, the delete operation should target the
         *                               text or element after the cursor position.  This value is ignored for
         *                               selections.
         *
         * \param[in] newCursor          Cursor being used to track the position in the document.  Note that the
         *                               command will update the cursor position, if needed.
         */
        DeleteCommand(bool deleteBehindCursor, CursorPointer newCursor);

        ~DeleteCommand() override;

        /**
         * Method that returns the command type.
         *
         * \return Returns the command type value.
         */
        CommandType commandType() const final;

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
         * Method you can use to indicate how the delete should be updated by this operation.
         *
         * \param[in] deleteBehindCursor If true, the delete operation should target the text or element before the
         *                               current cursor position.  If false, the delete operation should target the
         *                               text or element after the cursor position.  This value is ignored for
         *                               selections.
         */
        void setDeleteMode(bool deleteBehindCursor);

        /**
         * Method you can use to determine the requested delete mode.
         *
         * \return If true, the delete operation should target the text or element before the current cursor
         *         position.  If false, the delete operation should target the text or element after the cursor
         *         position.  This value is ignored for selections.
         */
        bool deleteMode() const;

    private:
        /**
         * The maximum number of deleted characters per merged undo command.
         */
        static constexpr unsigned maximumDeleteCount = 32;

        /**
         * Method that is called to delete a selection.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool deleteSelection(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to delete a single element.
         *
         * \param[in]     deletePosition        The position of the element to be deleted.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool deleteElement(
            const Ld::ElementPosition& deletePosition,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method that is called to delete text within a text region.  This method may also trigger text regions to be
         * merged.
         *
         * \param[in]     deleteCursor          The cursor pointing to the element to be deleted.
         *
         * \param[in]     saveState             If true, this method should save the undo state.  If false, the undo
         *                                      state will not be saved.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool deleteText(
            const Ld::ElementCursor&   deleteCursor,
            bool                       saveState,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method that is called to merge paragraphs.
         *
         * \param[in]     deleteCursor          The cursor pointing to the element to be deleted.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool mergeParagraphs(
            const Ld::ElementCursor&   deleteCursor,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Enumeration of delete modes.
         */
        enum class DeleteType : std::uint8_t {
            /**
             * Indicates we don't know the current delete mode.
             */
            UNKNOWN,

            /**
             * Indicates we're deleting a selection.
             */
            SELECTION,

            /**
             * Indicates we're deleting a character.
             */
            TEXT,

            /**
             * Indicates we're deleting an element.
             */
            ELEMENT,

            /**
             * Indicates that we're merging two paragraphs.
             */
            MERGE_PARAGRAPHS
        };

        /**
         * Value indicating the current delete mode.
         */
        DeleteType currentDeleteType;

        /**
         * Boolean value indicating how the delete operation is expected to be performed.
         */
        bool currentDeleteBehindCursor;

        /**
         * Value indicating the number of characters to delete.  The value is set during a merge to allow undo and
         * redo to remain symmetrical.  The value is set to 1 in the constructor.
         */
        unsigned currentDeleteCount;
};


#endif
