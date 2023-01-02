/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageFormatBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_paper_size_popup_menu.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_combo_box.h>
#include <eqt_tool_button.h>

#include "main_window.h"
#include "page_format_main_window_proxy.h"
#include "page_format_view_proxy.h"
#include "page_format_builder.h"

const char PageFormatBuilder::builderName[] = "PageFormatBuilder";
const char PageFormatBuilder::pageFormatDockName[] = "page_format";

PageFormatBuilder::PageFormatBuilder(QObject* parent):EQt::Builder(parent) {}


PageFormatBuilder::~PageFormatBuilder() {}


unsigned PageFormatBuilder::priority() const {
    return builderPriority;
}


const char* PageFormatBuilder::builderIdentifier() const {
    return builderName;
}


const char* PageFormatBuilder::plugInName() const {
    return "";
}


void PageFormatBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenuAction(EQt::Application::icon("format_page"), tr("F&ormat | P&age..."), "format_page_dialog");
    window->setToolTip("format_page_dialog", tr("Change the page format"));
    window->setWhatsThis(
        "format_page_dialog",
        tr("Use this option to update or change the page orientation, paper size, and margins.")
    );

    QAction* formatPageSizeAction = new QAction(window);
    window->addAction(formatPageSizeAction, "format_page_size");

    EQt::PaperSizePopupMenu* paperSizePopupMenu = new EQt::PaperSizePopupMenu(formatPageSizeAction, window);
    window->registerWidget(paperSizePopupMenu, "paper_size_popup_menu");

    QMenu* pageOrientationPopupMenu = new QMenu(window);
    window->registerWidget(pageOrientationPopupMenu, "page_orientation_popup_menu");

    QAction* pagePortraitOrientationAction = window->addAction(
        EQt::Application::icon("format_page_orientation_portrait"),
        tr("&Portrait"),
        "portrait_orientation_action"
    );
    pagePortraitOrientationAction->setCheckable(true);
    pagePortraitOrientationAction->setToolTip(tr("Set the page orientation to portrait"));
    pagePortraitOrientationAction->setWhatsThis(
        tr("You can use this option to set the page orientation to portrait.")
    );

    QAction* pageLandscapeOrientationAction = window->addAction(
        EQt::Application::icon("format_page_orientation_landscape"),
        tr("&Landscape"),
        "landscape_orientation_action"
    );
    pageLandscapeOrientationAction->setCheckable(true);
    pageLandscapeOrientationAction->setToolTip(tr("Set the page orientation to landscape"));
    pageLandscapeOrientationAction->setWhatsThis(
        tr("You can use this option to set the page orientation to landscape.")
    );

    pageOrientationPopupMenu->addAction(pagePortraitOrientationAction);
    pageOrientationPopupMenu->addAction(pageLandscapeOrientationAction);
}


void PageFormatBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* pageOrientationAction = new QAction(
        EQt::Application::icon("format_page_orientation_portrait"),
        tr("Change to portrait page orientation"),
        window
    );
    window->addAction(pageOrientationAction, "format_page_orientation");
    pageOrientationAction->setToolTip(tr("Click here to set pages to portrait orientation."));
    pageOrientationAction->setWhatsThis(
        tr("You can use this button to change the page orientation to portrait.")
    );

    QAction* paperSizeButtonAction = new QAction(
        EQt::Application::icon("format_page_size"),
        tr("Set page size"),
        window
    );
    window->addAction(paperSizeButtonAction, "format_page_size_button");
    paperSizeButtonAction->setToolTip(tr("Use this to change the page size."));
    paperSizeButtonAction->setWhatsThis(
        tr(
            "You can use this button to select a new paper size.  Clicking the button will present a popup menu "
            "with a list of standard paper sizes.  You will also be provided an option allowing you to enter a "
            "custom paper size."
          )
    );

    QAction* pageLeftMarginAction = new QAction(window);
    window->addAction(pageLeftMarginAction, "format_page_left_margin");
    pageLeftMarginAction->setToolTip(tr("Use this to change the left side page margin."));
    pageLeftMarginAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the left side margin for the page.  Values can be entered "
            "in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not provide "
            "a unit."
        )
    );

    QAction* pageRightMarginAction = new QAction(window);
    window->addAction(pageRightMarginAction, "format_page_right_margin");
    pageRightMarginAction->setToolTip(tr("Use this to change the right side page margin."));
    pageRightMarginAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the right side margin for the page.  Values can be "
            "entered in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* pageTopMarginAction = new QAction(window);
    window->addAction(pageTopMarginAction, "format_page_top_margin");
    pageTopMarginAction->setToolTip(tr("Use this to change the top side page margin."));
    pageTopMarginAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the top side margin for the page.  Values can be entered "
            "in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not provide "
            "a unit."
        )
    );

    QAction* pageBottomMarginAction = new QAction(window);
    window->addAction(pageBottomMarginAction, "format_page_bottom_margin");
    pageBottomMarginAction->setToolTip(tr("Use this to change the bottom side page margin."));
    pageBottomMarginAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the bottom side margin for the page.  Values can be "
            "entered in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );
}


void PageFormatBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        pageFormatDockName,
        tr("Page"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION, "list_format", false)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        pageFormatDockName,
        tr("&View | &Docks"),
        "page_format_dock_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Page dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Page dock.  You can use the Page dock to set the page "
            "size, page orientation, and page margins."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");
    dock->setHorizontalLayout(horizontalLayout);

    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* PageFormatBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new PageFormatMainWindowProxy(window);
}


void PageFormatBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* pageFormatDockToggleAction = mainWindow->action("page_format_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("pdock"), pageFormatDockToggleAction);

    QAction* formatPageDialogAction = mainWindow->action("format_page_dialog");
    success = mainWindow->setCommand(tr("pfmt"), formatPageDialogAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* PageFormatBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new PageFormatViewProxy(view, window);
}


void PageFormatBuilder::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("horizontal_controls");
    horizontalLayout->addLayout(controlsLayout);

    QMenu*   pageOrientationPopupMenu = window->EQt::ProgrammaticWidget::widget<QMenu>("page_orientation_popup_menu");
    QAction* pageOrientationAction    = window->action("format_page_orientation");

    EQt::ToolButton* pageOrientationButton = new EQt::ToolButton(pageOrientationAction, pageOrientationPopupMenu);
    controlsLayout->addWidget(pageOrientationButton, 0, 0);

    EQt::PaperSizePopupMenu* paperSizePopupMenu = window->widget<EQt::PaperSizePopupMenu>("paper_size_popup_menu");
    QAction* paperSizeButtonAction = window->action("format_page_size_button");
    EQt::ToolButton* paperSizeButton = new EQt::ToolButton(paperSizeButtonAction, paperSizePopupMenu);
    controlsLayout->addWidget(paperSizeButton, 0, 1);

    EQt::ToolButton* formatPageButton = new EQt::ToolButton(window->action("format_page_dialog"));
    controlsLayout->addWidget(formatPageButton, 1, 1);

    QFormLayout* pageMarginLayout = dock->newFormLayout("horizontal_margins");
    horizontalLayout->addLayout(pageMarginLayout);

    EQt::DimensionLineEdit* pageLeftMarginLineEdit   = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* pageRightMarginLineEdit  = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* pageTopMarginLineEdit    = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* pageBottomMarginLineEdit = new EQt::DimensionLineEdit;

    dock->registerWidget(pageLeftMarginLineEdit, "horizontal_left_margin_line_edit");
    dock->registerWidget(pageRightMarginLineEdit, "horizontal_right_margin_line_edit");
    dock->registerWidget(pageTopMarginLineEdit, "horizontal_top_margin_line_edit");
    dock->registerWidget(pageBottomMarginLineEdit, "horizontal_bottom_margin_line_edit");

    pageLeftMarginLineEdit->setDynamicUnits();
    pageRightMarginLineEdit->setDynamicUnits();
    pageTopMarginLineEdit->setDynamicUnits();
    pageBottomMarginLineEdit->setDynamicUnits();

    pageLeftMarginLineEdit->setDefaultAction(window->action("format_page_left_margin"));
    pageRightMarginLineEdit->setDefaultAction(window->action("format_page_right_margin"));
    pageTopMarginLineEdit->setDefaultAction(window->action("format_page_top_margin"));
    pageBottomMarginLineEdit->setDefaultAction(window->action("format_page_bottom_margin"));

    pageMarginLayout->addRow(tr("Top:"), pageTopMarginLineEdit);
    pageMarginLayout->addRow(tr("Left:"), pageLeftMarginLineEdit);
    pageMarginLayout->addRow(tr("Right:"), pageRightMarginLineEdit);
    pageMarginLayout->addRow(tr("Bottom:"), pageBottomMarginLineEdit);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void PageFormatBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("vertical_main");
    verticalLayout->addLayout(horizontalLayout);

    QGridLayout* controlsLayout = dock->newGridLayout("vertical_controls");
    horizontalLayout->addLayout(controlsLayout);

    QMenu*   pageOrientationPopupMenu = window->EQt::ProgrammaticWidget::widget<QMenu>("page_orientation_popup_menu");
    QAction* pageOrientationAction    = window->action("format_page_orientation");

    EQt::ToolButton* pageOrientationButton = new EQt::ToolButton(pageOrientationAction, pageOrientationPopupMenu);
    controlsLayout->addWidget(pageOrientationButton, 0, 0);

    EQt::PaperSizePopupMenu* paperSizePopupMenu = window->widget<EQt::PaperSizePopupMenu>("paper_size_popup_menu");
    QAction* paperSizeButtonAction = window->action("format_page_size_button");
    EQt::ToolButton* paperSizeButton = new EQt::ToolButton(paperSizeButtonAction, paperSizePopupMenu);
    controlsLayout->addWidget(paperSizeButton, 0, 1);

    EQt::ToolButton* formatPageButton = new EQt::ToolButton(window->action("format_page_dialog"));
    controlsLayout->addWidget(formatPageButton, 1, 1);

    QFormLayout* pageMarginLayout = dock->newFormLayout("vertical_margins");
    horizontalLayout->addLayout(pageMarginLayout);

    EQt::DimensionLineEdit* pageLeftMarginLineEdit   = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* pageRightMarginLineEdit  = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* pageTopMarginLineEdit    = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* pageBottomMarginLineEdit = new EQt::DimensionLineEdit;

    dock->registerWidget(pageLeftMarginLineEdit, "vertical_left_margin_line_edit");
    dock->registerWidget(pageRightMarginLineEdit, "vertical_right_margin_line_edit");
    dock->registerWidget(pageTopMarginLineEdit, "vertical_top_margin_line_edit");
    dock->registerWidget(pageBottomMarginLineEdit, "vertical_bottom_margin_line_edit");

    pageLeftMarginLineEdit->setDynamicUnits();
    pageRightMarginLineEdit->setDynamicUnits();
    pageTopMarginLineEdit->setDynamicUnits();
    pageBottomMarginLineEdit->setDynamicUnits();

    pageLeftMarginLineEdit->setDefaultAction(window->action("format_page_left_margin"));
    pageRightMarginLineEdit->setDefaultAction(window->action("format_page_right_margin"));
    pageTopMarginLineEdit->setDefaultAction(window->action("format_page_top_margin"));
    pageBottomMarginLineEdit->setDefaultAction(window->action("format_page_bottom_margin"));

    pageMarginLayout->addRow(tr("Top:"), pageTopMarginLineEdit);
    pageMarginLayout->addRow(tr("Left:"), pageLeftMarginLineEdit);
    pageMarginLayout->addRow(tr("Right:"), pageRightMarginLineEdit);
    pageMarginLayout->addRow(tr("Bottom:"), pageBottomMarginLineEdit);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}
