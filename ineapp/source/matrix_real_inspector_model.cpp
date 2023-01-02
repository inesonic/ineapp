/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref MatrixRealInspectorModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_exceptions.h>
#include <model_matrix_real.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "metatypes.h"
#include "matrix_inspector_model.h"
#include "matrix_real_inspector_model.h"

MatrixRealInspectorModel::MatrixRealInspectorModel(
        const Ld::CalculatedValue& calculatedValue,
        QObject*                   parent
    ):MatrixInspectorModel(
        parent
    ) {
    setCalculatedValue(calculatedValue);
}


MatrixRealInspectorModel::~MatrixRealInspectorModel() {}


Ld::CalculatedValue MatrixRealInspectorModel::currentCalculatedValue() const {
    return Ld::CalculatedValue(currentName, Model::Variant(currentMatrix));
}


bool MatrixRealInspectorModel::includeLoadSaveSupport() const {
    return true;
}


void MatrixRealInspectorModel::setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) {
    beginResetModel();

    currentName   = newCalculatedValue.name();
    inputMatrix   = newCalculatedValue.variant().toMatrixReal();
    currentMatrix = inputMatrix;

    currentInputMatrixIsDifferent = false;

    endResetModel();
}


void MatrixRealInspectorModel::resetModel() {
    beginResetModel();

    currentMatrix                 = inputMatrix;
    currentInputMatrixIsDifferent = false;

    endResetModel();
}


bool MatrixRealInspectorModel::loadValue(const QString& filename, bool /* binaryFormat */) {
    bool success = true;

    beginResetModel();

    Model::MatrixReal newMatrix;
    try {
        newMatrix = Model::MatrixReal::fromFile(filename.toLocal8Bit().data());
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


bool MatrixRealInspectorModel::saveValue(const QString& filename, bool binaryFormat) {
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


QVariant MatrixRealInspectorModel::cellValue(unsigned long rowIndex, unsigned long columnIndex) const {
    QVariant result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())       &&
        columnIndex <= static_cast<unsigned long>(currentMatrix.numberColumns())    ) {
        Model::MatrixReal::Scalar value = currentMatrix.at(rowIndex + 1, columnIndex + 1);
        result = QVariant(QString::number(value));
    }

    return result;
}


bool MatrixRealInspectorModel::updateCellValue(
        unsigned long   rowIndex,
        unsigned long   columnIndex,
        const QVariant& newValue
    ) {
    bool result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())       &&
        columnIndex <= static_cast<unsigned long>(currentMatrix.numberColumns())    ) {
        QString text  = newValue.toString();

        Model::MatrixReal::Scalar value = text.toDouble(&result);
        if (result) {
            Model::MatrixReal::Scalar inputValue;
            if (rowIndex < static_cast<unsigned long>(inputMatrix.numberRows())       &&
                columnIndex < static_cast<unsigned long>(inputMatrix.numberColumns())    ) {
                inputValue = inputMatrix.at(rowIndex + 1, columnIndex + 1);
            } else {
                inputValue = Model::MatrixReal::Scalar(0);
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
        }
    } else {
        result = false;
    }

    return result;
}


unsigned long MatrixRealInspectorModel::numberRows() const {
    return currentMatrix.numberRows();
}


unsigned long MatrixRealInspectorModel::numberColumns() const {
    return currentMatrix.numberColumns();
}


bool MatrixRealInspectorModel::removeMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (column > 0) {
        if (static_cast<unsigned long>(column + columnCount) < numberColumns) {
            Model::MatrixReal left = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(1), Model::Integer(column))
            );

            Model::MatrixReal right = currentMatrix.at(
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
            currentMatrix = Model::MatrixReal();
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


bool MatrixRealInspectorModel::insertMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (column == 0) {
        Model::MatrixReal left = Model::MatrixReal::zero(numberRows, columnCount);
        currentMatrix = left.combineLeftToRight(currentMatrix);
    } else if (static_cast<unsigned long>(column + 1) > numberColumns) {
        Model::MatrixReal right = Model::MatrixReal::zero(numberRows, columnCount);
        currentMatrix = currentMatrix.combineLeftToRight(right);
    } else {
        Model::MatrixReal left = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(column))
        );

        Model::MatrixReal right = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(column + 1), Model::Integer(numberColumns))
        );

        Model::MatrixReal center = Model::MatrixReal::zero(numberRows, columnCount);
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


bool MatrixRealInspectorModel::removeMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row > 0) {
        if (static_cast<unsigned long>(row + rowCount) < numberRows) {
            Model::MatrixReal top = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(row)),
                Model::Range(Model::Integer(1), Model::Integer(numberColumns))
            );

            Model::MatrixReal bottom = currentMatrix.at(
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
            currentMatrix = Model::MatrixReal();
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


bool MatrixRealInspectorModel::insertMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row == 0) {
        Model::MatrixReal top = Model::MatrixReal::zero(rowCount, numberColumns);
        currentMatrix = top.combineTopToBottom(currentMatrix);
    } else if (static_cast<unsigned long>(row + 1) > numberColumns) {
        Model::MatrixReal bottom = Model::MatrixReal::zero(rowCount, numberColumns);
        currentMatrix = currentMatrix.combineTopToBottom(bottom);
    } else {
        Model::MatrixReal top = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(row)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixReal bottom = currentMatrix.at(
            Model::Range(Model::Integer(row + 1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixReal center = Model::MatrixReal::zero(rowCount, numberColumns);
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
