/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref configure function.
***********************************************************************************************************************/

#include <cstdint>

#include <model_api_types.h>

#include <ld_configure.h>
#include <ld_visual.h>
#include <ld_document_setting.h>
#include <ld_data_type.h>
#include <ld_cpp_code_generator.h>
#include <ld_environment.h>
#include <ld_root_element.h>
#include <ld_paragraph_element.h>
#include <ld_image_element.h>
#include <ld_page_break_element.h>
#include <ld_text_element.h>
#include <ld_table_frame_element.h>
#include <ld_value_field_element.h>
#include <ld_placeholder_element.h>
#include <ld_list_placeholder_element.h>
#include <ld_function_placeholder_element.h>
#include <ld_assignment_operator_element.h>
#include <ld_variable_element.h>
#include <ld_literal_element.h>
#include <ld_set_element.h>
#include <ld_tuple_element.h>
#include <ld_range_2_element.h>
#include <ld_range_3_element.h>
#include <ld_pi_special_value_element.h>
#include <ld_eulers_number_special_value_element.h>
#include <ld_epsilon_special_value_element.h>
#include <ld_infinity_special_value_element.h>
#include <ld_null_set_special_value_element.h>
#include <ld_element_of_set_operator_element.h>
#include <ld_boolean_type_element.h>
#include <ld_integer_type_element.h>
#include <ld_real_type_element.h>
#include <ld_complex_type_element.h>
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
#include <ld_subscript_index_operator_element.h>
#include <ld_subscript_row_column_operator_element.h>
#include <ld_matrix_operator_element.h>
#include <ld_matrix_combine_left_to_right_operator_element.h>
#include <ld_matrix_combine_top_to_bottom_operator_element.h>
#include <ld_hadamard_product_operator_element.h>
#include <ld_kronecker_product_operator_element.h>
#include <ld_matrix_transpose_operator_element.h>
#include <ld_matrix_conjugate_transpose_operator_element.h>
#include <ld_for_all_in_operator_element.h>
#include <ld_while_operator_element.h>
#include <ld_compound_statement_operator_element.h>
#include <ld_if_operator_element.h>
#include <ld_else_if_operator_element.h>
#include <ld_else_operator_element.h>
#include <ld_brace_conditional_operator_element.h>
#include <ld_therefore_operator_element.h>
#include <ld_function_element.h>
#include <ld_plot_element.h>

#include <ld_code_generator.h>
#include <ld_html_code_generator.h>
#include <ld_latex_code_generator.h>

#include "registrar.h"
#include "application.h"

#include "cursor_position_setting.h"
#include "zoom_setting.h"
#include "cpp_code_generator_visual.h"

#include "fixer.h"
#include "root_element_fixer.h"
#include "text_element_fixer.h"
#include "paragraph_element_fixer.h"
#include "table_frame_element_fixer.h"
#include "placeholder_element_fixer.h"
#include "variable_element_fixer.h"
#include "literal_element_fixer.h"
#include "operator_fixer.h"
#include "list_fixer.h"
#include "grid_operator_fixer.h"
#include "function_fixer.h"

#include "selection_data.h"
#include "html_selection_data_translator.h"
#include "latex_selection_data_translator.h"

#include "boolean_data_type_presentation_generator.h"
#include "integer_data_type_presentation_generator.h"
#include "real_data_type_presentation_generator.h"
#include "complex_data_type_presentation_generator.h"
#include "set_data_type_presentation_generator.h"
#include "tuple_data_type_presentation_generator.h"
#include "matrix_boolean_data_type_presentation_generator.h"
#include "matrix_integer_data_type_presentation_generator.h"
#include "matrix_real_data_type_presentation_generator.h"
#include "matrix_complex_data_type_presentation_generator.h"

#include "inspector_widget.h"
#include "boolean_inspector_widget.h"
#include "integer_inspector_widget.h"
#include "real_inspector_widget.h"
#include "complex_inspector_widget.h"
#include "matrix_boolean_inspector_widget.h"
#include "matrix_integer_inspector_widget.h"
#include "matrix_real_inspector_widget.h"
#include "matrix_complex_inspector_widget.h"
#include "set_inspector_widget.h"
#include "tuple_inspector_widget.h"

#include "home_builder_initial.h"
#include "home_builder_final.h"
#include "run_export_builder.h"
#include "page_format_builder.h"
#include "block_format_builder_early.h"
#include "block_format_builder_mid.h"
#include "block_format_builder_late.h"
#include "character_format_builder_early.h"
#include "character_format_builder_late.h"
#include "page_break_builder.h"
#include "image_builder.h"
#include "operator_format_builder.h"
#include "special_characters_builder.h"
#include "table_format_builder.h"
#include "value_field_builder.h"
#include "basic_math_builder.h"
#include "sets_and_logic_builder.h"
#include "matrix_builder.h"
#include "control_flow_builder.h"
#include "function_builder.h"
#include "plot_builder.h"

#include "root_presentation.h"
#include "document.h"
#include "paragraph_presentation.h"
#include "text_presentation.h"
#include "image_presentation.h"
#include "page_break_presentation.h"
#include "table_frame_presentation.h"
#include "value_field_presentation.h"
#include "placeholder_presentation.h"
#include "list_placeholder_presentation.h"
#include "function_placeholder_presentation.h"
#include "assignment_operator_presentation.h"
#include "variable_presentation.h"
#include "literal_presentation.h"
#include "set_presentation.h"
#include "tuple_presentation.h"
#include "range_2_presentation.h"
#include "range_3_presentation.h"
#include "pi_special_value_presentation.h"
#include "eulers_number_special_value_presentation.h"
#include "epsilon_special_value_presentation.h"
#include "infinity_special_value_presentation.h"
#include "null_set_special_value_presentation.h"
#include "element_of_set_operator_presentation.h"
#include "boolean_type_presentation.h"
#include "integer_type_presentation.h"
#include "real_type_presentation.h"
#include "complex_type_presentation.h"
#include "addition_operator_presentation.h"
#include "subtraction_operator_presentation.h"
#include "multiplication_operator_presentation.h"
#include "division_operator_presentation.h"
#include "fraction_operator_presentation.h"
#include "unary_plus_operator_presentation.h"
#include "unary_minus_operator_presentation.h"
#include "grouping_parenthesis_operator_presentation.h"
#include "grouping_brackets_operator_presentation.h"
#include "grouping_braces_operator_presentation.h"
#include "power_operator_presentation.h"
#include "square_root_operator_presentation.h"
#include "root_operator_presentation.h"
#include "complex_conjugate_operator_presentation.h"
#include "factorial_operator_presentation.h"
#include "absolute_value_operator_presentation.h"
#include "floor_operator_presentation.h"
#include "ceiling_operator_presentation.h"
#include "nearest_integer_operator_presentation.h"
#include "not_equal_to_operator_presentation.h"
#include "less_than_operator_presentation.h"
#include "greater_than_operator_presentation.h"
#include "less_than_or_equal_to_operator_presentation.h"
#include "greater_than_or_equal_to_operator_presentation.h"
#include "union_operator_presentation.h"
#include "intersection_operator_presentation.h"
#include "disjoint_union_operator_presentation.h"
#include "cartesian_product_operator_presentation.h"
#include "set_difference_operator_presentation.h"
#include "symmetric_difference_operator_presentation.h"
#include "nary_union_operator_presentation.h"
#include "nary_disjoint_union_operator_presentation.h"
#include "not_element_of_set_operator_presentation.h"
#include "set_contains_as_member_operator_presentation.h"
#include "not_set_contains_as_member_operator_presentation.h"
#include "subset_operator_presentation.h"
#include "proper_subset_operator_presentation.h"
#include "superset_operator_presentation.h"
#include "proper_superset_operator_presentation.h"
#include "not_subset_operator_presentation.h"
#include "not_proper_subset_operator_presentation.h"
#include "not_superset_operator_presentation.h"
#include "not_proper_superset_operator_presentation.h"
#include "logical_unary_not_operator_presentation.h"
#include "logical_overbar_not_operator_presentation.h"
#include "logical_and_operator_presentation.h"
#include "logical_or_operator_presentation.h"
#include "logical_exclusive_or_operator_presentation.h"
#include "logical_conditional_operator_presentation.h"
#include "logical_biconditional_operator_presentation.h"
#include "logical_true_special_value_presentation.h"
#include "logical_false_special_value_presentation.h"
#include "subscript_index_operator_presentation.h"
#include "subscript_row_column_operator_presentation.h"
#include "matrix_operator_presentation.h"
#include "matrix_combine_left_to_right_operator_presentation.h"
#include "matrix_combine_top_to_bottom_operator_presentation.h"
#include "hadamard_product_operator_presentation.h"
#include "kronecker_product_operator_presentation.h"
#include "matrix_transpose_operator_presentation.h"
#include "matrix_conjugate_transpose_operator_presentation.h"
#include "for_all_in_operator_presentation.h"
#include "while_operator_presentation.h"
#include "compound_statement_operator_presentation.h"
#include "if_operator_presentation.h"
#include "else_if_operator_presentation.h"
#include "else_operator_presentation.h"
#include "brace_conditional_operator_presentation.h"
#include "therefore_operator_presentation.h"
#include "function_presentation.h"
#include "plot_presentation.h"

#include "plot_engine.h"
#include "xy_plot_engine.h"
#include "polar_2d_plot_engine.h"
#include "bar_chart_engine.h"
#include "histogram_engine.h"
#include "pie_chart_engine.h"
#include "donut_chart_engine.h"
#include "heat_chart_engine.h"
#include "rgb_image_engine.h"
#include "cmyk_image_engine.h"
#include "grayscale_image_engine.h"

void configure(Registrar* registrar) {

    Ld::Visual::registerCreator(Ld::RootElement::elementName, Document::creator);
    Ld::Visual::registerCreator(Ld::ParagraphElement::elementName, ParagraphPresentation::creator);
    Ld::Visual::registerCreator(Ld::TextElement::elementName, TextPresentation::creator);
    Ld::Visual::registerCreator(Ld::ImageElement::elementName, ImagePresentation::creator);
    Ld::Visual::registerCreator(Ld::PageBreakElement::elementName, PageBreakPresentation::creator);
    Ld::Visual::registerCreator(Ld::TableFrameElement::elementName, TableFramePresentation::creator);
    Ld::Visual::registerCreator(Ld::ValueFieldElement::elementName, ValueFieldPresentation::creator);
    Ld::Visual::registerCreator(Ld::PlaceholderElement::elementName, PlaceholderPresentation::creator);
    Ld::Visual::registerCreator(Ld::ListPlaceholderElement::elementName, ListPlaceholderPresentation::creator);
    Ld::Visual::registerCreator(Ld::FunctionPlaceholderElement::elementName, FunctionPlaceholderPresentation::creator);
    Ld::Visual::registerCreator(Ld::AssignmentOperatorElement::elementName, AssignmentOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::VariableElement::elementName, VariablePresentation::creator);
    Ld::Visual::registerCreator(Ld::LiteralElement::elementName, LiteralPresentation::creator);
    Ld::Visual::registerCreator(Ld::SetElement::elementName, SetPresentation::creator);
    Ld::Visual::registerCreator(Ld::TupleElement::elementName, TuplePresentation::creator);
    Ld::Visual::registerCreator(Ld::Range2Element::elementName, Range2Presentation::creator);
    Ld::Visual::registerCreator(Ld::Range3Element::elementName, Range3Presentation::creator);
    Ld::Visual::registerCreator(Ld::PiSpecialValueElement::elementName, PiSpecialValuePresentation::creator);
    Ld::Visual::registerCreator(
        Ld::EulersNumberSpecialValueElement::elementName,
        EulersNumberSpecialValuePresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::EpsilonSpecialValueElement::elementName,
        EpsilonSpecialValuePresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::InfinitySpecialValueElement::elementName,
        InfinitySpecialValuePresentation::creator
    );
    Ld::Visual::registerCreator(Ld::NullSetSpecialValueElement::elementName, NullSetSpecialValuePresentation::creator);
    Ld::Visual::registerCreator(
        Ld::ElementOfSetOperatorElement::elementName,
        ElementOfSetOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::BooleanTypeElement::elementName, BooleanTypePresentation::creator);
    Ld::Visual::registerCreator(Ld::IntegerTypeElement::elementName, IntegerTypePresentation::creator);
    Ld::Visual::registerCreator(Ld::RealTypeElement::elementName, RealTypePresentation::creator);
    Ld::Visual::registerCreator(Ld::ComplexTypeElement::elementName, ComplexTypePresentation::creator);
    Ld::Visual::registerCreator(Ld::AdditionOperatorElement::elementName, AdditionOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::SubtractionOperatorElement::elementName, SubtractionOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::MultiplicationOperatorElement::elementName,
        MultiplicationOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::DivisionOperatorElement::elementName, DivisionOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::FractionOperatorElement::elementName, FractionOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::UnaryPlusOperatorElement::elementName, UnaryPlusOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::UnaryMinusOperatorElement::elementName, UnaryMinusOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::GroupingParenthesisOperatorElement::elementName,
        GroupingParenthesisOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::GroupingBracketsOperatorElement::elementName,
        GroupingBracketsOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::GroupingBracesOperatorElement::elementName,
        GroupingBracesOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::PowerOperatorElement::elementName, PowerOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::SquareRootOperatorElement::elementName, SquareRootOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::RootOperatorElement::elementName, RootOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::ComplexConjugateOperatorElement::elementName,
        ComplexConjugateOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::FactorialOperatorElement::elementName, FactorialOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::AbsoluteValueOperatorElement::elementName,
        AbsoluteValueOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::FloorOperatorElement::elementName, FloorOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::CeilingOperatorElement::elementName, CeilingOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::NearestIntegerOperatorElement::elementName,
        NearestIntegerOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::NotEqualToOperatorElement::elementName, NotEqualToOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::LessThanOperatorElement::elementName, LessThanOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::GreaterThanOperatorElement::elementName, GreaterThanOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::LessThanOrEqualToOperatorElement::elementName,
        LessThanOrEqualToOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::GreaterThanOrEqualToOperatorElement::elementName,
        GreaterThanOrEqualToOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::UnionOperatorElement::elementName, UnionOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::IntersectionOperatorElement::elementName,
        IntersectionOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::DisjointUnionOperatorElement::elementName,
        DisjointUnionOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::CartesianProductOperatorElement::elementName,
        CartesianProductOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::SetDifferenceOperatorElement::elementName,
        SetDifferenceOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::SymmetricDifferenceOperatorElement::elementName,
        SymmetricDifferenceOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::NaryUnionOperatorElement::elementName, NaryUnionOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::NaryDisjointUnionOperatorElement::elementName,
        NaryDisjointUnionOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::NotElementOfSetOperatorElement::elementName,
        NotElementOfSetOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::SetContainsAsMemberOperatorElement::elementName,
        SetContainsAsMemberOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::NotSetContainsAsMemberOperatorElement::elementName,
        NotSetContainsAsMemberOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::SubsetOperatorElement::elementName, SubsetOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::ProperSubsetOperatorElement::elementName,
        ProperSubsetOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::SupersetOperatorElement::elementName, SupersetOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::ProperSupersetOperatorElement::elementName,
        ProperSupersetOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::NotSubsetOperatorElement::elementName, NotSubsetOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::NotProperSubsetOperatorElement::elementName,
        NotProperSubsetOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::NotSupersetOperatorElement::elementName, NotSupersetOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::NotProperSupersetOperatorElement::elementName,
        NotProperSupersetOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::LogicalUnaryNotOperatorElement::elementName,
        LogicalUnaryNotOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::LogicalOverbarNotOperatorElement::elementName,
        LogicalOverbarNotOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::LogicalAndOperatorElement::elementName, LogicalAndOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::LogicalOrOperatorElement::elementName, LogicalOrOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::LogicalExclusiveOrOperatorElement::elementName,
        LogicalExclusiveOrOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::LogicalConditionalOperatorElement::elementName,
        LogicalConditionalOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::LogicalBiconditionalOperatorElement::elementName,
        LogicalBiconditionalOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::LogicalTrueSpecialValueElement::elementName,
        LogicalTrueSpecialValuePresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::LogicalFalseSpecialValueElement::elementName,
        LogicalFalseSpecialValuePresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::SubscriptIndexOperatorElement::elementName,
        SubscriptIndexOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::SubscriptRowColumnOperatorElement::elementName,
        SubscriptRowColumnOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::MatrixOperatorElement::elementName, MatrixOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::MatrixCombineLeftToRightOperatorElement::elementName,
        MatrixCombineLeftToRightOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::MatrixCombineTopToBottomOperatorElement::elementName,
        MatrixCombineTopToBottomOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::HadamardProductOperatorElement::elementName,
        HadamardProductOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::KroneckerProductOperatorElement::elementName,
        KroneckerProductOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::MatrixTransposeOperatorElement::elementName,
        MatrixTransposeOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(
        Ld::MatrixConjugateTransposeOperatorElement::elementName,
        MatrixConjugateTransposeOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::ForAllInOperatorElement::elementName, ForAllInOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::WhileOperatorElement::elementName, WhileOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::CompoundStatementOperatorElement::elementName,
        CompoundStatementOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::IfOperatorElement::elementName, IfOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::ElseIfOperatorElement::elementName, ElseIfOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::ElseOperatorElement::elementName, ElseOperatorPresentation::creator);
    Ld::Visual::registerCreator(
        Ld::BraceConditionalOperatorElement::elementName,
        BraceConditionalOperatorPresentation::creator
    );
    Ld::Visual::registerCreator(Ld::ThereforeOperatorElement::elementName, ThereforeOperatorPresentation::creator);
    Ld::Visual::registerCreator(Ld::FunctionElement::elementName, FunctionPresentation::creator);
    Ld::Visual::registerCreator(Ld::PlotElement::elementName, PlotPresentation::creator);

    Ld::DataType::registerDecoder(Model::ValueType::BOOLEAN, &BooleanDataTypePresentationGenerator::instance);
    Ld::DataType::registerDecoder(Model::ValueType::INTEGER, &IntegerDataTypePresentationGenerator::instance);
    Ld::DataType::registerDecoder(Model::ValueType::REAL,    &RealDataTypePresentationGenerator::instance);
    Ld::DataType::registerDecoder(Model::ValueType::COMPLEX, &ComplexDataTypePresentationGenerator::instance);
    Ld::DataType::registerDecoder(Model::ValueType::SET,     &SetDataTypePresentationGenerator::instance);
    Ld::DataType::registerDecoder(Model::ValueType::TUPLE,   &TupleDataTypePresentationGenerator::instance);

    Ld::DataType::registerDecoder(
        Model::ValueType::MATRIX_BOOLEAN,
        &MatrixBooleanDataTypePresentationGenerator::instance
    );

    Ld::DataType::registerDecoder(
        Model::ValueType::MATRIX_INTEGER,
        &MatrixIntegerDataTypePresentationGenerator::instance
    );

    Ld::DataType::registerDecoder(
        Model::ValueType::MATRIX_REAL,
        &MatrixRealDataTypePresentationGenerator::instance
    );

    Ld::DataType::registerDecoder(
        Model::ValueType::MATRIX_COMPLEX,
        &MatrixComplexDataTypePresentationGenerator::instance
    );

    InspectorWidget::registerCreator(Model::ValueType::BOOLEAN,        BooleanInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::INTEGER,        IntegerInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::REAL,           RealInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::COMPLEX,        ComplexInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::MATRIX_BOOLEAN, MatrixBooleanInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::MATRIX_INTEGER, MatrixIntegerInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::MATRIX_REAL,    MatrixRealInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::MATRIX_COMPLEX, MatrixComplexInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::SET,            SetInspectorWidget::creator);
    InspectorWidget::registerCreator(Model::ValueType::TUPLE,          TupleInspectorWidget::creator);


    QSharedPointer<Ld::CppCodeGenerator>
        cppCodeGenerator = Ld::CodeGenerator::codeGenerator(Ld::CppCodeGenerator::codeGeneratorName)
                           .dynamicCast<Ld::CppCodeGenerator>();

    CppCodeGeneratorVisual* cppCodeGeneratorVisual = new CppCodeGeneratorVisual(Application::instance());
    cppCodeGenerator->setVisual(cppCodeGeneratorVisual);

    QSharedPointer<Ld::CodeGenerator>
       htmlCodeGenerator = Ld::CodeGenerator::codeGenerator(Ld::HtmlCodeGenerator::codeGeneratorName);

    htmlCodeGenerator->registerTranslator(new HtmlSelectionDataTranslator);

    QSharedPointer<Ld::CodeGenerator>
       latexCodeGenerator = Ld::CodeGenerator::codeGenerator(Ld::LaTeXCodeGenerator::codeGeneratorName);

    latexCodeGenerator->registerTranslator(new LaTeXSelectionDataTranslator);

    Fixer::registerFixer(Ld::RootElement::elementName, &RootElementFixer::instance);
    Fixer::registerFixer(Ld::TextElement::elementName, &TextElementFixer::instance);
    Fixer::registerFixer(Ld::ParagraphElement::elementName, &ParagraphElementFixer::instance);
    Fixer::registerFixer(Ld::TableFrameElement::elementName, &TableFrameElementFixer::instance);
    Fixer::registerFixer(Ld::PlaceholderElement::elementName, &PlaceholderElementFixer::instance);
    Fixer::registerFixer(Ld::ListPlaceholderElement::elementName, &PlaceholderElementFixer::instance);
    Fixer::registerFixer(Ld::VariableElement::elementName, &VariableElementFixer::instance);
    Fixer::registerFixer(Ld::LiteralElement::elementName, &LiteralElementFixer::instance);
    Fixer::registerFixer(Ld::AssignmentOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ElementOfSetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::AdditionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SubtractionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::MultiplicationOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::DivisionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::FractionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::UnaryPlusOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::UnaryMinusOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::GroupingParenthesisOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::GroupingBracketsOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::GroupingBracesOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::PowerOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SquareRootOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::RootOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ComplexConjugateOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::FactorialOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::AbsoluteValueOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::FloorOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::CeilingOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NearestIntegerOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NotEqualToOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LessThanOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::GreaterThanOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LessThanOrEqualToOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::GreaterThanOrEqualToOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::UnionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::IntersectionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::DisjointUnionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::CartesianProductOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SymmetricDifferenceOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NaryUnionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NaryDisjointUnionOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NotElementOfSetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SetContainsAsMemberOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NotSetContainsAsMemberOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SetDifferenceOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SubsetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ProperSubsetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SupersetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ProperSupersetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NotSubsetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NotProperSubsetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NotSupersetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::NotProperSupersetOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LogicalUnaryNotOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LogicalOverbarNotOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LogicalAndOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LogicalOrOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LogicalExclusiveOrOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LogicalConditionalOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::LogicalBiconditionalOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SubscriptIndexOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SubscriptRowColumnOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::MatrixOperatorElement::elementName, &GridOperatorFixer::instance);
    Fixer::registerFixer(Ld::MatrixCombineLeftToRightOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::MatrixCombineTopToBottomOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::HadamardProductOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::KroneckerProductOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::MatrixTransposeOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::MatrixConjugateTransposeOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ForAllInOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::WhileOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::CompoundStatementOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::BraceConditionalOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ThereforeOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::IfOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ElseIfOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::ElseOperatorElement::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::SetElement::elementName, &ListFixer::instance);
    Fixer::registerFixer(Ld::TupleElement::elementName, &ListFixer::instance);
    Fixer::registerFixer(Ld::Range2Element::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::Range3Element::elementName, &OperatorFixer::instance);
    Fixer::registerFixer(Ld::FunctionElement::elementName, &FunctionFixer::instance);
    Fixer::registerFixer(Ld::FunctionPlaceholderElement::elementName, &PlaceholderElementFixer::instance);

    Ld::DocumentSetting::registerCreator(CursorPositionSetting::settingName, CursorPositionSetting::creator);
    Ld::DocumentSetting::registerCreator(ZoomSetting::settingName, ZoomSetting::creator);

    registrar->registerBuilder(new HomeBuilderInitial);
    registrar->registerBuilder(new HomeBuilderFinal);
    registrar->registerBuilder(new RunExportBuilder);
    registrar->registerBuilder(new PageFormatBuilder);
    registrar->registerBuilder(new BlockFormatBuilderEarly);
    registrar->registerBuilder(new BlockFormatBuilderMid);
    registrar->registerBuilder(new BlockFormatBuilderLate);
    registrar->registerBuilder(new CharacterFormatBuilderEarly);
    registrar->registerBuilder(new CharacterFormatBuilderLate);
    registrar->registerBuilder(new TableFormatBuilder);
    registrar->registerBuilder(new ImageBuilder);
    registrar->registerBuilder(new PageBreakBuilder);
    registrar->registerBuilder(new SpecialCharactersBuilder);
    registrar->registerBuilder(new ValueFieldBuilder);
    registrar->registerBuilder(new OperatorFormatBuilder);
    registrar->registerBuilder(new BasicMathBuilder);
    registrar->registerBuilder(new SetsAndLogicBuilder);
    registrar->registerBuilder(new MatrixBuilder);
    registrar->registerBuilder(new ControlFlowBuilder);
    registrar->registerBuilder(new FunctionBuilder);
    registrar->registerBuilder(new PlotBuilder);

    PlotEngine::registerInstance(&XyPlotEngine::instance);
    PlotEngine::registerInstance(&Polar2DPlotEngine::instance);
    PlotEngine::registerInstance(&BarChartEngine::instance);
    PlotEngine::registerInstance(&HistogramEngine::instance);
    PlotEngine::registerInstance(&PieChartEngine::instance);
    PlotEngine::registerInstance(&DonutChartEngine::instance);
    PlotEngine::registerInstance(&HeatChartEngine::instance);
    PlotEngine::registerInstance(&RgbImageEngine::instance);
    PlotEngine::registerInstance(&CmykImageEngine::instance);
    PlotEngine::registerInstance(&GrayscaleImageEngine::instance);
}
