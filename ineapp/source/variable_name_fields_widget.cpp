/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref VariableNameFieldsWidget class.
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
#include <ld_special_characters.h>
#include <ld_variable_name.h>

#include "application.h"
#include "presentation.h"
#include "variable_name_fields_widget.h"

QRegularExpression VariableNameFieldsWidget::text1RegularExpression;
QRegularExpression VariableNameFieldsWidget::text2RegularExpression;

VariableNameFieldsWidget::VariableNameFieldsWidget(QWidget* parent):QWidget(parent) {
    configureWidget();
}



VariableNameFieldsWidget::VariableNameFieldsWidget(const QString& text1, QWidget* parent):QWidget(parent) {
    configureWidget();
    setText1(text1);
}


VariableNameFieldsWidget::VariableNameFieldsWidget(
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


VariableNameFieldsWidget::VariableNameFieldsWidget(
        const Ld::VariableName& variableName,
        QWidget*                parent
    ):QWidget(
        parent
    ) {
    configureWidget();
    setVariableName(variableName);
}


VariableNameFieldsWidget::~VariableNameFieldsWidget() {}


bool VariableNameFieldsWidget::isValid() const {
    int               position;
    const QValidator* text1Validator = text1LineEdit->validator();
    const QValidator* text2Validator = text2LineEdit->validator();

    QString text1 = text1LineEdit->text();
    QString text2 = text2LineEdit->text();

    return (
           !text1.isEmpty()
        && text1Validator->validate(text1, position) == QValidator::State::Acceptable
        && text2Validator->validate(text2, position) == QValidator::State::Acceptable
    );
}


bool VariableNameFieldsWidget::isInvalid() const {
    return !isValid();
}


QString VariableNameFieldsWidget::text1() const {
    return text1LineEdit->text();
}


QString VariableNameFieldsWidget::text2() const {
    return text2LineEdit->text();
}


Ld::VariableName VariableNameFieldsWidget::variableName() const {
    return Ld::VariableName(text1LineEdit->text(), text2LineEdit->text());
}


QFont VariableNameFieldsWidget::variableNameFont() const {
    return text1LineEdit->font();
}


bool VariableNameFieldsWidget::hasFocus() const {
    return text1LineEdit->hasFocus() || text2LineEdit->hasFocus();
}


bool VariableNameFieldsWidget::validate(const QString& text1, const QString& text2) {
    buildRegularExpressions();

    return (
        !text1.isEmpty()
        && text1RegularExpression.match(text1).hasMatch()
        && (text2.isEmpty() || text2RegularExpression.match(text2).hasMatch())
    );
}


bool VariableNameFieldsWidget::validate(const Ld::VariableName& variableName) {
    return validate(variableName.text1(), variableName.text2());
}


void VariableNameFieldsWidget::setVariableNameFont(const QFont& newFont) {
    QFont subscriptFont = newFont;
    subscriptFont.setPointSize(newFont.pointSize() * Presentation::subscriptSizeAdjustment);
    subscriptFont.setWeight(static_cast<QFont::Weight>(newFont.weight() + Presentation::subscriptWeightAdjustment));

    text1LineEdit->setFont(newFont);
    text2LineEdit->setFont(subscriptFont);
}


void VariableNameFieldsWidget::setText1(const QString& newText) {
    text1LineEdit->setText(newText);
}


void VariableNameFieldsWidget::setText2(const QString& newText) {
    text2LineEdit->setText(newText);
}


void VariableNameFieldsWidget::setVariableName(const Ld::VariableName& newVariableName) {
    text1LineEdit->setText(newVariableName.text1());
    text2LineEdit->setText(newVariableName.text2());
}


void VariableNameFieldsWidget::insertText(const QString& character) {
    if (text1LineEdit->hasFocus()) {
        text1LineEdit->insert(character);
    } else {
        if (text2LineEdit->hasFocus()) {
            text2LineEdit->insert(character);
        }
    }
}


void VariableNameFieldsWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    showTimer->start(1);
}


void VariableNameFieldsWidget::text1Edited(const QString& newText) {
    bool    isValid;
    QString updatedText = unLaTeXVariableName(newText, &isValid);
    if (!isValid) {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        text1LineEdit->setPalette(palette);
    } else {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        text1LineEdit->setPalette(palette);

        if (updatedText != newText) {
            text1LineEdit->setText(updatedText);
        }
    }

    emit changed(newText, text2LineEdit->text());
}


void VariableNameFieldsWidget::text2Edited(const QString& newText) {
    bool    isValid;
    QString updatedText = unLaTeXVariableName(newText, &isValid);
    if (!isValid) {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::red);
        text2LineEdit->setPalette(palette);
    } else {
        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        text2LineEdit->setPalette(palette);

        if (updatedText != newText) {
            text2LineEdit->setText(updatedText);
        }
    }

    emit changed(text1LineEdit->text(), newText);
}


void VariableNameFieldsWidget::editorFocusChanged() {
    if (text1LineEdit->hasFocus() || text2LineEdit->hasFocus()) {
        emit receivedFocus();
        emit focusChanged(true);
    } else {
        emit lostFocus();
        emit focusChanged(false);
    }
}


void VariableNameFieldsWidget::nowVisible() {
    text1LineEdit->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
}


void VariableNameFieldsWidget::configureWidget() {
    QHBoxLayout* mainLayout = new QHBoxLayout;
    setLayout(mainLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(mainLayout);

    QVBoxLayout* variableNameLeftVerticalLayout = new QVBoxLayout;
    mainLayout->addLayout(variableNameLeftVerticalLayout);

    QVBoxLayout* variableNameRightVerticalLayout = new QVBoxLayout;
    mainLayout->addLayout(variableNameRightVerticalLayout);

    QFont defaultMathIdentifierFont = Ld::CharacterFormat::applicationDefaultMathIdentifierFont()->toQFont();
    QFont identifierFont            = defaultMathIdentifierFont;
    QFont identifierSubscriptFont   = identifierFont;

    identifierFont.setPointSize(identifierFont.pointSize() / Presentation::subscriptSizeAdjustment);
    identifierSubscriptFont.setWeight(
        static_cast<QFont::Weight>(identifierSubscriptFont.weight() + Presentation::subscriptWeightAdjustment)
    );

    QFontMetrics identifierFontMetrics(identifierFont);
    QFontMetrics identifierSubscriptFontMetrics(identifierSubscriptFont);

    unsigned subscriptBaselineAdjustment = (
          identifierFontMetrics.ascent() - identifierSubscriptFontMetrics.ascent()
        - identifierFontMetrics.height() * Presentation::subscriptBaseline
    );

    text1LineEdit = new EQt::LineEdit;
    text1LineEdit->setAlignment(Qt::AlignRight);
    text1LineEdit->setFont(identifierFont);
    QMargins margins = text1LineEdit->textMargins();
    text1LineEdit->setTextMargins(margins.left(), margins.top(), 0, margins.bottom());
    variableNameLeftVerticalLayout->addWidget(text1LineEdit);

    variableNameLeftVerticalLayout->addStretch(1);

    variableNameRightVerticalLayout->addSpacing(subscriptBaselineAdjustment);

    text2LineEdit = new EQt::LineEdit;
    text2LineEdit->setAlignment(Qt::AlignLeft);
    text2LineEdit->setFont(identifierSubscriptFont);
    margins = text2LineEdit->textMargins();
    text2LineEdit->setTextMargins(0, margins.top(), margins.right(), margins.bottom());
    variableNameRightVerticalLayout->addWidget(text2LineEdit);

    buildRegularExpressions();

    QRegularExpressionValidator* text1Validator = new QRegularExpressionValidator(
        text1RegularExpression,
        text1LineEdit
    );
    text1LineEdit->setValidator(text1Validator);

    QRegularExpressionValidator* text2Validator = new QRegularExpressionValidator(
        text2RegularExpression,
        text2LineEdit
    );
    text2LineEdit->setValidator(text2Validator);

    showTimer = new QTimer(this);
    showTimer->setSingleShot(true);

    connect(text1LineEdit, &EQt::LineEdit::textEdited, this, &VariableNameFieldsWidget::text1Edited);
    connect(text2LineEdit, &EQt::LineEdit::textEdited, this, &VariableNameFieldsWidget::text2Edited);

    connect(text1LineEdit, &EQt::LineEdit::returnPressed, this, &VariableNameFieldsWidget::returnKeyPressed);
    connect(text2LineEdit, &EQt::LineEdit::returnPressed, this, &VariableNameFieldsWidget::returnKeyPressed);

    connect(text1LineEdit, &EQt::LineEdit::lostFocus, this, &VariableNameFieldsWidget::editorFocusChanged);
    connect(text2LineEdit, &EQt::LineEdit::lostFocus, this, &VariableNameFieldsWidget::editorFocusChanged);
    connect(text1LineEdit, &EQt::LineEdit::receivedFocus, this, &VariableNameFieldsWidget::editorFocusChanged);
    connect(text2LineEdit, &EQt::LineEdit::receivedFocus, this, &VariableNameFieldsWidget::editorFocusChanged);

    connect(showTimer, &QTimer::timeout, this, &VariableNameFieldsWidget::nowVisible);
}


void VariableNameFieldsWidget::buildRegularExpressions() {
    if (text1RegularExpression.pattern().isEmpty()) {
        QMap<Ld::SpecialCharacters::UnicodeValue, QString>
            latexCommandsByUnicodeValue = Ld::SpecialCharacters::characterCommandsByUnicodeValue(
                true,
                true,
                true,
                false
            );

        QString     specialCharacters;
        QStringList latexCommands;
        for (  QMap<Ld::SpecialCharacters::UnicodeValue, QString>::const_iterator
                   commandIterator    = latexCommandsByUnicodeValue.constBegin(),
                   commandEndIterator = latexCommandsByUnicodeValue.constEnd()
             ; commandIterator != commandEndIterator
             ; ++commandIterator
            ) {
            specialCharacters.append(QChar(commandIterator.key()));
            latexCommands << tr("\\\\%1").arg(commandIterator.value());
        }

        QString text1Regex = tr("([a-zA-Z%1]|%2)([a-zA-Z0-9%1]|%2)*")
                             .arg(specialCharacters)
                             .arg(latexCommands.join(tr("|")));
        QString text2Regex = tr("([a-zA-Z0-9%1]|%2)*")
                             .arg(specialCharacters)
                             .arg(latexCommands.join(tr("|")));

        text1RegularExpression.setPattern(text1Regex);
        Q_ASSERT(text1RegularExpression.isValid());

        text2RegularExpression.setPattern(text2Regex);
        Q_ASSERT(text2RegularExpression.isValid());
    }
}


QString VariableNameFieldsWidget::unLaTeXVariableName(const QString& newText, bool* isValid) {
    QString result = newText;
    int     latexCommandIndex;

    QMap<Ld::SpecialCharacters::UnicodeValue, QString>
        commandsByUnicode = Ld::SpecialCharacters::characterCommandsByUnicodeValue(true, true, true, false);

    QMap<Ld::SpecialCharacters::UnicodeValue, QString>::const_iterator beginIterator = commandsByUnicode.constBegin();
    QMap<Ld::SpecialCharacters::UnicodeValue, QString>::const_iterator endIterator   = commandsByUnicode.constEnd();

    bool valid = true;
    do {
        latexCommandIndex = result.indexOf(tr("\\"));
        if (latexCommandIndex >= 0) {
            QMap<Ld::SpecialCharacters::UnicodeValue, QString>::const_iterator iterator = beginIterator;
            while (iterator != endIterator                                                          &&
                   result.mid(latexCommandIndex + 1, iterator.value().length()) != iterator.value()    ) {
                ++iterator;
            }

            if (iterator != endIterator) {
                QChar    replacementCharacter(iterator.key());
                QString  command       = iterator.value();
                unsigned commandLength = static_cast<unsigned>(command.length() + 1);

                QString  before = result.left(latexCommandIndex);
                QString  after  = result.mid(latexCommandIndex + commandLength);

                result = before + replacementCharacter + after;
            } else {
                valid = false;
            }
        }
    } while (latexCommandIndex >= 0 && valid);

    if (isValid != Q_NULLPTR) {
        *isValid = valid;
    }

    return result;
}
