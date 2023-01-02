/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref InspectorDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QSet>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>
#include <QFont>
#include <QFontMetrics>
#include <QDialogButtonBox>

#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_calculated_value.h>
#include <ld_variable_name.h>

#include "application.h"
#include "inspector_widget.h"
#include "inspector_dialog.h"

InspectorDialog::InspectorDialog(QWidget* parent):ProgrammaticDialog("InspectorDialog", parent) {
    configureWidget();
    runBuilders();
}


InspectorDialog::InspectorDialog(
        const Ld::CalculatedValue& calculatedValue,
        QWidget*                   parent
    ):ProgrammaticDialog(
        "InspectorDialog",
        parent
    ) {
    configureWidget();
    runBuilders();

    setCalculatedValue(calculatedValue);
}


InspectorDialog::~InspectorDialog() {}


bool InspectorDialog::okAlwaysEnabled() const {
    return currentOkAlwaysEnabled;
}


bool InspectorDialog::okConditionallyEnabled() const {
    return !okAlwaysEnabled();
}


Ld::CalculatedValue InspectorDialog::calculatedValue() const {
    Ld::CalculatedValue result;

    InspectorWidget* inspectorWidget = widget<InspectorWidget>("inspector_widget");
    if (inspectorWidget != Q_NULLPTR) {
        result = inspectorWidget->calculatedValue();
    } else {
        result = currentCalculatedValue;
    }

    return result;
}


void InspectorDialog::populate() {
    Ld::VariableName variableName = currentCalculatedValue.name();

    QLabel* variableNameLabel        = widget<QLabel>("variable_name_label");
    QLabel* variableDescriptionLabel = widget<QLabel>("variable_description_label");

    if (variableName.text2().isEmpty()) {
        variableNameLabel->setText(variableName.text1());
    } else {
        variableNameLabel->setText(tr("%1<sub>%2</sub>").arg(variableName.text1()).arg(variableName.text2()));
    }

    variableDescriptionLabel->setText(currentCalculatedValue.description());

    QWidget* inspectorWidget = widget<QWidget>("inspector_widget");
    if (inspectorWidget != Q_NULLPTR) {
        inspectorWidget->deleteLater();
    }

    InspectorWidget* newInspectorWidget = InspectorWidget::createWidget(currentCalculatedValue);
    reRegisterWidget(newInspectorWidget, "inspector_widget");
    QHBoxLayout* inspectorWidgetLayout = EQt::ProgrammaticWidget::layout<QHBoxLayout>("inspector_widget_layout");
    inspectorWidgetLayout->addWidget(newInspectorWidget);

    connect(newInspectorWidget, &InspectorWidget::valueChanged, this, &InspectorDialog::valueChanged);
    connect(newInspectorWidget, &InspectorWidget::valueRestored, this, &InspectorDialog::valueRestored);

    QPushButton* loadPushButton = widget<QPushButton>("load_push_button");
    QPushButton* savePushButton = widget<QPushButton>("save_push_button");

    if (newInspectorWidget->includeLoadSaveSupport()) {
        loadPushButton->setVisible(true);
        savePushButton->setVisible(true);

        connect(loadPushButton, &QPushButton::clicked, newInspectorWidget, &InspectorWidget::loadValue);
        connect(savePushButton, &QPushButton::clicked, newInspectorWidget, &InspectorWidget::saveValue);
    } else {
        loadPushButton->setVisible(false);
        savePushButton->setVisible(false);
    }

    QPushButton* resetButton  = widget<QPushButton>("reset_push_button");
    QPushButton* okButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");

    resetButton->setEnabled(false);
    okButton->setEnabled(currentOkAlwaysEnabled);
    cancelButton->setEnabled(true);
}


void InspectorDialog::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    currentCalculatedValue = calculatedValue;

    if (isVisible()) {
        populate();
    }
}


void InspectorDialog::setOkAlwaysEnabled(bool nowAlwaysEnabled) {
    currentOkAlwaysEnabled = nowAlwaysEnabled;

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    if (nowAlwaysEnabled) {
        okButton->setEnabled(true);
    } else {
        InspectorWidget* inspectorWidget = widget<InspectorWidget>("inspector_widget");
        if (inspectorWidget != Q_NULLPTR) {
            okButton->setEnabled(inspectorWidget->calculatedValueChanged());
        } else {
            okButton->setEnabled(false);
        }
    }
}


void InspectorDialog::setOkConditionallyEnabled(bool nowConditionallyEnabled) {
    setOkAlwaysEnabled(!nowConditionallyEnabled);
}


void InspectorDialog::bind() {
    QPushButton* resetButton = widget<QPushButton>("reset_push_button");
    connect(resetButton, &QPushButton::clicked, this, &InspectorDialog::resetClicked);

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &InspectorDialog::accept);

    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
    connect(cancelButton, &QPushButton::clicked, this, &InspectorDialog::reject);

    ProgrammaticDialog::bind();
}


void InspectorDialog::resetClicked() {
    InspectorWidget* inspectorWidget = widget<InspectorWidget>("inspector_widget");
    if (inspectorWidget != Q_NULLPTR) {
        inspectorWidget->resetCalculatedValue();
        valueRestored(Ld::CalculatedValue());
    }
}


void InspectorDialog::valueChanged(const Ld::CalculatedValue& newCalculatedValue) {
    QPushButton* resetButton = widget<QPushButton>("reset_push_button");
    QPushButton* okButton    = widget<QPushButton>("ok_push_button");

    resetButton->setEnabled(true);
    okButton->setEnabled(true);

    QLabel* variableDescriptionLabel = widget<QLabel>("variable_description_label");
    variableDescriptionLabel->setText(tr("%1 changed").arg(newCalculatedValue.description()));
}


void InspectorDialog::valueRestored(const Ld::CalculatedValue& originalCalculatedValue) {
    QPushButton* resetButton = widget<QPushButton>("reset_push_button");
    QPushButton* okButton    = widget<QPushButton>("ok_push_button");

    resetButton->setEnabled(false);
    okButton->setEnabled(currentOkAlwaysEnabled);

    QLabel* variableDescriptionLabel = widget<QLabel>("variable_description_label");
    variableDescriptionLabel->setText(originalCalculatedValue.description());
}


void InspectorDialog::configureWidget() {
    setWindowTitle(tr("Inspector"));
    setSizeGripEnabled(true);

    currentOkAlwaysEnabled = false;

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* topLabelLayout = newHBoxLayout("top_horizontal_layout");
    mainLayout->addLayout(topLabelLayout);

    QLabel* variableNameLabel = new QLabel;
    registerWidget(variableNameLabel, "variable_name_label");
    topLabelLayout->addWidget(variableNameLabel);

    topLabelLayout->addSpacing(40);
    topLabelLayout->addStretch(1);

    QLabel* variableDescriptionLabel = new QLabel;
    registerWidget(variableDescriptionLabel, "variable_description_label");
    topLabelLayout->addWidget(variableDescriptionLabel);

    QHBoxLayout* loadSaveLayout = newHBoxLayout("load_save_layout");
    mainLayout->addLayout(loadSaveLayout);

    QPushButton* loadPushButton = new QPushButton("Load From file");
    registerWidget(loadPushButton, "load_push_button");
    loadSaveLayout->addWidget(loadPushButton);

    QPushButton* savePushButton = new QPushButton("Save to file");
    registerWidget(savePushButton, "save_push_button");
    loadSaveLayout->addWidget(savePushButton);

    QHBoxLayout* inspectorWidgetLayout = newHBoxLayout("inspector_widget_layout");
    mainLayout->addLayout(inspectorWidgetLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
          QDialogButtonBox::Reset
        | QDialogButtonBox::Ok
        | QDialogButtonBox::Cancel
    );

    buttonBox->button(QDialogButtonBox::Reset)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Reset)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Reset), "reset_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    mainLayout->addWidget(buttonBox);
}
