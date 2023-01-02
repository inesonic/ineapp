/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageFormatMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QSettings>
#include <QFormLayout>
#include <QGridLayout>
#include <QFontMetrics>

#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include <eqt_dimension_line_edit.h>
#include <eqt_tool_button.h>

#include "application.h"
#include "main_window.h"
#include "page_format_builder.h"
#include "page_format_main_window_proxy.h"

PageFormatMainWindowProxy::PageFormatMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


PageFormatMainWindowProxy::~PageFormatMainWindowProxy() {}


void PageFormatMainWindowProxy::loadSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(PageFormatBuilder::pageFormatDockName);

    loadSetting("horizontal_left_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("horizontal_right_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("horizontal_top_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("horizontal_bottom_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);

    loadSetting("vertical_left_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("vertical_right_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("vertical_top_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    loadSetting("vertical_bottom_margin_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);

    settings->endGroup();;
}


void PageFormatMainWindowProxy::saveSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(PageFormatBuilder::pageFormatDockName);

    saveSetting("horizontal_left_margin_line_edit");
    saveSetting("horizontal_right_margin_line_edit");
    saveSetting("horizontal_top_margin_line_edit");
    saveSetting("horizontal_bottom_margin_line_edit");

    saveSetting("vertical_left_margin_line_edit");
    saveSetting("vertical_right_margin_line_edit");
    saveSetting("vertical_top_margin_line_edit");
    saveSetting("vertical_bottom_margin_line_edit");

    settings->endGroup();;
}


void PageFormatMainWindowProxy::bind() {
    connect(Application::instance(), &Application::screenResized, this, &PageFormatMainWindowProxy::screenResized);
}


void PageFormatMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* dock = window()->dockWidget(PageFormatBuilder::pageFormatDockName);

    QGridLayout* horizControlsLayout   = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_controls");
    QFormLayout* horizPageMarginLayout = dock->EQt::ProgrammaticWidget::layout<QFormLayout>("horizontal_margins");

    EQt::DimensionLineEdit* horizontalPageLeftMarginLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("horizontal_left_margin_line_edit");

    QGridLayout* vertControlsLayout   = dock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_controls");
    QFormLayout* vertPageMarginLayout = dock->EQt::ProgrammaticWidget::layout<QFormLayout>("vertical_margins");

    EQt::DimensionLineEdit* verticalPageLeftMarginLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("vertical_left_margin_line_edit");

    QSize toolButtonSize = EQt::ToolButton::sizePixels();

    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
           3 * horizPageMarginLayout->verticalSpacing()
         + 3 * horizontalPageLeftMarginLineEdit->height()
         + horizControlsLayout->contentsMargins().top()
         + horizControlsLayout->contentsMargins().bottom()
        ),
        dock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    // The EQt::DimensionLineEdit widget is greedy in terms of the space it will take so we size the dialog based
    // on a reasonable space for the text within the widget.

    unsigned minimumWidthVerticalOrientation = (
          vertPageMarginLayout->horizontalSpacing()
        + vertPageMarginLayout->labelForField(verticalPageLeftMarginLineEdit)->width()
        + QFontMetrics(verticalPageLeftMarginLineEdit->font()).horizontalAdvance("MMMMMMM")
        + vertControlsLayout->spacing()
        + 2 * toolButtonSize.width()
        + vertControlsLayout->contentsMargins().left()
        + vertControlsLayout->contentsMargins().right()
    );

    dock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    dock->setMinimumDockWidth(minimumWidthVerticalOrientation);
}


void PageFormatMainWindowProxy::screenResized() {
    windowConfigured();
}


void PageFormatMainWindowProxy::loadSetting(const QString& widgetName, EQt::DimensionLineEdit::Unit defaultValue) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(PageFormatMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(PageFormatBuilder::pageFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->loadSettings(settings, widgetName, defaultValue);
}


void PageFormatMainWindowProxy::saveSetting(const QString& widgetName) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(PageFormatMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(PageFormatBuilder::pageFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->saveSettings(settings, widgetName);
}
