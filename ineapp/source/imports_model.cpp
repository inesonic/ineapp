/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImportsModel class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QString>
#include <QChar>
#include <QVariant>
#include <QColor>
#include <QModelIndex>
#include <QAbstractTableModel>
#include <QList>
#include <QFileInfo>
#include <QMessageBox>

#include <limits>

#include <ld_root_element.h>
#include <ld_root_import.h>

#include "metatypes.h"
#include "application.h"
#include "plug_in_manager.h"
#include "imports_model.h"


ImportsModel::ImportsModel(QObject* parent):QAbstractTableModel(parent) {
    configure();

    currentReferencePath.clear();
    currentImports.clear();
}


ImportsModel::ImportsModel(
        const QString&               newReferencePath,
        const QList<Ld::RootImport>& newImports,
        QObject*                     parent
    ):QAbstractTableModel(
        parent
    ) {
    configure();

    currentReferencePath = newReferencePath;
    currentImports       = newImports;
}


ImportsModel::~ImportsModel() {}


void ImportsModel::setReferencePath(const QString& newReferencePath) {
    beginResetModel();
    currentReferencePath = newReferencePath;
    endResetModel();
}


QString ImportsModel::referencePath() const {
    return currentReferencePath;
}


void ImportsModel::setImports(const QList<Ld::RootImport>& newImports) {
    beginResetModel();
    currentImports = newImports;
    endResetModel();
}


QList<Ld::RootImport> ImportsModel::imports() const {
    return currentImports;
}


Qt::ItemFlags ImportsModel::flags(const QModelIndex&) const {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}


QVariant ImportsModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    unsigned column = static_cast<unsigned>(index.column());
    switch (role) {
        case Qt::DisplayRole: {
            unsigned row = static_cast<unsigned>(index.row());
            const Ld::RootImport& import = currentImports.at(row);

            if (column == 0) {
                if (import.isRelativeImport()) {
                    result = QVariant::fromValue(tr("relative"));
                } else {
                    result = QVariant::fromValue(tr("absolute"));
                }
            } else {
                result = QVariant::fromValue(import.path(currentReferencePath));
            }

            break;
        }

        case Qt::EditRole: {
            unsigned row = static_cast<unsigned>(index.row());
            const Ld::RootImport& import = currentImports.at(row);

            if (column == 0) {
                if (import.isRelativeImport()) {
                    result = QVariant::fromValue(tr("relative"));
                } else {
                    result = QVariant::fromValue(tr("absolute"));
                }
            } else {
                QSharedPointer<Ld::RootElement> element = import.rootElement();
                result = QVariant::fromValue(element->shortformName(std::numeric_limits<unsigned>::max()));
            }

            break;

        }

        case Qt::WhatsThisRole: {
            QString text;
            if (column == 0) {
                text = tr(
                    "Indicates if the imported file is tracked relative to the current program or whether the "
                    "imported file's location is absolute, fixed and independent of where the current program "
                    "currently exists on the computer.\n\n"
                    "Use \"relative\" if you expect to bundle both files together and will move them around as a "
                    "group.  Use \"absolute\" if the imported file is treated as a library with a location, such as a "
                    "network share, that never changes."
                );
            } else {
                text = tr(
                    "Indicates the location and name of the imported file."
                );
            }

            result = QVariant::fromValue(text);
            break;
        }

        case Qt::ToolTipRole: {
            unsigned row = static_cast<unsigned>(index.row());
            const Ld::RootImport& import = currentImports.at(row);

            if (import.isInvalid()) {
                result = QVariant::fromValue(tr("Invalid file"));
            } else if (!import.hasFilename()) {
                result = QVariant::fromValue(tr("New/unsaved file"));
            } else {
                QString filePath = import.absolutePath();
                QFileInfo fileInformation(filePath);

                if (!fileInformation.exists()) {
                    result = QVariant::fromValue(tr("File does not exist"));
                } else if (!fileInformation.isFile()) {
                    result = QVariant::fromValue(tr("Not a normal file"));
                } else if (!fileInformation.isReadable()) {
                    result = QVariant::fromValue(tr("File is not readable"));
                } else {
                    if (import.isRelativeImport()) {
                        result = QVariant::fromValue(tr("Location is relative to this file"));
                    } else {
                        result = QVariant::fromValue(tr("Location is fixed"));
                    }
                }
            }

            break;
        }

        case Qt::ForegroundRole: {
            unsigned row = static_cast<unsigned>(index.row());
            const Ld::RootImport& import = currentImports.at(row);

            if (import.isInvalid()) {
                result = QVariant::fromValue(QColor(Qt::red));
            } else if (!import.hasFilename()) {
                result = QVariant::fromValue(QColor(Qt::blue));
            } else {
                QString filePath = import.absolutePath();
                QFileInfo fileInformation(filePath);

                if (fileInformation.exists() && fileInformation.isFile() && fileInformation.isReadable()) {
                    result = QVariant::fromValue(QColor(Qt::black));
                } else {
                    result = QVariant::fromValue(QColor(Qt::red));
                }
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


bool ImportsModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    bool result;

    if (role == Qt::EditRole) {
        unsigned       row         = static_cast<unsigned>(index.row());
        unsigned       column      = static_cast<unsigned>(index.column());
        Ld::RootImport import      = currentImports.at(row);
        QString        stringValue = value.toString();

        if (column == 0) {
            if (stringValue == tr("relative")) {
                import.setRelativeImport();
                result = true;
            } else if (stringValue == tr("absolute")) {
                import.setAbsoluteImport();
                result = true;
            } else {
                result = false;
            }

            if (result == true) {
                currentImports[row] = import;
                emit dataChanged(index, createIndex(row, 1));
            }
        } else {
            QString                         identifier  = stringValue;
            QSharedPointer<Ld::RootElement> rootElement = Ld::RootElement::byIdentifier(identifier);

            if (!rootElement.isNull()) {
                result = true;
            } else {
                Ld::PlugInsByName plugInsByName = Application::plugInManager()->plugInsByName();

                rootElement = Ld::Element::createElement<Ld::RootElement>();
                result      = rootElement->openExisting(stringValue, false, plugInsByName);

                if (result == false) {
                    emit failedToAccess(identifier);
                }
            }

            if (result == true) {
                import.setRootElement(rootElement);
                currentImports[row] = import;
                emit dataChanged(index, index);
            }
        }
    } else {
        result = false;
    }

    return result;
}

\


QVariant ImportsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant result;
    if (orientation == Qt::Horizontal) {
        switch (role) {
            case Qt::DisplayRole: {
                switch (section) {
                    case 0: {
                        result = QVariant::fromValue(tr("Relative"));
                        break;
                    }

                    case 1: {
                        result = QVariant::fromValue(tr("Path"));
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


int ImportsModel::columnCount(const QModelIndex&) const {
    return 2;
}


int ImportsModel::rowCount(const QModelIndex&) const {
    return currentImports.size();
}


void ImportsModel::configure() {}
