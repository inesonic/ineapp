/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFormatBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_stacking_layout.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_double_spin_box.h>
#include <eqt_tool_button.h>

#include "main_window.h"
#include "table_format_main_window_proxy.h"
#include "table_format_view_proxy.h"
#include "table_format_builder.h"

const char TableFormatBuilder::builderName[]             = "TableFormatBuilder";
const char TableFormatBuilder::tableFormatDockName[]     = "table_format";
const char TableFormatBuilder::tableCellFormatDockName[] = "table_cell_format";

TableFormatBuilder::TableFormatBuilder(QObject* parent):EQt::Builder(parent) {}


TableFormatBuilder::~TableFormatBuilder() {}


unsigned TableFormatBuilder::priority() const {
    return builderPriority;
}


const char* TableFormatBuilder::builderIdentifier() const {
    return builderName;
}


const char* TableFormatBuilder::plugInName() const {
    return "";
}


void TableFormatBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenuAction(
        EQt::Application::icon("insert_table"),
        tr("&Insert | &Table..."),
        "insert_table_dialog"
    );
    window->setToolTip("insert_table_dialog", tr("Inserts a table."));
    window->setWhatsThis(
        "insert_table_dialog",
        tr("Use this option to insert a new table at the cursor position.")
    );

    window->addMenuAction(
        EQt::Application::icon("format_table"),
        tr("F&ormat | &Table | &Settings..."),
        "format_table_settings_dialog"
    );
    window->setToolTip("format_table_settings_dialog", tr("Change table style and settings."));
    window->setWhatsThis(
        "format_table_settings_dialog",
        tr(
            "Use this option to update or change table settings, including alignment, cell gutters, default line "
            "styles and default cell colors."
          )
    );

    window->addSeparator(tr("F&ormat | &Table"));

    QAction* tableMergeCellsAction = window->addMenuAction(
        EQt::Application::icon("table_merge_cells"),
        tr("F&ormat | &Table | &Merge..."),
        "format_table_merge"
    );
    tableMergeCellsAction->setCheckable(true);
    tableMergeCellsAction->setToolTip(tr("Merge table cells together."));
    tableMergeCellsAction->setWhatsThis(tr("Use this option to merge table cells together."));

    window->addSeparator(tr("F&ormat | &Table"));

    window->addMenuAction(
        EQt::Application::icon("format_table_left_align"),
        tr("F&ormat | &Table | &Align | &Left..."),
        "format_table_align_left"
    );
    window->action("format_table_align_left")->setCheckable(true);
    window->setToolTip("format_table_align_left", tr("Left align table."));
    window->setWhatsThis(
        "format_table_align_left",
        tr("Use this option to left-align the table, taking into account table indentation settings.")
    );

    window->addMenuAction(
        EQt::Application::icon("format_table_center"),
        tr("F&ormat | &Table | &Align | &Center..."),
        "format_table_align_center"
    );
    window->action("format_table_align_center")->setCheckable(true);
    window->setToolTip("format_table_align_center", tr("Center table."));
    window->setWhatsThis(
        "format_table_align_center",
        tr("Use this option to horizontally center the table, taking into account table indentation settings.")
    );

    window->addMenuAction(
        EQt::Application::icon("format_table_right_align"),
        tr("F&ormat | &Table | &Align | &Right..."),
        "format_table_align_right"
    );
    window->action("format_table_align_right")->setCheckable(true);
    window->setToolTip("format_table_align_right", tr("Right align table."));
    window->setWhatsThis(
        "format_table_align_right",
        tr("Use this option to right-align the table, taking into account table indentation settings.")
    );

    window->addSeparator(tr("F&ormat | &Table"));

    window->addMenuAction(
        EQt::Application::icon("table_add_rows"),
        tr("F&ormat | &Table | &Add | &Row(s)..."),
        "format_table_add_rows_dialog"
    );
    window->setToolTip("format_table_add_rows_dialog", tr("Adds one or more rows to the table."));
    window->setWhatsThis(
        "format_table_add_rows_dialog",
        tr(
            "Use this option to add one or more rows to the table.  Rows can be added above or below the row "
            "where the cursor resides."
          )
    );

    window->addMenuAction(
        EQt::Application::icon("table_add_columns"),
        tr("F&ormat | &Table | &Add | &Column(s)..."),
        "format_table_add_columns_dialog"
    );
    window->setToolTip("format_table_add_columns_dialog", tr("Adds one or more columns from the table."));
    window->setWhatsThis(
        "format_table_add_columns_dialog",
        tr(
            "Use this option to add one or more columns to the table.  Rows can be added to the left or right of the "
            "column where the cursor resides."
          )
    );

    window->addMenuAction(
        EQt::Application::icon("table_remove_rows"),
        tr("F&ormat | &Table | Re&move | &Row(s)..."),
        "format_table_remove_rows_dialog"
    );
    window->setToolTip("format_table_remove_rows_dialog", tr("Removes one or more rows from the table."));
    window->setWhatsThis(
        "format_table_remove_rows_dialog",
        tr("Use this option to remove one or more rows to the table.")
    );

    window->addMenuAction(
        EQt::Application::icon("table_remove_columns"),
        tr("F&ormat | &Table | Re&move | &Column(s)..."),
        "format_table_remove_columns_dialog"
    );
    window->setToolTip("format_table_remove_columns_dialog", tr("Removes one or more columns from the table."));
    window->setWhatsThis(
        "format_table_remove_columns_dialog",
        tr("Use this option to remove one or more columns to the table.")
    );

    window->addMenuAction(
        EQt::Application::icon("table_remove"),
        tr("F&ormat | &Table | Re&move | &Table..."),
        "format_table_remove_table_dialog"
    );
    window->setToolTip("format_table_remove_table_dialog", tr("Removes an entire table."));
    window->setWhatsThis(
        "format_table_remove_table_dialog",
        tr("Use this option to remove an entire table.")
    );

    window->addSeparator(tr("F&ormat | &Table"));

    QAction* formatTableRowColorAction = window->addMenuAction(
        EQt::Application::icon("table_row_color"),
        tr("F&ormat | &Table | &Color | &Row..."),
        "format_table_row_color"
    );
    formatTableRowColorAction->setCheckable(true);
    formatTableRowColorAction->setToolTip(tr("Change the color to apply across a table row."));
    formatTableRowColorAction->setWhatsThis(
        tr("Use this option to change the color to apply across a table row.")
    );

    QAction* formatTableColumnColorAction = window->addMenuAction(
        EQt::Application::icon("table_column_color"),
        tr("F&ormat | &Table | &Color | Co&lumn..."),
        "format_table_column_color"
    );
    formatTableColumnColorAction->setCheckable(true);
    formatTableColumnColorAction->setToolTip(tr("Change the color to apply down a table column."));
    formatTableColumnColorAction->setWhatsThis(
        tr("Use this option to change the color to apply down an entire table column.")
    );

    QAction* formatTableCellColorAction = window->addMenuAction(
        EQt::Application::icon("table_cell_color"),
        tr("F&ormat | &Table | &Color | &Cell..."),
        "format_table_cell_color"
    );
    formatTableCellColorAction->setCheckable(true);
    formatTableCellColorAction->setToolTip(tr("Change the color to apply to specific cells."));
    formatTableCellColorAction->setWhatsThis(
        tr("Use this option to change the color to apply to one or more cells.")
    );

    QAction* formatTableRowLineStyleAction = window->addMenuAction(
        EQt::Application::icon("table_row_line_style"),
        tr("F&ormat | &Table | &Lines | &Horizontal..."),
        "format_table_lines_horizontal"
    );
    formatTableRowLineStyleAction->setCheckable(true);
    formatTableRowLineStyleAction->setToolTip(tr("Change the horizontal line style above and/or below."));
    formatTableRowLineStyleAction->setWhatsThis(
        tr("Use this option to change line style of horizontal lines above and/or below the current row.")
    );

    QAction* formatTableColumnLineStyleAction = window->addMenuAction(
        EQt::Application::icon("table_column_line_style"),
        tr("F&ormat | &Table | &Lines | &Vertical..."),
        "format_table_lines_vertical"
    );
    formatTableColumnLineStyleAction->setCheckable(true);
    formatTableColumnLineStyleAction->setToolTip(tr("Change the vertical line style left and/or right."));
    formatTableColumnLineStyleAction->setWhatsThis(
        tr("Use this option to change line style of vertical lines to the left and/or right of the current column.")
    );
}


void TableFormatBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* tableLeftIndentationAction = new QAction(window);
    window->addAction(tableLeftIndentationAction, "format_table_left_indentation");
    tableLeftIndentationAction->setToolTip(tr("Use this to change table left indentation."));
    tableLeftIndentationAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the table left side identation.  Values can be "
            "entered in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* tableRightIndentationAction = new QAction(window);
    window->addAction(tableRightIndentationAction, "format_table_right_indentation");
    tableRightIndentationAction->setToolTip(tr("Use this to change table right indentation."));
    tableRightIndentationAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the table right side identation.  Values can be "
            "entered in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* tableTopSpacingAction = new QAction(window);
    window->addAction(tableTopSpacingAction, "format_table_top_spacing");
    tableTopSpacingAction->setToolTip(tr("Use this to change table top spacing."));
    tableTopSpacingAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the table top spacing.  Values can be entered in "
            "inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* tableBottomSpacingAction = new QAction(window);
    window->addAction(tableBottomSpacingAction, "format_table_bottom_spacing");
    tableBottomSpacingAction->setToolTip(tr("Use this to change table bottom spacing."));
    tableBottomSpacingAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the table bottom spacing.  Values can be entered in "
            "inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* defaultCellColorAction = new QAction(window);
    defaultCellColorAction->setIcon(EQt::Application::icon("format_table_default_cell_color"));
    defaultCellColorAction->setToolTip(tr("Use this button to change the default table cell color."));
    defaultCellColorAction->setWhatsThis(
        tr(
           "You can use this button to change the default cell color to apply to the table.  Row, column, and cell "
           "colors will be blended with this color."
          )
    );
    window->addAction(defaultCellColorAction, "format_default_cell_color_dialog");

    QAction* defaultTableLineStyleAction = new QAction(tr("Default Line Style"));
    defaultTableLineStyleAction->setIcon(EQt::Application::icon("format_table_default_line_style"));
    window->addAction(defaultTableLineStyleAction, "format_table_default_line_style");
    defaultTableLineStyleAction->setToolTip(tr("Use this to change the table's default row/column line styles."));
    defaultTableLineStyleAction->setWhatsThis(
        tr(
            "You can use this buttom to change the default line style for rows and columns.  Note that you can "
            "override this style for individual row and column lines."
        )
    );
}


void TableFormatBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    buildAnnotationsDock(window);
    buildTableDock(window);
    buildTableCellDock(window);
}


EQt::ProgrammaticMainWindowProxy* TableFormatBuilder::createMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ) {
    return new TableFormatMainWindowProxy(window);
}


void TableFormatBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* tableFormatDockToggleAction = mainWindow->action("table_format_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("tdock"), tableFormatDockToggleAction);
    Q_ASSERT(success);

    QAction* tableCellFormatDockToggleAction = mainWindow->action("table_cell_format_dock_toggle_action");
    success = mainWindow->setCommand(tr("tcdock"), tableCellFormatDockToggleAction);
    Q_ASSERT(success);

    QAction* formatTableDialogAction = mainWindow->action("format_table_settings_dialog");
    success = mainWindow->setCommand(tr("tfmt"), formatTableDialogAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* TableFormatBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new TableFormatViewProxy(view, window);
}


void TableFormatBuilder::buildAnnotationsDock(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* insertDock = window->dockWidget("insert");

    EQt::StackingLayout* horizontalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("horizontal_annotations_stacking_layout");
    EQt::StackingLayout* verticalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("vertical_annotations_stacking_layout");

    QAction* insertPageBreakAction = window->action("insert_table_dialog");

    horizontalStackingLayout->appendWidget(new EQt::ToolButton(insertPageBreakAction));
    verticalStackingLayout->appendWidget(new EQt::ToolButton(insertPageBreakAction));
}


void TableFormatBuilder::buildTableDock(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* tableDock = window->addDockWidget(
        tableFormatDockName,
        tr("Table"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION, "image_format", false)
    );
    tableDock->setCloseButtonEnabled();

    QAction* tableDockToggleAction = window->addDockToggleMenuItem(
        tableFormatDockName,
        tr("&View | &Docks"),
        "table_format_dock_toggle_action"
    );
    tableDockToggleAction->setToolTip(tr("Show/hide the Table dock"));
    tableDockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Table dock.  The Table dock includes functions used to "
            "manipulate table position, cell gutters, and table-wide defaults such as table default colors and line "
            "settings."
        )
    );

    QHBoxLayout* tableDockHorizontalLayout = tableDock->newHBoxLayout("horizontal");
    tableDock->setHorizontalLayout(tableDockHorizontalLayout);

    buildTableDockHorizontalOrientation(tableDock, tableDockHorizontalLayout, window);

    QVBoxLayout* tableDockVerticalLayout = tableDock->newVBoxLayout("vertical");
    tableDock->setVerticalLayout(tableDockVerticalLayout);

    buildTableDockVerticalOrientation(tableDock, tableDockVerticalLayout, window);
}


void TableFormatBuilder::buildTableCellDock(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* tableCellDock = window->addDockWidget(
        tableCellFormatDockName,
        tr("Table Cell"),
        EQt::DockWidgetDefaults(
            EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION,
            tableFormatDockName,
            false
        )
    );
    tableCellDock->setCloseButtonEnabled();

    QAction* tableCellDockToggleAction = window->addDockToggleMenuItem(
        tableCellFormatDockName,
        tr("&View | &Docks"),
        "table_cell_format_dock_toggle_action"
    );
    tableCellDockToggleAction->setToolTip(tr("Show/hide the Table cell dock"));
    tableCellDockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Table Cell dock.  The Table Cell dock includes functions "
            "used to manipulate table position, cell gutters, and table-wide defaults such as table default colors "
            "and line settings."
        )
    );

    QHBoxLayout* tableCellDockHorizontalLayout = tableCellDock->newHBoxLayout("horizontal");
    tableCellDock->setHorizontalLayout(tableCellDockHorizontalLayout);

    buildTableCellDockHorizontalOrientation(tableCellDock, tableCellDockHorizontalLayout, window);

    QVBoxLayout* tableCellDockVerticalLayout = tableCellDock->newVBoxLayout("vertical");
    tableCellDock->setVerticalLayout(tableCellDockVerticalLayout);

    buildTableCellDockVerticalOrientation(tableCellDock, tableCellDockVerticalLayout, window);
}


void TableFormatBuilder::buildTableDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("horizontal_controls");
    horizontalLayout->addLayout(controlsLayout);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_align_left")), 0, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_align_center")), 0, 1);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_align_right")), 0, 2);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_default_cell_color_dialog")), 1, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_default_line_style")), 1, 1);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_settings_dialog")), 1, 2);

    QFormLayout* tableMarginLayout = dock->newFormLayout("horizontal_margins");
    horizontalLayout->addLayout(tableMarginLayout);

    EQt::DimensionLineEdit* tableLeftIndentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* tableRightIndentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* tableTopSpacingLineEdit      = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* tableBottomSpacingLineEdit   = new EQt::DimensionLineEdit;

    dock->registerWidget(tableLeftIndentationLineEdit, "horizontal_left_indentation_line_edit");
    dock->registerWidget(tableRightIndentationLineEdit, "horizontal_right_indentation_line_edit");
    dock->registerWidget(tableTopSpacingLineEdit, "horizontal_top_spacing_line_edit");
    dock->registerWidget(tableBottomSpacingLineEdit, "horizontal_bottom_spacing_line_edit");

    tableLeftIndentationLineEdit->setDynamicUnits();
    tableRightIndentationLineEdit->setDynamicUnits();
    tableTopSpacingLineEdit->setDynamicUnits();
    tableBottomSpacingLineEdit->setDynamicUnits();

    tableLeftIndentationLineEdit->setMinimumValue(0);
    tableRightIndentationLineEdit->setMinimumValue(0);
    tableTopSpacingLineEdit->setMinimumValue(0);
    tableBottomSpacingLineEdit->setMinimumValue(0);

    tableLeftIndentationLineEdit->setDefaultAction(window->action("format_table_left_indentation"));
    tableRightIndentationLineEdit->setDefaultAction(window->action("format_table_right_indentation"));
    tableTopSpacingLineEdit->setDefaultAction(window->action("format_table_top_spacing"));
    tableBottomSpacingLineEdit->setDefaultAction(window->action("format_table_bottom_spacing"));

    tableMarginLayout->addRow(tr("Top:"), tableTopSpacingLineEdit);
    tableMarginLayout->addRow(tr("Left:"), tableLeftIndentationLineEdit);
    tableMarginLayout->addRow(tr("Right:"), tableRightIndentationLineEdit);
    tableMarginLayout->addRow(tr("Bottom:"), tableBottomSpacingLineEdit);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(tableMarginLayout, 4);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void TableFormatBuilder::buildTableDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("vertical_controls");
    verticalLayout->addLayout(controlsLayout);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_align_left")), 0, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_align_center")), 0, 1);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_align_right")), 0, 2);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_default_cell_color_dialog")), 1, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_default_line_style")), 1, 1);
    controlsLayout->setColumnStretch(3, 1);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_settings_dialog")), 0, 4);

    QHBoxLayout* marginsLayout = dock->newHBoxLayout("vertical_margins");
    verticalLayout->addLayout(marginsLayout);

    QFormLayout* leftTableMarginLayout = dock->newFormLayout("vertical_left_margins");
    marginsLayout->addLayout(leftTableMarginLayout);

    QFormLayout* rightTableMarginLayout = dock->newFormLayout("vertical_right_margins");
    marginsLayout->addLayout(rightTableMarginLayout);

    EQt::DimensionLineEdit* tableLeftIndentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* tableRightIndentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* tableTopSpacingLineEdit      = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* tableBottomSpacingLineEdit   = new EQt::DimensionLineEdit;

    dock->registerWidget(tableLeftIndentationLineEdit, "vertical_left_indentation_line_edit");
    dock->registerWidget(tableRightIndentationLineEdit, "vertical_right_indentation_line_edit");
    dock->registerWidget(tableTopSpacingLineEdit, "vertical_top_spacing_line_edit");
    dock->registerWidget(tableBottomSpacingLineEdit, "vertical_bottom_spacing_line_edit");

    tableLeftIndentationLineEdit->setDynamicUnits();
    tableRightIndentationLineEdit->setDynamicUnits();
    tableTopSpacingLineEdit->setDynamicUnits();
    tableBottomSpacingLineEdit->setDynamicUnits();

    tableLeftIndentationLineEdit->setMinimumValue(0);
    tableRightIndentationLineEdit->setMinimumValue(0);
    tableTopSpacingLineEdit->setMinimumValue(0);
    tableBottomSpacingLineEdit->setMinimumValue(0);

    tableLeftIndentationLineEdit->setDefaultAction(window->action("format_table_left_indentation"));
    tableRightIndentationLineEdit->setDefaultAction(window->action("format_table_right_indentation"));
    tableTopSpacingLineEdit->setDefaultAction(window->action("format_table_top_spacing"));
    tableBottomSpacingLineEdit->setDefaultAction(window->action("format_table_bottom_spacing"));

    leftTableMarginLayout->addRow(tr("Left:"), tableLeftIndentationLineEdit);
    leftTableMarginLayout->addRow(tr("Right:"), tableRightIndentationLineEdit);
    rightTableMarginLayout->addRow(tr("Top:"), tableTopSpacingLineEdit);
    rightTableMarginLayout->addRow(tr("Bottom:"), tableBottomSpacingLineEdit);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(controlsLayout, 3);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void TableFormatBuilder::buildTableCellDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("horizontal_cell_controls");
    horizontalLayout->addLayout(controlsLayout);

    EQt::ProgrammaticDockWidget* tableDock  = window->dockWidget(tableFormatDockName);
    QWidget*                     lineEditor = tableDock->EQt::ProgrammaticWidget::widget<QWidget>(
        "horizontal_left_indentation_line_edit"
    );
    unsigned                     gapHeight  = 2 * lineEditor->sizeHint().height() + controlsLayout->spacing();
    unsigned                     gapWidth   = 6 * controlsLayout->spacing();

    QWidget* controlsGapWidget1 = new QWidget;
    dock->registerWidget(controlsGapWidget1, "horizontal_controls_gap_widget_1");
    controlsGapWidget1->setFixedWidth(gapWidth);
    controlsGapWidget1->setFixedHeight(gapHeight);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_add_rows_dialog")), 0, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_add_columns_dialog")), 0, 1);
    controlsLayout->addWidget(controlsGapWidget1, 0, 2);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_remove_rows_dialog")), 0, 3);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_remove_columns_dialog")), 0, 4);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_remove_table_dialog")), 0, 5);

    QWidget* controlsGapWidget2 = new QWidget;
    dock->registerWidget(controlsGapWidget2, "horizontal_controls_gap_widget_2");
    controlsGapWidget2->setFixedWidth(gapWidth);
    controlsGapWidget2->setFixedHeight(gapHeight);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_merge")), 1, 0);
    controlsLayout->addWidget(controlsGapWidget2, 1, 2);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_row_color")), 1, 3);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_column_color")), 1, 4);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_cell_color")), 1, 5);

    QWidget* controlsGapWidget3 = new QWidget;
    dock->registerWidget(controlsGapWidget3, "horizontal_controls_gap_widget_3");
    controlsGapWidget3->setFixedWidth(gapWidth);
    controlsGapWidget3->setFixedHeight(gapHeight);

    QWidget* controlsGapWidget4 = new QWidget;
    dock->registerWidget(controlsGapWidget4, "horizontal_controls_gap_widget_4");
    controlsGapWidget4->setFixedWidth(gapWidth);
    controlsGapWidget4->setFixedHeight(gapHeight);

    controlsLayout->addWidget(controlsGapWidget3, 0, 6);
    controlsLayout->addWidget(controlsGapWidget4, 1, 6);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_lines_horizontal")), 0, 7);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_lines_vertical")), 1, 7);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(controlsLayout, 2);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void TableFormatBuilder::buildTableCellDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("vertical_cell_controls");
    verticalLayout->addLayout(controlsLayout);

    EQt::ProgrammaticDockWidget* tableDock  = window->dockWidget(tableFormatDockName);
    QWidget*                     lineEditor = tableDock->EQt::ProgrammaticWidget::widget<QWidget>(
        "vertical_left_indentation_line_edit"
    );
    unsigned                     gapHeight  = 2 * lineEditor->sizeHint().height() + controlsLayout->spacing();
    unsigned                     gapWidth   = 6 * controlsLayout->spacing();

    QWidget* controlsGapWidget1 = new QWidget;
    dock->registerWidget(controlsGapWidget1, "vertical_controls_gap_widget_1");
    controlsGapWidget1->setFixedWidth(gapWidth);
    controlsGapWidget1->setFixedHeight(gapHeight);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_add_rows_dialog")), 0, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_add_columns_dialog")), 0, 1);
    controlsLayout->addWidget(controlsGapWidget1, 0, 2);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_remove_rows_dialog")), 0, 3);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_remove_columns_dialog")), 0, 4);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_remove_table_dialog")), 0, 5);

    QWidget* controlsGapWidget2 = new QWidget;
    dock->registerWidget(controlsGapWidget2, "vertical_controls_gap_widget_2");
    controlsGapWidget2->setFixedWidth(gapWidth);
    controlsGapWidget2->setFixedHeight(gapHeight);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_merge")), 1, 0);
    controlsLayout->addWidget(controlsGapWidget2, 1, 2);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_row_color")), 1, 3);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_column_color")), 1, 4);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_cell_color")), 1, 5);

    QWidget* controlsGapWidget3 = new QWidget;
    dock->registerWidget(controlsGapWidget3, "vertical_controls_gap_widget_3");
    controlsGapWidget3->setFixedWidth(gapWidth);
    controlsGapWidget3->setFixedHeight(gapHeight);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_lines_horizontal")), 2, 3);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_table_lines_vertical")), 2, 4);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(controlsLayout, 6);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}
