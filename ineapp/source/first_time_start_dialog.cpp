/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FirstTimeStartDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QDialogButtonBox>

#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_format.h>
#include <ld_character_format.h>
#include <ld_unordered_list_paragraph_format.h>

#include "first_time_start_dialog.h"

FirstTimeStartDialog::FirstTimeStartDialog(
        QString const& usageStatisticsUrl,
        QWidget*       parent
    ):ProgrammaticDialog(
        "FirstTimeStartDialog",
        parent
    ),currentUsageStatisticsUrl(
        usageStatisticsUrl
    ) {
    configureWidget();
    runBuilders();
}


FirstTimeStartDialog::~FirstTimeStartDialog() {}


bool FirstTimeStartDialog::usageStatisticsEnabled() const {
    QRadioButton* collectStatisticsRadioButton      = widget<QRadioButton>("collect_statistics_radio_button");
    return collectStatisticsRadioButton->isChecked();
}


void FirstTimeStartDialog::populate() {
}


void FirstTimeStartDialog::bind() {
    QRadioButton* collectStatisticsRadioButton      = widget<QRadioButton>("collect_statistics_radio_button");
    QRadioButton* doNotCollectStatisticsRadioButton = widget<QRadioButton>("do_not_collect_statistics_radio_button");

    connect(
        collectStatisticsRadioButton,
        &QRadioButton::clicked,
        this,
        &FirstTimeStartDialog::usageStatisticsValueChanged
    );
    connect(
        doNotCollectStatisticsRadioButton,
        &QRadioButton::clicked,
        this,
        &FirstTimeStartDialog::usageStatisticsValueChanged
    );

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &FirstTimeStartDialog::accept);

    ProgrammaticDialog::bind();
}


void FirstTimeStartDialog::usageStatisticsValueChanged(bool /* checked */) {
    conditionallyEnableOkButton("FirstTimeStartDialog");
}


void FirstTimeStartDialog::configureWidget() {
    setWindowTitle(tr("Welcome !"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QLabel* welcomeLabel = new QLabel(tr("<b>Welcome to Aion !</b>"));
    registerWidget(welcomeLabel, "welcome_label");
    welcomeLabel->setTextFormat(Qt::TextFormat::RichText);
    mainLayout->addWidget(welcomeLabel, 0, Qt::AlignmentFlag::AlignHCenter);

    mainLayout->addSpacing(5);

    QLabel* informationLabel = new QLabel(tr("Before we begin, please answer the questions below..."));
    registerWidget(informationLabel, "information_label");
    mainLayout->addWidget(informationLabel);
    mainLayout->addSpacing(5);

    QGroupBox* usageDataGroupBox = new QGroupBox(tr("Collection Of Usage Information"));
    registerWidget(usageDataGroupBox, "usage_data_group_box");
    mainLayout->addWidget(usageDataGroupBox);

    QVBoxLayout* usageDataLayout = newVBoxLayout("usage_data_layout");
    usageDataGroupBox->setLayout(usageDataLayout);

    QString explanationText = tr(
        "<p>"
        "We can collect anonymized usage statistics that we use to improve our software.  We use this information "
        "to better understand our customer's needs."
        "</p><p>"
        "&nbsp;"
        "</p><p>"
        "All information we collect is anonymous and the data we collect will <b>not</b> be shared with third parties."
        "</p><p>"
        "&nbsp;"
        "</p><p>"
        "For details on what information we collect and how we use that information, please see:"
        "</p><p align=\"center\">"
        "<a href=\"%1\">%1</a>."
        "</p><p>"
        "&nbsp;"
        "</p><p>"
        "Please select from one of the options below:"
        "</p>"
    ).arg(currentUsageStatisticsUrl);

    QLabel* explanationLabel = new QLabel(explanationText);
    registerWidget(explanationLabel, "usage_data_explanation_label");
    explanationLabel->setTextFormat(Qt::TextFormat::RichText);
    explanationLabel->setWordWrap(true);
    explanationLabel->setOpenExternalLinks(true);

    usageDataLayout->addWidget(explanationLabel);

    QRadioButton* collectStatisticsRadioButton = new QRadioButton(
        tr("Yes, Inesonic can collect anonymized usage statistics on my use of this software.")
    );
    registerWidget(collectStatisticsRadioButton, "collect_statistics_radio_button");

    QRadioButton* doNotCollectStatisticsRadioButton = new QRadioButton(
        tr("No, do not collect anonymized usage statistics on my use of this software.")
    );
    registerWidget(doNotCollectStatisticsRadioButton, "do_not_collect_statistics_radio_button");

    usageDataLayout->addWidget(collectStatisticsRadioButton);
    usageDataLayout->addWidget(doNotCollectStatisticsRadioButton);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");

    mainLayout->addWidget(buttonBox);
}
