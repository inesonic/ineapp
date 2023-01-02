/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFormatMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QSettings>
#include <QGridLayout>
#include <QFormLayout>
#include <QFont>
#include <QFontMetrics>

#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_tool_button.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include "application.h"
#include "main_window.h"
#include "table_format_builder.h"
#include "table_format_main_window_proxy.h"

TableFormatMainWindowProxy::TableFormatMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {}


TableFormatMainWindowProxy::~TableFormatMainWindowProxy() {}


void TableFormatMainWindowProxy::loadSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(TableFormatBuilder::tableFormatDockName);

    loadSetting("horizontal_left_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("horizontal_right_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("horizontal_top_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("horizontal_bottom_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);

    loadSetting("vertical_left_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("vertical_right_indentation_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("vertical_top_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);
    loadSetting("vertical_bottom_spacing_line_edit", EQt::DimensionLineEdit::Unit::POINT);

    settings->endGroup();;
}


void TableFormatMainWindowProxy::saveSettings() {
    QSettings* settings = Application::settings();

    settings->beginGroup(TableFormatBuilder::tableFormatDockName);

    saveSetting("horizontal_left_indentation_line_edit");
    saveSetting("horizontal_right_indentation_line_edit");
    saveSetting("horizontal_top_spacing_line_edit");
    saveSetting("horizontal_bottom_spacing_line_edit");

    saveSetting("vertical_left_indentation_line_edit");
    saveSetting("vertical_right_indentation_line_edit");
    saveSetting("vertical_top_spacing_line_edit");
    saveSetting("vertical_bottom_spacing_line_edit");

    settings->endGroup();;
}


void TableFormatMainWindowProxy::bind() {
    connect(Application::instance(), &Application::screenResized, this, &TableFormatMainWindowProxy::screenResized);
}


void TableFormatMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* tableDock     = window()->dockWidget(TableFormatBuilder::tableFormatDockName);
    EQt::ProgrammaticDockWidget* tableCellDock = window()->dockWidget(TableFormatBuilder::tableCellFormatDockName);

    QGridLayout* horizontalControlsLayout =
        tableDock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_controls");

    QGridLayout* verticalControlsLayout =
        tableDock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_controls");

    QGridLayout* verticalCellControlsLayout =
        tableCellDock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_cell_controls");

    QFormLayout* verticalLeftTableMarginLayout =
        tableDock->EQt::ProgrammaticWidget::layout<QFormLayout>("vertical_left_margins");

    EQt::DimensionLineEdit* verticalTableRightIndentationLineEdit =
        tableDock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>("vertical_right_indentation_line_edit");

    QSize toolButtonSize = EQt::ToolButton::sizePixels();

    QFontMetrics labelFontMetric(Application::font());
    QFontMetrics lineEditFontMetric(verticalTableRightIndentationLineEdit->font());

    unsigned tableMinimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
           1 * horizontalControlsLayout->spacing()
         + 2 * toolButtonSize.height()
         + horizontalControlsLayout->contentsMargins().top()
         + horizontalControlsLayout->contentsMargins().bottom()
        ),
        tableDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned tableCellMinimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
           1 * horizontalControlsLayout->spacing()
         + 2 * toolButtonSize.height()
         + horizontalControlsLayout->contentsMargins().top()
         + horizontalControlsLayout->contentsMargins().bottom()
        ),
        tableCellDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned tableMinimumWidthVerticalOrientation = (
          verticalLeftTableMarginLayout->horizontalSpacing()
        + labelFontMetric.horizontalAdvance(tr("Right:"))
        + lineEditFontMetric.horizontalAdvance(tr("MMMMMMM"))
        + labelFontMetric.horizontalAdvance(tr("Bottom:"))
        + lineEditFontMetric.horizontalAdvance(tr("MMMMMMM"))
        + verticalControlsLayout->contentsMargins().left()
        + verticalControlsLayout->contentsMargins().right()
    );

    unsigned tableCellMinimumWidthVerticalOrientation = (
          5 * verticalCellControlsLayout->spacing()
        + 6 * verticalCellControlsLayout->spacing()
        + 5 * toolButtonSize.width()
        + verticalCellControlsLayout->contentsMargins().left()
        + verticalCellControlsLayout->contentsMargins().right()
    );

    tableDock->setMinimumDockHeight(tableMinimumHeightHorizontalOrientation);
    tableDock->setMinimumDockWidth(tableMinimumWidthVerticalOrientation);

    tableCellDock->setMinimumDockHeight(tableCellMinimumHeightHorizontalOrientation);
    tableCellDock->setMinimumDockWidth(tableCellMinimumWidthVerticalOrientation);
}


void TableFormatMainWindowProxy::screenResized() {
    windowConfigured();
}


void TableFormatMainWindowProxy::loadSetting(const QString& widgetName, EQt::DimensionLineEdit::Unit defaultValue) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(TableFormatMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(TableFormatBuilder::tableFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->loadSettings(settings, widgetName, defaultValue);
}


void TableFormatMainWindowProxy::saveSetting(const QString& widgetName) {
    MainWindow*                  window   = dynamic_cast<MainWindow*>(TableFormatMainWindowProxy::window());
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(TableFormatBuilder::tableFormatDockName);
    QSettings*                   settings = Application::settings();

    EQt::DimensionLineEdit* lineEditor = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(widgetName);
    Q_ASSERT(lineEditor != Q_NULLPTR);

    lineEditor->saveSettings(settings, widgetName);
}
