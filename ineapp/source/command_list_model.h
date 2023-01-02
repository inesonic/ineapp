/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CommandListModel class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_LIST_MODEL_H
#define COMMAND_LIST_MODEL_H

#include <QString>
#include <QVariant>
#include <QMap>
#include <QFont>
#include <QModelIndex>
#include <QAbstractTableModel>

/**
 * Class that provides a model for the CommandLineEdit.  The class creates a model that provides both the commands and
 * help test.
 */
class CommandListModel:public QAbstractTableModel {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        CommandListModel(QObject* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] map    Pointer to the map tied to this model.
         *
         * \param[in] parent The parent object for this window.
         */
        CommandListModel(const QMap<QString, QString>* map, QObject* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] font   The font to be used for the list contents.
         *
         * \param[in] parent The parent object for this menu.
         */
        CommandListModel(const QFont& font, QObject* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] map    Pointer to the map tied to this model.
         *
         * \param[in] font   The font to be used for the list contents.
         *
         * \param[in] parent The parent object for this window.
         */
        CommandListModel(const QMap<QString, QString>* map, const QFont& font, QObject* parent = Q_NULLPTR);

        ~CommandListModel() override;

        /**
         * Method you can use to set the map containing the data handled by this model.  Note that this model assumes
         * that the map is invariant.  You should call this method whenever the map is changed.
         *
         * \param[in] newMap Pointer to the map to tie to this model.
         */
        void setMap(const QMap<QString, QString>* newMap);

        /**
         * Method you can use to obtain the map containing the data handled by this model.
         *
         * \return Returns a pointer to the map used by this model.
         */
        const QMap<QString, QString>* map() const;

        /**
         * Method you can use to specify the prefix to be placed in front of the user's command.
         *
         * \param[in] newPrefix The prefix to be placed in front of the user's command.  The value will be displayed but
         *                      will not be included in the command string.
         */
        void setPrefix(const QString& newPrefix);

        /**
         * Method you can use to obtain the current prefix.
         *
         * \return Returns the prefix to be placed in front of the user's command.  The value will be displayed but will
         *         not be included in the command string.
         */
        QString prefix() const;

        /**
         * Method you can use to set the font to use for the model's text.
         *
         * \param[in] newFont The font to be used.
         */
        void setFont(const QFont& newFont);

        /**
         * Method you can use to query the font being used for the model's text.
         *
         * \return Returns the font being used.
         */
        QFont font() const;

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
         * Signal that is emitted when the recommended table geometry changes.
         *
         * \param[out] model Pointer to the model that triggered the geometry change.
         */
        void recommendedGeometryChanged(CommandListModel* model);

    private:
        /**
         * Calculates recommended column widths.
         */
        void calculateRecommendedGeometryData();

        /**
         * Map containing the data to be displayed.
         */
        const QMap<QString, QString>* currentMap;

        /**
         * Prefix to be placed in front of each entry in column 1.
         */
        QString currentPrefix;

        /**
         * Font to be used for the model's content.
         */
        QFont currentFont;

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
