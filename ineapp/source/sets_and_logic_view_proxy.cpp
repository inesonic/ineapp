/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SetsAndLogicViewProxy class.
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
#include <ld_parenthesis_format.h>
#include <ld_logical_unary_not_operator_format.h>

#include <ld_element_cursor.h>

#include <ld_root_element.h>
#include <ld_null_set_special_value_element.h>
#include <ld_set_element.h>
#include <ld_tuple_element.h>
#include <ld_range_2_element.h>
#include <ld_range_3_element.h>
#include <ld_union_operator_element.h>
#include <ld_intersection_operator_element.h>
#include <ld_disjoint_union_operator_element.h>
#include <ld_cartesian_product_operator_element.h>
#include <ld_set_difference_operator_element.h>
#include <ld_symmetric_difference_operator_element.h>
#include <ld_nary_union_operator_element.h>
#include <ld_nary_disjoint_union_operator_element.h>
#include <ld_not_element_of_set_operator_element.h>
#include <ld_set_contains_as_member_operator_element.h>
#include <ld_not_set_contains_as_member_operator_element.h>
#include <ld_subset_operator_element.h>
#include <ld_proper_subset_operator_element.h>
#include <ld_superset_operator_element.h>
#include <ld_proper_superset_operator_element.h>
#include <ld_not_subset_operator_element.h>
#include <ld_not_proper_subset_operator_element.h>
#include <ld_not_superset_operator_element.h>
#include <ld_not_proper_superset_operator_element.h>
#include <ld_logical_unary_not_operator_element.h>
#include <ld_logical_overbar_not_operator_element.h>
#include <ld_logical_and_operator_element.h>
#include <ld_logical_or_operator_element.h>
#include <ld_logical_exclusive_or_operator_element.h>
#include <ld_logical_conditional_operator_element.h>
#include <ld_logical_biconditional_operator_element.h>
#include <ld_logical_true_special_value_element.h>
#include <ld_logical_false_special_value_element.h>

#include "application.h"
#include "insert_element_command.h"
#include "main_window.h"
#include "document.h"
#include "view_widget.h"
#include "cursor.h"
#include "math_view_proxy_base.h"
#include "sets_and_logic_view_proxy.h"

SetsAndLogicViewProxy::SetsAndLogicViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):MathViewProxyBase(
        view,
        window
    ) {}


SetsAndLogicViewProxy::~SetsAndLogicViewProxy() {}


void SetsAndLogicViewProxy::bindDynamic() {
    MainWindow* window     = SetsAndLogicViewProxy::window();
    ViewWidget* viewWidget = SetsAndLogicViewProxy::view();

    sessionConnect(
        viewWidget,
        &ViewWidget::textKeyPressedOnPlaceholder,
        &SetsAndLogicViewProxy::textKeyPressedOnPlaceholder
    );

    QAction* nullSetSpecialValueAction = window->action("null_set_special_value_action");
    sessionConnect(
        nullSetSpecialValueAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::nullSetSpecialValueButtonClicked
    );

    QAction* setAction = window->action("set_action");
    sessionConnect(setAction, &QAction::triggered, &SetsAndLogicViewProxy::setButtonClicked);

    QAction* tupleAction = window->action("tuple_action");
    sessionConnect(tupleAction, &QAction::triggered, &SetsAndLogicViewProxy::tupleButtonClicked);

    QAction* range2Action = window->action("range_2_action");
    sessionConnect(range2Action, &QAction::triggered, &SetsAndLogicViewProxy::range2ButtonClicked);

    QAction* range3Action = window->action("range_3_action");
    sessionConnect(range3Action, &QAction::triggered, &SetsAndLogicViewProxy::range3ButtonClicked);

    QAction* unionOperatorAction = window->action("union_operator_action");
    sessionConnect(unionOperatorAction, &QAction::triggered, &SetsAndLogicViewProxy::unionOperatorButtonClicked);

    QAction* intersectionOperatorAction = window->action("intersection_operator_action");
    sessionConnect(
        intersectionOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::intersectionOperatorButtonClicked
    );

    QAction* disjointUnionOperatorAction = window->action("disjoint_union_operator_action");
    sessionConnect(
        disjointUnionOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::disjointUnionOperatorButtonClicked
    );

    QAction* cartesianProductOperatorAction = window->action("cartesian_product_operator_action");
    sessionConnect(
        cartesianProductOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::cartesianProductOperatorButtonClicked
    );

    QAction* symmetricDifferenceOperatorAction = window->action("symmetric_difference_operator_action");
    sessionConnect(
        symmetricDifferenceOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::symmetricDifferenceOperatorButtonClicked
    );

    QAction* naryUnionOperatorAction = window->action("nary_union_operator_action");
    sessionConnect(
        naryUnionOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::naryUnionOperatorButtonClicked
    );

    QAction* naryDisjointUnionOperatorAction = window->action("nary_disjoint_union_operator_action");
    sessionConnect(
        naryDisjointUnionOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::naryDisjointUnionOperatorButtonClicked
    );

    QAction* setDifferenceOperatorAction = window->action("set_difference_operator_action");
    sessionConnect(
        setDifferenceOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::setDifferenceOperatorButtonClicked
    );

    QAction* notElementOfSetOperatorAction = window->action("not_element_of_set_operator_action");
    sessionConnect(
        notElementOfSetOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::notElementOfSetOperatorButtonClicked
    );

    QAction* setContainsAsMemberOperatorAction = window->action("set_contains_as_member_operator_action");
    sessionConnect(
        setContainsAsMemberOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::setContainsAsMemberOperatorButtonClicked
    );

    QAction* notSetContainsAsMemberOperatorAction = window->action("not_set_contains_as_member_operator_action");
    sessionConnect(
        notSetContainsAsMemberOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::notSetContainsAsMemberOperatorButtonClicked
    );

    QAction* subsetOperatorAction = window->action("subset_operator_action");
    sessionConnect(subsetOperatorAction, &QAction::triggered, &SetsAndLogicViewProxy::subsetOperatorButtonClicked);

    QAction* properSubsetOperatorAction = window->action("proper_subset_operator_action");
    sessionConnect(
        properSubsetOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::properSubsetOperatorButtonClicked
    );

    QAction* supersetOperatorAction = window->action("superset_operator_action");
    sessionConnect(supersetOperatorAction, &QAction::triggered, &SetsAndLogicViewProxy::supersetOperatorButtonClicked);

    QAction* properSupersetOperatorAction = window->action("proper_superset_operator_action");
    sessionConnect(
        properSupersetOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::properSupersetOperatorButtonClicked
    );

    QAction* notSubsetOperatorAction = window->action("not_subset_operator_action");
    sessionConnect(
        notSubsetOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::notSubsetOperatorButtonClicked
    );

    QAction* notProperSubsetOperatorAction = window->action("not_proper_subset_operator_action");
    sessionConnect(
        notProperSubsetOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::notProperSubsetOperatorButtonClicked
    );

    QAction* notSupersetOperatorAction = window->action("not_superset_operator_action");
    sessionConnect(
        notSupersetOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::notSupersetOperatorButtonClicked
    );

    QAction* notProperSupersetOperatorAction = window->action("not_proper_superset_operator_action");
    sessionConnect(
        notProperSupersetOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::notProperSupersetOperatorButtonClicked
    );

    QAction* logicalUnaryNotOperatorAction = window->action("logical_unary_not_operator_action");
    sessionConnect(
        logicalUnaryNotOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalUnaryNotOperatorButtonClicked
    );

    QAction* logicalOverbarNotOperatorAction = window->action("logical_overbar_not_operator_action");
    sessionConnect(
        logicalOverbarNotOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalOverbarNotOperatorButtonClicked
    );

    QAction* logicalAndOperatorAction = window->action("logical_and_operator_action");
    sessionConnect(
        logicalAndOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalAndOperatorButtonClicked
    );

    QAction* logicalOrOperatorAction = window->action("logical_or_operator_action");
    sessionConnect(
        logicalOrOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalOrOperatorButtonClicked
    );

    QAction* logicalExclusiveOrOperatorAction = window->action("logical_exclusive_or_operator_action");
    sessionConnect(
        logicalExclusiveOrOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalExclusiveOrOperatorButtonClicked
    );

    QAction* logicalConditionalOperatorAction = window->action("logical_conditional_operator_action");
    sessionConnect(
        logicalConditionalOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalConditionalOperatorButtonClicked
    );

    QAction* logicalBiconditionalOperatorAction = window->action("logical_biconditional_operator_action");
    sessionConnect(
        logicalBiconditionalOperatorAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalBiconditionalOperatorButtonClicked
    );

    QAction* logicalTrueSpecialValueAction = window->action("logical_true_special_value_action");
    sessionConnect(
        logicalTrueSpecialValueAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalTrueSpecialValueButtonClicked
    );

    QAction* logicalTrueSpecialValueButtonAction = window->action("logical_true_special_value_button_action");
    sessionConnect(
        logicalTrueSpecialValueButtonAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalTrueSpecialValueButtonClicked
    );

    QAction* logicalFalseSpecialValueAction = window->action("logical_false_special_value_action");
    sessionConnect(
        logicalFalseSpecialValueAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalFalseSpecialValueButtonClicked
    );

    QAction* logicalFalseSpecialValueButtonAction = window->action("logical_false_special_value_button_action");
    sessionConnect(
        logicalFalseSpecialValueButtonAction,
        &QAction::triggered,
        &SetsAndLogicViewProxy::logicalFalseSpecialValueButtonClicked
    );
}


void SetsAndLogicViewProxy::unbindDynamic() {
    sessionDisconnectAll();
}


void SetsAndLogicViewProxy::textKeyPressedOnPlaceholder(
        int                   /* keyCode */,
        Qt::KeyboardModifiers /* modifiers */,
        unsigned long         /* nativeModifiers */,
        unsigned long         /* nativeScanCodes */,
        unsigned long         /* nativeVirtualKey */,
        const QString&        text
    ) {
    if (text.length() == 1) {
        if (text == tr("&")) {
            if (window()->action("logical_and_operator_action")->isEnabled()) {
                logicalAndOperatorButtonClicked();
            } else {
                Application::beep();
            }
        } else if (text == tr("|")) {
            if (window()->action("logical_or_operator_action")->isEnabled()) {
                logicalOrOperatorButtonClicked();
            } else {
                Application::beep();
            }
        }
    }
}


void SetsAndLogicViewProxy::nullSetSpecialValueButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NullSetSpecialValueElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::setButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SetElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::tupleButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::TupleElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::range2ButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::Range2Element::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::range3ButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::Range3Element::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::unionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::UnionOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::intersectionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::IntersectionOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::disjointUnionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::DisjointUnionOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::cartesianProductOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::CartesianProductOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::setDifferenceOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SetDifferenceOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::symmetricDifferenceOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SymmetricDifferenceOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::naryUnionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NaryUnionOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::naryDisjointUnionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NaryDisjointUnionOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::notElementOfSetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NotElementOfSetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::setContainsAsMemberOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SetContainsAsMemberOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::notSetContainsAsMemberOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NotSetContainsAsMemberOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::subsetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SubsetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::properSubsetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ProperSubsetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::supersetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SupersetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::properSupersetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ProperSupersetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::notSubsetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NotSubsetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::notProperSubsetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NotProperSubsetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::notSupersetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NotSupersetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::notProperSupersetOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NotProperSupersetOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalUnaryNotOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalUnaryNotOperatorElement::elementName);
    element->setFormat(logicalUnaryNotOperatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalOverbarNotOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalOverbarNotOperatorElement::elementName);
    element->setFormat(logicalUnaryNotOperatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalAndOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalAndOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalOrOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalOrOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalExclusiveOrOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalExclusiveOrOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalConditionalOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalConditionalOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalBiconditionalOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalBiconditionalOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalTrueSpecialValueButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalTrueSpecialValueElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void SetsAndLogicViewProxy::logicalFalseSpecialValueButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LogicalFalseSpecialValueElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


Ld::FormatPointer SetsAndLogicViewProxy::logicalUnaryNotOperatorFormat() const {
    ViewWidget*                     view        = SetsAndLogicViewProxy::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    Ld::FormatPointer format = Ld::Format::create(Ld::LogicalUnaryNotOperatorFormat::formatName);
    copyFontData(format, rootElement->defaultMathTextFormat());

    return format;
}
