/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FunctionDockWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QString>
#include <QAction>
#include <QSize>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeView>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QMenu>

#include <eqt_tool_button.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>

#include <ld_function_data.h>

#include "application.h"
#include "function_browser_model.h"
#include "function_browser_delegate.h"
#include "function_dock_widget.h"

FunctionDockWidget::FunctionDockWidget(
        EQt::ProgrammaticMainWindow* parent,
        Qt::WindowFlags             flags
    ):EQt::ProgrammaticDockWidget(
        tr("Function"),
        parent,
        flags
    ) {
}


FunctionDockWidget::~FunctionDockWidget() {}


void FunctionDockWidget::configureDockWidget(EQt::ProgrammaticMainWindow* mainWindow) {
    QFont functionFont    = Application::mathFont();
    QFont descriptionFont = Application::font();
    FunctionBrowserModel* functionBrowserModel = new FunctionBrowserModel(functionFont, descriptionFont, mainWindow);
    registerObject(functionBrowserModel, "function_browser_model");

    buildHorizontalOrientation(mainWindow, functionBrowserModel);
    buildVerticalOrientation(mainWindow, functionBrowserModel);
}


void FunctionDockWidget::setHeightAdjustment(unsigned heightAdjustment) {
    unsigned minimumHeight = minimumHeightHorizontalOrientation();

    if (heightAdjustment > minimumHeight) {
        setFixedHeight(heightAdjustment + minimumHeight);
    } else {
        setFixedHeight(QWIDGETSIZE_MAX);
        setMinimumHeight(minimumHeight);
    }

    updateGeometry();
}


void FunctionDockWidget::setWidthAdjustment(unsigned widthAdjustment) {
    unsigned minimumWidth = minimumWidthVerticalOrientation();

    if (widthAdjustment > minimumWidth) {
        setFixedWidth(widthAdjustment + minimumWidth);
    } else {
        setFixedWidth(QWIDGETSIZE_MAX);
        setMinimumWidth(minimumWidth);
    }

    updateGeometry();
}


void FunctionDockWidget::requirementsChanged(const Ld::Capabilities& newRequirements) {
    QTreeView*      horizontalBrowserTable                 = EQt::ProgrammaticWidget::widget<QTreeView>(
        "horizontal_browser_tree"
    );
    QLineEdit*       horizontalLineEdit                     = EQt::ProgrammaticWidget::widget<QLineEdit>(
        "horizontal_search_line_edit"
    );
    EQt::ToolButton* horizontalInsertStandardFunctionButton = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "horizontal_insert_standard_function_tool_button"
    );
    EQt::ToolButton* horizontalFunctionHelpButton           = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "horizontal_function_help_tool_button"
    );
    QTreeView*      verticalBrowserTable                   = EQt::ProgrammaticWidget::widget<QTreeView>(
        "vertical_browser_tree"
    );
    QLineEdit*       verticalLineEdit                       = EQt::ProgrammaticWidget::widget<QLineEdit>(
        "vertical_search_line_edit"
    );
    EQt::ToolButton* verticalInsertStandardFunctionButton   = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "vertical_insert_standard_function_tool_button"
    );
    EQt::ToolButton* verticalFunctionHelpButton             = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "vertical_function_help_tool_button"
    );

    FunctionBrowserModel*
        functionBrowserModel = EQt::ProgrammaticWidget::object<FunctionBrowserModel>("function_browser_model");

    horizontalLineEdit->clear();
    verticalLineEdit->clear();

    functionBrowserModel->setCapabilities(newRequirements);
    functionBrowserModel->setSelectionString(QString());
    functionBrowserModel->setSelectedCategories(QList<QString>());

    if (functionBrowserModel->presentedFunctions().isEmpty()) {
        horizontalBrowserTable->setEnabled(false);
        horizontalLineEdit->setEnabled(false);
        horizontalInsertStandardFunctionButton->setEnabled(false);
        horizontalFunctionHelpButton->setEnabled(false);
        verticalBrowserTable->setEnabled(false);
        verticalLineEdit->setEnabled(false);
        verticalInsertStandardFunctionButton->setEnabled(false);
        verticalFunctionHelpButton->setEnabled(false);
    } else {
        horizontalBrowserTable->setEnabled(true);
        horizontalLineEdit->setEnabled(true);
        horizontalInsertStandardFunctionButton->setEnabled(true);
        horizontalFunctionHelpButton->setEnabled(true);
        verticalBrowserTable->setEnabled(true);
        verticalLineEdit->setEnabled(true);
        verticalInsertStandardFunctionButton->setEnabled(true);
        verticalFunctionHelpButton->setEnabled(true);
    }
}


void FunctionDockWidget::topLevelChanged(bool topLevel) {
    EQt::ProgrammaticDockWidget::topLevelChanged(topLevel);

    if (topLevel) {
        setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
}


void FunctionDockWidget::horizontalTableClicked(const QModelIndex& /* index */) {
    QTreeView*          browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("horizontal_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();

    EQt::ToolButton* insertStandardFunctionButton = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "horizontal_insert_standard_function_tool_button"
    );

    EQt::ToolButton* functionHelpButton = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "horizontal_function_help_tool_button"
    );

    if (selectionModel->hasSelection()) {
        insertStandardFunctionButton->setEnabled(true);
        functionHelpButton->setEnabled(true);
    } else {
        insertStandardFunctionButton->setEnabled(false);
        functionHelpButton->setEnabled(false);
    }
}


void FunctionDockWidget::verticalTableClicked(const QModelIndex& /* index */) {
    QTreeView*          browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("vertical_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();

    EQt::ToolButton* insertStandardFunctionButton = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "vertical_insert_standard_function_tool_button"
    );

    EQt::ToolButton* functionHelpButton = EQt::ProgrammaticWidget::widget<EQt::ToolButton>(
        "vertical_function_help_tool_button"
    );

    if (selectionModel->hasSelection()) {
        insertStandardFunctionButton->setEnabled(true);
        functionHelpButton->setEnabled(true);
    } else {
        insertStandardFunctionButton->setEnabled(false);
        functionHelpButton->setEnabled(false);
    }
}


void FunctionDockWidget::horizontalTableDoubleClicked(const QModelIndex& index) {
    QTreeView*          browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("horizontal_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();
    if (selectionModel->hasSelection()) {
        emitFunctionDataEvent(index);
    }
}


void FunctionDockWidget::verticalTableDoubleClicked(const QModelIndex& index) {
    QTreeView*           browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("vertical_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();
    if (selectionModel->hasSelection()) {
        emitFunctionDataEvent(index);
    }
}


void FunctionDockWidget::horizontalInsertStandardFunctionButtonClicked() {
    QTreeView*          browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("horizontal_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();
    if (selectionModel->hasSelection()) {
        QModelIndex modelIndex = selectionModel->currentIndex();
        emitFunctionDataEvent(modelIndex);
    }
}


void FunctionDockWidget::verticalInsertStandardFunctionButtonClicked() {
    QTreeView*          browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("vertical_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();
    if (selectionModel->hasSelection()) {
        QModelIndex modelIndex = selectionModel->currentIndex();
        emitFunctionDataEvent(modelIndex);
    }
}


void FunctionDockWidget::horizontalFunctionHelpButtonClicked() {
    QTreeView*          browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("horizontal_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();
    if (selectionModel->hasSelection()) {
        QModelIndex modelIndex = selectionModel->currentIndex();
        displayFunctionHelp(modelIndex);
    }
}


void FunctionDockWidget::verticalFunctionHelpButtonClicked() {
    QTreeView*          browserTree   = EQt::ProgrammaticWidget::widget<QTreeView>("vertical_browser_tree");
    QItemSelectionModel* selectionModel = browserTree->selectionModel();
    if (selectionModel->hasSelection()) {
        QModelIndex modelIndex = selectionModel->currentIndex();
        displayFunctionHelp(modelIndex);
    }
}


void FunctionDockWidget::horizontalLineEditorEdited(const QString& newText) {
    QLineEdit* verticalSearchLineEdit = EQt::ProgrammaticWidget::widget<QLineEdit>("vertical_search_line_edit");
    verticalSearchLineEdit->setText(newText);

    FunctionBrowserModel*
        functionBrowserModel = EQt::ProgrammaticWidget::object<FunctionBrowserModel>("function_browser_model");

    functionBrowserModel->setSelectionString(newText);
}


void FunctionDockWidget::verticalLineEditorEdited(const QString& newText) {
    QLineEdit* horizontalSearchLineEdit = EQt::ProgrammaticWidget::widget<QLineEdit>("horizontal_search_line_edit");
    horizontalSearchLineEdit->setText(newText);

    FunctionBrowserModel*
        functionBrowserModel = EQt::ProgrammaticWidget::object<FunctionBrowserModel>("function_browser_model");

    functionBrowserModel->setSelectionString(newText);
}


void FunctionDockWidget::buildHorizontalOrientation(
        EQt::ProgrammaticMainWindow* mainWindow,
        FunctionBrowserModel*        functionBrowserModel
    ) {
    QHBoxLayout* horizontalLayout = newHBoxLayout("horizontal_layout");
    setHorizontalLayout(horizontalLayout);

    QVBoxLayout* leftVerticalLayout = newVBoxLayout("horizontal_left_vertical_layout");
    horizontalLayout->addLayout(leftVerticalLayout, 1);

    QTreeView* browserTree = new QTreeView;
    browserTree->setModel(functionBrowserModel);
    browserTree->header()->hide();
    browserTree->setWordWrap(true);
    browserTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    browserTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    browserTree->setSelectionMode(QTreeView::SingleSelection);
    browserTree->setEditTriggers(QTreeView::NoEditTriggers);
    registerWidget(browserTree, "horizontal_browser_tree");
    leftVerticalLayout->addWidget(browserTree, 1);

    QAbstractItemDelegate* oldDelegate = browserTree->itemDelegate();
    browserTree->setItemDelegate(new FunctionBrowserDelegate(browserTree));
    delete oldDelegate;

    QHBoxLayout* searchLineLayout = newHBoxLayout("horizontal_search_line_layout");
    leftVerticalLayout->addLayout(searchLineLayout);

    QLabel* searchLabel = new QLabel(tr("Search: "));
    registerWidget(searchLabel, "horizontal_search_label");
    searchLineLayout->addWidget(searchLabel);

    QLineEdit* searchLineEdit = new QLineEdit;
    searchLineEdit->setClearButtonEnabled(true);
    registerWidget(searchLineEdit, "horizontal_search_line_edit");
    searchLineLayout->addWidget(searchLineEdit);

    QVBoxLayout* buttonLayout = newVBoxLayout("horizontal_button_layout");
    horizontalLayout->addLayout(buttonLayout);

    QAction* functionOperatorAction = mainWindow->action("function_operator_action");
    EQt::ToolButton* functionOperatorToolButton = new EQt::ToolButton(functionOperatorAction);
    functionOperatorToolButton->setRelativeSizeFactor(-2);
    functionOperatorToolButton->setXScaling(2.0F);
    functionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    registerWidget(functionOperatorToolButton, "horizontal_function_operator_tool_button");
    buttonLayout->addWidget(functionOperatorToolButton);

    buttonLayout->addStretch(1);

    EQt::ToolButton* insertStandardFunctionButton = new EQt::ToolButton;
    insertStandardFunctionButton->setIcon(EQt::Application::icon("insert_standard_function"));
    insertStandardFunctionButton->setRelativeSizeFactor(-2);
    insertStandardFunctionButton->setXScaling(2.0F);
    insertStandardFunctionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    registerWidget(insertStandardFunctionButton, "horizontal_insert_standard_function_tool_button");
    buttonLayout->addWidget(insertStandardFunctionButton);

    insertStandardFunctionButton->setToolTip(tr("Insert the selected standard function"));
    insertStandardFunctionButton->setWhatsThis(
       tr(
           "You can use this button to insert the standard function selected in the table to the left of this button "
           "into your model.  Alternately, you can double click on a function in the table."
       )
    );

    EQt::ToolButton* functionHelpButton = new EQt::ToolButton;
    functionHelpButton->setIcon(EQt::Application::icon("function_help"));
    functionHelpButton->setRelativeSizeFactor(-2);
    functionHelpButton->setXScaling(2.0F);
    functionHelpButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    registerWidget(functionHelpButton, "horizontal_function_help_tool_button");
    buttonLayout->addWidget(functionHelpButton);

    functionHelpButton->setToolTip(tr("Get help on a standard function"));
    functionHelpButton->setWhatsThis(
        tr(
           "You can use this button to obtain help information on the function selected in the table to the left of "
           "this button."
        )
    );

    insertStandardFunctionButton->setEnabled(false);
    functionHelpButton->setEnabled(false);

    connect(browserTree, &QTreeView::clicked, this, &FunctionDockWidget::horizontalTableClicked);
    connect(browserTree, &QTreeView::doubleClicked, this, &FunctionDockWidget::horizontalTableDoubleClicked);
    connect(searchLineEdit, &QLineEdit::textEdited, this, &FunctionDockWidget::horizontalLineEditorEdited);
    connect(
        insertStandardFunctionButton,
        &EQt::ToolButton::clicked,
        this,
        &FunctionDockWidget::horizontalInsertStandardFunctionButtonClicked
    );
    connect(
        functionHelpButton,
        &EQt::ToolButton::clicked,
        this,
        &FunctionDockWidget::horizontalFunctionHelpButtonClicked
    );
}


void FunctionDockWidget::buildVerticalOrientation(
        EQt::ProgrammaticMainWindow* mainWindow,
        FunctionBrowserModel*        functionBrowserModel
    ) {
    QVBoxLayout* verticalLayout = newVBoxLayout("vertical_layout");
    setVerticalLayout(verticalLayout);

    QTreeView* browserTree = new QTreeView;
    browserTree->setModel(functionBrowserModel);
    browserTree->header()->hide();
    browserTree->setWordWrap(true);
    browserTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    browserTree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    browserTree->setSelectionMode(QTreeView::SingleSelection);
    browserTree->setEditTriggers(QTreeView::NoEditTriggers);
    registerWidget(browserTree, "vertical_browser_tree");
    verticalLayout->addWidget(browserTree, 1);

    QAbstractItemDelegate* oldDelegate = browserTree->itemDelegate();
    browserTree->setItemDelegate(new FunctionBrowserDelegate(browserTree));
    delete oldDelegate;

    QHBoxLayout* searchLineLayout = newHBoxLayout("vertical_search_line_layout");
    verticalLayout->addLayout(searchLineLayout);

    QLabel* searchLabel = new QLabel(tr("Search: "));
    registerWidget(searchLabel, "vertical_search_label");
    searchLineLayout->addWidget(searchLabel);

    QLineEdit* searchLineEdit = new QLineEdit;
    searchLineEdit->setClearButtonEnabled(true);
    registerWidget(searchLineEdit, "vertical_search_line_edit");
    searchLineLayout->addWidget(searchLineEdit);

    QHBoxLayout* buttonLayout = newHBoxLayout("vertical_button_layout");
    verticalLayout->addLayout(buttonLayout);

    QAction* functionOperatorAction = mainWindow->action("function_operator_action");
    EQt::ToolButton* functionOperatorToolButton = new EQt::ToolButton(functionOperatorAction);
    functionOperatorToolButton->setRelativeSizeFactor(-2);
    functionOperatorToolButton->setXScaling(2.0F);
    functionOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    registerWidget(functionOperatorToolButton, "vertical_function_operator_tool_button");
    buttonLayout->addWidget(functionOperatorToolButton);

    buttonLayout->addStretch(1);

    EQt::ToolButton* insertStandardFunctionButton = new EQt::ToolButton;
    insertStandardFunctionButton->setIcon(EQt::Application::icon("insert_standard_function"));
    insertStandardFunctionButton->setRelativeSizeFactor(-2);
    insertStandardFunctionButton->setXScaling(2.0F);
    insertStandardFunctionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    registerWidget(insertStandardFunctionButton, "vertical_insert_standard_function_tool_button");
    buttonLayout->addWidget(insertStandardFunctionButton);

    insertStandardFunctionButton->setToolTip(tr("Insert the selected standard function"));
    insertStandardFunctionButton->setWhatsThis(
       tr(
           "You can use this button to insert the standard function selected in the table above this button "
           "into your model.  Alternately, you can double click on a function in the table."
       )
    );

    EQt::ToolButton* functionHelpButton = new EQt::ToolButton;
    functionHelpButton->setIcon(EQt::Application::icon("function_help"));
    functionHelpButton->setRelativeSizeFactor(-2);
    functionHelpButton->setXScaling(2.0F);
    functionHelpButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    registerWidget(functionHelpButton, "vertical_function_help_tool_button");
    buttonLayout->addWidget(functionHelpButton);

    functionHelpButton->setToolTip(tr("Get help on a standard function"));
    functionHelpButton->setWhatsThis(
        tr(
           "You can use this button to obtain help information on the function selected in the table above this "
           "button."
        )
    );

    insertStandardFunctionButton->setEnabled(false);
    functionHelpButton->setEnabled(false);

    connect(browserTree, &QTreeView::clicked, this, &FunctionDockWidget::verticalTableClicked);
    connect(browserTree, &QTreeView::doubleClicked, this, &FunctionDockWidget::verticalTableDoubleClicked);
    connect(searchLineEdit, &QLineEdit::textEdited, this, &FunctionDockWidget::verticalLineEditorEdited);
    connect(
        insertStandardFunctionButton,
        &EQt::ToolButton::clicked,
        this,
        &FunctionDockWidget::verticalInsertStandardFunctionButtonClicked
    );
    connect(
        functionHelpButton,
        &EQt::ToolButton::clicked,
        this,
        &FunctionDockWidget::verticalFunctionHelpButtonClicked
    );
}


void FunctionDockWidget::emitFunctionDataEvent(const QModelIndex& modelIndex) {
    Ld::FunctionData  functionData;
    QStringList       parameterDescriptions;

    FunctionBrowserModel*
        functionBrowserModel = EQt::ProgrammaticWidget::object<FunctionBrowserModel>("function_browser_model");

    if (functionBrowserModel->identifyFunction(modelIndex, functionData, parameterDescriptions)) {
        emit functionSelected(functionData, parameterDescriptions);
    }
}


void FunctionDockWidget::displayFunctionHelp(const QModelIndex& modelIndex) {
    Ld::FunctionData  functionData;
    QStringList       parameterDescriptions;

    FunctionBrowserModel*
        functionBrowserModel = EQt::ProgrammaticWidget::object<FunctionBrowserModel>("function_browser_model");

    if (functionBrowserModel->identifyFunction(modelIndex, functionData, parameterDescriptions)) {
        emit helpRequested(functionData);
    }
}
