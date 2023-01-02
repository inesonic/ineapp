/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.f
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ControlFlowBuilder class.
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

#include <ld_for_all_in_operator_element.h>
#include <ld_while_operator_element.h>
#include <ld_compound_statement_operator_element.h>
#include <ld_if_operator_element.h>
#include <ld_else_if_operator_element.h>
#include <ld_else_operator_element.h>
#include <ld_brace_conditional_operator_element.h>
#include <ld_therefore_operator_element.h>

#include "application.h"
#include "main_window.h"
#include "control_flow_main_window_proxy.h"
#include "control_flow_view_proxy.h"
#include "control_flow_builder.h"

const char ControlFlowBuilder::builderName[] = "ControlFlowBuilder";
const char ControlFlowBuilder::controlFlowDockName[] = "control_flow";

ControlFlowBuilder::ControlFlowBuilder(QObject* parent):EQt::Builder(parent) {}


ControlFlowBuilder::~ControlFlowBuilder() {}


unsigned ControlFlowBuilder::priority() const {
    return builderPriority;
}


const char* ControlFlowBuilder::builderIdentifier() const {
    return builderName;
}


const char* ControlFlowBuilder::plugInName() const {
    return "";
}

#if (defined(Q_OS_DARWIN))

    #define MENU_TEXT(_text,_suffix) ((_text) + (_suffix))

#else

    #define MENU_TEXT(_text,_suffix) (_text)

#endif

void ControlFlowBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* forAllInOperatorAction = window->addMenuAction(
        EQt::Application::icon("for_all_in_operator"),
        MENU_TEXT(tr("&Insert | &Control Flow | &For Each"), tr("  %1 ? %2 ? : ?").arg(QChar(0x2200)).arg(QChar(0x2208))),
        Ld::ForAllInOperatorElement::childProvides,
        "for_all_in_operator_action"
    );
    forAllInOperatorAction->setToolTip(tr("Iterate through sets, tuple, and matrix values"));
    forAllInOperatorAction->setWhatsThis(
        tr(
           "You can use the %1 operator to iterate through the contents of iterable values such as sets, tuples, and "
           "matrices."
          )
    );

    QAction* whileOperatorAction = window->addMenuAction(
        EQt::Application::icon("while_operator"),
        MENU_TEXT(tr("&Insert | &Control Flow | &While"), tr("  while ? : ?")),
        Ld::WhileOperatorElement::childProvides,
        "while_operator_action"
    );
    whileOperatorAction->setToolTip(tr("Iterate while a condition is met"));
    whileOperatorAction->setWhatsThis(
        tr(
           "You can use the \"while\" operator to iterate through one or more operations so long as a specified "
           "condition is true."
          )
    );

    QAction* compoundStatementOperatorAction = window->addMenuAction(
        EQt::Application::icon("compound_statement_operator"),
        MENU_TEXT(
            tr("&Insert | &Control Flow | &Compound Statement"),
            tr("  %1%2").arg(QChar(0x2502)).arg(QChar(0x22EE))
        ),
        Ld::CompoundStatementOperatorElement::childProvides,
        "compound_statement_operator_action"
    );
    compoundStatementOperatorAction->setToolTip(tr("Define a compound statement or block"));
    compoundStatementOperatorAction->setWhatsThis(
        tr(
           "You can use this operator to define a compound or block statement that can contain multiple operations. "
           "compound statements can optionally return a value allowing them to be used to define lambda functions."
          )
    );

    QAction* ifOperatorAction = window->addMenuAction(
        EQt::Application::icon("if_operator"),
        MENU_TEXT(tr("&Insert | &Control Flow | &If Conditional"), tr("  if ? : ?")),
        Ld::IfOperatorElement::childProvides,
        "if_operator_action"
    );
    ifOperatorAction->setToolTip(tr("If conditional operator."));
    ifOperatorAction->setWhatsThis(
        tr(
           "You can use the \"if\" conditional operator to conditionally perform an operation.  Unlike the "
           "conditional operator, \"? %1 ?\", the \"if\" operator does not provide to a true or false value."
          ).arg(0x2192)
    );

    QAction* elseIfOperatorAction = window->addMenuAction(
        EQt::Application::icon("else_if_operator"),
        MENU_TEXT(tr("&Insert | &Control Flow | &Else If Conditional"), tr("  else if ? : ?")),
        Ld::ElseIfOperatorElement::childProvides,
        "else_if_operator_action"
    );
    elseIfOperatorAction->setToolTip(tr("Else-if conditional operator."));
    elseIfOperatorAction->setWhatsThis(
        tr("You can use the \"else if\" conditional operator to conditionally perform an operation.")
    );

    QAction* elseOperatorAction = window->addMenuAction(
        EQt::Application::icon("else_operator"),
        MENU_TEXT(tr("&Insert | &Control Flow | &Else Conditional"), tr("  else: ?")),
        Ld::ElseOperatorElement::childProvides,
        "else_operator_action"
    );
    elseOperatorAction->setToolTip(tr("Else conditional operator."));
    elseOperatorAction->setWhatsThis(
        tr(
            "You can use the \"else\" conditional operator to conditionally perform an operation only if all "
            "previous \"if\" and \"else if\" operations evaluate to false."
        )
    );

    QAction* thereforeOperatorAction = window->addMenuAction(
        EQt::Application::icon("therefore_operator"),
        MENU_TEXT(
            tr("&Insert | &Control Flow | &Therefore"),
            tr("  %1").arg(QChar(0x2234))
        ),
        Ld::ThereforeOperatorElement::childProvides,
        "therefore_operator_action"
    );
    thereforeOperatorAction->setToolTip(tr("Therefore operator specifies the value of a compound statement."));
    thereforeOperatorAction->setWhatsThis(
        tr(
           "You can use the therefore operator to specify the value assigned to a compound statement."
          )
    );

    QAction* braceConditionalOperatorAction = window->addMenuAction(
        EQt::Application::icon("brace_conditional_operator"),
        MENU_TEXT(
            tr("&Insert | &Control Flow | &Brace Conditional"),
            tr("  {%1 %1").arg(QChar(0x22EE))
        ),
        Ld::BraceConditionalOperatorElement::childProvides,
        "brace_conditional_operator_action"
    );
    braceConditionalOperatorAction->setToolTip(tr("Calculate a value based on a set of conditions"));
    braceConditionalOperatorAction->setWhatsThis(
        tr(
           "You can use the brace conditional operator to calculate different values based on one or more "
           "conditions.  The last row represents the \"all else\" case and can include either text or an expression.  "
           "The expression is for notational purposes only and is ignored."
          )
    );
}


void ControlFlowBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        controlFlowDockName,
        tr("Control Flow"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION, "matrix", true)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        controlFlowDockName,
        tr("&View | &Docks"),
        "control_flow_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Control Flow dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Control Flow dock.  You can use the Control Flow dock to "
            "operators used to group operations, conditionally execute statements, and iterate through code."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");

    dock->setHorizontalLayout(horizontalLayout);
    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* ControlFlowBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new ControlFlowMainWindowProxy(window);
}


void ControlFlowBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* forAllInOperatorAction = mainWindow->action("for_all_in_operator_action");
    bool success = mainWindow->setCommand(tr("forall"), forAllInOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("foreach"), forAllInOperatorAction);
    Q_ASSERT(success);

    QAction* whileOperatorAction = mainWindow->action("while_operator_action");
    success = mainWindow->setCommand(tr("while"), whileOperatorAction);
    Q_ASSERT(success);

    QAction* compoundStatementOperatorAction = mainWindow->action("compound_statement_operator_action");
    success = mainWindow->setCommand(tr("|"), compoundStatementOperatorAction);
    Q_ASSERT(success);

    QAction* ifOperatorAction = mainWindow->action("if_operator_action");
    success = mainWindow->setCommand(tr("if"), ifOperatorAction);
    Q_ASSERT(success);

    QAction* elseIfOperatorAction = mainWindow->action("else_if_operator_action");
    success = mainWindow->setCommand(tr("elif"), elseIfOperatorAction);
    Q_ASSERT(success);

    QAction* elseOperatorAction = mainWindow->action("else_operator_action");
    success = mainWindow->setCommand(tr("else"), elseOperatorAction);
    Q_ASSERT(success);

    QAction* thereforeOperatorAction = mainWindow->action("therefore_operator_action");
    success = mainWindow->setCommand(tr("therefore"), thereforeOperatorAction);
    Q_ASSERT(success);

    QAction* braceConditionalOperatorAction = mainWindow->action("brace_conditional_operator_action");
    success = mainWindow->setCommand(tr("cases"), braceConditionalOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("bc"), braceConditionalOperatorAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* ControlFlowBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new ControlFlowViewProxy(view, window);
}


void ControlFlowBuilder::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("horizontal_buttons");
    horizontalLayout->addLayout(buttonsLayout);

    buildDockTopComponents(buttonsLayout, window, 0, 0);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(buttonsLayout, 4);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void ControlFlowBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("vertical_buttons");
    verticalLayout->addLayout(buttonsLayout);

    buildDockTopComponents(buttonsLayout, window, 0, 0);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(buttonsLayout, 4);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void ControlFlowBuilder::buildDockTopComponents(
        QGridLayout*                 gridLayout,
        EQt::ProgrammaticMainWindow* window,
        unsigned                     topOffset,
        unsigned                     leftOffset
    ) {
    QAction* forAllInOperatorAction = window->action("for_all_in_operator_action");
    EQt::ToolButton* forAllInOperatorToolButton = new EQt::ToolButton(forAllInOperatorAction);
    forAllInOperatorToolButton->setRelativeSizeFactor(-2);
    forAllInOperatorToolButton->setXScaling(4.0F);
    forAllInOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(forAllInOperatorToolButton, topOffset + 0, leftOffset + 0, 1, 4);

    QAction* whileOperatorAction = window->action("while_operator_action");
    EQt::ToolButton* whileOperatorToolButton = new EQt::ToolButton(whileOperatorAction);
    whileOperatorToolButton->setRelativeSizeFactor(-2);
    whileOperatorToolButton->setXScaling(4.0F);
    whileOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(whileOperatorToolButton, topOffset + 1, leftOffset + 0, 1, 4);

    QAction* compoundStatementOperatorAction = window->action("compound_statement_operator_action");
    EQt::ToolButton* compoundStatementOperatorToolButton = new EQt::ToolButton(compoundStatementOperatorAction);
    compoundStatementOperatorToolButton->setRelativeSizeFactor(2);
    compoundStatementOperatorToolButton->setXScaling(2.0F/3.0F);
    compoundStatementOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(compoundStatementOperatorToolButton, topOffset + 0, leftOffset + 4, 3, 2);

    QAction* thereforeOperatorAction = window->action("therefore_operator_action");
    EQt::ToolButton* thereforeOperatorToolButton = new EQt::ToolButton(thereforeOperatorAction);
    thereforeOperatorToolButton->setRelativeSizeFactor(-2);
    thereforeOperatorToolButton->setXScaling(2.0F);
    thereforeOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(thereforeOperatorToolButton, topOffset + 0, leftOffset + 6, 1, 1);

    QAction* ifOperatorAction = window->action("if_operator_action");
    EQt::ToolButton* ifOperatorToolButton = new EQt::ToolButton(ifOperatorAction);
    ifOperatorToolButton->setRelativeSizeFactor(-2);
    ifOperatorToolButton->setXScaling(2.0F);
    ifOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(ifOperatorToolButton, topOffset + 1, leftOffset + 6, 1, 2);

    QAction* elseIfOperatorAction = window->action("else_if_operator_action");
    EQt::ToolButton* elseIfOperatorToolButton = new EQt::ToolButton(elseIfOperatorAction);
    elseIfOperatorToolButton->setRelativeSizeFactor(-2);
    elseIfOperatorToolButton->setXScaling(4.0F);
    elseIfOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(elseIfOperatorToolButton, topOffset + 3, leftOffset + 4, 1, 4);

    QAction* elseOperatorAction = window->action("else_operator_action");
    EQt::ToolButton* elseOperatorToolButton = new EQt::ToolButton(elseOperatorAction);
    elseOperatorToolButton->setRelativeSizeFactor(-2);
    elseOperatorToolButton->setXScaling(2.0F);
    elseOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(elseOperatorToolButton, topOffset + 2, leftOffset + 6, 1, 2);

    QAction* braceConditionalOperatorAction = window->action("brace_conditional_operator_action");
    EQt::ToolButton* braceConditionalOperatorToolButton = new EQt::ToolButton(braceConditionalOperatorAction);
    braceConditionalOperatorToolButton->setRelativeSizeFactor(0);
    braceConditionalOperatorToolButton->setXScaling(2.0F);
    braceConditionalOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(braceConditionalOperatorToolButton, topOffset + 2, leftOffset + 0, 2, 4);
}
