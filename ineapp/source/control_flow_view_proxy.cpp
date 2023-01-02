/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ControlFlowViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSet>

#include <algorithm>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format_structures.h>
#include <ld_format.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_operator_format.h>
#include <ld_brace_conditional_format.h>
#include <ld_multiplication_operator_format.h>
#include <ld_parenthesis_format.h>

#include <ld_element_cursor.h>

#include <ld_root_element.h>
#include <ld_placeholder_element.h>
#include <ld_for_all_in_operator_element.h>
#include <ld_while_operator_element.h>
#include <ld_compound_statement_operator_element.h>
#include <ld_if_operator_element.h>
#include <ld_else_if_operator_element.h>
#include <ld_else_operator_element.h>
#include <ld_therefore_operator_element.h>
#include <ld_brace_conditional_operator_element.h>

#include "application.h"
#include "insert_element_command.h"
#include "main_window.h"
#include "document.h"
#include "view_widget.h"
#include "cursor.h"
#include "brace_conditional_configuration_dialog.h"
#include "math_view_proxy_base.h"
#include "control_flow_view_proxy.h"

ControlFlowViewProxy::ControlFlowViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):MathViewProxyBase(
        view,
        window
    ) {}


ControlFlowViewProxy::~ControlFlowViewProxy() {}


void ControlFlowViewProxy::bindDynamic() {
    MainWindow* window     = ControlFlowViewProxy::window();
//    ViewWidget* viewWidget = ControlFlowViewProxy::view();

    QAction* forAllInOperatorAction = window->action("for_all_in_operator_action");
    sessionConnect(
        forAllInOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::forAllInOperatorButtonClicked
    );

    QAction* whileOperatorAction = window->action("while_operator_action");
    sessionConnect(
        whileOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::whileOperatorButtonClicked
    );

    QAction* compoundStatementOperatorAction = window->action("compound_statement_operator_action");
    sessionConnect(
        compoundStatementOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::compoundStatementOperatorButtonClicked
    );

    QAction* ifOperatorAction = window->action("if_operator_action");
    sessionConnect(
        ifOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::ifOperatorButtonClicked
    );

    QAction* elseIfOperatorAction = window->action("else_if_operator_action");
    sessionConnect(
        elseIfOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::elseIfOperatorButtonClicked
    );

    QAction* elseOperatorAction = window->action("else_operator_action");
    sessionConnect(
        elseOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::elseOperatorButtonClicked
    );

    QAction* thereforeOperatorAction = window->action("therefore_operator_action");
    sessionConnect(
        thereforeOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::thereforeOperatorButtonClicked
    );

    QAction* braceConditionalOperatorAction = window->action("brace_conditional_operator_action");
    sessionConnect(
        braceConditionalOperatorAction,
        &QAction::triggered,
        &ControlFlowViewProxy::braceConditionalOperatorButtonClicked
    );
}


void ControlFlowViewProxy::unbindDynamic() {
    sessionDisconnectAll();
}


void ControlFlowViewProxy::forAllInOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ForAllInOperatorElement::elementName);
    element->setFormat(mathFunctionFontFormat());

    insertElement(element);
}


void ControlFlowViewProxy::whileOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::WhileOperatorElement::elementName);
    element->setFormat(mathFunctionFontFormat());

    insertElement(element);
}


void ControlFlowViewProxy::compoundStatementOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::CompoundStatementOperatorElement::elementName);
    element->setFormat(mathFunctionFontFormat());

    insertElement(element);
}


void ControlFlowViewProxy::ifOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::IfOperatorElement::elementName);
    element->setFormat(mathFunctionFontFormat());

    insertElement(element);
}


void ControlFlowViewProxy::elseIfOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ElseIfOperatorElement::elementName);
    element->setFormat(mathFunctionFontFormat());

    insertElement(element);
}


void ControlFlowViewProxy::elseOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ElseOperatorElement::elementName);
    element->setFormat(mathFunctionFontFormat());

    insertElement(element);
}


void ControlFlowViewProxy::thereforeOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ThereforeOperatorElement::elementName);
    element->setFormat(mathFunctionFontFormat());

    insertElement(element);
}


void ControlFlowViewProxy::braceConditionalOperatorButtonClicked() {
    ViewWidget*                     view        = ControlFlowViewProxy::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    QSharedPointer<Ld::BraceConditionalFormat>
        braceConditionalFormat = rootElement->defaultBraceConditionalFormat()->clone()
                                 .dynamicCast<Ld::BraceConditionalFormat>();

    QSharedPointer<Ld::CharacterFormat> defaultMathTextFormat = rootElement->defaultMathTextFormat();

    BraceConditionalConfigurationDialog braceConditionalConfigurationDialog(window());

    braceConditionalConfigurationDialog.setApplyButtonHidden();
    braceConditionalConfigurationDialog.setAutoApplyDisabled();
    braceConditionalConfigurationDialog.setOkButtonAlwaysEnabled();
    braceConditionalConfigurationDialog.setWindowTitle(tr("Create Brace Conditional"));

    braceConditionalConfigurationDialog.setMathTextFont(defaultMathTextFormat->toQFont());
    braceConditionalConfigurationDialog.setMathIdentifierFont(braceConditionalFormat->toQFont());
    braceConditionalConfigurationDialog.setConditionPreambleText(braceConditionalFormat->conditionPreambleString());
    braceConditionalConfigurationDialog.setImplicitElseConditionText(braceConditionalFormat->elseConditionString());

    bool elseClauseShown = rootElement->defaultBraceConditionalElseClauseShown();
    braceConditionalConfigurationDialog.setHaveExplicitElseCondition(elseClauseShown);

    int result = braceConditionalConfigurationDialog.exec();
    if (result == BraceConditionalConfigurationDialog::Accepted) {
        QSharedPointer<Ld::BraceConditionalOperatorElement>
            element = Ld::Element::create(Ld::BraceConditionalOperatorElement::elementName)
                      .dynamicCast<Ld::BraceConditionalOperatorElement>();

        unsigned long numberExplicitConditions  = braceConditionalConfigurationDialog.numberExplicitConditions();
        bool          showExplicitElseCondition = braceConditionalConfigurationDialog.hasExplicitElseCondition();
        QString       newConditionPreambleText  = braceConditionalConfigurationDialog.conditionPreambleText();
        QString       newElseConditionText      = braceConditionalConfigurationDialog.implicitElseConditionText();

        Ld::ElementPointer placeholder = Ld::Element::create(Ld::PlaceholderElement::elementName);

        element->setNumberConditions(numberExplicitConditions, showExplicitElseCondition, placeholder, Q_NULLPTR);

        braceConditionalFormat->setConditionPreambleString(newConditionPreambleText);
        braceConditionalFormat->setElseConditionString(newElseConditionText);

        element->setFormat(braceConditionalFormat);

        insertElement(element);
    }
}
