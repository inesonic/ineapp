/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref OperatorFormatMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QTimer>
#include <QStatusBar>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>
#include <QGridLayout>
//#include <QFormLayout>
//#include <QFont>
//#include <QFontMetrics>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_tool_button.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_progress_bar.h>

#include "main_window.h"
#include "application.h"
#include "console_device.h"
#include "operator_format_builder.h"
#include "operator_format_main_window_proxy.h"

OperatorFormatMainWindowProxy::OperatorFormatMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


OperatorFormatMainWindowProxy::~OperatorFormatMainWindowProxy() {}


void OperatorFormatMainWindowProxy::bind() {
    connect(Application::instance(), &Application::screenResized, this, &OperatorFormatMainWindowProxy::screenResized);
}


void OperatorFormatMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(OperatorFormatBuilder::operatorFormatDockName);

    QGridLayout* horizontalButtonsLayout = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_grid");
    QSize        toolButtonSize = EQt::ToolButton::sizePixels();

    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              horizontalButtonsLayout->spacing()
            + 2 * toolButtonSize.height()
            + horizontalButtonsLayout->contentsMargins().top()
            + horizontalButtonsLayout->contentsMargins().bottom()
        ),
        dock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    QGridLayout* verticalButtonsLayout = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_grid");

    unsigned minimumWidthVerticalOrientation = (
          verticalButtonsLayout->horizontalSpacing()
        + 5 * toolButtonSize.width()
        + verticalButtonsLayout->contentsMargins().left()
        + verticalButtonsLayout->contentsMargins().right()
    );

    dock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    dock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void OperatorFormatMainWindowProxy::screenResized() {
    windowConfigured();
}
