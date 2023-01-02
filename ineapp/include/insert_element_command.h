/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref InsertElementCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef INSERT_ELEMENT_COMMAND_H
#define INSERT_ELEMENT_COMMAND_H

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
#include "selection_data.h"
#include "command_base.h"

namespace Ld {
    class Element;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command that can be issued to insert a new element at a specified location.  If the location points to text within
 * an element, this method will attempt to split the element prior to insertion.
 *
 * The command also allows you to specify where the editor cursor should be placed after the command is executed.
 */
class APP_PUBLIC_API InsertElementCommand:public CommandBase {
    Q_DECLARE_TR_FUNCTIONS(InsertElementCommand)

    public:
        /**
         * Enumeration indicating where the insertion should be performed.
         */
        enum class InsertionLocation {
            /**
             * Indicates insertion at the cursor.  For positional elements, this will either split text or place the
             * element before the current element.  If the cursor points to an entire element and the element is a
             * placeholder element, this insertion location will overwrite the positional element.
             */
            AT_CURSOR,

            /**
             * Indicates this insert command should replace the element located at the cursor.
             */
            REPLACE_ELEMENT,

            /**
             * Indication insertion after the cursor.  This value is treated the same as
             * \ref InsertElementCommand::InsertionLocation::AT_CURSOR when used with fixed child placement.  For
             * positional elements, this will insert the element after the current element.
             */
            AFTER_CURSOR,

            /**
             * Indicates insertion should place this element directly under the root, splitting elements as needed to
             * do so.
             */
            UNDER_ROOT
        };

        /**
         * Enumeration indicating where the cursor should be positioned after insertion.
         */
        enum class FinalCursorPosition {
            /**
             * Indicates the cursor should not be adjusted.
             */
            UNCHANGED,

            /**
             * Indicates the cursor should select the entire element.
             */
            ELEMENT_WHOLE,

            /**
             * Indicates the cursor should be placed at the first text position of the element.
             */
            ELEMENT_START_OF_TEXT,

            /**
             * Indicates the cursor should be placed past the last text position of the element.
             */
            ELEMENT_END_OF_TEXT,

            /**
             * Indicates the cursor should select the first child.
             */
            FIRST_CHILD_ELEMENT,

            /**
             * Indicates the cursor should select the first text position of the first child.
             */
            FIRST_CHILD_START_OF_TEXT
        };

        InsertElementCommand();

        /**
         * Constructor
         *
         * \param[in] newElement          A shared pointer to the element to be inserted.
         *
         * \param[in] insertionLocation   The desired insertion location.
         *
         * \param[in] finalCursorPosition The final cursor position after the element insertion.
         */
        InsertElementCommand(
            Ld::ElementPointer  newElement,
            InsertionLocation   insertionLocation = InsertionLocation::AT_CURSOR,
            FinalCursorPosition finalCursorPosition = FinalCursorPosition::UNCHANGED
        );

        /**
         * Constructor
         *
         * \param[in] newElement          A shared pointer to the element to be inserted.
         *
         * \param[in] newCursor           Cursor being used to track the position in the document.  Note that the
         *                                command will update the cursor position, if needed.
         *
         * \param[in] insertionLocation   The desired insertion location.
         *
         * \param[in] finalCursorPosition The final cursor position after the element insertion.
         */
        InsertElementCommand(
            Ld::ElementPointer  newElement,
            CursorPointer       newCursor,
            InsertionLocation   insertionLocation = InsertionLocation::AT_CURSOR,
            FinalCursorPosition finalCursorPosition = FinalCursorPosition::UNCHANGED
        );

        ~InsertElementCommand() override;

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
         * \return Returns true on success, returns false if the commands could not be merged.
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
         * Method you can use to set the element to be inserted.
         *
         * \param[in] newElement The new element to be inserted.
         */
        void setElement(Ld::ElementPointer newElement);

        /**
         * Method that returns the element that is going to be inserted.
         *
         * \return Returns a shared pointer to the element to be inserted.
         */
        Ld::ElementPointer element() const;

        /**
         * Method you can use to set the desired insertion location.
         *
         * \param[in] newInsertionLocation The new insertion location setting.
         */
        void setInsertionLocation(InsertionLocation newInsertionLocation);

        /**
         * Method you can use to determine the desired insertion location.
         *
         * \return Returns the desired insertion location.
         */
        InsertionLocation insertionLocation() const;

        /**
         * Method you can use to set the final cursor position, post insertion.
         *
         * \param[in] newFinalCursorPosition The new final cursor position after the insertion.
         */
        void setFinalCursorPosition(FinalCursorPosition newFinalCursorPosition);

        /**
         * Method you can use to determine the final cursor position, post insertion.
         *
         * \return Returns the final cursor position after insertion.
         */
        FinalCursorPosition finalCursorPosition() const;

    private:
        /**
         * Enumeration of insertion modes.
         */
        enum class InsertionMode {
            /**
             * Indicates no insertion has been performed.
             */
            NO_INSERTION_PERFORMED,

            /**
             * Indicates we inserted the element into a null/empty position.
             */
            INSERT_INTO_EMPTY_POSITION,

            /**
             * Indicates we performed a split during the insertion process.
             */
            SPLIT_AND_INSERT,

            /**
             * Indicates that we performed a split at the root during the insertion process.
             */
            SPLIT_AT_ROOT_AND_INSERT,

            /**
             * Indicates we overwrote an existing element with this element.
             */
            INSERT_AT_POSITION
        };

        /**
         * Method that is called to insert an element into a null/empty position.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool insertIntoEmptyPosition(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to split an element and insert between split portions.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool splitAndInsert(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to split an element and parents and insert the new element just below the root.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool splitToRootAndInsert(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to insert an element into or before an existing position.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool insertAtPosition(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to undo inserting an element into a null/empty position.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undoInsertIntoEmptyPosition(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to undo splitting an element and inserting between split portions.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undoSplitAndInsert(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to undo splitting at the root and inserting between split portions.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undoSplitAtRootAndInsert(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that is called to undo inserting an element into or before an existing position.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undoInsertAtPosition(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that updates the final cursor position.
         */
        void updateFinalCursorPosition();

        /**
         * The element to be inserted.
         */
        Ld::ElementPointer currentElement;

        /**
         * The insertion location setting.
         */
        InsertionLocation currentInsertionLocation;

        /**
         * The final cursor position after insertion.
         */
        FinalCursorPosition currentFinalCursorPosition;

        /**
         * The insertion mode.
         */
        InsertionMode currentInsertionMode;

        /**
         * A string description of the element used after execution of this command to provide a description.
         */
        QString currentDescription;

        /**
         * A shared pointer to the content that previously occupied this position.
         */
        Ld::ElementPointer oldContents;
};

#endif
