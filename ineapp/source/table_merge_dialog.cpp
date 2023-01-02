/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableMergeDialog class.
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

#include "table_merge_dialog.h"

TableMergeDialog::TableMergeDialog(
        QWidget* parent
    ):EQt::ProgrammaticDialog(
        "TableMergeDialog",
        parent
    ) {
    configureWidget();
    runBuilders();
}


TableMergeDialog::TableMergeDialog(
        unsigned maximumRowSpan,
        unsigned maximumColumnSpan,
        QWidget* parent
    ):EQt::ProgrammaticDialog(
        "TablemergeDialog",
        parent
    ) {
    configureWidget();
    runBuilders();

    inputMaximumRowSpan    = maximumRowSpan;
    inputMaximumColumnSpan = maximumColumnSpan;
}


TableMergeDialog::~TableMergeDialog() {}


unsigned TableMergeDialog::rowSpan() const {
    QSpinBox* rowSpinBox = widget<QSpinBox>("row_spin_box");
    return static_cast<unsigned>(rowSpinBox->value());
}


unsigned TableMergeDialog::columnSpan() const {
    QSpinBox* columnSpinBox = widget<QSpinBox>("column_spin_box");
    return static_cast<unsigned>(columnSpinBox->value());
}


unsigned TableMergeDialog::maximumRowSpan() const {
    QSpinBox* rowSpinBox = widget<QSpinBox>("row_spin_box");
    return static_cast<unsigned>(rowSpinBox->maximum());
}


unsigned TableMergeDialog::maximumColumnSpan() const {
    QSpinBox* columnSpinBox = widget<QSpinBox>("column_spin_box");
    return static_cast<unsigned>(columnSpinBox->maximum());
}


void TableMergeDialog::populate() {
    unsigned forcedRowSpan           = inputRowSpan;
    unsigned forcedColumnSpan        = inputColumnSpan;
    unsigned forcedMinimumRowSpan    = 1;
    unsigned forcedMinumumColumnSpan = 1;

    if (inputMaximumRowSpan == 1) {
        forcedMinumumColumnSpan = 2;
        Q_ASSERT(inputMaximumColumnSpan > 1);

        if (inputColumnSpan < 2) {
            forcedColumnSpan = 2;
        }
    } else if (inputMaximumColumnSpan == 1) {
        forcedMinimumRowSpan = 2;

        if (inputRowSpan < 2) {
            forcedRowSpan = 2;
        }
    }

    QSpinBox* rowSpinBox    = widget<QSpinBox>("row_spin_box");
    QSpinBox* columnSpinBox = widget<QSpinBox>("column_spin_box");

    rowSpinBox->setMinimum(forcedMinimumRowSpan);
    rowSpinBox->setMaximum(inputMaximumRowSpan);
    rowSpinBox->setValue(forcedRowSpan);

    columnSpinBox->setMinimum(forcedMinumumColumnSpan);
    columnSpinBox->setMaximum(inputMaximumColumnSpan);
    columnSpinBox->setValue(forcedColumnSpan);
}


void TableMergeDialog::setRowSpan(unsigned newRowSpan) {
    inputRowSpan = newRowSpan;

    if (isVisible()) {
        populate();
    }
}


void TableMergeDialog::setColumnSpan(unsigned newColumnSpan) {
    inputColumnSpan = newColumnSpan;

    if (isVisible()) {
        populate();
    }
}


void TableMergeDialog::setMaximumRowSpan(unsigned newMaximumRowSpan) {
    inputMaximumRowSpan = newMaximumRowSpan;

    if (isVisible()) {
        populate();
    }
}


void TableMergeDialog::setMaximumColumnSpan(unsigned newMaximumColumnSpan) {
    inputMaximumColumnSpan = newMaximumColumnSpan;

    if (isVisible()) {
        populate();
    }
}


void TableMergeDialog::bind() {
    QSpinBox* rowSpinBox    = widget<QSpinBox>("row_spin_box");
    QSpinBox* columnSpinBox = widget<QSpinBox>("column_spin_box");

    connect(
        rowSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &TableMergeDialog::rowSpanChanged
    );
    connect(
        columnSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &TableMergeDialog::columnSpanChanged
    );

    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    connect(okPushButton,     &QPushButton::clicked, this, &TableMergeDialog::accept);
    connect(cancelPushButton, &QPushButton::clicked, this, &TableMergeDialog::reject);
}


void TableMergeDialog::rowSpanChanged(int newValue) {
    if (newValue == 1) {
        QSpinBox* columnSpinBox = widget<QSpinBox>("column_spin_box");
        if (columnSpinBox->value() == 1) {
            columnSpinBox->setValue(2);
        }
    }
}


void TableMergeDialog::columnSpanChanged(int newValue) {
    if (newValue == 1) {
        QSpinBox* rowSpinBox = widget<QSpinBox>("row_spin_box");
        if (rowSpinBox->value() == 1) {
            rowSpinBox->setValue(2);
        }
    }
}


void TableMergeDialog::configureWidget() {
    inputRowSpan           = 2;
    inputColumnSpan        = 2;
    inputMaximumRowSpan    = std::numeric_limits<unsigned>::max();
    inputMaximumColumnSpan = std::numeric_limits<unsigned>::max();

    setWindowTitle(tr("Merge Cells"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QFormLayout* formLayout = newFormLayout("form_layout");
    mainLayout->addLayout(formLayout);

    QSpinBox* rowSpinBox = new QSpinBox;
    registerWidget(rowSpinBox, "row_spin_box");
    formLayout->addRow(tr("Row Span: "), rowSpinBox);

    QSpinBox* columnSpinBox = new QSpinBox;
    registerWidget(columnSpinBox, "column_spin_box");
    formLayout->addRow(tr("Column Span: "), columnSpinBox);

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
