/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixIntegerInspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_INTEGER_INSPECTOR_WIDGET_H
#define MATRIX_INTEGER_INSPECTOR_WIDGET_H

#include <QWidget>
#include <QComboBox>

#include <ld_calculated_value.h>

#include "app_common.h"
#include "matrix_inspector_widget.h"

namespace Ld {
    class CalculatedValue;
};

class MatrixInspectorModel;

/**
 * Inspector widget for integer matrix variables.
 */
class APP_PUBLIC_API MatrixIntegerInspectorWidget:public MatrixInspectorWidget {
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
        MatrixIntegerInspectorWidget(QWidget* parent = Q_NULLPTR);

        ~MatrixIntegerInspectorWidget() override;

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
         * Method that calculates a recommended/initial column size.
         *
         * \return Returns the recommended column size for each column.  A value of 0 will cause the horizontal
         *         headers to auto-resize.
         */
        unsigned initialColumnWidth() const override;
};

#endif
