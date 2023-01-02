/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ValueFieldMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QTimer>
#include <QStatusBar>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFont>
#include <QFontMetrics>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_tool_button.h>
#include <eqt_combo_box.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_progress_bar.h>

#include "main_window.h"
#include "application.h"
#include "console_device.h"
#include "value_field_builder.h"
#include "value_field_main_window_proxy.h"

ValueFieldMainWindowProxy::ValueFieldMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


ValueFieldMainWindowProxy::~ValueFieldMainWindowProxy() {}


void ValueFieldMainWindowProxy::bind() {
    connect(Application::instance(), &Application::screenResized, this, &ValueFieldMainWindowProxy::screenResized);
}


void ValueFieldMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(ValueFieldBuilder::valueFieldFormatDockName);

    QVBoxLayout* horizontalButtonsLayout = dock->EQt::ProgrammaticWidget::layout<QVBoxLayout>("horizontal_buttons");
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

    QFormLayout* verticalIntegerControlsLayout =
        dock->EQt::ProgrammaticWidget::layout<QFormLayout>("vertical_integer_controls_layout");

    QFormLayout* verticalRealComplexControlsLayout =
        dock->EQt::ProgrammaticWidget::layout<QFormLayout>("vertical_real_controls_vertical_layout");

    QFontMetrics labelFontMetrics(Application::font());

    unsigned integerMinimumWidthVerticalOrientation = (
          verticalIntegerControlsLayout->horizontalSpacing()
        + labelFontMetrics.horizontalAdvance(tr("Digits: "))
        + labelFontMetrics.horizontalAdvance(tr("Verilog style prefixMMMMM"))
        + verticalIntegerControlsLayout->contentsMargins().left()
        + verticalIntegerControlsLayout->contentsMargins().right()
    );

    unsigned realMinimumWidthVerticalOrientation = (
          verticalRealComplexControlsLayout->horizontalSpacing()
        + labelFontMetrics.horizontalAdvance(tr("Multiplier: "))
        + labelFontMetrics.horizontalAdvance(tr("Computer scientificMMMMM"))
        + verticalRealComplexControlsLayout->contentsMargins().left()
        + verticalRealComplexControlsLayout->contentsMargins().right()
    );

    unsigned minimumWidthVerticalOrientation = std::max(
        integerMinimumWidthVerticalOrientation,
        realMinimumWidthVerticalOrientation
    );

    dock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    dock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void ValueFieldMainWindowProxy::screenResized() {
    windowConfigured();
}
