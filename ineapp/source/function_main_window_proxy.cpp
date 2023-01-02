/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************f************************************************************************//**
* \file
*
* This file implements the \ref FunctionMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QSettings>
#include <QMessageBox>
#include <QUrl>
#include <QFont>
#include <QFontMetrics>

#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include <ld_function_data.h>

#include "application.h"
#include "application_settings.h"
#include "desktop_services.h"
#include "main_window.h"
#include "function_dock_widget.h"
#include "function_builder.h"
#include "function_main_window_proxy.h"

FunctionMainWindowProxy::FunctionMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


FunctionMainWindowProxy::~FunctionMainWindowProxy() {}


void FunctionMainWindowProxy::bind() {
    MainWindow*                  window             = dynamic_cast<MainWindow*>(FunctionMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dockWidget         = window->dockWidget(FunctionBuilder::functionDockName);
    FunctionDockWidget*          functionDockWidget = dynamic_cast<FunctionDockWidget*>(dockWidget);

    connect(functionDockWidget, &FunctionDockWidget::helpRequested, this, &FunctionMainWindowProxy::helpRequested);
    connect(Application::instance(), &Application::screenResized, this, &FunctionMainWindowProxy::screenResized);
}


void FunctionMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(FunctionBuilder::functionDockName);

    QFontMetrics fontMetrics(Application::font());

    unsigned minimumHeightHorizontalOrientation = dock->minimumRecommendedHorizontalOrientationTitleBarHeight();
    unsigned minimumWidthVerticalOrientation    = fontMetrics.horizontalAdvance(tr("FunctionMMMMMMM"));

    dock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    dock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void FunctionMainWindowProxy::screenResized() {
    windowConfigured();
}


void FunctionMainWindowProxy::helpRequested(const Ld::FunctionData& functionData) {
    if (functionData.isValid()) {
        QString functionKeyword = functionData.helpData();
        bool success = DesktopServices::openFunctionReference(functionKeyword);
        if (!success) {
            QMessageBox::warning(
                window(),
                tr("Unable to provide help"),
                tr("Could not launch web browser.")
            );
        }
    }
}
