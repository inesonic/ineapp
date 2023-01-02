/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref VariableNameEditorWidget class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QLabel>
#include <QFontMetrics>
#include <QTimer>

#include <eqt_line_edit.h>

#include <ld_character_format.h>
#include <ld_variable_name.h>

#include "application.h"
#include "presentation.h"
#include "variable_name_fields_widget.h"
#include "variable_name_special_characters_widget.h"
#include "variable_name_editor_widget.h"

VariableNameEditorWidget::VariableNameEditorWidget(QWidget* parent):QWidget(parent) {
    configureWidget();
}



VariableNameEditorWidget::VariableNameEditorWidget(const QString& text1, QWidget* parent):QWidget(parent) {
    configureWidget();
    setText1(text1);
}


VariableNameEditorWidget::VariableNameEditorWidget(
        const QString& text1,
        const QString& text2,
        QWidget* parent
    ):QWidget(
        parent
    ) {
    configureWidget();
    setText1(text1);
    setText2(text2);
}


VariableNameEditorWidget::VariableNameEditorWidget(
        const Ld::VariableName& variableName,
        QWidget*                parent
    ):QWidget(
        parent
    ) {
    configureWidget();
    setVariableName(variableName);
}


VariableNameEditorWidget::~VariableNameEditorWidget() {}


bool VariableNameEditorWidget::isValid() const {
    return fieldsWidget->isValid();
}


bool VariableNameEditorWidget::isInvalid() const {
    return fieldsWidget->isInvalid();
}


QString VariableNameEditorWidget::text1() const {
    return fieldsWidget->text1();
}


QString VariableNameEditorWidget::text2() const {
    return fieldsWidget->text2();
}


Ld::VariableName VariableNameEditorWidget::variableName() const {
    return fieldsWidget->variableName();
}


QFont VariableNameEditorWidget::variableNameFont() const {
    return fieldsWidget->variableNameFont();
}


bool VariableNameEditorWidget::validate(const QString& text1, const QString& text2) {
    return VariableNameFieldsWidget::validate(text1, text2);
}


bool VariableNameEditorWidget::validate(const Ld::VariableName& variableName) {
    return VariableNameFieldsWidget::validate(variableName);
}


void VariableNameEditorWidget::setVariableNameFont(const QFont& newFont) {
    fieldsWidget->setVariableNameFont(newFont);
}


void VariableNameEditorWidget::setText1(const QString& newText) {
    fieldsWidget->setText1(newText);
}


void VariableNameEditorWidget::setText2(const QString& newText) {
    fieldsWidget->setText2(newText);
}


void VariableNameEditorWidget::setVariableName(const Ld::VariableName& newVariableName) {
    fieldsWidget->setVariableName(newVariableName);
}


void VariableNameEditorWidget::focusChanged(bool nowHasFocus) {
    specialCharactersWidget->setEnabled(nowHasFocus);
}


void VariableNameEditorWidget::configureWidget() {
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    fieldsWidget = new VariableNameFieldsWidget;
    mainLayout->addWidget(fieldsWidget);

    specialCharactersWidget = new VariableNameSpecialCharactersWidget;
    mainLayout->addWidget(specialCharactersWidget);

    QLabel* latexCommandLabel = new QLabel(tr("<b>Note:</b> Some LaTeX commands also accepted."));
    mainLayout->addWidget(latexCommandLabel);

    connect(fieldsWidget, &VariableNameFieldsWidget::changed, this, &VariableNameEditorWidget::changed);
    connect(fieldsWidget, &VariableNameFieldsWidget::focusChanged, this, &VariableNameEditorWidget::focusChanged);
    connect(
        fieldsWidget,
        &VariableNameFieldsWidget::returnKeyPressed,
        this,
        &VariableNameEditorWidget::returnKeyPressed
    );

    connect(
        specialCharactersWidget,
        &VariableNameSpecialCharactersWidget::selected,
        fieldsWidget,
        &VariableNameFieldsWidget::insertText
    );
}
