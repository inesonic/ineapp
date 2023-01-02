/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SetInspectorModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SET_INSPECTOR_MODEL_H
#define SET_INSPECTOR_MODEL_H

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_set.h>
#include <model_variant.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "app_common.h"

/**
 * Class that provides a list model for sets.
 */
class APP_PUBLIC_API SetInspectorModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        SetInspectorModel(QObject* parent = Q_NULLPTR);

        ~SetInspectorModel() override;

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
        const Model::Variant& valueAt(unsigned long rowIndex) const;

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

        /**
         * Slot you can trigger to remove a value from the model.
         *
         * \param[in] value The value to be removed.
         *
         * \return Returns true on success, returns false if the value does not exist in the set.
         */
        bool removeValue(const Model::Variant& value);

        /**
         * Slot you can trigger to remove multiple values from a model.
         *
         * \param[in] values The values to be removed.
         *
         * \return Returns true on success, returns false if the value does not exist in the set.
         */
        bool removeValues(const QList<Model::Variant>& values);

        /**
         * Slot you can trigger to insert a new value into the model.
         *
         * \param[in] value The value to be added.
         *
         * \return Returns true on success, returns false if the value already exists in the set.
         */
        bool addValue(const Model::Variant& value);

    private:
        /**
         * Method that builds the variant list from a list.
         */
        void buildVariantList();

        /**
         * A dummy variant value used when the row index is invalid.
         */
        static const Model::Variant dummyVariant;

        /**
         * The variable name.
         */
        Ld::VariableName currentName;

        /**
         * The input set.
         */
        Model::Set inputSet;

        /**
         * The current set.
         */
        Model::Set currentSet;

        /**
         * The list of variants in the set.
         */
        QList<Model::Variant> currentVariantList;
};

#endif
