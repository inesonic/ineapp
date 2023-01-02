/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TextElementFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TEXT_ELEMENT_FIXER_H
#define TEXT_ELEMENT_FIXER_H

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
 * Class that performs fix-ups to text elements after common operations.
 */
class APP_PUBLIC_API TextElementFixer:public Fixer {
    public:
        /**
         * Static instance of this fixer to be used by the application.
         */
        static const TextElementFixer instance;

        TextElementFixer();

        ~TextElementFixer() override;

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
         * \param[in] element The element we are testing.  This value should change as we traverse up the element
         *                    tree.
         *
         * \param[in] reason  The reason we are checking if a split is allowed.
         *
         * \param[in] cursor  The cursor position to be tested.
         *
         * \return Returns true if the element can be split at the cursor position.  Returns false if the element
         *         can not be split at the cursor position.  This version will return the status reported by the
         *         parent.
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
         * \return Returns an Ld::ElementCursor instance pointing to the newly merged location.  An invalid cursor
         *         is returned if the merge can not be performed.
         */
        Ld::ElementCursor mergeElements(
            Ld::ElementPointer         leftElement,
            Ld::ElementPointer         rightElement,
            Ld::CursorStateCollection* cursorStateCollection
        ) const override;

        /**
         * Method you can call to split this element in two at a given position.  This method should recursively call
         * the \ref Fixer::processSplitChildren on the ancestors to tie the split element back into the element tree.
         *
         * The split operation should not fail is \ref Fixer::isSplitAllowed returned true prior to calling this
         * method.  Calling this method if \ref Fixer::isSplitAllowed returns false, should be considered a fatal
         * error.
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
        void splitElement(
            SplitReason                reason,
            const Ld::ElementCursor&   cursor,
            Ld::CursorStateCollection* cursorStateCollection
        ) const override;
};

#endif
