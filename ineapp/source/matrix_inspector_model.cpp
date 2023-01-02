/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref MatrixInspectorModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "metatypes.h"
#include "application.h"
#include "matrix_inspector_model.h"

MatrixInspectorModel::MatrixInspectorModel(QObject* parent):QAbstractTableModel(parent) {}


MatrixInspectorModel::~MatrixInspectorModel() {}


QVariant MatrixInspectorModel::headerData(int section, Qt::Orientation /* orientation */, int role) const {
    QVariant result;

    if (role == Qt::DisplayRole) {
        result = QVariant::fromValue(QString::number(section + 1));
    } else if (role == Qt::TextAlignmentRole) {
        result = QVariant::fromValue(Qt::AlignRight);
    }

    return result;
}


Qt::ItemFlags MatrixInspectorModel::flags(const QModelIndex& index) const {
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}


QVariant MatrixInspectorModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole: {
            unsigned long row    = static_cast<unsigned long>(index.row());
            unsigned long column = static_cast<unsigned long>(index.column());

            result = cellValue(row, column);

            break;
        }

        case Qt::TextAlignmentRole: {
            result = QVariant::fromValue(Qt::AlignRight);
            break;
        }
    }

    return result;
}


bool MatrixInspectorModel::setData(const QModelIndex& index, const QVariant& variant, int role) {
    bool result;

    if (role == Qt::EditRole) {
        unsigned long row    = static_cast<unsigned long>(index.row());
        unsigned long column = static_cast<unsigned long>(index.column());

        result = updateCellValue(row, column, variant);
        if (result) {
            emit dataChanged(index, index, { role });
        }
    } else {
        result = false;
    }

    return result;
}


int MatrixInspectorModel::columnCount(const QModelIndex& /* parent */) const {
    return numberColumns();
}


int MatrixInspectorModel::rowCount(const QModelIndex& /* parent */) const {
    return numberRows();
}


bool MatrixInspectorModel::removeColumns(int column, int columnCount, const QModelIndex& /* parent */) {
    beginResetModel();
    bool result = removeMatrixColumns(column, columnCount);
    endResetModel();

    return result;
}


bool MatrixInspectorModel::insertColumns(int column, int columnCount, const QModelIndex& /* parent */) {
    beginResetModel();
    bool result = insertMatrixColumns(column, columnCount);
    endResetModel();

    return result;
}


bool MatrixInspectorModel::removeRows(int row, int rowCount, const QModelIndex& /* parent */) {
    beginResetModel();
    bool result = removeMatrixRows(row, rowCount);
    endResetModel();

    return result;
}


bool MatrixInspectorModel::insertRows(int row, int rowCount, const QModelIndex& /* parent */) {
    beginResetModel();
    bool result = insertMatrixRows(row, rowCount);
    endResetModel();

    return result;
}


QModelIndex MatrixInspectorModel::siblingIndex(const QModelIndex& /*index */) const {
    Q_ASSERT(false);
    return QModelIndex();
}


bool MatrixInspectorModel::loadValue(const QString& /* filename */, bool /* binaryFormat */) {
    return false;
}


bool MatrixInspectorModel::saveValue(const QString& /* filename */, bool /* binaryFormat */) {
    return false;
}
