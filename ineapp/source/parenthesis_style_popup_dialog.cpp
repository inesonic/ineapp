/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ParenthesisStylePopupDialog class.
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
#include "parenthesis_style_popup_dialog.h"

ParenthesisStylePopupDialog::ParenthesisStylePopupDialog(QWidget* parent):QDialog(parent, Qt::Popup) {
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QGroupBox* parenthesisStyleGroupBox = new QGroupBox(tr("Parenthesis Styles"));
    mainLayout->addWidget(parenthesisStyleGroupBox);

    QHBoxLayout* parenthesisStyleLayout = new QHBoxLayout;
    parenthesisStyleGroupBox->setLayout(parenthesisStyleLayout);

    QList<QToolButton*> toolButtons;

    QFont buttonFont = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    buttonFont.setPointSize(18);

    defaultParenthesisToolButton = new QToolButton();
    defaultParenthesisToolButton->setText(tr("Default"));
    defaultParenthesisToolButton->setFont(buttonFont);
    defaultParenthesisToolButton->setToolTip(tr("Click to select the default parenthesis style for each operator"));
    defaultParenthesisToolButton->setWhatsThis(
        tr("You can use this button to select the default parenthesis style for each operator.")
    );
    parenthesisStyleLayout->addWidget(defaultParenthesisToolButton);
    toolButtons.append(defaultParenthesisToolButton);

    parenthesisToolButton = new QToolButton();
    parenthesisToolButton->setText(tr("( )"));
    parenthesisToolButton->setFont(buttonFont);
    parenthesisToolButton->setToolTip(tr("Click to select traditional parenthesis"));
    parenthesisToolButton->setWhatsThis(tr("You can use this button to select traditional parenthesis."));
    parenthesisStyleLayout->addWidget(parenthesisToolButton);
    toolButtons.append(parenthesisToolButton);

    bracketsToolButton = new QToolButton();
    bracketsToolButton->setText(tr("[ ]"));
    bracketsToolButton->setFont(buttonFont);
    bracketsToolButton->setToolTip(tr("Click to select square brackets"));
    bracketsToolButton->setWhatsThis(tr("You can use this button to select square brackets."));
    parenthesisStyleLayout->addWidget(bracketsToolButton);
    toolButtons.append(bracketsToolButton);

    bracesToolButton = new QToolButton();
    bracesToolButton->setText(tr("{ }"));
    bracesToolButton->setFont(buttonFont);
    bracesToolButton->setToolTip(tr("Click to select braces"));
    bracesToolButton->setWhatsThis(tr("You can use this button to select braces."));
    parenthesisStyleLayout->addWidget(bracesToolButton);
    toolButtons.append(bracesToolButton);

    forceParenthesisCheckBox = new QCheckBox(tr("Force parenthesis"));
    forceParenthesisCheckBox->setTristate(true);
    forceParenthesisCheckBox->setToolTip(tr("Click to force parenthesis."));
    forceParenthesisCheckBox->setWhatsThis(
        tr(
           "You can click this check-box to force parenthesis around this operator.  If un-checked, parenthesis will "
           "be displayed automatically based on the operator precedence."
          )
    );
    mainLayout->addWidget(forceParenthesisCheckBox);

    mainLayout->addSpacing(5);

    parameterParenthesisStyleGroupBox = new QGroupBox(tr("Function Parameter Parenthesis Styles"));
    mainLayout->addWidget(parameterParenthesisStyleGroupBox);

    QHBoxLayout* parameterParenthesisStyleLayout = new QHBoxLayout;
    parameterParenthesisStyleGroupBox->setLayout(parameterParenthesisStyleLayout);

    parameterParenthesisToolButton = new QToolButton();
    parameterParenthesisToolButton->setText(tr("( )"));
    parameterParenthesisToolButton->setFont(buttonFont);
    parameterParenthesisToolButton->setToolTip(tr("Click to select traditional parenthesis"));
    parameterParenthesisToolButton->setWhatsThis(tr("You can use this button to select traditional parenthesis."));
    parameterParenthesisStyleLayout->addWidget(parameterParenthesisToolButton);
    toolButtons.append(parameterParenthesisToolButton);

    parameterBracketsToolButton = new QToolButton();
    parameterBracketsToolButton->setText(tr("[ ]"));
    parameterBracketsToolButton->setFont(buttonFont);
    parameterBracketsToolButton->setToolTip(tr("Click to select square brackets"));
    parameterBracketsToolButton->setWhatsThis(tr("You can use this button to select square brackets."));
    parameterParenthesisStyleLayout->addWidget(parameterBracketsToolButton);
    toolButtons.append(parameterBracketsToolButton);

    parameterBracesToolButton = new QToolButton();
    parameterBracesToolButton->setText(tr("{ }"));
    parameterBracesToolButton->setFont(buttonFont);
    parameterBracesToolButton->setToolTip(tr("Click to select braces"));
    parameterBracesToolButton->setWhatsThis(tr("You can use this button to select braces."));
    parameterParenthesisStyleLayout->addWidget(parameterBracesToolButton);
    toolButtons.append(parameterBracesToolButton);

    forceParameterParenthesisCheckBox = new QCheckBox(tr("Force parenthesis around function parameters"));
    forceParameterParenthesisCheckBox->setTristate(true);
    forceParameterParenthesisCheckBox->setToolTip(tr("Click to force parenthesis around function parameters."));
    forceParameterParenthesisCheckBox->setWhatsThis(
        tr(
           "You can click this check-box to force parenthesis around the function parameters.  If un-checked, "
           "parenthesis will be displayed automatically based on the number of function parameters and the placement "
           "of the function."
          )
    );
    mainLayout->addWidget(forceParameterParenthesisCheckBox);

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
        defaultParenthesisToolButton,
        &QToolButton::clicked,
        this,
        &ParenthesisStylePopupDialog::defaultParenthesisButtonClicked
    );

    connect(
        parenthesisToolButton,
        &QToolButton::clicked,
        this,
        &ParenthesisStylePopupDialog::parenthesisButtonClicked
    );

    connect(
        bracketsToolButton,
        &QToolButton::clicked,
        this,
        &ParenthesisStylePopupDialog::bracketButtonClicked
    );

    connect(
        bracesToolButton,
        &QToolButton::clicked,
        this,
        &ParenthesisStylePopupDialog::braceButtonClicked
    );

    connect(
        forceParenthesisCheckBox,
        static_cast<void (QCheckBox::*)(bool)>(&QCheckBox::clicked),
        this,
        &ParenthesisStylePopupDialog::forceParenthesisButtonClicked
    );

    connect(
        parameterParenthesisToolButton,
        &QToolButton::clicked,
        this,
        &ParenthesisStylePopupDialog::parameterParenthesisButtonClicked
    );

    connect(
        parameterBracketsToolButton,
        &QToolButton::clicked,
        this,
        &ParenthesisStylePopupDialog::parameterBracketButtonClicked
    );

    connect(
        parameterBracesToolButton,
        &QToolButton::clicked,
        this,
        &ParenthesisStylePopupDialog::parameterBraceButtonClicked
    );

    connect(
        forceParameterParenthesisCheckBox,
        static_cast<void (QCheckBox::*)(bool)>(&QCheckBox::clicked),
        this,
        &ParenthesisStylePopupDialog::forceParameterParenthesisButtonClicked
    );
}


ParenthesisStylePopupDialog::~ParenthesisStylePopupDialog() {}


bool ParenthesisStylePopupDialog::forceParenthesisCheckboxVisible() const {
    return forceParenthesisCheckBox->isVisible();
}


bool ParenthesisStylePopupDialog::forceParenthesisCheckboxHidden() const {
    return !forceParenthesisCheckboxVisible();
}


bool ParenthesisStylePopupDialog::defaultParenthesisStyleButtonVisible() const {
    return defaultParenthesisToolButton->isVisible();
}


bool ParenthesisStylePopupDialog::defaultParenthesisStyleButtonHidden() const {
    return !defaultParenthesisStyleButtonVisible();
}


bool ParenthesisStylePopupDialog::functionParameterParenthesisButtonsVisible() const {
    return parameterParenthesisStyleGroupBox->isVisible();
}


bool ParenthesisStylePopupDialog::functionParameterParenthesisButtonsHidden() const {
    return !functionParameterParenthesisButtonsVisible();
}


bool ParenthesisStylePopupDialog::forceFunctionParameterParenthesisCheckboxVisible() const {
    return forceParameterParenthesisCheckBox->isVisible();
}


bool ParenthesisStylePopupDialog::forceFunctionParameterParenthesisCheckboxHidden() const {
    return !forceFunctionParameterParenthesisCheckboxVisible();
}


void ParenthesisStylePopupDialog::setParenthesisStyle(ParenthesisStylePopupDialog::ParenthesisStyle parenthesisStyle) {
    QSet<ParenthesisStyle> styles;
    styles << parenthesisStyle;

    setParenthesisStyle(styles);
}


void ParenthesisStylePopupDialog::setParenthesisStyle(
        const QSet<ParenthesisStylePopupDialog::ParenthesisStyle>& parenthesisStyles
    ) {
    defaultParenthesisToolButton->setChecked(parenthesisStyles.contains(ParenthesisStyle::DEFAULT));
    parenthesisToolButton->setChecked(parenthesisStyles.contains(ParenthesisStyle::PARENTHESIS));
    bracketsToolButton->setChecked(parenthesisStyles.contains(ParenthesisStyle::BRACKETS));
    bracesToolButton->setChecked(parenthesisStyles.contains(ParenthesisStyle::BRACES));
}


void ParenthesisStylePopupDialog::setForceParenthesisButtonChecked(bool forceParenthesisChecked) {
    forceParenthesisCheckBox->setTristate(false);
    forceParenthesisCheckBox->setChecked(forceParenthesisChecked);
}


void ParenthesisStylePopupDialog::setForceParenthesisButtonUnchecked(bool forceParenthesisUnchecked) {
    setForceParenthesisButtonChecked(!forceParenthesisUnchecked);
}


void ParenthesisStylePopupDialog::setForceParenthesisTriStated() {
    forceParenthesisCheckBox->setTristate(true);
    forceParenthesisCheckBox->setCheckState(Qt::PartiallyChecked);
}


void ParenthesisStylePopupDialog::setForceParenthesisCheckboxVisible(bool nowVisible) {
    forceParenthesisCheckBox->setVisible(nowVisible);
}


void ParenthesisStylePopupDialog::setForceParenthesisCheckboxHidden(bool nowHidden) {
    setForceParenthesisCheckboxVisible(!nowHidden);
}


void ParenthesisStylePopupDialog::setDefaultParenthesisStyleButtonVisible(bool nowVisible) {
    defaultParenthesisToolButton->setVisible(nowVisible);
}


void ParenthesisStylePopupDialog::setDefaultParenthesisStyleButtonHidden(bool nowHidden) {
    setDefaultParenthesisStyleButtonVisible(!nowHidden);
}


void ParenthesisStylePopupDialog::setParameterParenthesisStyle(
        ParenthesisStylePopupDialog::ParenthesisStyle parenthesisStyle
    ) {
    setParameterParenthesisStyle(QSet<ParenthesisStyle>() << parenthesisStyle);
}


void ParenthesisStylePopupDialog::setParameterParenthesisStyle(
        const QSet<ParenthesisStylePopupDialog::ParenthesisStyle>& parenthesisStyles
    ) {
    parameterParenthesisToolButton->setChecked(parenthesisStyles.contains(ParenthesisStyle::PARENTHESIS));
    parameterBracketsToolButton->setChecked(parenthesisStyles.contains(ParenthesisStyle::BRACKETS));
    parameterBracesToolButton->setChecked(parenthesisStyles.contains(ParenthesisStyle::BRACES));
}


void ParenthesisStylePopupDialog::setForceParameterParenthesisButtonChecked(bool forceParenthesisChecked) {
    forceParameterParenthesisCheckBox->setChecked(forceParenthesisChecked);
    forceParameterParenthesisCheckBox->setTristate(false);
}


void ParenthesisStylePopupDialog::setForceParameterParenthesisButtonUnchecked(bool forceParenthesisUnchecked) {
    setForceParameterParenthesisButtonChecked(forceParenthesisUnchecked);
}


void ParenthesisStylePopupDialog::setForceParameterParenthesisTriStated() {
    forceParameterParenthesisCheckBox->setCheckState(Qt::PartiallyChecked);

}


void ParenthesisStylePopupDialog::setForceParameterParenthesisCheckboxVisible(bool nowVisible) {
    forceParameterParenthesisCheckBox->setVisible(nowVisible);
}


void ParenthesisStylePopupDialog::setForceParameterParenthesisCheckboxHidden(bool nowHidden) {
    setForceParameterParenthesisCheckboxVisible(!nowHidden);
}


void ParenthesisStylePopupDialog::setParameterParenthesisButtonsVisible(bool nowVisible) {
    parameterParenthesisStyleGroupBox->setVisible(nowVisible);
}


void ParenthesisStylePopupDialog::setParameterParenthesisButtonsHidden(bool nowHidden) {
    setParameterParenthesisButtonsVisible(!nowHidden);
}


void ParenthesisStylePopupDialog::focusOutEvent(QFocusEvent*) {
    QWidget* focusWidget = Application::focusWidget();

    if (focusWidget == this                         ||
        focusWidget == defaultParenthesisToolButton ||
        focusWidget == parenthesisToolButton        ||
        focusWidget == bracketsToolButton           ||
        focusWidget == bracesToolButton             ||
        focusWidget == forceParenthesisCheckBox        ) {
        emit focusLost();
    }
}


void ParenthesisStylePopupDialog::defaultParenthesisButtonClicked() {
    emit parenthesisStyleSelected(ParenthesisStyle::DEFAULT);
}


void ParenthesisStylePopupDialog::parenthesisButtonClicked() {
    emit parenthesisStyleSelected(ParenthesisStyle::PARENTHESIS);
}


void ParenthesisStylePopupDialog::bracketButtonClicked() {
    emit parenthesisStyleSelected(ParenthesisStyle::BRACKETS);
}


void ParenthesisStylePopupDialog::braceButtonClicked() {
    emit parenthesisStyleSelected(ParenthesisStyle::BRACES);
}


void ParenthesisStylePopupDialog::forceParenthesisButtonClicked(bool nowChecked) {
    forceParenthesisCheckBox->setTristate(false);
    emit forceParenthesisButtonChecked(nowChecked);
}


void ParenthesisStylePopupDialog::parameterParenthesisButtonClicked() {
    emit parameterParenthesisStyleSelected(ParenthesisStyle::PARENTHESIS);
}


void ParenthesisStylePopupDialog::parameterBracketButtonClicked() {
    emit parameterParenthesisStyleSelected(ParenthesisStyle::BRACKETS);
}


void ParenthesisStylePopupDialog::parameterBraceButtonClicked() {
    emit parameterParenthesisStyleSelected(ParenthesisStyle::BRACES);
}


void ParenthesisStylePopupDialog::forceParameterParenthesisButtonClicked(bool nowChecked) {
    forceParameterParenthesisCheckBox->setTristate(false);
    emit forceParameterParenthesisButtonChecked(nowChecked);
}
