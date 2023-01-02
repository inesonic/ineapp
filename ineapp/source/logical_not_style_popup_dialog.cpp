/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LogicalNotStylePopupDialog class.
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
#include <ld_logical_unary_not_operator_format.h>

#include "application.h"
#include "logical_not_style_popup_dialog.h"

LogicalNotStylePopupDialog::LogicalNotStylePopupDialog(QWidget* parent):QDialog(parent, Qt::Popup) {
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
        tr("Click to select the default logical not symbol for the selected operation")
    );
    currentDefaultToolButton->setWhatsThis(
        tr("You can use this button to select the default logical not symbol based on the originally selected style.")
    );
    mainLayout->addWidget(currentDefaultToolButton);
    toolButtons.append(currentDefaultToolButton);

    currentNegationToolButton = new QToolButton();
    currentNegationToolButton->setText(tr("%1a").arg(QChar(0x00AC)));
    currentNegationToolButton->setFont(buttonFont);
    currentNegationToolButton->setToolTip(tr("Click to select a negation symbol, \"%1\"").arg(QChar(0x00AC)));
    currentNegationToolButton->setWhatsThis(
        tr("You can use this button to select a negation symbol, \"%1\"").arg(QChar(0x00AC))
    );
    mainLayout->addWidget(currentNegationToolButton);
    toolButtons.append(currentNegationToolButton);

    currentOverbarToolButton = new QToolButton();
    currentOverbarToolButton->setText(tr("a"));
    QFont overbarButtonFont = buttonFont;
    overbarButtonFont.setOverline(true);
    currentOverbarToolButton->setFont(overbarButtonFont);
    currentOverbarToolButton->setToolTip(tr("Click to select an overbar for negation"));
    currentOverbarToolButton->setWhatsThis(
        tr("You can use this button to present logical negation using an overbar")
    );
    mainLayout->addWidget(currentOverbarToolButton);
    toolButtons.append(currentOverbarToolButton);

    currentTildeToolButton = new QToolButton();
    currentTildeToolButton->setText(tr("~a"));
    currentTildeToolButton->setFont(buttonFont);
    currentTildeToolButton->setToolTip(tr("Click to select a tilde symbol, \"~\""));
    currentTildeToolButton->setWhatsThis(tr("You can use this button to select a tilde symbol, \"~\""));
    mainLayout->addWidget(currentTildeToolButton);
    toolButtons.append(currentTildeToolButton);

    QFontMetrics fontMetrics(buttonFont);
    unsigned     maximumSide = static_cast<unsigned>(3 * fontMetrics.height());

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(toolButtons.size()) ; ++bulletIndex) {
        QToolButton* toolButton = toolButtons.at(bulletIndex);
        unsigned     textWidth  = fontMetrics.horizontalAdvance(toolButton->text());
        if (textWidth > maximumSide) {
            maximumSide = textWidth;
        }
    }

    QSize buttonSize(1.25 * maximumSide, 1.25 * maximumSide);
    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(toolButtons.size()) ; ++bulletIndex) {
        QToolButton* toolButton = toolButtons.at(bulletIndex);
        toolButton->setFixedSize(buttonSize);
        toolButton->setCheckable(true);
    }

    connect(
        currentDefaultToolButton,
        &QToolButton::clicked,
        this,
        &LogicalNotStylePopupDialog::defaultButtonClicked
    );

    connect(
        currentNegationToolButton,
        &QToolButton::clicked,
        this,
        &LogicalNotStylePopupDialog::negationButtonClicked
    );

    connect(
        currentOverbarToolButton,
        &QToolButton::clicked,
        this,
        &LogicalNotStylePopupDialog::overbarButtonClicked
    );

    connect(
        currentTildeToolButton,
        &QToolButton::clicked,
        this,
        &LogicalNotStylePopupDialog::tildeButtonClicked
    );
}


LogicalNotStylePopupDialog::~LogicalNotStylePopupDialog() {}


void LogicalNotStylePopupDialog::setLogicalNotSymbol(LogicalNotStylePopupDialog::LogicalNotSymbol logicalNotSymbol) {
    QSet<LogicalNotSymbol> symbols;
    symbols << logicalNotSymbol;

    setLogicalNotSymbol(symbols);
}


void LogicalNotStylePopupDialog::setLogicalNotSymbol(
        const QSet<LogicalNotStylePopupDialog::LogicalNotSymbol>& logicalNotSymbols
    ) {
    currentDefaultToolButton->setChecked(logicalNotSymbols.contains(LogicalNotSymbol::DEFAULT));
    currentNegationToolButton->setChecked(logicalNotSymbols.contains(LogicalNotSymbol::NEGATION));
    currentOverbarToolButton->setChecked(logicalNotSymbols.contains(LogicalNotSymbol::OVERBAR));
    currentTildeToolButton->setChecked(logicalNotSymbols.contains(LogicalNotSymbol::TILDE));
}


void LogicalNotStylePopupDialog::focusOutEvent(QFocusEvent*) {
    QWidget* focusWidget = Application::focusWidget();

    if (focusWidget == this                      ||
        focusWidget == currentDefaultToolButton  ||
        focusWidget == currentNegationToolButton ||
        focusWidget == currentOverbarToolButton  ||
        focusWidget == currentTildeToolButton       ) {
        emit focusLost();
    }
}


void LogicalNotStylePopupDialog::defaultButtonClicked() {
    emit logicalNotSymbolSelected(LogicalNotSymbol::DEFAULT);
}


void LogicalNotStylePopupDialog::negationButtonClicked() {
    emit logicalNotSymbolSelected(LogicalNotSymbol::NEGATION);
}


void LogicalNotStylePopupDialog::overbarButtonClicked() {
    emit logicalNotSymbolSelected(LogicalNotSymbol::OVERBAR);
}


void LogicalNotStylePopupDialog::tildeButtonClicked() {
    emit logicalNotSymbolSelected(LogicalNotSymbol::TILDE);
}
