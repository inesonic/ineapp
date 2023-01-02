/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref GridOperatorFixer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef GRID_OPERATOR_FIXER_H
#define GRID_OPERATOR_FIXER_H

#include <QString>
#include <QHash>

#include <ld_element_structures.h>

#include "app_common.h"
#include "operator_fixer.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * Class that performs fix-ups to most operator class instances that rely on grid elements.
 */
class APP_PUBLIC_API GridOperatorFixer:public OperatorFixer {
    public:
        /**
         * Static instance of the fixer to be used by the application.
         */
        static const GridOperatorFixer instance;

        GridOperatorFixer();

        ~GridOperatorFixer() override;

        /**
         * Method you can overload to perform fix-up of elements prior to new row insertion.  This method is called on
         * each row just prior to the new row being inserted.  The method can also provide default content to be
         * inserted into each cell.
         *
         * \param[in] element  The grid element that has been modified.
         *
         * \param[in] rowIndex The row index of the row to be added.
         *
         * \return Returns a pointer to default content to be cloned into each cell. This implementation returns a
         *         placeholder element.
         */
        Ld::ElementPointer preInsertRow(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               rowIndex
        ) const override;

        /**
         * Method you can overload to perform fix-up of elements prior to new column insertion.  This method is called
         * on each column just prior to the new column being inserted.  The method can also provide default content to
         * be inserted into each cell.
         *
         * \param[in] element  The grid element that has been modified.
         *
         * \param[in] columnIndex The column index of the column to be added.
         *
         * \return Returns a pointer to default content to be cloned into each cell.  This implementation returns a
         *         placeholder element.
         */
        Ld::ElementPointer preInsertColumn(
            QSharedPointer<Ld::ElementWithGridChildren> element,
            unsigned long                               columnIndex
        ) const override;

        /**
         * Method you can overload to fix-up a clone of an element in a selection.  This method is triggered for the
         * fixer for each element that was copied.  This version removes empty rows and columns in the grid.
         *
         * \param[in] element The element to be fixed up.
         */
        void processCopiedClone(Ld::ElementPointer element) const override;
};

#endif
