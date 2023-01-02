/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFrameElementFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_ELEMENT_FIXER_H
#define TABLE_FRAME_ELEMENT_FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>

#include "app_common.h"
#include "fixer.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * Class that performs fix-ups to Ld::TableFrameElement class instaces.
 */
class APP_PUBLIC_API TableFrameElementFixer:public Fixer {
    public:
        /**
         * Static instance of this fixer to be used by the application.
         */
        static const TableFrameElementFixer instance;

        TableFrameElementFixer();

        ~TableFrameElementFixer() override;

        /**
         * Method you can overload to determine if it's OK to delete an element.  Note that you should query both this
         * method and the \ref Fixer::isDeleteChildAllowed method before performing a delete operation.
         *
         * \param[in] element The element to be deleted.
         *
         * \param[in] reason  The reason the delete operation is being triggered.
         *
         * \return Returns true if the element can be deleted.  Returns false if the element should not be deleted.
         *         This version always returns false.
         */
        bool isDeleteAllowed(Ld::ElementPointer element, DeleteReason reason) const override;

        /**
         * Method you can call to determine if two tables can be combined.
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
        bool isMergeAllowed(
            Ld::ElementPointer leftElement,
            Ld::ElementPointer rightElement,
            bool               checkingLeft,
            bool               checkFormats = false
        ) const override;

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
         *         This version returns true only if both children belong to the same group.
         */
        bool isMergeChildrenAllowed(Ld::ElementPointer firstChild, Ld::ElementPointer secondChild) const override;

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
        bool isSplitAllowed(
            Ld::ElementPointer       element,
            SplitReason              reason,
            const Ld::ElementCursor& cursor
        ) const override;

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
         *         returned if the merge can not be performed.
         */
        Ld::ElementCursor mergeElements(
            Ld::ElementPointer         leftElement,
            Ld::ElementPointer         rightElement,
            Ld::CursorStateCollection* cursorStateCollection
        ) const override;

        /**
         * Virtual that is called on parent elements just after a new child is inserted during a paste operation.
         * The fixer is not run on inserted text, paragraph, table, or page break elements that are treated as special
         * cases.
         *
         * \param[in]     parent                The parent element receiving the child.
         *
         * \param[in]     child                 The child we are about to insert.
         *
         * \param[in]     childIndex            The child index were we just inserted.
         *
         * \param[in,out] cursorStateCollection A cursor state collection holding the active cursors.
         */
        void postPasteChild(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            unsigned long              childIndex,
            Ld::CursorStateCollection* cursorStateCollection
        ) const override;

        /**
         * Virtual method that is called on newly pasted elements after they are pasted.  This method is not called
         * on elements that are merged.
         *
         * \param[in] element               The element to be fixed up.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        void postPaste(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) const override;

        /**
         * Method you can overload to perform initialization when a class instance is created.  This method is called
         * before the element is inserted into the element tree.  The default implementation does nothing.
         *
         * \param[in] element The element to be populated.
         */
        void preInsert(Ld::ElementPointer element) const override;

        /**
         * Method you can overload to split an element in two.  You only need to overload this if an element supports
         * some type of split.  Note that fixer used should be the fixer for the parent.
         *
         * This method will only be called if \ref Fixer::isSplitAllowed returns true so a failure should be considered
         * a critical error.
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
        void processSplitChildren(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         leftChild,
            Ld::ElementPointer         rightChild,
            SplitReason                reason,
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        ) const override;

        /**
         * Method you can overload to fix-up a clone of an element in a selection.  This method is triggered for the
         * fixer for each element that was copied.  This version removes empty rows and columns in the table.
         *
         * \param[in] element The element to be fixed up.
         */
        void processCopiedClone(Ld::ElementPointer element) const override;

    private:
        /**
         * Method that removes default children from a table.
         *
         * \param[in] element               The table element to be processed.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        void removeDefaultChildren(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) const;

        /**
         * Method that removes unused rows and columns from a table.
         *
         * \param[in] element               The table element to be processed.
         *
         * \param[in] cursorStateCollection An optional cursor state collection to be updated to reflect any remove
         *                                  row operations.
         */
        void removeUnusedRowsAndColumns(
            Ld::ElementPointer         element,
            Ld::CursorStateCollection* cursorStateCollection = Q_NULLPTR
        ) const;

        /**
         * Method that populates empty table cells with a default paragraph entry.
         *
         * \param[in] element               The table element to be processed.
         *
         * \param[in] cursorStateCollection An optional cursor state collection to be updated to reflect any remove
         *                                  row operations.
         */
        void populateEmptyCells(
            Ld::ElementPointer         element,
            Ld::CursorStateCollection* cursorStateCollection = Q_NULLPTR
        ) const;
};

#endif
