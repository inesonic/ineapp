/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_OPERATOR_PRESENTATION_H
#define MATRIX_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_matrix_operator_visual.h>

#include "app_common.h"
#include "matrix_operator_presentation_base.h"
#include "presentation_with_grid_children.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::MatrixOperatorElement class.  Note that this class' inheritance is
 * rather odd in that it inherits from \ref PresentationWithFixedChildren rather than the more expected
 * \ref PresentationWithGridChildren class.
 */
class APP_PUBLIC_API MatrixOperatorPresentation:public         MatrixOperatorPresentationBase,
                                                public virtual Ld::MatrixOperatorVisual {
    Q_OBJECT

    public:
        MatrixOperatorPresentation();

        ~MatrixOperatorPresentation() override;

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::GROUPED.
         */
        Ld::Element::ChildPlacement childPlacement() const override;

        /**
         * Static method that creates an instance of a \ref MatrixOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    signals:
        /**
         * Signal that is emitted after the grid dimensions are changed.  This method is not triggered when rows and
         * columns are inserted or removed.
         *
         * \param[in] oldNumberRows    The old number of rows.
         *
         * \param[in] oldNumberColumns The old number of columns.
         *
         * \param[in] newNumberRows    The new number of rows.
         *
         * \param[in] newNumberColumns The new number of columns.
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
         * \param[in] rowIndex The row index of the row that is about to be removed.
         */
        void aboutToRemoveRowFromElement(unsigned long rowIndex);

        /**
         * Signal that is emitted just before a column is removed.
         *
         * \param[in] columnIndex The column index of the column that is about to be removed.
         */
        void aboutToRemoveColumnFromElement(unsigned long columnIndex);

        /**
         * Signal that is emitted after a new row is inserted.
         *
         * \param[in] rowIndex The row index of the newly inserted row.
         */
        void rowInsertedIntoElement(unsigned long rowIndex);

        /**
         * Signal that is emitted after a new column is inserted.
         *
         * \param[in] columnIndex The column index of the newly inserted column.
         */
        void columnInsertedIntoElement(unsigned long columnIndex);

    protected slots:
        /**
         * Slot that is triggeredafter the grid dimensions are changed.  This method is not triggered when rows and
         * columns are inserted or removed.
         *
         * \param[in] oldNumberRows    The old number of rows.
         *
         * \param[in] oldNumberColumns The old number of columns.
         *
         * \param[in] newNumberRows    The new number of rows.
         *
         * \param[in] newNumberColumns The new number of columns.
         */
        void processGeometryChanged(
            unsigned long oldNumberRows,
            unsigned long oldNumberColumns,
            unsigned long newNumberRows,
            unsigned long newNumberColumns
        );

        /**
         * Slot that is triggeredjust before a row is removed.
         *
         * \param[in] rowIndex The row index of the row that is about to be removed.
         */
        void processRemovedRow(unsigned long rowIndex);

        /**
         * Slot that is triggeredjust before a column is removed.
         *
         * \param[in] columnIndex The column index of the column that is about to be removed.
         */
        void processRemovedColumn(unsigned long columnIndex);

        /**
         * Slot that is triggeredafter a new row is inserted.
         *
         * \param[in] rowIndex The row index of the newly inserted row.
         */
        void processRowInserted(unsigned long rowIndex);

        /**
         * Slot that is triggeredafter a new column is inserted.
         *
         * \param[in] columnIndex The column index of the newly inserted column.
         */
        void processColumnInserted(unsigned long columnIndex);

    protected:
        /**
         * Method that is used to build the graphics item for this operator.  The default implementation builds the
         * graphics item from calls to other virtual methods.  This method will also call update the border and tool
         * tip based on any currently reported diagnostics.
         *
         * \param[in]  format                     Pointer to the format instance tied to the element.
         *
         * \param[in]  parenthesisStyle           Indicates the type of parenthesis that should be placed around the
         *                                        operator.  A value of
         *                                        OperatorPresentationBase::ParenthesisStyle::INVALID indicates that
         *                                        parenthesis should not be included.
         *
         * \param[in]  relativeScale              The relative scale to apply to the various symbols.
         *
         * \param[in]  childPresentationAreaData  The presentation area data for the children.
         *
         * \param[out] requiredSize               The required size of the graphics item, in scene units.
         *
         * \param[out] requiredAscent             The required ascent of the graphics item, in scene units.
         *
         * \return Returns the newly constructed graphics item.
         */
        EQt::GraphicsMathGroup* buildGraphicsItem(
            Ld::FormatPointer                     format,
            ParenthesisStyle                      parenthesisStyle,
            float                                 relativeScale,
            const QList<PresentationAreaTracker>& childPresentationAreaData,
            QSizeF&                               requiredSize,
            float&                                requiredAscent
        ) override;

        /**
         * Method that is used to build the graphics item for this operator.  The default implementation builds the
         * graphics item from calls to other virtual methods.  This method will also call update the border and tool
         * tip based on any currently reported diagnostics.
         *
         * \param[in]  format                     Pointer to the format instance tied to the element.
         *
         * \param[in]  currentItem                The current graphics multi-text item to be updated.
         *
         * \param[in]  parenthesisStyle           Indicates the type of parenthesis that should be placed around the
         *                                        operator.  A value of
         *                                        OperatorPresentationBase::ParenthesisStyle::INVALID indicates that
         *                                        parenthesis should not be included.
         *
         * \param[in]  relativeScale              The relative scale to apply to the various symbols.
         *
         * \param[in]  childPresentationAreaData  The presentation area data for the children.
         *
         * \param[out] requiredSize               The required size of the graphics item, in scene units.
         *
         * \param[out] requiredAscent             The required ascent of the graphics item, in scene units.
         *
         * \return Returns the a pointer to the current (or new) EQt::GraphicsMathGroup item.
         */
        EQt::GraphicsMathGroup* updateGraphicsItem(
            Ld::FormatPointer                     format,
            EQt::GraphicsMathGroup*               currentItem,
            ParenthesisStyle                      parenthesisStyle,
            float                                 relativeScale,
            const QList<PresentationAreaTracker>& childPresentationAreaData,
            QSizeF&                               requiredSize,
            float&                                requiredAscent
        ) override;

    private:
        /**
         * Overloaded method that is called after the grid dimensions are changed.  This method is not triggered when
         * rows and columns are inserted or removed.
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

        /**
         * Method that is called to trigger repositioning of this element.
         */
        void forceRepositioning();
};

#endif
