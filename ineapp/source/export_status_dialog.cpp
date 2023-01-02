/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ExportStatusDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QSizePolicy>
#include <QString>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QVBoxLayout>

#include "export_status_dialog.h"

ExportStatusDialog::ExportStatusDialog(QWidget* parent):QDialog(parent) {
    setWindowTitle(tr("Exporting ???"));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setSizeGripEnabled(false);

    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    leaderLabel = new QLabel;
    layout->addWidget(leaderLabel);

    progressBar = new QProgressBar;
    layout->addWidget(progressBar);

    footerLabel = new QLabel;
    layout->addWidget(footerLabel);

    buttonBox = new QDialogButtonBox;
    layout->addWidget(buttonBox);

    leaderLabel->setVisible(false);
    footerLabel->setVisible(false);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ExportStatusDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ExportStatusDialog::reject);
}


ExportStatusDialog::~ExportStatusDialog() {}


int ExportStatusDialog::minimumValue() const {
    return progressBar->minimum();
}


int ExportStatusDialog::maximumValue() const {
    return progressBar->maximum();
}


int ExportStatusDialog::currentValue() const {
    return progressBar->value();
}


QString ExportStatusDialog::progressText() const {
    return progressBar->text();
}


QString ExportStatusDialog::progressFormatText() const {
    return progressBar->format();
}


QString ExportStatusDialog::leaderText() const {
    return leaderLabel->text();
}


QString ExportStatusDialog::footerText() const {
    return footerLabel->text();
}


QDialogButtonBox::StandardButtons ExportStatusDialog::visibleButtons() const {
    return buttonBox->standardButtons();
}


void ExportStatusDialog::setMinimumValue(int newMinimumValue) {
    progressBar->setMinimum(newMinimumValue);
}


void ExportStatusDialog::setMaximumValue(int newMaximumValue) {
    progressBar->setMaximum(newMaximumValue);
}


void ExportStatusDialog::setRange(int newMinimumValue, int newMaximumValue) {
    progressBar->setRange(newMinimumValue, newMaximumValue);
}


void ExportStatusDialog::setCurrentValue(int newCompletionValue) {
    progressBar->setValue(newCompletionValue);
}


void ExportStatusDialog::setProgressFormatText(const QString& newProgressBarText) {
    progressBar->setFormat(newProgressBarText);
}


void ExportStatusDialog::setLeaderText(const QString& newLeaderText) {
    leaderLabel->setVisible(!newLeaderText.isEmpty());
    leaderLabel->setText(newLeaderText);
}


void ExportStatusDialog::setFooterText(const QString& newFooterText) {
    footerLabel->setVisible(!newFooterText.isEmpty());
    footerLabel->setText(newFooterText);
}


void ExportStatusDialog::setVisibleButtons(QDialogButtonBox::StandardButtons buttons) {
    buttonBox->setStandardButtons(buttons);
}
