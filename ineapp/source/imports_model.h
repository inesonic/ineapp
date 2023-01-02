/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImportsModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMPORTS_MODEL_H
#define IMPORTS_MODEL_H

#include <QtGlobal>
#include <QString>
#include <QVariant>
#include <QList>
#include <QModelIndex>
#include <QAbstractTableModel>

#include <ld_root_import.h>

/**
 * Class that provides a model for the table view widget in the imports dialog.
 */
class ImportsModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this model.
         */
        ImportsModel(QObject* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] newReferencePath The reference path used for relative references.
         *
         * \param[in] newImports       List of imports to be managed by this model.
         *
         * \param[in] parent           The parent object for this window.
         */
        ImportsModel(
            const QString&               newReferencePath,
            const QList<Ld::RootImport>& newImports,
            QObject*                     parent = Q_NULLPTR
        );

        ~ImportsModel() override;

        /**
         * Method you can use to set the reference path used for relative references.
         *
         * \param[in] newReferencePath The reference path to use to locate relative references.
         */
        void setReferencePath(const QString& newReferencePath);

        /**
         * Method you can use to obtain the reference path used for relative references.
         *
         * \return Returns the reference path used for relative references.
         */
        QString referencePath() const;

        /**
         * Method you can use to update the list of imports.
         *
         * \param[in] newImports The new list of imports to be tracked by this model.
         */
        void setImports(const QList<Ld::RootImport>& newImports);

        /**
         * Method you can use to obtain the current list of imports.
         *
         * \return Returns the current list of imports.
         */
        QList<Ld::RootImport> imports() const;

        /**
         * Method used by the view to determine what operations can be performed on the cell.
         *
         * \param[in] index The index into the model used to locate the cell.
         *
         * \return Returns flags indicating what operations are acceptable.
         */
        Qt::ItemFlags flags(const QModelIndex& index) const final;

        /**
         * Method used by the view to obtain the data to be displayed.
         *
         * \param[in] index The index into the model used to locate the data.
         *
         * \param[in] role  The display role for the data.
         *
         * \return Returns a variant type holding the data to be displayed.
         */
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const final;

        /**
         * Method used by the view to update data in the model.
         *
         * \param[in] index The index into the model used to point to the location of the data.
         *
         * \param[in] value The new data for the cell.
         *
         * \param[in] role  The role associated with the newly edited data.
         *
         * \return Returns true if the entry was successfully updated.  Returns false if the cell could not be updated.
         */
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) final;

        /**
         * Method used by the view to obtain headers to be displayed.
         *
         * \param[in] section     The section number for the header.
         *
         * \param[in] orientation The orientation of the headers being displayed.
         *
         * \param[in] role        The role for the displayed data.
         *
         * \return Returns the data for the header.
         */
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;

        /**
         * Method used by the view to obtain the number of columns to be displayed.
         *
         * \return Returns the number of displayable columns.
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const final;

        /**
         * Method used by the view to obtain the number of rows to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const final;

    signals:
        /**
         * Signal that is emitted when the model is unable to access a program file.
         *
         * \param[in] identifier The identifier of the file to be opened.
         */
        void failedToAccess(const QString& identifier);

    private:
        /**
         * Method that performs initialziation that is common to all constructors.
         */
        void configure();

        /**
         * The reference path used for relative references.
         */
        QString currentReferencePath;

        /**
         * The current/dynamic list of imports.
         */
        QList<Ld::RootImport> currentImports;
};

#endif
