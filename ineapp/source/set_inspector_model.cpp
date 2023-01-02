/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref SetInspectorModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_set.h>
#include <model_variant.h>

#include <ld_data_type.h>
#include <ld_data_type_decoder.h>
#include <ld_calculated_value.h>

#include "set_inspector_model.h"

const Model::Variant SetInspectorModel::dummyVariant;

SetInspectorModel::SetInspectorModel(QObject* parent):QAbstractTableModel(parent) {}


SetInspectorModel::~SetInspectorModel() {}


QVariant SetInspectorModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole: {
            unsigned long row    = static_cast<unsigned long>(index.row());
            unsigned long column = static_cast<unsigned long>(index.column());

            if (row < static_cast<unsigned>(currentVariantList.size())) {
                Ld::CalculatedValue calculatedValue(QString(), currentVariantList.at(row));

                if (column == 0) {
                    Ld::DataType dataType = Ld::DataType::fromValueType(currentVariantList.at(row).valueType());
                    if (dataType.isValid()) {
                        if ((dataType.properties() & (Ld::DataType::matrix | Ld::DataType::container)) == 0) {
                            const Ld::DataTypeDecoder* decoder = dataType.decoder();
                            if (decoder != Q_NULLPTR) {
                                result = QVariant::fromValue(decoder->toString(currentVariantList.at(row)));
                            } else {
                                result = tr("*** COULD NOT DECODE ***");
                            }
                        }
                    } else {
                        result = tr("*** Invalid content ***");
                    }
                } else if (column == 1) {
                    result = QVariant::fromValue(calculatedValue.description());
                }
            }

            break;
        }
    }

    return result;
}


int SetInspectorModel::rowCount(const QModelIndex& /* parent */) const {
    return currentVariantList.size();
}


int SetInspectorModel::columnCount(const QModelIndex& /* parent */) const {
    return 2;
}


Ld::CalculatedValue SetInspectorModel::currentCalculatedValue() const {
    return Ld::CalculatedValue(currentName, Model::Variant(currentSet));
}


bool SetInspectorModel::calculatedValueChanged() const {
    return (currentSet != inputSet);
}


const Model::Variant& SetInspectorModel::valueAt(unsigned long rowIndex) const {
    return   rowIndex < static_cast<unsigned long>(currentVariantList.size())
           ? currentVariantList.at(rowIndex)
           : dummyVariant;
}


void SetInspectorModel::setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) {
    inputSet    = newCalculatedValue.variant().toSet();
    currentName = newCalculatedValue.name();

    resetModel();
}


void SetInspectorModel::resetModel() {
    beginResetModel();

    currentSet = inputSet;
    buildVariantList();

    endResetModel();
}


bool SetInspectorModel::removeValue(const Model::Variant& value) {
    bool success = currentSet.remove(value);

    if (success) {
        beginResetModel();
        buildVariantList();
        endResetModel();
    }

    return success;
}


bool SetInspectorModel::removeValues(const QList<Model::Variant>& values) {
    bool success = true;
    for (QList<Model::Variant>::const_iterator it=values.constBegin(),end=values.constEnd() ; it!=end ; ++it) {
        if (!currentSet.remove(*it)) {
            success = false;
        }
    }

    beginResetModel();
    buildVariantList();
    endResetModel();

    return success;
}


bool SetInspectorModel::addValue(const Model::Variant& value) {
    bool success = currentSet.insert(value);

    if (success) {
        beginResetModel();
        buildVariantList();
        endResetModel();
    }

    return success;
}


void SetInspectorModel::buildVariantList() {
    currentVariantList.clear();
    for (Model::Set::ConstIterator it=currentSet.constBegin(),end=currentSet.constEnd() ; it!=end ; ++it) {
        currentVariantList.append(*it);
    }
}
