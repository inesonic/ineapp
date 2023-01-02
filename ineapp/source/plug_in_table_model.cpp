/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref PlugInTableModel class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <ld_plug_in_information.h>

#include "application.h"
#include "plug_in_manager.h"
#include "plug_in_table_model.h"

PlugInTableModel::PlugInTableModel(QObject* parent):QAbstractTableModel(parent) {
    PlugInManager*           PlugInManager = Application::plugInManager();
    const Ld::PlugInsByName& plugInsByName = PlugInManager->plugInsByName();

    for (  Ld::PlugInsByName::const_iterator it  = plugInsByName.constBegin(),
                                             end = plugInsByName.constEnd()
         ; it!=end
         ; ++it
        ) {
        allPlugIns.append(it.value());
    }
}


PlugInTableModel::~PlugInTableModel() {}


QVariant PlugInTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant result;

    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (section) {
                case 0: { result = QVariant(tr("Name"));          break; }
                case 1: { result = QVariant(tr("Version"));       break; }
                case 2: { result = QVariant(tr("Vendor"));        break; }
                case 3: { result = QVariant(tr("Author"));        break; }
                case 4: { result = QVariant(tr("Description"));   break; }
            }
        } else if (role == Qt::TextAlignmentRole) {
            result = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        }
    }

    return result;
}


QVariant PlugInTableModel::data(const QModelIndex& index, int role) const {
    QVariant result;

    if (role == Qt::EditRole || role == Qt::DisplayRole) {
        unsigned row    = static_cast<unsigned>(index.row());
        unsigned column = static_cast<unsigned>(index.column());

        if (row < static_cast<unsigned>(allPlugIns.size()) || column <= 4) {
            const Ld::PlugInInformation& plugInInformation = allPlugIns.at(row);

            switch (column) {
                case 0: { result = QVariant(plugInInformation.name());               break; }
                case 1: { result = QVariant(plugInInformation.version());            break; }
                case 2: { result = QVariant(plugInInformation.company());            break; }
                case 3: { result = QVariant(plugInInformation.author());             break; }
                case 4: { result = QVariant(plugInInformation.briefDescription());   break; }
            }
        }
    } else if (role == Qt::TextAlignmentRole) {
        result = QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    } else if (role == Qt::FontRole) {
        unsigned row = static_cast<unsigned>(index.row());
        if (row < static_cast<unsigned>(allPlugIns.size())) {
            QFont font = Application::font();
            font.setItalic(allPlugIns.at(row).isThirdPartyPlugIn());
            result = QVariant(font);
        }
    }

    return result;
}


int PlugInTableModel::rowCount(const QModelIndex& /* parent */) const {
    return allPlugIns.size();
}


int PlugInTableModel::columnCount(const QModelIndex& /* parent */) const {
    return 5;
}


Ld::PlugInInformation PlugInTableModel::plugInInformation(const QModelIndex& index) const {
    unsigned row = static_cast<unsigned>(index.row());
    return row < static_cast<unsigned>(allPlugIns.size()) ? allPlugIns.at(row) : Ld::PlugInInformation();
}
