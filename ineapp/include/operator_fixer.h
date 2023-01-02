/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref OperatorFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef OPERATOR_FIXER_H
#define OPERATOR_FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>

#include "app_common.h"
#include "fixer.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * Class that performs fix-ups to most operator class instances.
 */
class APP_PUBLIC_API OperatorFixer:public Fixer {
    public:
        /**
         * The static \ref OperatorFixer instance.
         */
        static const OperatorFixer instance;

        OperatorFixer();

        ~OperatorFixer() override;

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
         *         merged together.  This version always returns false.
         */
        bool isMergeAllowed(
            Ld::ElementPointer leftElement,
            Ld::ElementPointer rightElement,
            bool               checkingLeft,
            bool               checkFormats = false
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
         * Method you can overload to delete a child.  You can use this to modify how delete options are performed in
         * order to deal with placeholders and other requirements, when needed.
         *
         * If the child placement is Ld::Element::ChildPlacement::FIXED or Ld::Element::ChildPlacement::GRID, then this
         * version will replace the child with a placeholder.  If the child placement is anything else, then this
         * version will simply call Ld::Element::removeChild to remove the child.
         *
         * \param[in] parent                The parent to containing the child to be deleted.
         *
         * \param[in] child                 The child to be deleted.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        void deleteChild(
            Ld::ElementPointer         parent,
            Ld::ElementPointer         child,
            Ld::CursorStateCollection* cursorStateCollection
        ) const override;

    private:
        /**
         * Method that removes default children from an element.
         *
         * \param[in] element               The table element to be processed.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        void removeDefaultChildren(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) const;

        /**
         * Method that inserts default placeholder childen.
         *
         * \param[in] element               The element to receive placeholder children.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        void insertDefaultChildren(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) const;
};

#endif
