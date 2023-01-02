/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableColumnWidthsModel class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QString>
#include <QChar>
#include <QVariant>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>
#include <QList>

#include <util_units.h>

#include "application.h"
#include "metatypes.h"
#include "table_column_widths_delegate.h"
#include "table_column_widths_model.h"

const bool  TableColumnWidthsModel::defaultToProportional          = true;
const float TableColumnWidthsModel::defaultColumnFixedWidth        = 72.0;
const float TableColumnWidthsModel::defaultColumnProportionalWidth = 0.125;

TableColumnWidthsModel::TableColumnWidthsModel(QObject* parent):QAbstractTableModel(parent) {
    currentUnits = Unit::POINT;
}


TableColumnWidthsModel::~TableColumnWidthsModel() {}


unsigned TableColumnWidthsModel::numberTableColumns() const {
    return static_cast<unsigned>(currentColumnWidths.size());
}


bool TableColumnWidthsModel::isProportionalColumn(unsigned tableColumnIndex) const {
    return currentProportionalColumn.at(tableColumnIndex);
}


bool TableColumnWidthsModel::isFixedColumn(unsigned tableColumnIndex) const {
    return !currentProportionalColumn.at(tableColumnIndex);
}


float TableColumnWidthsModel::columnWidth(unsigned tableColumnIndex) const {
    return currentColumnWidths.at(tableColumnIndex);
}


TableColumnWidthsModel::Unit TableColumnWidthsModel::units() const {
    return currentUnits;
}


Qt::ItemFlags TableColumnWidthsModel::flags(const QModelIndex&) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


QVariant TableColumnWidthsModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole: {
            unsigned row = static_cast<unsigned>(index.row());
            if (row < static_cast<unsigned>(currentColumnWidths.size())) {
                QString columnWidthString;
                float   columnWidth = currentColumnWidths.at(row);

                if (currentProportionalColumn.at(row)) {
                    if (columnWidth > 0) {
                        columnWidthString = tr("%1%").arg(columnWidth * 100.0);
                    }
                } else {
                    if (columnWidth > 0) {
                        switch (currentUnits) {
                            case Unit::INVALID: {
                                columnWidthString = tr("Invalid");
                                break;
                            }

                            case Unit::MILLIMETER: {
                                columnWidthString = tr("%1 mm").arg(Util::Units::mmPerPoint * columnWidth);
                                break;
                            }

                            case Unit::CENTIMETER: {
                                columnWidthString = tr("%1 cm").arg(Util::Units::cmPerPoint * columnWidth);
                                break;
                            }

                            case Unit::POINT: {
                                columnWidthString = tr("%1 pt").arg(columnWidth);
                                break;
                            }

                            case Unit::PICA: {
                                columnWidthString = tr("%1 pc").arg(columnWidth / 12.0F);
                                break;
                            }

                            case Unit::INCH: {
                                columnWidthString = tr("%1\"").arg(Util::Units::inchesPerPoint * columnWidth);
                                break;
                            }

                            default: {
                                Q_ASSERT(false);
                                break;
                            }
                        }
                    }
                }

                result = QVariant::fromValue(columnWidthString);
            }

            break;
        }

        case Qt::WhatsThisRole: {
            QString text = tr(
                "You can use these fields to enter column width settings.  You can enter column widths either as "
                "fixed width values with a dimension or as a dimensionless proportional size.  If you specify a fixed "
                "width, you can use the dimensions: pt (point), pc (pica), in (inch), \" (inch), mm (millimeter), "
                "or cm (centimeter).   A proportional size is indicated by either a dimensionless value or with a "
                "percentage value."
            );

            result = QVariant::fromValue(text);
            break;
        }

        case Qt::ToolTipRole: {
            unsigned row  = static_cast<unsigned>(index.row());
            QString  text = tr("Enter the width of column %1 here.").arg(row + 1);
            result = QVariant::fromValue(text);

            break;
        }

        case Qt::TextAlignmentRole: {
            result = QVariant::fromValue(Qt::AlignLeft);
            break;
        }
    }

    return result;
}


bool TableColumnWidthsModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    bool result;

    if (role == Qt::EditRole) {
        QString text = value.toString();
        QRegularExpressionMatch match = TableColumnWidthsDelegate::inputRegularExpression.match(text);

        if (match.hasMatch()) {
            QString valueString = match.captured(1);
            QString unitString  = match.captured(2);

            float rawValue = valueString.toFloat(&result);
            if (result && rawValue > 0.0F) {
                bool  isProportional = true;
                float columnWidth    = 0.0F;

                if (!unitString.isNull()) {
                    QString units = unitString.toLower();
                    if (units == tr("mm")) {
                        isProportional = false;
                        columnWidth    = Util::Units::pointsPerMm * rawValue;
                    } else if (units == tr("cm")) {
                        isProportional = false;
                        columnWidth    = Util::Units::pointsPerCm * rawValue;
                    } else if (units == tr("m") || units == tr("meter") || units == tr("meters")) {
                        isProportional = false;
                        columnWidth    = Util::Units::pointsPerMm * rawValue * 1000.0F;
                    } else if (units == tr("pt") || units == tr("point") || units == tr("points")) {
                        isProportional = false;
                        columnWidth    = rawValue;
                    } else if (units == tr("pc") || units == tr("pica") || units == tr("picas")) {
                        isProportional = false;
                        columnWidth    = rawValue / 12.0F;
                    } else if (units == tr("\"") || units == tr("in") || units == tr("inch") || units == tr("inches")) {
                        isProportional = false;
                        columnWidth    = Util::Units::pointsPerInch * rawValue;
                    } else if (units == tr("ft") || units == tr("foot") || units == tr("feet")) {
                        isProportional = false;
                        columnWidth    = Util::Units::pointsPerInch * rawValue;
                    } else if (units == tr("%")) {
                        isProportional = true;
                        columnWidth    = rawValue / 100.0F;
                    } else {
                        result = false;
                    }
                }

                if (result) {
                    unsigned tableColumnIndex = static_cast<unsigned>(index.row());
                    setColumnWidth(tableColumnIndex, isProportional, columnWidth);

                    emit columnWidthChanged(tableColumnIndex, isProportional, columnWidth);
                }
            } else {
                result = false;
            }
        } else {
            result = false;
        }
    } else {
        result = false;
    }

    return result;
}


QVariant TableColumnWidthsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant result;

    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole: {
                result = QVariant::fromValue(tr("Column Width (include unit or %)"));
                break;
            }

            case Qt::TextAlignmentRole: {
                result = QVariant::fromValue(Qt::AlignLeft);
                break;
            }
        }
    } else {
        switch (role) {
            case Qt::DisplayRole: {
                result = QVariant::fromValue(section + 1);
                break;
            }

            case Qt::TextAlignmentRole: {
                result = QVariant::fromValue(Qt::AlignRight);
                break;
            }
        }
    }

    return result;
}


int TableColumnWidthsModel::columnCount(const QModelIndex&) const {
    return 1;
}


int TableColumnWidthsModel::rowCount(const QModelIndex&) const {
    return numberTableColumns();
}


void TableColumnWidthsModel::setNumberTableColumns(unsigned newNumberColumns) {
    unsigned currentNumberColumns = static_cast<unsigned>(currentColumnWidths.size());

    if (newNumberColumns > currentNumberColumns) {
        beginResetModel();

        while (static_cast<unsigned>(currentProportionalColumn.size()) < newNumberColumns) {
            currentProportionalColumn.append(defaultToProportional);
        }

        while (static_cast<unsigned>(currentColumnWidths.size()) < newNumberColumns) {
            currentColumnWidths.append(
                defaultToProportional ? defaultColumnProportionalWidth : defaultColumnFixedWidth
            );
        }

        adjustProportionalColumns();
        endResetModel();
    } else if (newNumberColumns < currentNumberColumns) {
        beginResetModel();

        currentProportionalColumn.erase(
            currentProportionalColumn.begin() + newNumberColumns,
            currentProportionalColumn.end()
        );

        currentColumnWidths.erase(currentColumnWidths.begin() + newNumberColumns, currentColumnWidths.end());

        endResetModel();
    }
}


void TableColumnWidthsModel::setProportionalColumn(unsigned tableColumnIndex, bool nowProportional) {
    if (currentProportionalColumn.at(tableColumnIndex) != nowProportional) {
        beginResetModel();

        currentProportionalColumn[tableColumnIndex] = nowProportional;

        if (nowProportional) {
            currentColumnWidths[tableColumnIndex] = defaultColumnProportionalWidth;
            adjustProportionalColumns();
        } else {
            currentColumnWidths[tableColumnIndex] = defaultColumnFixedWidth;
        }

        endResetModel();
    }
}


void TableColumnWidthsModel::setFixedColumn(unsigned tableColumnIndex, bool nowFixed) {
    setProportionalColumn(tableColumnIndex, !nowFixed);
}


void TableColumnWidthsModel::setColumnWidth(unsigned tableColumnIndex, float newWidth) {
    beginResetModel();

    currentColumnWidths[tableColumnIndex] = newWidth;
    if (currentProportionalColumn.at(tableColumnIndex)) {
        adjustProportionalColumns();
    }

    endResetModel();
}


void TableColumnWidthsModel::setColumnWidth(unsigned tableColumnIndex, bool nowProportional, float newWidth) {
    beginResetModel();

    currentColumnWidths[tableColumnIndex]       = newWidth;
    currentProportionalColumn[tableColumnIndex] = nowProportional;
    if (nowProportional) {
        adjustProportionalColumns();
    }

    endResetModel();
}


void TableColumnWidthsModel::setUnits(Unit newUnits) {
    beginResetModel();
    currentUnits = newUnits;
    endResetModel();
}


void TableColumnWidthsModel::adjustProportionalColumns() {}
