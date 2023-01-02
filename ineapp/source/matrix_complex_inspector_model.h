/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixComplexInspectorModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_COMPLEX_INSPECTOR_MODEL_H
#define MATRIX_COMPLEX_INSPECTOR_MODEL_H

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_matrix_complex.h>

#include <ld_calculated_value.h>

#include "matrix_inspector_model.h"

/**
 * Class that provides a model for complex matrices.
 */
class MatrixComplexInspectorModel:public MatrixInspectorModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] calculatedValue The new calculated value.
         *
         * \param[in] parent          The parent object for this menu.
         */
        MatrixComplexInspectorModel(const Ld::CalculatedValue& calculatedValue, QObject* parent = Q_NULLPTR);

        ~MatrixComplexInspectorModel() override;

        /**
         * Method used by the view to obtain the header data to be displayed.
         *
         * \param[in] section     The column or row number.
         *
         * \param[in] orientation The orientation requiring header data.
         *
         * \param[in] role        The desired display role.
         *
         * \return Returns a variant type holding the data to beinde displayed.
         */
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        /**
         * Method used by the view to obtain the data to be displayed.
         *
         * \param[in] index The index into the model used to locate the data.
         *
         * \param[in] role  The display role for the data.
         *
         * \return Returns a variant type holding the data to be displayed.
         */
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        /**
         * Method that returns the currently displayed data.
         *
         * \return Returns the currently displayed data.
         */
        Ld::CalculatedValue currentCalculatedValue() const override;

        /**
         * Method that determines if load/save buttons should be displayed.
         *
         * \return Returns true if the load/save buttons should be included.  Returns false if the load/save buttons
         *         should be excluded.
         */
        bool includeLoadSaveSupport() const override;

        /**
         * Method that determines the sibling cell for a given cell.  This method exists to support custom item
         *  selection models.  The default implementation will assert.
         *
         * \param[in] index The index of the desired cell.
         *
         * \return Returns the sibling cell index.
         */
        QModelIndex siblingIndex(const QModelIndex& index) const override;

    public slots:
        /**
         * Slot you can trigger to set the calculated value for the model.
         *
         * \param[in] newCalculatedValue The new calculated value the model should operate on.
         */
        void setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) override;

        /**
         * Slot you can trigger to restore the model to the initial values.
         */
        void resetModel() override;

        /**
         * Method that is called to load the variable contents from a file.
         *
         * \param[in] filename     The filename of the file holding the contents to be loaded.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        bool loadValue(const QString& filename, bool binaryFormat) override;

        /**
         * Method that is called to save the variable contents to a file.
         *
         * \param[in] filename     The filename of the file to save the contents to.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        bool saveValue(const QString& filename, bool binaryFormat) override;

    protected:
        /**
         * Method that gets the value for a cell.
         *
         * \param[in] rowIndex    The zero based row index of the row to be updated.
         *
         * \param[in] columnIndex The zero based column index of the column to be updated.
         *
         * \return Returns the value for the cell.
         */
        QVariant cellValue(unsigned long rowIndex, unsigned long columnIndex) const override;

        /**
         * Method that updates the calculated value for a cell.
         *
         * \param[in] rowIndex    The zero based row index of the row to be updated.
         *
         * \param[in] columnIndex The zero based column index of the column to be updated.
         *
         * \param[in] newValue    The new value to apply.
         *
         * \return Returns true on success, returns false if the variant can not be converted.
         */
        bool updateCellValue(unsigned long rowIndex, unsigned long columnIndex, const QVariant& newValue) override;

        /**
         * Method that reports the number of matrix rows.
         *
         * \return Returns the number of matrix rows.
         */
        unsigned long numberRows() const override;

        /**
         * Method that reports the number of matrix columns.
         *
         * \return Returns the number of matrix columns.
         */
        unsigned long numberColumns() const override;

        /**
         * Method that is called to remove one or more columns.
         *
         * \param[in] column      The starting column to be removed.
         *
         * \param[in] columnCount The number of columns to be removed.
         *
         * \return Returns true on success, returns false on error.
         */
        bool removeMatrixColumns(int column, int columnCount) override;

        /**
         * Method that is called to insert one or more columns.
         *
         * \param[in] column      The zero based index of the column to be inserted.
         *
         * \param[in] columnCount The number of columns to be inserted.
         *
         * \return Returns true on success, returns false on error.
         */
        bool insertMatrixColumns(int column, int columnCount) override;

        /**
         * Method that is called to remove one or more rows.
         *
         * \param[in] row      The starting row to be removed.
         *
         * \param[in] rowCount The number of rows to be removed.
         *
         * \return Returns true on success, returns false on error.
         */
        bool removeMatrixRows(int row, int rowCount) override;

        /**
         * Method that is called to insert one or more rows.
         *
         * \param[in] row      The zero based index of the row to be inserted.
         *
         * \param[in] rowCount The number of rows to be inserted.
         *
         * \return Returns true on success, returns false on error.
         */
        bool insertMatrixRows(int row, int rowCount) override;

    private:
        /**
         * The underlying variable name.
         */
        Ld::VariableName currentName;

        /**
         * The input matrix.
         */
        Model::MatrixComplex inputMatrix;

        /**
         * The active matrix.
         */
        Model::MatrixComplex currentMatrix;

        /**
         * Flag indicating if the current matrix differs from the input matrix.
         */
        bool currentInputMatrixIsDifferent;
};

#endif
