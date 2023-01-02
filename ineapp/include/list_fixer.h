/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ListFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LIST_FIXER_H
#define LIST_FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>

#include "app_common.h"
#include "operator_fixer.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * Class that performs fix-ups to most list class instances, including sets and arrays.
 */
class APP_PUBLIC_API ListFixer:public OperatorFixer {
    public:
        /**
         * Static instance of this fixer to be used by the application.
         */
        static const ListFixer instance;

        ListFixer();

        ~ListFixer() override;

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
         * version will either call Ld::Element::removeChild to remove the child or will replace the child with a
         * single placeholder element if the removed child is the only element.  The method will guarantee that the
         * list of children always ends with a placeholder.
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
         * Method that removes any list placeholder from the element.
         *
         * \param[in] element               The table element to be processed.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        void removeListPlaceholder(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) const;

        /**
         * Method that conditionally insers a list placeholder into the element.
         *
         * \param[in] element
         *
         * \param[in] cursorStateCollection
         */
        void insertListPlaceholder(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) const;
};

#endif
