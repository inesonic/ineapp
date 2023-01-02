/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DoubleAutomaticLineEdit class.
***********************************************************************************************************************/

#include <QWidget>
#include <QString>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QCheckBox>
#include <QLineEdit>
#include <QFontMetrics>
#include <QValidator>
#include <QDoubleValidator>
#include <QPalette>

#include <limits>

#include "application.h"
#include "double_automatic_line_edit.h"

DoubleAutomaticLineEdit::DoubleAutomaticLineEdit(QWidget* parent):QWidget(parent) {
    configureWidget();
}


DoubleAutomaticLineEdit::DoubleAutomaticLineEdit(double initialValue, QWidget* parent):QWidget(parent) {
    configureWidget();
    setValue(initialValue);
}


DoubleAutomaticLineEdit::DoubleAutomaticLineEdit(
        double   initialValue,
        bool     automatic,
        QWidget* parent
    ):QWidget(
        parent
    ) {
    configureWidget();
    setValue(initialValue);
    setAutomatic(automatic);
}


DoubleAutomaticLineEdit::~DoubleAutomaticLineEdit() {}


bool DoubleAutomaticLineEdit::isValid() const {
    QString text = doubleLineEditor->text();
    int     pos;

    return doubleValidator->validate(text, pos) == QValidator::State::Acceptable;
}


bool DoubleAutomaticLineEdit::isInvalid() const {
    return !isValid();
}


bool DoubleAutomaticLineEdit::automatic() const {
    return automaticCheckBox->isChecked() && automaticCheckBox->isEnabled();
}


double DoubleAutomaticLineEdit::value() const {
    double result;

    if (automatic()) {
        result = std::numeric_limits<double>::quiet_NaN();
    } else {
        bool ok;
        result = doubleLineEditor->text().toDouble(&ok);

        if (!ok) {
            result = std::numeric_limits<double>::quiet_NaN();
        }
    }

    return result;
}


double DoubleAutomaticLineEdit::minimumValue() const {
    return doubleValidator->bottom();
}


double DoubleAutomaticLineEdit::maximumValue() const {
    return doubleValidator->top();
}


bool DoubleAutomaticLineEdit::automaticCheckboxEnabled() const {
    return automaticCheckBox->isEnabled();
}


bool DoubleAutomaticLineEdit::automaticCheckboxDisabled() const {
    return !automaticCheckboxEnabled();
}


bool DoubleAutomaticLineEdit::lineEditorEnabled() const {
    return doubleLineEditor->isEnabled();
}


bool DoubleAutomaticLineEdit::lineEditorDisabled() const {
    return !lineEditorEnabled();
}


void DoubleAutomaticLineEdit::setAutomatic(bool nowAutomatic) {
    if (nowAutomatic != automatic()) {
        automaticCheckBox->setChecked(nowAutomatic);

        if (automaticControlsEnabled) {
            doubleLineEditor->setDisabled(nowAutomatic);

            if (nowAutomatic) {
                lastLineEditorValue = doubleLineEditor->text();
                doubleLineEditor->setText(tr("auto"));
                doubleLineEditor->setAlignment(Qt::AlignmentFlag::AlignCenter);
            } else {
                doubleLineEditor->setText(lastLineEditorValue);
                lastLineEditorValue.clear();
                doubleLineEditor->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);
            }
        }
    }
}


void DoubleAutomaticLineEdit::setManual(bool nowManual) {
    setAutomatic(!nowManual);
}


void DoubleAutomaticLineEdit::setValue(double newValue) {
    automaticControlsEnabled = false;
    editorEnabled            = true;

    doubleLineEditor->setText(QString::number(newValue));
    automaticCheckBox->setChecked(false);
    doubleLineEditor->setEnabled(true);
}


void DoubleAutomaticLineEdit::setMinimumValue(double newMinimum) {
    doubleValidator->setBottom(newMinimum);
    textChanged(doubleLineEditor->text());
}


void DoubleAutomaticLineEdit::setMaximumValue(double newMaximum) {
    doubleValidator->setTop(newMaximum);
    textChanged(doubleLineEditor->text());
}


void DoubleAutomaticLineEdit::setAutomaticCheckboxEnabled(bool nowEnabled) {
    automaticControlsEnabled = nowEnabled;

    automaticCheckBox->setEnabled(nowEnabled);
    automaticCheckBox->setVisible(nowEnabled);

    if (nowEnabled) {
        automaticChanged(automaticCheckBox->isChecked());
    } else {
        doubleLineEditor->setEnabled(editorEnabled);
        doubleLineEditor->setText(lastLineEditorValue);
        lastLineEditorValue.clear();

        doubleLineEditor->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);
    }
}


void DoubleAutomaticLineEdit::setAutomaticCheckboxDisabled(bool nowDisabled) {
    setAutomaticCheckboxEnabled(!nowDisabled);
}


void DoubleAutomaticLineEdit::setLineEditorEnabled(bool nowEnabled) {
    editorEnabled = nowEnabled;

    doubleLineEditor->setEnabled(nowEnabled);
    doubleLineEditor->setVisible(nowEnabled);
}


void DoubleAutomaticLineEdit::setLineEditorDisabled(bool nowDisabled) {
    setLineEditorEnabled(!nowDisabled);
}


void DoubleAutomaticLineEdit::textChanged(const QString& text) {
    QString t = text;
    int     pos;

    QPalette palette;
    if (automaticCheckBox->isChecked()) {
        palette.setColor(QPalette::ColorRole::Text, Qt::GlobalColor::darkGray);
        emit changed(std::numeric_limits<double>::quiet_NaN(), true);
    } else if (doubleValidator->validate(t, pos) == QValidator::State::Acceptable) {
        palette.setColor(QPalette::ColorRole::Text, Qt::GlobalColor::black);
        emit changed(text.toDouble(), false);
    } else {
        palette.setColor(QPalette::ColorRole::Text, Qt::GlobalColor::red);
        emit changed(std::numeric_limits<double>::quiet_NaN(), false);
    }

    doubleLineEditor->setPalette(palette);
}


void DoubleAutomaticLineEdit::automaticChanged(bool checked) {
    if (checked) {
        lastLineEditorValue = doubleLineEditor->text();
        doubleLineEditor->setText(tr("auto"));
        doubleLineEditor->setAlignment(Qt::AlignmentFlag::AlignCenter);
        doubleLineEditor->setEnabled(false);
    } else {
        if (lastLineEditorValue == tr("auto")) {
            doubleLineEditor->setText(QString());
        } else {
            doubleLineEditor->setText(lastLineEditorValue);
        }

        lastLineEditorValue.clear();
        doubleLineEditor->setEnabled(true);
        doubleLineEditor->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);
    }

    textChanged(doubleLineEditor->text());
}


void DoubleAutomaticLineEdit::configureWidget() {
    QHBoxLayout* mainLayout = new QHBoxLayout;
    setLayout(mainLayout);

    doubleLineEditor = new QLineEdit;
    mainLayout->addWidget(doubleLineEditor);

    QFontMetricsF fontMetrics(Application::font());
    doubleLineEditor->setFixedWidth(fontMetrics.horizontalAdvance(tr("MMMMMMM")));

    doubleValidator = new QDoubleValidator(doubleLineEditor);
    doubleLineEditor->setValidator(doubleValidator);

    automaticCheckBox = new QCheckBox(tr("Automatic"));
    mainLayout->addWidget(automaticCheckBox);

    mainLayout->addStretch(1);

    automaticControlsEnabled = true;
    editorEnabled            = true;

    connect(doubleLineEditor, &QLineEdit::textChanged, this, &DoubleAutomaticLineEdit::textChanged);
    connect(automaticCheckBox, &QCheckBox::clicked, this, &DoubleAutomaticLineEdit::automaticChanged);
}
