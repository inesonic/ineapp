/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref AboutDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QLabel>
#include <QScreen>
#include <QRect>
#include <QFont>
#include <QDateTime>
#include <QDate>

#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include "application.h"
#include "application_settings.h"
#include "plug_in_manager.h"
#include "about_plugins_dialog.h"
#include "about_dialog.h"

AboutDialog::AboutDialog(QWidget* parent):ProgrammaticDialog("AboutDialog", parent) {
    setWindowTitle(tr("About %1").arg(Application::applicationName()));
    setSizeGripEnabled(false);

    configureWidget();
    runBuilders();
}


AboutDialog::~AboutDialog() {}


void AboutDialog::populate() {
    widget<QPushButton>("plugins_push_button")->setVisible(plugInsLoaded());
    ProgrammaticDialog::populate();
}

void AboutDialog::listPlugIns() {
    AboutPlugInsDialog plugInsDialog(this);
    plugInsDialog.exec();
}


void AboutDialog::bind() {
    connect(widget<QPushButton>("ok_push_button"), SIGNAL(clicked()), this, SLOT(close()));

    QPushButton* plugInsPushButton = widget<QPushButton>("plugins_push_button");
    if (plugInsPushButton != Q_NULLPTR) {
        connect(plugInsPushButton, SIGNAL(clicked()), this, SLOT(listPlugIns()));
    }

    ProgrammaticDialog::bind();
}


void AboutDialog::configureWidget() {
    QVBoxLayout* verticalLayout = newVBoxLayout("vertical_layout");
    setLayout(verticalLayout);

    QHBoxLayout* horizontalLayout = newHBoxLayout("horizontal_layout");
    verticalLayout->addLayout(horizontalLayout);

    QVBoxLayout* leftVerticalLayout = newVBoxLayout("left_vertical_layout");
    horizontalLayout->addLayout(leftVerticalLayout);

    buildLogo(leftVerticalLayout);

    horizontalLayout->addSpacing(8);

    QVBoxLayout* rightVerticalLayout = newVBoxLayout("right_vertical_layout");
    horizontalLayout->addLayout(rightVerticalLayout);

    QGroupBox* companyGroupBox = new QGroupBox();
    rightVerticalLayout->addWidget(companyGroupBox);
    registerWidget(companyGroupBox, "company_group_box");

    QVBoxLayout* companyLayout = newVBoxLayout("company_layout");
    companyGroupBox->setLayout(companyLayout);

    buildCompanyData(companyLayout);

    QGroupBox* licensingGroupBox = new QGroupBox();
    rightVerticalLayout->addWidget(licensingGroupBox);
    registerWidget(licensingGroupBox, "license_group_box");

    QVBoxLayout* licensingLayout = newVBoxLayout("licensing_layout");
    licensingGroupBox->setLayout(licensingLayout);

    QHBoxLayout* buttonLayout = newHBoxLayout("button_layout");
    verticalLayout->addLayout(buttonLayout);

    QPushButton* okPushButton = new QPushButton(tr("OK"));
    registerWidget(okPushButton, "ok_push_button");

    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okPushButton);
    buttonLayout->addStretch(1);
}


void AboutDialog::buildLogo(QVBoxLayout* layout) {
    layout->addStretch(1);

    QPixmap* nativeLogo   = new QPixmap(":inesonic_logo.png");
    unsigned nativeWidth  = nativeLogo->width();
    unsigned nativeHeight = nativeLogo->height();

    unsigned desiredWidth  = unsigned(0.5 + logicalDpiX() * float(logoWidth));
    unsigned desiredHeight = unsigned(0.5 + nativeHeight * float(desiredWidth)/float(nativeWidth));

    QPixmap logo = nativeLogo->scaled(desiredWidth, desiredHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QLabel* logoLabel = new QLabel;
    registerWidget(logoLabel, "logo_label");

    logoLabel->setPixmap(logo);
    layout->addWidget(logoLabel);

    layout->addSpacing(8);

    QFont defaultFont = Application::font();
    defaultFont.setStyleHint(QFont::Helvetica, QFont::PreferDefault);

    QFont productNameFont = defaultFont;
    productNameFont.setPointSize(productNamePointSize);

    QLabel* productNameLabel = new QLabel(Application::applicationName());
    registerWidget(productNameLabel, "product_name_label");

    productNameLabel->setFont(productNameFont);
    productNameLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(productNameLabel);

    QFont catchPhraseFont = defaultFont;
    catchPhraseFont.setPointSize(catchPhrasePointSize);

    QLabel* catchPhraseLabel = new QLabel(CATCH_PHRASE);
    registerWidget(catchPhraseLabel, "catch_phrase_label");

    catchPhraseLabel->setFont(catchPhraseFont);
    catchPhraseLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(catchPhraseLabel);

    QFont versionFont = defaultFont;
    versionFont.setPointSize(versionPointSize);

    QLabel* versionLabel = new QLabel(tr("Version %1").arg(Application::applicationVersion()));
    registerWidget(versionLabel, "version_label");

    versionLabel->setFont(versionFont);
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);

//    QLabel* buildLabel = new QLabel(tr("Build %1").arg(Application::applicationBuildString()));
//    registerWidget(buildLabel, "build_label");

//    buildLabel->setFont(versionFont);
//    buildLabel->setAlignment(Qt::AlignCenter);
//    layout->addWidget(buildLabel);

    layout->addStretch(1);
}


void AboutDialog::buildCompanyData(QVBoxLayout* layout) {
    QFont defaultFont = Application::font();
    defaultFont.setStyleHint(QFont::Helvetica, QFont::PreferDefault);

    QFont companyNameFont = defaultFont;
    companyNameFont.setPointSize(companyNamePointSize);

    QLabel* companyNameLabel = new QLabel(Application::organizationName());
    registerWidget(companyNameLabel, "company_name_label");

    companyNameLabel->setFont(companyNameFont);
    companyNameLabel->setAlignment(Qt::AlignLeft);
    layout->addWidget(companyNameLabel);

    layout->addSpacing(8);

    QFont companyUrlFont = defaultFont;
    companyUrlFont.setPointSize(companyUrlPointSize);

    QLabel* companyUrlLabel = new QLabel(tr("http://www.%1").arg(Application::organizationDomain()));
    registerWidget(companyUrlLabel, "company_url_label");

    companyUrlLabel->setFont(companyUrlFont);
    companyUrlLabel->setAlignment(Qt::AlignLeft);
    layout->addWidget(companyUrlLabel);
}


bool AboutDialog::plugInsLoaded() {
    return !Application::plugInManager()->plugInsByName().isEmpty();
}
