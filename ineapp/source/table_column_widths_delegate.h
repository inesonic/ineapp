/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableColumnWidthsDelegate class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_COLUMN_WIDTHS_DELEGATE_H
#define TABLE_COLUMN_WIDTHS_DELEGATE_H

#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QRegularExpression>
#include <QStyledItemDelegate>

class QWidget;

/**
 * Class that provides a delegate for the table column widths widget.  The delegate provides an editor with a validator
 * that limits user input to a controlled range of values.
 */
class TableColumnWidthsDelegate:public QStyledItemDelegate {
    Q_OBJECT

    public:
        /**
         * The validator regular expression.  The regular expression includes two ranges you can use to extract the
         * numerical value and optional units value.
         */
        static const QRegularExpression inputRegularExpression;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this delegate.
         */
        TableColumnWidthsDelegate(QObject* parent = Q_NULLPTR);

        ~TableColumnWidthsDelegate() override;

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
};

#endif
