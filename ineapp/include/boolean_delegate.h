/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BooleanDelegate class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BOOLEAN_DELEGATE_H
#define BOOLEAN_DELEGATE_H

#include <QString>
#include <QVariant>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QAbstractItemModel>

#include "app_common.h"

/**
 * Class that provides a delegate for boolean values.
 */
class APP_PUBLIC_API BooleanDelegate:public QStyledItemDelegate {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        BooleanDelegate(QObject* parent = Q_NULLPTR);

        ~BooleanDelegate() override;

        /**
         * Method that creates an editor object.
         *
         * \param[in] parent The widget that should be the parent of the editor.
         *
         * \param[in] option Style options to apply to the editor being created.
         *
         * \param[in] index  Index indicating where the editor is to be used.
         *
         * \return Returns a pointer to the newly created editor.
         */
        QWidget* createEditor(
            QWidget*                    parent,
            const QStyleOptionViewItem& option,
            const QModelIndex&          index
        ) const override;

        /**
         * Method that configures an editor.
         *
         * \param[in,out] editor The editor to be configured.
         *
         * \param[in]     index  The index indicating where the data is to be configured.
         */
        void setEditorData(QWidget* editor, const QModelIndex& index) const override;

        /**
         * Method that updates a model from an editor.
         *
         * \param[in]     editor The editor holding the data used to configure the model.
         *
         * \param[in,out] model  The model to receive the data.
         *
         * \param[in]     index  Index indicating the location
         */
        void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

        /**
         * Method that updates the geometry for the editor.
         *
         * \param[in,out] editor
         *
         * \param[in]     option
         *
         * \param[in]     index
         */
        void updateEditorGeometry(
            QWidget*                    editor,
            const QStyleOptionViewItem& option,
            const QModelIndex&          index
        ) const override;
};

#endif
