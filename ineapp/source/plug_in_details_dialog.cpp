/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlugInDetailsDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QMap>

#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_plug_in_information.h>

#include "plug_in_details_dialog.h"

PlugInDetailsDialog::PlugInDetailsDialog(QWidget* parent):ProgrammaticDialog("PlugInDetailsDialog", parent) {
    setWindowTitle(tr("Plug-In Details"));
    setSizeGripEnabled(true);

    configureWidget();
    runBuilders();
}


PlugInDetailsDialog::PlugInDetailsDialog(
        const Ld::PlugInInformation& plugInInformation,
        QWidget*                     parent
    ):ProgrammaticDialog(
        "PlugInDetailsDialog",
        parent
    ),currentPlugInInformation(
        plugInInformation
    ) {
    setWindowTitle(tr("Plug-In Details"));
    setSizeGripEnabled(true);

    configureWidget();
    runBuilders();
}


PlugInDetailsDialog::~PlugInDetailsDialog() {}


Ld::PlugInInformation PlugInDetailsDialog::plugInInformation() const {
    return currentPlugInInformation;
}


void PlugInDetailsDialog::populate() {
    QLabel*    nameLabel                   = widget<QLabel>("name_label");
    QLabel*    versionLabel                = widget<QLabel>("version_label");
    QLabel*    companyLabel                = widget<QLabel>("company_label");
    QLabel*    authorLabel                 = widget<QLabel>("author_label");
    QLabel*    licenseLabel                = widget<QLabel>("license_label");
    QLabel*    briefDescriptionLabel       = widget<QLabel>("brief_description_label");
    QTextEdit* detailedDescriptionTextEdit = widget<QTextEdit>("detailed_description_text_edit");

    nameLabel->setText(currentPlugInInformation.name());
    versionLabel->setText(currentPlugInInformation.version());
    companyLabel->setText(currentPlugInInformation.company());
    authorLabel->setText(currentPlugInInformation.author());
    licenseLabel->setText(currentPlugInInformation.license());
    briefDescriptionLabel->setText(currentPlugInInformation.briefDescription());

    detailedDescriptionTextEdit->setText(currentPlugInInformation.detailedDescription());
}


void PlugInDetailsDialog::setPlugInInformation(const Ld::PlugInInformation& newPlugInInformation) {
    currentPlugInInformation = newPlugInInformation;

    if (isVisible()) {
        populate();
    }
}


void PlugInDetailsDialog::bind() {
    connect(widget<QPushButton>("ok_push_button"), SIGNAL(clicked()), this, SLOT(close()));
    ProgrammaticDialog::bind();
}


void PlugInDetailsDialog::configureWidget() {
    QVBoxLayout* verticalLayout = newVBoxLayout("vertical_layout");
    setLayout(verticalLayout);

    QFormLayout* mainLayout = newFormLayout("main_layout");
    verticalLayout->addLayout(mainLayout);

    QLabel* nameLabel = new QLabel(tr("---name---"));
    registerWidget(nameLabel, "name_label");

    QLabel* versionLabel = new QLabel(tr("---version---"));
    registerWidget(versionLabel, "version_label");

    QLabel* companyLabel = new QLabel(tr("---company---"));
    registerWidget(companyLabel, "company_label");

    QLabel* authorLabel = new QLabel(tr("---author---"));
    registerWidget(authorLabel, "author_label");

    QLabel* licenseLabel = new QLabel(tr("---license---"));
    registerWidget(licenseLabel, "license_label");

    QLabel* briefDescriptionLabel = new QLabel(tr("---brief description---"));
    registerWidget(briefDescriptionLabel, "brief_description_label");

    mainLayout->addRow(tr("Name: "), nameLabel);
    mainLayout->addRow(tr("Version: "), versionLabel);
    mainLayout->addRow(tr("Brief Description: "), briefDescriptionLabel);
    mainLayout->addRow(tr("Company: "), companyLabel);
    mainLayout->addRow(tr("Author: "), authorLabel);
    mainLayout->addRow(tr("Licensing: "), licenseLabel);

    verticalLayout->addSpacing(10);

    QTextEdit* detailedDescriptionTextEdit = new QTextEdit;
    detailedDescriptionTextEdit->setReadOnly(true);
    registerWidget(detailedDescriptionTextEdit, "detailed_description_text_edit");

    verticalLayout->addWidget(detailedDescriptionTextEdit);

    QHBoxLayout* buttonLayout = newHBoxLayout("button_layout");
    verticalLayout->addLayout(buttonLayout);

    QPushButton* okPushButton = new QPushButton(tr("OK"));
    registerWidget(okPushButton, "ok_push_button");
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okPushButton);
    buttonLayout->addStretch(1);
}
