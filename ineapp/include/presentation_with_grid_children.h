/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationWithGridChildren class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_WITH_GRID_CHILDREN_H
#define PRESENTATION_WITH_GRID_CHILDREN_H

#include <QObject>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_grid_children.h>

#include "app_common.h"
#include "presentation_with_fixed_children.h"

/**
 * This is base class used to present language elements.  The class extends the Ld::VisualWithGridChildren class to
 * bring the grid visuals into the Qt QObject and signal/slot framework and to include additional methods used to\
 * support positioning and rendering of components.
 */
class APP_PUBLIC_API PresentationWithGridChildren:public         PresentationWithFixedChildren,
                                                  public virtual Ld::VisualWithGridChildren {
    Q_OBJECT

    public:
        PresentationWithGridChildren();

        ~PresentationWithGridChildren() override;

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::GROUPED.
         */
        Ld::Element::ChildPlacement childPlacement() const override;

    signals:
        /**
         * Signal that is emitted after the grid dimensions are changed.
         *
         * \param[out] oldNumberRows    The old number of rows.
         *
         * \param[out] oldNumberColumns The old number of columns.
         *
         * \param[out] newNumberRows    The new number of rows.
         *
         * \param[out] newNumberColumns The new number of columns.
         */
        void geometryWasChanged(
            unsigned long oldNumberRows,
            unsigned long oldNumberColumns,
            unsigned long newNumberRows,
            unsigned long newNumberColumns
        );

        /**
         * Signal that is emitted just before a row is removed.
         *
         * \param[out] rowIndex The row index of the row that is about to be removed.
         */
        void aboutToRemoveRowCalled(unsigned long rowIndex);

        /**
         * Signal that is emitted just before a column is removed.
         *
         * \param[out] columnIndex The column index of the column that is about to be removed.
         */
        void aboutToRemoveColumnCalled(unsigned long columnIndex);

        /**
         * Signal that is emitted after a new row is inserted.
         *
         * \param[out] rowIndex The row index of the newly inserted row.
         */
        void rowWasInserted(unsigned long rowIndex);

        /**
         * Signal that is emitted after a new column is inserted.
         *
         * \param[out] columnIndex The column index of the newly inserted column.
         */
        void columnWasInserted(unsigned long columnIndex);

    protected slots:
        /**
         * Slot that is triggered after the grid dimensions are changed.
         *
         * \param[in] oldNumberRows    The old number of rows.
         *
         * \param[in] oldNumberColumns The old number of columns.
         *
         * \param[in] newNumberRows    The new number of rows.
         *
         * \param[in] newNumberColumns The new number of columns.
         */
        virtual void processGeometryChanged(
            unsigned long oldNumberRows,
            unsigned long oldNumberColumns,
            unsigned long newNumberRows,
            unsigned long newNumberColumns
        );

        /**
         * Slot that is triggered just before a row is removed.
         *
         * \param[in] rowIndex The row index of the row that is about to be removed.
         */
        virtual void processAboutToRemoveRow(unsigned long rowIndex);

        /**
         * Slot that is triggered just before a column is removed.
         *
         * \param[in] columnIndex The column index of the column that is about to be removed.
         */
        virtual void processAboutToRemoveColumn(unsigned long columnIndex);

        /**
         * Slot that is triggered after a new row is inserted.
         *
         * \param[in] rowIndex The row index of the newly inserted row.
         */
        virtual void processRowInserted(unsigned long rowIndex);

        /**
         * Slot that is triggered after a new column is inserted.
         *
         * \param[in] columnIndex The column index of the newly inserted column.
         */
        virtual void processColumnInserted(unsigned long columnIndex);

    private:
        /**
         * Overloaded method that is called after the grid dimensions are changed.
         *
         * \param[in] oldNumberRows    The old number of rows.
         *
         * \param[in] oldNumberColumns The old number of columns.
         *
         * \param[in] newNumberRows    The new number of rows.
         *
         * \param[in] newNumberColumns The new number of columns.
         */
        void geometryChanged(
            unsigned long oldNumberRows,
            unsigned long oldNumberColumns,
            unsigned long newNumberRows,
            unsigned long newNumberColumns
        ) override;

        /**
         * Overloaded method that is called just before a row is removed.
         *
         * \param[in] rowIndex The row index of the row that is about to be removed.
         */
        void aboutToRemoveRow(unsigned long rowIndex) override;

        /**
         * Overloaded method that is called just before a column is removed.
         *
         * \param[in] columnIndex The column index of the column that is about to be removed.
         */
        void aboutToRemoveColumn(unsigned long columnIndex) override;

        /**
         * Overloaded method that is called after a new row is inserted.
         *
         * \param[in] rowIndex The row index of the newly inserted row.
         */
        void rowInserted(unsigned long rowIndex) override;

        /**
         * Overloaded method that is called after a new column is inserted.
         *
         * \param[in] columnIndex The column index of the newly inserted column.
         */
        void columnInserted(unsigned long columnIndex) override;
};

#endif
