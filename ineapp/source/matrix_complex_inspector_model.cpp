/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref MatrixComplexInspectorModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <model_exceptions.h>
#include <model_matrix_complex.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "metatypes.h"
#include "matrix_inspector_model.h"
#include "matrix_complex_inspector_model.h"

MatrixComplexInspectorModel::MatrixComplexInspectorModel(
        const Ld::CalculatedValue& calculatedValue,
        QObject*                   parent
    ):MatrixInspectorModel(
        parent
    ) {
    setCalculatedValue(calculatedValue);
}


MatrixComplexInspectorModel::~MatrixComplexInspectorModel() {}


QVariant MatrixComplexInspectorModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant result;

    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if ((section % 2) == 0) {
                result = QVariant::fromValue(QString::number(1 + section / 2));
            } else {
                result = QVariant::fromValue(tr("%1i").arg(1 + section / 2));
            }
        } else {
            result = QVariant::fromValue(QString::number(section + 1));
        }
    } else if (role == Qt::BackgroundRole) {
        if (orientation == Qt::Horizontal && (section % 2) == 1) {
            result = QVariant::fromValue(QColor(Qt::lightGray));
        }
    } else if (role == Qt::TextAlignmentRole) {
        result = QVariant::fromValue(Qt::AlignRight);
    }

    return result;
}


QVariant MatrixComplexInspectorModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    if (role == Qt::BackgroundRole) {
        unsigned long columnIndex = static_cast<unsigned long>(index.column());
        if ((columnIndex % 2) == 1) {
            result = QVariant::fromValue(QColor(Qt::lightGray));
        }
    } else {
        result = MatrixInspectorModel::data(index, role);
    }

    return result;
}


Ld::CalculatedValue MatrixComplexInspectorModel::currentCalculatedValue() const {
    return Ld::CalculatedValue(currentName, Model::Variant(currentMatrix));
}


bool MatrixComplexInspectorModel::includeLoadSaveSupport() const {
    return true;
}


QModelIndex MatrixComplexInspectorModel::siblingIndex(const QModelIndex& index) const {
    unsigned long row    = index.row();
    unsigned long column = index.column();
    void*         ptr    = index.internalPointer();

    return (column % 2) == 0 ? createIndex(row, column + 1, ptr) : createIndex(row, column - 1, ptr);
}


void MatrixComplexInspectorModel::setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) {
    beginResetModel();

    currentName   = newCalculatedValue.name();
    inputMatrix   = newCalculatedValue.variant().toMatrixComplex();
    currentMatrix = inputMatrix;

    currentInputMatrixIsDifferent = false;

    endResetModel();
}


void MatrixComplexInspectorModel::resetModel() {
    beginResetModel();

    currentMatrix                 = inputMatrix;
    currentInputMatrixIsDifferent = false;

    endResetModel();
}


bool MatrixComplexInspectorModel::loadValue(const QString& filename, bool /* binaryFormat */) {
    bool success = true;

    beginResetModel();

    Model::MatrixComplex newMatrix;
    try {
        newMatrix = Model::MatrixComplex::fromFile(filename.toLocal8Bit().data());
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


bool MatrixComplexInspectorModel::saveValue(const QString& filename, bool binaryFormat) {
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


QVariant MatrixComplexInspectorModel::cellValue(unsigned long rowIndex, unsigned long columnIndex) const {
    QVariant result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())           &&
        columnIndex <= static_cast<unsigned long>(2 * currentMatrix.numberColumns())    ) {
        Model::MatrixComplex::Scalar value = currentMatrix.at(rowIndex + 1, (columnIndex / 2) + 1);

        if ((columnIndex % 2) == 0) {
            result = QVariant::fromValue(QString::number(value.real()));
        } else {
            result = QVariant::fromValue(QString::number(value.imag()));
        }
    }

    return result;
}


bool MatrixComplexInspectorModel::updateCellValue(
        unsigned long   rowIndex,
        unsigned long   columnIndex,
        const QVariant& newValue
    ) {
    bool result;

    if (rowIndex <= static_cast<unsigned long>(currentMatrix.numberRows())       &&
        columnIndex <= static_cast<unsigned long>(currentMatrix.numberColumns())    ) {
        QString                         text  = newValue.toString();
        Model::MatrixComplex::Scalar::T value = text.toDouble(&result);
        if (result) {
            Model::MatrixComplex::Scalar inputValue;
            if (rowIndex < static_cast<unsigned long>(inputMatrix.numberRows())           &&
                columnIndex < static_cast<unsigned long>(2 * inputMatrix.numberColumns())    ) {
                inputValue = inputMatrix.at(rowIndex + 1, (columnIndex / 2) + 1);
            } else {
                inputValue = Model::MatrixComplex::Scalar(0);
            }

            Model::MatrixComplex::Scalar currentValue = currentMatrix.at(rowIndex + 1, (columnIndex / 2) + 1);

            Model::MatrixComplex::Scalar newValue;
            if ((columnIndex % 2) == 0) {
                newValue = Model::MatrixComplex::Scalar(value, currentValue.imag());
            } else {
                newValue = Model::MatrixComplex::Scalar(currentValue.real(), value);
            }

            currentMatrix.update(rowIndex + 1, (columnIndex / 2) + 1, newValue);

            if (newValue != inputValue) {
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


unsigned long MatrixComplexInspectorModel::numberRows() const {
    return currentMatrix.numberRows();
}


unsigned long MatrixComplexInspectorModel::numberColumns() const {
    return 2 * currentMatrix.numberColumns();
}


bool MatrixComplexInspectorModel::removeMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    Q_ASSERT((column % 2) == 0);
    Q_ASSERT((columnCount % 2) == 0);

    column      /= 2;
    columnCount /= 2;

    if (column > 0) {
        if (static_cast<unsigned long>(column + columnCount) < numberColumns) {
            Model::MatrixComplex left = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(numberRows)),
                Model::Range(Model::Integer(1), Model::Integer(column))
            );

            Model::MatrixComplex right = currentMatrix.at(
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
            currentMatrix = Model::MatrixComplex();
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


bool MatrixComplexInspectorModel::insertMatrixColumns(int column, int columnCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    Q_ASSERT((columnCount % 2) == 0);

    column = (column + 1) / 2; // This is to handle the insert column after case.
    columnCount /= 2;

    if (column == 0) {
        Model::MatrixComplex left = Model::MatrixComplex::zero(numberRows, columnCount);
        currentMatrix = left.combineLeftToRight(currentMatrix);
    } else if (static_cast<unsigned long>(column + 1) > numberColumns) {
        Model::MatrixComplex right = Model::MatrixComplex::zero(numberRows, columnCount);
        currentMatrix = currentMatrix.combineLeftToRight(right);
    } else {
        Model::MatrixComplex left = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(column))
        );

        Model::MatrixComplex right = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(column + 1), Model::Integer(numberColumns))
        );

        Model::MatrixComplex center = Model::MatrixComplex::zero(numberRows, columnCount);
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


bool MatrixComplexInspectorModel::removeMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row > 0) {
        if (static_cast<unsigned long>(row + rowCount) < numberRows) {
            Model::MatrixComplex top = currentMatrix.at(
                Model::Range(Model::Integer(1), Model::Integer(row)),
                Model::Range(Model::Integer(1), Model::Integer(numberColumns))
            );

            Model::MatrixComplex bottom = currentMatrix.at(
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
            currentMatrix = Model::MatrixComplex();
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


bool MatrixComplexInspectorModel::insertMatrixRows(int row, int rowCount) {
    unsigned long numberRows    = static_cast<unsigned long>(currentMatrix.numberRows());
    unsigned long numberColumns = static_cast<unsigned long>(currentMatrix.numberColumns());

    if (row == 0) {
        Model::MatrixComplex top = Model::MatrixComplex::zero(rowCount, numberColumns);
        currentMatrix = top.combineTopToBottom(currentMatrix);
    } else if (static_cast<unsigned long>(row + 1) > numberColumns) {
        Model::MatrixComplex bottom = Model::MatrixComplex::zero(rowCount, numberColumns);
        currentMatrix = currentMatrix.combineTopToBottom(bottom);
    } else {
        Model::MatrixComplex top = currentMatrix.at(
            Model::Range(Model::Integer(1), Model::Integer(row)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixComplex bottom = currentMatrix.at(
            Model::Range(Model::Integer(row + 1), Model::Integer(numberRows)),
            Model::Range(Model::Integer(1), Model::Integer(numberColumns))
        );

        Model::MatrixComplex center = Model::MatrixComplex::zero(rowCount, numberColumns);
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
