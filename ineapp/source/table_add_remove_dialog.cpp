/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableAddRemoveDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QLocale>
#include <QDialog>
#include <QString>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QSizePolicy>
#include <QPushButton>
#include <QDialogButtonBox>

#include <limits>

#include <eqt_programmatic_dialog.h>

#include "table_add_remove_dialog.h"

TableAddRemoveDialog::TableAddRemoveDialog(
        TableAddRemoveDialog::DialogMode dialogMode,
        QWidget*                         parent
    ):EQt::ProgrammaticDialog(
        "TableAddRemoveDialog",
        parent
    ) {
    configureWidget(dialogMode);
    runBuilders();
}


TableAddRemoveDialog::~TableAddRemoveDialog() {}


TableAddRemoveDialog::DialogMode TableAddRemoveDialog::dialogMode() const {
    return currentDialogMode;
}


unsigned TableAddRemoveDialog::numberEntries() const {
    QSpinBox* numberEntriesSpinBox = widget<QSpinBox>("number_entries_spin_box");
    return static_cast<unsigned>(numberEntriesSpinBox->value());
}


unsigned TableAddRemoveDialog::beforeMaximumNumberEntries() const {
    return inputBeforeMaximumNumberEntries;
}


unsigned TableAddRemoveDialog::afterMaximumNumberEntries() const {
    return inputAfterMaximumNumberEntries;
}


bool TableAddRemoveDialog::addBefore() const {
    QRadioButton* beforeRadioButton = widget<QRadioButton>("before_radio_button");
    return beforeRadioButton->isChecked();
}


bool TableAddRemoveDialog::addAfter() const {
    QRadioButton* afterRadioButton = widget<QRadioButton>("after_radio_button");
    return afterRadioButton->isChecked();
}


void TableAddRemoveDialog::populate() {
    QSpinBox*     numberEntriesSpinBox = widget<QSpinBox>("number_entries_spin_box");
    QRadioButton* beforeRadioButton    = widget<QRadioButton>("before_radio_button");
    QRadioButton* afterRadioButton     = widget<QRadioButton>("after_radio_button");

    numberEntriesSpinBox->setValue(inputNumberEntries);

    beforeRadioButton->setChecked(inputAddBefore);
    afterRadioButton->setChecked(!inputAddBefore);

    if (inputAddBefore) {
        numberEntriesSpinBox->setMaximum(inputBeforeMaximumNumberEntries);
    } else {
        numberEntriesSpinBox->setMaximum(inputAfterMaximumNumberEntries);
    }
}


void TableAddRemoveDialog::setNumberEntries(unsigned newNumberEntries) {
    inputNumberEntries = newNumberEntries;

    if (isVisible()) {
        populate();
    }
}


void TableAddRemoveDialog::setBeforeMaximumNumberEntries(unsigned newMaximumNumberEntries) {
    inputBeforeMaximumNumberEntries = newMaximumNumberEntries;

    if (isVisible()){
        populate();
    }
}


void TableAddRemoveDialog::setAfterMaximumNumberEntries(unsigned newMaximumNumberEntries) {
    inputAfterMaximumNumberEntries = newMaximumNumberEntries;

    if (isVisible()){
        populate();
    }
}


void TableAddRemoveDialog::setEntryAddedBefore(bool nowBefore) {
    inputAddBefore = nowBefore;

    if (isVisible()){
        populate();
    }

}


void TableAddRemoveDialog::setEntryAddedAfter(bool nowAfter) {
    setEntryAddedBefore(!nowAfter);
}


void TableAddRemoveDialog::bind() {
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    connect(okPushButton,     &QPushButton::clicked, this, &TableAddRemoveDialog::accept);
    connect(cancelPushButton, &QPushButton::clicked, this, &TableAddRemoveDialog::reject);

    QRadioButton* beforeRadioButton = widget<QRadioButton>("before_radio_button");
    QRadioButton* afterRadioButton  = widget<QRadioButton>("after_radio_button");

    connect(beforeRadioButton, &QRadioButton::clicked, this, &TableAddRemoveDialog::beforeClicked);
    connect(afterRadioButton,  &QRadioButton::clicked, this, &TableAddRemoveDialog::afterClicked);
}


void TableAddRemoveDialog::beforeClicked() {
    QSpinBox* numberEntriesSpinBox = widget<QSpinBox>("number_entries_spin_box");
    if (static_cast<unsigned>(numberEntriesSpinBox->value()) > inputBeforeMaximumNumberEntries) {
        numberEntriesSpinBox->setValue(inputBeforeMaximumNumberEntries);
    }

    numberEntriesSpinBox->setMaximum(inputBeforeMaximumNumberEntries);
}


void TableAddRemoveDialog::afterClicked() {
    QSpinBox* numberEntriesSpinBox = widget<QSpinBox>("number_entries_spin_box");
    if (static_cast<unsigned>(numberEntriesSpinBox->value()) > inputAfterMaximumNumberEntries) {
        numberEntriesSpinBox->setValue(inputAfterMaximumNumberEntries);
    }

    numberEntriesSpinBox->setMaximum(inputAfterMaximumNumberEntries);
}


void TableAddRemoveDialog::configureWidget(TableAddRemoveDialog::DialogMode dialogMode) {
    currentDialogMode = dialogMode;

    inputNumberEntries              = 1;
    inputAfterMaximumNumberEntries  = std::numeric_limits<int>::max();
    inputBeforeMaximumNumberEntries = std::numeric_limits<int>::max();
    inputAddBefore                  = true;

    QString windowTitle;
    QString numberEntriesTitle;
    QString beforeLabel;
    QString afterLabel;

    switch (dialogMode) {
        case DialogMode::ADD_COLUMNS: {
            windowTitle        = tr("Add Columns");
            numberEntriesTitle = tr("Number Columns To Add");
            beforeLabel        = tr("To left");
            afterLabel         = tr("To right");

            break;
        }

        case DialogMode::ADD_ROWS: {
            windowTitle        = tr("Add Rows");
            numberEntriesTitle = tr("Number Rows To Add");
            beforeLabel        = tr("Above");
            afterLabel         = tr("Below");

            break;
        }

        case DialogMode::REMOVE_COLUMNS: {
            windowTitle        = tr("Remove Columns");
            numberEntriesTitle = tr("Number Columns To Remove");
            beforeLabel        = tr("This column and columns to left");
            afterLabel         = tr("This column and columns to right");

            break;
        }

        case DialogMode::REMOVE_ROWS: {
            windowTitle        = tr("Remove Rows");
            numberEntriesTitle = tr("Number Rows To Remove");
            beforeLabel        = tr("This row and rows above");
            afterLabel         = tr("This row and rows below");

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    setWindowTitle(windowTitle);
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QGroupBox* numberEntriesGroupBox = new QGroupBox(numberEntriesTitle);
    registerWidget(numberEntriesGroupBox, "number_entries_group_box");
    mainLayout->addWidget(numberEntriesGroupBox);

    QVBoxLayout* numberEntriesLayout = newVBoxLayout("number_entries_layout");
    numberEntriesGroupBox->setLayout(numberEntriesLayout);

    QSpinBox* numberEntriesSpinBox = new QSpinBox;
    numberEntriesSpinBox->setMinimum(1);
    registerWidget(numberEntriesSpinBox, "number_entries_spin_box");
    numberEntriesLayout->addWidget(numberEntriesSpinBox);

    QGroupBox* locationGroupBox = new QGroupBox(tr("Location"));
    registerWidget(locationGroupBox, "location_group_box");
    mainLayout->addWidget(locationGroupBox);

    QBoxLayout* locationLayout = newVBoxLayout("location_layout");
    locationGroupBox->setLayout(locationLayout);

    QRadioButton* beforeRadioButton = new QRadioButton(beforeLabel);
    registerWidget(beforeRadioButton, "before_radio_button");
    locationLayout->addWidget(beforeRadioButton);

    QRadioButton* afterRadioButton = new QRadioButton(afterLabel);
    registerWidget(afterRadioButton, "after_radio_button");
    locationLayout->addWidget(afterRadioButton);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    mainLayout->addWidget(buttonBox);
}
