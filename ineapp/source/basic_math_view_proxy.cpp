/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BasicMathViewProxy class.
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
#include <ld_multiplication_operator_format.h>
#include <ld_division_operator_format.h>
#include <ld_parenthesis_format.h>
#include <ld_logical_unary_not_operator_format.h>

#include <ld_element_cursor.h>

#include <ld_root_element.h>
#include <ld_assignment_operator_element.h>
#include <ld_variable_element.h>
#include <ld_literal_element.h>
#include <ld_subscript_index_operator_element.h>
#include <ld_subscript_row_column_operator_element.h>
#include <ld_pi_special_value_element.h>
#include <ld_eulers_number_special_value_element.h>
#include <ld_epsilon_special_value_element.h>
//#include <ld_infinity_special_value_element.h>
#include <ld_element_of_set_operator_element.h>
#include <ld_boolean_type_element.h>
#include <ld_integer_type_element.h>
#include <ld_real_type_element.h>
#include <ld_complex_type_element.h>
#include <ld_value_field_element.h>
#include <ld_value_field_format.h>
#include <ld_addition_operator_element.h>
#include <ld_subtraction_operator_element.h>
#include <ld_multiplication_operator_element.h>
#include <ld_division_operator_element.h>
#include <ld_fraction_operator_element.h>
#include <ld_unary_plus_operator_element.h>
#include <ld_unary_minus_operator_element.h>
#include <ld_grouping_parenthesis_operator_element.h>
#include <ld_grouping_brackets_operator_element.h>
#include <ld_grouping_braces_operator_element.h>
#include <ld_power_operator_element.h>
#include <ld_square_root_operator_element.h>
#include <ld_root_operator_element.h>
#include <ld_complex_conjugate_operator_element.h>
#include <ld_factorial_operator_element.h>
#include <ld_absolute_value_operator_element.h>
#include <ld_floor_operator_element.h>
#include <ld_ceiling_operator_element.h>
#include <ld_nearest_integer_operator_element.h>
#include <ld_not_equal_to_operator_element.h>
#include <ld_less_than_operator_element.h>
#include <ld_greater_than_operator_element.h>
#include <ld_less_than_or_equal_to_operator_element.h>
#include <ld_greater_than_or_equal_to_operator_element.h>

#include "application.h"
#include "insert_element_command.h"
#include "main_window.h"
#include "document.h"
#include "view_widget.h"
#include "cursor.h"
#include "math_view_proxy_base.h"
#include "basic_math_view_proxy.h"

BasicMathViewProxy::BasicMathViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):MathViewProxyBase(
        view,
        window
    ) {}


BasicMathViewProxy::~BasicMathViewProxy() {}


void BasicMathViewProxy::bindDynamic() {
    MainWindow* window     = BasicMathViewProxy::window();
    ViewWidget* viewWidget = BasicMathViewProxy::view();

    sessionConnect(
        viewWidget,
        &ViewWidget::textKeyPressedOnPlaceholder,
        &BasicMathViewProxy::textKeyPressedOnPlaceholder
    );

    QAction* assignmentOperatorAction = window->action("assignment_operator_action");
    sessionConnect(
        assignmentOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::assignmentOperatorButtonClicked
    );

    QAction* variableAction = window->action("variable_action");
    sessionConnect(variableAction, &QAction::triggered, &BasicMathViewProxy::variableButtonClicked);

    QAction* literalAction = window->action("literal_action");
    sessionConnect(literalAction, &QAction::triggered, &BasicMathViewProxy::literalButtonClicked);

    QAction* subscriptIndexOperatorAction = window->action("subscript_index_action");
    sessionConnect(
        subscriptIndexOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::subscriptIndexOperatorButtonClicked
    );

    QAction* subscriptRowColumnOperatorAction = window->action("subscript_row_column_action");
    sessionConnect(
        subscriptRowColumnOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::subscriptRowColumnOperatorButtonClicked
    );

    QAction* piSpecialValueAction = window->action("pi_special_value_action");
    sessionConnect(piSpecialValueAction, &QAction::triggered, &BasicMathViewProxy::piSpecialValueButtonClicked);

    QAction* eulersNumberSpecialValueAction = window->action("eulers_number_special_value_action");
    sessionConnect(
        eulersNumberSpecialValueAction,
        &QAction::triggered,
        &BasicMathViewProxy::eulersNumberSpecialValueButtonClicked
    );

    QAction* epsilonSpecialValueAction = window->action("epsilon_special_value_action");
    sessionConnect(
        epsilonSpecialValueAction,
        &QAction::triggered,
        &BasicMathViewProxy::epsilonSpecialValueButtonClicked
    );

//    QAction* infinitySpecialValueAction = window->action("infinity_special_value_action");
//    sessionConnect(
//        infinitySpecialValueAction,
//        &QAction::triggered,
//        &BasicMathViewProxy::infinitySpecialValueButtonClicked
//    );

    QAction* elementOfSetAction = window->action("element_of_set_action");
    sessionConnect(elementOfSetAction, &QAction::triggered, &BasicMathViewProxy::elementOfSetButtonClicked);

    QAction* booleanTypeAction = window->action("boolean_type_action");
    sessionConnect(booleanTypeAction, &QAction::triggered, &BasicMathViewProxy::booleanTypeButtonClicked);

    QAction* integerTypeAction = window->action("integer_type_action");
    sessionConnect(integerTypeAction, &QAction::triggered, &BasicMathViewProxy::integerTypeButtonClicked);

    QAction* realTypeAction = window->action("real_type_action");
    sessionConnect(realTypeAction, &QAction::triggered, &BasicMathViewProxy::realTypeButtonClicked);

    QAction* complexTypeAction = window->action("complex_type_action");
    sessionConnect(complexTypeAction, &QAction::triggered, &BasicMathViewProxy::complexTypeButtonClicked);

    QAction* additionOperatorAction = window->action("addition_operator_action");
    sessionConnect(additionOperatorAction, &QAction::triggered, &BasicMathViewProxy::additionOperatorButtonClicked);

    QAction* subtractionOperatorAction = window->action("subtraction_operator_action");
    sessionConnect(
        subtractionOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::subtractionOperatorButtonClicked
    );

    QAction* multiplicationOperatorAction = window->action("multiplication_operator_action");
    sessionConnect(
        multiplicationOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::multiplicationOperatorButtonClicked
    );

    QAction* divisionOperatorAction = window->action("division_operator_action");
    sessionConnect(
        divisionOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::divisionOperatorButtonClicked
    );

    QAction* fractionOperatorAction = window->action("fraction_operator_action");
    sessionConnect(
        fractionOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::fractionOperatorButtonClicked
    );

    QAction* unaryPlusOperatorAction = window->action("unary_plus_operator_action");
    sessionConnect(
        unaryPlusOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::unaryPlusOperatorButtonClicked
    );

    QAction* unaryMinusOperatorAction = window->action("unary_minus_operator_action");
    sessionConnect(
        unaryMinusOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::unaryMinusOperatorButtonClicked
    );

    QAction* groupingParenthesisOperatorAction = window->action("grouping_parenthesis_operator_action");
    sessionConnect(
        groupingParenthesisOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::groupingParenthesisButtonClicked
    );

    QAction* groupingBracketsOperatorAction = window->action("grouping_brackets_operator_action");
    sessionConnect(
        groupingBracketsOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::groupingBracketsButtonClicked
    );

    QAction* groupingBracesOperatorAction = window->action("grouping_braces_operator_action");
    sessionConnect(
        groupingBracesOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::groupingBracesButtonClicked
    );

    QAction* powerOperatorAction = window->action("power_operator_action");
    sessionConnect(powerOperatorAction, &QAction::triggered, &BasicMathViewProxy::powerOperatorButtonClicked);

    QAction* squareRootOperatorAction = window->action("square_root_operator_action");
    sessionConnect(
        squareRootOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::squareRootOperatorButtonClicked
    );

    QAction* rootOperatorAction = window->action("root_operator_action");
    sessionConnect(rootOperatorAction, &QAction::triggered, &BasicMathViewProxy::rootOperatorButtonClicked);

    QAction* complexConjugateOperatorAction = window->action("complex_conjugate_operator_action");
    sessionConnect(
        complexConjugateOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::complexConjugateOperatorButtonClicked
    );

    QAction* factorialOperatorAction = window->action("factorial_operator_action");
    sessionConnect(factorialOperatorAction, &QAction::triggered, &BasicMathViewProxy::factorialOperatorButtonClicked);

    QAction* absoluteValueOperatorAction = window->action("absolute_value_operator_action");
    sessionConnect(
        absoluteValueOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::absoluteValueOperatorButtonClicked
    );

    QAction* floorOperatorAction = window->action("floor_operator_action");
    sessionConnect(floorOperatorAction, &QAction::triggered, &BasicMathViewProxy::floorOperatorButtonClicked);

    QAction* ceilingOperatorAction = window->action("ceiling_operator_action");
    sessionConnect(ceilingOperatorAction, &QAction::triggered, &BasicMathViewProxy::ceilingOperatorButtonClicked);

    QAction* nearestIntegerOperatorAction = window->action("nearest_integer_operator_action");
    sessionConnect(
        nearestIntegerOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::nearestIntegerOperatorButtonClicked
    );

    QAction* notEqualToOperatorAction = window->action("not_equal_to_operator_action");
    sessionConnect(
        notEqualToOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::notEqualToOperatorButtonClicked
    );

    QAction* lessThanOperatorAction = window->action("less_than_operator_action");
    sessionConnect(
        lessThanOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::lessThanOperatorButtonClicked
    );

    QAction* greaterThanOperatorAction = window->action("greater_than_operator_action");
    sessionConnect(
        greaterThanOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::greaterThanOperatorButtonClicked
    );

    QAction* lessThanOrEqualToOperatorAction = window->action("less_than_or_equal_to_operator_action");
    sessionConnect(
        lessThanOrEqualToOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::lessThanOrEqualToOperatorButtonClicked
    );

    QAction* greaterThanOrEqualToOperatorAction = window->action("greater_than_or_equal_to_operator_action");
    sessionConnect(
        greaterThanOrEqualToOperatorAction,
        &QAction::triggered,
        &BasicMathViewProxy::greaterThanOrEqualToOperatorButtonClicked
    );
}


void BasicMathViewProxy::unbindDynamic() {
    sessionDisconnectAll();
}


void BasicMathViewProxy::textKeyPressedOnPlaceholder(
        int                   /* keyCode */,
        Qt::KeyboardModifiers /* modifiers */,
        unsigned long         /* nativeModifiers */,
        unsigned long         /* nativeScanCodes */,
        unsigned long         /* nativeVirtualKey */,
        const QString&        text
    ) {
    if (text.length() == 1) {
        QChar character = text.at(0);
        if (character.isLetter()) {
            if (window()->action("variable_action")->isEnabled()) {
                Ld::ElementPointer element = Ld::Element::create(Ld::VariableElement::elementName);
                element->setFormat(mathIdentifierCharacterFormat());
                element->setText(text);

                InsertElementCommand* newCommand = new InsertElementCommand(
                    element,
                    InsertElementCommand::InsertionLocation::AT_CURSOR,
                    InsertElementCommand::FinalCursorPosition::ELEMENT_END_OF_TEXT
                );

                view()->insertCommand(newCommand);
            } else {
                Application::beep();
            }
        } else if (character.isDigit() || character == QChar('"') || character == QChar('\'')) {
            if (window()->action("literal_action")->isEnabled()) {
                Ld::ElementPointer element = Ld::Element::create(Ld::LiteralElement::elementName);
                element->setFormat(operatorFormat());
                element->setText(text);

                InsertElementCommand* newCommand = new InsertElementCommand(
                    element,
                    InsertElementCommand::InsertionLocation::AT_CURSOR,
                    InsertElementCommand::FinalCursorPosition::ELEMENT_END_OF_TEXT
                );

                view()->insertCommand(newCommand);
            } else {
                Application::beep();
            }
        } else if (text == tr("+")) {
            if (window()->action("addition_operator_action")->isEnabled()) {
                additionOperatorButtonClicked();
            } else {
                Application::beep();
            }
        } else if (text == tr("-")) {
            if (window()->action("subtraction_operator_action")->isEnabled()) {
                subtractionOperatorButtonClicked();
            } else {
                Application::beep();
            }
        } else if (text == tr("*")) {
            if (window()->action("multiplication_operator_action")->isEnabled()) {
                multiplicationOperatorButtonClicked();
            } else {
                Application::beep();
            }
        } else if (text == tr("/")) {
            if (window()->action("fraction_operator_action")->isEnabled()) {
                fractionOperatorButtonClicked();
            } else {
                Application::beep();
            }
        } else if (text == tr("=")) {
            if (window()->action("assignment_operator_action")->isEnabled()) {
                assignmentOperatorButtonClicked();
            } else {
                Application::beep();
            }
        } else if (text == tr("<")) {
            if (window()->action("less_than_operator_action")->isEnabled()) {
                lessThanOperatorButtonClicked();
            } else {
                Application::beep();
            }
        } else if (text == tr(">")) {
            if (window()->action("greater_than_operator_action")->isEnabled()) {
                greaterThanOperatorButtonClicked();
            } else {
                Application::beep();
            }
        }
    }
}


void BasicMathViewProxy::assignmentOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::AssignmentOperatorElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::variableButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::VariableElement::elementName);
    element->setFormat(mathIdentifierCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::literalButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LiteralElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::piSpecialValueButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::PiSpecialValueElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::eulersNumberSpecialValueButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::EulersNumberSpecialValueElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::epsilonSpecialValueButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::EpsilonSpecialValueElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


//void BasicMathViewProxy::infinitySpecialValueButtonClicked() {
//    Ld::ElementPointer element = Ld::Element::create(Ld::InfinitySpecialValueElement::elementName);
//    element->setFormat(mathCharacterFormat());

//    insertElement(element);
//}


void BasicMathViewProxy::elementOfSetButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ElementOfSetOperatorElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::booleanTypeButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::BooleanTypeElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::integerTypeButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::IntegerTypeElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::realTypeButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::RealTypeElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::complexTypeButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ComplexTypeElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::additionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::AdditionOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::subtractionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SubtractionOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::multiplicationOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::MultiplicationOperatorElement::elementName);
    element->setFormat(multiplicationOperatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::divisionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::DivisionOperatorElement::elementName);
    element->setFormat(divisionOperatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::fractionOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::FractionOperatorElement::elementName);
    element->setFormat(divisionOperatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::unaryPlusOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::UnaryPlusOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::unaryMinusOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::UnaryMinusOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::groupingParenthesisButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::GroupingParenthesisOperatorElement::elementName);
    element->setFormat(parenthesisOperatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::groupingBracketsButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::GroupingBracketsOperatorElement::elementName);
    element->setFormat(parenthesisOperatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::groupingBracesButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::GroupingBracesOperatorElement::elementName);
    element->setFormat(parenthesisOperatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::powerOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::PowerOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::squareRootOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SquareRootOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::rootOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::RootOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::complexConjugateOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::ComplexConjugateOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::factorialOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::FactorialOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::absoluteValueOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::AbsoluteValueOperatorElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::floorOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::FloorOperatorElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::ceilingOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::CeilingOperatorElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::nearestIntegerOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NearestIntegerOperatorElement::elementName);
    element->setFormat(mathCharacterFormat());

    insertElement(element);
}


void BasicMathViewProxy::notEqualToOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::NotEqualToOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::lessThanOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LessThanOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::greaterThanOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::GreaterThanOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::lessThanOrEqualToOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::LessThanOrEqualToOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::greaterThanOrEqualToOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::GreaterThanOrEqualToOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::subscriptIndexOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SubscriptIndexOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}


void BasicMathViewProxy::subscriptRowColumnOperatorButtonClicked() {
    Ld::ElementPointer element = Ld::Element::create(Ld::SubscriptRowColumnOperatorElement::elementName);
    element->setFormat(operatorFormat());

    insertElement(element);
}
