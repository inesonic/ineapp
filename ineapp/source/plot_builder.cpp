/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotBuilder class.
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
#include <eqt_spin_box.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_tool_button.h>
#include <eqt_stacking_layout.h>

#include <ld_capabilities.h>

#include "application.h"
#include "main_window.h"
#include "plot_view_proxy.h"
#include "plot_builder.h"

const char PlotBuilder::builderName[] = "PlotBuilder";

PlotBuilder::PlotBuilder(QObject* parent):EQt::Builder(parent) {}


PlotBuilder::~PlotBuilder() {}


unsigned PlotBuilder::priority() const {
    return builderPriority;
}


const char* PlotBuilder::builderIdentifier() const {
    return builderName;
}


const char* PlotBuilder::plugInName() const {
    return "";
}


void PlotBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenuAction(
        EQt::Application::icon("insert_plot"),
        tr("&Insert | &Plot..."),
        Ld::Capabilities::nonTextAnnotations,
        "insert_plot_dialog"
    );
    window->setToolTip("insert_plot_dialog", tr("Insert a plot or chart"));
    window->setWhatsThis(
        "insert_plot_dialog",
        tr("Use this option to insert an a plot, chart, or program generated image.")
    );

    window->addMenuAction(
        tr("F&ormat | &Plot..."),
        "format_plot_dialog"
    );
    window->setToolTip("format_plot_dialog", tr("Formats a plot or chart"));
    window->setWhatsThis(
        "format_plot_dialog",
        tr("Use this option to modify the appearance of a plot or chart.")
    );
}


void PlotBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* plotWidthAction = new QAction(window);
    window->addAction(plotWidthAction, "plot_width");
    plotWidthAction->setToolTip(tr("Use this to change the width of a plot area."));
    plotWidthAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the width of a plot.  Value can be entered in inches, cm, "
            "mm, or points.  Inches or cm will be assumed, based on your locale, if you do not provide a unit."
        )
    );
}


void PlotBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* insertDock = window->dockWidget("insert");

    EQt::StackingLayout* horizontalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("horizontal_annotations_stacking_layout");
    EQt::StackingLayout* verticalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("vertical_annotations_stacking_layout");

    QAction* insertPlotDialogAction = window->action("insert_plot_dialog");

    horizontalStackingLayout->appendWidget(new EQt::ToolButton(insertPlotDialogAction));
    verticalStackingLayout->appendWidget(new EQt::ToolButton(insertPlotDialogAction));
}


EQt::ProgrammaticMainWindowProxy* PlotBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* /* window */) {
    return Q_NULLPTR;
}


void PlotBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

//    QAction* plotFormatDockToggleAction = mainWindow->action("format_plot_dock_toggle_action");
//    bool success = mainWindow->setCommand(tr("pltdock"), plotFormatDockToggleAction);

    QAction* formatPlotDialogAction = mainWindow->action("format_plot_dialog");
    bool success = mainWindow->setCommand(tr("pltfmt"), formatPlotDialogAction);
    Q_ASSERT(success);

    QAction* insertPlotDialogAction = mainWindow->action("insert_plot_dialog");
    success = mainWindow->setCommand(tr("insplt"), insertPlotDialogAction);
    Q_ASSERT(success);

    QMenu* rightClickMenu = mainWindow->widget<QMenu>("right_click_menu");
    rightClickMenu->addAction(formatPlotDialogAction);
}


EQt::ProgrammaticViewProxy* PlotBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new PlotViewProxy(view, window);
}
