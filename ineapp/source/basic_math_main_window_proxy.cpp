/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BasicMathMainWindowProxy class.
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
#include "basic_math_builder.h"
#include "basic_math_main_window_proxy.h"

BasicMathMainWindowProxy::BasicMathMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


BasicMathMainWindowProxy::~BasicMathMainWindowProxy() {}


void BasicMathMainWindowProxy::bind() {
    connect(Application::instance(), &Application::screenResized, this, &BasicMathMainWindowProxy::screenResized);
}


void BasicMathMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(BasicMathBuilder::basicMathDockName);

    QGridLayout* horizontalButtonsLayout = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_buttons");
    QGridLayout* verticalButtonsLayout   = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_buttons");
    QSize        toolButtonSize          = EQt::ToolButton::sizePixels(-2);

    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              3 * horizontalButtonsLayout->spacing()
            + 4 * toolButtonSize.height()
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


void BasicMathMainWindowProxy::screenResized() {
    windowConfigured();
}
