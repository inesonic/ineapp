/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CommandListModel class.
***********************************************************************************************************************/

#include <QString>
#include <QChar>
#include <QVariant>
#include <QMap>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>

#include "metatypes.h"
#include "application.h"
#include "command_popup_dialog.h"
#include "command_list_model.h"

CommandListModel::CommandListModel(QObject* parent):QAbstractTableModel(parent) {
    currentPrefix        = QString();
    currentFont          = Application::font();
    currentMap           = Q_NULLPTR;
    geometryUpdateNeeded = true;
}


CommandListModel::CommandListModel(const QMap<QString, QString>* map, QObject* parent):QAbstractTableModel(parent) {
    currentPrefix        = QString();
    currentFont          = Application::font();
    geometryUpdateNeeded = true;

    setMap(map);
}


CommandListModel::CommandListModel(const QFont& font, QObject* parent):QAbstractTableModel(parent) {
    currentPrefix        = QString();
    currentFont          = font;
    currentMap           = Q_NULLPTR;
    geometryUpdateNeeded = true;
}


CommandListModel::CommandListModel(
        const QMap<QString, QString>* map,
        const QFont&                  font,
        QObject*                      parent
    ):QAbstractTableModel(
        parent
    ) {
    currentPrefix = QString();
    currentFont          = font;
    geometryUpdateNeeded = true;
    setMap(map);
}


CommandListModel::~CommandListModel() {}


void CommandListModel::setMap(const QMap<QString, QString>* map) {
    beginResetModel();

    currentMap           = map;
    geometryUpdateNeeded = true;
    emit recommendedGeometryChanged(this);

    endResetModel();
}


const QMap<QString, QString>* CommandListModel::map() const {
    return currentMap;
}


void CommandListModel::setPrefix(const QString& newPrefix) {
    beginResetModel();

    currentPrefix        = newPrefix;
    geometryUpdateNeeded = true;
    emit recommendedGeometryChanged(this);

    endResetModel();
}


QString CommandListModel::prefix() const {
    return currentPrefix;
}


void CommandListModel::setFont(const QFont& newFont) {
    beginResetModel();

    currentFont          = newFont;
    geometryUpdateNeeded = true;
    emit recommendedGeometryChanged(this);

    endResetModel();
}

unsigned CommandListModel::recommendedColumn1Width() {
    if (geometryUpdateNeeded) {
        geometryUpdateNeeded = false;
        calculateRecommendedGeometryData();
    }

    return currentColumn1Width;
}


unsigned CommandListModel::recommendedColumn2Width() {
    if (geometryUpdateNeeded) {
        geometryUpdateNeeded = false;
        calculateRecommendedGeometryData();
    }

    return currentColumn2Width;
}


unsigned CommandListModel::recommendedRowHeight() {
    if (geometryUpdateNeeded) {
        geometryUpdateNeeded = false;
        calculateRecommendedGeometryData();
    }

    return currentRowHeight;
}


QVariant CommandListModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    unsigned column = static_cast<unsigned>(index.column());
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole: {
            unsigned row = static_cast<unsigned>(index.row());
            QMap<QString,QString>::const_iterator pos = currentMap->constBegin() + row;

            if (column == 0) {
                QString value = pos.key(); // FIXME
                result = QVariant::fromValue(currentPrefix + value);
            } else {
                QString value = pos.value();
                result = QVariant::fromValue(tr("- %1").arg(value));
            }

            break;
        }

        case Qt::FontRole: {
            QFont font = currentFont;

            if (column == 1) {
                font.setItalic(true);
            }

            result = QVariant::fromValue(font);
            break;
        }

        case Qt::ForegroundRole: {
            if (column == 0) {
                result = QVariant::fromValue(QColor(Qt::black));
            } else {
                result = QVariant::fromValue(QColor(Qt::darkGray));
            }

            break;
        }

        case Qt::TextAlignmentRole: {
            result = QVariant::fromValue(Qt::AlignLeft);
            break;
        }
    }

    return result;
}


QVariant CommandListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant result;
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole: {
                switch (section) {
                    case 0: {
                        result = QVariant::fromValue(tr("Command"));
                        break;
                    }

                    case 1: {
                        result = QVariant::fromValue(tr("Description"));
                        break;
                    }

                    default: {
                        break;
                    }
                }

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


int CommandListModel::columnCount(const QModelIndex&) const {
    return currentMap != Q_NULLPTR ? 2 : 0;
}


int CommandListModel::rowCount(const QModelIndex&) const {
    return currentMap != Q_NULLPTR ? currentMap->size() : 0;
}


void CommandListModel::calculateRecommendedGeometryData() {
    QFontMetrics metrics(currentFont);

    unsigned column1Width = 0;
    unsigned column2Width = 0;

    if (currentMap != Q_NULLPTR) {
        QMap<QString, QString>::const_iterator pos = currentMap->constBegin();
        QMap<QString, QString>::const_iterator end = currentMap->constEnd();

        while (pos != end) {
            unsigned width1 = metrics.horizontalAdvance(currentPrefix + pos.key());
            unsigned width2 = metrics.horizontalAdvance(tr("- %1").arg(pos.value()));

            if (width1 > column1Width) {
                column1Width = width1;
            }

            if (width2 > column2Width) {
                column2Width = width2;
            }

            ++pos;
        }
    }

    currentColumn1Width = column1Width;
    currentColumn2Width = column2Width;
    currentRowHeight    = metrics.height();
}
