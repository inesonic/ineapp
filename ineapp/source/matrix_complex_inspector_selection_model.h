/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixComplexInspectorSelectionModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_COMPLEX_INSPECTOR_SELECTION_MODEL_H
#define MATRIX_COMPLEX_INSPECTOR_SELECTION_MODEL_H

#include <QObject>
#include <QModelIndex>
#include <QItemSelection>
#include <QItemSelectionModel>

#include <model_matrix_complex.h>

#include <ld_calculated_value.h>

#include "matrix_inspector_model.h"

/**
 * Class that provides a selection model for complex matrices.
 */
class MatrixComplexInspectorSelectionModel:public QItemSelectionModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] model  The model managing the data behind the selection.
         *
         * \param[in] parent The parent object for this menu.
         */
        MatrixComplexInspectorSelectionModel(MatrixInspectorModel* model, QObject* parent);

        /**
         * Constructor.
         *
         * \param[in] model  The model managing the data behind the selection.
         */
        MatrixComplexInspectorSelectionModel(MatrixInspectorModel* model = Q_NULLPTR);

        ~MatrixComplexInspectorSelectionModel() override;

    public slots:
        /**
         * Slot that is triggered to clear the selection model.
         */
        void clear() override;

        /**
         * Slot that is triggered to clear the current index in the model.
         */
        void clearCurrentIndex() override;

        /**
         * Slot that is triggered to reset the selection model.
         */
        void reset() override;

        /**
         * Slot that is triggered to select a range of cells.
         *
         * \param[in] selection The selection
         *
         * \param[in] command   The operation to perform on the selection.
         */
        void select(const QItemSelection& selection, SelectionFlags command) override;

        /**
         * Slot that is triggered to select a single cell.
         *
         * \param[in] index   The cell index.
         *
         * \param[in] command The operation to perform on the selection.
         */
        void select(const QModelIndex& index, SelectionFlags command) override;

        /**
         * Slot that is triggered to set the current index.
         *
         * \param[in] index
         *
         * \param[in] command
         */
        void setCurrentIndex(const QModelIndex& index, SelectionFlags command) override;

    private:
        MatrixInspectorModel* currentModel;
};

#endif
