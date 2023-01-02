/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RootElementFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ROOT_ELEMENT_FIXER_H
#define ROOT_ELEMENT_FIXER_H

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
 * Class that performs fix-ups for the root element.  This class exists to simplify other classes.
 */
class APP_PUBLIC_API RootElementFixer:public Fixer {
    public:
        /**
         * Static instance of this fixer to be used by the application.
         */
        static const RootElementFixer instance;

        RootElementFixer();

        ~RootElementFixer() override;

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
};

#endif
