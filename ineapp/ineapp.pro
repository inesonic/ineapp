##-*-makefile-*-########################################################################################################
# Copyright 2016 Inesonic, LLC
# All Rights Reserved
########################################################################################################################

########################################################################################################################
# Basic build characteristics
#

TEMPLATE = lib

QT += core gui widgets svg network printsupport multimedia charts
CONFIG += shared c++14

equals(QT_MAJOR_VERSION, 6) {
    QT += svgwidgets
}

DEFINES += INEAPP_BUILD
DEFINES += INEM_USE_OS_ESCAPE

unix:!macx {
    QT += dbus
}

win32 {
    # Windows produces a rather stupid warning indicating branch of a tree inherits a method when virtual inheritance is
    # used.  Since this warning is stating the intent of the inheritance, and clutters things needlessly, we disable it
    # for Windows.
    QMAKE_CXXFLAGS_WARN_ON += -wd4250
}

########################################################################################################################
# Inesonic Public includes
#

INCLUDEPATH += include
API_HEADERS = include/app_common.h \
              include/version.h \
              include/metatypes.h \
              include/mac_proxy_style.h \
              include/application.h \
              include/splash_screen.h \
              include/first_time_start_dialog.h \
              include/selection_data.h \
              include/html_selection_data_translator.h \
              include/latex_selection_data_translator.h \
              include/clipboard.h \
              include/plug_in_registrar.h \
              include/registrar.h \
              include/desktop_services.h \
              include/plug_in_manager.h \
              include/fixer.h \
              include/root_element_fixer.h \
              include/text_element_fixer.h \
              include/paragraph_element_fixer.h \
              include/table_frame_element_fixer.h \
              include/variable_element_fixer.h \
              include/literal_element_fixer.h \
              include/operator_fixer.h \
              include/placeholder_element_fixer.h \
              include/list_fixer.h \
              include/grid_operator_fixer.h \
              include/function_fixer.h \
              include/cpp_code_generator_visual.h \
              include/console_device.h \
              include/runtime_diagnostic.h \
              include/build_execute_state_machine.h \
              include/edit_helpers.h \
              include/command.h \
              include/command_base.h \
              include/command_base_with_undo.h \
              include/command_container.h \
              include/command_queue_base.h \
              include/insert_string_command.h \
              include/delete_command.h \
              include/insert_element_command.h \
              include/modify_grid_command.h \
              include/modify_grid_add_rows_command.h \
              include/modify_grid_add_columns_command.h \
              include/modify_grid_remove_row_command.h \
              include/modify_grid_remove_column_command.h \
              include/modify_table_command.h \
              include/modify_table_add_rows_command.h \
              include/modify_table_remove_rows_command.h \
              include/modify_table_add_columns_command.h \
              include/modify_table_remove_columns_command.h \
              include/modify_table_merge_cells_command.h \
              include/modify_table_unmerge_cells_command.h \
              include/modify_table_delete_command.h \
              include/update_page_format_command.h \
              include/update_format_command.h \
              include/cut_copy_command.h \
              include/paste_command.h \
              include/main_window.h \
              include/view_widget.h \
              include/view_proxy.h \
              include/math_view_proxy_base.h \
              include/scene_units.h \
              include/command_queue.h \
              include/document.h \
              include/page_list.h \
              include/editor.h \
              include/document_file_dialog.h \
              include/image_file_dialog.h \
              include/about_dialog.h \
              include/about_plugins_dialog.h \
              include/plug_in_details_dialog.h \
              include/find_and_replace_dialog.h \
              include/imports_dialog.h \
              include/application_preferences_dialog.h \
              include/document_preferences_dialog.h \
              include/page_format_dialog.h \
              include/block_format_dialog.h \
              include/bullet_number_popup_dialog.h \
              include/character_format_dialog.h \
              include/image_format_dialog.h \
              include/list_format_dialog.h \
              include/export_status_dialog.h \
              include/code_generator_export_status_dialog.h \
              include/printing_status_dialog.h \
              include/pdf_export_dialog.h \
              include/html_export_dialog.h \
              include/latex_export_dialog.h \
              include/runtime_library_file_dialog.h \
              include/library_export_dialog.h \
              include/object_export_dialog.h \
              include/table_column_widths_widget.h \
              include/table_format_dialog.h \
              include/table_default_line_style_dialog.h \
              include/table_line_style_dialog.h \
              include/table_add_remove_dialog.h \
              include/table_merge_dialog.h \
              include/printing_engine.h \
              include/cursor.h \
              include/placement_negotiator.h \
              include/placement_tracker.h \
              include/placement_status_notifier.h \
              include/application_status_bar.h \
              include/placement_line_data.h \
              include/presentation_area.h \
              include/placement_line_start.h \
              include/placement_line_start_list.h \
              include/command_popup_dialog.h \
              include/presentation.h \
              include/presentation_with_no_children.h \
              include/presentation_with_fixed_children.h \
              include/presentation_with_positional_children.h \
              include/presentation_with_grouped_children.h \
              include/presentation_with_grid_children.h \
              include/presentation_with_floating_children.h \
              include/data_type_presenter.h \
              include/data_type_presentation_generator.h \
              include/leaf_presentation.h \
              include/presentation_locator.h \
              include/presentation_area_tracker.h \
              include/root_child_location.h \
              include/root_presentation.h \
              include/text_presentation_helper.h \
              include/text_presentation.h \
              include/frame_presentation_base.h \
              include/paragraph_presentation_base.h \
              include/paragraph_presentation.h \
              include/image_presentation.h \
              include/page_break_presentation.h \
              include/table_frame_presentation.h \
              include/value_field_presentation.h \
              include/variable_name_fields_widget.h \
              include/variable_name_special_characters_widget.h \
              include/variable_name_editor_widget.h \
              include/numeric_value_format_dialog.h \
              include/parenthesis_style_popup_dialog.h \
              include/multiplication_style_popup_dialog.h \
              include/division_style_popup_dialog.h \
              include/logical_not_style_popup_dialog.h \
              include/operator_format_dialog.h \
              include/matrix_size_dialog.h \
              include/matrix_insert_row_column_dialog.h \
              include/brace_conditional_configuration_dialog.h \
              include/boolean_delegate.h \
              include/integer_delegate.h \
              include/real_delegate.h \
              include/inspector_dialog.h \
              include/inspector_widget.h \
              include/matrix_inspector_widget.h \
              include/matrix_inspector_model.h \
              include/boolean_inspector_widget.h \
              include/integer_inspector_widget.h \
              include/real_inspector_widget.h \
              include/complex_inspector_widget.h \
              include/matrix_boolean_inspector_widget.h \
              include/matrix_integer_inspector_widget.h \
              include/matrix_real_inspector_widget.h \
              include/matrix_complex_inspector_widget.h \
              include/set_inspector_widget.h \
              include/tuple_inspector_widget.h \
\
              include/special_symbol_presentation_base.h \
              include/operator_presentation_base.h \
              include/unary_operator_presentation_base.h \
              include/unary_superscript_operator_presentation_base.h \
              include/binary_operator_presentation_base.h \
              include/trinary_operator_presentation_base.h \
              include/multiplication_operator_presentation_base.h \
              include/division_operator_presentation_base.h \
              include/logical_not_operator_presentation_base.h \
              include/grouping_operator_presentation_base.h \
              include/simple_grouping_operator_presentation_base.h \
              include/unary_large_operator_presentation_base.h \
              include/root_operator_presentation_base.h \
              include/matrix_operator_presentation_base.h \
              include/control_flow_operator_presentation_base.h \
              include/placeholder_presentation.h \
              include/list_placeholder_presentation.h \
              include/assignment_operator_presentation.h \
              include/variable_presentation.h \
              include/literal_presentation.h \
              include/element_of_set_operator_presentation.h \
              include/boolean_type_presentation.h \
              include/simple_text_data_type_presentation_generator_base.h \
              include/matrix_data_type_presentation_generator_base.h \
              include/boolean_data_type_presentation_generator.h \
              include/integer_type_presentation.h \
              include/integer_data_type_presentation_generator.h \
              include/real_type_presentation.h \
              include/real_data_type_presentation_generator_base.h \
              include/real_data_type_presentation_generator.h \
              include/complex_type_presentation.h \
              include/complex_data_type_presentation_generator.h \
              include/list_presentation_base.h \
              include/set_presentation.h \
              include/set_data_type_presentation_generator.h \
              include/tuple_presentation.h \
              include/tuple_data_type_presentation_generator.h \
              include/matrix_boolean_data_type_presentation_generator.h \
              include/matrix_integer_data_type_presentation_generator.h \
              include/matrix_real_data_type_presentation_generator.h \
              include/matrix_complex_data_type_presentation_generator.h \
              include/range_2_presentation.h \
              include/range_3_presentation.h \
              include/pi_special_value_presentation.h \
              include/eulers_number_special_value_presentation.h \
              include/epsilon_special_value_presentation.h \
              include/infinity_special_value_presentation.h \
              include/null_set_special_value_presentation.h \
\
              include/addition_operator_presentation.h \
              include/subtraction_operator_presentation.h \
              include/multiplication_operator_presentation.h \
              include/division_operator_presentation.h \
              include/fraction_operator_presentation.h \
              include/unary_plus_operator_presentation.h \
              include/unary_minus_operator_presentation.h \
              include/grouping_parenthesis_operator_presentation.h \
              include/grouping_brackets_operator_presentation.h \
              include/grouping_braces_operator_presentation.h \
              include/power_operator_presentation.h \
              include/square_root_operator_presentation.h \
              include/root_operator_presentation.h \
              include/complex_conjugate_operator_presentation.h \
              include/factorial_operator_presentation.h \
              include/absolute_value_operator_presentation.h \
              include/floor_operator_presentation.h \
              include/ceiling_operator_presentation.h \
              include/nearest_integer_operator_presentation.h \
\
              include/not_equal_to_operator_presentation.h \
              include/less_than_operator_presentation.h \
              include/greater_than_operator_presentation.h \
              include/less_than_or_equal_to_operator_presentation.h \
              include/greater_than_or_equal_to_operator_presentation.h \
\
              include/union_operator_presentation.h \
              include/intersection_operator_presentation.h \
              include/disjoint_union_operator_presentation.h \
              include/cartesian_product_operator_presentation.h \
              include/set_difference_operator_presentation.h \
              include/symmetric_difference_operator_presentation.h \
              include/nary_union_operator_presentation.h \
              include/nary_disjoint_union_operator_presentation.h \
              include/not_element_of_set_operator_presentation.h \
              include/set_contains_as_member_operator_presentation.h \
              include/not_set_contains_as_member_operator_presentation.h \
              include/subset_operator_presentation.h \
              include/proper_subset_operator_presentation.h \
              include/superset_operator_presentation.h \
              include/proper_superset_operator_presentation.h \
              include/not_subset_operator_presentation.h \
              include/not_proper_subset_operator_presentation.h \
              include/not_superset_operator_presentation.h \
              include/not_proper_superset_operator_presentation.h \
\
              include/logical_unary_not_operator_presentation.h \
              include/logical_overbar_not_operator_presentation.h \
              include/logical_and_operator_presentation.h \
              include/logical_or_operator_presentation.h \
              include/logical_exclusive_or_operator_presentation.h \
              include/logical_conditional_operator_presentation.h \
              include/logical_biconditional_operator_presentation.h \
              include/logical_true_special_value_presentation.h \
              include/logical_false_special_value_presentation.h \
\
              include/subscript_index_operator_presentation.h \
              include/subscript_row_column_operator_presentation.h \
              include/matrix_operator_presentation.h \
              include/matrix_combine_left_to_right_operator_presentation.h \
              include/matrix_combine_top_to_bottom_operator_presentation.h \
              include/hadamard_product_operator_presentation.h \
              include/kronecker_product_operator_presentation.h \
              include/matrix_transpose_operator_presentation.h \
              include/matrix_conjugate_transpose_operator_presentation.h \
\
              include/for_all_in_operator_presentation.h \
              include/while_operator_presentation.h \
              include/compound_statement_operator_presentation.h \
              include/if_operator_presentation.h \
              include/else_if_operator_presentation.h \
              include/else_operator_presentation.h \
              include/brace_conditional_operator_presentation.h \
              include/therefore_operator_presentation.h \
\
              include/function_presentation.h \
              include/function_placeholder_presentation.h \
\
              include/plot_presentation.h \
              include/plot_presentation_data.h \
              include/plot_wrapped_presentation_data.h \
              include/plot_wrapped_data_presentation_data.h \
              include/plot_engine.h \
              include/plot_wrapped_engine_base.h \
              include/plot_settings_widget.h \
              include/plot_wizard.h \
              include/plot_format_dialog.h \
              include/plot_widget_base.h \
              include/plot_style_widget.h \
              include/plot_data_sources_widget.h \
              include/plot_marker_painter.h \
              include/plot_series_demo_widget.h \
              include/double_automatic_line_edit.h \
              include/line_style_widget.h \
              include/plot_axis_settings_widget.h \
              include/plot_series_widget.h \
              include/plot_series_dialog.h \
              include/plot_appearance_widget.h \
              include/plot_2d_presentation_data.h \
              include/plot_2d_engine_base.h \
              include/xy_plot_engine.h \
              include/polar_2d_plot_engine.h \
              include/plot_2d_categorized_presentation_data.h \
              include/plot_2d_categorized_engine_base.h \
              include/bar_chart_engine.h \
              include/histogram_engine.h \
              include/pie_chart_engine.h \
              include/donut_chart_engine.h \
              include/heat_chart_engine.h \
              include/image_render_engine.h \
              include/rgb_image_engine.h \
              include/cmyk_image_engine.h \
              include/grayscale_image_engine.h \

########################################################################################################################
# Source files
#

SOURCES = source/metatypes.cpp \
          source/mac_proxy_style.cpp \
          source/application.cpp \
          source/splash_screen.cpp \
          source/first_time_start_dialog.cpp \
          source/selection_data.cpp \
          source/html_selection_data_translator.cpp \
          source/latex_selection_data_translator.cpp \
          source/clipboard.cpp \
          source/clipboard_mime_data.cpp \
          source/registrar.cpp \
          source/desktop_services.cpp \
          source/tool_button_sizing_dialog.cpp \
          source/plug_in_manager.cpp \
          source/fixer.cpp \
          source/root_element_fixer.cpp \
          source/text_element_fixer.cpp \
          source/paragraph_element_fixer.cpp \
          source/table_frame_element_fixer.cpp \
          source/variable_element_fixer.cpp \
          source/literal_element_fixer.cpp \
          source/operator_fixer.cpp \
          source/placeholder_element_fixer.cpp \
          source/list_fixer.cpp \
          source/grid_operator_fixer.cpp \
          source/function_fixer.cpp \
          source/console_device.cpp \
          source/function_browser_model.cpp \
          source/function_browser_delegate.cpp \
          source/runtime_diagnostic.cpp \
          source/build_execute_state_machine.cpp \
          source/cpp_code_generator_visual.cpp \
          source/loaded_model_status.cpp \
          source/edit_helpers.cpp \
          source/command.cpp \
          source/command_base.cpp \
          source/command_base_with_undo.cpp \
          source/command_container.cpp \
          source/command_queue_base.cpp \
          source/insert_string_command.cpp \
          source/delete_command.cpp \
          source/insert_element_command.cpp \
          source/modify_grid_command.cpp \
          source/modify_grid_add_rows_command.cpp \
          source/modify_grid_add_columns_command.cpp \
          source/modify_grid_remove_row_command.cpp \
          source/modify_grid_remove_column_command.cpp \
          source/modify_table_command.cpp \
          source/modify_table_add_rows_command.cpp \
          source/modify_table_remove_rows_command.cpp \
          source/modify_table_add_columns_command.cpp \
          source/modify_table_remove_columns_command.cpp \
          source/modify_table_merge_cells_command.cpp \
          source/modify_table_unmerge_cells_command.cpp \
          source/modify_table_delete_command.cpp \
          source/update_page_format_command.cpp \
          source/update_format_command.cpp \
          source/cut_copy_command.cpp \
          source/paste_command.cpp \
          source/configure.cpp \
          source/main_window.cpp \
          source/view_widget.cpp \
          source/view_proxy.cpp \
          source/scene_units.cpp \
          source/command_queue.cpp \
          source/document.cpp \
          source/cursor_position_setting.cpp \
          source/zoom_setting.cpp \
          source/page_list_page.cpp \
          source/page_list.cpp \
          source/home_builder_initial.cpp \
          source/home_builder_final.cpp \
          source/home_main_window_proxy.cpp \
          source/home_view_proxy.cpp \
          source/run_export_builder.cpp \
          source/run_export_main_window_proxy.cpp \
          source/run_export_view_proxy.cpp \
          source/page_format_builder.cpp \
          source/page_format_main_window_proxy.cpp \
          source/page_format_view_proxy.cpp \
          source/block_format_builder_early.cpp \
          source/block_format_builder_mid.cpp \
          source/block_format_builder_late.cpp \
          source/block_format_main_window_proxy.cpp \
          source/block_format_view_proxy.cpp \
          source/list_format_updater.cpp \
          source/remove_list_format_updater.cpp \
          source/ordered_list_style_format_updater.cpp \
          source/unordered_list_style_format_updater.cpp \
          source/full_list_format_updater.cpp \
          source/character_format_builder_early.cpp \
          source/character_format_builder_late.cpp \
          source/character_format_main_window_proxy.cpp \
          source/character_format_view_proxy.cpp \
          source/character_format_helpers.cpp \
          source/image_builder.cpp \
          source/image_main_window_proxy.cpp \
          source/image_view_proxy.cpp \
          source/page_break_builder.cpp \
          source/page_break_view_proxy.cpp \
          source/special_characters_builder.cpp \
          source/special_characters_marshaller.cpp \
          source/special_characters_main_window_proxy.cpp \
          source/special_characters_view_proxy.cpp \
          source/editor.cpp \
          source/document_file_dialog.cpp \
          source/image_file_dialog.cpp \
          source/about_dialog.cpp \
          source/plug_in_table_model.cpp \
          source/about_plugins_dialog.cpp \
          source/plug_in_details_dialog.cpp \
          source/find_and_replace_dialog.cpp \
          source/imports_dialog.cpp \
          source/imports_model.cpp \
          source/imports_delegate.cpp \
          source/application_preferences_dialog.cpp \
          source/document_preferences_dialog.cpp \
          source/page_format_dialog.cpp \
          source/block_format_dialog.cpp \
          source/bullet_number_popup_dialog.cpp \
          source/unicode_marshaller.cpp \
          source/character_format_dialog.cpp \
          source/image_format_dialog.cpp \
          source/list_format_dialog.cpp \
          source/export_status_dialog.cpp \
          source/code_generator_export_status_dialog.cpp \
          source/printing_status_dialog.cpp \
          source/pdf_export_dialog.cpp \
          source/html_export_dialog.cpp \
          source/latex_export_dialog.cpp \
          source/runtime_library_file_dialog.cpp \
          source/library_export_dialog.cpp \
          source/object_export_dialog.cpp \
          source/line_settings_widget.cpp \
          source/table_format_dialog.cpp \
          source/table_default_line_style_dialog.cpp \
          source/table_line_style_dialog.cpp \
          source/table_add_remove_dialog.cpp \
          source/table_merge_dialog.cpp \
          source/printing_engine.cpp \
          source/cursor.cpp \
          source/placement_negotiator.cpp \
          source/placement_tracker.cpp \
          source/placement_status_notifier.cpp \
          source/application_status_bar.cpp \
          source/placement_line_data.cpp \
          source/presentation_area.cpp \
          source/placement_line_start.cpp \
          source/placement_line_start_list.cpp \
          source/command_list_model.cpp \
          source/command_popup_dialog.cpp \
          source/presentation.cpp \
          source/presentation_with_no_children.cpp \
          source/presentation_with_fixed_children.cpp \
          source/presentation_with_positional_children.cpp \
          source/presentation_with_grouped_children.cpp \
          source/presentation_with_grid_children.cpp \
          source/presentation_with_floating_children.cpp \
          source/data_type_presenter.cpp \
          source/data_type_presentation_generator.cpp \
          source/leaf_presentation.cpp \
          source/presentation_locator.cpp \
          source/presentation_area_tracker.cpp \
          source/root_child_location.cpp \
          source/root_presentation.cpp \
          source/text_presentation_helper.cpp \
          source/text_presentation.cpp \
          source/frame_presentation_base.cpp \
          source/paragraph_presentation_base.cpp \
          source/paragraph_presentation.cpp \
          source/image_presentation.cpp \
          source/page_break_presentation.cpp \
          source/table_column_widths_model.cpp \
          source/table_column_widths_delegate.cpp \
          source/table_column_widths_widget.cpp \
          source/table_frame_presentation_format.cpp \
          source/table_frame_presentation_area.cpp \
          source/table_frame_presentation_cursor.cpp \
          source/table_frame_presentation_row_location.cpp \
          source/table_frame_presentation_rectangle.cpp \
          source/table_frame_presentation_tracker.cpp \
          source/table_frame_presentation.cpp \
          source/table_format_builder.cpp \
          source/table_format_main_window_proxy.cpp \
          source/table_format_view_proxy.cpp \
          source/value_field_presentation.cpp \
          source/variable_name_fields_widget.cpp \
          source/variable_name_special_characters_widget.cpp \
          source/variable_name_editor_widget.cpp \
          source/numeric_value_format_dialog.cpp \
          source/value_field_builder.cpp \
          source/value_field_main_window_proxy.cpp \
          source/value_field_view_proxy.cpp \
          source/operator_format_builder.cpp \
          source/operator_format_main_window_proxy.cpp \
          source/operator_format_view_proxy.cpp \
          source/parenthesis_style_popup_dialog.cpp \
          source/multiplication_style_popup_dialog.cpp \
          source/division_style_popup_dialog.cpp \
          source/logical_not_style_popup_dialog.cpp \
          source/operator_format_dialog.cpp \
          source/matrix_size_dialog.cpp \
          source/matrix_insert_row_column_dialog.cpp \
          source/brace_conditional_configuration_dialog.cpp \
          source/boolean_delegate.cpp \
          source/integer_delegate.cpp \
          source/real_delegate.cpp \
          source/inspector_dialog.cpp \
          source/inspector_widget.cpp \
          source/matrix_inspector_widget.cpp \
          source/matrix_inspector_model.cpp \
          source/boolean_inspector_widget.cpp \
          source/integer_inspector_widget.cpp \
          source/real_inspector_widget.cpp \
          source/complex_inspector_widget.cpp \
          source/matrix_boolean_inspector_widget.cpp \
          source/matrix_boolean_inspector_model.cpp \
          source/matrix_integer_inspector_widget.cpp \
          source/matrix_integer_inspector_model.cpp \
          source/matrix_real_inspector_widget.cpp \
          source/matrix_real_inspector_model.cpp \
          source/matrix_complex_inspector_widget.cpp \
          source/matrix_complex_inspector_model.cpp \
          source/matrix_complex_inspector_selection_model.cpp \
          source/set_inspector_widget.cpp \
          source/set_inspector_model.cpp \
          source/tuple_inspector_widget.cpp \
          source/tuple_inspector_model.cpp \
\
          source/math_view_proxy_base.cpp \
          source/basic_math_builder.cpp \
          source/basic_math_main_window_proxy.cpp \
          source/basic_math_view_proxy.cpp \
          source/sets_and_logic_builder.cpp \
          source/sets_and_logic_main_window_proxy.cpp \
          source/sets_and_logic_view_proxy.cpp \
          source/matrix_builder.cpp \
          source/matrix_main_window_proxy.cpp \
          source/matrix_view_proxy.cpp \
          source/control_flow_builder.cpp \
          source/control_flow_main_window_proxy.cpp \
          source/control_flow_view_proxy.cpp \
          source/function_marshaller.cpp \
          source/function_dock_widget.cpp \
          source/function_builder.cpp \
          source/function_main_window_proxy.cpp \
          source/function_view_proxy.cpp \
\
          source/special_symbol_presentation_base.cpp \
          source/operator_presentation_base.cpp \
          source/unary_operator_presentation_base.cpp \
          source/unary_superscript_operator_presentation_base.cpp \
          source/binary_operator_presentation_base.cpp \
          source/trinary_operator_presentation_base.cpp \
          source/multiplication_operator_presentation_base.cpp \
          source/division_operator_presentation_base.cpp \
          source/logical_not_operator_presentation_base.cpp \
          source/grouping_operator_presentation_base.cpp \
          source/simple_grouping_operator_presentation_base.cpp \
          source/unary_large_operator_presentation_base.cpp \
          source/root_operator_presentation_base.cpp \
          source/matrix_operator_presentation_base.cpp \
          source/control_flow_operator_presentation_base.cpp \
          source/placeholder_presentation.cpp \
          source/list_placeholder_presentation.cpp \
          source/assignment_operator_presentation.cpp \
          source/variable_presentation.cpp \
          source/literal_presentation.cpp \
          source/element_of_set_operator_presentation.cpp \
          source/simple_text_data_type_presentation_generator_base.cpp \
          source/matrix_data_type_presentation_generator_base.cpp \
          source/boolean_type_presentation.cpp \
          source/boolean_data_type_presentation_generator.cpp \
          source/integer_type_presentation.cpp \
          source/integer_data_type_presentation_generator.cpp \
          source/real_type_presentation.cpp \
          source/real_data_type_presentation_generator_base.cpp \
          source/real_data_type_presentation_generator.cpp \
          source/complex_type_presentation.cpp \
          source/complex_data_type_presentation_generator.cpp \
          source/list_presentation_base.cpp \
          source/set_presentation.cpp \
          source/set_data_type_presentation_generator.cpp \
          source/tuple_presentation.cpp \
          source/tuple_data_type_presentation_generator.cpp \
          source/matrix_boolean_data_type_presentation_generator.cpp \
          source/matrix_integer_data_type_presentation_generator.cpp \
          source/matrix_real_data_type_presentation_generator.cpp \
          source/matrix_complex_data_type_presentation_generator.cpp \
          source/range_2_presentation.cpp \
          source/range_3_presentation.cpp \
          source/pi_special_value_presentation.cpp \
          source/eulers_number_special_value_presentation.cpp \
          source/epsilon_special_value_presentation.cpp \
          source/infinity_special_value_presentation.cpp \
          source/null_set_special_value_presentation.cpp \
\
          source/addition_operator_presentation.cpp \
          source/subtraction_operator_presentation.cpp \
          source/multiplication_operator_presentation.cpp \
          source/division_operator_presentation.cpp \
          source/fraction_operator_presentation.cpp \
          source/unary_plus_operator_presentation.cpp \
          source/unary_minus_operator_presentation.cpp \
          source/grouping_parenthesis_operator_presentation.cpp \
          source/grouping_brackets_operator_presentation.cpp \
          source/grouping_braces_operator_presentation.cpp \
          source/power_operator_presentation.cpp \
          source/square_root_operator_presentation.cpp \
          source/root_operator_presentation.cpp \
          source/complex_conjugate_operator_presentation.cpp \
          source/factorial_operator_presentation.cpp \
          source/absolute_value_operator_presentation.cpp \
          source/floor_operator_presentation.cpp \
          source/ceiling_operator_presentation.cpp \
          source/nearest_integer_operator_presentation.cpp \
\
          source/not_equal_to_operator_presentation.cpp \
          source/less_than_operator_presentation.cpp \
          source/greater_than_operator_presentation.cpp \
          source/less_than_or_equal_to_operator_presentation.cpp \
          source/greater_than_or_equal_to_operator_presentation.cpp \
\
          source/union_operator_presentation.cpp \
          source/intersection_operator_presentation.cpp \
          source/disjoint_union_operator_presentation.cpp \
          source/cartesian_product_operator_presentation.cpp \
          source/set_difference_operator_presentation.cpp \
          source/symmetric_difference_operator_presentation.cpp \
          source/nary_union_operator_presentation.cpp \
          source/nary_disjoint_union_operator_presentation.cpp \
          source/not_element_of_set_operator_presentation.cpp \
          source/set_contains_as_member_operator_presentation.cpp \
          source/not_set_contains_as_member_operator_presentation.cpp \
          source/subset_operator_presentation.cpp \
          source/proper_subset_operator_presentation.cpp \
          source/superset_operator_presentation.cpp \
          source/proper_superset_operator_presentation.cpp \
          source/not_subset_operator_presentation.cpp \
          source/not_proper_subset_operator_presentation.cpp \
          source/not_superset_operator_presentation.cpp \
          source/not_proper_superset_operator_presentation.cpp \
\
          source/logical_unary_not_operator_presentation.cpp \
          source/logical_overbar_not_operator_presentation.cpp \
          source/logical_and_operator_presentation.cpp \
          source/logical_or_operator_presentation.cpp \
          source/logical_exclusive_or_operator_presentation.cpp \
          source/logical_conditional_operator_presentation.cpp \
          source/logical_biconditional_operator_presentation.cpp \
          source/logical_true_special_value_presentation.cpp \
          source/logical_false_special_value_presentation.cpp \
\
          source/subscript_index_operator_presentation.cpp \
          source/subscript_row_column_operator_presentation.cpp \
          source/matrix_operator_presentation.cpp \
          source/matrix_combine_left_to_right_operator_presentation.cpp \
          source/matrix_combine_top_to_bottom_operator_presentation.cpp \
          source/hadamard_product_operator_presentation.cpp \
          source/kronecker_product_operator_presentation.cpp \
          source/matrix_transpose_operator_presentation.cpp \
          source/matrix_conjugate_transpose_operator_presentation.cpp \
\
          source/for_all_in_operator_presentation.cpp \
          source/while_operator_presentation.cpp \
          source/compound_statement_operator_presentation.cpp \
          source/if_operator_presentation.cpp \
          source/else_if_operator_presentation.cpp \
          source/else_operator_presentation.cpp \
          source/brace_conditional_operator_presentation.cpp \
          source/therefore_operator_presentation.cpp \
\
          source/function_presentation.cpp \
          source/function_placeholder_presentation.cpp \
\
          source/plot_presentation.cpp \
          source/plot_presentation_data.cpp \
          source/plot_wrapped_presentation_data.cpp \
          source/plot_wrapped_data_presentation_data.cpp \
          source/plot_engine.cpp \
          source/plot_wrapped_engine_base.cpp \
          source/plot_builder.cpp \
          source/plot_view_proxy.cpp \
          source/plot_settings_widget.cpp \
          source/plot_wizard.cpp \
          source/plot_format_dialog.cpp \
          source/plot_widget_base.cpp \
          source/plot_style_widget.cpp \
          source/plot_data_sources_widget.cpp \
          source/plot_data_sources_model.cpp \
          source/plot_data_sources_delegate.cpp \
          source/plot_marker_painter.cpp \
          source/plot_series_demo_widget.cpp \
          source/double_automatic_line_edit.cpp \
          source/line_style_widget.cpp \
          source/plot_axis_settings_widget.cpp \
          source/plot_series_widget.cpp \
          source/plot_series_dialog.cpp \
          source/plot_appearance_widget.cpp \
          source/plot_2d_engine_base.cpp \
          source/plot_2d_presentation_data.cpp \
          source/xy_plot_presentation_data.cpp \
          source/xy_plot_engine.cpp \
          source/polar_2d_plot_engine.cpp \
          source/polar_2d_plot_presentation_data.cpp \
          source/plot_2d_categorized_presentation_data.cpp \
          source/plot_2d_categorized_engine_base.cpp \
          source/bar_chart_engine.cpp \
          source/bar_chart_presentation_data.cpp \
          source/histogram_engine.cpp \
          source/histogram_presentation_data.cpp \
          source/pie_chart_engine.cpp \
          source/pie_chart_presentation_data.cpp \
          source/donut_chart_engine.cpp \
          source/donut_chart_presentation_data.cpp \
          source/heat_chart_engine.cpp \
          source/heat_chart_presentation_data.cpp \
          source/image_render_engine.cpp \
          source/image_render_presentation_data.cpp \
          source/rgb_image_engine.cpp \
          source/rgb_image_presentation_data.cpp \
          source/cmyk_image_engine.cpp \
          source/cmyk_image_presentation_data.cpp \
          source/grayscale_image_engine.cpp \
          source/grayscale_image_presentation_data.cpp \

########################################################################################################################
# Private includes
#

INCLUDEPATH += source
PRIVATE_HEADERS = source/plug_in_table_model.h \
                  source/application_settings.h \
                  source/tool_button_sizing_dialog.h \
                  source/loaded_model_status.h \
                  source/clipboard_mime_data.h \
                  source/imports_model.h \
                  source/imports_delegate.h \
                  source/page_list_page.h \
                  source/command_list_model.h \
                  source/cursor_position_setting.h \
                  source/zoom_setting.h \
                  source/function_browser_model.h \
                  source/function_browser_delegate.h \
                  source/configure.h \
                  source/home_builder_initial.h \
                  source/home_builder_final.h \
                  source/home_main_window_proxy.h \
                  source/home_view_proxy.h \
                  source/run_export_builder.h \
                  source/run_export_main_window_proxy.h \
                  source/run_export_view_proxy.h \
                  source/matrix_boolean_inspector_model.h \
                  source/matrix_integer_inspector_model.h \
                  source/matrix_real_inspector_model.h \
                  source/matrix_complex_inspector_model.h \
                  source/matrix_complex_inspector_selection_model.h \
                  source/set_inspector_model.h \
                  source/tuple_inspector_model.h \
                  source/page_format_builder.h \
                  source/page_format_main_window_proxy.h \
                  source/page_format_view_proxy.h \
                  source/block_format_builder_early.h \
                  source/block_format_builder_mid.h \
                  source/block_format_builder_late.h \
                  source/block_format_main_window_proxy.h \
                  source/block_format_view_proxy.h \
                  source/list_format_updater.h \
                  source/remove_list_format_updater.h \
                  source/ordered_list_style_format_updater.h \
                  source/unordered_list_style_format_updater.h \
                  source/full_list_format_updater.h \
                  source/character_format_builder_early.h \
                  source/character_format_builder_late.h \
                  source/character_format_main_window_proxy.h \
                  source/character_format_view_proxy.h \
                  source/character_format_helpers.h \
                  source/image_builder.h \
                  source/image_main_window_proxy.h \
                  source/image_view_proxy.h \
                  source/page_break_builder.h \
                  source/page_break_view_proxy.h \
                  source/special_characters_builder.h \
                  source/special_characters_marshaller.h \
                  source/special_characters_main_window_proxy.h \
                  source/special_characters_view_proxy.h \
                  source/unicode_marshaller.h \
                  source/line_settings_widget.h \
                  source/table_column_widths_model.h \
                  source/table_column_widths_delegate.h \
                  source/table_frame_presentation_format.h \
                  source/table_frame_presentation_area.h \
                  source/table_frame_presentation_cursor.h \
                  source/table_frame_presentation_row_location.h \
                  source/table_frame_presentation_rectangle.h \
                  source/table_frame_presentation_tracker.h \
                  source/table_format_builder.h \
                  source/table_format_main_window_proxy.h \
                  source/table_format_view_proxy.h \
                  source/value_field_builder.h \
                  source/value_field_main_window_proxy.h \
                  source/value_field_view_proxy.h \
                  source/operator_format_builder.h \
                  source/operator_format_main_window_proxy.h \
                  source/operator_format_view_proxy.h \
                  source/basic_math_builder.h \
                  source/basic_math_main_window_proxy.h \
                  source/basic_math_view_proxy.h \
                  source/sets_and_logic_builder.h \
                  source/sets_and_logic_main_window_proxy.h \
                  source/sets_and_logic_view_proxy.h \
                  source/matrix_builder.h \
                  source/matrix_main_window_proxy.h \
                  source/matrix_view_proxy.h \
                  source/control_flow_builder.h \
                  source/control_flow_main_window_proxy.h \
                  source/control_flow_view_proxy.h \
                  source/function_marshaller.h \
                  source/function_dock_widget.h \
                  source/function_builder.h \
                  source/function_main_window_proxy.h \
                  source/function_view_proxy.h \
\
                  source/plot_builder.h \
                  source/plot_view_proxy.h \
                  source/plot_data_sources_model.h \
                  source/plot_data_sources_delegate.h \
                  source/xy_plot_presentation_data.h \
                  source/polar_2d_plot_presentation_data.h \
                  source/bar_chart_presentation_data.h \
                  source/histogram_presentation_data.h \
                  source/pie_chart_presentation_data.h \
                  source/donut_chart_presentation_data.h \
                  source/heat_chart_presentation_data.h \
                  source/image_render_presentation_data.h \
                  source/rgb_image_presentation_data.h \
                  source/cmyk_image_presentation_data.h \
                  source/grayscale_image_presentation_data.h \

########################################################################################################################
# Platform specific headhers and implementation files:
#

macx {
    API_HEADERS += include/macos_linker_locator.h
    SOURCES += source/macos_linker_locator.cpp
}

########################################################################################################################
# Setup headers and installation
#

HEADERS = $$CUSTOMER_HEADERS $$API_HEADERS $$PRIVATE_HEADERS $$BUILD_HEADERS

########################################################################################################################
# Application resources:
#

RESOURCES = ineapp.qrc

########################################################################################################################
# Libraries
#

defined(SETTINGS_PRI, var) {
    include($${SETTINGS_PRI})
}

defined(LLVM_PRI, var) {
    include($${LLVM_PRI})
}

INCLUDEPATH += $${INELD_INCLUDE}
INCLUDEPATH += $${INEEQT_INCLUDE}
INCLUDEPATH += $${INECONTAINER_INCLUDE}
INCLUDEPATH += $${INEQCONTAINER_INCLUDE}
INCLUDEPATH += $${INECBE_INCLUDE}
INCLUDEPATH += $${INEM_INCLUDE}
INCLUDEPATH += $${INEUTIL_INCLUDE}
INCLUDEPATH += $${INEUD_INCLUDE}
INCLUDEPATH += $${INEWH_INCLUDE}
INCLUDEPATH += $${INECRYPTO_INCLUDE}
INCLUDEPATH += $${BOOST_INCLUDE}

LIBS += -L$${INELD_LIBDIR} -lineld
LIBS += -L$${INEEQT_LIBDIR} -lineeqt
LIBS += -L$${INECONTAINER_LIBDIR} -linecontainer
LIBS += -L$${INEQCONTAINER_LIBDIR} -lineqcontainer
LIBS += -L$${INECBE_LIBDIR} -linecbe
LIBS += -L$${INEM_LIBDIR} -linem
LIBS += -L$${INEUTIL_LIBDIR} -lineutil
LIBS += -L$${INEUD_LIBDIR} -lineud
LIBS += -L$${INEWH_LIBDIR} -linewh
LIBS += -L$${INECRYPTO_LIBDIR} -linecrypto

defined(INEMAT_PRI, var) {
    include($${INEMAT_PRI})
}

########################################################################################################################
# Operating System
#

unix {
#  Note that some of these libraries may not be part of the OS.  See if we need a distinct PRI file for any of them.
    unix:!macx {
        LIBS += -lrt -ldl
    } else {
        LIBS += -lncurses
    }

    LIBS += -lpthread
    LIBS += -lz
    LIBS += -lm
}

win32 {
    exists("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0") {
        WINDOWS_KIT_BASE="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0"
    } else {
        exists("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0") {
            WINDOWS_KIT_BASE="C:/Program Files (x86)/Windows Kits/10/Lib/10.0.19041.0"
        } else {
            error("Unknown/Missing Windows kit.")
        }
    }

    contains(QMAKE_TARGET.arch, x86_64) {
        WINDOWS_KIT_DIRECTORY="$${WINDOWS_KIT_BASE}/um/x64"
    } else {
        WINDOWS_KIT_DIRECTORY="$${WINDOWS_KIT_BASE}/um/x86"
    }

    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Version.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Psapi.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/shell32.lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Ole32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Uuid.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Kernel32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/User32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/Gdi32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/WinSpool.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/OleAut32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/User32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/ComDlg32.Lib"
    LIBS += "$${WINDOWS_KIT_DIRECTORY}/AdvAPI32.Lib"

    !contains(DEFINES, CBE_EXTERNAL_LINKER) {
        contains(QMAKE_TARGET.arch, x86_64) {
            LIBS += "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/DIA SDK/lib/amd64/diaguids.lib"
        } else {
            LIBS += "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/DIA SDK/lib/diaguids.lib"
        }
    }
}

########################################################################################################################
# Locate build intermediate and output products
#

TARGET = ineapp

CONFIG(debug, debug|release) {
    unix:DESTDIR = build/debug
    win32:DESTDIR = build/Debug
} else {
    unix:DESTDIR = build/release
    win32:DESTDIR = build/Release
}

win32 {
    # The use of template libraries causes multiple copies of the same symbols (instantiated by templates) in libraries
    # Unlike real, properly architected operating systems, Windows thinks this is an error.  These linker options tells
    # Windows to perform the link even if this is the case and to suppress the warning messages generated.
    #
    # Unfortunately, Microsoft still insists on issuing a warning indicating the user of the /FORCE option which the
    # linker will not allow you to override.

    QMAKE_LFLAGS += /FORCE:MULTIPLE /IGNORE:4006
}

OBJECTS_DIR = $${DESTDIR}/objects
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

