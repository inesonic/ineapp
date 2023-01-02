/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref MatrixBooleanInspectorModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_exceptions.h>
#include <model_matrix_boolean.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "metatypes.h"
#include "matrix_inspector_model.h"
#include "matrix_boolean_inspector_model.h"

MatrixBooleanInspectorModel::MatrixBooleanInspectorModel(
        const Ld::CalculatedValue& calculatedValue,
        QObject*                   parent
    ):MatrixInspectorModel(
        parent
    ) {
    setCalculatedValue(calculatedValue);
}


MatrixBooleanInspectorModel::~MatrixBooleanInspectorModel() {}


Ld::CalculatedValue MatrixBooleanInspectorModel::currentCalculatedValue() const {
    return Ld::CalculatedValue(currentName, Model::Variant(currentMatrix));
}


bool MatrixBooleanInspectorModel::includeLoadSaveSupport() const {
    return true;
}


void MatrixBooleanInspectorModel::setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) {
    beginResetModel();

    currentName   = newCalculatedValue.name();
    inputMatrix   = newCalculatedValue.variant().toMatrixBoolean();
    currentMatrix = inputMatrix;

    currentInputMatrixIsDifferent = false;

    endResetModel();
}


void MatrixBooleanInspectorModel::resetModel() {
    beginResetModel();

    currentMatrix                 = inputMatrix;
    currentInputMatrixIsDifferent = false;

    endResetModel();
}


bool MatrixBooleanInspectorModel::loadValue(const QString& filename, bool /* binaryFormat */) {
    bool success = true;

    beginResetModel();

    Model::MatrixBoolean newMatrix;
    try {
        newMatrix = Model::MatrixBoolean::fromFile(filename.toLocal8Bit().data());
    } catch (const Model::InesonicException& e) {
        (void) e;
        success = false;
    }

    if (success) {
        currentMatrix                 = newMatrix;
        currentInputMatrixIsDifferent = (currentMatrix != inputMatrix);

        if (currentInputMatrixIsDifferent) {
            emit valueChanged(Ld::CalculatedValue(currentName, currentMatrix));
        } else {
            emit valueRestored(Ld::CalculatedValue(currentName, currentMatrix));
        }
    }

    endResetModel();

    return success;
}


bool MatrixBooleanInspectorModel::saveValue(const QString& filename, bool binaryFormat) {
    bool success = true;

    try {
        currentMatrix.toFile(
            filename.toLocal8Bit().data(),
            binaryFormat ? Model::DataFileFormat::BINARY : Model::DataFileFormat::CSV
        );
    } catch (const Model::InesonicException& e) {
        (void) e;
        success = false;
    }

    return success;
}


QVariant MatrixBooleanInspectorModel::cellValue(unsigned long rowIndex, unsigned long columnIndex) const {
    QVariant result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())       &&
        columnIndex <= static_cast<unsigned long>(currentMatrix.numberColumns())    ) {
        Model::MatrixBoolean::Scalar value = currentMatrix.at(rowIndex + 1, columnIndex + 1);
        result = QVariant(value ? tr("true") : tr("false"));
    }

    return result;
}


bool MatrixBooleanInspectorModel::updateCellValue(
        unsigned long   rowIndex,
        unsigned long   columnIndex,
        const QVariant& newValue
    ) {
    bool result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())       &&
        columnIndex <= static_cast<unsigned long>(currentMatrix.numberColumns())    ) {
        QString                      text       = newValue.toString().toLower();
        Model::MatrixBoolean::Scalar value      = (text == tr("true") || text == tr("1"));

        Model::MatrixBoolean::Scalar inputValue;
        if (rowIndex < static_cast<unsigned long>(inputMatrix.numberRows())       &&
            columnIndex < static_cast<unsigned long>(inputMatrix.numberColumns())    ) {
            inputValue = inputMatrix.at(rowIndex + 1, columnIndex + 1);
        } else {
            inputValue = Model::MatrixBoolean::Scalar(0);
        }

        currentMatrix.update(rowIndex + 1, columnIndex + 1, value);

        if (value != inputValue) {
            currentInputMatrixIsDifferent = true;
        } else {
            currentInputMatrixIsDifferent = (currentMatrix != inputMatrix);
        }

        if (currentInputMatrixIsDifferent) {
            emit valueChanged(Ld::CalculatedValue(currentName, currentMatrix));
        } else {
            emit valueRestored(Ld::CalculatedValue(currentName, currentMatrix));
        }

        result = true;
    } else {
        result = false;
    }

    return result;
}


unsigned long MatrixBooleanInspectorModel::numberRows() const {
    return currentMatrix.numberRows();
}


unsigned long MatrixBooleanInspectorModel::numberColumns() const {
    return currentMatrix.numberColumns();
}


bool MatrixBooleanInspectorModel::removeMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (column > 0) {
        if (static_cast<unsigned long>(column + columnCount) < numberColumns) {
            Model::MatrixBoolean left = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(1), Model::Integer(column))
            );

            Model::MatrixBoolean right = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(column + columnCount + 1), Model::Integer(numberColumns))
            );

            currentMatrix = left.combineLeftToRight(right);
        } else {
            currentMatrix = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(1), Model::Integer(column))
            );
        }
    } else {
        if (static_cast<unsigned long>(columnCount) == numberColumns) {
            currentMatrix = Model::MatrixBoolean();
        } else {
            currentMatrix = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(column + columnCount + 1), Model::Integer(numberColumns))
            );
        }
    }

    currentInputMatrixIsDifferent = (currentMatrix != inputMatrix);

    if (currentInputMatrixIsDifferent) {
        emit valueChanged(Ld::CalculatedValue(currentName, currentMatrix));
    } else {
        emit valueRestored(Ld::CalculatedValue(currentName, currentMatrix));
    }

    return true;
}


bool MatrixBooleanInspectorModel::insertMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (column == 0) {
        Model::MatrixBoolean left = Model::MatrixBoolean::zero(numberRows, columnCount);
        currentMatrix = left.combineLeftToRight(currentMatrix);
    } else if (static_cast<unsigned long>(column + 1) > numberColumns) {
        Model::MatrixBoolean right = Model::MatrixBoolean::zero(numberRows, columnCount);
        currentMatrix = currentMatrix.combineLeftToRight(right);
    } else {
        Model::MatrixBoolean left = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(column))
        );

        Model::MatrixBoolean right = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(column + 1), Model::Integer(numberColumns))
        );

        Model::MatrixBoolean center = Model::MatrixBoolean::zero(numberRows, columnCount);
        currentMatrix = left.combineLeftToRight(center).combineLeftToRight(right);
    }

    currentInputMatrixIsDifferent = (currentMatrix != inputMatrix);

    if (currentInputMatrixIsDifferent) {
        emit valueChanged(Ld::CalculatedValue(currentName, currentMatrix));
    } else {
        emit valueRestored(Ld::CalculatedValue(currentName, currentMatrix));
    }

    return true;
}


bool MatrixBooleanInspectorModel::removeMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row > 0) {
        if (static_cast<unsigned long>(row + rowCount) < numberRows) {
            Model::MatrixBoolean top = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(row)),
                Model::Range(Model::Integer(1), Model::Integer(numberColumns))
            );

            Model::MatrixBoolean bottom = currentMatrix.at(
                Model::Range(Model::Integer(row + rowCount + 1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(1), Model::Integer(numberColumns))
            );

            currentMatrix = top.combineTopToBottom(bottom);
        } else {
            currentMatrix = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(row)),
                Model::Range(Model::Integer(1), Model::Integer(numberColumns))
            );
        }
    } else {
        if (static_cast<unsigned long>(rowCount) == numberRows) {
            currentMatrix = Model::MatrixBoolean();
        } else {
            currentMatrix = currentMatrix.at(
                Model::Range(Model::Integer(row + rowCount + 1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(1), Model::Integer(numberColumns))
            );
        }
    }

    currentInputMatrixIsDifferent = (currentMatrix != inputMatrix);

    if (currentInputMatrixIsDifferent) {
        emit valueChanged(Ld::CalculatedValue(currentName, currentMatrix));
    } else {
        emit valueRestored(Ld::CalculatedValue(currentName, currentMatrix));
    }

    return true;
}


bool MatrixBooleanInspectorModel::insertMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row == 0) {
        Model::MatrixBoolean top = Model::MatrixBoolean::zero(rowCount, numberColumns);
        currentMatrix = top.combineTopToBottom(currentMatrix);
    } else if (static_cast<unsigned long>(row + 1) > numberColumns) {
        Model::MatrixBoolean bottom = Model::MatrixBoolean::zero(rowCount, numberColumns);
        currentMatrix = currentMatrix.combineTopToBottom(bottom);
    } else {
        Model::MatrixBoolean top = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(row)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixBoolean bottom = currentMatrix.at(
            Model::Range(Model::Integer(row + 1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixBoolean center = Model::MatrixBoolean::zero(rowCount, numberColumns);
        currentMatrix = top.combineTopToBottom(center).combineTopToBottom(bottom);
    }

    currentInputMatrixIsDifferent = (currentMatrix != inputMatrix);

    if (currentInputMatrixIsDifferent) {
        emit valueChanged(Ld::CalculatedValue(currentName, currentMatrix));
    } else {
        emit valueRestored(Ld::CalculatedValue(currentName, currentMatrix));
    }

    return true;
}
