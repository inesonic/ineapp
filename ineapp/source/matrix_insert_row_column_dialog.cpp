/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixInsertRowColumnDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QRadioButton>
#include <QSizePolicy>
#include <QPushButton>
#include <QDialogButtonBox>

#include "matrix_insert_row_column_dialog.h"

MatrixInsertRowColumnDialog::MatrixInsertRowColumnDialog(Orientation orientation, QWidget* parent):QDialog(parent) {
    setWindowTitle(tr("FIXME"));
    setSizeGripEnabled(false);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QFormLayout* countLayout = new QFormLayout;
    mainLayout->addLayout(countLayout);

    countLabel = new QLabel(tr("FIXME"));

    countSpinBox = new QSpinBox;
    countSpinBox->setMinimum(1);

    countLayout->addRow(countLabel, countSpinBox);

    beforeRadioButton = new QRadioButton(tr("Add rows above"));
    mainLayout->addWidget(beforeRadioButton);

    afterRadioButton = new QRadioButton(tr("Add rows below"));
    mainLayout->addWidget(afterRadioButton);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    setOrientation(orientation);
    setLocation(Location::BEFORE);
    setCount(1);

    connect(
        buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &MatrixInsertRowColumnDialog::accept
    );

    connect(
        buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &MatrixInsertRowColumnDialog::reject
    );
}


MatrixInsertRowColumnDialog::~MatrixInsertRowColumnDialog() {}


MatrixInsertRowColumnDialog::Orientation MatrixInsertRowColumnDialog::orientation() const {
    Orientation result;

    if (windowTitle() == tr("Insert Rows")) {
        result = Orientation::ROWS;
    } else {
        result = Orientation::COLUMNS;
    }

    return result;
}


MatrixInsertRowColumnDialog::Location MatrixInsertRowColumnDialog::location() const {
    Location result;

    if (beforeRadioButton->isChecked()) {
        result = Location::BEFORE;
    } else {
        result = Location::AFTER;
    }

    return result;
}


unsigned MatrixInsertRowColumnDialog::count() const {
    return static_cast<unsigned>(countSpinBox->value());
}


void MatrixInsertRowColumnDialog::setOrientation(MatrixInsertRowColumnDialog::Orientation newOrientation) {
    if (newOrientation == Orientation::ROWS) {
        setWindowTitle(tr("Insert Rows"));
        countLabel->setText(tr("Number rows to insert: "));
        beforeRadioButton->setText(tr("Insert above"));
        afterRadioButton->setText(tr("Insert below"));
    } else {
        setWindowTitle(tr("Insert Columns"));
        countLabel->setText(tr("Number columns to insert: "));
        beforeRadioButton->setText(tr("Insert to left"));
        afterRadioButton->setText(tr("Insert to right"));
    }
}


void MatrixInsertRowColumnDialog::setLocation(MatrixInsertRowColumnDialog::Location newLocation) {
    if (newLocation == Location::BEFORE) {
        beforeRadioButton->setChecked(true);
    } else {
        afterRadioButton->setChecked(true);
    }
}


void MatrixInsertRowColumnDialog::setCount(unsigned newCount) {
    countSpinBox->setValue(newCount);
}
