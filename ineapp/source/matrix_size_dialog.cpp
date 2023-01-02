/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixSizeDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QSizePolicy>
#include <QPushButton>
#include <QDialogButtonBox>

#include "matrix_size_dialog.h"

MatrixSizeDialog::MatrixSizeDialog(QWidget* parent):QDialog(parent) {
    setWindowTitle(tr("Add Matrix"));
    setSizeGripEnabled(false);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QFormLayout* controlsLayout = new QFormLayout;
    mainLayout->addLayout(controlsLayout);

    rowsSpinBox = new QSpinBox;
    rowsSpinBox->setMinimum(1);
    rowsSpinBox->setValue(1);

    columnsSpinBox = new QSpinBox;
    columnsSpinBox->setMinimum(1);
    columnsSpinBox->setValue(1);

    controlsLayout->addRow(tr("Rows: "), rowsSpinBox);
    controlsLayout->addRow(tr("Columns: "), columnsSpinBox);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mainLayout->addWidget(buttonBox);

    connect(
        buttonBox,
        &QDialogButtonBox::accepted,
        this,
        &MatrixSizeDialog::accept
    );

    connect(
        buttonBox,
        &QDialogButtonBox::rejected,
        this,
        &MatrixSizeDialog::reject
    );
}


MatrixSizeDialog::~MatrixSizeDialog() {}


unsigned MatrixSizeDialog::numberRows() const {
    return static_cast<unsigned>(rowsSpinBox->value());
}


unsigned MatrixSizeDialog::numberColumns() const {
    return static_cast<unsigned>(columnsSpinBox->value());
}


void MatrixSizeDialog::setNumberRows(unsigned newNumberRows) {
    rowsSpinBox->setValue(newNumberRows);
}


void MatrixSizeDialog::setNumberColumns(unsigned newNumberColumns) {
    columnsSpinBox->setValue(newNumberColumns);
}
