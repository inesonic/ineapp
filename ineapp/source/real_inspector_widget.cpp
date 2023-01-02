/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RealInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QRadioButton>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "inspector_widget.h"
#include "real_inspector_widget.h"

InspectorWidget* RealInspectorWidget::creator(QWidget* parent) {
    return new RealInspectorWidget(parent);
}


RealInspectorWidget::RealInspectorWidget(QWidget* parent):InspectorWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QFormLayout* valueLayout = new QFormLayout;
    mainLayout->addLayout(valueLayout);

    realLineEditor = new QLineEdit;
    valueLayout->addRow("Value: ", realLineEditor);

    QDoubleValidator* validator = new QDoubleValidator(realLineEditor);
    realLineEditor->setValidator(validator);

    connect(realLineEditor, &QLineEdit::textEdited, this, &RealInspectorWidget::textEdited);
}


RealInspectorWidget::~RealInspectorWidget() {}


Ld::CalculatedValue RealInspectorWidget::calculatedValue() const {
    Ld::CalculatedValue     result;
    const QDoubleValidator* validator = dynamic_cast<const QDoubleValidator*>(realLineEditor->validator());

    int                                position;
    QString                            text = realLineEditor->text();
    QRegularExpressionValidator::State s    = validator->validate(text, position);

    if (s == QRegularExpressionValidator::Acceptable) {
        bool   ok;
        double value = text.toDouble(&ok);

        if (ok) {
            result = Ld::CalculatedValue(currentCalculatedValue.name(), Model::Variant(value));
        }
    }

    return result;
}


bool RealInspectorWidget::calculatedValueChanged() const {
    bool    result;
    QString text  = realLineEditor->text();
    double  value = text.toDouble(&result);

    if (result) {
        result = (value != currentCalculatedValue.variant().toReal());
    }

    return result;
}


void RealInspectorWidget::textEdited(const QString& text) {
    bool    ok;
    double  value = text.toDouble(&ok);

    if (ok) {
        double inputValue = currentCalculatedValue.variant().toReal();

        if (inputValue != value) {
            emit valueChanged(
                Ld::CalculatedValue(
                    currentCalculatedValue.name(),
                    Model::Variant(static_cast<Model::Integer>(value))
                )
            );
        } else {
            emit valueRestored(currentCalculatedValue);
        }
    } else {
        emit valueRestored(currentCalculatedValue);
    }
}


void RealInspectorWidget::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    currentCalculatedValue = calculatedValue;
    resetCalculatedValue();
}


void RealInspectorWidget::resetCalculatedValue() {
    Model::Real value = currentCalculatedValue.variant().toReal();
    realLineEditor->setText(QString::number(value));
}
