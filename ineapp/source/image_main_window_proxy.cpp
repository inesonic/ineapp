/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImageMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QSettings>
#include <QFormLayout>
#include <QVBoxLayout>

#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_tool_button.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include <eqt_dimension_line_edit.h>
#include <eqt_spin_box.h>

#include "application.h"
#include "main_window.h"
#include "image_builder.h"
#include "image_main_window_proxy.h"

ImageMainWindowProxy::ImageMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


ImageMainWindowProxy::~ImageMainWindowProxy() {}


void ImageMainWindowProxy::loadSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(ImageBuilder::imageFormatDockName);

    loadSetting("horizontal_width_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("horizontal_height_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);

    loadSetting("vertical_width_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("vertical_height_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);

    settings->endGroup();;
}


void ImageMainWindowProxy::saveSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(ImageBuilder::imageFormatDockName);

    saveSetting("horizontal_width_line_edit");
    saveSetting("horizontal_height_line_edit");

    saveSetting("vertical_width_line_edit");
    saveSetting("vertical_height_line_edit");

    settings->endGroup();;
}


void ImageMainWindowProxy::bind() {
    connect(Application::instance(), &Application::screenResized, this, &ImageMainWindowProxy::screenResized);
}


void ImageMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(ImageBuilder::imageFormatDockName);

    QVBoxLayout* horizontalButtonsLayout = dock->EQt::ProgrammaticWidget::layout<QVBoxLayout>("horizontal_buttons");
    QSize        toolButtonSize          = EQt::ToolButton::sizePixels();

    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              horizontalButtonsLayout->spacing()
            + 2 * toolButtonSize.height()
            + horizontalButtonsLayout->contentsMargins().top()
            + horizontalButtonsLayout->contentsMargins().bottom()
        ),
        dock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    QHBoxLayout* verticalHorizontalLayout =
        dock->EQt::ProgrammaticWidget::layout<QHBoxLayout>("vertical_horizontal");

    QFormLayout* verticalScalingLayout =
        dock->EQt::ProgrammaticWidget::layout<QFormLayout>("vertical_scaling");

    QHBoxLayout* verticalHeightScalingModeLayout =
        dock->EQt::ProgrammaticWidget::layout<QHBoxLayout>("vertical_height_scaling_controls");

    EQt::DimensionLineEdit* imageHeightLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("vertical_height_line_edit");

    // The EQt::DimensionLineEdit widget is greedy in terms of the space it will take so we size the dialog based
    // on a reasonable space for the text within the widget.

    unsigned minimumWidthVerticalOrientation = (
          verticalHorizontalLayout->spacing()
        + toolButtonSize.width()
        + verticalScalingLayout->horizontalSpacing()
        + QFontMetrics(Application::font()).horizontalAdvance(tr("Height: "))
        + QFontMetrics(imageHeightLineEdit->font()).horizontalAdvance(tr("MMMMMM"))
        + verticalHeightScalingModeLayout->spacing()
        + toolButtonSize.width()
        + verticalHorizontalLayout->contentsMargins().left()
        + verticalHorizontalLayout->contentsMargins().right()
    );

    dock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    dock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void ImageMainWindowProxy::screenResized() {
    windowConfigured();
}


void ImageMainWindowProxy::loadSetting(const QString& widgetName, EQt::DimensionLineEdit::Unit defaultValue) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(ImageMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(ImageBuilder::imageFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->loadSettings(settings, widgetName, defaultValue);
}


void ImageMainWindowProxy::saveSetting(const QString& widgetName) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(ImageMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(ImageBuilder::imageFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->saveSettings(settings, widgetName);
}
