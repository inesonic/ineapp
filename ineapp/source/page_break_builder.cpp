/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageBreakBuilder class.
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

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_tool_button.h>
#include <eqt_stacking_layout.h>

#include <ld_capabilities.h>

#include "application.h"
#include "main_window.h"
#include "page_break_view_proxy.h"
#include "page_break_builder.h"

const char PageBreakBuilder::builderName[] = "PageBreakBuilder";

PageBreakBuilder::PageBreakBuilder(QObject* parent):EQt::Builder(parent) {}


PageBreakBuilder::~PageBreakBuilder() {}


unsigned PageBreakBuilder::priority() const {
    return builderPriority;
}


const char* PageBreakBuilder::builderIdentifier() const {
    return builderName;
}


const char* PageBreakBuilder::plugInName() const {
    return "";
}


void PageBreakBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenuAction(
        EQt::Application::icon("insert_page_break"),
        tr("&Insert | Page &Break"),
        Ld::Capabilities::nonTextAnnotations,
        "insert_page_break_action"
    );
    window->setToolTip("insert_page_break_action", tr("Insert a page break"));
    window->setWhatsThis(
        "insert_page_break_action",
        tr("Use this option to insert a page break at the current cursor position.")
    );
}


void PageBreakBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* insertDock = window->dockWidget("insert");

    EQt::StackingLayout* horizontalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("horizontal_annotations_stacking_layout");
    EQt::StackingLayout* verticalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("vertical_annotations_stacking_layout");

    QAction* insertPageBreakAction = window->action("insert_page_break_action");

    horizontalStackingLayout->appendWidget(new EQt::ToolButton(insertPageBreakAction));
    verticalStackingLayout->appendWidget(new EQt::ToolButton(insertPageBreakAction));
}


void PageBreakBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* insertPageBreakAction = mainWindow->action("insert_page_break_action");
    bool success = mainWindow->setCommand(tr("pagebreak"), insertPageBreakAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* PageBreakBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new PageBreakViewProxy(view, window);
}
