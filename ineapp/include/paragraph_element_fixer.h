/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ParagraphElementFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PARAGRAPH_ELEMENT_FIXER_H
#define PARAGRAPH_ELEMENT_FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>

#include "app_common.h"
#include "fixer.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * Class that performs fix-ups to paragraphs after common operations.
 */
class APP_PUBLIC_API ParagraphElementFixer:public Fixer {
    public:
        /**
         * Static instance of this fixer to be used by the application.
         */
        static const ParagraphElementFixer instance;

        ParagraphElementFixer();

        ~ParagraphElementFixer() override;

        /**
         * Method you can overload to determine if it's OK to delete an element.  Note that you should query both this
         * method and the \ref Fixer::isDeleteChildAllowed method before performing a delete operation.
         *
         * \param[in] element The element to be deleted.
         *
         * \param[in] reason  The reason the delete operation is being triggered.
         *
         * \return Returns true if the element can be deleted.  Returns true unless this paragraph is directly under
         *         the root and is the only child of the root element.
         */
        bool isDeleteAllowed(Ld::ElementPointer element, DeleteReason reason) const override;

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
         * \return Returns true if the element can be deleted.
         */
        bool isDeleteChildAllowed(
            Ld::ElementPointer parent,
            Ld::ElementPointer element,
            DeleteReason       reason
        ) const override;

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
         *         merged together.  This version always returns true.
         */
        bool isMergeAllowed(
            Ld::ElementPointer leftElement,
            Ld::ElementPointer rightElement,
            bool               checkingLeft,
            bool               checkFormats = false
        ) const override;

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
         * Method you can overload to perform initialization when a class instance is created.  This method is called
         * before the element is inserted into the element tree.
         *
         * If the element supports fixed child placement, all null children will be replaced with placeholder elements.
         * If the element supports positional placement, a single placeholder will be inserted if there are no
         * children.
         *
         * All other child placement methods will cause this method to assert.
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

    private:
        /**
         * Method that identifies a reasonable format to use for a text element.
         *
         * \param[in] paragraphElement   The paragraph element to scan from.
         *
         * \param[in] scanBackwardsFirst If true, we should scan backwards first.
         *
         * \param[in] removedElement     The optional removed element.
         *
         * \return Returns a pointer to a format that can be cloned for a replacement text element.
         */
        static Ld::FormatPointer selectTextFormat(
            Ld::ElementPointer paragraphElement,
            bool               scanBackwardsFirst,
            Ld::ElementPointer removedElement = Ld::ElementPointer()
        );

        /**
         * Method you can overload to perform fix-up of elements after a child has been deleted.  This method is called
         * on the parent element just after a child is removed.
         *
         * This method will check the parent to determine if the parent has any null children.
         *
         * If the parent supports fixed child placement, all null children will be replaced with placeholders.  If the
         * parent supports positional placement and the parent is empty, this method will insert a single placeholder
         * element.
         *
         * \param[in] parent                The parent to containing the child to be deleted.
         *
         * \param[in] child                 The child that we just deleted.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        static void fixupAfterContentsChanged(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            Ld::CursorStateCollection* cursorStateCollection
        );
};

#endif
