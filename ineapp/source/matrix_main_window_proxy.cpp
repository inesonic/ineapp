/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QGridLayout>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_tool_button.h>
#include <eqt_combo_box.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_progress_bar.h>

#include "main_window.h"
#include "application.h"
#include "console_device.h"
#include "matrix_builder.h"
#include "matrix_main_window_proxy.h"

MatrixMainWindowProxy::MatrixMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


MatrixMainWindowProxy::~MatrixMainWindowProxy() {}


void MatrixMainWindowProxy::bind() {
    connect(Application::instance(), &Application::screenResized, this, &MatrixMainWindowProxy::screenResized);
}


void MatrixMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(MatrixBuilder::matrixDockName);

    QGridLayout* horizontalButtonsLayout = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_buttons");
    QGridLayout* verticalButtonsLayout   = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_buttons");
    QSize        toolButtonSize          = EQt::ToolButton::sizePixels(-2);

    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              2 * horizontalButtonsLayout->spacing()
            + 3 * toolButtonSize.height()
            + horizontalButtonsLayout->contentsMargins().top()
            + horizontalButtonsLayout->contentsMargins().bottom()
        ),
        dock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned minimumWidthVerticalOrientation = (
        7 * verticalButtonsLayout->spacing()
        + 8 * toolButtonSize.width()
        + verticalButtonsLayout->contentsMargins().left()
        + verticalButtonsLayout->contentsMargins().right()
    );

    dock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    dock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void MatrixMainWindowProxy::screenResized() {
    windowConfigured();
}
