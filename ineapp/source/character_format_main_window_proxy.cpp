/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CharacterFormatMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QSettings>
#include <QGridLayout>

#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_tool_button.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include "application.h"
#include "main_window.h"
#include "character_format_builder_late.h"
#include "character_format_main_window_proxy.h"

CharacterFormatMainWindowProxy::CharacterFormatMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


CharacterFormatMainWindowProxy::~CharacterFormatMainWindowProxy() {}


void CharacterFormatMainWindowProxy::loadSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(CharacterFormatBuilderLate::characterFormatDockName);

    settings->endGroup();;
}


void CharacterFormatMainWindowProxy::saveSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(CharacterFormatBuilderLate::characterFormatDockName);

    settings->endGroup();;
}


void CharacterFormatMainWindowProxy::bind() {
    connect(
        Application::instance(),
        &Application::screenResized,
        this,
        &CharacterFormatMainWindowProxy::screenResized
    );
}


void CharacterFormatMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(CharacterFormatBuilderLate::characterFormatDockName);

    QGridLayout* hControlsLayout = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_controls");
    QGridLayout* vControlsLayout = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_controls");

    QSize toolButtonSize = EQt::ToolButton::sizePixels();

    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
           1 * hControlsLayout->spacing()
         + 2 * toolButtonSize.height()
         + hControlsLayout->contentsMargins().top()
         + hControlsLayout->contentsMargins().bottom()
        ),
        dock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned minimumWidthVerticalOrientation = (
          4 * vControlsLayout->spacing()
        + 5 * toolButtonSize.width()
        + vControlsLayout->contentsMargins().left()
        + vControlsLayout->contentsMargins().right()
    );

    dock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    dock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void CharacterFormatMainWindowProxy::screenResized() {
    windowConfigured();
}
