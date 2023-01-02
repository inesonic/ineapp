/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FunctionBrowserModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FUNCTION_BROWSER_MODEL_H
#define FUNCTION_BROWSER_MODEL_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QVariant>
#include <QFont>
#include <QModelIndex>
#include <QAbstractItemModel>

#include <ld_capabilities.h>
#include <ld_data_type.h>
#include <ld_function_data.h>

class QItemSelectionModel;

/**
 * Class that provides a model for the function browser.  The class creates a model that provides both the function
 * names and descriptions.
 */
class FunctionBrowserModel:public QAbstractItemModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        FunctionBrowserModel(QObject* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] functionFont    The font to be used for the function names.
         *
         * \param[in] descriptionFont The font to be used for the description fields.
         *
         * \param[in] parent          The parent object for this menu.
         */
        FunctionBrowserModel(const QFont& functionFont, const QFont& descriptionFont, QObject* parent = Q_NULLPTR);

        ~FunctionBrowserModel() override;

        /**
         * Method you can use to obtain a list of all the functions presented in the model view.
         *
         * \return Returns a reference to a list of function instances.
         */
        const QList<Ld::FunctionData>& presentedFunctions() const;

        /**
         * Method you can use to obtain a list of the active capabilities in the search.
         *
         * \return Returns the active capabilities in the search.
         */
        Ld::Capabilities capabilities() const;

        /**
         * Method you can use to obtain a list of all function categories.
         *
         * \return Returns a list of all function categories.
         */
        QList<QString> functionCategories() const;

        /**
         * Method you can use to obtain a list of all selected function categories.
         *
         * \return Returns a list of all selected function categories.
         */
        const QList<QString>& selectedFunctionCategories() const;

        /**
         * Method you can use to obtain the current function search string.
         *
         * \return Returns the current function search string.
         */
        const QString& searchString() const;

        /**
         * Method you can use to obtain the current function font.
         *
         * \return Returns the current function font.
         */
        const QFont& functionFont() const;

        /**
         * Method you can use to obtain the current description font.
         *
         * \return Returns the current description font.
         */
        const QFont& descriptionFont() const;

        /**
         * The recommended width for column 1, in pixels.
         *
         * \return Returns the recommended width for column 1.
         */
        unsigned recommendedColumn1Width();

        /**
         * The recommended width for column 2, in pixels.
         *
         * \return Returns the recommended width for column 2.
         */
        unsigned recommendedColumn2Width();

        /**
         * The recommended row height, in pixels.
         *
         * \return Returns the recommended row height in pixels.
         */
        unsigned recommendedRowHeight();

        /**
         * Method you can use to obtain a function data instance and variant from a selection instance.
         *
         * \param[in] modelIndex            The model index used to obtain a function data instance and variant.
         *
         * \param[in] functionData          The function data instance.
         *
         * \param[in] parameterDescriptions A list of selected parameter descriptions.
         *
         * \return Returns true if the selection data points to a valid function variant.  Returns false if the
         *         selection data does not point to a valid function variant.
         */
        bool identifyFunction(
            const QModelIndex& modelIndex,
            Ld::FunctionData&  functionData,
            QStringList&       parameterDescriptions
        ) const;

        /**
         * Method used by the view to obtain the index of an item based on a row, column, and parent.
         *
         * \param[in] row    The desired row.
         *
         * \param[in] column The desired column.
         *
         * \param[in] parent The index of the parent in question.
         *
         * \return Returns a model index for the desired position.
         */
        QModelIndex index(int row, int column, const QModelIndex& parent) const override;

        /**
         * Method used by the view to obtain the parent index of an item.
         *
         * \param[in] index The index of the child in question.
         *
         * \return Returns a model index for the parent.
         */
        QModelIndex parent(const QModelIndex& index) const override;

        /**
         * Method used by the view to obtain the header data to be displayed.
         *
         * \param[in] section     The column or row number.
         *
         * \param[in] orientation The orientation requiring header data.
         *
         * \param[in] role        The desired display role.
         *
         * \return Returns a variant type holding the data to be displayed.
         */
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

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
         * Method used by the view to obtain the number of columns to be displayed.
         *
         * \return Returns the number of displayable columns.
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;

        /**
         * Method used by the view to obtain the number of rows to be displayed.
         *
         * \return Returns the number of displayable rows.
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    signals:
        /**
         * Signal that is emitted when the recommended table geometry changes.
         *
         * \param[out] model Pointer to the model that triggered the geometry change.
         */
        void recommendedGeometryChanged(FunctionBrowserModel* model);

    public slots:
        /**
         * Slot you can trigger to update the current seleciton capabilities.
         *
         * \param[in] newCapabilities The new capabilities.  An empty capability field will cause all functions to
         *                            be included.
         */
        void setCapabilities(const Ld::Capabilities& newCapabilities);

        /**
         * Slot you can trigger to update the list of selected categories.
         *
         * \param[in] categoryList The list of selected categories.
         */
        void setSelectedCategories(const QList<QString>& categoryList);

        /**
         * Slot you can trigger to update the selection string.
         *
         * \param[in] selectionString The selection string.
         */
        void setSelectionString(const QString& selectionString);

    private:
        /**
         * Method that calculates recommended column widths.
         */
        void calculateRecommendedGeometryData();

        /**
         * Method that calculates all the variants of a given function.
         *
         * \param[in] functionData The function data to calculate the variants for.
         *
         * \return Returns a list of all the variant descriptions.
         */
        static QList<QStringList> variants(const Ld::FunctionData& functionData);

        /**
         * Method that provides a string description of a type.
         *
         * \param[in] valueType The value type to obtain a string description for.
         *
         * \return Returns a string description of the value type.
         */
        static QString toString(Ld::DataType::ValueType valueType);

        /**
         * The currently enabled capabilities.
         */
        Ld::Capabilities currentCapabilities;

        /**
         * The currently selected function categories.
         */
        QList<QString> currentSelectedCategories;

        /**
         * The current search string.
         */
        QString currentSearchString;

        /**
         * The current function font.
         */
        QFont currentFunctionFont;

        /**
         * The current description font.
         */
        QFont currentDescriptionFont;

        /**
         * The current list of functions.
         */
        QList<Ld::FunctionData> currentFunctionList;

        /**
         * Flag that indicates if geometry data needs to be updated.
         */
        bool geometryUpdateNeeded;

        /**
         * The recommended column 1 width.
         */
        unsigned currentColumn1Width;

        /**
         * The recommended column 2 width.
         */
        unsigned currentColumn2Width;

        /**
         * The recommended row height, in pixels.
         */
        unsigned currentRowHeight;
};

#endif
