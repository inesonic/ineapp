/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref EditHelpers class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef EDIT_HELPERS_H
#define EDIT_HELPERS_H

#include <QString>
#include <QSharedPointer>

#include <ld_element_structures.h>

#include "app_common.h"
#include "cursor.h"
#include "fixer.h"

namespace Ld {
    class ElementCursor;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Class that provides useful functions for editing content.
 */
class APP_PUBLIC_API EditHelpers {
    public:
        /**
         * Enumeration indicating how a cursor position should be interpreted after deleting elements.
         */
        enum class DeleteLocation {
            /**
             * Indicates an invalid delete location.
             */
            INVALID,

            /**
             * Indicates the delete location represents the location where elements were merged together after a
             * delete.
             */
            EXACT_DELETE_LOCATION,

            /**
             * Indicates the cursor points to the element after the delete location.
             */
            AFTER_DELETE_LOCATION,

            /**
             * Indicates the cursor points to the element before the delete location.
             */
            BEFORE_DELETE_LOCATION,

            /**
             * Indicates the cursor points to the parent that held the deleted entry as the only child.
             */
            PARENT_OF_DELETE_LOCATION
        };

        EditHelpers();

        ~EditHelpers();

        /**
         * Method that determines if a cursor is at the start of a paragraph.
         *
         * \param[in] cursor The cursor to be tested.
         *
         * \return Returns true if the cursor is at the first text position in a paragraph.  Returns false if the
         *         cursor is anywhere else.
         */
        static bool atStartOfParagraph(const Ld::ElementCursor& cursor);

        /**
         * Method that determines if a cursor is at the end of a paragraph.  Note that this method differs from
         * \ref DeleteCommand::atStartOfParagraph in that being parked on a whole element at the end of a paragraph
         * does not count as being at the end of the paragraph.
         *
         * \param[in] cursor The cursor to be tested.
         *
         * \return Returns true if the cursor is at the last text position in a paragraph.  Returns false if the cursor
         *         is anywhere else.
         */
        static bool atEndOfParagraph(const Ld::ElementCursor& cursor);

        /**
         * Method that determines if an element can be deleted.
         *
         * \param[in] element      The element to be tested.
         *
         * \param[in] deleteReason The reason we are deleting this element.
         *
         * \return Returns true if the element can be removed.  Returns false if the element should not be removed.
         */
        static bool isDeleteAllowed(Ld::ElementPointer element, Fixer::DeleteReason deleteReason);

        /**
         * Method you can use to determine if two elements can be merged together.  This method checks for permission
         * from both the elements and, if the elements share a common parent, their parent.
         *
         * \param[in] leftElement  The left element to be checked.
         *
         * \param[in] rightElement The right element to be checked.
         *
         * \param[in] checkFormats If true, the format should also be checked.  If false, the format should not be
         *                         checked.
         *
         * \param[in] checkParent  If true, the parent is also queried.
         *
         * \return Returns true if the two elements can be merged.  Returns false if the two elements can-not be
         *         merged.
         */
        static bool isMergeAllowed(
            Ld::ElementPointer leftElement,
            Ld::ElementPointer rightElement,
            bool               checkFormats = false,
            bool               checkParent  = true
        );

        /**
         * Method you can use to determine if an element can be split.  This method checks for permission from both
         * the element and the parent.
         *
         * \param[in] reason The reason for the split.
         *
         * \param[in] cursor The cursor indicating the element being split as well as the position within the element.
         *
         * \return Returns true if the requested element can be split.  Returns false if the requested element can-not
         *         be split.
         */
        static bool isSplitAllowed(Fixer::SplitReason reason, const Ld::ElementCursor& cursor);

        /**
         * Method you can use to merge two elements.  This method assumes that \ref EditHelpers::isMergeAllowed has
         * already been called and returned a value of true.
         *
         * \param[in,out] leftElement           The left element to be merged.
         *
         * \param[in]     rightElement          The right element to be checked.
         *
         * \param[in,out] cursorStateCollection An optional cursor state collection to be updated to reflect the
         *                                      new, merged, positions in the left element.
         *
         * \return Returns the cursor position representing the position of the merge in the left element.
         */
        static Ld::ElementCursor mergeElements(
            Ld::ElementPointer         leftElement,
            Ld::ElementPointer         rightElement,
            Ld::CursorStateCollection* cursorStateCollection = Q_NULLPTR
        );

        /**
         * Method you can use to split an element in two.  The method assumes that \ref CommandBase::isSplitAllowed
         * has already been called and returned true.
         *
         * \param[in]     reason                The reason for the split.  This value determines how the split should
         *                                      be handled.
         *
         * \param[in]     cursor                The cursor position where the split should be performed.
         *
         * \param[in,out] cursorStateCollection A pointer to a collection of cursor instances to be adjusted by
         *                                      this method.  No adjustments are performed if a null pointer is
         *                                      provided.  This method will prefer the left side of the split if
         *                                      a cursor lands at the split location.
         */
        static void splitElement(
            Fixer::SplitReason         reason,
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method you can use to split an element in two and insert a new element.  The method will check if a split
         * is allowed and return false on error.
         *
         * \param[in]     reason                The reason for the split.  This value determines how the split should
         *                                      be handled.
         *
         * \param[in]     cursor                The cursor position where the split should be performed.
         *
         * \param[in]     elementToInsert       The element to be inserted at the cursor position.
         *
         * \param[in,out] cursorStateCollection A pointer to a collection of cursor instances to be adjusted by
         *                                      this method.  No adjustments are performed if a null pointer is
         *                                      provided.  This method will prefer the left side of the split if
         *                                      a cursor lands at the split location.
         *
         * \return Returns true on success, returns false on error.
         */
        static bool splitElementAndInsert(
            Fixer::SplitReason         reason,
            const Ld::ElementCursor&   cursor,
            Ld::ElementPointer         elementToInsert,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method you can use to remove a top level element and merge children.
         *
         * \param[in] cursor                The cursor pointing to the left side of the deepest element to merge.
         *
         * \param[in] cursorStateCollection An optional cursor state collection used to
         *
         * \return Returns a pointer to the removed element.
         */
        static Ld::ElementPointer removeTopLevelElementAndMerge(
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method that can be called by derived classes to merge the element pointed to by a cursor's parent with
         * its next sibling.
         *
         * \param[in]     cursor                The cursor where the split should be performed.  The cursor is
         *                                      expected to be positioned just before the split.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        static bool mergeAcrossNextParent(
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method you can use to delete an element and, if allowed by adjascent elements, merge the asjacent elements.
         *
         * \param[in] element               The element to be removed.
         *
         * \param[in] cursorStateCollection The cursor state collection holding the active cursors.
         *
         * \return Returns the position where the element was removed.  This will be one of:
         *           * Last position in the previous sibling element (if the removed element was not the first child).
         *           * The first position in the next sibling (if removed element was the first child).
         *           * The parent element (if the removed element was the only child).
         *           * The empty position created by the removed element (if the parent does not support positional or
         *             grouped placement).
         */
        static Ld::ElementCursor removeElementAndMerge(
            Ld::ElementPointer         element,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method you can use to delete a child element using the \ref Fixer classes.  If no fixer is available then
         * this method will either call Ld::Element::removeChild directly or will assert if the placement rules for
         * the parent are Ld::Element::ChildPlacement::FIXED or Ld::Element::ChildPlacement::GRID.
         *
         * \param[in] parent                The parent to containing the child to be deleted.
         *
         * \param[in] child                 The child that we want to delete.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        static void deleteElement(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method you can use to delete a child element using the \ref Fixer classes.
         *
         * \param[in] parent                The parent to containing the child to be deleted.
         *
         * \param[in] child                 The child that we want to delete.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        static void deleteElementInSelection(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            Ld::CursorStateCollection* cursorStateCollection
        );

    private:
        /**
         * Method that checks if an element in question has any children.
         *
         * \param[in] element       The element to be tested.
         *
         * \param[in] childToIgnore An optional element to be ignored during the test.
         *
         * \return Returns true if the element has no children.
         */
        bool hasNoOtherChildren(Ld::ElementPointer element, Ld::ElementPointer childToIgnore = Ld::ElementPointer());
};

#endif
