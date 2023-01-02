/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref MatrixIntegerInspectorModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_exceptions.h>
#include <model_matrix_integer.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "metatypes.h"
#include "matrix_inspector_model.h"
#include "matrix_integer_inspector_model.h"

MatrixIntegerInspectorModel::MatrixIntegerInspectorModel(
        const Ld::CalculatedValue& calculatedValue,
        QObject*                   parent
    ):MatrixInspectorModel(
        parent
    ) {
    setCalculatedValue(calculatedValue);
}


MatrixIntegerInspectorModel::~MatrixIntegerInspectorModel() {}


Ld::CalculatedValue MatrixIntegerInspectorModel::currentCalculatedValue() const {
    return Ld::CalculatedValue(currentName, Model::Variant(currentMatrix));
}


bool MatrixIntegerInspectorModel::includeLoadSaveSupport() const {
    return true;
}


void MatrixIntegerInspectorModel::setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) {
    beginResetModel();

    currentName   = newCalculatedValue.name();
    inputMatrix   = newCalculatedValue.variant().toMatrixInteger();
    currentMatrix = inputMatrix;

    currentInputMatrixIsDifferent = false;

    endResetModel();
}


void MatrixIntegerInspectorModel::resetModel() {
    beginResetModel();

    currentMatrix                 = inputMatrix;
    currentInputMatrixIsDifferent = false;

    endResetModel();
}


bool MatrixIntegerInspectorModel::loadValue(const QString& filename, bool /* binaryFormat */) {
    bool success = true;

    beginResetModel();

    Model::MatrixInteger newMatrix;
    try {
        newMatrix = Model::MatrixInteger::fromFile(filename.toLocal8Bit().data());
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


bool MatrixIntegerInspectorModel::saveValue(const QString& filename, bool binaryFormat) {
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


QVariant MatrixIntegerInspectorModel::cellValue(unsigned long rowIndex, unsigned long columnIndex) const {
    QVariant result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())       &&
        columnIndex <= static_cast<unsigned long>(currentMatrix.numberColumns())    ) {
        Model::MatrixInteger::Scalar value = currentMatrix.at(rowIndex + 1, columnIndex + 1);
        result = QVariant(QString::number(value));
    }

    return result;
}


bool MatrixIntegerInspectorModel::updateCellValue(
        unsigned long   rowIndex,
        unsigned long   columnIndex,
        const QVariant& newValue
    ) {
    bool result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())       &&
        columnIndex <= static_cast<unsigned long>(currentMatrix.numberColumns())    ) {
        QString        text  = newValue.toString().toLower();
        unsigned       radix = 10;

        if        (text.startsWith(tr("0b")) || text.startsWith(tr("'b"))) {
            text  = text.mid(2);
            radix = 2;
        } else if (text.startsWith(tr("0x")) || text.startsWith(tr("'h"))) {
            text  = text.mid(2);
            radix = 16;
        } else {
            radix = 10;
        }

        Model::Integer value = text.toLongLong(&result, radix);
        if (result) {
            Model::MatrixInteger::Scalar inputValue;
            if (rowIndex < static_cast<unsigned long>(inputMatrix.numberRows())       &&
                columnIndex < static_cast<unsigned long>(inputMatrix.numberColumns())    ) {
                inputValue = inputMatrix.at(rowIndex + 1, columnIndex + 1);
            } else {
                inputValue = Model::MatrixInteger::Scalar(0);
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


unsigned long MatrixIntegerInspectorModel::numberRows() const {
    return currentMatrix.numberRows();
}


unsigned long MatrixIntegerInspectorModel::numberColumns() const {
    return currentMatrix.numberColumns();
}


bool MatrixIntegerInspectorModel::removeMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (column > 0) {
        if (static_cast<unsigned long>(column + columnCount) < numberColumns) {
            Model::MatrixInteger left = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(1), Model::Integer(column))
            );

            Model::MatrixInteger right = currentMatrix.at(
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
            currentMatrix = Model::MatrixInteger();
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


bool MatrixIntegerInspectorModel::insertMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (column == 0) {
        Model::MatrixInteger left = Model::MatrixInteger::zero(numberRows, columnCount);
        currentMatrix = left.combineLeftToRight(currentMatrix);
    } else if (static_cast<unsigned long>(column + 1) > numberColumns) {
        Model::MatrixInteger right = Model::MatrixInteger::zero(numberRows, columnCount);
        currentMatrix = currentMatrix.combineLeftToRight(right);
    } else {
        Model::MatrixInteger left = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(column))
        );

        Model::MatrixInteger right = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(column + 1), Model::Integer(numberColumns))
        );

        Model::MatrixInteger center = Model::MatrixInteger::zero(numberRows, columnCount);
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


bool MatrixIntegerInspectorModel::removeMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row > 0) {
        if (static_cast<unsigned long>(row + rowCount) < numberRows) {
            Model::MatrixInteger top = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(row)),
                Model::Range(Model::Integer(1), Model::Integer(numberColumns))
            );

            Model::MatrixInteger bottom = currentMatrix.at(
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
            currentMatrix = Model::MatrixInteger();
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


bool MatrixIntegerInspectorModel::insertMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row == 0) {
        Model::MatrixInteger top = Model::MatrixInteger::zero(rowCount, numberColumns);
        currentMatrix = top.combineTopToBottom(currentMatrix);
    } else if (static_cast<unsigned long>(row + 1) > numberColumns) {
        Model::MatrixInteger bottom = Model::MatrixInteger::zero(rowCount, numberColumns);
        currentMatrix = currentMatrix.combineTopToBottom(bottom);
    } else {
        Model::MatrixInteger top = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(row)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixInteger bottom = currentMatrix.at(
            Model::Range(Model::Integer(row + 1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixInteger center = Model::MatrixInteger::zero(rowCount, numberColumns);
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
