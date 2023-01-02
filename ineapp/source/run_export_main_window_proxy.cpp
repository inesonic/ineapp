/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RunExportMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QTimer>
#include <QStatusBar>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_tool_button.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_progress_bar.h>

#include "main_window.h"
#include "application.h"
#include "console_device.h"
#include "run_export_builder.h"
#include "run_export_main_window_proxy.h"

RunExportMainWindowProxy::RunExportMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


RunExportMainWindowProxy::~RunExportMainWindowProxy() {}


void RunExportMainWindowProxy::bind() {
    MainWindow* window = dynamic_cast<MainWindow*>(RunExportMainWindowProxy::window());

    QTimer* buildProgressBarTimer = window->object<QTimer>("build_progress_bar_timer");
    QTimer* executeProgressBarTimer = window->object<QTimer>("execute_progress_bar_timer");

    connect(buildProgressBarTimer, &QTimer::timeout, this, &RunExportMainWindowProxy::buildProgressBarTimeout);
    connect(executeProgressBarTimer, &QTimer::timeout, this, &RunExportMainWindowProxy::executeProgressBarTimeout);

    if (Application::debugModeEnabled()) {
        ConsoleDevice* consoleDevice   = Application::consoleDevice();
        QTextEdit*     consoleTextEdit = window->widget<QTextEdit>("console_text_edit");
        connect(consoleDevice, &ConsoleDevice::textAdded, consoleTextEdit, &QTextEdit::append, Qt::QueuedConnection);
    }

    connect(Application::instance(), &Application::screenResized, this, &RunExportMainWindowProxy::screenResized);
}


void RunExportMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* debugDock = window()->dockWidget(RunExportBuilder::debugDockName);

    QGridLayout* hDebugLayout = debugDock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_debug");
    QHBoxLayout* vDebugLayout = debugDock->EQt::ProgrammaticWidget::layout<QHBoxLayout>("vertical_debug");

    QSize toolButtonSize = EQt::ToolButton::sizePixels();
    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
           1 * hDebugLayout->spacing()
         + 2 * toolButtonSize.height()
         + hDebugLayout->contentsMargins().top()
         + hDebugLayout->contentsMargins().bottom()
        ),
        debugDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned minimumWidthVerticalOrientation = (
          3 * vDebugLayout->spacing()
        + 4 * toolButtonSize.width()
        + vDebugLayout->contentsMargins().left()
        + vDebugLayout->contentsMargins().right()
    );

    debugDock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    debugDock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void RunExportMainWindowProxy::buildProgressBarTimeout() {
    MainWindow* window = dynamic_cast<MainWindow*>(RunExportMainWindowProxy::window());

    EQt::ProgressBar* buildProgressBar = window->widget<EQt::ProgressBar>("build_progress_bar");
    QProgressBar*     executeProgressBar = window->widget<QProgressBar>("execute_progress_bar");

    buildProgressBar->hide();
    if (!buildProgressBar->isVisible() && !executeProgressBar->isVisible()) {
        QStatusBar* statusBar = window->widget<QStatusBar>("status_bar");
        statusBar->hide();
    }
}


void RunExportMainWindowProxy::executeProgressBarTimeout() {
    MainWindow* window = dynamic_cast<MainWindow*>(RunExportMainWindowProxy::window());

    EQt::ProgressBar* buildProgressBar = window->widget<EQt::ProgressBar>("build_progress_bar");
    QProgressBar*     executeProgressBar = window->widget<QProgressBar>("execute_progress_bar");
    QLabel*           executeStatusLabel = window->widget<QLabel>("execute_status_label");

    executeProgressBar->hide();
    executeStatusLabel->hide();

    if (!buildProgressBar->isVisible() && !executeProgressBar->isVisible()) {
        QStatusBar* statusBar = window->widget<QStatusBar>("status_bar");
        statusBar->hide();
    }
}


void RunExportMainWindowProxy::screenResized() {
    windowConfigured();
}
