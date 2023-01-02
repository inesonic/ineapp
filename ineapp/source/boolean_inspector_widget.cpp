/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BooleanInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QComboBox>
#include <QFormLayout>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "inspector_widget.h"
#include "boolean_inspector_widget.h"

InspectorWidget* BooleanInspectorWidget::creator(QWidget* parent) {
    return new BooleanInspectorWidget(parent);
}


BooleanInspectorWidget::BooleanInspectorWidget(QWidget* parent):InspectorWidget(parent) {
    QFormLayout* layout = new QFormLayout;
    setLayout(layout);

    booleanComboBox = new QComboBox;
    booleanComboBox->addItem(tr("false"));
    booleanComboBox->addItem(tr("true"));
    booleanComboBox->setEditable(false);

    layout->addRow(tr("Value: "), booleanComboBox);

    connect(
        booleanComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &BooleanInspectorWidget::currentIndexChanged
    );
}


BooleanInspectorWidget::~BooleanInspectorWidget() {}


Ld::CalculatedValue BooleanInspectorWidget::calculatedValue() const {
    Model::Boolean newValue = (booleanComboBox->currentIndex() != 0);
    return Ld::CalculatedValue(currentCalculatedValue.name(), Model::Variant(newValue));
}


bool BooleanInspectorWidget::calculatedValueChanged() const {
    Model::Boolean newValue = (booleanComboBox->currentIndex() != 0);
    return newValue != currentCalculatedValue.variant().toBoolean();
}


void BooleanInspectorWidget::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    currentCalculatedValue = calculatedValue;
    resetCalculatedValue();
}


void BooleanInspectorWidget::resetCalculatedValue() {
    if (currentCalculatedValue.variant().toBoolean()) {
        booleanComboBox->setCurrentIndex(1);
    } else {
        booleanComboBox->setCurrentIndex(0);
    }
}


void BooleanInspectorWidget::currentIndexChanged(int index) {
    Model::Boolean newValue = (index != 0);
    if (currentCalculatedValue.variant().toBoolean() != newValue) {
        emit valueChanged(calculatedValue());
    } else {
        emit valueRestored(currentCalculatedValue);
    }
}
