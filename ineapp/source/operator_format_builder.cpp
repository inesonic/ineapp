/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref OperatorFormatBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QImageReader>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_tool_button.h>

#include <ld_capabilities.h>

#include "application.h"
#include "main_window.h"
#include "operator_format_main_window_proxy.h"
#include "operator_format_view_proxy.h"
#include "operator_format_builder.h"

const char OperatorFormatBuilder::builderName[] = "OperatorFormatBuilder";
const char OperatorFormatBuilder::operatorFormatDockName[] = "operator_format";

OperatorFormatBuilder::OperatorFormatBuilder(QObject* parent):EQt::Builder(parent) {}


OperatorFormatBuilder::~OperatorFormatBuilder() {}


unsigned OperatorFormatBuilder::priority() const {
    return builderPriority;
}


const char* OperatorFormatBuilder::builderIdentifier() const {
    return builderName;
}


const char* OperatorFormatBuilder::plugInName() const {
    return "";
}


void OperatorFormatBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* operatorFormatDialogAction = window->addMenuAction(
        EQt::Application::icon("format_operator"),
        tr("F&ormat | &Operator..."),
        "operator_format_dialog"
    );
    operatorFormatDialogAction->setToolTip(tr("Modify surrounding parenthesis or operator symbol"));
    operatorFormatDialogAction->setWhatsThis(
        tr("You can use this option to modify parenthesis and modify operator symbols.")
    );

    QAction* braceConditionalFormatDialogAction = window->addMenuAction(
        EQt::Application::icon("format_operator_brace_conditional"),
        tr("F&ormat | &Brace Conditional..."),
        "brace_conditional_format_dialog"
    );
    braceConditionalFormatDialogAction->setToolTip(tr("Modify a brace conditional operator"));
    braceConditionalFormatDialogAction->setWhatsThis(
        tr(
            "You can use this option to modify the appearance and number of conditions in a brace conditional "
            "operator."
        )
    );

    QAction* matrixInsertRows = window->addMenuAction(
        EQt::Application::icon("matrix_insert_row"),
        tr("F&ormat | &Matrix | &Insert | &Rows..."),
        "matrix_insert_row_action"
    );
    matrixInsertRows->setToolTip(tr("Insert one or more rows into a matrix"));
    matrixInsertRows->setWhatsThis(
        tr("You can use this option to insert one or more rows into a matrix.")
    );

    QAction* matrixInsertColumns = window->addMenuAction(
        EQt::Application::icon("matrix_insert_column"),
        tr("F&ormat | &Matrix | &Insert | &Columns..."),
        "matrix_insert_column_action"
    );
    matrixInsertColumns->setToolTip(tr("Insert one or more columns into a matrix"));
    matrixInsertColumns->setWhatsThis(
        tr("You can use this option to insert one or more columns into a matrix.")
    );

    QAction* matrixRemoveRow = window->addMenuAction(
        EQt::Application::icon("matrix_remove_row"),
        tr("F&ormat | &Matrix | &Remove | &Row"),
        "matrix_remove_row_action"
    );
    matrixRemoveRow->setToolTip(tr("Remove a single row from from a matrix"));
    matrixRemoveRow->setWhatsThis(
        tr("You can use this option to remove a single row from a matrix.")
    );

    QAction* matrixRemoveColumn = window->addMenuAction(
        EQt::Application::icon("matrix_remove_column"),
        tr("F&ormat | &Matrix | &Remove | &Column"),
        "matrix_remove_column_action"
    );
    matrixRemoveColumn->setToolTip(tr("Remove a single column from from a matrix"));
    matrixRemoveColumn->setWhatsThis(
        tr("You can use this option to remove a single column from a matrix.")
    );
}


void OperatorFormatBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* parenthesisStyleAction = window->addAction(
        EQt::Application::icon("format_operator_parenthesis"),
        tr("Parenthesis Style"),
        "format_parenthesis_style"
    );

    parenthesisStyleAction->setToolTip(tr("Click here to change the parenthesis style"));
    parenthesisStyleAction->setWhatsThis(
        tr("You can use this button to change the parenthesis style.")
    );

    QAction* multiplicationSymbolAction = window->addAction(
        EQt::Application::icon("format_operator_multiplication"),
        tr("Multiplication Symbol"),
        "format_multiplication_symbol"
    );

    multiplicationSymbolAction->setToolTip(tr("Click here to change the symbol used for multiplication operations"));
    multiplicationSymbolAction->setWhatsThis(
        tr("You can use this button to change the symbol shown for multiplication operations.")
    );

    QAction* divisionSymbolAction = window->addAction(
        EQt::Application::icon("format_operator_division"),
        tr("Division Symbol"),
        "format_division_symbol"
    );

    divisionSymbolAction->setToolTip(tr("Click here to change the symbol used for division operations"));
    divisionSymbolAction->setWhatsThis(
        tr("You can use this button to change the symbol shown for division operations.")
    );

    QAction* logicalNotSymbolAction = window->addAction(
        EQt::Application::icon("format_operator_logical_not"),
        tr("Logical Not Symbol"),
        "format_logical_not_symbol"
    );

    logicalNotSymbolAction->setToolTip(tr("Click here to change the symbol used for logical unary not operations"));
    logicalNotSymbolAction->setWhatsThis(
        tr("You can use this button to change the symbol shown for logical not operations.")
    );
}


void OperatorFormatBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        operatorFormatDockName,
        tr("Operator"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION, "page_format", true)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        operatorFormatDockName,
        tr("&View | &Docks"),
        "operator_format_dock_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Operator Format dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Operator Format dock.  You can use the Operator Format "
            "dock to change parenthesis style and symbols used for various operators."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");

    dock->setHorizontalLayout(horizontalLayout);
    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* OperatorFormatBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new OperatorFormatMainWindowProxy(window);
}


void OperatorFormatBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow                 = dynamic_cast<MainWindow*>(window);

    QMenu*   rightClickMenu                     = mainWindow->widget<QMenu>("right_click_menu");
    QAction* operatorFormatDialogAction         = mainWindow->action("operator_format_dialog");
    QAction* braceConditionalFormatDialogAction = mainWindow->action("brace_conditional_format_dialog");
    rightClickMenu->addAction(operatorFormatDialogAction);
    rightClickMenu->addAction(braceConditionalFormatDialogAction);

    QMenu* matrixMenu = rightClickMenu->addMenu(tr("&Matrix"));
    QMenu* matrixInsertMenu = matrixMenu->addMenu("&Insert");
    QMenu* matrixRemoveMenu = matrixMenu->addMenu("&Remove");

    QAction* matrixInsertRows = window->action("matrix_insert_row_action");
    QAction* matrixInsertColumns = window->action("matrix_insert_column_action");
    QAction* matrixRemoveRow = window->action("matrix_remove_row_action");
    QAction* matrixRemoveColumn = window->action("matrix_remove_column_action");

    matrixInsertMenu->addAction(matrixInsertRows);
    matrixInsertMenu->addAction(matrixInsertColumns);
    matrixRemoveMenu->addAction(matrixRemoveRow);
    matrixRemoveMenu->addAction(matrixRemoveColumn);

    bool success = mainWindow->setCommand(tr("ofmt"), operatorFormatDialogAction);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("bcfmt"), braceConditionalFormatDialogAction);
    Q_ASSERT(success);

    QAction* operatorFormatDockToggleAction = mainWindow->action("operator_format_dock_toggle_action");
    success = mainWindow->setCommand(tr("odock"), operatorFormatDockToggleAction);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("matir"), matrixInsertRows);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("matic"), matrixInsertColumns);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("matrr"), matrixRemoveRow);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("matrc"), matrixRemoveColumn);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* OperatorFormatBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new OperatorFormatViewProxy(view, window);
}


void OperatorFormatBuilder::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("horizontal_grid");
    horizontalLayout->addLayout(buttonsLayout);

    QAction* multiplicationSymbolAction = window->action("format_multiplication_symbol");
    EQt::ToolButton* multiplicationSymbolToolButton = new EQt::ToolButton(multiplicationSymbolAction);
    buttonsLayout->addWidget(multiplicationSymbolToolButton, 0, 0);
    window->registerWidget(multiplicationSymbolToolButton, "horizontal_format_multiplication_symbol_tool_button");

    QAction* divisionSymbolAction = window->action("format_division_symbol");
    EQt::ToolButton* divisionSymbolToolButton = new EQt::ToolButton(divisionSymbolAction);
    buttonsLayout->addWidget(divisionSymbolToolButton, 0, 1);
    window->registerWidget(divisionSymbolToolButton, "horizontal_format_division_symbol_tool_button");

    QAction* logicalNotSymbolAction = window->action("format_logical_not_symbol");
    EQt::ToolButton* logicalNotSymbolToolButton = new EQt::ToolButton(logicalNotSymbolAction);
    buttonsLayout->addWidget(logicalNotSymbolToolButton, 0, 2);
    window->registerWidget(logicalNotSymbolToolButton, "horizontal_format_logical_not_symbol_tool_button");

    QAction* parenthesisStyleAction = window->action("format_parenthesis_style");
    EQt::ToolButton* parenthesisToolButton = new EQt::ToolButton(parenthesisStyleAction);
    buttonsLayout->addWidget(parenthesisToolButton, 1, 0);
    window->registerWidget(parenthesisToolButton, "horizontal_format_parenthesis_tool_button");

    QAction* braceConditionalFormatDialogAction = window->action("brace_conditional_format_dialog");
    EQt::ToolButton* braceConditionalFormatDialogToolButton = new EQt::ToolButton(braceConditionalFormatDialogAction);
    buttonsLayout->addWidget(braceConditionalFormatDialogToolButton, 1, 1);
    window->registerWidget(
        braceConditionalFormatDialogToolButton,
        "horizontal_format_brace_conditional_dialog_tool_button"
    );

    QAction* operatorFormatDialogAction = window->action("operator_format_dialog");
    EQt::ToolButton* operatorFormatDialogToolButton = new EQt::ToolButton(operatorFormatDialogAction);
    buttonsLayout->addWidget(operatorFormatDialogToolButton, 1, 2);
    window->registerWidget(operatorFormatDialogToolButton, "horizontal_format_operator_dialog_tool_button");

    buttonsLayout->setColumnMinimumWidth(3, 4);

    QAction* matrixInsertRowsAction = window->action("matrix_insert_row_action");
    EQt::ToolButton* matrixInsertRowsButton = new EQt::ToolButton(matrixInsertRowsAction);
    buttonsLayout->addWidget(matrixInsertRowsButton, 0, 4);
    window->registerWidget(matrixInsertRowsButton, "horizontal_matrix_insert_rows_tool_button");

    QAction* matrixInsertColumnsAction = window->action("matrix_insert_column_action");
    EQt::ToolButton* matrixInsertColumnsButton = new EQt::ToolButton(matrixInsertColumnsAction);
    buttonsLayout->addWidget(matrixInsertColumnsButton, 0, 5);
    window->registerWidget(matrixInsertColumnsButton, "horizontal_matrix_insert_columns_tool_button");

    QAction* matrixRemoveRowAction = window->action("matrix_remove_row_action");
    EQt::ToolButton* matrixRemoveRowButton = new EQt::ToolButton(matrixRemoveRowAction);
    buttonsLayout->addWidget(matrixRemoveRowButton, 1, 4);
    window->registerWidget(matrixRemoveRowButton, "horizontal_matrix_remove_row_tool_button");

    QAction* matrixRemoveColumnAction = window->action("matrix_remove_column_action");
    EQt::ToolButton* matrixRemoveColumnButton = new EQt::ToolButton(matrixRemoveColumnAction);
    buttonsLayout->addWidget(matrixRemoveColumnButton, 1, 5);
    window->registerWidget(matrixRemoveColumnButton, "horizontal_matrix_remove_column_tool_button");

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(buttonsLayout, 2);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void OperatorFormatBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("vertical_grid");
    verticalLayout->addLayout(buttonsLayout);

    QAction* multiplicationSymbolAction = window->action("format_multiplication_symbol");
    EQt::ToolButton* multiplicationSymbolToolButton = new EQt::ToolButton(multiplicationSymbolAction);
    buttonsLayout->addWidget(multiplicationSymbolToolButton, 0, 0);
    window->registerWidget(multiplicationSymbolToolButton, "vertical_format_multiplication_symbol_tool_button");

    QAction* divisionSymbolAction = window->action("format_division_symbol");
    EQt::ToolButton* divisionSymbolToolButton = new EQt::ToolButton(divisionSymbolAction);
    buttonsLayout->addWidget(divisionSymbolToolButton, 0, 1);
    window->registerWidget(divisionSymbolToolButton, "vertical_format_division_symbol_tool_button");

    QAction* logicalNotSymbolAction = window->action("format_logical_not_symbol");
    EQt::ToolButton* logicalNotSymbolToolButton = new EQt::ToolButton(logicalNotSymbolAction);
    buttonsLayout->addWidget(logicalNotSymbolToolButton, 0, 2);
    window->registerWidget(logicalNotSymbolToolButton, "vertical_format_logical_not_symbol_tool_button");

    QAction* parenthesisStyleAction = window->action("format_parenthesis_style");
    EQt::ToolButton* parenthesisToolButton = new EQt::ToolButton(parenthesisStyleAction);
    buttonsLayout->addWidget(parenthesisToolButton, 0, 3);
    window->registerWidget(parenthesisToolButton, "vertical_format_parenthesis_tool_button");

    QAction* operatorFormatDialogAction = window->action("operator_format_dialog");
    EQt::ToolButton* operatorFormatDialogToolButton = new EQt::ToolButton(operatorFormatDialogAction);
    buttonsLayout->addWidget(operatorFormatDialogToolButton, 0, 4);
    window->registerWidget(operatorFormatDialogToolButton, "vertical_format_operator_dialog_tool_button");

    QAction* braceConditionalFormatDialogAction = window->action("brace_conditional_format_dialog");
    EQt::ToolButton* braceConditionalFormatDialogToolButton = new EQt::ToolButton(braceConditionalFormatDialogAction);
    buttonsLayout->addWidget(braceConditionalFormatDialogToolButton, 1, 0);
    window->registerWidget(
        braceConditionalFormatDialogToolButton,
        "vertical_format_brace_conditional_dialog_tool_button"
    );

    QAction* matrixInsertRowsAction = window->action("matrix_insert_row_action");
    EQt::ToolButton* matrixInsertRowsButton = new EQt::ToolButton(matrixInsertRowsAction);
    buttonsLayout->addWidget(matrixInsertRowsButton, 1, 1);
    window->registerWidget(matrixInsertRowsButton, "vertical_matrix_insert_rows_tool_button");

    QAction* matrixInsertColumnsAction = window->action("matrix_insert_column_action");
    EQt::ToolButton* matrixInsertColumnsButton = new EQt::ToolButton(matrixInsertColumnsAction);
    buttonsLayout->addWidget(matrixInsertColumnsButton, 1, 2);
    window->registerWidget(matrixInsertColumnsButton, "vertical_matrix_insert_columns_tool_button");

    QAction* matrixRemoveRowAction = window->action("matrix_remove_row_action");
    EQt::ToolButton* matrixRemoveRowButton = new EQt::ToolButton(matrixRemoveRowAction);
    buttonsLayout->addWidget(matrixRemoveRowButton, 1, 3);
    window->registerWidget(matrixRemoveRowButton, "vertical_matrix_remove_row_tool_button");

    QAction* matrixRemoveColumnAction = window->action("matrix_remove_column_action");
    EQt::ToolButton* matrixRemoveColumnButton = new EQt::ToolButton(matrixRemoveColumnAction);
    buttonsLayout->addWidget(matrixRemoveColumnButton, 1, 4);
    window->registerWidget(matrixRemoveColumnButton, "vertical_matrix_remove_column_tool_button");

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(buttonsLayout, 4);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}
