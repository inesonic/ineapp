/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BlockFormatMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QSettings>
#include <QGridLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>

#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_tool_button.h>
#include <eqt_spin_box.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include "application.h"
#include "main_window.h"
#include "block_format_builder_late.h"
#include "block_format_main_window_proxy.h"

BlockFormatMainWindowProxy::BlockFormatMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


BlockFormatMainWindowProxy::~BlockFormatMainWindowProxy() {}


void BlockFormatMainWindowProxy::loadSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(BlockFormatBuilderLate::blockFormatDockName);

    loadSetting("horizontal_left_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("horizontal_right_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("horizontal_top_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("horizontal_bottom_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);

    loadSetting("horizontal_first_line_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);

    loadSetting("vertical_left_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("vertical_right_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("vertical_top_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("vertical_bottom_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);

    loadSetting("vertical_first_line_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);

    settings->endGroup();;
}


void BlockFormatMainWindowProxy::saveSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(BlockFormatBuilderLate::blockFormatDockName);

    saveSetting("horizontal_left_indentation_line_edit");
    saveSetting("horizontal_right_indentation_line_edit");
    saveSetting("horizontal_top_spacing_line_edit");
    saveSetting("horizontal_bottom_spacing_line_edit");

    saveSetting("horizontal_first_line_indentation_line_edit");

    saveSetting("vertical_left_indentation_line_edit");
    saveSetting("vertical_right_indentation_line_edit");
    saveSetting("vertical_top_spacing_line_edit");
    saveSetting("vertical_bottom_spacing_line_edit");

    saveSetting("vertical_first_line_indentation_line_edit");

    settings->endGroup();;
}


void BlockFormatMainWindowProxy::bind() {
    connect(
        Application::instance(),
        &Application::screenResized,
        this,
        &BlockFormatMainWindowProxy::screenResized
    );
}


void BlockFormatMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* blockDock = window()->dockWidget(BlockFormatBuilderLate::blockFormatDockName);
    EQt::ProgrammaticDockWidget* listDock  = window()->dockWidget(BlockFormatBuilderLate::listFormatDockName);

    QFormLayout* horizontalParagraphMarginsLayout =
        blockDock->EQt::ProgrammaticWidget::layout<QFormLayout>("horizontal_margins");

    QGridLayout* verticalTopControlsLayout =
        blockDock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_controls");

    EQt::DimensionLineEdit* horizontalParagraphLeftIndentationLineEdit =
        blockDock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("horizontal_left_indentation_line_edit");

    unsigned blockDockMinimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              3 * horizontalParagraphMarginsLayout->spacing()
            + 2 * horizontalParagraphLeftIndentationLineEdit->height()
            + horizontalParagraphMarginsLayout->contentsMargins().top()
            + horizontalParagraphMarginsLayout->contentsMargins().bottom()
        ),
        blockDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    // Dock width is highly adjustable due to the line edit fields.  We therefore size it based on the tool buttons
    // and let the line edit widths scale accordingly.

    unsigned blockDockMinimumWidthVerticalOrientation = (
          4 * verticalTopControlsLayout->spacing()
        + 5 * EQt::ToolButton::sizePixels().width()
        + verticalTopControlsLayout->contentsMargins().left()
        + verticalTopControlsLayout->contentsMargins().right()
    );

    blockDock->setMinimumDockHeight(blockDockMinimumHeightHorizontalOrientation);
    blockDock->setMinimumDockWidth(blockDockMinimumWidthVerticalOrientation);

    QGridLayout* listHorizontalControlsLayout =
        listDock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_controls");

    QFormLayout* listFieldLayout = listDock->EQt::ProgrammaticWidget::layout<QFormLayout>("vertical_fields");

    EQt::SpinBox* listStartingNumberSpinBox =
        window()->EQt::ProgrammaticWidget::widget<EQt::SpinBox>("vertical_starting_number_spin_box");

    QSize toolButtonSize = EQt::ToolButton::sizePixels();

    unsigned listDockMinimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
              1 * listHorizontalControlsLayout->spacing()
            + 2 * toolButtonSize.height()
            + listHorizontalControlsLayout->contentsMargins().top()
            + listHorizontalControlsLayout->contentsMargins().bottom()
        ),
        listDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned listDockMinimumWidthVerticalOrientation = (
          listHorizontalControlsLayout->spacing()
        + 2 * toolButtonSize.width()
        + listFieldLayout->horizontalSpacing()
        + listFieldLayout->labelForField(listStartingNumberSpinBox)->width()
        + listStartingNumberSpinBox->width()
        + listHorizontalControlsLayout->contentsMargins().left()
        + listHorizontalControlsLayout->contentsMargins().right()
        + listFieldLayout->contentsMargins().left()
    );

    listDock->setMinimumDockHeight(listDockMinimumHeightHorizontalOrientation);
    listDock->setMinimumDockWidth(listDockMinimumWidthVerticalOrientation);
}


void BlockFormatMainWindowProxy::screenResized() {
    windowConfigured();
}


void BlockFormatMainWindowProxy::loadSetting(const QString& widgetName, EQt::DimensionLineEdit::Unit defaultValue) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(BlockFormatMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(BlockFormatBuilderLate::blockFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->loadSettings(settings, widgetName, defaultValue);
}


void BlockFormatMainWindowProxy::saveSetting(const QString& widgetName) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(BlockFormatMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(BlockFormatBuilderLate::blockFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->saveSettings(settings, widgetName);
}
