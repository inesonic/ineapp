/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImportsDelegate class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMPORTS_DELEGATE_H
#define IMPORTS_DELEGATE_H

#include <QMap>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

class QWidget;

/**
 * Class that provides a delegate for the imports dialog.
 */
class ImportsDelegate:public QStyledItemDelegate {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this delegate.
         */
        ImportsDelegate(QObject* parent = Q_NULLPTR);

        ~ImportsDelegate() override;

        /**
         * Method called by the view to create an editor suitable for a given cell.
         *
         * \param[in] parent Pointer to the parent to assign to the newly created editor widget.
         *
         * \param[in] option Style options that can be used to adjust the look of the returned widget.
         *
         * \param[in] index  Index to the cell requiring the newly created editor.
         *
         * \return Returns a newly created widget that can be used to edit the specified cell.
         */
        QWidget* createEditor(
            QWidget*                    parent,
            const QStyleOptionViewItem& option,
            const QModelIndex&          index
        ) const final;

        /**
         * Method called by the view to populate the editor with data for the user.
         *
         * \param[in] editor The editor to populate with data.
         *
         * \param[in] index  Index into the model to contain the requested data.
         */
        void setEditorData(QWidget* editor, const QModelIndex& index) const final;

        /**
         * Method called by the view to populate the model with data provided by the editor.
         *
         * \param[in] editor The editor to populate with data.
         *
         * \param[in] model  The model to be used to populate the editor data with.
         *
         * \param[in] index  Index into the model to contain the requested data.
         */
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const final;

        /**
         * Method that is called by the view to setup the geometry of the editor.
         *
         * \param[in] editor The editor to have its geometry adjusted.
         *
         * \param[in] option Style options that can be used to adjust the look of the returned widget.  Note that this
         *                   value includes the rectangle to contain the editor.
         *
         * \param[in] index  Index to the cell requiring the newly created editor.
         */
        void updateEditorGeometry(
            QWidget*                    editor,
            const QStyleOptionViewItem& option,
            const QModelIndex&          index
        ) const final;

    private slots:
        /**
         * Slot that receives notification from an editor indicating that the user has made a selection.
         *
         * Note that this method relies on the use of QObject::sender.  Ideally we should probably have a class to
         * marshall the signal and provide the editor as a parameter.
         *
         * \param[in] index The index of the user's selection.
         */
        void selectionActivated(int index);

    private:
        /**
         * Type used for the map of root elements by identifier.
         */
        typedef QMap<QString, QString> IdentifiersByShortformName;

        /**
         * Method called to update the list of identifiers by shortform name.
         *
         * \return Returns a map of identifiers by shortform name.
         */
        static IdentifiersByShortformName generateIdentifiersByShortformName();
};

#endif
