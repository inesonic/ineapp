/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableColumnWidthsDelegate class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QObject>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSharedPointer>
#include <QString>
#include <QWidget>
#include <QMap>
#include <QList>
#include <QComboBox>
#include <QVariant>
#include <QRect>
#include <QStyledItemDelegate>

#include "application.h"
#include "table_column_widths_delegate.h"

const QRegularExpression TableColumnWidthsDelegate::inputRegularExpression(
    QT_TR_NOOP(
        "^([0-9]*\\.?[0-9]*)\\s*"
        "(mm|cm|m|meter|meters|pt|point|points|pc|pica|picas|in|inch|inches|\"|ft|foot|feet|%)?$"
    )
);

TableColumnWidthsDelegate::TableColumnWidthsDelegate(QObject* parent):QStyledItemDelegate(parent) {}


TableColumnWidthsDelegate::~TableColumnWidthsDelegate() {}


QWidget* TableColumnWidthsDelegate::createEditor(
        QWidget*                    parent,
        const QStyleOptionViewItem&,
        const QModelIndex&
    ) const {
    QLineEdit*                   editor    = new QLineEdit(parent);
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(inputRegularExpression, editor);

    editor->setValidator(validator);

    return editor;

}


void TableColumnWidthsDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    QString    currentValue = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit* lineEditor   = dynamic_cast<QLineEdit*>(editor);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->setText(currentValue);
}


void TableColumnWidthsDelegate::setModelData(
        QWidget*            editor,
        QAbstractItemModel* model,
        const QModelIndex&  index
    ) const {
    QLineEdit* lineEditor   = dynamic_cast<QLineEdit*>(editor);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    QString text = lineEditor->text();
    model->setData(index, text, Qt::EditRole);
};


void TableColumnWidthsDelegate::updateEditorGeometry(
        QWidget*                    editor,
        const QStyleOptionViewItem& option,
        const QModelIndex&
    ) const {
    QRect editRectangle = option.rect;
    editor->setGeometry(editRectangle);
}
