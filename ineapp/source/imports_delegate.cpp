/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImportsDelegate class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QWidget>
#include <QMap>
#include <QList>
#include <QComboBox>
#include <QVariant>
#include <QRect>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

#include <limits>

#include <ld_root_element.h>

#include "application.h"
#include "document_file_dialog.h"
#include "imports_delegate.h"

ImportsDelegate::ImportsDelegate(QObject* parent):QStyledItemDelegate(parent) {}


ImportsDelegate::~ImportsDelegate() {}


QWidget* ImportsDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const {
    QComboBox* editor = new QComboBox(parent);

    editor->setEditable(false);
    editor->setFrame(false);

    // Double clicking on the combo box is un-natural when in the QTableView.  This connection removes the need to
    // double click on the selection to select it.

    connect(
        editor,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
        this,
        &ImportsDelegate::selectionActivated
    );

    return editor;

}


void ImportsDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    unsigned column       = static_cast<unsigned>(index.column());
    QString  currentValue = index.model()->data(index, Qt::EditRole).toString();

    QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
    Q_ASSERT(comboBox != Q_NULLPTR);

    comboBox->clear();

    if (column == 0) {
        comboBox->addItem(tr("relative"));
        comboBox->addItem(tr("absolute"));

        comboBox->setCurrentText(currentValue);
    } else {
        IdentifiersByShortformName identifiersByShortformName = generateIdentifiersByShortformName();

        for (  IdentifiersByShortformName::const_iterator pos = identifiersByShortformName.constBegin(),
                                                          end = identifiersByShortformName.constEnd()
             ; pos != end
             ; ++pos
            ) {
            comboBox->addItem(pos.key(), QVariant::fromValue(pos.value()));
        }

        comboBox->addItem(tr("Select new file..."));
        comboBox->setCurrentText(currentValue);
    }
}


void ImportsDelegate::setModelData(
        QWidget*            editor,
        QAbstractItemModel* model,
        const QModelIndex&  index
    ) const {
    unsigned column = static_cast<unsigned>(index.column());

    QComboBox* comboBox = dynamic_cast<QComboBox*>(editor);
    Q_ASSERT(comboBox != Q_NULLPTR);

    if (column == 0) {
        QString text = comboBox->currentText();
        model->setData(index, text, Qt::EditRole);
    } else {
        QString text       = comboBox->currentText();
        QString identifier = comboBox->currentData().toString();
        bool    accepted;

        if (identifier.isEmpty()) {
            DocumentFileDialog* fileDialog = Application::documentFileDialog();
            fileDialog->configureForFileOpen();

            accepted = fileDialog->exec();

            if (accepted) {
                Q_ASSERT(fileDialog->selectedFiles().count() == 1);
                QString filename = fileDialog->selectedFiles().at(0);
                identifier = Ld::RootElement::identifier(filename);
            }
        } else {
            accepted = true;
        }

        if (accepted) {
            model->setData(index, identifier, Qt::EditRole);
        }
    }
};


void ImportsDelegate::updateEditorGeometry(
        QWidget*                    editor,
        const QStyleOptionViewItem& option,
        const QModelIndex&
    ) const {
    QRect editRectangle = option.rect;
    editor->setGeometry(editRectangle);
}


void ImportsDelegate::selectionActivated(int index) {
    QComboBox* editor = dynamic_cast<QComboBox*>(sender());
    Q_ASSERT(editor  != Q_NULLPTR);

    editor->setCurrentIndex(index);

    emit commitData(editor);
    emit closeEditor(editor);
}


ImportsDelegate::IdentifiersByShortformName ImportsDelegate::generateIdentifiersByShortformName() {
    IdentifiersByShortformName       identifiersByShortformName;
    Ld::RootElement::RootElementList rootElements = Ld::RootElement::rootElements();

    for (  Ld::RootElement::RootElementList::const_iterator it  = rootElements.constBegin(),
                                                            end = rootElements.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::RootElement> element       = *it;
        QString                         identifier    = element->identifier();
        QString                         shortformName = element->shortformName(std::numeric_limits<unsigned>::max());

        identifiersByShortformName.insert(shortformName, identifier);
    }

    return identifiersByShortformName;
}
