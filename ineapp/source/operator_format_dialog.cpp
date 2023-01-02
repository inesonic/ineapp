/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref OperatorFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QSet>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QToolButton>
#include <QPushButton>
#include <QCheckBox>
#include <QFont>
#include <QFontMetrics>
#include <QDialogButtonBox>

#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_character_format.h>

#include "application.h"
#include "operator_format_dialog.h"

OperatorFormatDialog::OperatorFormatDialog(QWidget* parent):ProgrammaticDialog("OperatorFormatDialog", parent) {
    configureWidget();
    runBuilders();
}


OperatorFormatDialog::~OperatorFormatDialog() {}


void OperatorFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void OperatorFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool OperatorFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool OperatorFormatDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


OperatorFormatDialog::ParenthesisStyle OperatorFormatDialog::parenthesisStyle() const {
    ParenthesisStyle result;

    QToolButton* defaultParenthesisToolButton = widget<QToolButton>("default_parenthesis_tool_button");
    QToolButton* parenthesisToolButton        = widget<QToolButton>("parenthesis_tool_button");
    QToolButton* bracketsToolButton           = widget<QToolButton>("brackets_tool_button");
    QToolButton* bracesToolButton             = widget<QToolButton>("braces_tool_button");

    bool defaultParenthesisToolButtonChecked = defaultParenthesisToolButton->isChecked();
    bool parenthesisToolButtonChecked        = parenthesisToolButton->isChecked();
    bool bracketsToolButtonChecked           = bracketsToolButton->isChecked();
    bool bracesToolButtonChecked             = bracesToolButton->isChecked();

    if        ( defaultParenthesisToolButtonChecked && !parenthesisToolButtonChecked &&
               !bracketsToolButtonChecked           && !bracesToolButtonChecked         ) {
        result = ParenthesisStyle::DEFAULT;
    } else if (!defaultParenthesisToolButtonChecked &&  parenthesisToolButtonChecked &&
               !bracketsToolButtonChecked           && !bracesToolButtonChecked         ) {
        result = ParenthesisStyle::PARENTHESIS;
    } else if (!defaultParenthesisToolButtonChecked && !parenthesisToolButtonChecked &&
                bracketsToolButtonChecked           && !bracesToolButtonChecked         ) {
        result = ParenthesisStyle::BRACKETS;
    } else if (!defaultParenthesisToolButtonChecked && !parenthesisToolButtonChecked &&
               !bracketsToolButtonChecked           &&  bracesToolButtonChecked         ) {
        result = ParenthesisStyle::BRACES;
    } else {
        result = ParenthesisStyle::INVALID_OR_NONE;
    }

    return result;
}


bool OperatorFormatDialog::forceParenthesis() const {
    QCheckBox* forceParenthesisCheckBox = widget<QCheckBox>("force_parenthesis_checkbox");
    return forceParenthesisCheckBox->checkState() == Qt::Checked;
}


bool OperatorFormatDialog::forceParenthesisSet() const {
    QCheckBox* forceParenthesisCheckBox = widget<QCheckBox>("force_parenthesis_checkbox");
    return forceParenthesisCheckBox->checkState() != Qt::PartiallyChecked;
}


OperatorFormatDialog::ParenthesisStyle OperatorFormatDialog::parameterParenthesisStyle() const {
    ParenthesisStyle result;

    QToolButton* parenthesisToolButton = widget<QToolButton>("parameter_parenthesis_tool_button");
    QToolButton* bracketsToolButton    = widget<QToolButton>("parameter_brackets_tool_button");
    QToolButton* bracesToolButton      = widget<QToolButton>("parameter_braces_tool_button");

    bool parenthesisToolButtonChecked = parenthesisToolButton->isChecked();
    bool bracketsToolButtonChecked    = bracketsToolButton->isChecked();
    bool bracesToolButtonChecked      = bracesToolButton->isChecked();

    if        ( parenthesisToolButtonChecked && !bracketsToolButtonChecked && !bracesToolButtonChecked) {
        result = ParenthesisStyle::PARENTHESIS;
    } else if (!parenthesisToolButtonChecked &&  bracketsToolButtonChecked && !bracesToolButtonChecked) {
        result = ParenthesisStyle::BRACKETS;
    } else if (!parenthesisToolButtonChecked && !bracketsToolButtonChecked &&  bracesToolButtonChecked) {
        result = ParenthesisStyle::BRACES;
    } else {
        result = ParenthesisStyle::INVALID_OR_NONE;
    }

    return result;
}


bool OperatorFormatDialog::forceParameterParenthesis() const {
    QCheckBox* forceParameterParenthesisCheckBox = widget<QCheckBox>("force_parameter_parenthesis_checkbox");
    return forceParameterParenthesisCheckBox->checkState() == Qt::Checked;
}


bool OperatorFormatDialog::forceParameterParenthesisSet() const {
    QCheckBox* forceParameterParenthesisCheckBox = widget<QCheckBox>("force_parameter_parenthesis_checkbox");
    return forceParameterParenthesisCheckBox->checkState() != Qt::PartiallyChecked;
}


bool OperatorFormatDialog::subscriptedFirstParameter() const {
    QCheckBox* subscriptFirstFunctionParameterCheckBox = widget<QCheckBox>(
        "subscript_first_function_parameter_checkbox"
    );
    return subscriptFirstFunctionParameterCheckBox->checkState() == Qt::Checked;
}


bool OperatorFormatDialog::subscriptedFirstParameterSet() const {
    QCheckBox* subscriptFirstFunctionParameterCheckBox = widget<QCheckBox>(
        "subscript_first_function_parameter_checkbox"
    );
    return subscriptFirstFunctionParameterCheckBox->checkState() != Qt::PartiallyChecked;
}


bool OperatorFormatDialog::forceParenthesisCheckboxVisible() const {
    return inputForceParenthesisVisible;
}


bool OperatorFormatDialog::forceParenthesisCheckboxHidden() const {
    return !inputForceParenthesisVisible;
}


bool OperatorFormatDialog::defaultParenthesisStyleButtonVisible() const {
    return inputDefaultParenthesisStyleVisible;
}


bool OperatorFormatDialog::defaultParenthesisStyleButtonHidden() const {
    return !inputDefaultParenthesisStyleVisible;
}


bool OperatorFormatDialog::parameterParenthesisButtonsVisible() {
    QGroupBox* parameterParenthesisStyleGroupBox = widget<QGroupBox>("parameter_parenthesis_style_group_box");
    return parameterParenthesisStyleGroupBox->isVisible();
}


bool OperatorFormatDialog::parameterParenthesisButtonsHidden() {
    return !parameterParenthesisButtonsVisible();
}


bool OperatorFormatDialog::forceParameterParenthesisCheckboxVisible() {
    QCheckBox* forceParameterParenthesisCheckBox = widget<QCheckBox>("force_parameter_parenthesis_checkbox");
    return forceParameterParenthesisCheckBox->isVisible();
}


bool OperatorFormatDialog::forceParameterParenthesisCheckboxHidden() {
    return !forceParameterParenthesisCheckboxVisible();
}


bool OperatorFormatDialog::subscriptedFirstParameterCheckboxVisible() {
    QCheckBox* subscriptFirstFunctionParameterCheckBox = widget<QCheckBox>(
        "subscript_first_function_parameter_checkbox"
    );
    return subscriptFirstFunctionParameterCheckBox->isVisible();
}


bool OperatorFormatDialog::subscriptedFirstParameterCheckboxHidden() {
    return !subscriptedFirstParameterCheckboxVisible();
}


OperatorFormatDialog::MultiplicationSymbol OperatorFormatDialog::multiplicationSymbol() const {
    MultiplicationSymbol result;

    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");

    bool defaultMultiplicationStyleToolButtonChecked = defaultMultiplicationStyleToolButton->isChecked();
    bool spaceMultiplicationStyleToolButtonChecked = spaceMultiplicationStyleToolButton->isChecked();
    bool dotMultiplicationStyleToolButtonChecked = dotMultiplicationStyleToolButton->isChecked();
    bool crossMultiplicationStyleToolButtonChecked = crossMultiplicationStyleToolButton->isChecked();
    bool circleMultiplicationStyleToolButtonChecked = circleMultiplicationStyleToolButton->isChecked();

    if        ( defaultMultiplicationStyleToolButtonChecked &&
               !spaceMultiplicationStyleToolButtonChecked   &&
               !dotMultiplicationStyleToolButtonChecked     &&
               !crossMultiplicationStyleToolButtonChecked   &&
               !circleMultiplicationStyleToolButtonChecked     ) {
        result = MultiplicationSymbol::DEFAULT;
    } else if (!defaultMultiplicationStyleToolButtonChecked &&
                spaceMultiplicationStyleToolButtonChecked   &&
               !dotMultiplicationStyleToolButtonChecked     &&
               !crossMultiplicationStyleToolButtonChecked   &&
               !circleMultiplicationStyleToolButtonChecked     ) {
        result = MultiplicationSymbol::SPACE;
    } else if (!defaultMultiplicationStyleToolButtonChecked &&
               !spaceMultiplicationStyleToolButtonChecked   &&
                dotMultiplicationStyleToolButtonChecked     &&
               !crossMultiplicationStyleToolButtonChecked   &&
               !circleMultiplicationStyleToolButtonChecked     ) {
        result = MultiplicationSymbol::DOT;
    } else if (!defaultMultiplicationStyleToolButtonChecked &&
               !spaceMultiplicationStyleToolButtonChecked   &&
               !dotMultiplicationStyleToolButtonChecked     &&
                crossMultiplicationStyleToolButtonChecked   &&
               !circleMultiplicationStyleToolButtonChecked     ) {
        result = MultiplicationSymbol::CROSS;
    } else if (!defaultMultiplicationStyleToolButtonChecked &&
               !spaceMultiplicationStyleToolButtonChecked   &&
               !dotMultiplicationStyleToolButtonChecked     &&
               !crossMultiplicationStyleToolButtonChecked   &&
                circleMultiplicationStyleToolButtonChecked     ) {
        result = MultiplicationSymbol::CIRCLE;
    } else {
        result = MultiplicationSymbol::INVALID;
    }

    return result;
}


bool OperatorFormatDialog::multiplicationSymbolsVisible() const {
    return inputMultiplicationSymbolsVisible;
}


bool OperatorFormatDialog::multiplicationSymbolsHidden() const {
    return !multiplicationSymbolsVisible();
}


OperatorFormatDialog::DivisionSymbol OperatorFormatDialog::divisionSymbol() const {
    DivisionSymbol result;

    QToolButton* defaultDivisionStyleToolButton  = widget<QToolButton>("default_division_style_tool_button");
    QToolButton* obelusDivisionStyleToolButton   = widget<QToolButton>("obelus_division_style_tool_button");
    QToolButton* fractionDivisionStyleToolButton = widget<QToolButton>("fraction_division_style_tool_button");

    bool defaultDivisionStyleChecked = defaultDivisionStyleToolButton->isChecked();
    bool obelusDivisionStyleChecked   = obelusDivisionStyleToolButton->isChecked();
    bool fractionDivisionStyleChecked = fractionDivisionStyleToolButton->isChecked();

    if        ( defaultDivisionStyleChecked && !obelusDivisionStyleChecked && !fractionDivisionStyleChecked) {
        result = DivisionSymbol::DEFAULT;
    } else if (!defaultDivisionStyleChecked &&  obelusDivisionStyleChecked && !fractionDivisionStyleChecked) {
        result = DivisionSymbol::OBELUS;
    } else if (!defaultDivisionStyleChecked && !obelusDivisionStyleChecked &&  fractionDivisionStyleChecked) {
        result = DivisionSymbol::FRACTION;
    } else {
        result = DivisionSymbol::INVALID;
    }

    return result;
}


bool OperatorFormatDialog::divisionSymbolsVisible() const {
    return inputDivisionSymbolsVisible;
}


bool OperatorFormatDialog::divisionSymbolsHidden() const {
    return !inputDivisionSymbolsVisible;
}


OperatorFormatDialog::LogicalNotSymbol OperatorFormatDialog::logicalNotSymbol() const {
    LogicalNotSymbol result;

    QToolButton* defaultLogicalNotStyleToolButton  = widget<QToolButton>("default_logical_not_style_tool_button");
    QToolButton* negationLogicalNotStyleToolButton = widget<QToolButton>("negation_logical_not_style_tool_button");
    QToolButton* overbarLogicalNotStyleToolButton  = widget<QToolButton>("overbar_logical_not_style_tool_button");
    QToolButton* tildeLogicalNotStyleToolButton    = widget<QToolButton>("tilde_logical_not_style_tool_button");

    bool defaultLogicalNotStyleChecked  = defaultLogicalNotStyleToolButton->isChecked();
    bool negationLogicalNotStyleChecked = negationLogicalNotStyleToolButton->isChecked();
    bool overbarLogicalNotStyleChecked  = overbarLogicalNotStyleToolButton->isChecked();
    bool tildeLogicalNotStyleChecked    = tildeLogicalNotStyleToolButton->isChecked();

    if        ( defaultLogicalNotStyleChecked && !negationLogicalNotStyleChecked &&
               !overbarLogicalNotStyleChecked && !tildeLogicalNotStyleChecked       ) {
        result = LogicalNotSymbol::DEFAULT;
    } else if (!defaultLogicalNotStyleChecked &&  negationLogicalNotStyleChecked &&
               !overbarLogicalNotStyleChecked && !tildeLogicalNotStyleChecked       ) {
        result = LogicalNotSymbol::NEGATION;
    } else if (!defaultLogicalNotStyleChecked && !negationLogicalNotStyleChecked &&
                overbarLogicalNotStyleChecked && !tildeLogicalNotStyleChecked       ) {
        result = LogicalNotSymbol::OVERBAR;
    } else if (!defaultLogicalNotStyleChecked && !negationLogicalNotStyleChecked &&
               !overbarLogicalNotStyleChecked &&  tildeLogicalNotStyleChecked       ) {
        result = LogicalNotSymbol::TILDE;
     } else {
        result = LogicalNotSymbol::INVALID;

     }

    return result;
}


bool OperatorFormatDialog::logicalNotSymbolsVisible() const {
    return inputLogicalNotSymbolsVisible;
}


bool OperatorFormatDialog::logicalNotSymbolsHidden() const {
    return !inputLogicalNotSymbolsVisible;
}


void OperatorFormatDialog::populate() {
    QToolButton* defaultParenthesisToolButton = widget<QToolButton>("default_parenthesis_tool_button");
    QToolButton* parenthesisToolButton        = widget<QToolButton>("parenthesis_tool_button");
    QToolButton* bracketsToolButton           = widget<QToolButton>("brackets_tool_button");
    QToolButton* bracesToolButton             = widget<QToolButton>("braces_tool_button");

    defaultParenthesisToolButton->setChecked(inputParenthesisStyles.contains(ParenthesisStyle::DEFAULT));
    parenthesisToolButton->setChecked(inputParenthesisStyles.contains(ParenthesisStyle::PARENTHESIS));
    bracketsToolButton->setChecked(inputParenthesisStyles.contains(ParenthesisStyle::BRACKETS));
    bracesToolButton->setChecked(inputParenthesisStyles.contains(ParenthesisStyle::BRACES));

    defaultParenthesisToolButton->setVisible(inputDefaultParenthesisStyleVisible);

    QCheckBox* forceParenthesisCheckBox = widget<QCheckBox>("force_parenthesis_checkbox");
    switch (inputForceParenthesisState) {
        case Tristate::CHECKED: {
            forceParenthesisCheckBox->setTristate(false);
            forceParenthesisCheckBox->setChecked(true);
            break;
        }

        case Tristate::UNCHECKED: {
            forceParenthesisCheckBox->setTristate(false);
            forceParenthesisCheckBox->setChecked(false);
            break;
        }

        case Tristate::TRISTATED: {
            forceParenthesisCheckBox->setTristate(true);
            forceParenthesisCheckBox->setCheckState(Qt::PartiallyChecked);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    forceParenthesisCheckBox->setVisible(inputForceParenthesisVisible);


    QToolButton* parameterParenthesisToolButton = widget<QToolButton>("parameter_parenthesis_tool_button");
    QToolButton* parameterBracketsToolButton    = widget<QToolButton>("parameter_brackets_tool_button");
    QToolButton* parameterBracesToolButton      = widget<QToolButton>("parameter_braces_tool_button");

    parameterParenthesisToolButton->setChecked(inputParameterParenthesisStyles.contains(ParenthesisStyle::PARENTHESIS));
    parameterBracketsToolButton->setChecked(inputParameterParenthesisStyles.contains(ParenthesisStyle::BRACKETS));
    parameterBracesToolButton->setChecked(inputParameterParenthesisStyles.contains(ParenthesisStyle::BRACES));

    QGroupBox* parameterParenthesisStyleGroupBox = widget<QGroupBox>("parameter_parenthesis_style_group_box");
    parameterParenthesisStyleGroupBox->setVisible(inputParameterParenthesisStylesVisible);


    QCheckBox* forceParameterParenthesisCheckBox = widget<QCheckBox>("force_parameter_parenthesis_checkbox");
    switch (inputForceParameterParenthesisState) {
        case Tristate::CHECKED: {
            forceParameterParenthesisCheckBox->setTristate(false);
            forceParameterParenthesisCheckBox->setChecked(true);
            break;
        }

        case Tristate::UNCHECKED: {
            forceParameterParenthesisCheckBox->setTristate(false);
            forceParameterParenthesisCheckBox->setChecked(false);
            break;
        }

        case Tristate::TRISTATED: {
            forceParameterParenthesisCheckBox->setTristate(true);
            forceParameterParenthesisCheckBox->setCheckState(Qt::PartiallyChecked);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    forceParameterParenthesisCheckBox->setVisible(inputForceParameterParenthesisVisible);


    QCheckBox* subscriptFirstFunctionParameterCheckBox = widget<QCheckBox>(
        "subscript_first_function_parameter_checkbox"
    );

    switch (inputUseSubscriptedFirstParameterState) {
        case Tristate::CHECKED: {
            subscriptFirstFunctionParameterCheckBox->setTristate(false);
            subscriptFirstFunctionParameterCheckBox->setChecked(true);
            break;
        }

        case Tristate::UNCHECKED: {
            subscriptFirstFunctionParameterCheckBox->setTristate(false);
            subscriptFirstFunctionParameterCheckBox->setChecked(false);
            break;
        }

        case Tristate::TRISTATED: {
            subscriptFirstFunctionParameterCheckBox->setTristate(true);
            subscriptFirstFunctionParameterCheckBox->setCheckState(Qt::PartiallyChecked);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    subscriptFirstFunctionParameterCheckBox->setVisible(inputUseSubscriptedFirstParameterVisible);


    QGroupBox* multiplicationSymbolGroupBox = widget<QGroupBox>("multiplication_symbol_group_box");
    multiplicationSymbolGroupBox->setVisible(inputMultiplicationSymbolsVisible);

    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");

    defaultMultiplicationStyleToolButton->setChecked(
        inputMultiplicationSymbols.contains(MultiplicationSymbol::DEFAULT)
    );

    spaceMultiplicationStyleToolButton->setChecked(inputMultiplicationSymbols.contains(MultiplicationSymbol::SPACE));
    dotMultiplicationStyleToolButton->setChecked(inputMultiplicationSymbols.contains(MultiplicationSymbol::DOT));
    crossMultiplicationStyleToolButton->setChecked(inputMultiplicationSymbols.contains(MultiplicationSymbol::CROSS));
    circleMultiplicationStyleToolButton->setChecked(inputMultiplicationSymbols.contains(MultiplicationSymbol::CIRCLE));


    QGroupBox* divisionSymbolGroupBox = widget<QGroupBox>("division_symbol_group_box");
    divisionSymbolGroupBox->setVisible(inputDivisionSymbolsVisible);

    QToolButton* defaultDivisionStyleToolButton  = widget<QToolButton>("default_division_style_tool_button");
    QToolButton* obelusDivisionStyleToolButton   = widget<QToolButton>("obelus_division_style_tool_button");
    QToolButton* fractionDivisionStyleToolButton = widget<QToolButton>("fraction_division_style_tool_button");

    defaultDivisionStyleToolButton->setChecked(inputDivisionSymbols.contains(DivisionSymbol::DEFAULT));
    obelusDivisionStyleToolButton->setChecked(inputDivisionSymbols.contains(DivisionSymbol::OBELUS));
    fractionDivisionStyleToolButton->setChecked(inputDivisionSymbols.contains(DivisionSymbol::FRACTION));


    QGroupBox* logicalNotSymbolGroupBox = widget<QGroupBox>("logical_not_symbol_group_box");
    logicalNotSymbolGroupBox->setVisible(inputLogicalNotSymbolsVisible);

    QToolButton* defaultLogicalNotStyleToolButton  = widget<QToolButton>("default_logical_not_style_tool_button");
    QToolButton* negationLogicalNotStyleToolButton = widget<QToolButton>("negation_logical_not_style_tool_button");
    QToolButton* overbarLogicalNotStyleToolButton  = widget<QToolButton>("overbar_logical_not_style_tool_button");
    QToolButton* tildeLogicalNotStyleToolButton    = widget<QToolButton>("tilde_logical_not_style_tool_button");

    defaultLogicalNotStyleToolButton->setChecked(inputLogicalNotSymbols.contains(LogicalNotSymbol::DEFAULT));
    negationLogicalNotStyleToolButton->setChecked(inputLogicalNotSymbols.contains(LogicalNotSymbol::NEGATION));
    overbarLogicalNotStyleToolButton->setChecked(inputLogicalNotSymbols.contains(LogicalNotSymbol::OVERBAR));
    tildeLogicalNotStyleToolButton->setChecked(inputLogicalNotSymbols.contains(LogicalNotSymbol::TILDE));


    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(false);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif
}


void OperatorFormatDialog::setParenthesisStyles(
        const QSet<OperatorFormatDialog::ParenthesisStyle> newParenthesisStyles
    ) {
    inputParenthesisStyles = newParenthesisStyles;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setParenthesisStyle(OperatorFormatDialog::ParenthesisStyle newParenthesisStyle) {
    QSet<ParenthesisStyle> styles;
    styles << newParenthesisStyle;

    setParenthesisStyles(styles);
}


void OperatorFormatDialog::setForceParenthesisChecked(bool nowChecked) {
    inputForceParenthesisState = nowChecked ? Tristate::CHECKED : Tristate::UNCHECKED;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setForceParenthesisUnchecked(bool nowUnchecked) {
    setForceParenthesisChecked(!nowUnchecked);
}


void OperatorFormatDialog::setForceParenthesisTristated() {
    inputForceParenthesisState = Tristate::TRISTATED;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setForceParenthesisCheckboxVisible(bool nowVisible) {
    inputForceParenthesisVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setForceParenthesisCheckboxHidden(bool nowHidden) {
    setForceParenthesisCheckboxVisible(!nowHidden);
}


void OperatorFormatDialog::setDefaultParenthesisStyleButtonVisible(bool nowVisible) {
    inputDefaultParenthesisStyleVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setDefaultParenthesisStyleButtonHidden(bool nowHidden) {
    setDefaultParenthesisStyleButtonVisible(!nowHidden);
}


void OperatorFormatDialog::setParameterParenthesisStyle(OperatorFormatDialog::ParenthesisStyle newParenthesisStyle) {
    setParameterParenthesisStyle(QSet<ParenthesisStyle>() << newParenthesisStyle);
}


void OperatorFormatDialog::setParameterParenthesisStyle(
        const QSet<OperatorFormatDialog::ParenthesisStyle> newParenthesisStyles
    ) {
    inputParameterParenthesisStyles = newParenthesisStyles;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setForceParameterParenthesisChecked(bool nowChecked) {
    inputForceParameterParenthesisState = nowChecked ? Tristate::CHECKED : Tristate::UNCHECKED;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setForceParameterParenthesisUnchecked(bool nowUnchecked) {
    setForceParameterParenthesisChecked(!nowUnchecked);
}


void OperatorFormatDialog::setForceParameterParenthesisTristated() {
    inputForceParameterParenthesisState = Tristate::TRISTATED;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setSubscriptedFirstParameterChecked(bool nowChecked) {
    inputUseSubscriptedFirstParameterState = nowChecked ? Tristate::CHECKED : Tristate::UNCHECKED;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setSubscriptedFirstParameterUnchecked(bool nowUnchecked) {
    setSubscriptedFirstParameterChecked(!nowUnchecked);
}


void OperatorFormatDialog::setSubscriptedFirstParameterTristated() {
    inputUseSubscriptedFirstParameterState = Tristate::TRISTATED;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setParameterParenthesisButtonsVisible(bool nowVisible) {
    inputParameterParenthesisStylesVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setParameterParenthesisButtonsHidden(bool nowHidden) {
    setParameterParenthesisButtonsVisible(!nowHidden);
}


void OperatorFormatDialog::setForceParameterParenthesisCheckboxVisible(bool nowVisible) {
    inputForceParameterParenthesisVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setForceParameterParenthesisCheckboxHidden(bool nowHidden) {
    setForceParameterParenthesisCheckboxVisible(!nowHidden);
}


void OperatorFormatDialog::setSubscriptedFirstParameterCheckboxVisible(bool nowVisible) {
    inputUseSubscriptedFirstParameterVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setSubscriptedFirstParameterCheckboxHidden(bool nowHidden) {
    setSubscriptedFirstParameterCheckboxVisible(!nowHidden);
}


void OperatorFormatDialog::setMultiplicationSymbols(
        const QSet<OperatorFormatDialog::MultiplicationSymbol> newMultiplicationSymbols
    ) {
    inputMultiplicationSymbols = newMultiplicationSymbols;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setMultiplicationSymbol(
        OperatorFormatDialog::MultiplicationSymbol newMultiplicationSymbol
    ) {
    QSet<MultiplicationSymbol> symbols;
    symbols << newMultiplicationSymbol;

    setMultiplicationSymbols(symbols);
}


void OperatorFormatDialog::setMultiplicationSymbolsVisible(bool nowVisible) {
    inputMultiplicationSymbolsVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setMultiplicationSymbolsHidden(bool nowHidden) {
    setMultiplicationSymbolsVisible(!nowHidden);
}


void OperatorFormatDialog::setDivisionSymbols(const QSet<OperatorFormatDialog::DivisionSymbol> newDivisionSymbols) {
    inputDivisionSymbols = newDivisionSymbols;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setDivisionSymbol(OperatorFormatDialog::DivisionSymbol newDivisionSymbol) {
    QSet<DivisionSymbol> newDivisionSymbols;
    newDivisionSymbols << newDivisionSymbol;

    setDivisionSymbols(newDivisionSymbols);
}


void OperatorFormatDialog::setDivisionSymbolsVisible(bool nowVisible) {
    inputDivisionSymbolsVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setDivisionSymbolsHidden(bool nowHidden) {
    setDivisionSymbolsVisible(!nowHidden);
}


void OperatorFormatDialog::setLogicalNotSymbols(
        const QSet<OperatorFormatDialog::LogicalNotSymbol> newLogicalNotSymbols
    ) {
    inputLogicalNotSymbols = newLogicalNotSymbols;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setLogicalNotSymbol(OperatorFormatDialog::LogicalNotSymbol newLogicalNotSymbol) {
    QSet<LogicalNotSymbol> newLogicalNotSymbols;
    newLogicalNotSymbols << newLogicalNotSymbol;

    setLogicalNotSymbols(newLogicalNotSymbols);
}


void OperatorFormatDialog::setLogicalNotSymbolsVisible(bool nowVisible) {
    inputLogicalNotSymbolsVisible = nowVisible;

    if (isVisible()) {
        populate();
    }
}


void OperatorFormatDialog::setLogicalNotSymbolsHidden(bool nowHidden) {
    setLogicalNotSymbolsVisible(!nowHidden);
}


void OperatorFormatDialog::bind() {
    QToolButton* defaultParenthesisToolButton = widget<QToolButton>("default_parenthesis_tool_button");
    connect(
        defaultParenthesisToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::defaultParenthesisButtonClicked
    );

    QToolButton* parenthesisToolButton = widget<QToolButton>("parenthesis_tool_button");
    connect(parenthesisToolButton, &QToolButton::clicked, this, &OperatorFormatDialog::parenthesisToolButtonClicked);

    QToolButton* bracketsToolButton = widget<QToolButton>("brackets_tool_button");
    connect(bracketsToolButton, &QToolButton::clicked, this, &OperatorFormatDialog::bracketsToolButtonClicked);

    QToolButton* bracesToolButton = widget<QToolButton>("braces_tool_button");
    connect(bracesToolButton, &QToolButton::clicked, this, &OperatorFormatDialog::bracesToolButtonClicked);

    QCheckBox* forceParenthesisCheckBox = widget<QCheckBox>("force_parenthesis_checkbox");
    connect(
        forceParenthesisCheckBox,
        &QCheckBox::clicked,
        this,
        &OperatorFormatDialog::forceParenthesisCheckBoxClicked
    );

    QToolButton* parameterParenthesisToolButton = widget<QToolButton>("parameter_parenthesis_tool_button");
    connect(
        parameterParenthesisToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::parameterParenthesisToolButtonClicked
    );

    QToolButton* parameterBracketsToolButton = widget<QToolButton>("parameter_brackets_tool_button");
    connect(
        parameterBracketsToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::parameterBracketsToolButtonClicked
    );

    QToolButton* parameterBracesToolButton = widget<QToolButton>("parameter_braces_tool_button");
    connect(
        parameterBracesToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::parameterBracesToolButtonClicked
    );

    QCheckBox* forceParameterParenthesisCheckBox = widget<QCheckBox>("force_parameter_parenthesis_checkbox");
    connect(
        forceParameterParenthesisCheckBox,
        &QCheckBox::clicked,
        this,
        &OperatorFormatDialog::forceParameterParenthesisCheckBoxClicked
    );

    QCheckBox* subscriptFirstFunctionParameterCheckBox = widget<QCheckBox>(
        "subscript_first_function_parameter_checkbox"
    );
    connect(
        subscriptFirstFunctionParameterCheckBox,
        &QCheckBox::clicked,
        this,
        &OperatorFormatDialog::subscriptFirstParameterCheckBoxClicked
    );

    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    connect(
        defaultMultiplicationStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::defaultMultiplicationStyleToolButtonClicked
    );

    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    connect(
        spaceMultiplicationStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::spaceMultiplicationStyleToolButtonClicked
    );

    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    connect(
        dotMultiplicationStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::dotMultiplicationStyleToolButtonClicked
    );

    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    connect(
        crossMultiplicationStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::crossMultiplicationStyleToolButtonClicked
    );

    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");
    connect(
        circleMultiplicationStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::circleMultiplicationStyleToolButtonClicked
    );


    QToolButton* defaultDivisionStyleToolButton = widget<QToolButton>("default_division_style_tool_button");
    connect(
        defaultDivisionStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::defaultDivisionStyleToolButtonClicked
    );

    QToolButton* obelusDivisionStyleToolButton = widget<QToolButton>("obelus_division_style_tool_button");
    connect(
        obelusDivisionStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::obelusDivisionStyleToolButtonClicked
    );

    QToolButton* fractionDivisionStyleToolButton = widget<QToolButton>("fraction_division_style_tool_button");
    connect(
        fractionDivisionStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::fractionDivisionStyleToolButtonClicked
    );

    QToolButton* defaultLogicalNotStyleToolButton = widget<QToolButton>("default_logical_not_style_tool_button");
    connect(
        defaultLogicalNotStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::defaultLogicalNotStyleToolButtonClicked
    );

    QToolButton* negationLogicalNotStyleToolButton = widget<QToolButton>("negation_logical_not_style_tool_button");
    connect(
        negationLogicalNotStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::negationLogicalNotStyleToolButtonClicked
    );

    QToolButton* overbarLogicalNotStyleToolButton = widget<QToolButton>("overbar_logical_not_style_tool_button");
    connect(
        overbarLogicalNotStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::overbarLogicalNotStyleToolButtonClicked
    );

    QToolButton* tildeLogicalNotStyleToolButton = widget<QToolButton>("tilde_logical_not_style_tool_button");
    connect(
        tildeLogicalNotStyleToolButton,
        &QToolButton::clicked,
        this,
        &OperatorFormatDialog::tildeLogicalNotStyleToolButtonClicked
    );


    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &OperatorFormatDialog::accept);

    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
    connect(cancelButton, &QPushButton::clicked, this, &OperatorFormatDialog::reject);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        connect(applyButton, &QPushButton::clicked, this, &OperatorFormatDialog::applyClicked);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    ProgrammaticDialog::bind();
}


void OperatorFormatDialog::defaultParenthesisButtonClicked() {
    QToolButton* defaultParenthesisToolButton = widget<QToolButton>("default_parenthesis_tool_button");
    QToolButton* parenthesisToolButton        = widget<QToolButton>("parenthesis_tool_button");
    QToolButton* bracketsToolButton           = widget<QToolButton>("brackets_tool_button");
    QToolButton* bracesToolButton             = widget<QToolButton>("braces_tool_button");

    defaultParenthesisToolButton->setChecked(true);
    parenthesisToolButton->setChecked(false);
    bracketsToolButton->setChecked(false);
    bracesToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::parenthesisToolButtonClicked() {
    QToolButton* defaultParenthesisToolButton = widget<QToolButton>("default_parenthesis_tool_button");
    QToolButton* parenthesisToolButton        = widget<QToolButton>("parenthesis_tool_button");
    QToolButton* bracketsToolButton           = widget<QToolButton>("brackets_tool_button");
    QToolButton* bracesToolButton             = widget<QToolButton>("braces_tool_button");

    defaultParenthesisToolButton->setChecked(false);
    parenthesisToolButton->setChecked(true);
    bracketsToolButton->setChecked(false);
    bracesToolButton->setChecked(false);

    updateDialog();
}

void OperatorFormatDialog::bracketsToolButtonClicked() {
    QToolButton* defaultParenthesisToolButton = widget<QToolButton>("default_parenthesis_tool_button");
    QToolButton* parenthesisToolButton        = widget<QToolButton>("parenthesis_tool_button");
    QToolButton* bracketsToolButton           = widget<QToolButton>("brackets_tool_button");
    QToolButton* bracesToolButton             = widget<QToolButton>("braces_tool_button");

    defaultParenthesisToolButton->setChecked(false);
    parenthesisToolButton->setChecked(false);
    bracketsToolButton->setChecked(true);
    bracesToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::bracesToolButtonClicked() {
    QToolButton* defaultParenthesisToolButton = widget<QToolButton>("default_parenthesis_tool_button");
    QToolButton* parenthesisToolButton        = widget<QToolButton>("parenthesis_tool_button");
    QToolButton* bracketsToolButton           = widget<QToolButton>("brackets_tool_button");
    QToolButton* bracesToolButton             = widget<QToolButton>("braces_tool_button");

    defaultParenthesisToolButton->setChecked(false);
    parenthesisToolButton->setChecked(false);
    bracketsToolButton->setChecked(false);
    bracesToolButton->setChecked(true);

    updateDialog();
}


void OperatorFormatDialog::forceParenthesisCheckBoxClicked() {
    QCheckBox* forceParenthesisCheckBox = widget<QCheckBox>("force_parenthesis_checkbox");
    forceParenthesisCheckBox->setTristate(false);

    updateDialog();
}


void OperatorFormatDialog::parameterParenthesisToolButtonClicked() {
    QToolButton* parenthesisToolButton = widget<QToolButton>("parameter_parenthesis_tool_button");
    QToolButton* bracketsToolButton    = widget<QToolButton>("parameter_brackets_tool_button");
    QToolButton* bracesToolButton      = widget<QToolButton>("parameter_braces_tool_button");

    parenthesisToolButton->setChecked(true);
    bracketsToolButton->setChecked(false);
    bracesToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::parameterBracketsToolButtonClicked() {
    QToolButton* parenthesisToolButton = widget<QToolButton>("parameter_parenthesis_tool_button");
    QToolButton* bracketsToolButton    = widget<QToolButton>("parameter_brackets_tool_button");
    QToolButton* bracesToolButton      = widget<QToolButton>("parameter_braces_tool_button");

    parenthesisToolButton->setChecked(false);
    bracketsToolButton->setChecked(true);
    bracesToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::parameterBracesToolButtonClicked() {
    QToolButton* parenthesisToolButton = widget<QToolButton>("parameter_parenthesis_tool_button");
    QToolButton* bracketsToolButton    = widget<QToolButton>("parameter_brackets_tool_button");
    QToolButton* bracesToolButton      = widget<QToolButton>("parameter_braces_tool_button");

    parenthesisToolButton->setChecked(false);
    bracketsToolButton->setChecked(false);
    bracesToolButton->setChecked(true);

    updateDialog();
}


void OperatorFormatDialog::forceParameterParenthesisCheckBoxClicked() {
    QCheckBox* forceParameterParenthesisCheckBox = widget<QCheckBox>("force_parameter_parenthesis_checkbox");
    forceParameterParenthesisCheckBox->setTristate(false);

    updateDialog();
}


void OperatorFormatDialog::subscriptFirstParameterCheckBoxClicked() {
    QCheckBox* subscriptFirstFunctionParameterCheckBox = widget<QCheckBox>(
        "subscript_first_function_parameter_checkbox"
    );
    subscriptFirstFunctionParameterCheckBox->setTristate(false);

    updateDialog();
}


void OperatorFormatDialog::defaultMultiplicationStyleToolButtonClicked() {
    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");

    defaultMultiplicationStyleToolButton->setChecked(true);
    spaceMultiplicationStyleToolButton->setChecked(false);
    dotMultiplicationStyleToolButton->setChecked(false);
    crossMultiplicationStyleToolButton->setChecked(false);
    circleMultiplicationStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::spaceMultiplicationStyleToolButtonClicked() {
    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");

    defaultMultiplicationStyleToolButton->setChecked(false);
    spaceMultiplicationStyleToolButton->setChecked(true);
    dotMultiplicationStyleToolButton->setChecked(false);
    crossMultiplicationStyleToolButton->setChecked(false);
    circleMultiplicationStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::dotMultiplicationStyleToolButtonClicked() {
    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");

    defaultMultiplicationStyleToolButton->setChecked(false);
    spaceMultiplicationStyleToolButton->setChecked(false);
    dotMultiplicationStyleToolButton->setChecked(true);
    crossMultiplicationStyleToolButton->setChecked(false);
    circleMultiplicationStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::crossMultiplicationStyleToolButtonClicked() {
    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");

    defaultMultiplicationStyleToolButton->setChecked(false);
    spaceMultiplicationStyleToolButton->setChecked(false);
    dotMultiplicationStyleToolButton->setChecked(false);
    crossMultiplicationStyleToolButton->setChecked(true);
    circleMultiplicationStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::circleMultiplicationStyleToolButtonClicked() {
    QToolButton* defaultMultiplicationStyleToolButton = widget<QToolButton>("default_multiplication_style_tool_button");
    QToolButton* spaceMultiplicationStyleToolButton = widget<QToolButton>("space_multiplication_style_tool_button");
    QToolButton* dotMultiplicationStyleToolButton = widget<QToolButton>("dot_multiplication_style_tool_button");
    QToolButton* crossMultiplicationStyleToolButton = widget<QToolButton>("cross_multiplication_style_tool_button");
    QToolButton* circleMultiplicationStyleToolButton = widget<QToolButton>("circle_multiplication_style_tool_button");

    defaultMultiplicationStyleToolButton->setChecked(false);
    spaceMultiplicationStyleToolButton->setChecked(false);
    dotMultiplicationStyleToolButton->setChecked(false);
    crossMultiplicationStyleToolButton->setChecked(false);
    circleMultiplicationStyleToolButton->setChecked(true);

    updateDialog();
}


void OperatorFormatDialog::defaultDivisionStyleToolButtonClicked() {
    QToolButton* defaultDivisionStyleToolButton  = widget<QToolButton>("default_division_style_tool_button");
    QToolButton* obelusDivisionStyleToolButton   = widget<QToolButton>("obelus_division_style_tool_button");
    QToolButton* fractionDivisionStyleToolButton = widget<QToolButton>("fraction_division_style_tool_button");

    defaultDivisionStyleToolButton->setChecked(true);
    obelusDivisionStyleToolButton->setChecked(false);
    fractionDivisionStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::obelusDivisionStyleToolButtonClicked() {
    QToolButton* defaultDivisionStyleToolButton  = widget<QToolButton>("default_division_style_tool_button");
    QToolButton* obelusDivisionStyleToolButton   = widget<QToolButton>("obelus_division_style_tool_button");
    QToolButton* fractionDivisionStyleToolButton = widget<QToolButton>("fraction_division_style_tool_button");

    defaultDivisionStyleToolButton->setChecked(false);
    obelusDivisionStyleToolButton->setChecked(true);
    fractionDivisionStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::fractionDivisionStyleToolButtonClicked() {
    QToolButton* defaultDivisionStyleToolButton  = widget<QToolButton>("default_division_style_tool_button");
    QToolButton* obelusDivisionStyleToolButton   = widget<QToolButton>("obelus_division_style_tool_button");
    QToolButton* fractionDivisionStyleToolButton = widget<QToolButton>("fraction_division_style_tool_button");

    defaultDivisionStyleToolButton->setChecked(false);
    obelusDivisionStyleToolButton->setChecked(false);
    fractionDivisionStyleToolButton->setChecked(true);

    updateDialog();
}


void OperatorFormatDialog::defaultLogicalNotStyleToolButtonClicked() {
    QToolButton* defaultLogicalNotStyleToolButton  = widget<QToolButton>("default_logical_not_style_tool_button");
    QToolButton* negationLogicalNotStyleToolButton = widget<QToolButton>("negation_logical_not_style_tool_button");
    QToolButton* overbarLogicalNotStyleToolButton  = widget<QToolButton>("overbar_logical_not_style_tool_button");
    QToolButton* tildeLogicalNotStyleToolButton    = widget<QToolButton>("tilde_logical_not_style_tool_button");

    defaultLogicalNotStyleToolButton->setChecked(true);
    negationLogicalNotStyleToolButton->setChecked(false);
    overbarLogicalNotStyleToolButton->setChecked(false);
    tildeLogicalNotStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::negationLogicalNotStyleToolButtonClicked() {
    QToolButton* defaultLogicalNotStyleToolButton  = widget<QToolButton>("default_logical_not_style_tool_button");
    QToolButton* negationLogicalNotStyleToolButton = widget<QToolButton>("negation_logical_not_style_tool_button");
    QToolButton* overbarLogicalNotStyleToolButton  = widget<QToolButton>("overbar_logical_not_style_tool_button");
    QToolButton* tildeLogicalNotStyleToolButton    = widget<QToolButton>("tilde_logical_not_style_tool_button");

    defaultLogicalNotStyleToolButton->setChecked(false);
    negationLogicalNotStyleToolButton->setChecked(true);
    overbarLogicalNotStyleToolButton->setChecked(false);
    tildeLogicalNotStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::overbarLogicalNotStyleToolButtonClicked() {
    QToolButton* defaultLogicalNotStyleToolButton  = widget<QToolButton>("default_logical_not_style_tool_button");
    QToolButton* negationLogicalNotStyleToolButton = widget<QToolButton>("negation_logical_not_style_tool_button");
    QToolButton* overbarLogicalNotStyleToolButton  = widget<QToolButton>("overbar_logical_not_style_tool_button");
    QToolButton* tildeLogicalNotStyleToolButton    = widget<QToolButton>("tilde_logical_not_style_tool_button");

    defaultLogicalNotStyleToolButton->setChecked(false);
    negationLogicalNotStyleToolButton->setChecked(false);
    overbarLogicalNotStyleToolButton->setChecked(true);
    tildeLogicalNotStyleToolButton->setChecked(false);

    updateDialog();
}


void OperatorFormatDialog::tildeLogicalNotStyleToolButtonClicked() {
    QToolButton* defaultLogicalNotStyleToolButton  = widget<QToolButton>("default_logical_not_style_tool_button");
    QToolButton* negationLogicalNotStyleToolButton = widget<QToolButton>("negation_logical_not_style_tool_button");
    QToolButton* overbarLogicalNotStyleToolButton  = widget<QToolButton>("overbar_logical_not_style_tool_button");
    QToolButton* tildeLogicalNotStyleToolButton    = widget<QToolButton>("tilde_logical_not_style_tool_button");

    defaultLogicalNotStyleToolButton->setChecked(false);
    negationLogicalNotStyleToolButton->setChecked(false);
    overbarLogicalNotStyleToolButton->setChecked(false);
    tildeLogicalNotStyleToolButton->setChecked(true);

    updateDialog();
}


void OperatorFormatDialog::applyClicked() {
    ParenthesisStyle parenthesisStyle = OperatorFormatDialog::parenthesisStyle();
    if (parenthesisStyle != ParenthesisStyle::INVALID_OR_NONE) {
        inputParenthesisStyles.clear();
        inputParenthesisStyles << parenthesisStyle;
    }

    if (forceParenthesisSet()) {
        inputForceParenthesisState = forceParenthesis() ? Tristate::CHECKED : Tristate::UNCHECKED;
    }

    MultiplicationSymbol multiplicationSymbol = OperatorFormatDialog::multiplicationSymbol();
    if (multiplicationSymbol != MultiplicationSymbol::INVALID) {
        inputMultiplicationSymbols.clear();
        inputMultiplicationSymbols << multiplicationSymbol;
    }

    DivisionSymbol divisionSymbol = OperatorFormatDialog::DivisionSymbol();
    if (divisionSymbol != DivisionSymbol::INVALID) {
        inputDivisionSymbols.clear();
        inputDivisionSymbols << divisionSymbol;
    }

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(false);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    emit apply(this);
}


void OperatorFormatDialog::updateDialog() {
    bool valuesHaveChanged = valuesChanged();

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(valuesHaveChanged);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(valuesHaveChanged);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    if (currentAutoApplyEnabled) {
        applyClicked();
    }
}


void OperatorFormatDialog::configureWidget() {
    setWindowTitle(tr("Operator Format"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentAutoApplyEnabled                  = false;
    inputDefaultParenthesisStyleVisible      = true;
    inputForceParenthesisState               = Tristate::UNCHECKED;
    inputForceParenthesisVisible             = false;
    inputParameterParenthesisStylesVisible   = false;
    inputForceParameterParenthesisState      = Tristate::UNCHECKED;
    inputForceParameterParenthesisVisible    = false;
    inputUseSubscriptedFirstParameterState   = Tristate::UNCHECKED;
    inputUseSubscriptedFirstParameterVisible = false;
    inputMultiplicationSymbolsVisible        = false;
    inputDivisionSymbolsVisible              = false;
    inputLogicalNotSymbolsVisible            = false;

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QGroupBox* parenthesisStyleGroupBox = new QGroupBox(tr("Parenthesis"));
    registerWidget(parenthesisStyleGroupBox, "parenthesis_style_group_box");

    QVBoxLayout* parenthesisVerticalLayout = newVBoxLayout("parenthesis_vertical_layout");
    parenthesisStyleGroupBox->setLayout(parenthesisVerticalLayout);

    mainLayout->addWidget(parenthesisStyleGroupBox);

    QHBoxLayout* parenthesisButtonsLayout = newHBoxLayout("parenthesis_buttons_layout");
    parenthesisVerticalLayout->addLayout(parenthesisButtonsLayout);

    QList<QToolButton*> toolButtons;

    QFont buttonFont = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    buttonFont.setPointSize(18);

    QToolButton* defaultParenthesisToolButton = new QToolButton();
    registerWidget(defaultParenthesisToolButton, "default_parenthesis_tool_button");
    defaultParenthesisToolButton->setText(tr("Default"));
    defaultParenthesisToolButton->setFont(buttonFont);
    defaultParenthesisToolButton->setToolTip(tr("Click to select the default parenthesis style for each operator"));
    defaultParenthesisToolButton->setWhatsThis(
        tr("You can use this button to select the default parenthesis style for each operator.")
    );
    parenthesisButtonsLayout->addWidget(defaultParenthesisToolButton);
    toolButtons.append(defaultParenthesisToolButton);

    QToolButton* parenthesisToolButton = new QToolButton();
    registerWidget(parenthesisToolButton, "parenthesis_tool_button");
    parenthesisToolButton->setText(tr("( )"));
    parenthesisToolButton->setFont(buttonFont);
    parenthesisToolButton->setToolTip(tr("Click to select traditional parenthesis"));
    parenthesisToolButton->setWhatsThis(tr("You can use this button to select traditional parenthesis."));
    parenthesisButtonsLayout->addWidget(parenthesisToolButton);
    toolButtons.append(parenthesisToolButton);

    QToolButton* bracketsToolButton = new QToolButton();
    registerWidget(bracketsToolButton, "brackets_tool_button");
    bracketsToolButton->setText(tr("[ ]"));
    bracketsToolButton->setFont(buttonFont);
    bracketsToolButton->setToolTip(tr("Click to select square brackets"));
    bracketsToolButton->setWhatsThis(tr("You can use this button to select square brackets."));
    parenthesisButtonsLayout->addWidget(bracketsToolButton);
    toolButtons.append(bracketsToolButton);

    QToolButton* bracesToolButton = new QToolButton();
    registerWidget(bracesToolButton, "braces_tool_button");
    bracesToolButton->setText(tr("{ }"));
    bracesToolButton->setFont(buttonFont);
    bracesToolButton->setToolTip(tr("Click to select braces"));
    bracesToolButton->setWhatsThis(tr("You can use this button to select braces."));
    parenthesisButtonsLayout->addWidget(bracesToolButton);
    toolButtons.append(bracesToolButton);

    QCheckBox* forceParenthesisCheckBox = new QCheckBox(tr("Always show parenthesis"));
    registerWidget(forceParenthesisCheckBox, "force_parenthesis_checkbox");
    forceParenthesisCheckBox->setTristate(true);
    parenthesisVerticalLayout->addWidget(forceParenthesisCheckBox);


    QGroupBox* parameterParenthesisStyleGroupBox = new QGroupBox(tr("Function Parameter Parenthesis"));
    registerWidget(parameterParenthesisStyleGroupBox, "parameter_parenthesis_style_group_box");

    QVBoxLayout* parameterParenthesisVerticalLayout = newVBoxLayout("parameter_parenthesis_vertical_layout");
    parameterParenthesisStyleGroupBox->setLayout(parameterParenthesisVerticalLayout);

    mainLayout->addWidget(parameterParenthesisStyleGroupBox);

    QHBoxLayout* parameterParenthesisButtonsLayout = newHBoxLayout("parameter_parenthesis_buttons_layout");
    parameterParenthesisVerticalLayout->addLayout(parameterParenthesisButtonsLayout);

    QToolButton* parameterParenthesisToolButton = new QToolButton();
    registerWidget(parameterParenthesisToolButton, "parameter_parenthesis_tool_button");
    parameterParenthesisToolButton->setText(tr("( )"));
    parameterParenthesisToolButton->setFont(buttonFont);
    parameterParenthesisToolButton->setToolTip(tr("Click to select traditional parenthesis"));
    parameterParenthesisToolButton->setWhatsThis(tr("You can use this button to select traditional parenthesis."));
    parameterParenthesisButtonsLayout->addWidget(parameterParenthesisToolButton);
    toolButtons.append(parameterParenthesisToolButton);

    QToolButton* parameterBracketsToolButton = new QToolButton();
    registerWidget(parameterBracketsToolButton, "parameter_brackets_tool_button");
    parameterBracketsToolButton->setText(tr("[ ]"));
    parameterBracketsToolButton->setFont(buttonFont);
    parameterBracketsToolButton->setToolTip(tr("Click to select square brackets"));
    parameterBracketsToolButton->setWhatsThis(tr("You can use this button to select square brackets."));
    parameterParenthesisButtonsLayout->addWidget(parameterBracketsToolButton);
    toolButtons.append(parameterBracketsToolButton);

    QToolButton* parameterBracesToolButton = new QToolButton();
    registerWidget(parameterBracesToolButton, "parameter_braces_tool_button");
    parameterBracesToolButton->setText(tr("{ }"));
    parameterBracesToolButton->setFont(buttonFont);
    parameterBracesToolButton->setToolTip(tr("Click to select braces"));
    parameterBracesToolButton->setWhatsThis(tr("You can use this button to select braces."));
    parameterParenthesisButtonsLayout->addWidget(parameterBracesToolButton);
    toolButtons.append(parameterBracesToolButton);

    QCheckBox* forceParameterParenthesisCheckBox = new QCheckBox(
        tr("Always show parenthesis around function parameters")
    );
    registerWidget(forceParameterParenthesisCheckBox, "force_parameter_parenthesis_checkbox");
    forceParameterParenthesisCheckBox->setTristate(true);
    parameterParenthesisVerticalLayout->addWidget(forceParameterParenthesisCheckBox);

    QCheckBox* subscriptFirstFunctionParameterCheckBox = new QCheckBox(
        tr("Subscript the first function parameter (e.g. log%1x)").arg(QChar(0x2082))
    );
    registerWidget(subscriptFirstFunctionParameterCheckBox, "subscript_first_function_parameter_checkbox");
    subscriptFirstFunctionParameterCheckBox->setTristate(true);
    mainLayout->addWidget(subscriptFirstFunctionParameterCheckBox);


    QGroupBox* multiplicationSymbolGroupBox = new QGroupBox(tr("Multiplication Symbol"));
    registerWidget(multiplicationSymbolGroupBox, "multiplication_symbol_group_box");
    mainLayout->addWidget(multiplicationSymbolGroupBox);

    QHBoxLayout* multiplicationSymbolLayout = newHBoxLayout("multiplication_symbol_layout");
    multiplicationSymbolGroupBox->setLayout(multiplicationSymbolLayout);

    QToolButton* defaultMultiplicationStyleToolButton = new QToolButton();
    registerWidget(defaultMultiplicationStyleToolButton, "default_multiplication_style_tool_button");
    defaultMultiplicationStyleToolButton->setText(tr("Default"));
    defaultMultiplicationStyleToolButton->setFont(buttonFont);
    defaultMultiplicationStyleToolButton->setToolTip(
        tr("Click to select the default multiplication symbol for the type of multiplication")
    );
    defaultMultiplicationStyleToolButton->setWhatsThis(
        tr("You can use this button to select the default multiplication symbol for each multiplication operation.")
    );
    multiplicationSymbolLayout->addWidget(defaultMultiplicationStyleToolButton);
    toolButtons.append(defaultMultiplicationStyleToolButton);

    QToolButton* spaceMultiplicationStyleToolButton = new QToolButton();
    registerWidget(spaceMultiplicationStyleToolButton, "space_multiplication_style_tool_button");
    spaceMultiplicationStyleToolButton->setText(tr("a%1b").arg(QChar(0x2009)));
    spaceMultiplicationStyleToolButton->setFont(buttonFont);
    spaceMultiplicationStyleToolButton->setToolTip(tr("Click to select a small space between operands"));
    spaceMultiplicationStyleToolButton->setWhatsThis(
        tr("You can use this button to select a small space between operands.")
    );
    multiplicationSymbolLayout->addWidget(spaceMultiplicationStyleToolButton);
    toolButtons.append(spaceMultiplicationStyleToolButton);

    QToolButton* dotMultiplicationStyleToolButton = new QToolButton();
    registerWidget(dotMultiplicationStyleToolButton, "dot_multiplication_style_tool_button");
    dotMultiplicationStyleToolButton->setText(tr("a%1b").arg(QChar(0x2219)));
    dotMultiplicationStyleToolButton->setFont(buttonFont);
    dotMultiplicationStyleToolButton->setToolTip(tr("Click to select a small middle dot betwen operands"));
    dotMultiplicationStyleToolButton->setWhatsThis(
        tr("You can use this button to select a small middle dot between operands.")
    );
    multiplicationSymbolLayout->addWidget(dotMultiplicationStyleToolButton);
    toolButtons.append(dotMultiplicationStyleToolButton);

    QToolButton* crossMultiplicationStyleToolButton = new QToolButton();
    registerWidget(crossMultiplicationStyleToolButton, "cross_multiplication_style_tool_button");
    crossMultiplicationStyleToolButton->setText(tr("a%1b").arg(QChar(0x00D7)));
    crossMultiplicationStyleToolButton->setFont(buttonFont);
    crossMultiplicationStyleToolButton->setToolTip(tr("Click to select a cross multiplication symbol betwen operands"));
    crossMultiplicationStyleToolButton->setWhatsThis(
        tr("You can use this button to select a cross multiplication symbol between operands.")
    );
    multiplicationSymbolLayout->addWidget(crossMultiplicationStyleToolButton);
    toolButtons.append(crossMultiplicationStyleToolButton);

    QToolButton* circleMultiplicationStyleToolButton = new QToolButton();
    registerWidget(circleMultiplicationStyleToolButton, "circle_multiplication_style_tool_button");
    circleMultiplicationStyleToolButton->setText(tr("a%1b").arg(QChar(0x2218)));
    circleMultiplicationStyleToolButton->setFont(buttonFont);
    circleMultiplicationStyleToolButton->setToolTip(tr("Click to select a small circle symbol betwen operands"));
    circleMultiplicationStyleToolButton->setWhatsThis(
        tr("You can use this button to select a small circule symbol between operands.")
    );
    multiplicationSymbolLayout->addWidget(circleMultiplicationStyleToolButton);
    toolButtons.append(circleMultiplicationStyleToolButton);


    QGroupBox* divisionSymbolGroupBox = new QGroupBox(tr("Division Symbol"));
    registerWidget(divisionSymbolGroupBox, "division_symbol_group_box");
    mainLayout->addWidget(divisionSymbolGroupBox);

    QHBoxLayout* divisionSymbolLayout = newHBoxLayout("division_symbol_layout");
    divisionSymbolGroupBox->setLayout(divisionSymbolLayout);

    QToolButton* defaultDivisionStyleToolButton = new QToolButton();
    registerWidget(defaultDivisionStyleToolButton, "default_division_style_tool_button");
    defaultDivisionStyleToolButton->setText(tr("Default"));
    defaultDivisionStyleToolButton->setFont(buttonFont);
    defaultDivisionStyleToolButton->setToolTip(
        tr("Click to select the default division symbol for the type of division")
    );
    defaultDivisionStyleToolButton->setWhatsThis(
        tr("You can use this button to select the default division symbol for each division operation.")
    );
    divisionSymbolLayout->addWidget(defaultDivisionStyleToolButton);
    toolButtons.append(defaultDivisionStyleToolButton);

    QToolButton* obelusDivisionStyleToolButton = new QToolButton();
    registerWidget(obelusDivisionStyleToolButton, "obelus_division_style_tool_button");
    obelusDivisionStyleToolButton->setText(tr("a%1b").arg(QChar(0x00F7)));
    obelusDivisionStyleToolButton->setFont(buttonFont);
    obelusDivisionStyleToolButton->setToolTip(
        tr("Click to select an obelus (%1) as the division symbol").arg(QChar(0x00F7))
    );
    obelusDivisionStyleToolButton->setWhatsThis(
        tr("You can use this button to select an obelus, \"%1\" as the division symbol.").arg(QChar(0x00F7))
    );
    divisionSymbolLayout->addWidget(obelusDivisionStyleToolButton);
    toolButtons.append(obelusDivisionStyleToolButton);

    QToolButton* fractionDivisionStyleToolButton = new QToolButton();
    registerWidget(fractionDivisionStyleToolButton, "fraction_division_style_tool_button");
    fractionDivisionStyleToolButton->setText(tr("a\n%1%1%1\nb").arg(QChar(0x2500)));
    fractionDivisionStyleToolButton->setFont(buttonFont);
    fractionDivisionStyleToolButton->setToolTip(tr("Click to select a fraction for division"));
    fractionDivisionStyleToolButton->setWhatsThis(
        tr("You can use this button to select a fraction as a division symbol.")
    );
    divisionSymbolLayout->addWidget(fractionDivisionStyleToolButton);
    toolButtons.append(fractionDivisionStyleToolButton);


    QGroupBox* logicalNotSymbolGroupBox = new QGroupBox(tr("Not/Negation Symbol"));
    registerWidget(logicalNotSymbolGroupBox, "logical_not_symbol_group_box");
    mainLayout->addWidget(logicalNotSymbolGroupBox);

    QHBoxLayout* logicalNotSymbolLayout = newHBoxLayout("logical_not_symbol_layout");
    logicalNotSymbolGroupBox->setLayout(logicalNotSymbolLayout);

    QToolButton* defaultLogicalNotStyleToolButton = new QToolButton();
    registerWidget(defaultLogicalNotStyleToolButton, "default_logical_not_style_tool_button");
    defaultLogicalNotStyleToolButton->setText(tr("Default"));
    defaultLogicalNotStyleToolButton->setFont(buttonFont);
    defaultLogicalNotStyleToolButton->setToolTip(
        tr("Click to select the default not/negation symbol for the type of operation")
    );
    defaultLogicalNotStyleToolButton->setWhatsThis(
        tr("You can use this button to select the default not/negation symbol for each operation.")
    );
    logicalNotSymbolLayout->addWidget(defaultLogicalNotStyleToolButton);
    toolButtons.append(defaultLogicalNotStyleToolButton);

    QToolButton* negationLogicalNotStyleToolButton = new QToolButton();
    registerWidget(negationLogicalNotStyleToolButton, "negation_logical_not_style_tool_button");
    negationLogicalNotStyleToolButton->setText(tr("%1%2a").arg(QChar(0x00AC)).arg(QChar(0x200A)));
    negationLogicalNotStyleToolButton->setFont(buttonFont);
    negationLogicalNotStyleToolButton->setToolTip(
        tr("Click to select the negation symbol, %1, for the type of operation").arg(QChar(0x00AC))
    );
    negationLogicalNotStyleToolButton->setWhatsThis(
        tr("You can use this button to select the negation symbol, %1, for each operation.").arg(QChar(0x00AC))
    );
    logicalNotSymbolLayout->addWidget(negationLogicalNotStyleToolButton);
    toolButtons.append(negationLogicalNotStyleToolButton);

    QToolButton* overbarLogicalNotStyleToolButton = new QToolButton();
    registerWidget(overbarLogicalNotStyleToolButton, "overbar_logical_not_style_tool_button");
    overbarLogicalNotStyleToolButton->setText(tr("a"));
    QFont overbarButtonFont = buttonFont;
    overbarButtonFont.setOverline(true);
    overbarLogicalNotStyleToolButton->setFont(overbarButtonFont);
    overbarLogicalNotStyleToolButton->setToolTip(tr("Click to select an overbar for not/negation operations."));
    overbarLogicalNotStyleToolButton->setWhatsThis(
        tr("You can use this button to select an overbar for not/negation operations.")
    );
    logicalNotSymbolLayout->addWidget(overbarLogicalNotStyleToolButton);
    toolButtons.append(overbarLogicalNotStyleToolButton);

    QToolButton* tildeLogicalNotStyleToolButton = new QToolButton();
    registerWidget(tildeLogicalNotStyleToolButton, "tilde_logical_not_style_tool_button");
    tildeLogicalNotStyleToolButton->setText(tr("~a"));
    tildeLogicalNotStyleToolButton->setFont(buttonFont);
    tildeLogicalNotStyleToolButton->setToolTip(tr("Click to select an tilde for not/negation operations."));
    tildeLogicalNotStyleToolButton->setWhatsThis(
        tr("You can use this button to select an tilde for not/negation operations.")
    );
    logicalNotSymbolLayout->addWidget(tildeLogicalNotStyleToolButton);
    toolButtons.append(tildeLogicalNotStyleToolButton);


    setButtonSizes(toolButtons);

    multiplicationSymbolGroupBox->setVisible(false);
    divisionSymbolGroupBox->setVisible(false);


    #if (defined(Q_OS_DARWIN))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    #elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
        );

    #else

        #error Unknown platform

    #endif

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
        registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platforma

    #endif

    mainLayout->addWidget(buttonBox);
}


bool OperatorFormatDialog::valuesChanged() const {
    ParenthesisStyle     parenthesisStyle             = OperatorFormatDialog::parenthesisStyle();
    bool                 forceParenthesisSet          = OperatorFormatDialog::forceParenthesisSet();
    bool                 forceParenthesis             = OperatorFormatDialog::forceParenthesis();
    ParenthesisStyle     parameterParenthesisStyle    = OperatorFormatDialog::parameterParenthesisStyle();
    bool                 forceParameterParenthesisSet = OperatorFormatDialog::forceParameterParenthesisSet();
    bool                 forceParameterParenthesis    = OperatorFormatDialog::forceParameterParenthesis();
    bool                 subscriptFirstParameter      = OperatorFormatDialog::subscriptedFirstParameter();
    bool                 subscriptFirstParameterSet   = OperatorFormatDialog::subscriptedFirstParameterSet();
    MultiplicationSymbol multiplicationSymbol         = OperatorFormatDialog::multiplicationSymbol();
    DivisionSymbol       divisionSymbol               = OperatorFormatDialog::divisionSymbol();
    LogicalNotSymbol     logicalNotSymbol             = OperatorFormatDialog::logicalNotSymbol();

    bool valueHasChanged = (
           (inputParenthesisStyles.size() != 1  && parenthesisStyle != ParenthesisStyle::INVALID_OR_NONE)
        || (inputParenthesisStyles.size() == 1 && parenthesisStyle != *inputParenthesisStyles.begin())
        || (inputForceParenthesisState == Tristate::TRISTATED && forceParenthesisSet)
        || (inputForceParenthesisState == Tristate::CHECKED && !forceParenthesis)
        || (inputForceParenthesisState == Tristate::UNCHECKED && forceParenthesis)
        || (   inputParameterParenthesisStyles.size() != 1
            && parameterParenthesisStyle != ParenthesisStyle::INVALID_OR_NONE
           )
        || (   inputParameterParenthesisStyles.size() == 1
            && parameterParenthesisStyle != *inputParameterParenthesisStyles.begin()
           )
        || (inputForceParameterParenthesisState == Tristate::TRISTATED && forceParameterParenthesisSet)
        || (inputForceParameterParenthesisState == Tristate::CHECKED && !forceParameterParenthesis)
        || (inputForceParameterParenthesisState == Tristate::UNCHECKED && forceParameterParenthesis)
        || (inputUseSubscriptedFirstParameterState == Tristate::TRISTATED && subscriptFirstParameterSet)
        || (inputUseSubscriptedFirstParameterState == Tristate::CHECKED && !subscriptFirstParameter)
        || (inputUseSubscriptedFirstParameterState == Tristate::UNCHECKED && subscriptFirstParameter)
        || (inputMultiplicationSymbols.size() != 1 && multiplicationSymbol != MultiplicationSymbol::INVALID)
        || (inputMultiplicationSymbols.size() == 1 && multiplicationSymbol != *inputMultiplicationSymbols.begin())
        || (inputDivisionSymbols.size() != 1 && divisionSymbol != DivisionSymbol::INVALID)
        || (inputDivisionSymbols.size() == 1 && divisionSymbol != *inputDivisionSymbols.begin())
        || (inputLogicalNotSymbols.size() != 1 && logicalNotSymbol != LogicalNotSymbol::INVALID)
        || (inputLogicalNotSymbols.size() == 1 && logicalNotSymbol != *inputLogicalNotSymbols.begin())
    );

    return valueHasChanged;
}


void OperatorFormatDialog::setButtonSizes(const QList<QToolButton*>& buttons) {
    unsigned maximumSide   = 0;
    unsigned numberButtons = static_cast<unsigned>(buttons.size());

    for (unsigned buttonIndex=0 ; buttonIndex<numberButtons ; ++buttonIndex) {
        QToolButton* toolButton = buttons.at(buttonIndex);
        QFont        font       = toolButton->font();
        QString      text       = toolButton->text();

        QFontMetrics fontMetrics(font);

        unsigned textHeight = fontMetrics.height() * (1 + text.count(tr("\n")));
        unsigned textWidth  = fontMetrics.horizontalAdvance(text);

        if (maximumSide < textHeight) {
            maximumSide = textHeight;
        }

        if (maximumSide < textWidth) {
            maximumSide = textWidth;
        }
    }

    QSize buttonSize(1.25 * maximumSide, 1.25 * maximumSide);
    for (unsigned buttonIndex=0 ; buttonIndex<numberButtons ; ++buttonIndex) {
        QToolButton* toolButton = buttons.at(buttonIndex);
        toolButton->setFixedSize(buttonSize);
        toolButton->setCheckable(true);
    }
}
