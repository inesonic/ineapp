/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MultiplicationStylePopupDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QPoint>
#include <QSet>
#include <QHash>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>

#include <ld_character_format.h>
#include <ld_operator_format.h>

#include "application.h"
#include "multiplication_style_popup_dialog.h"

MultiplicationStylePopupDialog::MultiplicationStylePopupDialog(QWidget* parent):QDialog(parent, Qt::Popup) {
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    setLayout(mainLayout);

    QList<QToolButton*> toolButtons;

    QFont buttonFont = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    buttonFont.setPointSize(18);

    currentDefaultToolButton = new QToolButton();
    currentDefaultToolButton->setText(tr("Default"));
    currentDefaultToolButton->setFont(buttonFont);
    currentDefaultToolButton->setToolTip(
        tr("Click to select the default multiplication symbol for the type of multiplication")
    );
    currentDefaultToolButton->setWhatsThis(
        tr("You can use this button to select the default multiplication symbol for each multiplication operation.")
    );
    mainLayout->addWidget(currentDefaultToolButton);
    toolButtons.append(currentDefaultToolButton);

    currentSpaceToolButton = new QToolButton();
    currentSpaceToolButton->setText(tr("a%1b").arg(QChar(0x2009)));
    currentSpaceToolButton->setFont(buttonFont);
    currentSpaceToolButton->setToolTip(tr("Click to select a small space between operands"));
    currentSpaceToolButton->setWhatsThis(tr("You can use this button to select a small space between operands."));
    mainLayout->addWidget(currentSpaceToolButton);
    toolButtons.append(currentSpaceToolButton);

    currentDotToolButton = new QToolButton();
    currentDotToolButton->setText(tr("a%1b").arg(QChar(0x2219)));
    currentDotToolButton->setFont(buttonFont);
    currentDotToolButton->setToolTip(tr("Click to select a small middle dot betwen operands"));
    currentDotToolButton->setWhatsThis(
        tr("You can use this button to select a small middle dot between operands.")
    );
    mainLayout->addWidget(currentDotToolButton);
    toolButtons.append(currentDotToolButton);

    currentCrossToolButton = new QToolButton();
    currentCrossToolButton->setText(tr("a%1b").arg(QChar(0x00D7)));
    currentCrossToolButton->setFont(buttonFont);
    currentCrossToolButton->setToolTip(tr("Click to select a cross multiplication symbol betwen operands"));
    currentCrossToolButton->setWhatsThis(
        tr("You can use this button to select a cross multiplication symbol between operands.")
    );
    mainLayout->addWidget(currentCrossToolButton);
    toolButtons.append(currentCrossToolButton);

    currentCircleToolButton = new QToolButton();
    currentCircleToolButton->setText(tr("a%1b").arg(QChar(0x2218)));
    currentCircleToolButton->setFont(buttonFont);
    currentCircleToolButton->setToolTip(tr("Click to select a small circle symbol betwen operands"));
    currentCircleToolButton->setWhatsThis(
        tr("You can use this button to select a small circule symbol between operands.")
    );
    mainLayout->addWidget(currentCircleToolButton);
    toolButtons.append(currentCircleToolButton);

    QFontMetrics fontMetrics(buttonFont);
    unsigned     maximumSide = static_cast<unsigned>(fontMetrics.height());

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(toolButtons.size()) ; ++bulletIndex) {
        QToolButton* toolButton = toolButtons.at(bulletIndex);
        unsigned     textWidth  = fontMetrics.horizontalAdvance(toolButton->text());
        if (textWidth > maximumSide) {
            maximumSide = textWidth;
        }
    }

    QSize buttonSize(1.5 * maximumSide, 1.5 * maximumSide);
    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(toolButtons.size()) ; ++bulletIndex) {
        QToolButton* toolButton = toolButtons.at(bulletIndex);
        toolButton->setFixedSize(buttonSize);
        toolButton->setCheckable(true);
    }

    connect(
        currentDefaultToolButton,
        &QToolButton::clicked,
        this,
        &MultiplicationStylePopupDialog::defaultButtonClicked
    );

    connect(
        currentSpaceToolButton,
        &QToolButton::clicked,
        this,
        &MultiplicationStylePopupDialog::spaceButtonClicked
    );

    connect(
        currentDotToolButton,
        &QToolButton::clicked,
        this,
        &MultiplicationStylePopupDialog::dotButtonClicked
    );

    connect(
        currentCrossToolButton,
        &QToolButton::clicked,
        this,
        &MultiplicationStylePopupDialog::crossButtonClicked
    );

    connect(
        currentCircleToolButton,
        &QToolButton::clicked,
        this,
        &MultiplicationStylePopupDialog::circleButtonClicked
    );
}


MultiplicationStylePopupDialog::~MultiplicationStylePopupDialog() {}


void MultiplicationStylePopupDialog::setMultiplicationSymbol(
        MultiplicationStylePopupDialog::MultiplicationSymbol multiplicationSymbol
    ) {
    QSet<MultiplicationSymbol> symbols;
    symbols << multiplicationSymbol;

    setMultiplicationSymbol(symbols);
}


void MultiplicationStylePopupDialog::setMultiplicationSymbol(
        const QSet<MultiplicationStylePopupDialog::MultiplicationSymbol>& multiplicationSymbols
    ) {
    currentDefaultToolButton->setChecked(multiplicationSymbols.contains(MultiplicationSymbol::DEFAULT));
    currentSpaceToolButton->setChecked(multiplicationSymbols.contains(MultiplicationSymbol::SPACE));
    currentDotToolButton->setChecked(multiplicationSymbols.contains(MultiplicationSymbol::DOT));
    currentCrossToolButton->setChecked(multiplicationSymbols.contains(MultiplicationSymbol::CROSS));
    currentCircleToolButton->setChecked(multiplicationSymbols.contains(MultiplicationSymbol::CIRCLE));
}


void MultiplicationStylePopupDialog::focusOutEvent(QFocusEvent*) {
    QWidget* focusWidget = Application::focusWidget();

    if (focusWidget == this                     ||
        focusWidget == currentDefaultToolButton ||
        focusWidget == currentSpaceToolButton   ||
        focusWidget == currentDotToolButton     ||
        focusWidget == currentCrossToolButton   ||
        focusWidget == currentCircleToolButton     ) {
        emit focusLost();
    }
}


void MultiplicationStylePopupDialog::defaultButtonClicked() {
    emit multiplicationSymbolSelected(MultiplicationSymbol::DEFAULT);
}


void MultiplicationStylePopupDialog::spaceButtonClicked() {
    emit multiplicationSymbolSelected(MultiplicationSymbol::SPACE);
}


void MultiplicationStylePopupDialog::dotButtonClicked() {
    emit multiplicationSymbolSelected(MultiplicationSymbol::DOT);
}


void MultiplicationStylePopupDialog::crossButtonClicked() {
    emit multiplicationSymbolSelected(MultiplicationSymbol::CROSS);
}


void MultiplicationStylePopupDialog::circleButtonClicked() {
    emit multiplicationSymbolSelected(MultiplicationSymbol::CIRCLE);
}
