/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Fixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FIXER_H
#define FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element_cursor.h>

#include "app_common.h"

namespace Ld {
    class CursorStateCollection;
    class ElementWithGridChildren;
}

/**
 * Base class you can overload to provide element specific engines that fix-up elements after specific operations.
 *
 * Note that fixer instances are expected to be created as static const instances.
 */
class APP_PUBLIC_API Fixer {
    public:
        /**
         * Emumeration of reasons why we think this element might need to be deleted.
         */
        enum class DeleteReason : std::uint8_t {
            /**
             * Indicates that the user selected this entire element and requested it be deleted.
             */
            ELEMENT_SELECTED_FOR_DELETE = 0,

            /**
             * Indicates that the user selected this element as part of a larger selection.
             */
            SELECTION_DELETE = 1,

            /**
             * Indicates that we are about to delete the last child under the element.
             */
            DELETING_CHILD = 2,

            /**
             * Indicates that we are about to delete all remaining text in the element.
             */
            LAST_TEXT_DELETED = 3,
        };

        /**
         * Enumeration of reasons why we think this element might need to be split.
         */
        enum class SplitReason : std::uint8_t {
            /**
             * Indicates that the element is being split because of an inserted newline.  In this scenario, the
             * second cursor is not used.  This value is also used when pasting when a paragraph break is needed.
             */
            NEWLINE = 0,

            /**
             * Indicates that an element is being inserted at the indicated cursor position.  This value is also used
             * when pasting.
             */
            INSERT_ELEMENT = 1,

            /**
             * Indicates that an element is being inserted directly under the root at the indicated cursor postion.
             * This value is used for page breaks and tables.
             */
            INSERT_ELEMENT_TOP_LEVEL = 2,

            /**
             * Indicates that the element is being split due to a format change.
             */
            FORMAT_CHANGE = 3,

            /**
             * Indicates that the element is being split due to a cut operation.  In this scenario, the second cursor
             * points to the lower/right portion of the cut.
             */
            CUT = 4
        };

        Fixer();

        virtual ~Fixer();

        /**
         * Static method you can use to register a new default child provider with the system.
         *
         * \param[in] elementName The element type name that this provider should be used with.
         *
         * \param[in] provider    The provider that should be used to manipulate the element.
         *
         * \return Returns true on success, returns false if a provider has already been registered for the
         *         indicated element.
         */
        static bool registerFixer(const QString& elementName, const Fixer& provider);

        /**
         * Static method you can use to register a new default child provider with the system.
         *
         * \param[in] elementName The element type name that this provider should be used with.
         *
         * \param[in] provider    The provider that should be used to manipulate the element.
         *
         * \return Returns true on success, returns false if a provider has already been registered for the
         *         indicated element.
         */
        static bool registerFixer(const QString& elementName, const Fixer* provider);

        /**
         * Method you can use to obtain the provider for a given element.
         *
         * \param[in] elementName The name of the element requiring the provider.
         *
         * \return Returns a pointer to the provider.  A null pointer is returned if no provider has been
         *         registered.
         */
        static const Fixer* fixer(const QString& elementName);

        /**
         * Method you can overload to determine if it's OK to delete an element.  Note that you should query both this
         * method and the \ref Fixer::isDeleteChildAllowed method before performing a delete operation.
         *
         * \param[in] element The element to be deleted.
         *
         * \param[in] reason  The reason the delete operation is being triggered.
         *
         * \return Returns true if the element can be deleted.  Returns false if the element should not be deleted.
         *         This version returns troe if the delete reason is \ref DeleteReason::LAST_TEXT_DELETED,
         *         \ref DeleteReason::ELEMENT_SELECTED_FOR_DELETE, or \ref DeleteReason::SELECTION_DELETE.
         */
        virtual bool isDeleteAllowed(Ld::ElementPointer element, DeleteReason reason) const;

        /**
         * Method you can overload to determine if it's OK to delete a a child element.  Note that you should query
         * both this method and the \ref Fixer::isDeleteAllowed method before performing a delete operation.
         *
         * \param[in] parent  The parent element to query.  This is the element associated with this fixer.
         *
         * \param[in] element The chilkd element to be deleted.
         *
         * \param[in] reason  The reason the delete operation is being triggered.
         *
         * \return Returns true if the element can be deleted.  This version always returns true.
         */
        virtual bool isDeleteChildAllowed(
            Ld::ElementPointer parent,
            Ld::ElementPointer element,
            DeleteReason       reason
        ) const;

        /**
         * Method you can call to determine if two elements can be merged to create a single aggregate element.
         *
         * This method should be called for both elements being merged using the "checkingLeft" parameter to indicate
         * the relative ordering.
         *
         * Note that you must also call the \ref Fixer::isMergeChildrenAllowed method to determine if the parent
         * element allowed these children to be merged.
         *
         * \param[in] leftElement  A shared pointer to the first element to check against.
         *
         * \param[in] rightElement A shared pointer to the second element to check against.
         *
         * \param[in] checkingLeft If true, the fixer for the left element is being invoked.  If false, the fixer for
         *                         the right element is being invoked.
         *
         * \param[in] checkFormats If true, then this method should include checks of the formats before merging.
         *
         * \return Returns true if the elements can be merged together.  Returns false if the elements can not be
         *         merged together.  The default implementation always returns false.
         */
        virtual bool isMergeAllowed(
            Ld::ElementPointer leftElement,
            Ld::ElementPointer rightElement,
            bool               checkingLeft,
            bool               checkFormats = false
        ) const;

        /**
         * Method you can call to determine if this element allows any two children to be merged.  Both children
         * must be children of this element.
         *
         * This method should only be called if the two children have a common parent and should be invoked using
         * the fixer for the parent.
         *
         * Note that you must also call the \ref Fixer::isMergeAllowed method to determine if the children also allow
         * merging.
         *
         * \param[in] firstChild   The first child we are considering merging.
         *
         * \param[in] secondChild  The second child we are considering merging.
         *
         * \return Returns true if the children can be merged.  Returns false if the children can not be merged.
         *         The default implementation always returns true.
         */
        virtual bool isMergeChildrenAllowed(Ld::ElementPointer firstChild, Ld::ElementPointer secondChild) const;

        /**
         * Method you can call to determine if you can split this element in two at a given position.  This method
         * should also check the parent.
         *
         * \param[in] element  The element we are testing.  This value should change as we traverse up the element
         *                     tree.
         *
         * \param[in] reason   The reason we are checking if a split is allowed.
         *
         * \param[in] cursor   The cursor position to be tested.
         *
         * \return Returns true if the element can be split at the cursor position.  Returns false if the element
         *         can not be split at the cursor position.  The default implementation always returns false.
         */
        virtual bool isSplitAllowed(
            Ld::ElementPointer       element,
            SplitReason              reason,
            const Ld::ElementCursor& cursor
        ) const;

        /**
         * Method you can call to merge this element with another element.  Note that you are expected to use the
         * \ref Fixer::isMergeAllowed and \ref Fixer::isMergeChildrenAllowed method to confirm that a merge can be
         * performed before calling this method.
         *
         * The fixer should be selected based on the left element.
         *
         * \param[in,out] leftElement           The left element to be merged.  This element will be modified during
         *                                      the merge operation.  If the formsts are different, the format for this
         *                                      element should be preserved.
         *
         * \param[in]     rightElement          The right element to be merged.  This element is not modified during
         *                                      the merge operation.  If needed, you are expected to also remove this
         *                                      element from its parent.
         *
         * \param[in,out] cursorStateCollection A pointer to a collection of cursor instances to be adjusted by
         *                                      this method.  No adjustments are performed if a null pointer is
         *                                      provided.  This method will assume that the "other" element will
         *                                      be removed and that cursors should be updated to point to this
         *                                      element, if needed.
         *
         * \return Returns an Ld::ElementCursor instance pointing to the newly merged location.  An invalid cursor is
         *         returned if the merge can not be performed.  This version always returns an invalid
         *         cursor.
         */
        virtual Ld::ElementCursor mergeElements(
            Ld::ElementPointer         leftElement,
            Ld::ElementPointer         rightElement,
            Ld::CursorStateCollection* cursorStateCollection
        ) const;

        /**
         * Method you can call to split this element in two at a given position.  This method should recursively call
         * the \ref Fixer::processSplitChildren on the ancestors to tie the split element back into the element tree.
         *
         * The split operation should not fail is \ref Fixer::isSplitAllowed returned true prior to calling this
         * method.  Calling this method if \ref Fixer::isSplitAllowed returns false, should be considered a fatal
         * error.
         *
         * The default implementation will assert.
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
        virtual void splitElement(
            SplitReason                reason,
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        ) const;

        /**
         * Method you can overload to perform initialization when a class instance is created.  This method is called
         * before the element is inserted into the element tree.  The default implementation does nothing.
         *
         * \param[in] element The element to be populated.
         */
        virtual void preInsert(Ld::ElementPointer element) const;

        /**
         * Method you can overload to delete a child.  You can use this to modify how delete options are performed in
         * order to deal with placeholders and other requirements, when needed.  The default implementation simply
         * calls Ld::Element::removeChild.
         *
         * \param[in] parent                The parent to containing the child to be deleted.
         *
         * \param[in] child                 The child to be deleted.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        virtual void deleteChild(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            Ld::CursorStateCollection* cursorStateCollection
        ) const;

        /**
         * Method you can overload to delete a child as part of a selection.  The method will only be invoked when
         * the parent elements are not also being deleted.  The default implementation simply calls
         * \ref Fixer::deleteChild.
         *
         * \param[in] parent                The parent to containing the child to be deleted.
         *
         * \param[in] child                 The child that we just deleted.
         *
         * \param[in] startingCursor        The starting cursor for the collection.
         *
         * \param[in] endingCursor          The ending cursor for the collection.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        virtual void deleteChildInSelection(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            const Ld::ElementCursor&   startingCursor,
            const Ld::ElementCursor&   endingCursor,
            Ld::CursorStateCollection* cursorStateCollection
        ) const;

        /**
         * Method you can overload to perform fix-up of elements prior to new row insertion.  This method is called on
         * each row just prior to the new row being inserted.  The method can also provide default content to be
         * inserted into each cell.  The default implementation does nothing.
         *
         * \param[in] element  The grid element that has been modified.
         *
         * \param[in] rowIndex The row index of the row to be added.
         *
         * \return Returns a pointer to default content to be cloned into each cell.  The default implementation
         *         returns a null pointer.
         */
        virtual Ld::ElementPointer preInsertRow(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               rowIndex
        ) const;

        /**
         * Method you can overload to perform fix-up of elements after a new row has been inserted.  This method is
         * called on the element immediately after each insertion operation.  The default implementation does
         * nothing.
         *
         * \param[in] element  The grid element that has been modified.
         *
         * \param[in] rowIndex The row index of the newly added row.
         */
        virtual void postInsertRow(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               rowIndex
        ) const;

        /**
         * Method you can overload to perform fix-up of elements prior to new column insertion.  This method is called
         * on each column just prior to the new column being inserted.  The method can also provide default content to
         * be inserted into each cell.  The default implementation does nothing.
         *
         * \param[in] element  The grid element that has been modified.
         *
         * \param[in] columnIndex The column index of the column to be added.
         *
         * \return Returns a pointer to default content to be cloned into each cell.  The default implementation
         *         returns a null pointer.
         */
        virtual Ld::ElementPointer preInsertColumn(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               columnIndex
        ) const;

        /**
         * Method you can overload to perform fix-up of elements after a new column has been inserted.  This method is
         * called on the element immediately after each insertion operation.  The default implementation does
         * nothing.
         *
         * \param[in] element  The grid element that has been modified.
         *
         * \param[in] columnIndex The column index of the newly added column.
         */
        virtual void postInsertColumn(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               columnIndex
        ) const;

        /**
         * Method you can overload to perform fix-up of elements prior to removing a row.  The default implementation
         * does nothing.
         *
         * \param[in] element  The grid element that has been modified.
         *
         * \param[in] rowIndex The row index of the row to be removed.
         */
        virtual void preRemoveRow(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               rowIndex
        ) const;

        /**
         * Method you can overload to perform fix-up of elements prior to removing a column.  The default
         * implementation does nothing.
         *
         * \param[in] element     The grid element that has been modified.
         *
         * \param[in] columnIndex The column index of the column to be removed.
         */
        virtual void preRemoveColumn(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               columnIndex
        ) const;

        /**
         * Method you can overload to perform fix-up of elements after a format change.  The default implementation
         * does nothing.
         *
         * \param[in] element               The element being modified.
         *
         * \param[in] oldFormat             The old format for the element.
         *
         * \param[in] newFormat             The new format for the element.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        virtual void postFormatChange(
            Ld::ElementPointer         element,
            Ld::FormatPointer          oldFormat,
            Ld::FormatPointer          newFormat,
            Ld::CursorStateCollection* cursorStateCollection
        ) const;

        /**
         * Method you can overload to split an element in two.  You only need to overload this if an element supports
         * some type of split.  Note that fixer used should be the fixer for the parent.
         *
         * This method will only be called if \ref Fixer::isSplitAllowed returns true so a failure should be considered
         * a critical error.
         *
         * The default implementation will assert.
         *
         * \param[in,out] parent                The parent element to contain the children.
         *
         * \param[in,out] leftChild             The existing, modified, left child.
         *
         * \param[in,out] rightChild            The newly created right child.
         *
         * \param[in]     reason                The reason for the split to be performed.
         *
         * \param[in]     cursor                The cursor location where the split is to be performed.
         *
         * \param[in,out] cursorStateCollection The cursor state collection containing cursors to be updated by the
         *                                      split.  Note that normally \ref Fixer::splitElement should update the
         *                                      cursor state collection.  This parameter will rarely be used.
         */
        virtual void processSplitChildren(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         leftChild,
            Ld::ElementPointer         rightChild,
            SplitReason                reason,
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        ) const;

        /**
         * Method you can overload to fix-up a clone of an element in a selection.  This method is triggered for the
         * fixer for each element that was copied.  The default implementation does nothing.
         *
         * \param[in] element The element to be fixed up.
         */
        virtual void processCopiedClone(Ld::ElementPointer element) const;

        /**
         * Virtual method that is called on newly pasted elements after they are pasted.  The fixer is not run on
         * inserted text, paragraph, table, or page break elements that are treated as special cases.
         *
         * The default implementation does nothing.
         *
         * \param[in] element               The element to be fixed up.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        virtual void postPaste(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) const;

        /**
         * Virtual that is called on parent elements just after a new child is inserted during a paste operation.
         * The fixer is not run on inserted text, paragraph, table, or page break elements that are treated as special
         * cases.
         *
         * The default implementation does nothing.
         *
         * \param[in]     parent                The parent element receiving the child.
         *
         * \param[in]     child                 The child we are about to insert.
         *
         * \param[in]     childIndex            The child index were we just inserted.
         *
         * \param[in,out] cursorStateCollection A cursor state collection holding the active cursors.
         */
        virtual void postPasteChild(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            unsigned long              childIndex,
            Ld::CursorStateCollection* cursorStateCollection
        ) const;

    protected:
        /**
         * Convenience method you can use to check if a parent element allowed a split.
         *
         * \param[in] element The element we want to test the parent of.
         *
         * \param[in] reason  The reason we are checking if a split is allowed.
         *
         * \param[in] cursor  The cursor position to be tested.
         *
         * \return Returns true if the element can be split at the cursor position.  Returns false if the element
         *         can not be split at the cursor position.  This version returns the result of
         *         \ref Fixer::isSplitAllowed reported by the parent of the element.  A false value is returned if
         *         element is null and a true value is returned if the parent of the element is null.
         */
        static bool isSplitAllowedByParent(
            Ld::ElementPointer       element,
            SplitReason              reason,
            const Ld::ElementCursor& cursor
        );

        /**
         * Method that creates a default paragraph instance.
         *
         * \return Returns a default paragraph instance.
         */
        static Ld::ElementPointer createDefaultParagraph();

        /**
         * Method that determines if a child is a default paragraph instance.
         *
         * \param[in] element The element to be tested.
         *
         * \return Returns true if this element is a default paragraph, returns false if this element is not a default
         *         paragraph.
         */
        static bool isDefaultParagraph(Ld::ElementPointer element);

    private:
        /**
         * Hash of default child providers by element.
         */
        static QHash<QString, const Fixer*> providers;
};

#endif
