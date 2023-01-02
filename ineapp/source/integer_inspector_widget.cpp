/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref IntegerInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QGroupBox>
#include <QRadioButton>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "inspector_widget.h"
#include "integer_inspector_widget.h"

const QRegularExpression IntegerInspectorWidget::regularExpression(
    "(0|[+-]?[1-9][0-9]*|0b[01][01]*|0x[0-9a-fA-F][0-9a-fA-F]*|'[bB][01][01]*|'[hH][0-9a-fA-F][0-9a-fA-F]*)"
);

InspectorWidget* IntegerInspectorWidget::creator(QWidget* parent) {
    return new IntegerInspectorWidget(parent);
}


IntegerInspectorWidget::IntegerInspectorWidget(QWidget* parent):InspectorWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QFormLayout* valueLayout = new QFormLayout;
    mainLayout->addLayout(valueLayout);

    integerLineEditor = new QLineEdit;
    valueLayout->addRow("Value: ", integerLineEditor);

    QRegularExpressionValidator* validator = new QRegularExpressionValidator(regularExpression, integerLineEditor);
    integerLineEditor->setValidator(validator);

    QGroupBox* radixGroupBox = new QGroupBox(tr("Displayed Radix"));
    mainLayout->addWidget(radixGroupBox);

    QVBoxLayout* radixLayout = new QVBoxLayout;
    radixGroupBox->setLayout(radixLayout);

    base2RadioButton = new QRadioButton(tr("Base 2"));
    radixLayout->addWidget(base2RadioButton);

    base10RadioButton = new QRadioButton(tr("Base 10"));
    radixLayout->addWidget(base10RadioButton);

    base16RadioButton = new QRadioButton(tr("Base 16"));
    radixLayout->addWidget(base16RadioButton);

    base2RadioButton->setChecked(false);
    base10RadioButton->setChecked(true);
    base16RadioButton->setChecked(false);

    connect(integerLineEditor, &QLineEdit::textEdited, this, &IntegerInspectorWidget::textEdited);
    connect(base2RadioButton,  &QRadioButton::clicked, this, &IntegerInspectorWidget::newRadixSelected);
    connect(base10RadioButton, &QRadioButton::clicked, this, &IntegerInspectorWidget::newRadixSelected);
    connect(base16RadioButton, &QRadioButton::clicked, this, &IntegerInspectorWidget::newRadixSelected);
}


IntegerInspectorWidget::~IntegerInspectorWidget() {}


Ld::CalculatedValue IntegerInspectorWidget::calculatedValue() const {
    Ld::CalculatedValue result;

    const QRegularExpressionValidator*
        validator = dynamic_cast<const QRegularExpressionValidator*>(integerLineEditor->validator());

    int                                position;
    QString                            text = integerLineEditor->text();
    QRegularExpressionValidator::State s    = validator->validate(text, position);

    if (s == QRegularExpressionValidator::Acceptable) {
        unsigned  radix;
        long long value = valueAndRadixOf(text, radix);

        if (radix > 0) {
            result = Ld::CalculatedValue(currentCalculatedValue.name(), Model::Variant(value));
        }
    }

    return result;
}


bool IntegerInspectorWidget::calculatedValueChanged() const {
    bool      result;
    unsigned  enteredRadix;
    long long value = valueAndRadixOf(integerLineEditor->text(), enteredRadix);

    if (enteredRadix > 0) {
        result = (currentCalculatedValue.variant().toInteger() != value);
    } else {
        result = false;
    }

    return result;
}


void IntegerInspectorWidget::textEdited(const QString& text) {
    unsigned  radix;
    long long value = valueAndRadixOf(text, radix);

    if (radix == 0) {
        emit valueRestored(currentCalculatedValue);
    } else {
        long long inputValue = currentCalculatedValue.variant().toInteger();
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
    }
}


void IntegerInspectorWidget::newRadixSelected() {
    unsigned  enteredRadix;
    long long value = valueAndRadixOf(integerLineEditor->text(), enteredRadix);

    if (enteredRadix > 0) {
        displayValue(value);
    } else {
        displayValue(currentCalculatedValue.variant().toInteger());
    }
}


void IntegerInspectorWidget::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    currentCalculatedValue = calculatedValue;
    resetCalculatedValue();
}


void IntegerInspectorWidget::resetCalculatedValue() {
    Model::Integer value = currentCalculatedValue.variant().toInteger();
    displayValue(value);
}


long long IntegerInspectorWidget::valueAndRadixOf(const QString& text, unsigned int& radix) {
    QString cleanedText = text.toLower();
    if (cleanedText.startsWith(tr("0b")) || cleanedText.startsWith(tr("'b"))) {
        cleanedText = cleanedText.mid(2);
        radix       = 2;
    } else if (cleanedText.startsWith(tr("0x")) || cleanedText.startsWith(tr("'h"))) {
        cleanedText  = cleanedText.mid(2);
        radix        = 16;
    } else {
        radix = 10;
    }

    bool      ok;
    long long result = text.toLongLong(&ok, radix);

    if (!ok) {
        radix = 0;
    }

    return result;
}


unsigned IntegerInspectorWidget::selectedRadix() const {
    unsigned result;

    if (base2RadioButton->isChecked()) {
        result = 2;
    } else if (base16RadioButton->isChecked()) {
        result = 16;
    } else {
        result = 10;
    }

    return result;
}


void IntegerInspectorWidget::displayValue(long long value) {
    unsigned radix = selectedRadix();
    QString  text;

    if (radix == 2) {
        text = tr("0b%1").arg(QString::number(value, 2));
    } else if (radix == 16) {
        text = tr("0x%1").arg(QString::number(value, 16).toUpper());
    } else {
        text = QString::number(value);
    }

    integerLineEditor->setText(text);
}
