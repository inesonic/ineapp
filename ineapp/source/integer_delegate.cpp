/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref IntegerDelegate class.
***********************************************************************************************************************/

#include <QString>
#include <QVariant>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include "app_common.h"
#include "integer_delegate.h"

const QRegularExpression IntegerDelegate::regularExpression(
    "(0|[+-]?[1-9][0-9]*|0b[01][01]*|0x[0-9a-fA-F][0-9a-fA-F]*|'[bB][01][01]*|'[hH][0-9a-fA-F][0-9a-fA-F]*)"
);

IntegerDelegate::IntegerDelegate(QObject* parent):QStyledItemDelegate(parent) {}


IntegerDelegate::~IntegerDelegate() {}


QWidget* IntegerDelegate::createEditor(
        QWidget*                    parent,
        const QStyleOptionViewItem& /* option */,
        const QModelIndex&          /* index */
    ) const {
    QLineEdit*                   lineEditor = new QLineEdit(parent);
    QRegularExpressionValidator* validator  = new QRegularExpressionValidator(regularExpression, lineEditor);
    lineEditor->setValidator(validator);

    return lineEditor;
}


void IntegerDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QLineEdit* lineEditor = dynamic_cast<QLineEdit*>(editor);

    QString value = index.model()->data(index, Qt::EditRole).toString();
    lineEditor->setText(value);
}


void IntegerDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    QLineEdit* lineEditor = dynamic_cast<QLineEdit*>(editor);
    QString    text       = lineEditor->text();

    model->setData(index, text, Qt::EditRole);
}


void IntegerDelegate::updateEditorGeometry(
        QWidget*                    editor,
        const QStyleOptionViewItem& option,
        const QModelIndex&          /* index */
    ) const {
    editor->setGeometry(option.rect);
}
