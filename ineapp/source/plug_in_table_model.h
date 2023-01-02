/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlugInTableModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLUG_IN_TABLE_MODEL_H
#define PLUG_IN_TABLE_MODEL_H

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QAbstractTableModel>

#include "ld_plug_in_information.h"

#include "app_common.h"

/**
 * Class that provides a table model for a table of plug-ins.
 */
class APP_PUBLIC_API PlugInTableModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        PlugInTableModel(QObject* parent = Q_NULLPTR);

        ~PlugInTableModel() override;

        /**
         * Method used by the view to obtain the header names.
         *
         * \param[in] section     The index into the header.
         *
         * \param[in] orientation The orientation of the header we need data for.
         *
         * \param[in] role        The role used to indicate what data is needed.
         *
         * \return Returns a variant holding the requested data.
         */
        QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

        /**
         * Method used by the view to obtain the data to be displayed.
         *
         * \param[in] index The index into the model used to locate the data.
         *
         * \param[in] role  The display role for the data.
         *
         * \return Returns a variant type holding the data to be displayed.
         */
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        /**
         * Method used by the view to obtain the number of rows to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method used by the view to obtain the number of columns to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method you can use to get the plug-in information associated with an index.
         *
         * \param[in] index The index containing the desired plug-in information.
         *
         * \return Returns a reference to a plug-in information instance associated with the index.
         */
        Ld::PlugInInformation plugInInformation(const QModelIndex& index) const;

    private:
        /**
         * List of plug-ins.
         */
        QList<Ld::PlugInInformation> allPlugIns;
};

#endif
