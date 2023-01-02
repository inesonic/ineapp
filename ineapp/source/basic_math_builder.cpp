/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BasicMathBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_tool_button.h>
#include <eqt_tool_button_array.h>

#include <ld_capabilities.h>
#include <ld_character_format.h>

#include <ld_assignment_operator_element.h>
#include <ld_variable_element.h>
#include <ld_literal_element.h>
#include <ld_subscript_index_operator_element.h>
#include <ld_subscript_row_column_operator_element.h>
#include <ld_matrix_operator_element.h>
#include <ld_pi_special_value_element.h>
#include <ld_eulers_number_special_value_element.h>
#include <ld_epsilon_special_value_element.h>
//#include <ld_infinity_special_value_element.h>
#include <ld_element_of_set_operator_element.h>
#include <ld_boolean_type_element.h>
#include <ld_integer_type_element.h>
#include <ld_real_type_element.h>
#include <ld_complex_type_element.h>
#include <ld_addition_operator_element.h>
#include <ld_subtraction_operator_element.h>
#include <ld_multiplication_operator_element.h>
#include <ld_division_operator_element.h>
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
#include "main_window.h"
#include "basic_math_main_window_proxy.h"
#include "basic_math_view_proxy.h"
#include "basic_math_builder.h"

const char BasicMathBuilder::builderName[] = "BasicMathBuilder";
const char BasicMathBuilder::basicMathDockName[] = "basic_math";

BasicMathBuilder::BasicMathBuilder(QObject* parent):EQt::Builder(parent) {}


BasicMathBuilder::~BasicMathBuilder() {}


unsigned BasicMathBuilder::priority() const {
    return builderPriority;
}


const char* BasicMathBuilder::builderIdentifier() const {
    return builderName;
}


const char* BasicMathBuilder::plugInName() const {
    return "";
}

#if (defined(Q_OS_DARWIN))

    #define MENU_TEXT(_text,_suffix) ((_text) + (_suffix))

#else

    #define MENU_TEXT(_text,_suffix) (_text)

#endif

void BasicMathBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* assignmentOperatorAction = window->addMenuAction(
        EQt::Application::icon("assignment_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Assignment"), tr(" (=)")),
        Ld::AssignmentOperatorElement::childProvides,
        "assignment_operator_action"
    );
    assignmentOperatorAction->setToolTip(
        tr(
            "Assignment operator and is equal to relational operator.  "
            "Assign value to a variable, define a function, or compare two values."
        )
    );
    assignmentOperatorAction->setWhatsThis(
        tr(
            "You can use the assignment operator to assign a value to a variable, define a new function, or compare "
            "two values.  The function of this operator is determined by context."
        )
    );

    QAction* variableAction = window->addMenuAction(
        EQt::Application::icon("variable"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Variable"), tr(" (a%1)").arg(QChar(0x2093))),
        Ld::VariableElement::childProvides,
        "variable_action"
    );
    variableAction->setToolTip(tr("Insert a variable"));
    variableAction->setWhatsThis(tr("You can use the Variable option to insert a variable into an equation."));

    QAction* literalAction = window->addMenuAction(
        EQt::Application::icon("literal"),
        MENU_TEXT(tr("&Insert | Basic &Math | Numeric &Literal"), tr(" (1.2)")),
        Ld::LiteralElement::childProvides,
        "literal_action"
    );
    literalAction->setToolTip(tr("Insert a literal"));
    literalAction->setWhatsThis(
        tr("You can use the literal option to insert a numeric literal value into an equation.")
    );

    QAction* subscriptIndexAction = window->addMenuAction(
        EQt::Application::icon("subscript_index_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | Subscript (Single &Index)"), tr(" (b%1)").arg(QChar(0x1D62))),
        Ld::SubscriptIndexOperatorElement::childProvides,
        "subscript_index_action"
    );
    subscriptIndexAction->setToolTip(tr("Subscripting operator, single index"));
    subscriptIndexAction->setWhatsThis(
        tr("You can use the subscripting operator to access the element of a tuple or matrix by a single index value.")
    );

    QAction* subscriptRowColumnAction = window->addMenuAction(
        EQt::Application::icon("subscript_row_column_operator"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | Subscript (&Row/Column)"),
            tr(" (b%1%2)").arg(QChar(0x1D62)).arg(QChar(0x2C7C))
        ),
        Ld::SubscriptRowColumnOperatorElement::childProvides,
        "subscript_row_column_action"
    );
    subscriptRowColumnAction->setToolTip(tr("Subscripting operator, row & column"));
    subscriptRowColumnAction->setWhatsThis(
        tr("You can use the subscripting operator to access the coefficient of a matrix by row and column.")
    );


    QAction* elementOfSetAction = window->addMenuAction(
        EQt::Application::icon("element_of_set"),
        MENU_TEXT(tr("&Insert | Basic &Math | Data &Types | &Element Of"),tr(" (%1)").arg(QChar(0x2208))),
        Ld::ElementOfSetOperatorElement::childProvides,
        "element_of_set_action"
    );
    elementOfSetAction->setToolTip(
        tr("\"Element Of\" operator.  Sets a variable type or determines if an element is contained in a set.")
    );
    elementOfSetAction->setWhatsThis(
        tr(
            "You can use the Element Of Set operator to set the type of a variable.  You can also use the Element Of "
            "Set operator to determine if an element is contained in a set.  The behavior of this operator depends "
            "on context."
        )
    );

    QAction* booleanTypeAction = window->addMenuAction(
        EQt::Application::icon("boolean_type"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | Data &Types | &Boolean Type"),
            tr(" (%1)").arg(QString::fromUtf8("\xF0\x9D\x94\xB9"))
        ),
        Ld::BooleanTypeElement::childProvides,
        "boolean_type_action"
    );
    booleanTypeAction->setToolTip(tr("Boolean type"));
    booleanTypeAction->setWhatsThis(tr("You can use the boolean set symbol to set a variable to an boolean type."));

    QAction* integerTypeAction = window->addMenuAction(
        EQt::Application::icon("integer_type"),
        MENU_TEXT(tr("&Insert | Basic &Math | Data &Types | &Integer Type"), tr(" (%1)").arg(QChar(0x2124))),
        Ld::IntegerTypeElement::childProvides,
        "integer_type_action"
    );
    integerTypeAction->setToolTip(tr("Integer type"));
    integerTypeAction->setWhatsThis(tr("You can use the integer set symbol to set a variable to an integer type."));

    QAction* realTypeAction = window->addMenuAction(
        EQt::Application::icon("real_type"),
        MENU_TEXT(tr("&Insert | Basic &Math | Data &Types | &Real Type"), tr(" (%1)").arg(QChar(0x211D))),
        Ld::RealTypeElement::childProvides,
        "real_type_action"
    );
    realTypeAction->setToolTip(tr("Real type"));
    realTypeAction->setWhatsThis(tr("You can use the real set symbol to set a variable to an real type."));

    QAction* complexTypeAction = window->addMenuAction(
        EQt::Application::icon("complex_type"),
        MENU_TEXT(tr("&Insert | Basic &Math | Data &Types | &Complex Type"), tr(" (%1)").arg(QChar(0x2102))),
        Ld::ComplexTypeElement::childProvides,
        "complex_type_action"
    );
    complexTypeAction->setToolTip(tr("Complex type"));
    complexTypeAction->setWhatsThis(tr("You can use the complex set symbol to set a variable to an complex type."));

    QAction* piSpecialValueAction = window->addMenuAction(
        EQt::Application::icon("pi"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Constants | &pi, 3.14..."), tr(" (%1)").arg(QChar(0x03C0))),
        Ld::PiSpecialValueElement::childProvides,
        "pi_special_value_action"
    );
    piSpecialValueAction->setToolTip(tr("Special value %1, 3.14...").arg(QChar(0x03C0)));
    piSpecialValueAction->setWhatsThis(
        tr("You can use this symbol for the special value %1, 3.14...").arg(QChar(0x02C0))
    );

    QString e = QString::fromUtf8("\xF0\x9D\x91\x92");
    QAction* eulersNumberSpecialValueAction = window->addMenuAction(
        EQt::Application::icon("eulers_number"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Constants | &Euler's number, 2.718..."), tr(" (%1)").arg(e)),
        Ld::EulersNumberSpecialValueElement::childProvides,
        "eulers_number_special_value_action"
    );
    eulersNumberSpecialValueAction->setToolTip(tr("Special value %1, 2.718...").arg(e));
    eulersNumberSpecialValueAction->setWhatsThis(
        tr("You can use this symbol for the special value %1, 2.718...").arg(e)
    );

    QString epsilon = QString::fromUtf8("\xF0\x9D\x9B\xA6");
    QAction* epsilonSpecialValueAction = window->addMenuAction(
        EQt::Application::icon("epsilon"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Constants | &Epsilon"), tr(" (%1)").arg(epsilon)),
        Ld::EpsilonSpecialValueElement::childProvides,
        "epsilon_special_value_action"
    );
    epsilonSpecialValueAction->setToolTip(tr("Special value %1 (Epsilon)").arg(epsilon));
    epsilonSpecialValueAction->setWhatsThis(
        tr(
            "You can use this symbol for the special value %1.  Epslion is the distance between 1 and the next "
            "closest real value that can be represented by the computer."
           )
    );

//    QAction* infinitySpecialValueAction = window->addMenuAction(
//        EQt::Application::icon("infinity"),
//        MENU_TEXT(tr("&Insert | Basic &Math | &Constants | &Infinity"), tr(" (%1)").arg(QChar(0x221E))),
//        Ld::InfinitySpecialValueElement::childProvides,
//        "infinity_special_value_action"
//    );
//    infinitySpecialValueAction->setToolTip(tr("Special value %1, 3.14...").arg(QChar(0x221E)));
//    infinitySpecialValueAction->setWhatsThis(
//        tr("You can use this symbol for infinity, %1").arg(QChar(0x221E))
//    );

    QAction* additionOperatorAction = window->addMenuAction(
        EQt::Application::icon("addition_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | &Addition"), tr(" (+)")),
        Ld::AdditionOperatorElement::childProvides,
        "addition_operator_action"
    );
    additionOperatorAction->setToolTip(tr("Addition operator.  Adds two values"));
    additionOperatorAction->setWhatsThis(tr("You can use the Addition operator to add two values."));

    QAction* subtractionOperatorAction = window->addMenuAction(
        EQt::Application::icon("subtraction_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | &Subtraction"), tr(" (-)")),
        Ld::SubtractionOperatorElement::childProvides,
        "subtraction_operator_action"
    );
    subtractionOperatorAction->setToolTip(
        tr(
            "Subtraction operator.  Subtracts two values.  You can also use this operator to perform set relative "
            "complement or set difference operations."
        )
    );
    subtractionOperatorAction->setWhatsThis(
        tr(
           "You can use the Subtraction operator to subtract two values.  You can also use this operator to perform "
           "set relative complement or set difference operations."
        )
    );

    QAction* multiplicationOperatorAction = window->addMenuAction(
        EQt::Application::icon("multiplication_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | &Multiplication"), tr(" (%1)").arg(QChar(0x00D7))),
        Ld::MultiplicationOperatorElement::childProvides,
        "multiplication_operator_action"
    );
    multiplicationOperatorAction->setToolTip(tr("Multiplication operator.  Multiplies two values"));
    multiplicationOperatorAction->setWhatsThis(tr("You can use the Multiplication operator to add two values."));

    QAction* divisionOperatorAction = window->addMenuAction(
        EQt::Application::icon("division_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | &Division"), tr(" (%1)").arg(QChar(0x00F7))),
        Ld::DivisionOperatorElement::childProvides,
        "division_operator_action"
    );
    divisionOperatorAction->setToolTip(tr("Division operator.  Divides two values"));
    divisionOperatorAction->setWhatsThis(tr("You can use the division operator to divide two values."));

    QAction* fractionOperatorAction = window->addMenuAction(
        EQt::Application::icon("fraction_operator"),
        tr("&Insert | Basic &Math | &Operators | &Basic | &Fraction"),
        Ld::DivisionOperatorElement::childProvides,
        "fraction_operator_action"
    );
    fractionOperatorAction->setToolTip(tr("Fraction (division) operator.  Divides two values"));
    fractionOperatorAction->setWhatsThis(tr("You can use the Fraction operator to divide two values."));

    window->addSeparator(tr("&Insert | Basic &Math | &Operators | &Basic"));

    QAction* unaryPlusOperatorAction = window->addMenuAction(
        EQt::Application::icon("unary_plus_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | Unary &Plus"), tr(" (+x)")),
        Ld::UnaryPlusOperatorElement::childProvides,
        "unary_plus_operator_action"
    );
    unaryPlusOperatorAction->setToolTip(tr("Unary plus operator.  Multiplies a value by +1."));
    unaryPlusOperatorAction->setWhatsThis(tr("You can use the unary plus operator to multiply a value by +1."));

    QAction* unaryMinusOperatorAction = window->addMenuAction(
        EQt::Application::icon("unary_minus_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | Unary &Minus"), tr(" (-x)")),
        Ld::UnaryMinusOperatorElement::childProvides,
        "unary_minus_operator_action"
    );
    unaryMinusOperatorAction->setToolTip(tr("Unary minus operator.  Multiplies a value by -1."));
    unaryMinusOperatorAction->setWhatsThis(tr("You can use the unary minus operator to multiply a value by -1."));

    QAction* groupingParenthesisOperatorAction = window->addMenuAction(
        EQt::Application::icon("parenthesis"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Grouping | &Parenthesis"), tr(" ( (x) )")),
        Ld::GroupingParenthesisOperatorElement::childProvides,
        "grouping_parenthesis_operator_action"
    );
    groupingParenthesisOperatorAction->setToolTip(tr("Parenthesis grouping operator."));
    groupingParenthesisOperatorAction->setWhatsThis(
        tr(
            "You can use the parenthesis grouping operator to insert parenthesis into an equation.  Note that this "
            "operator is rarely needed."
        )
    );

    QAction* groupingBracketsOperatorAction = window->addMenuAction(
        EQt::Application::icon("brackets"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Grouping | &Brackets"), tr(" ( [x] )")),
        Ld::GroupingBracketsOperatorElement::childProvides,
        "grouping_brackets_operator_action"
    );
    groupingBracketsOperatorAction->setToolTip(tr("Brackets grouping operator."));
    groupingBracketsOperatorAction->setWhatsThis(
        tr(
            "You can use the brackets grouping operator to insert brackets into an equation.  Note that this "
            "operator is rarely needed."
        )
    );

    QAction* groupingBracesOperatorAction = window->addMenuAction(
        EQt::Application::icon("braces"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Grouping | B&races"), tr(" ( {x} )")),
        Ld::GroupingBracesOperatorElement::childProvides,
        "grouping_braces_operator_action"
    );
    groupingBracesOperatorAction->setToolTip(tr("Braces grouping operator."));
    groupingBracesOperatorAction->setWhatsThis(
        tr(
            "You can use the braces grouping operator to insert braces into an equation.  Note that this "
            "operator is rarely needed."
        )
    );

    window->addSeparator(tr("&Insert | Basic &Math | &Operators | &Basic"));

    QAction* powerOperatorAction = window->addMenuAction(
        EQt::Application::icon("power"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | P&ower"), tr(" (a%1)").arg(QChar(0x207F))),
        Ld::PowerOperatorElement::childProvides,
        "power_operator_action"
    );
    powerOperatorAction->setToolTip(tr("Power operator"));
    powerOperatorAction->setWhatsThis(
        tr("You can use the power operator to raise a value to a power.")
    );

    QAction* squareRootOperatorAction = window->addMenuAction(
        EQt::Application::icon("square_root_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | S&quare Root"), tr(" (%1)").arg(QChar(0x221A))),
        Ld::SquareRootOperatorElement::childProvides,
        "square_root_operator_action"
    );
    squareRootOperatorAction->setToolTip(tr("Square root operator"));
    squareRootOperatorAction->setWhatsThis(
        tr("You can use the square root operator to take the square root of a value.")
    );

    QAction* rootOperatorAction = window->addMenuAction(
        EQt::Application::icon("root_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | &Root"), tr(" (%1)").arg(QChar(0x221C))),
        Ld::RootOperatorElement::childProvides,
        "root_operator_action"
    );
    rootOperatorAction->setToolTip(tr("Nth root operator"));
    rootOperatorAction->setWhatsThis(
        tr("You can use the root operator to take the Nth root of a value.")
    );

    QAction* complexConjugateOperatorAction = window->addMenuAction(
        EQt::Application::icon("complex_conjugate"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Basic | &Complex Conjugate"),
            tr(" (%1%2)").arg(QChar(0x2002)).arg(QChar(0x20F0))
        ),
        Ld::ComplexConjugateOperatorElement::childProvides,
        "complex_conjugate_operator_action"
    );
    complexConjugateOperatorAction->setToolTip(tr("Complex conjugate operator"));
    complexConjugateOperatorAction->setWhatsThis(
        tr("You can use the complex conjugate operator to calculate the complex conjugate of a value.")
    );

    QAction* factorialOperatorAction = window->addMenuAction(
        EQt::Application::icon("factorial"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Basic | &Factorial"), tr(" (!)")),
        Ld::FactorialOperatorElement::childProvides,
        "factorial_operator_action"
    );
    factorialOperatorAction->setToolTip(tr("Factorial operator"));
    factorialOperatorAction->setWhatsThis(
        tr("You can use the factorial operator to calculate the factorial of a non-negative whole number.")
    );

    window->addSeparator(tr("&Insert | Basic &Math | &Operators | &Basic"));

    QAction* absoluteValueOperatorAction = window->addMenuAction(
        EQt::Application::icon("absolute_value"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Basic | A&bsolute Value, Determinant, Set Cardinality"),
            tr(" ( %1x%1 )").arg(QChar(0x2223))
        ),
        Ld::AbsoluteValueOperatorElement::childProvides,
        "absolute_value_operator_action"
    );
    absoluteValueOperatorAction->setToolTip(tr("Absolute value, magnitude, determinant, and set cardinality"));
    absoluteValueOperatorAction->setWhatsThis(
        tr(
            "You can use the this operator to calculate the absolute value or magnitude, determinant of a matrix or "
            "the cardinality of a set."
          )
    );

    QAction* floorOperatorAction = window->addMenuAction(
        EQt::Application::icon("floor"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Basic | &Floor"),
            tr(" ( %1x%2 )").arg(QChar(0x230A)).arg(QChar(0x230B))
        ),
        Ld::FloorOperatorElement::childProvides,
        "floor_operator_action"
    );
    floorOperatorAction->setToolTip(tr("Floor, next lower integer value"));
    floorOperatorAction->setWhatsThis(
        tr(
            "You can use the this operator to calculate the floor of a value.  The floor is defined as the closest "
            "integer equal to or less than the provided value.  The floor of 3.5 is 3.  The floor of -4.5 is -5."
          )
    );

    QAction* ceilingOperatorAction = window->addMenuAction(
        EQt::Application::icon("ceiling"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Basic | &Ceiling"),
            tr(" ( %1x%2 )").arg(QChar(0x2308)).arg(QChar(0x2309))
        ),
        Ld::CeilingOperatorElement::childProvides,
        "ceiling_operator_action"
    );
    ceilingOperatorAction->setToolTip(tr("Ceiling, next higher integer value"));
    ceilingOperatorAction->setWhatsThis(
        tr(
            "You can use the this operator to calculate the ceiling of a value.  The ceiling is defined as the closest "
            "integer equal to or greater than the provided value.  The ceiling of 3.5 is 3.  The ceiling of -4.5 is "
            "-4."
          )
    );

    QAction* nearestIntegerOperatorAction = window->addMenuAction(
        EQt::Application::icon("nearest_integer"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Basic | &Nearest Integer"),
            tr(" ( %1x%2 )").arg(QChar(0x230A)).arg(QChar(0x2309))
        ),
        Ld::NearestIntegerOperatorElement::childProvides,
        "nearest_integer_operator_action"
    );
    nearestIntegerOperatorAction->setToolTip(tr("Nearest integer, whole value cloest to a given value"));
    nearestIntegerOperatorAction->setWhatsThis(
        tr(
            "You can use the this operator to calculate the nearest integer to a value.  The nearest integer is "
            "defined as the closest whole number to the provided number.  Values that are 0.5 away from the next "
            "integer will round to the closest even integer.  The nearest integer of 3.3 is 3.  The nearest integer "
            "to 3.51 is 4.  The nearest integer to 2.5 is 2.  The nearest integer to 5.5 is 6."
          )
    );

    QAction* notEqualToOperatorAction = window->addMenuAction(
        EQt::Application::icon("not_equal_to_operator"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Relational | &Not Equal To"),
            tr(" (%1)").arg(QChar(0x2260))
        ),
        Ld::NotEqualToOperatorElement::childProvides,
        "not_equal_to_operator_action"
    );
    notEqualToOperatorAction->setToolTip(tr("Not equal to relational operator."));
    notEqualToOperatorAction->setWhatsThis(
        tr("You can use the not equal to relational operator to determine if two values are not equal.")
    );

    QAction* lessThanOperatorAction = window->addMenuAction(
        EQt::Application::icon("less_than_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Relational | &Less Than"), tr(" (<)")),
        Ld::LessThanOperatorElement::childProvides,
        "less_than_operator_action"
    );
    lessThanOperatorAction->setToolTip(tr("Less than relational operator."));
    lessThanOperatorAction->setWhatsThis(
        tr("You can use the less than relational operator to determine if one value is less than another value.")
    );

    QAction* greaterThanOperatorAction = window->addMenuAction(
        EQt::Application::icon("greater_than_operator"),
        MENU_TEXT(tr("&Insert | Basic &Math | &Operators | &Relational | &Greater Than"), tr(" (>)")),
        Ld::GreaterThanOperatorElement::childProvides,
        "greater_than_operator_action"
    );
    greaterThanOperatorAction->setToolTip(tr("Greater than relational operator."));
    greaterThanOperatorAction->setWhatsThis(
        tr("You can use the greater than relational operator to determine if one value is greater than another value.")
    );

    QAction* lessThanOrEqualToOperatorAction = window->addMenuAction(
        EQt::Application::icon("less_than_or_equal_to_operator"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Relational | L&ess Than Or Equal To"),
            tr(" (%1)").arg(QChar(0x2264))
        ),
        Ld::LessThanOrEqualToOperatorElement::childProvides,
        "less_than_or_equal_to_operator_action"
    );
    lessThanOrEqualToOperatorAction->setToolTip(tr("Less than or equal to relational operator."));
    lessThanOrEqualToOperatorAction->setWhatsThis(
        tr(
            "You can use the less than or equal to relational operator to determine if one value is less than or "
            "equal to another value."
        )
    );

    QAction* greaterThanOrEqualToOperatorAction = window->addMenuAction(
        EQt::Application::icon("greater_than_or_equal_to_operator"),
        MENU_TEXT(
            tr("&Insert | Basic &Math | &Operators | &Relational | G&reater Than Or Equal To"),
            tr(" (%1)").arg(QChar(0x2265))
        ),
        Ld::GreaterThanOrEqualToOperatorElement::childProvides,
        "greater_than_or_equal_to_operator_action"
    );
    greaterThanOrEqualToOperatorAction->setToolTip(tr("Greater than or equal to relational operator."));
    greaterThanOrEqualToOperatorAction->setWhatsThis(
        tr(
            "You can use the greater than or equal to relational operator to determine if one value is greater than "
            "or equal to another value."
        )
    );
}


void BasicMathBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        basicMathDockName,
        tr("Basic Math"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Area::LEFT, true)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        basicMathDockName,
        tr("&View | &Docks"),
        "basic_math_dock_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Basic Math dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Basic Math dock.  You can use the Basic Math dock to add "
            "math elements."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");

    dock->setHorizontalLayout(horizontalLayout);
    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* BasicMathBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new BasicMathMainWindowProxy(window);
}


void BasicMathBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* basicMathDockToggleAction = mainWindow->action("basic_math_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("bmdock"), basicMathDockToggleAction);
    Q_ASSERT(success);

    QAction* assignmentOperatorAction = mainWindow->action("assignment_operator_action");
    success = mainWindow->setCommand(tr("="), assignmentOperatorAction);
    Q_ASSERT(success);

    QAction* variableAction = mainWindow->action("variable_action");
    success = mainWindow->setCommand(tr("var"), variableAction);
    Q_ASSERT(success);

    QAction* literalAction = mainWindow->action("literal_action");
    success = mainWindow->setCommand(tr("lit"), literalAction);
    Q_ASSERT(success);

    QAction* subscriptIndexAction = mainWindow->action("subscript_index_action");
    success = mainWindow->setCommand(tr("__"), subscriptIndexAction);
    Q_ASSERT(success);

    QAction* subscriptRowColumnAction = mainWindow->action("subscript_row_column_action");
    success = mainWindow->setCommand(tr("_"), subscriptRowColumnAction);
    Q_ASSERT(success);

    QAction* piSpecialValueAction = window->action("pi_special_value_action");
    success = mainWindow->setCommand(tr("piconst"), piSpecialValueAction);
    Q_ASSERT(success);

    QAction* eulersNumberSpecialValueAction = window->action("eulers_number_special_value_action");
    success = mainWindow->setCommand(tr("econst"), eulersNumberSpecialValueAction);
    Q_ASSERT(success);

    QAction* epsilonSpecialValueAction = window->action("epsilon_special_value_action");
    success = mainWindow->setCommand(tr("epsilonconst"), epsilonSpecialValueAction);
    Q_ASSERT(success);

//    QAction* infinitySpecialValueAction = window->action("infinity_special_value_action");
//    success = mainWindow->setCommand(tr("inftyconst"), infinitySpecialValueAction);
//    Q_ASSERT(success);

    QAction* elementOfSetAction = mainWindow->action("element_of_set_action");
    success = mainWindow->setCommand(tr("in"), elementOfSetAction);
    Q_ASSERT(success);

    QAction* booleanTypeAction = mainWindow->action("boolean_type_action");
    success = mainWindow->setCommand(tr("mathbb{B}"), booleanTypeAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("bset"), booleanTypeAction);
    Q_ASSERT(success);

    QAction* integerTypeAction = mainWindow->action("integer_type_action");
    success = mainWindow->setCommand(tr("mathbb{Z}"), integerTypeAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("iset"), integerTypeAction);
    Q_ASSERT(success);

    QAction* realTypeAction = mainWindow->action("real_type_action");
    success = mainWindow->setCommand(tr("mathbb{R}"), realTypeAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("rset"), realTypeAction);
    Q_ASSERT(success);

    QAction* complexTypeAction = mainWindow->action("complex_type_action");
    success = mainWindow->setCommand(tr("mathbb{C}"), complexTypeAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("cset"), complexTypeAction);
    Q_ASSERT(success);

    QAction* additionOperatorAction = window->action("addition_operator_action");
    success = mainWindow->setCommand(tr("+"), additionOperatorAction);
    Q_ASSERT(success);

    QAction* subtractionOperatorAction = window->action("subtraction_operator_action");
    success = mainWindow->setCommand(tr("-"), subtractionOperatorAction);
    Q_ASSERT(success);

    QAction* multiplicationOperatorAction = window->action("multiplication_operator_action");
    success = mainWindow->setCommand(tr("*"), multiplicationOperatorAction);
    Q_ASSERT(success);

    QAction* divisionOperatorAction = window->action("division_operator_action");
    success = mainWindow->setCommand(tr("div"), divisionOperatorAction);
    Q_ASSERT(success);

    QAction* fractionOperatorAction = window->action("fraction_operator_action");
    success = mainWindow->setCommand(tr("frac"), fractionOperatorAction);
    Q_ASSERT(success);

    QAction* unaryPlusOperatorAction = window->action("unary_plus_operator_action");
    success = mainWindow->setCommand(tr("plus"), unaryPlusOperatorAction);
    Q_ASSERT(success);

    QAction* unaryMinusOperatorAction = window->action("unary_minus_operator_action");
    success = mainWindow->setCommand(tr("minus"), unaryMinusOperatorAction);
    Q_ASSERT(success);

    QAction* groupingParenthesisOperatorAction = window->action("grouping_parenthesis_operator_action");
    success = mainWindow->setCommand(tr("("), groupingParenthesisOperatorAction);
    Q_ASSERT(success);

    QAction* groupingBracketsOperatorAction = window->action("grouping_brackets_operator_action");
    success = mainWindow->setCommand(tr("["), groupingBracketsOperatorAction);
    Q_ASSERT(success);

    QAction* groupingBracesOperatorAction = window->action("grouping_braces_operator_action");
    success = mainWindow->setCommand(tr("{"), groupingBracesOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("lbrace"), groupingBracesOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("rbrace"), groupingBracesOperatorAction);
    Q_ASSERT(success);

    QAction* powerOperatorAction = window->action("power_operator_action");
    success = mainWindow->setCommand(tr("^"), powerOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("**"), powerOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("pow"), powerOperatorAction);
    Q_ASSERT(success);

    QAction* squareRootOperatorAction = window->action("square_root_operator_action");
    success = mainWindow->setCommand(tr("sqrt"), squareRootOperatorAction);
    Q_ASSERT(success);

    QAction* rootOperatorAction = window->action("root_operator_action");
    success = mainWindow->setCommand(tr("nroot"), rootOperatorAction);
    Q_ASSERT(success);

    QAction* complexConjugateOperatorAction = window->action("complex_conjugate_operator_action");
    success = mainWindow->setCommand(tr("conj"), complexConjugateOperatorAction);
    Q_ASSERT(success);

    QAction* factorialOperatorAction = window->action("factorial_operator_action");
    success = mainWindow->setCommand(tr("fact"), factorialOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("!"), factorialOperatorAction);
    Q_ASSERT(success);

    QAction* absoluteValueOperatorAction = window->action("absolute_value_operator_action");
    success = mainWindow->setCommand(tr("abs"), absoluteValueOperatorAction);
    Q_ASSERT(success);

    QAction* floorOperatorAction = window->action("floor_operator_action");
    success = mainWindow->setCommand(tr("floor"), floorOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("lfloor"), floorOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("rfloor"), floorOperatorAction);
    Q_ASSERT(success);

    QAction* ceilingOperatorAction = window->action("ceiling_operator_action");
    success = mainWindow->setCommand(tr("ceil"), ceilingOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("lceil"), ceilingOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("rceil"), ceilingOperatorAction);
    Q_ASSERT(success);

    QAction* nearestIntegerOperatorAction = window->action("nearest_integer_operator_action");
    success = mainWindow->setCommand(tr("nint"), nearestIntegerOperatorAction);
    Q_ASSERT(success);

    QAction* notEqualToOperatorAction = window->action("not_equal_to_operator_action");
    success = mainWindow->setCommand(tr("neq"), notEqualToOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("ne"), notEqualToOperatorAction);
    Q_ASSERT(success);

    QAction* lessThanOperatorAction = window->action("less_than_operator_action");
    success = mainWindow->setCommand(tr("<"), lessThanOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("lt"), lessThanOperatorAction);
    Q_ASSERT(success);

    QAction* greaterThanOperatorAction = window->action("greater_than_operator_action");
    success = mainWindow->setCommand(tr(">"), greaterThanOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("gt"), greaterThanOperatorAction);
    Q_ASSERT(success);

    QAction* lessThanOrEqualToOperatorAction = window->action("less_than_or_equal_to_operator_action");
    success = mainWindow->setCommand(tr("leq"), lessThanOrEqualToOperatorAction);
    Q_ASSERT(success);

    QAction* greaterThanOrEqualToOperatorAction = window->action("greater_than_or_equal_to_operator_action");
    success = mainWindow->setCommand(tr("geq"), greaterThanOrEqualToOperatorAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* BasicMathBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new BasicMathViewProxy(view, window);
}


void BasicMathBuilder::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("horizontal_buttons");
    horizontalLayout->addLayout(buttonsLayout);

    buildButtonLayout1(window, buttonsLayout);
    buttonsLayout->setColumnMinimumWidth(8,4);
    buildButtonLayout2(window, buttonsLayout, 0, 9);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(buttonsLayout, 4);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void BasicMathBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("vertical_buttons");
    verticalLayout->addLayout(buttonsLayout);

    buildButtonLayout1(window, buttonsLayout, 0, 0);
    buildButtonLayout2(window, buttonsLayout, 4, 0);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(buttonsLayout, 8);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void BasicMathBuilder::buildButtonLayout1(
        EQt::ProgrammaticMainWindow* window,
        QGridLayout*                 buttonsLayout,
        unsigned                     topEdge,
        unsigned                     leftEdge
    ) {
    QAction* assignmentOperatorAction = window->action("assignment_operator_action");
    EQt::ToolButton* assignmentOperatorToolButton = new EQt::ToolButton(assignmentOperatorAction);
    assignmentOperatorToolButton->setRelativeSizeFactor(-2);
    assignmentOperatorToolButton->setXScaling(2.0F);
    assignmentOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(assignmentOperatorToolButton, topEdge + 0, leftEdge + 0, 1, 2);

    QAction* variableAction = window->action("variable_action");
    EQt::ToolButton* variableToolButton = new EQt::ToolButton(variableAction);
    variableToolButton->setRelativeSizeFactor(-2);
    buttonsLayout->addWidget(variableToolButton, topEdge + 0, leftEdge + 2, 1, 1);

    QAction* literalAction = window->action("literal_action");
    EQt::ToolButton* literalToolButton = new EQt::ToolButton(literalAction);
    literalToolButton->setRelativeSizeFactor(-2);
    literalToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(literalToolButton, topEdge + 0, leftEdge + 3, 1, 1);

    QAction* subscriptIndexAction = window->action("subscript_index_action");
    EQt::ToolButton* subscriptIndexToolButton = new EQt::ToolButton(subscriptIndexAction);
    subscriptIndexToolButton->setRelativeSizeFactor(-2);
    subscriptIndexToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(subscriptIndexToolButton, topEdge + 0, leftEdge + 4, 1, 1);

    QAction* subscriptRowColumnAction = window->action("subscript_row_column_action");
    EQt::ToolButton* subscriptRowColumnToolButton = new EQt::ToolButton(subscriptRowColumnAction);
    subscriptRowColumnToolButton->setRelativeSizeFactor(-2);
    subscriptRowColumnToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(subscriptRowColumnToolButton, topEdge + 0, leftEdge + 5, 1, 1);

    QAction* matrixOperatorAction = window->action("matrix_operator_action");
    EQt::ToolButton* matrixOperatorToolButton = new EQt::ToolButton(matrixOperatorAction);
    matrixOperatorToolButton->setRelativeSizeFactor(0);
    matrixOperatorToolButton->setXScaling(1.0F);
    matrixOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonsLayout->addWidget(matrixOperatorToolButton, topEdge + 0, leftEdge + 6, 2, 2);

    QAction* elementOfSetAction = window->action("element_of_set_action");
    EQt::ToolButton* elementOfSetToolButton = new EQt::ToolButton(elementOfSetAction);
    elementOfSetToolButton->setRelativeSizeFactor(-2);
    elementOfSetToolButton->setXScaling(2.0F);
    elementOfSetToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(elementOfSetToolButton, topEdge + 1, leftEdge + 0, 1, 2);

    QAction* booleanTypeAction = window->action("boolean_type_action");
    EQt::ToolButton* booleanTypeToolButton = new EQt::ToolButton(booleanTypeAction);
    booleanTypeToolButton->setRelativeSizeFactor(-2);
    buttonsLayout->addWidget(booleanTypeToolButton, topEdge + 1, leftEdge + 2, 1, 1);

    QAction* integerTypeAction = window->action("integer_type_action");
    EQt::ToolButton* integerTypeToolButton = new EQt::ToolButton(integerTypeAction);
    integerTypeToolButton->setRelativeSizeFactor(-2);
    buttonsLayout->addWidget(integerTypeToolButton, topEdge + 1, leftEdge + 3, 1, 1);

    QAction* realTypeAction = window->action("real_type_action");
    EQt::ToolButton* realTypeToolButton = new EQt::ToolButton(realTypeAction);
    realTypeToolButton->setRelativeSizeFactor(-2);
    buttonsLayout->addWidget(realTypeToolButton, topEdge + 1, leftEdge + 4);

    QAction* complexTypeAction = window->action("complex_type_action");
    EQt::ToolButton* complexTypeToolButton = new EQt::ToolButton(complexTypeAction);
    complexTypeToolButton->setRelativeSizeFactor(-2);
    buttonsLayout->addWidget(complexTypeToolButton, topEdge + 1, leftEdge + 5);

    QAction* unaryPlusAction = window->action("unary_plus_operator_action");
    EQt::ToolButton* unaryPlusToolButton = new EQt::ToolButton(unaryPlusAction);
    unaryPlusToolButton->setRelativeSizeFactor(-2);
    unaryPlusToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(unaryPlusToolButton, topEdge + 2, leftEdge + 0, 1, 1);

    QAction* unaryMinusAction = window->action("unary_minus_operator_action");
    EQt::ToolButton* unaryMinusToolButton = new EQt::ToolButton(unaryMinusAction);
    unaryMinusToolButton->setRelativeSizeFactor(-2);
    unaryMinusToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(unaryMinusToolButton, topEdge + 2, leftEdge + 1, 1, 1);

    QAction* groupingParenthesisOperatorAction = window->action("grouping_parenthesis_operator_action");
    EQt::ToolButton* groupingParenthesisToolButton = new EQt::ToolButton(groupingParenthesisOperatorAction);
    groupingParenthesisToolButton->setRelativeSizeFactor(-2);
    groupingParenthesisToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(groupingParenthesisToolButton, topEdge + 2, leftEdge + 2, 1, 1);

    QAction* groupingBracketsOperatorAction = window->action("grouping_brackets_operator_action");
    EQt::ToolButton* groupingBracketsToolButton = new EQt::ToolButton(groupingBracketsOperatorAction);
    groupingBracketsToolButton->setRelativeSizeFactor(-2);
    groupingBracketsToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(groupingBracketsToolButton, topEdge + 2, leftEdge + 3, 1, 1);

    QAction* groupingBracesOperatorAction = window->action("grouping_braces_operator_action");
    EQt::ToolButton* groupingBracesToolButton = new EQt::ToolButton(groupingBracesOperatorAction);
    groupingBracesToolButton->setRelativeSizeFactor(-2);
    groupingBracesToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(groupingBracesToolButton, topEdge + 2, leftEdge + 4, 1, 1);

    QAction* piSpecialValueAction = window->action("pi_special_value_action");
    EQt::ToolButton* piSpecialValueToolButton = new EQt::ToolButton(piSpecialValueAction);
    piSpecialValueToolButton->setRelativeSizeFactor(-2);
    piSpecialValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(piSpecialValueToolButton, topEdge + 2, leftEdge + 5, 1, 1);

    QAction* eulersNumberSpecialValueAction = window->action("eulers_number_special_value_action");
    EQt::ToolButton* eulersNumberSpecialValueToolButton = new EQt::ToolButton(eulersNumberSpecialValueAction);
    eulersNumberSpecialValueToolButton->setRelativeSizeFactor(-2);
    eulersNumberSpecialValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(eulersNumberSpecialValueToolButton, topEdge + 2, leftEdge + 6, 1, 1);

    QAction* epsilonSpecialValueAction = window->action("epsilon_special_value_action");
    EQt::ToolButton* epsilonSpecialValueToolButton = new EQt::ToolButton(epsilonSpecialValueAction);
    epsilonSpecialValueToolButton->setRelativeSizeFactor(-2);
    epsilonSpecialValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(epsilonSpecialValueToolButton, topEdge + 2, leftEdge + 7, 1, 1);

    QAction* additionOperatorAction = window->action("addition_operator_action");
    EQt::ToolButton* additionOperatorToolButton = new EQt::ToolButton(additionOperatorAction);
    additionOperatorToolButton->setRelativeSizeFactor(-2);
    additionOperatorToolButton->setXScaling(2.0F);
    additionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(additionOperatorToolButton, topEdge + 3, leftEdge + 0, 1, 2);

    QAction* subtractionOperatorAction = window->action("subtraction_operator_action");
    EQt::ToolButton* subtractionOperatorToolButton = new EQt::ToolButton(subtractionOperatorAction);
    subtractionOperatorToolButton->setRelativeSizeFactor(-2);
    subtractionOperatorToolButton->setXScaling(2.0F);
    subtractionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(subtractionOperatorToolButton, topEdge + 3, leftEdge + 2, 1, 2);

    QAction* multiplicationOperatorAction = window->action("multiplication_operator_action");
    EQt::ToolButton* multiplicationOperatorToolButton = new EQt::ToolButton(multiplicationOperatorAction);
    multiplicationOperatorToolButton->setRelativeSizeFactor(-2);
    multiplicationOperatorToolButton->setXScaling(2.0F);
    multiplicationOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(multiplicationOperatorToolButton, topEdge + 3, leftEdge + 4, 1, 2);

    QAction* divisionOperatorAction = window->action("division_operator_action");
    EQt::ToolButton* divisionOperatorToolButton = new EQt::ToolButton(divisionOperatorAction);
    divisionOperatorToolButton->setRelativeSizeFactor(-2);
    divisionOperatorToolButton->setXScaling(2.0F);
    divisionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(divisionOperatorToolButton, topEdge + 3, leftEdge + 6, 1, 2);
}


void BasicMathBuilder::buildButtonLayout2(
        EQt::ProgrammaticMainWindow* window,
        QGridLayout*                 buttonsLayout,
        unsigned                     topEdge,
        unsigned                     leftEdge
    ) {
    QAction* fractionOperatorAction = window->action("fraction_operator_action");
    EQt::ToolButton* fractionOperatorToolButton = new EQt::ToolButton(fractionOperatorAction);
    fractionOperatorToolButton->setRelativeSizeFactor(0);
    fractionOperatorToolButton->setXScaling(1.0F);
    fractionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    buttonsLayout->addWidget(fractionOperatorToolButton, topEdge + 0, leftEdge + 0, 2, 2);

    QAction* absoluteValueOperatorAction = window->action("absolute_value_operator_action");
    EQt::ToolButton* absoluteValueToolButton = new EQt::ToolButton(absoluteValueOperatorAction);
    absoluteValueToolButton->setRelativeSizeFactor(-2);
    absoluteValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(absoluteValueToolButton, topEdge + 0, leftEdge + 2, 1, 1);

    QAction* nearestIntegerOperatorAction = window->action("nearest_integer_operator_action");
    EQt::ToolButton* nearestIntegerToolButton = new EQt::ToolButton(nearestIntegerOperatorAction);
    nearestIntegerToolButton->setRelativeSizeFactor(-2);
    nearestIntegerToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(nearestIntegerToolButton, topEdge + 0, leftEdge + 3, 1, 1);

    QAction* floorOperatorAction = window->action("floor_operator_action");
    EQt::ToolButton* floorToolButton = new EQt::ToolButton(floorOperatorAction);
    floorToolButton->setRelativeSizeFactor(-2);
    floorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(floorToolButton, topEdge + 1, leftEdge + 2, 1, 1);

    QAction* ceilingOperatorAction = window->action("ceiling_operator_action");
    EQt::ToolButton* ceilingToolButton = new EQt::ToolButton(ceilingOperatorAction);
    ceilingToolButton->setRelativeSizeFactor(-2);
    ceilingToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(ceilingToolButton, topEdge + 1, leftEdge + 3, 1, 1);

    QAction* squareRootOperatorAction = window->action("square_root_operator_action");
    EQt::ToolButton* squareRootToolButton = new EQt::ToolButton(squareRootOperatorAction);
    squareRootToolButton->setRelativeSizeFactor(-2);
    squareRootToolButton->setXScaling(2.0F);
    squareRootToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(squareRootToolButton, topEdge + 0, leftEdge + 4, 1, 2);

    QAction* rootOperatorAction = window->action("root_operator_action");
    EQt::ToolButton* rootToolButton = new EQt::ToolButton(rootOperatorAction);
    rootToolButton->setRelativeSizeFactor(-2);
    rootToolButton->setXScaling(2.0F);
    rootToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(rootToolButton, topEdge + 0, leftEdge + 6, 1, 2);

    QAction* complexConjugateOperatorAction = window->action("complex_conjugate_operator_action");
    EQt::ToolButton* complexConjugateToolButton = new EQt::ToolButton(complexConjugateOperatorAction);
    complexConjugateToolButton->setRelativeSizeFactor(-2);
    complexConjugateToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(complexConjugateToolButton, topEdge + 1, leftEdge + 4, 1, 1);

    QAction* powerOperatorAction = window->action("power_operator_action");
    EQt::ToolButton* powerToolButton = new EQt::ToolButton(powerOperatorAction);
    powerToolButton->setRelativeSizeFactor(-2);
    powerToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(powerToolButton, topEdge + 1, leftEdge + 5, 1, 1);

    QAction* factorialOperatorAction = window->action("factorial_operator_action");
    EQt::ToolButton* factorialToolButton = new EQt::ToolButton(factorialOperatorAction);
    factorialToolButton->setRelativeSizeFactor(-2);
    factorialToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(factorialToolButton, topEdge + 1, leftEdge + 6, 1, 1);

    QAction* matrixTransposeOperatorAction = window->action("matrix_transpose_operator_action");
    EQt::ToolButton* matrixTransposeOperatorToolButton = new EQt::ToolButton(matrixTransposeOperatorAction);
    matrixTransposeOperatorToolButton->setRelativeSizeFactor(-2);
    matrixTransposeOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(matrixTransposeOperatorToolButton, topEdge + 1, leftEdge + 7, 1, 1);

    QAction* notEqualToOperatorAction = window->action("not_equal_to_operator_action");
    EQt::ToolButton* notEqualToOperatorToolButton = new EQt::ToolButton(notEqualToOperatorAction);
    notEqualToOperatorToolButton->setRelativeSizeFactor(-2);
    notEqualToOperatorToolButton->setXScaling(2.0F);
    notEqualToOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(notEqualToOperatorToolButton, topEdge + 2, leftEdge + 2, 1, 2);

    QAction* lessThanOperatorAction = window->action("less_than_operator_action");
    EQt::ToolButton* lessThanOperatorToolButton = new EQt::ToolButton(lessThanOperatorAction);
    lessThanOperatorToolButton->setRelativeSizeFactor(-2);
    lessThanOperatorToolButton->setXScaling(2.0F);
    lessThanOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(lessThanOperatorToolButton, topEdge + 2, leftEdge + 4, 1, 2);

    QAction* greaterThanOperatorAction = window->action("greater_than_operator_action");
    EQt::ToolButton* greaterThanOperatorToolButton = new EQt::ToolButton(greaterThanOperatorAction);
    greaterThanOperatorToolButton->setRelativeSizeFactor(-2);
    greaterThanOperatorToolButton->setXScaling(2.0F);
    greaterThanOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(greaterThanOperatorToolButton, topEdge + 2, leftEdge + 6, 1, 2);

    QAction* lessThanOrEqualToOperatorAction = window->action("less_than_or_equal_to_operator_action");
    EQt::ToolButton* lessThanOrEqualToOperatorToolButton = new EQt::ToolButton(lessThanOrEqualToOperatorAction);
    lessThanOrEqualToOperatorToolButton->setRelativeSizeFactor(-2);
    lessThanOrEqualToOperatorToolButton->setXScaling(2.0F);
    lessThanOrEqualToOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(lessThanOrEqualToOperatorToolButton, topEdge + 3, leftEdge + 4, 1, 2);

    QAction* greaterThanOrEqualToOperatorAction = window->action("greater_than_or_equal_to_operator_action");
    EQt::ToolButton* greaterThanOrEqualToOperatorToolButton = new EQt::ToolButton(greaterThanOrEqualToOperatorAction);
    greaterThanOrEqualToOperatorToolButton->setRelativeSizeFactor(-2);
    greaterThanOrEqualToOperatorToolButton->setXScaling(2.0F);
    greaterThanOrEqualToOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(greaterThanOrEqualToOperatorToolButton, topEdge + 3, leftEdge + 6, 1, 2);

    QAction* logicalUnaryNotOperatorAction = window->action("logical_unary_not_operator_action");
    EQt::ToolButton* logicalUnaryNotOperatorToolButton = new EQt::ToolButton(logicalUnaryNotOperatorAction);
    logicalUnaryNotOperatorToolButton->setRelativeSizeFactor(-2);
    logicalUnaryNotOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalUnaryNotOperatorToolButton, topEdge + 3, leftEdge + 2, 1, 1);

    QAction* logicalOverbarNotOperatorAction = window->action("logical_overbar_not_operator_action");
    EQt::ToolButton* logicalOverbarNotOperatorToolButton = new EQt::ToolButton(logicalOverbarNotOperatorAction);
    logicalOverbarNotOperatorToolButton->setRelativeSizeFactor(-2);
    logicalOverbarNotOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalOverbarNotOperatorToolButton, topEdge + 3, leftEdge + 3, 1, 1);

    QAction* logicalAndOperatorAction = window->action("logical_and_operator_action");
    EQt::ToolButton* logicalAndOperatorToolButton = new EQt::ToolButton(logicalAndOperatorAction);
    logicalAndOperatorToolButton->setRelativeSizeFactor(-2);
    logicalAndOperatorToolButton->setXScaling(2.0F);
    logicalAndOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalAndOperatorToolButton, topEdge + 2, leftEdge + 0, 1, 2);

    QAction* logicalOrOperatorAction = window->action("logical_or_operator_action");
    EQt::ToolButton* logicalOrOperatorToolButton = new EQt::ToolButton(logicalOrOperatorAction);
    logicalOrOperatorToolButton->setRelativeSizeFactor(-2);
    logicalOrOperatorToolButton->setXScaling(2.0F);
    logicalOrOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalOrOperatorToolButton, topEdge + 3, leftEdge + 0, 1, 2);
}
