/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref VariableNameSpecialCharactersWidget class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>
#include <QList>
#include <QWidget>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QFontMetrics>

#include <eqt_tool_button.h>
#include <eqt_tool_button_array.h>

#include <ld_character_format.h>
#include <ld_special_characters.h>

#include "application.h"
#include "presentation.h"
#include "variable_name_special_characters_widget.h"

VariableNameSpecialCharactersWidget::VariableNameSpecialCharactersWidget(QWidget* parent):QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QList<EQt::ToolButton*> characterButtons;

    QList<Ld::SpecialCharacters::UnicodeValue> greekCharacters = Ld::SpecialCharacters::recommendedCharacterOrder(
        true,
        false,
        false,
        false
    );

    greekToolButtonArray = buildButtonArray(greekCharacters, 5, characterButtons);
    layout->addWidget(greekToolButtonArray);

    QList<Ld::SpecialCharacters::UnicodeValue> specialCharacters = Ld::SpecialCharacters::recommendedCharacterOrder(
        false,
        false,
        true,
        false
    );

    specialCharactersButtonArray = buildButtonArray(specialCharacters, 5, characterButtons);
    layout->addWidget(specialCharactersButtonArray);

    QFont defaultMathIdentifierFont = Ld::CharacterFormat::applicationDefaultMathIdentifierFont()->toQFont();
    QFontMetrics defaultMathIdentifierFontMetrics(defaultMathIdentifierFont);

    unsigned maximumSide = static_cast<unsigned>(defaultMathIdentifierFontMetrics.height());
    for (  QList<EQt::ToolButton*>::const_iterator buttonIterator    = characterButtons.constBegin(),
                                                   buttonEndIterator = characterButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        EQt::ToolButton* button = *buttonIterator;
        button->setFont(defaultMathIdentifierFont);

        QString  buttonText      = button->text();
        unsigned buttonTextWidth = static_cast<unsigned>(
            defaultMathIdentifierFontMetrics.horizontalAdvance(buttonText)
        );

        if (buttonTextWidth > maximumSide) {
            maximumSide = buttonTextWidth;
        }
    }

    unsigned buttonSide = maximumSide * 1.25;
    for (  QList<EQt::ToolButton*>::const_iterator buttonIterator    = characterButtons.constBegin(),
                                                   buttonEndIterator = characterButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        EQt::ToolButton* button = *buttonIterator;
        button->setFixedSize(buttonSide, buttonSide);
    }

    connect(
        greekToolButtonArray,
        static_cast<void (EQt::ToolButtonArray::*)(EQt::ToolButton*)>(&EQt::ToolButtonArray::buttonClicked),
        this,
        &VariableNameSpecialCharactersWidget::buttonClicked
    );

    connect(
        specialCharactersButtonArray,
        static_cast<void (EQt::ToolButtonArray::*)(EQt::ToolButton*)>(&EQt::ToolButtonArray::buttonClicked),
        this,
        &VariableNameSpecialCharactersWidget::buttonClicked
    );
}


VariableNameSpecialCharactersWidget::~VariableNameSpecialCharactersWidget() {}


void VariableNameSpecialCharactersWidget::buttonClicked(EQt::ToolButton* button) {
    QString character = button->text();
    emit selected(character);
}


EQt::ToolButtonArray* VariableNameSpecialCharactersWidget::buildButtonArray(
        const QList<Ld::SpecialCharacters::UnicodeValue>& characterList,
        unsigned                                          maximumHeight,
        QList<EQt::ToolButton*>&                          buttonList
    ) {
    EQt::ToolButtonArray* array = new EQt::ToolButtonArray(
        EQt::ToolButtonArray::FixedDimension::HEIGHT,
        maximumHeight
    );
    array->setButtonSpacing(1);

    static QMap<Ld::SpecialCharacters::UnicodeValue, QString>
        descriptionsByUnicode = Ld::SpecialCharacters::characterDescriptionsByUnicodeValue(true, true, true, true);

    for (  QList<Ld::SpecialCharacters::UnicodeValue>::const_iterator unicodeIterator    = characterList.constBegin(),
                                                                      unicodeEndIterator = characterList.constEnd()
         ; unicodeIterator != unicodeEndIterator
         ; ++unicodeIterator
        ) {
        Ld::SpecialCharacters::UnicodeValue unicode     = *unicodeIterator;
        QString                             description = descriptionsByUnicode.value(unicode);
        EQt::ToolButton*                    toolButton  = new EQt::ToolButton;

        toolButton->setText(QString(QChar(unicode)));
        toolButton->setToolTip(description);
        toolButton->setGlobalScaleFactorSettingsDisabled();

        array->addButton(toolButton);
        buttonList.append(toolButton);
    }

    return array;
}
