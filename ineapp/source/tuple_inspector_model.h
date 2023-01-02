/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TupleInspectorModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TUPLE_INSPECTOR_MODEL_H
#define TUPLE_INSPECTOR_MODEL_H

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_tuple.h>
#include <model_variant.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "app_common.h"

/**
 * Class that provides a table model for tuples.
 */
class APP_PUBLIC_API TupleInspectorModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        TupleInspectorModel(QObject* parent = Q_NULLPTR);

        ~TupleInspectorModel() override;

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
         * Method used by the view to obtain the number of rows to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method used by the view to obtain the number of columns to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

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
         * Method that returns the currently displayed data.
         *
         * \return Returns the currently displayed data.
         */
        Ld::CalculatedValue currentCalculatedValue() const;

        /**
         * Method that indicates if the calculated value has been changed from the initial value.
         *
         * \return Returns true if the calculated value changed.  Returns false if the calculated value is unchanged.
         */
        bool calculatedValueChanged() const;

        /**
         * Method that obtains the variant at a specified row.
         *
         * \param[in] rowIndex The zero based row index to get the variant data for.
         *
         * \return Returns the variant data at the specified row.  An invalid value is returned if the row index is
         *         invalid.
         */
        Model::Variant valueAt(unsigned long rowIndex) const;

        /**
         * Method that aggregates items and moves the item up.
         *
         * \param[in] newRow         The new insertion position.
         *
         * \param[in] aggregatedRows The list of aggregated rows.
         */
        void mergeAndRelocate(unsigned long newRow, const QList<unsigned long>& aggregatedRows);

        /**
         * Method that replaces a value at a specified index.
         *
         * \param[in] rowIndex The row index of the entry to replace.
         *
         * \param[in] variant  The variant to replace at the specified location.
         */
        void replaceValue(unsigned long rowIndex, const Model::Variant& variant);

        /**
         * Method that inserts a value at a specified index.
         *
         * \param[in] rowIndex The row index of the entry to insert.  The new entry will reside at this location.
         *
         * \param[in] variant  The variant to replace at the specified location.
         */
        void insertValue(unsigned long rowIndex, const Model::Variant& variant);

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
        void setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue);

        /**
         * Slot you can trigger to restore the model to the initial values.
         */
        void resetModel();

    private:
        /**
         * Method that generates signals for the model.
         */
        void generateSignals();

        /**
         * A dummy variant value used when the row index is invalid.
         */
        static const Model::Variant dummyVariant;

        /**
         * The variable name.
         */
        Ld::VariableName currentName;

        /**
         * The input tuple.
         */
        Model::Tuple inputTuple;

        /**
         * The current tuple.
         */
        Model::Tuple currentTuple;
};

#endif
