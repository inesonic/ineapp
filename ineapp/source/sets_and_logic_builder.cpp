/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SetsAndLogicBuilder class.
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

#include <ld_set_element.h>
#include <ld_tuple_element.h>
#include <ld_range_2_element.h>
#include <ld_range_3_element.h>
#include <ld_null_set_special_value_element.h>
#include <ld_union_operator_element.h>
#include <ld_intersection_operator_element.h>
#include <ld_disjoint_union_operator_element.h>
#include <ld_cartesian_product_operator_element.h>
#include <ld_set_difference_operator_element.h>
#include <ld_symmetric_difference_operator_element.h>
#include <ld_nary_union_operator_element.h>
#include <ld_nary_disjoint_union_operator_element.h>
#include <ld_element_of_set_operator_element.h>
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
#include "main_window.h"
#include "sets_and_logic_main_window_proxy.h"
#include "sets_and_logic_view_proxy.h"
#include "sets_and_logic_builder.h"

const char SetsAndLogicBuilder::builderName[] = "SetsAndLogicBuilder";
const char SetsAndLogicBuilder::setsAndLogicDockName[] = "sets_and_logic";

SetsAndLogicBuilder::SetsAndLogicBuilder(QObject* parent):EQt::Builder(parent) {}


SetsAndLogicBuilder::~SetsAndLogicBuilder() {}


unsigned SetsAndLogicBuilder::priority() const {
    return builderPriority;
}


const char* SetsAndLogicBuilder::builderIdentifier() const {
    return builderName;
}


const char* SetsAndLogicBuilder::plugInName() const {
    return "";
}

#if (defined(Q_OS_DARWIN))

    #define MENU_TEXT(_text,_suffix) ((_text) + (_suffix))

#else

    #define MENU_TEXT(_text,_suffix) (_text)

#endif

void SetsAndLogicBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* setAction = window->addMenuAction(
        EQt::Application::icon("set"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Set"), tr(" {x}")),
        Ld::SetElement::childProvides,
        "set_action"
    );
    setAction->setToolTip(tr("Insert a set"));
    setAction->setWhatsThis(tr("You can use the set option to insert a set containing zero or more values."));

    QAction* tupleAction = window->addMenuAction(
        EQt::Application::icon("tuple"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Tuple"), tr(" ( (x) )")),
        Ld::TupleElement::childProvides,
        "tuple_action"
    );
    tupleAction->setToolTip(tr("Insert a tuple"));
    tupleAction->setWhatsThis(tr("You can use the tuple option to insert a tuple containing zero or more values."));

    QAction* range2Action = window->addMenuAction(
        EQt::Application::icon("range_2"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | Ran&ge (increment of 1)"), tr(" (?, %1, ?)").arg(QChar(0x2026))),
        Ld::Range2Element::childProvides,
        "range_2_action"
    );
    range2Action->setToolTip(tr("Insert a range with an implied increment of 1"));
    range2Action->setWhatsThis(tr("You can use the range option to insert a range with an implied increment of 1."));

    QAction* range3Action = window->addMenuAction(
        EQt::Application::icon("range_3"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Range"), tr(" (?, ?, %1, ?)").arg(QChar(0x2026))),
        Ld::Range3Element::childProvides,
        "range_3_action"
    );
    range3Action->setToolTip(tr("Insert a range with an explicit increment"));
    range3Action->setWhatsThis(tr("You can use the range option to insert a range with an explicit increment."));

    window->addSeparator(tr("&Insert | &Sets && Logic"));

    QAction* nullSetSpecialValueAction = window->addMenuAction(
        EQt::Application::icon("null_set"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Empty Set"), tr(" (%1)").arg(QChar(0x2205))),
        Ld::NullSetSpecialValueElement::childProvides,
        "null_set_special_value_action"
    );
    nullSetSpecialValueAction->setToolTip(tr("Special value null set, %1").arg(QChar(0x2205)));
    nullSetSpecialValueAction->setWhatsThis(
        tr("You can use this symbol for the null set, %1").arg(QChar(0x2205))
    );

    QAction* logicalTrueSpecialValueAction = window->addMenuAction(
        MENU_TEXT(tr("&Insert | &Sets && Logic | Boolean &True Value"), tr(" (true)")),
        Ld::LogicalTrueSpecialValueElement::childProvides,
        "logical_true_special_value_action"
    );
    logicalTrueSpecialValueAction->setToolTip(tr("Special value \"true\""));
    logicalTrueSpecialValueAction->setWhatsThis(tr("You can use this symbol for the boolean \"true\" value."));

    QAction* logicalFalseSpecialValueAction = window->addMenuAction(
        MENU_TEXT(tr("&Insert | &Sets && Logic | Boolean &False Value"), tr(" (false)")),
        Ld::LogicalFalseSpecialValueElement::childProvides,
        "logical_false_special_value_action"
    );
    logicalFalseSpecialValueAction->setToolTip(tr("Special value \"false\""));
    logicalFalseSpecialValueAction->setWhatsThis(tr("You can use this symbol for the boolean \"false\" value."));

    window->addSeparator(tr("&Insert | &Sets && Logic"));

    QAction* unionOperatorAction = window->addMenuAction(
        EQt::Application::icon("union_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic |  &Set Operators | &Union"), tr(" (%1)").arg(QChar(0x222A))),
        Ld::UnionOperatorElement::childProvides,
        "union_operator_action"
    );
    unionOperatorAction->setToolTip(tr("Set binary union operator."));
    unionOperatorAction->setWhatsThis(
        tr(
            "You can use the union operator to calculate the union of two sets.  The union of two sets is a set "
            "containing every child of each argument set."
        )
    );

    QAction* intersectionOperatorAction = window->addMenuAction(
        EQt::Application::icon("intersection_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic |  &Set Operators | &Intersection"), tr(" (%1)").arg(QChar(0x2229))),
        Ld::IntersectionOperatorElement::childProvides,
        "intersection_operator_action"
    );
    intersectionOperatorAction->setToolTip(tr("Set binary intersection operator."));
    intersectionOperatorAction->setWhatsThis(
        tr(
            "You can use the intersection operator to calculate the intersection of two sets.  The intersection of "
            "two sets is the a set containing only the members found in each child."
        )
    );

    QAction* disjointUnionOperatorAction = window->addMenuAction(
        EQt::Application::icon("disjoint_union_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic |  &Set Operators | &Disjoint Union"), tr(" (%1)").arg(QChar(0x2294))),
        Ld::DisjointUnionOperatorElement::childProvides,
        "disjoint_union_operator_action"
    );
    disjointUnionOperatorAction->setToolTip(tr("Disjoint union operator."));
    disjointUnionOperatorAction->setWhatsThis(
        tr(
            "You can use the disjoint union operator to create a categoried union of two sets.  The resulting set "
            "will contain tuples holding each member from each set along with a value indicating from which set the "
            "member came from."
        )
    );

    QAction* cartesianProductOperatorAction = window->addMenuAction(
        EQt::Application::icon("cartesian_product_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Set Operators | &Cartesian Product"),
            tr(" (%1)").arg(QChar(0x00D7))
        ),
        Ld::CartesianProductOperatorElement::childProvides,
        "cartesian_product_operator_action"
    );
    cartesianProductOperatorAction->setToolTip(tr("Cartesian product operator."));
    cartesianProductOperatorAction->setWhatsThis(
        tr(
            "You can use the cartesian product operator to create a set of tuples containing every combination of "
            "values from the argument sets."
        )
    );

    QAction* setDifferenceOperatorAction = window->addMenuAction(
        EQt::Application::icon("set_difference_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Set Operators | Difference or &Relative Complement"),
            tr(" (%1)").arg(QChar(0x29F5))
        ),
        Ld::SetDifferenceOperatorElement::childProvides,
        "set_difference_operator_action"
    );
    setDifferenceOperatorAction->setToolTip(tr("Set difference operator."));
    setDifferenceOperatorAction->setWhatsThis(
        tr(
            "You can use the set difference operator to calculate the relative complement or difference of a set.  "
            "The relative complement of a set is the set resulting from the members of the second argument being "
            "removed from the fist argument."
        )
    );

    QAction* symmetricDifferenceOperatorAction = window->addMenuAction(
        EQt::Application::icon("symmetric_difference_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Set Operators | &Symmetric Difference"),
            tr(" (%1)").arg(QChar(0x25B3))
        ),
        Ld::SymmetricDifferenceOperatorElement::childProvides,
        "symmetric_difference_operator_action"
    );
    symmetricDifferenceOperatorAction->setToolTip(tr("Set symmetric difference operator."));
    symmetricDifferenceOperatorAction->setWhatsThis(
        tr(
            "You can use the symmetric difference operator to calculate the symmetric difference of two sets.  The "
            "symmetric difference of two sets is the set containing members found in one of the two sets, but not "
            "found in both sets.  The symmetric difference, A %1 B is equivalent to (A %2 B) %3 (A %4 B)"
        ).arg(QChar(0x25B3))
         .arg(QChar(0x222A))
         .arg(QChar(0x29F5))
         .arg(QChar(0x2229))
    );

    QAction* naryUnionOperatorAction = window->addMenuAction(
        EQt::Application::icon("nary_union_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Set Operators | &N-ary Union"),
            tr(" (%1x)").arg(QChar(0x22C3))
        ),
        Ld::NaryUnionOperatorElement::childProvides,
        "nary_union_operator_action"
    );
    naryUnionOperatorAction->setToolTip(tr("N-ary union operator."));
    naryUnionOperatorAction->setWhatsThis(
        tr(
            "You can use the N-ary union operator to create a union from one or more sets contained within a set or "
            "tuple."
        )
    );

    QAction* naryDisjointUnionOperatorAction = window->addMenuAction(
        EQt::Application::icon("nary_disjoint_union_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Set Operators | N-ary Dis&joint Union"),
            tr(" (%1x)").arg(QChar(0x2A06))
        ),
        Ld::NaryDisjointUnionOperatorElement::childProvides,
        "nary_disjoint_union_operator_action"
    );
    naryDisjointUnionOperatorAction->setToolTip(tr("N-ary disjoint union operator."));
    naryDisjointUnionOperatorAction->setWhatsThis(
        tr(
            "You can use the N-ary disjoint union operator to create a disjoint union from one or more sets contained "
            "within a set or tuple.  Each member of each set will be combined with a one-based index in a tuple and "
            "added to the resulting set.  The index indicates which set sourced the element."
        )
    );

    QMenu* relationalOperatorsMenu = window->addMenu(tr("&Insert | &Sets && Logic | &Relational Operators"));

    QAction* elementOfSetAction      = window->action("element_of_set_action");
    relationalOperatorsMenu->addAction(elementOfSetAction);

    QAction* setContainsAsMemberOperatorAction = window->addMenuAction(
        EQt::Application::icon("set_contains_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | Set &Contains As Member"),
            tr(" (%1)").arg(QChar(0x220B))
        ),
        Ld::SetContainsAsMemberOperatorElement::childProvides,
        "set_contains_as_member_operator_action"
    );
    setContainsAsMemberOperatorAction->setToolTip(tr("Set contains as member operator."));
    setContainsAsMemberOperatorAction->setWhatsThis(
        tr(
            "You can use the set contains as member operator to test if a set contains a specified member.  The "
            "operator is identical to the %1 operator except that the parameters are flipped."
        ).arg(QChar(0x2208))
    );

    window->addSeparator(tr("&Insert | &Sets && Logic | &Relational Operators"));

    QAction* notElementOfSetOperatorAction = window->addMenuAction(
        EQt::Application::icon("not_element_of_set_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | &Not Element Of"),
            tr(" (%1)").arg(QChar(0x2209))
        ),
        Ld::NotElementOfSetOperatorElement::childProvides,
        "not_element_of_set_operator_action"
    );
    notElementOfSetOperatorAction->setToolTip(tr("Not element of set operator."));
    notElementOfSetOperatorAction->setWhatsThis(
        tr(
            "You can use the not element of set operator to determine if a set (right parameter) does not contain an "
            "element (first parameter)."
        )
    );

    QAction* notSetContainsAsMemberOperatorAction = window->addMenuAction(
        EQt::Application::icon("not_set_contains_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | Set Does N&ot Contains As Member"),
            tr(" (%1)").arg(QChar(0x220C))
        ),
        Ld::NotSetContainsAsMemberOperatorElement::childProvides,
        "not_set_contains_as_member_operator_action"
    );
    notSetContainsAsMemberOperatorAction->setToolTip(tr("Set contains as member operator."));
    notSetContainsAsMemberOperatorAction->setWhatsThis(
        tr(
            "You can use the set does not contains as member operator to test if a set excludes a specified member.  "
            "The operator is identical to the %1 operator except that the parameters are flipped."
        ).arg(QChar(0x2209))
    );

    window->addSeparator(tr("&Insert | &Sets && Logic | &Relational Operators"));

    QAction* subsetOperatorAction = window->addMenuAction(
        EQt::Application::icon("subset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | &Subset"),
            tr(" (%1)").arg(QChar(0x2286))
        ),
        Ld::SubsetOperatorElement::childProvides,
        "subset_operator_action"
    );
    subsetOperatorAction->setToolTip(tr("Subset operator."));
    subsetOperatorAction->setWhatsThis(
        tr(
            "You can use the subset operator to test if one set is a subset of another set.  The operator will "
            "evaluate to true if all the first parameter's members exist in the second parameter."
        )
    );

    QAction* properSubsetOperatorAction = window->addMenuAction(
        EQt::Application::icon("proper_subset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | &Proper Subset"),
            tr(" (%1)").arg(QChar(0x2282))
        ),
        Ld::ProperSubsetOperatorElement::childProvides,
        "proper_subset_operator_action"
    );
    properSubsetOperatorAction->setToolTip(tr("Proper subset operator."));
    properSubsetOperatorAction->setWhatsThis(
        tr(
            "You can use the proper subset operator to test if one set is a proper subset of another set.  The "
            "operator will evaluate to true if all the first parameter's members exist in the second parameter and "
            "the two parameter sets are not equal."
        )
    );

    QAction* supersetOperatorAction = window->addMenuAction(
        EQt::Application::icon("superset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | S&uperset"),
            tr(" (%1)").arg(QChar(0x2287))
        ),
        Ld::SupersetOperatorElement::childProvides,
        "superset_operator_action"
    );
    supersetOperatorAction->setToolTip(tr("Superset operator."));
    supersetOperatorAction->setWhatsThis(
        tr(
            "You can use the superset operator to test if one set is a superset of another set.  The operator will "
            "evaluate to true if all the second parameter's members exist in the first parameter."
        )
    );

    QAction* properSupersetOperatorAction = window->addMenuAction(
        EQt::Application::icon("proper_superset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | P&roper Superset"),
            tr(" (%1)").arg(QChar(0x2283))
        ),
        Ld::ProperSupersetOperatorElement::childProvides,
        "proper_superset_operator_action"
    );
    properSupersetOperatorAction->setToolTip(tr("Proper superset operator."));
    properSupersetOperatorAction->setWhatsThis(
        tr(
            "You can use the proper superset operator to test if one set is a proper superset of another set.  The "
            "operator will evaluate to true if all the second parameter's members exist in the first parameter and "
            "the sets are not equal."
        )
    );

    window->addSeparator(tr("&Insert | &Sets && Logic | &Relational Operators"));

    QAction* notSubsetOperatorAction = window->addMenuAction(
        EQt::Application::icon("not_subset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | &Not Subset"),
            tr(" (%1)").arg(QChar(0x2288))
        ),
        Ld::NotSubsetOperatorElement::childProvides,
        "not_subset_operator_action"
    );
    notSubsetOperatorAction->setToolTip(tr("Not subset operator."));
    notSubsetOperatorAction->setWhatsThis(
        tr(
            "You can use the not subset operator test if one set is a not a subset of another set.  The operator will "
            "evaluate to false if all the second parameter's members exist in the first parameter."
        )
    );

    QAction* notProperSubsetOperatorAction = window->addMenuAction(
        EQt::Application::icon("not_proper_subset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | N&ot Proper Subset"),
            tr(" (%1)").arg(QChar(0x2284))
        ),
        Ld::NotProperSubsetOperatorElement::childProvides,
        "not_proper_subset_operator_action"
    );
    notProperSubsetOperatorAction->setToolTip(tr("Not proper subset operator."));
    notProperSubsetOperatorAction->setWhatsThis(
        tr(
            "You can use the not proper subset operator test if one set is a not a proper subset of another set.  The "
            "operator will evaluate to false if all the second parameter's members exist in the first parameter and "
            "the sets are not equal."
        )
    );

    QAction* notSupersetOperatorAction = window->addMenuAction(
        EQt::Application::icon("not_superset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | No&t Superset"),
            tr(" (%1)").arg(QChar(0x2289))
        ),
        Ld::NotSupersetOperatorElement::childProvides,
        "not_superset_operator_action"
    );
    notSupersetOperatorAction->setToolTip(tr("Not superset operator."));
    notSupersetOperatorAction->setWhatsThis(
        tr(
            "You can use the not superset operator test if one set is a not a superset of another set.  The operator "
            "will evaluate to false if all the second parameter's members exist in the first parameter."
        )
    );

    QAction* notProperSupersetOperatorAction = window->addMenuAction(
        EQt::Application::icon("not_proper_superset_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic |  &Relational Operators | Not Pro&per Superset"),
            tr(" (%1)").arg(QChar(0x2285))
        ),
        Ld::NotProperSupersetOperatorElement::childProvides,
        "not_proper_superset_operator_action"
    );
    notProperSupersetOperatorAction->setToolTip(tr("Not superset operator."));
    notProperSupersetOperatorAction->setWhatsThis(
        tr(
            "You can use the not proper superset operator test if one set is a not a superset of another set.  The "
            "operator will evaluate to false if all the second parameter's members exist in the first parameter and "
            "the sets are not equal."
        )
    );

    QAction* logicalUnaryNotOperatorAction = window->addMenuAction(
        EQt::Application::icon("logical_unary_not_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Boolean Operators | Unary &Not"), tr(" (%1)").arg(QChar(0x00AC))),
        Ld::LogicalUnaryNotOperatorElement::childProvides,
        "logical_unary_not_operator_action"
    );
    logicalUnaryNotOperatorAction->setToolTip(tr("Unary not operator."));
    logicalUnaryNotOperatorAction->setWhatsThis(
        tr("You can use the logical unary lot operator to negate a boolean value.")
    );

    QAction* logicalOverbarNotOperatorAction = window->addMenuAction(
        EQt::Application::icon("logical_overbar_not_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Boolean Operators | Over&bar Not"), tr(" (?%1)").arg(QChar(0x0305))),
        Ld::LogicalOverbarNotOperatorElement::childProvides,
        "logical_overbar_not_operator_action"
    );
    logicalOverbarNotOperatorAction->setToolTip(tr("Overbar not operator."));
    logicalOverbarNotOperatorAction->setWhatsThis(
        tr("You can use the logical overbar lot operator to negate a boolean value.")
    );

    QAction* logicalAndOperatorAction = window->addMenuAction(
        EQt::Application::icon("logical_and_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Boolean Operators | &And"), tr(" (%1)").arg(QChar(0x2227))),
        Ld::LogicalAndOperatorElement::childProvides,
        "logical_and_operator_action"
    );
    logicalAndOperatorAction->setToolTip(tr("Logical AND operator."));
    logicalAndOperatorAction->setWhatsThis(
        tr("You can use the logical AND operator to report a true value only if both parameters are true.")
    );

    QAction* logicalOrOperatorAction = window->addMenuAction(
        EQt::Application::icon("logical_or_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Boolean Operators | &Or"), tr(" (%1)").arg(QChar(0x2228))),
        Ld::LogicalOrOperatorElement::childProvides,
        "logical_or_operator_action"
    );
    logicalOrOperatorAction->setToolTip(tr("Logical OR operator."));
    logicalOrOperatorAction->setWhatsThis(
        tr("You can use the logical OR operator to report a true value if either parameter is true.")
    );

    QAction* logicalExclusiveOrOperatorAction = window->addMenuAction(
        EQt::Application::icon("logical_exclusive_or_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Boolean Operators | &Exclusive Or"), tr(" (%1)").arg(QChar(0x2A5D))),
        Ld::LogicalExclusiveOrOperatorElement::childProvides,
        "logical_exclusive_or_operator_action"
    );
    logicalExclusiveOrOperatorAction->setToolTip(tr("Logical exclusive OR operator."));
    logicalExclusiveOrOperatorAction->setWhatsThis(
        tr(
           "You can use the logical exclusive OR operator to report a true value if either parameter is true but not "
           "if both parameters are true."
        )
    );

    QAction* logicalConditionalOperatorAction = window->addMenuAction(
        EQt::Application::icon("logical_conditional_operator"),
        MENU_TEXT(tr("&Insert | &Sets && Logic | &Boolean Operators | &Conditional"), tr(" (%1)").arg(QChar(0x2192))),
        Ld::LogicalConditionalOperatorElement::childProvides,
        "logical_conditional_operator_action"
    );
    logicalConditionalOperatorAction->setToolTip(tr("Logical conditional operator."));
    logicalConditionalOperatorAction->setWhatsThis(
        tr(
           "You can use the logical conditional operator to report true if the first parameter is false or the second "
           "parameter is true.  The logical conditional operator employs short-circuit evaluation so the second "
           "parameter will not be evaluated if the first parameter is false.  You can use this feature to "
           "conditionally execute a function or other statement."
        )
    );

    QAction* logicalBiconditionalOperatorAction = window->addMenuAction(
        EQt::Application::icon("logical_biconditional_operator"),
        MENU_TEXT(
            tr("&Insert | &Sets && Logic | &Boolean Operators | &Biconditional"),
            tr(" (%1)").arg(QChar(0x21D4))
        ),
        Ld::LogicalBiconditionalOperatorElement::childProvides,
        "logical_biconditional_operator_action"
    );
    logicalBiconditionalOperatorAction->setToolTip(tr("Logical biconditional operator."));
    logicalBiconditionalOperatorAction->setWhatsThis(
        tr(
           "You can use the logical conditional operator to report true if the boolean value of the first parameter "
           "is the same as the boolean value of the second parameter."
        )
    );
}


void SetsAndLogicBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* logicalTrueSpecialValueButtonAction = new QAction(tr("true"), window);
    window->addAction(
        logicalTrueSpecialValueButtonAction,
        Ld::LogicalTrueSpecialValueElement::childProvides,
        "logical_true_special_value_button_action"
    );
    logicalTrueSpecialValueButtonAction->setToolTip(tr("Logical true special value."));
    logicalTrueSpecialValueButtonAction->setWhatsThis(tr("You can use this symbol for the boolean \"true\" value."));

    QAction* logicalFalseSpecialValueButtonAction = new QAction(tr("false"), window);
    window->addAction(
        logicalFalseSpecialValueButtonAction,
        Ld::LogicalFalseSpecialValueElement::childProvides,
        "logical_false_special_value_button_action"
    );
    logicalFalseSpecialValueButtonAction->setToolTip(tr("Logical false special value."));
    logicalFalseSpecialValueButtonAction->setWhatsThis(tr("You can use this symbol for the boolean \"false\" value."));
}


void SetsAndLogicBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        setsAndLogicDockName,
        tr("Sets/Logic"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER, "basic_math", true)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        setsAndLogicDockName,
        tr("&View | &Docks"),
        "sets_and_logic_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Sets & Logic dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Sets & Logic dock.  You can use the Sets & Logic dock to "
            "add math elements associated with sets and boolean arithmatic."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");

    dock->setHorizontalLayout(horizontalLayout);
    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* SetsAndLogicBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new SetsAndLogicMainWindowProxy(window);
}


void SetsAndLogicBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* setAction = mainWindow->action("set_action");
    bool success = mainWindow->setCommand(tr("set"), setAction);
    Q_ASSERT(success);

    QAction* tupleAction = mainWindow->action("tuple_action");
    success = mainWindow->setCommand(tr("tuple"), tupleAction);
    Q_ASSERT(success);

    QAction* range2Action = mainWindow->action("range_2_action");
    success = mainWindow->setCommand(tr("range1"), range2Action);
    Q_ASSERT(success);

    QAction* range3Action = mainWindow->action("range_3_action");
    success = mainWindow->setCommand(tr("range"), range3Action);
    Q_ASSERT(success);

    QAction* nullSetSpecialValueAction = window->action("null_set_special_value_action");
    success = mainWindow->setCommand(tr("varnothing"), nullSetSpecialValueAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("emptysetconst"), nullSetSpecialValueAction);
    Q_ASSERT(success);

    QAction* unionOperatorAction = window->action("union_operator_action");
    success = mainWindow->setCommand(tr("cup"), unionOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("union"), unionOperatorAction);
    Q_ASSERT(success);

    QAction* intersectionOperatorAction = window->action("intersection_operator_action");
    success = mainWindow->setCommand(tr("cap"), intersectionOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("intersection"), intersectionOperatorAction);
    Q_ASSERT(success);

    QAction* disjointUnionOperatorAction = window->action("disjoint_union_operator_action");
    success = mainWindow->setCommand(tr("sqcup"), disjointUnionOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("dju"), disjointUnionOperatorAction);
    Q_ASSERT(success);

    QAction* cartesianProductOperatorAction = window->action("cartesian_product_operator_action");
    success = mainWindow->setCommand(tr("cprod"), cartesianProductOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("times"), cartesianProductOperatorAction);
    Q_ASSERT(success);

    QAction* setDifferenceOperatorAction = window->action("set_difference_operator_action");
    success = mainWindow->setCommand(tr("setminus"), setDifferenceOperatorAction);
    Q_ASSERT(success);

    QAction* symmetricDifferenceOperatorAction = window->action("symmetric_difference_operator_action");
    success = mainWindow->setCommand(tr("triangle"), symmetricDifferenceOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("symdiff"), symmetricDifferenceOperatorAction);
    Q_ASSERT(success);

    QAction* naryUnionOperatorAction = window->action("nary_union_operator_action");
    success = mainWindow->setCommand(tr("bigcup"), naryUnionOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("naryunion"), naryUnionOperatorAction);
    Q_ASSERT(success);

    QAction* naryDisjointUnionOperatorAction = window->action("nary_disjoint_union_operator_action");
    success = mainWindow->setCommand(tr("bigcap"), naryDisjointUnionOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("narydju"), naryDisjointUnionOperatorAction);
    Q_ASSERT(success);

    QAction* notElementOfSetOperatorAction = window->action("not_element_of_set_operator_action");
    success = mainWindow->setCommand(tr("notin"), notElementOfSetOperatorAction);
    Q_ASSERT(success);

    QAction* setContainsAsMemberOperatorAction = window->action("set_contains_as_member_operator_action");
    success = mainWindow->setCommand(tr("ni"), setContainsAsMemberOperatorAction);
    Q_ASSERT(success);

    QAction* notSetContainsAsMemberOperatorAction = window->action("not_set_contains_as_member_operator_action");
    success = mainWindow->setCommand(tr("notni"), notSetContainsAsMemberOperatorAction);
    Q_ASSERT(success);

    QAction* subsetOperatorAction = window->action("subset_operator_action");
    success = mainWindow->setCommand(tr("subseteq"), subsetOperatorAction);
    Q_ASSERT(success);

    QAction* properSubsetOperatorAction = window->action("proper_subset_operator_action");
    success = mainWindow->setCommand(tr("subset"), properSubsetOperatorAction);
    Q_ASSERT(success);

    QAction* supersetOperatorAction = window->action("superset_operator_action");
    success = mainWindow->setCommand(tr("supseteq"), supersetOperatorAction);
    Q_ASSERT(success);

    QAction* properSupersetOperatorAction = window->action("proper_superset_operator_action");
    success = mainWindow->setCommand(tr("supset"), properSupersetOperatorAction);
    Q_ASSERT(success);

    QAction* notSubsetOperatorAction = window->action("not_subset_operator_action");
    success = mainWindow->setCommand(tr("nsubseteq"), notSubsetOperatorAction);
    Q_ASSERT(success);

    QAction* notProperSubsetOperatorAction = window->action("not_proper_subset_operator_action");
    success = mainWindow->setCommand(tr("nsubset"), notProperSubsetOperatorAction);
    Q_ASSERT(success);

    QAction* notSupersetOperatorAction = window->action("not_superset_operator_action");
    success = mainWindow->setCommand(tr("nsupseteq"), notSupersetOperatorAction);
    Q_ASSERT(success);

    QAction* notProperSupersetOperatorAction = window->action("not_proper_superset_operator_action");
    success = mainWindow->setCommand(tr("nsupset"), notProperSupersetOperatorAction);
    Q_ASSERT(success);

    QAction* logicalUnaryNotOperatorAction = window->action("logical_unary_not_operator_action");
    success = mainWindow->setCommand(tr("neg"), logicalUnaryNotOperatorAction);
    Q_ASSERT(success);

    QAction* logicalOverbarNotOperatorAction = window->action("logical_overbar_not_operator_action");
    success = mainWindow->setCommand(tr("overbar"), logicalOverbarNotOperatorAction);
    Q_ASSERT(success);

    QAction* logicalAndOperatorAction = window->action("logical_and_operator_action");
    success = mainWindow->setCommand(tr("land"), logicalAndOperatorAction);
    Q_ASSERT(success);

    QAction* logicalOrOperatorAction = window->action("logical_or_operator_action");
    success = mainWindow->setCommand(tr("lor"), logicalOrOperatorAction);
    Q_ASSERT(success);

    QAction* logicalExclusiveOrOperatorAction = window->action("logical_exclusive_or_operator_action");
    success = mainWindow->setCommand(tr("veebar"), logicalExclusiveOrOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("lxor"), logicalExclusiveOrOperatorAction);
    Q_ASSERT(success);

    QAction* logicalConditionalOperatorAction = window->action("logical_conditional_operator_action");
    success = mainWindow->setCommand(tr("lcond"), logicalConditionalOperatorAction);
    Q_ASSERT(success);

    QAction* logicalBiconditionalOperatorAction = window->action("logical_biconditional_operator_action");
    success = mainWindow->setCommand(tr("lbicond"), logicalBiconditionalOperatorAction);
    Q_ASSERT(success);

    QAction* logicalTrueSpecialValueAction = window->action("logical_true_special_value_action");
    success = mainWindow->setCommand(tr("true"), logicalTrueSpecialValueAction);
    Q_ASSERT(success);

    QAction* logicalFalseSpecialValueAction = window->action("logical_false_special_value_action");
    success = mainWindow->setCommand(tr("false"), logicalFalseSpecialValueAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* SetsAndLogicBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new SetsAndLogicViewProxy(view, window);
}


void SetsAndLogicBuilder::buildDockHorizontalOrientation(
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


void SetsAndLogicBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("vertical_buttons");
    verticalLayout->addLayout(buttonsLayout);

    buildButtonLayout1(window, buttonsLayout, 0, 0);
    buildButtonLayout2(window, buttonsLayout, 4, 0);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(buttonsLayout, 5);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void SetsAndLogicBuilder::buildButtonLayout1(
        EQt::ProgrammaticMainWindow* window,
        QGridLayout*                 buttonsLayout,
        unsigned                     topEdge,
        unsigned                     leftEdge
    ) {
    QAction* setAction = window->action("set_action");
    EQt::ToolButton* setToolButton = new EQt::ToolButton(setAction);
    setToolButton->setRelativeSizeFactor(-2);
    setToolButton->setXScaling(2.0F);
    setToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(setToolButton, topEdge + 0, leftEdge + 0, 1, 2);

    QAction* range2Action = window->action("range_2_action");
    EQt::ToolButton* range2ToolButton = new EQt::ToolButton(range2Action);
    range2ToolButton->setRelativeSizeFactor(-2);
    range2ToolButton->setXScaling(2.0F);
    range2ToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(range2ToolButton, topEdge + 0, leftEdge + 2, 1, 2);

    QAction* range3Action = window->action("range_3_action");
    EQt::ToolButton* range3ToolButton = new EQt::ToolButton(range3Action);
    range3ToolButton->setRelativeSizeFactor(-2);
    range3ToolButton->setXScaling(3.0F);
    range3ToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(range3ToolButton, topEdge + 0, leftEdge + 4, 1, 3);

    QAction* nullSetSpecialValueAction = window->action("null_set_special_value_action");
    EQt::ToolButton* nullSetSpecialValueToolButton = new EQt::ToolButton(nullSetSpecialValueAction);
    nullSetSpecialValueToolButton->setRelativeSizeFactor(-2);
    nullSetSpecialValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(nullSetSpecialValueToolButton, topEdge + 0, leftEdge + 7, 1, 1);

    QAction* tupleAction = window->action("tuple_action");
    EQt::ToolButton* tupleToolButton = new EQt::ToolButton(tupleAction);
    tupleToolButton->setRelativeSizeFactor(-2);
    tupleToolButton->setXScaling(2.0F);
    tupleToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(tupleToolButton, topEdge + 1, leftEdge + 0, 1, 2);

    QAction* intersectionOperatorAction = window->action("intersection_operator_action");
    EQt::ToolButton* intersectionOperatorToolButton = new EQt::ToolButton(intersectionOperatorAction);
    intersectionOperatorToolButton->setRelativeSizeFactor(-2);
    intersectionOperatorToolButton->setXScaling(2.0F);
    intersectionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(intersectionOperatorToolButton, topEdge + 1, leftEdge + 2, 1, 2);

    QAction* unionOperatorAction = window->action("union_operator_action");
    EQt::ToolButton* unionOperatorToolButton = new EQt::ToolButton(unionOperatorAction);
    unionOperatorToolButton->setRelativeSizeFactor(-2);
    unionOperatorToolButton->setXScaling(2.0F);
    unionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(unionOperatorToolButton, topEdge + 1, leftEdge + 4, 1, 2);

    QAction* disjointUnionOperatorAction = window->action("disjoint_union_operator_action");
    EQt::ToolButton* disjointUnionOperatorToolButton = new EQt::ToolButton(disjointUnionOperatorAction);
    disjointUnionOperatorToolButton->setRelativeSizeFactor(-2);
    disjointUnionOperatorToolButton->setXScaling(2.0F);
    disjointUnionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(disjointUnionOperatorToolButton, topEdge + 1, leftEdge + 6, 1, 2);

    QAction* cartesianProductOperatorAction = window->action("cartesian_product_operator_action");
    EQt::ToolButton* cartesianProductOperatorToolButton = new EQt::ToolButton(cartesianProductOperatorAction);
    cartesianProductOperatorToolButton->setRelativeSizeFactor(-2);
    cartesianProductOperatorToolButton->setXScaling(2.0F);
    cartesianProductOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(cartesianProductOperatorToolButton, topEdge + 2, leftEdge + 6, 1, 2);

    QAction* setDifferenceOperatorAction = window->action("set_difference_operator_action");
    EQt::ToolButton* setDifferenceOperatorToolButton = new EQt::ToolButton(setDifferenceOperatorAction);
    setDifferenceOperatorToolButton->setRelativeSizeFactor(-2);
    setDifferenceOperatorToolButton->setXScaling(2.0F);
    setDifferenceOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(setDifferenceOperatorToolButton, topEdge + 2, leftEdge + 0, 1, 2);

    QAction* symmetricDifferenceOperatorAction = window->action("symmetric_difference_operator_action");
    EQt::ToolButton* symmetricDifferenceOperatorToolButton = new EQt::ToolButton(symmetricDifferenceOperatorAction);
    symmetricDifferenceOperatorToolButton->setRelativeSizeFactor(-2);
    symmetricDifferenceOperatorToolButton->setXScaling(2.0F);
    symmetricDifferenceOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(symmetricDifferenceOperatorToolButton, topEdge + 2, leftEdge + 2, 1, 2);

    QAction* naryUnionOperatorAction = window->action("nary_union_operator_action");
    EQt::ToolButton* naryUnionOperatorToolButton = new EQt::ToolButton(naryUnionOperatorAction);
    naryUnionOperatorToolButton->setRelativeSizeFactor(-2);
    naryUnionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(naryUnionOperatorToolButton, topEdge + 2, leftEdge + 4, 1, 1);

    QAction* naryDisjointUnionOperatorAction = window->action("nary_disjoint_union_operator_action");
    EQt::ToolButton* naryDisjointUnionOperatorToolButton = new EQt::ToolButton(naryDisjointUnionOperatorAction);
    naryDisjointUnionOperatorToolButton->setRelativeSizeFactor(-2);
    naryDisjointUnionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(naryDisjointUnionOperatorToolButton, topEdge + 2, leftEdge + 5, 1, 1);

    QAction* elementOfSetOperatorAction = window->action("element_of_set_action");
    EQt::ToolButton* elementOfSetOperatorToolButton = new EQt::ToolButton(elementOfSetOperatorAction);
    elementOfSetOperatorToolButton->setRelativeSizeFactor(-2);
    elementOfSetOperatorToolButton->setXScaling(2.0F);
    elementOfSetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(elementOfSetOperatorToolButton, topEdge + 3, leftEdge + 0, 1, 2);

    QAction* notElementOfSetOperatorAction = window->action("not_element_of_set_operator_action");
    EQt::ToolButton* notElementOfSetOperatorToolButton = new EQt::ToolButton(notElementOfSetOperatorAction);
    notElementOfSetOperatorToolButton->setRelativeSizeFactor(-2);
    notElementOfSetOperatorToolButton->setXScaling(2.0F);
    notElementOfSetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(notElementOfSetOperatorToolButton, topEdge + 3, leftEdge + 2, 1, 2);

    QAction* setContainsAsMemberOperatorAction = window->action("set_contains_as_member_operator_action");
    EQt::ToolButton* setContainsAsMemberOperatorToolButton = new EQt::ToolButton(setContainsAsMemberOperatorAction);
    setContainsAsMemberOperatorToolButton->setRelativeSizeFactor(-2);
    setContainsAsMemberOperatorToolButton->setXScaling(2.0F);
    setContainsAsMemberOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(setContainsAsMemberOperatorToolButton, topEdge + 3, leftEdge + 4, 1, 2);

    QAction* notSetContainsAsMemberOperatorAction = window->action("not_set_contains_as_member_operator_action");
    EQt::ToolButton* notSetContainsAsMemberToolButton = new EQt::ToolButton(notSetContainsAsMemberOperatorAction);
    notSetContainsAsMemberToolButton->setRelativeSizeFactor(-2);
    notSetContainsAsMemberToolButton->setXScaling(2.0F);
    notSetContainsAsMemberToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(notSetContainsAsMemberToolButton, topEdge + 3, leftEdge + 6, 1, 2);
}


void SetsAndLogicBuilder::buildButtonLayout2(
        EQt::ProgrammaticMainWindow* window,
        QGridLayout*                 buttonsLayout,
        unsigned                     topEdge,
        unsigned                     leftEdge
    ) {
    QAction* subsetOperatorAction = window->action("subset_operator_action");
    EQt::ToolButton* subsetOperatorToolButton = new EQt::ToolButton(subsetOperatorAction);
    subsetOperatorToolButton->setRelativeSizeFactor(-2);
    subsetOperatorToolButton->setXScaling(2.0F);
    subsetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(subsetOperatorToolButton, topEdge + 0, leftEdge + 0, 1, 2);

    QAction* properSubsetOperatorAction = window->action("proper_subset_operator_action");
    EQt::ToolButton* properSubsetOperatorToolButton = new EQt::ToolButton(properSubsetOperatorAction);
    properSubsetOperatorToolButton->setRelativeSizeFactor(-2);
    properSubsetOperatorToolButton->setXScaling(2.0F);
    properSubsetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(properSubsetOperatorToolButton, topEdge + 0, leftEdge + 2, 1, 2);

    QAction* supersetOperatorAction = window->action("superset_operator_action");
    EQt::ToolButton* supersetOperatorToolButton = new EQt::ToolButton(supersetOperatorAction);
    supersetOperatorToolButton->setRelativeSizeFactor(-2);
    supersetOperatorToolButton->setXScaling(2.0F);
    supersetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(supersetOperatorToolButton, topEdge + 0, leftEdge + 4, 1, 2);

    QAction* properSupersetOperatorAction = window->action("proper_superset_operator_action");
    EQt::ToolButton* properSupersetOperatorToolButton = new EQt::ToolButton(properSupersetOperatorAction);
    properSupersetOperatorToolButton->setRelativeSizeFactor(-2);
    properSupersetOperatorToolButton->setXScaling(2.0F);
    properSupersetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(properSupersetOperatorToolButton, topEdge + 0, leftEdge + 6, 1, 2);

    QAction* notSubsetOperatorAction = window->action("not_subset_operator_action");
    EQt::ToolButton* notSubsetOperatorToolButton = new EQt::ToolButton(notSubsetOperatorAction);
    notSubsetOperatorToolButton->setRelativeSizeFactor(-2);
    notSubsetOperatorToolButton->setXScaling(2.0F);
    notSubsetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(notSubsetOperatorToolButton, topEdge + 1, leftEdge + 0, 1, 2);

    QAction* notProperSubsetOperatorAction = window->action("not_proper_subset_operator_action");
    EQt::ToolButton* notProperSubsetOperatorToolButton = new EQt::ToolButton(notProperSubsetOperatorAction);
    notProperSubsetOperatorToolButton->setRelativeSizeFactor(-2);
    notProperSubsetOperatorToolButton->setXScaling(2.0F);
    notProperSubsetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(notProperSubsetOperatorToolButton, topEdge + 1, leftEdge + 2, 1, 2);

    QAction* notSupersetOperatorAction = window->action("not_superset_operator_action");
    EQt::ToolButton* notSupersetOperatorToolButton = new EQt::ToolButton(notSupersetOperatorAction);
    notSupersetOperatorToolButton->setRelativeSizeFactor(-2);
    notSupersetOperatorToolButton->setXScaling(2.0F);
    notSupersetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(notSupersetOperatorToolButton, topEdge + 1, leftEdge + 4, 1, 2);

    QAction* notProperSupersetOperatorAction = window->action("not_proper_superset_operator_action");
    EQt::ToolButton* notProperSupersetOperatorToolButton = new EQt::ToolButton(notProperSupersetOperatorAction);
    notProperSupersetOperatorToolButton->setRelativeSizeFactor(-2);
    notProperSupersetOperatorToolButton->setXScaling(2.0F);
    notProperSupersetOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(notProperSupersetOperatorToolButton, topEdge + 1, leftEdge + 6, 1, 2);

    QAction* logicalUnaryNotOperatorAction = window->action("logical_unary_not_operator_action");
    EQt::ToolButton* logicalUnaryNotOperatorToolButton = new EQt::ToolButton(logicalUnaryNotOperatorAction);
    logicalUnaryNotOperatorToolButton->setRelativeSizeFactor(-2);
    logicalUnaryNotOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalUnaryNotOperatorToolButton, topEdge + 2, leftEdge + 0, 1, 1);

    QAction* logicalOverbarNotOperatorAction = window->action("logical_overbar_not_operator_action");
    EQt::ToolButton* logicalOverbarNotOperatorToolButton = new EQt::ToolButton(logicalOverbarNotOperatorAction);
    logicalOverbarNotOperatorToolButton->setRelativeSizeFactor(-2);
    logicalOverbarNotOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalOverbarNotOperatorToolButton, topEdge + 2, leftEdge + 1, 1, 1);

    QAction* logicalAndOperatorAction = window->action("logical_and_operator_action");
    EQt::ToolButton* logicalAndOperatorToolButton = new EQt::ToolButton(logicalAndOperatorAction);
    logicalAndOperatorToolButton->setRelativeSizeFactor(-2);
    logicalAndOperatorToolButton->setXScaling(2.0F);
    logicalAndOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalAndOperatorToolButton, topEdge + 2, leftEdge + 2, 1, 2);

    QAction* logicalOrOperatorAction = window->action("logical_or_operator_action");
    EQt::ToolButton* logicalOrOperatorToolButton = new EQt::ToolButton(logicalOrOperatorAction);
    logicalOrOperatorToolButton->setRelativeSizeFactor(-2);
    logicalOrOperatorToolButton->setXScaling(2.0F);
    logicalOrOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalOrOperatorToolButton, topEdge + 2, leftEdge + 4, 1, 2);

    QAction* logicalExclusiveOrOperatorAction = window->action("logical_exclusive_or_operator_action");
    EQt::ToolButton* logicalExclusiveOrOperatorToolButton = new EQt::ToolButton(logicalExclusiveOrOperatorAction);
    logicalExclusiveOrOperatorToolButton->setRelativeSizeFactor(-2);
    logicalExclusiveOrOperatorToolButton->setXScaling(2.0F);
    logicalExclusiveOrOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalExclusiveOrOperatorToolButton, topEdge + 2, leftEdge + 6, 1, 2);

    QAction* logicalConditionalOperatorAction = window->action("logical_conditional_operator_action");
    EQt::ToolButton* logicalConditionalOperatorToolButton = new EQt::ToolButton(logicalConditionalOperatorAction);
    logicalConditionalOperatorToolButton->setRelativeSizeFactor(-2);
    logicalConditionalOperatorToolButton->setXScaling(2.0F);
    logicalConditionalOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalConditionalOperatorToolButton, topEdge + 3, leftEdge + 0, 1, 2);

    QAction* logicalBiconditionalOperatorAction = window->action("logical_biconditional_operator_action");
    EQt::ToolButton* logicalBiconditionalOperatorToolButton = new EQt::ToolButton(logicalBiconditionalOperatorAction);
    logicalBiconditionalOperatorToolButton->setRelativeSizeFactor(-2);
    logicalBiconditionalOperatorToolButton->setXScaling(2.0F);
    logicalBiconditionalOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalBiconditionalOperatorToolButton, topEdge + 3, leftEdge + 2, 1, 2);

    QFont specialValueFont = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    specialValueFont.setItalic(true);

    QAction* logicalTrueSpecialValueAction = window->action("logical_true_special_value_button_action");
    EQt::ToolButton* logicalTrueSpecialValueToolButton = new EQt::ToolButton(logicalTrueSpecialValueAction);
    logicalTrueSpecialValueToolButton->setFont(specialValueFont);
    logicalTrueSpecialValueToolButton->setRelativeSizeFactor(-2);
    logicalTrueSpecialValueToolButton->setXScaling(2.0F);
    logicalTrueSpecialValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalTrueSpecialValueToolButton, topEdge + 3, leftEdge + 4, 1, 2);

    QAction* logicalFalseSpecialValueAction = window->action("logical_false_special_value_button_action");
    EQt::ToolButton* logicalFalseSpecialValueToolButton = new EQt::ToolButton(logicalFalseSpecialValueAction);
    logicalFalseSpecialValueToolButton->setFont(specialValueFont);
    logicalFalseSpecialValueToolButton->setRelativeSizeFactor(-2);
    logicalFalseSpecialValueToolButton->setXScaling(2.0F);
    logicalFalseSpecialValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    buttonsLayout->addWidget(logicalFalseSpecialValueToolButton, topEdge + 3, leftEdge + 6, 1, 2);
}
