/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref TupleInspectorModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_tuple.h>
#include <model_variant.h>

#include <ld_data_type.h>
#include <ld_data_type_decoder.h>
#include <ld_calculated_value.h>

#include "tuple_inspector_model.h"

const Model::Variant TupleInspectorModel::dummyVariant;

TupleInspectorModel::TupleInspectorModel(QObject* parent):QAbstractTableModel(parent) {}


TupleInspectorModel::~TupleInspectorModel() {}


QVariant TupleInspectorModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole: {
            unsigned long row    = static_cast<unsigned long>(index.row());
            unsigned long column = static_cast<unsigned long>(index.column());

            if (row < static_cast<unsigned>(currentTuple.size())) {
                Model::Variant      value = currentTuple.at(row + 1);
                Ld::CalculatedValue calculatedValue(QString(), value);

                if (column == 0) {
                    Ld::DataType dataType = Ld::DataType::fromValueType(value.valueType());
                    if (dataType.isValid()) {
                        if ((dataType.properties() & (Ld::DataType::matrix | Ld::DataType::container)) == 0) {
                            const Ld::DataTypeDecoder* decoder = dataType.decoder();
                            if (decoder != Q_NULLPTR) {
                                result = QVariant::fromValue(decoder->toString(value));
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


int TupleInspectorModel::rowCount(const QModelIndex& /* parent */) const {
    return currentTuple.size();
}


int TupleInspectorModel::columnCount(const QModelIndex& /* parent */) const {
    return 2;
}


bool TupleInspectorModel::removeRows(int row, int rowCount, const QModelIndex& /* parent */) {
    Model::Tuple  newTuple;
    unsigned long numberRows = static_cast<unsigned long>(currentTuple.size());
    for (unsigned long i=0 ; i<numberRows ; ++i) {
        if (i < static_cast<unsigned long>(row) || i >= static_cast<unsigned long>(row + rowCount)) {
            newTuple.append(currentTuple.at(i + 1));
        }
    }

    beginResetModel();
    currentTuple = newTuple;
    endResetModel();

    generateSignals();

    return true;
}


Ld::CalculatedValue TupleInspectorModel::currentCalculatedValue() const {
    return Ld::CalculatedValue(currentName, Model::Variant(currentTuple));
}


bool TupleInspectorModel::calculatedValueChanged() const {
    return (currentTuple != inputTuple);
}


Model::Variant TupleInspectorModel::valueAt(unsigned long rowIndex) const {
    return rowIndex < static_cast<unsigned long>(currentTuple.size()) ? currentTuple.at(rowIndex + 1) : dummyVariant;
}


void TupleInspectorModel::mergeAndRelocate(unsigned long newRow, const QList<unsigned long>& aggregateRows) {
    QSet<unsigned long> rowSet(aggregateRows.constBegin(), aggregateRows.constEnd());

    Model::Tuple newTuple;
    unsigned long numberTerms = static_cast<unsigned long>(currentTuple.size());
    for (unsigned long i=0 ; i<numberTerms ; ++i) {
        if (i == newRow) {
            for (  QList<unsigned long>::const_iterator aggregateRowIterator    = aggregateRows.constBegin(),
                                                        aggregateRowEndIterator = aggregateRows.constEnd()
                 ; aggregateRowIterator != aggregateRowEndIterator
                 ; ++aggregateRowIterator
                ) {
                unsigned long rowIndex = *aggregateRowIterator;
                newTuple.append(currentTuple.at(rowIndex + 1));
            }
        }

        if (!rowSet.contains(i)) {
            newTuple.append(currentTuple.at(i + 1));
        }
    }

    if (newRow >= numberTerms) {
        for (  QList<unsigned long>::const_iterator aggregateRowIterator    = aggregateRows.constBegin(),
                                                    aggregateRowEndIterator = aggregateRows.constEnd()
             ; aggregateRowIterator != aggregateRowEndIterator
             ; ++aggregateRowIterator
            ) {
            unsigned long rowIndex = *aggregateRowIterator;
            newTuple.append(currentTuple.at(rowIndex + 1));
        }
    }

    beginResetModel();
    currentTuple = newTuple;
    endResetModel();
}


void TupleInspectorModel::replaceValue(unsigned long rowIndex, const Model::Variant& variant) {
    beginResetModel();
    currentTuple.update(rowIndex + 1, variant);
    endResetModel();

    generateSignals();
}


void TupleInspectorModel::insertValue(unsigned long rowIndex, const Model::Variant& variant) {
    Model::Tuple newTuple;

    if (rowIndex == 0) {
        newTuple = currentTuple;
        newTuple.prepend(variant);
    } else {
        unsigned long numberTerms = static_cast<unsigned long>(currentTuple.size());
        for (unsigned long i=1 ; i<=numberTerms ; ++i) {
            if (i == rowIndex + 1) {
                newTuple.append(variant);
            }

            newTuple.append(currentTuple.at(i));
        }
    }

    beginResetModel();
    currentTuple = newTuple;
    endResetModel();
}


void TupleInspectorModel::setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) {
    inputTuple  = newCalculatedValue.variant().toTuple();
    currentName = newCalculatedValue.name();

    resetModel();
}


void TupleInspectorModel::resetModel() {
    beginResetModel();
    currentTuple = inputTuple;
    endResetModel();

    generateSignals();
}


void TupleInspectorModel::generateSignals() {
    if (currentTuple != inputTuple) {
        emit valueChanged(Ld::CalculatedValue(currentName, Model::Variant(currentTuple)));
    } else {
        emit valueRestored(Ld::CalculatedValue(currentName, Model::Variant(currentTuple)));
    }
}

