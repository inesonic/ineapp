/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref RealDelegate class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "app_common.h"
#include "real_delegate.h"

RealDelegate::RealDelegate(QObject* parent):QStyledItemDelegate(parent) {}


RealDelegate::~RealDelegate() {}


QWidget* RealDelegate::createEditor(
        QWidget*                    parent,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex&          /* index */
    ) const {
    QLineEdit*        lineEditor = new QLineEdit(parent);
    QDoubleValidator* validator  = new QDoubleValidator(lineEditor);
    lineEditor->setValidator(validator);

    return lineEditor;
}


void RealDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QLineEdit* lineEditor = dynamic_cast<QLineEdit*>(editor);

    QString value = index.model()->data(index, Qt::EditRole).toString();
    lineEditor->setText(value);
}


void RealDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QLineEdit* lineEditor = dynamic_cast<QLineEdit*>(editor);
    QString    text       = lineEditor->text();

    model->setData(index, text, Qt::EditRole);
}


void RealDelegate::updateEditorGeometry(
        QWidget*                    editor,
        const QStyleOptionViewItem& option,
        const QModelIndex&          /* index */
    ) const {
    editor->setGeometry(option.rect);
}
