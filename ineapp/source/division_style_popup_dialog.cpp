/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DivisionStylePopupDialog class.
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
#include "division_style_popup_dialog.h"

DivisionStylePopupDialog::DivisionStylePopupDialog(QWidget* parent):QDialog(parent, Qt::Popup) {
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
        tr("Click to select the default division symbol for the type of division")
    );
    currentDefaultToolButton->setWhatsThis(
        tr("You can use this button to select the default division symbol for each division operation.")
    );
    mainLayout->addWidget(currentDefaultToolButton);
    toolButtons.append(currentDefaultToolButton);

    currentObelusToolButton = new QToolButton();
    currentObelusToolButton->setText(tr("a%1b").arg(QChar(0x00F7)));
    currentObelusToolButton->setFont(buttonFont);
    currentObelusToolButton->setToolTip(tr("Click to select an obelus between operands"));
    currentObelusToolButton->setWhatsThis(tr("You can use this button to select an obelus between operands."));
    mainLayout->addWidget(currentObelusToolButton);
    toolButtons.append(currentObelusToolButton);

    currentFractionToolButton = new QToolButton();
    currentFractionToolButton->setText(tr("a\n%1%1%1\nb").arg(QChar(0x2500)));
    currentFractionToolButton->setFont(buttonFont);
    currentFractionToolButton->setToolTip(tr("Click to present division as a fractions"));
    currentFractionToolButton->setWhatsThis(
        tr("You can use this button to present your division operation(s) as fractions.")
    );
    mainLayout->addWidget(currentFractionToolButton);
    toolButtons.append(currentFractionToolButton);

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
        &DivisionStylePopupDialog::defaultButtonClicked
    );

    connect(
        currentObelusToolButton,
        &QToolButton::clicked,
        this,
        &DivisionStylePopupDialog::obelusButtonClicked
    );

    connect(
        currentFractionToolButton,
        &QToolButton::clicked,
        this,
        &DivisionStylePopupDialog::fractionButtonClicked
    );
}


DivisionStylePopupDialog::~DivisionStylePopupDialog() {}


void DivisionStylePopupDialog::setDivisionSymbol(
        DivisionStylePopupDialog::DivisionSymbol divisionSymbol
    ) {
    QSet<DivisionSymbol> symbols;
    symbols << divisionSymbol;

    setDivisionSymbol(symbols);
}


void DivisionStylePopupDialog::setDivisionSymbol(
        const QSet<DivisionStylePopupDialog::DivisionSymbol>& divisionSymbols
    ) {
    currentDefaultToolButton->setChecked(divisionSymbols.contains(DivisionSymbol::DEFAULT));
    currentObelusToolButton->setChecked(divisionSymbols.contains(DivisionSymbol::OBELUS));
    currentFractionToolButton->setChecked(divisionSymbols.contains(DivisionSymbol::FRACTION));
}


void DivisionStylePopupDialog::focusOutEvent(QFocusEvent*) {
    QWidget* focusWidget = Application::focusWidget();

    if (focusWidget == this                      ||
        focusWidget == currentDefaultToolButton  ||
        focusWidget == currentObelusToolButton   ||
        focusWidget == currentFractionToolButton    ) {
        emit focusLost();
    }
}


void DivisionStylePopupDialog::defaultButtonClicked() {
    emit divisionSymbolSelected(DivisionSymbol::DEFAULT);
}


void DivisionStylePopupDialog::obelusButtonClicked() {
    emit divisionSymbolSelected(DivisionSymbol::OBELUS);
}


void DivisionStylePopupDialog::fractionButtonClicked() {
    emit divisionSymbolSelected(DivisionSymbol::FRACTION);
}
