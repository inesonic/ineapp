/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixComplexInspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_COMPLEX_INSPECTOR_WIDGET_H
#define MATRIX_COMPLEX_INSPECTOR_WIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QItemSelectionModel>

#include <ld_calculated_value.h>

#include "app_common.h"
#include "matrix_inspector_widget.h"

namespace Ld {
    class CalculatedValue;
};

class MatrixInspectorModel;

/**
 * Inspector widget for complex matrix variables.
 */
class APP_PUBLIC_API MatrixComplexInspectorWidget:public MatrixInspectorWidget {
    Q_OBJECT

    public:
        /**
         * Factory function
         *
         * \param[in] parent The parent widget.
         */
        static InspectorWidget* creator(QWidget* parent);

        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent widget.
         */
        MatrixComplexInspectorWidget(QWidget* parent = Q_NULLPTR);

        ~MatrixComplexInspectorWidget() override;

    protected:
        /**
         * Method that is called to generate the model behind the table view.
         *
         * \param[in] calculatedValue The calculated value.
         *
         * \return Returns a model suitable for use.
         */
        MatrixInspectorModel* createModel(const Ld::CalculatedValue& calculatedValue) override;

        /**
         * Method that is called to generate a custom delegate for the table view.
         *
         * \return Returns the item delegate.
         */
        QAbstractItemDelegate* createDelegate() override;

        /**
         * Method that is called to generate a custom selection model for the table view.  The default implementation
         * returns a null pointer.
         *
         * \param[in] tableView The table view the selection model is interacting with.
         *
         * \param[in] model     The model holding the table data.
         *
         * \return Returns the custom selection model.  Returns a null pointer if the default selection model should
         *         be used.
         */
        QItemSelectionModel* createSelectionModel(QTableView* tableView, MatrixInspectorModel* model) override;

        /**
         * Method that calculates a recommended/initial column size.
         *
         * \return Returns the recommended column size for each column.  A value of 0 will cause the horizontal
         *         headers to auto-resize.
         */
        unsigned initialColumnWidth() const override;
};

#endif
