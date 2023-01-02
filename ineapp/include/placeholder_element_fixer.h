/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlaceholderElementFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEHOLDER_ELEMENT_FIXER_H
#define PLACEHOLDER_ELEMENT_FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element_cursor.h>

#include "app_common.h"
#include "fixer.h"

namespace Ld {
    class CursorStateCollection;
    class ElementWithGridChildren;
}

/**
 * Class that fixes-up placeholder elements after specific operations.
 *
 * Note that fixer instances are expected to be created as static const instances.
 */
class APP_PUBLIC_API PlaceholderElementFixer:public Fixer {
    public:
        /**
         * Static instance of this fixer to be used by the application.
         */
        static const PlaceholderElementFixer instance;

        PlaceholderElementFixer();

        ~PlaceholderElementFixer() override;

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
};

#endif
