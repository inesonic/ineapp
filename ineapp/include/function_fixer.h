/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FunctionFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FUNCTION_FIXER_H
#define FUNCTION_FIXER_H

#include <QApplication>
#include <QString>
#include <QHash>

#include <ld_element_structures.h>

#include "app_common.h"
#include "operator_fixer.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * Class that performs fix-ups to function class instances.
 */
class APP_PUBLIC_API FunctionFixer:public OperatorFixer {
    Q_DECLARE_TR_FUNCTIONS(Ld::FunctionFixer)

    public:
        /**
         * The static \ref FunctionFixer instance used by the application.
         */
        static const FunctionFixer instance;

        FunctionFixer();

        ~FunctionFixer() override;

        /**
         * Method you can overload to determine if it's OK to delete an element.  Note that you should query both this
         * method and the \ref Fixer::isDeleteChildAllowed method before performing a delete operation.
         *
         * \param[in] element The element to be deleted.
         *
         * \param[in] reason  The reason the delete operation is being triggered.
         *
         * \return Returns true if the element can be deleted.  Returns false if the element should not be deleted.
         *         This version returns true if the delete reason is \ref DeleteReason::ELEMENT_SELECTED_FOR_DELETE
         *         and \ref DeleteReason::SELECTION_DELETE and false for all other scenarios.
         */
        bool isDeleteAllowed(Ld::ElementPointer element, DeleteReason reason) const override;

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
         * This version will either call Ld::Element::removeChild to remove the child or will replace the child with
         * a single placeholder element if the removed child is the only element.  The method will guarantee that the
         * list of children always ends with an empty function placeholder.
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
        void postFormatChange(
            Ld::ElementPointer         element,
            Ld::FormatPointer          oldFormat,
            Ld::FormatPointer          newFormat,
            Ld::CursorStateCollection* cursorStateCollection
        ) const override;

    private:
        /**
         * Method that removes any function placeholder from the element.
         *
         * \param[in] element               The function element to be processed.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        static void removeFunctionPlaceholders(
            Ld::ElementPointer         element,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method that conditionally insers a function placeholder into the element.
         *
         * \param[in] element               A function element to be processed.
         *
         * \param[in] cursorStateCollection A cursor state collection holding the active cursors.
         */
        static void insertFunctionPlaceholders(
            Ld::ElementPointer         element,
            Ld::CursorStateCollection* cursorStateCollection
        );

        /**
         * Method that determines the minimum number of allowed children.
         *
         * \param[in] element A function element to be processed.
         *
         * \return Returns the minimum number of allowed children.
         */
        static unsigned minimumNumberAllowedChildren(Ld::ElementPointer element);
};

#endif
