/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref BooleanDelegate class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QComboBox>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "app_common.h"
#include "boolean_delegate.h"

BooleanDelegate::BooleanDelegate(QObject* parent):QStyledItemDelegate(parent) {}


BooleanDelegate::~BooleanDelegate() {}


QWidget* BooleanDelegate::createEditor(
        QWidget*                    parent,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex&          /* index */
    ) const {
    QComboBox* comboBox = new QComboBox(parent);
    comboBox->setEditable(true);
    comboBox->setFrame(false);
    comboBox->addItem(tr("false"), QVariant(false));
    comboBox->addItem(tr("true"),  QVariant(true));

    return comboBox;
}


void BooleanDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);

    QString value = index.model()->data(index, Qt::EditRole).toString();
    comboBox->setCurrentText(value);
}


void BooleanDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
    QString    text     = comboBox->currentText();

    model->setData(index, text, Qt::EditRole);
}


void BooleanDelegate::updateEditorGeometry(
        QWidget*                    editor,
        const QStyleOptionViewItem& option,
        const QModelIndex&          /* index */
    ) const {
    editor->setGeometry(option.rect);
}
