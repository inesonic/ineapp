/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixInspectorModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_INSPECTOR_MODEL_H
#define MATRIX_INSPECTOR_MODEL_H

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <ld_calculated_value.h>

/**
 * Class that provides a base class for the matrix inspector models.  You can extend this class to customize it for
 * specific data types.
 */
class MatrixInspectorModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        MatrixInspectorModel(QObject* parent = Q_NULLPTR);

        ~MatrixInspectorModel() override;

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
         * Method used by the view to obtain the item flags.
         *
         * \param[in] index The index of the cell being queried.
         *
         * \return Returns the flags tied to the cell.
         */
        Qt::ItemFlags flags(const QModelIndex& index) const override;

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
         * Method used by the model to update the contents of a cell.
         *
         * \param[in] index The index indicating the row/column being updated.
         *
         * \param[in] value A variant holding the new value for the cell.
         *
         * \param[in] role  The role for the data being updated.
         *
         * \return Returns true on success.  Returns false on error.
         */
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

        /**
         * Method used by the view to obtain the number of columns to be displayed.
         *
         * \return Returns the number of displayable columns.
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method used by the view to obtain the number of rows to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method that is called to remove one or more columns.
         *
         * \param[in] column      The starting column to be removed.
         *
         * \param[in] columnCount The number of columns to be removed.
         *
         * \param[in] parent      The model index of the parent.
         *
         * \return Returns true on success, returns false on error.
         */
        bool removeColumns(int column, int columnCount, const QModelIndex& parent = QModelIndex()) override;

        /**
         * Method that is called to insert one or more columns.
         *
         * \param[in] column      The zero based index of the column to be inserted.
         *
         * \param[in] columnCount The number of columns to be inserted.
         *
         * \param[in] parent      The model index of the parent.
         *
         * \return Returns true on success, returns false on error.
         */
        bool insertColumns(int column, int columnCount, const QModelIndex& parent = QModelIndex()) override;

        /**
         * Method that is called to remove one or more rows.
         *
         * \param[in] row      The starting row to be removed.
         *
         * \param[in] rowCount The number of rows to be removed.
         *
         * \param[in] parent   The model index of the parent.
         *
         * \return Returns true on success, returns false on error.
         */
        bool removeRows(int row, int rowCount, const QModelIndex& parent = QModelIndex()) override;

        /**
         * Method that is called to insert one or more rows.
         *
         * \param[in] row      The zero based index of the row to be inserted.
         *
         * \param[in] rowCount The number of rows to be inserted.
         *
         * \param[in] parent   The model index of the parent.
         *
         * \return Returns true on success, returns false on error.
         */
        bool insertRows(int row, int rowCount, const QModelIndex& parent = QModelIndex()) override;

        /**
         * Method that returns the currently displayed data.
         *
         * \return Returns the currently displayed data.
         */
        virtual Ld::CalculatedValue currentCalculatedValue() const = 0;

        /**
         * Method that determines if load/save buttons should be displayed.
         *
         * \return Returns true if the load/save buttons should be included.  Returns false if the load/save buttons
         *         should be excluded.
         */
        virtual bool includeLoadSaveSupport() const = 0;

        /**
         * Method that determines the sibling cell for a given cell.  This method exists to support custom item
         *  selection models.  The default implementation will assert.
         *
         * \param[in] index The index of the desired cell.
         *
         * \return Returns the sibling cell index.
         */
        virtual QModelIndex siblingIndex(const QModelIndex& index) const;

    signals:
        /**
         * Signal you should emit when the calculated value is updated.  This signal should not be emitted when the
         * calculated value is set.
         *
         * \param[out] newCalculatedValue The newly changed calculated value.
         */
        void valueChanged(const Ld::CalculatedValue& newCalculatedValue);

        /**
         * Signal you should emit when the calculated value is restored to its original value.  This signal should not
         * be emitted when the calculated value is reset.
         *
         * \param[out] originalCalculatedValue The restored calculated value.
         */
        void valueRestored(const Ld::CalculatedValue& originalCalculatedValue);

    public slots:
        /**
         * Slot you can trigger to set the calculated value for the model.
         *
         * \param[in] newCalculatedValue The new calculated value the model should operate on.
         */
        virtual void setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) = 0;

        /**
         * Slot you can trigger to restore the model to the initial values.
         */
        virtual void resetModel() = 0;

        /**
         * Method that is called to load the variable contents from a file.
         *
         * \param[in] filename     The filename of the file holding the contents to be loaded.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        virtual bool loadValue(const QString& filename, bool binaryFormat) = 0;

        /**
         * Method that is called to save the variable contents to a file.
         *
         * \param[in] filename     The filename of the file to save the contents to.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        virtual bool saveValue(const QString& filename, bool binaryFormat) = 0;

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
        virtual QVariant cellValue(unsigned long rowIndex, unsigned long columnIndex) const = 0;

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
        virtual bool updateCellValue(unsigned long rowIndex, unsigned long columnIndex, const QVariant& newValue) = 0;

        /**
         * Method that reports the number of matrix rows.
         *
         * \return Returns the number of matrix rows.
         */
        virtual unsigned long numberRows() const = 0;

        /**
         * Method that reports the number of matrix columns.
         *
         * \return Returns the number of matrix columns.
         */
        virtual unsigned long numberColumns() const = 0;

        /**
         * Method that is called to remove one or more columns.
         *
         * \param[in] column      The starting column to be removed.
         *
         * \param[in] columnCount The number of columns to be removed.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool removeMatrixColumns(int column, int columnCount) = 0;

        /**
         * Method that is called to insert one or more columns.
         *
         * \param[in] column      The zero based index of the column to be inserted.
         *
         * \param[in] columnCount The number of columns to be inserted.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool insertMatrixColumns(int column, int columnCount) = 0;

        /**
         * Method that is called to remove one or more rows.
         *
         * \param[in] row      The starting row to be removed.
         *
         * \param[in] rowCount The number of rows to be removed.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool removeMatrixRows(int row, int rowCount) = 0;

        /**
         * Method that is called to insert one or more rows.
         *
         * \param[in] row      The zero based index of the row to be inserted.
         *
         * \param[in] rowCount The number of rows to be inserted.
         *
         * \return Returns true on success, returns false on error.
         */
        virtual bool insertMatrixRows(int row, int rowCount) = 0;
};

#endif
