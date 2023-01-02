/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref VariableElementFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VARIABLE_ELEMENT_FIXER_H
#define VARIABLE_ELEMENT_FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>

#include "app_common.h"
#include "fixer.h"

namespace Ld {
    class CursorStateCollection;
    class ElementWithGridChildren;
}

/**
 * Class that fixes-up variable elements after specific operations.
 *
 * Note that fixer instances are expected to be created as static const instances.
 */
class APP_PUBLIC_API VariableElementFixer:public Fixer {
    public:
        /**
         * Static instance of this fixer to be used by the application.
         */
        static const VariableElementFixer instance;

        VariableElementFixer();

        ~VariableElementFixer() override;

        /**
         * Method you can overload to determine if it's OK to delete an element.  Note that you should query both this
         * method and the \ref Fixer::isDeleteChildAllowed method before performing a delete operation.
         *
         * \param[in] element The element to be deleted.
         *
         * \param[in] reason  The reason the delete operation is being triggered.
         *
         * \return Returns true if the element can be deleted.  Returns false if the element should not be deleted.
         *         This version will allow deletion only if the element was selected for delete.
         */
        bool isDeleteAllowed(Ld::ElementPointer element, DeleteReason reason) const override;

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
         *         merged together.  This version returns true if both elements contain at least one text region.
         *         If checkFormats and checkingLeft is true, this method will return true only if the formats also
         *         exactly match.
         */
        bool isMergeAllowed(
            Ld::ElementPointer leftElement,
            Ld::ElementPointer rightElement,
            bool               checkingLeft,
            bool               checkFormats = false
        ) const override;
};

#endif
