/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ComplexInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "inspector_widget.h"
#include "complex_inspector_widget.h"

InspectorWidget* ComplexInspectorWidget::creator(QWidget* parent) {
    return new ComplexInspectorWidget(parent);
}


ComplexInspectorWidget::ComplexInspectorWidget(QWidget* parent):InspectorWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QHBoxLayout* valueHorizontalLayout = new QHBoxLayout;
    mainLayout->addLayout(valueHorizontalLayout);

    QGridLayout* valueGridLayout = new QGridLayout;

    valueHorizontalLayout->addStretch(1);
    valueHorizontalLayout->addLayout(valueGridLayout);
    valueHorizontalLayout->addStretch(1);

    valueGridLayout->addWidget(new QLabel(tr("Value: ")), 1, 0);
    valueGridLayout->addWidget(new QLabel(tr("Real")), 0, 1);
    valueGridLayout->addWidget(new QLabel(tr("Imaginary")), 0, 3);
    valueGridLayout->addWidget(new QLabel(tr("i")), 1, 3);
    valueGridLayout->addWidget(new QLabel(tr("+")), 1, 2);
    valueGridLayout->addWidget(new QLabel(tr("i")), 1, 4);

    realLineEditor = new QLineEdit;
    valueGridLayout->addWidget(realLineEditor, 1, 1);

    QDoubleValidator* realValidator = new QDoubleValidator(realLineEditor);
    realLineEditor->setValidator(realValidator);

    imaginaryLineEditor = new QLineEdit;
    valueGridLayout->addWidget(imaginaryLineEditor, 1, 3);

    QDoubleValidator* imaginaryValidator = new QDoubleValidator(imaginaryLineEditor);
    imaginaryLineEditor->setValidator(imaginaryValidator);

    connect(realLineEditor, &QLineEdit::textEdited, this, &ComplexInspectorWidget::realTextEdited);
    connect(imaginaryLineEditor, &QLineEdit::textEdited, this, &ComplexInspectorWidget::imaginaryTextEdited);
}


ComplexInspectorWidget::~ComplexInspectorWidget() {}


Ld::CalculatedValue ComplexInspectorWidget::calculatedValue() const {
    Ld::CalculatedValue     result;

    const QDoubleValidator* realValidator = dynamic_cast<const QDoubleValidator*>(realLineEditor->validator());
    const QDoubleValidator* imagValidator = dynamic_cast<const QDoubleValidator*>(imaginaryLineEditor->validator());

    int                                position;
    QString                            realText = realLineEditor->text();
    QRegularExpressionValidator::State rs       = realValidator->validate(realText, position);

    QString                            imagText = imaginaryLineEditor->text();
    QRegularExpressionValidator::State is       = imagValidator->validate(imagText, position);

    if (rs == QRegularExpressionValidator::Acceptable && is == QRegularExpressionValidator::Acceptable) {
        bool   realOk;
        double realValue = realText.toDouble(&realOk);

        bool   imagOk;
        double imagValue = imagText.toDouble(&imagOk);

        if (realOk && imagOk) {
            Model::Complex value(realValue, imagValue);
            result = Ld::CalculatedValue(currentCalculatedValue.name(), Model::Variant(value));
        }
    }

    return result;
}


bool ComplexInspectorWidget::calculatedValueChanged() const {
    bool result;

    const QDoubleValidator* realValidator = dynamic_cast<const QDoubleValidator*>(realLineEditor->validator());
    const QDoubleValidator* imagValidator = dynamic_cast<const QDoubleValidator*>(imaginaryLineEditor->validator());

    int                                position;
    QString                            realText = realLineEditor->text();
    QRegularExpressionValidator::State rs       = realValidator->validate(realText, position);

    QString                            imagText = imaginaryLineEditor->text();
    QRegularExpressionValidator::State is       = imagValidator->validate(imagText, position);

    if (rs == QRegularExpressionValidator::Acceptable && is == QRegularExpressionValidator::Acceptable) {
        bool   realOk;
        double realValue = realText.toDouble(&realOk);

        bool   imagOk;
        double imagValue = imagText.toDouble(&imagOk);

        if (realOk && imagOk) {
            Model::Complex inputValue(realValue, imagValue);
            Model::Complex originalValue = currentCalculatedValue.variant().toComplex();

            result = (inputValue != originalValue);
        } else {
            result = false;
        }
    } else {
        result = false;
    }

    return result;
}


void ComplexInspectorWidget::realTextEdited(const QString& /* text */) {
    generateSignals();
}


void ComplexInspectorWidget::imaginaryTextEdited(const QString& text) {
    generateSignals();

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


void ComplexInspectorWidget::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    currentCalculatedValue = calculatedValue;
    resetCalculatedValue();
}


void ComplexInspectorWidget::resetCalculatedValue() {
    Model::Variant variant   = currentCalculatedValue.variant();
    Model::Complex value     = variant.toComplex();
    Model::Real    realValue = value.real();
    Model::Real    imagValue = value.imag();

    realLineEditor->setText(QString::number(realValue));
    imaginaryLineEditor->setText(QString::number(imagValue));
}


void ComplexInspectorWidget::generateSignals() {
    const QDoubleValidator* realValidator = dynamic_cast<const QDoubleValidator*>(realLineEditor->validator());
    const QDoubleValidator* imagValidator = dynamic_cast<const QDoubleValidator*>(imaginaryLineEditor->validator());

    int                                position;
    QString                            realText = realLineEditor->text();
    QRegularExpressionValidator::State rs       = realValidator->validate(realText, position);

    QString                            imagText = imaginaryLineEditor->text();
    QRegularExpressionValidator::State is       = imagValidator->validate(imagText, position);

    if (rs == QRegularExpressionValidator::Acceptable && is == QRegularExpressionValidator::Acceptable) {
        bool   realOk;
        double realValue = realText.toDouble(&realOk);

        bool   imagOk;
        double imagValue = imagText.toDouble(&imagOk);

        if (realOk && imagOk) {
            Model::Complex inputValue(realValue, imagValue);
            Model::Complex originalValue = currentCalculatedValue.variant().toComplex();

            if (inputValue != originalValue) {
                emit valueChanged(Ld::CalculatedValue(currentCalculatedValue.name(), Model::Variant(inputValue)));
            } else {
                emit valueRestored(currentCalculatedValue);
            }
        } else {
            emit valueRestored(currentCalculatedValue);
        }
    } else {
        emit valueRestored(currentCalculatedValue);
    }
}
