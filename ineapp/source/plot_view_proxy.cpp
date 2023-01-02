/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QAction>
#include <QVariant>
#include <QPageSize>
#include <QList>
#include <QMap>
#include <QSet>
#include <QIODevice>
#include <QFile>
#include <QMessageBox>

#include <util_page_size.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_spin_box.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "insert_element_command.h"
#include "update_format_command.h"
#include "image_file_dialog.h"
#include "document.h"
#include "main_window.h"
#include "view_widget.h"
#include "plot_presentation.h"
#include "plot_builder.h"
#include "plot_wizard.h"
#include "plot_format_dialog.h"
#include "plot_view_proxy.h"

PlotViewProxy::PlotViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {
    currentPlotFormatDialog = Q_NULLPTR;
}


PlotViewProxy::~PlotViewProxy() {}


void PlotViewProxy::bindDynamic() {
    connectSignals();

    ViewWidget* view = PlotViewProxy::view();
    formatsUnderCursorChanged(
        view->aggregationsByCapabilityWithAncestors(),
        view->aggregationsByCapabilityWithoutAncestors()
    );
}


void PlotViewProxy::unbindDynamic() {
    disconnectSignals();
}


void PlotViewProxy::formatsUnderCursorChanged(
        const Ld::AggregationsByCapability& /* aggregationsByCapabilityWithAncestors */,
        const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
    ) {
    currentAggregationsByCapability = aggregationsByCapabilityWithoutAncestors;

    QAction* formatPlotDialogAction = window()->action("format_plot_dialog");

    bool enablePlotFormatDialog;
    if (aggregationsByCapabilityWithoutAncestors.contains(Ld::PlotFormat::formatName) &&
        aggregationsByCapabilityWithoutAncestors.elements().size() == 1                  ) {
        const Ld::PlotFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::PlotFormat>();

        enablePlotFormatDialog = (aggregation.size() == 1);
    } else {
        enablePlotFormatDialog = false;
    }

    formatPlotDialogAction->setEnabled(enablePlotFormatDialog);

    if (currentPlotFormatDialog != Q_NULLPTR) {
        if (enablePlotFormatDialog) {
            updatePlotFormatDialog(currentPlotFormatDialog);
        } else {
            currentPlotFormatDialog->deleteLater();
            currentPlotFormatDialog = Q_NULLPTR;
        }
    }
}


void PlotViewProxy::insertPlotRequested() {
    PlotWizard plotWizard(window());

    int result = plotWizard.exec();
    if (result == PlotWizard::DialogCode::Accepted) {
        QSharedPointer<Ld::PlotFormat> plotFormat = Ld::Format::create(Ld::PlotFormat::formatName)
                                                    .dynamicCast<Ld::PlotFormat>();

        QSharedPointer<Ld::PlotElement> plotElement = Ld::Element::create(Ld::PlotElement::elementName)
                                                      .dynamicCast<Ld::PlotElement>();

        updatePlotElementAndFormat(plotElement, plotFormat, &plotWizard);
        plotElement->setFormat(plotFormat);

        insertElement(plotElement, false, InsertElementCommand::FinalCursorPosition::ELEMENT_WHOLE);
    }
}


void PlotViewProxy::formatDialogRequested() {
    MainWindow* window = PlotViewProxy::window();

    PlotFormatDialog plotFormatDialog(window);

    plotFormatDialog.loadSettings(QString("PlotFormatDialog"));
    updatePlotFormatDialog(&plotFormatDialog);

    currentPlotFormatDialog = &plotFormatDialog;
    connect(&plotFormatDialog, &PlotFormatDialog::accepted, this, &PlotViewProxy::plotFormatDialogApplyUpdates);

    int result = plotFormatDialog.exec();
    currentPlotFormatDialog = Q_NULLPTR;

    if (result == PlotFormatDialog::Accepted) {
        plotFormatDialogApplyUpdates(&plotFormatDialog);
    }

    currentPlotFormatDialog = Q_NULLPTR;
}


void PlotViewProxy::plotFormatDialogApplyUpdates(PlotFormatDialog* dialog) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::PlotFormat::formatName));

    const Ld::PlotFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::PlotFormat>();

    Q_ASSERT(aggregation.size() == 1);
    Ld::ElementPointer element = aggregation.begin()->toStrongRef();

    QSharedPointer<Ld::PlotElement> plotElement = element->clone(false).dynamicCast<Ld::PlotElement>();
    QSharedPointer<Ld::PlotFormat>  plotFormat  = plotElement->format()->clone().dynamicCast<Ld::PlotFormat>();

    bool updateElement = updatePlotElementAndFormat(plotElement, plotFormat, dialog);

    if (updateElement) {
        plotElement->setFormat(plotFormat);
        insertElement(plotElement, true, InsertElementCommand::FinalCursorPosition::ELEMENT_WHOLE);
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;
        formatsByElement.insert(plotElement, plotFormat);

        issueUpdateFormatCommand(formatsByElement);
    }

    dialog->saveSettings("ImageFormatDialog");
}


void PlotViewProxy::connectSignals() {
    MainWindow* window = PlotViewProxy::window();
    ViewWidget* view   = PlotViewProxy::view();

    sessionConnect(view, &ViewWidget::formatsAtCursorChanged, &PlotViewProxy::formatsUnderCursorChanged);

    QAction* insertPlotDialogAction = window->action("insert_plot_dialog");
    sessionConnect(insertPlotDialogAction, &QAction::triggered, &PlotViewProxy::insertPlotRequested);

    QAction* formatPlotDialogAction = window->action("format_plot_dialog");
    sessionConnect(formatPlotDialogAction, &QAction::triggered, &PlotViewProxy::formatDialogRequested);
}


void PlotViewProxy::disconnectSignals() {
    sessionDisconnectAll();
}


void PlotViewProxy::updatePlotFormatDialog(PlotFormatDialog* plotFormatDialog) {
    disconnectSignals();

    Q_ASSERT(currentAggregationsByCapability.contains(Ld::PlotFormat::formatName));

    const Ld::PlotFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::PlotFormat>();

    Q_ASSERT(aggregation.size() == 1);
    Ld::ElementPointer element = aggregation.begin()->toStrongRef();

    QSharedPointer<Ld::PlotElement> plotElement = element.dynamicCast<Ld::PlotElement>();
    QSharedPointer<Ld::PlotFormat>  plotFormat  = plotElement->format().dynamicCast<Ld::PlotFormat>();

    QString plotType = plotFormat->chartType();
    const PlotEngine* plotEngine = PlotEngine::byName(plotType);

    if (plotEngine != Q_NULLPTR) {
        unsigned numberDataSeries               = plotElement->numberDataSeries();
        unsigned numberDataSourcesPerDataSeries = plotEngine->dataSourcesPerSeries();

        plotFormatDialog->setPlotStyle(plotEngine);
        plotFormatDialog->setTitleText(plotElement->plotTitle());
        // NOTE: If we ever had a sub-title, we would deal with it here.

        Ld::FontFormat titleFontFormat = plotFormat->titleFontFormat();
        plotFormatDialog->setTitleFont(titleFontFormat.toQFont());
        plotFormatDialog->setTitleFontColor(titleFontFormat.fontColor());
        plotFormatDialog->setTitleFontBackgroundColor(titleFontFormat.fontBackgroundColor());

        const Ld::ChartLineStyle& borderLineStyle = plotFormat->borderLineStyle();
        plotFormatDialog->setBorderLineStyle(borderLineStyle.lineStyle());
        plotFormatDialog->setBorderLineWidth(borderLineStyle.lineWidth());
        plotFormatDialog->setBorderLineColor(borderLineStyle.lineColor());

        plotFormatDialog->setBackgroundColor(plotFormat->plotAreaBackgroundColor());

        const Ld::ChartLineStyle& activeAreaBorderLineStyle = plotFormat->drawingAreaOutlineStyle();
        plotFormatDialog->setActiveAreaBorderLineStyle(activeAreaBorderLineStyle.lineStyle());
        plotFormatDialog->setActiveAreaBorderLineWidth(activeAreaBorderLineStyle.lineWidth());
        plotFormatDialog->setActiveAreaBorderLineColor(activeAreaBorderLineStyle.lineColor());

        plotFormatDialog->setPlotAreaColor(plotFormat->plotDrawingAreaBackgroundColor());

        Ld::FontFormat legendFontFormat = plotFormat->legendFontFormat();
        plotFormatDialog->setLegendFont(legendFontFormat.toQFont());
        plotFormatDialog->setLegendFontColor(legendFontFormat.fontColor());
        plotFormatDialog->setLegendFontBackgroundColor(legendFontFormat.fontBackgroundColor());

        plotFormatDialog->setLegendLocation(plotFormat->legendLocation());

        const Ld::ChartLineStyle& legendBorderLineStyle = plotFormat->legendBorderlineStyle();
        plotFormatDialog->setLegendLineStyle(legendBorderLineStyle.lineStyle());
        plotFormatDialog->setLegendLineWidth(legendBorderLineStyle.lineWidth());
        plotFormatDialog->setLegendLineColor(legendBorderLineStyle.lineColor());

        plotFormatDialog->setLegendBackgroundColor(plotFormat->legendBackgroundColor());

        plotFormatDialog->setPlotWidth(plotFormat->chartWidth());
        plotFormatDialog->setPlotHeight(plotFormat->chartHeight());

        // TODO: When we support margins, update the plot dialog here.

        QList<Ld::PlotFormat::AxisLocation> axisLocations = plotFormat->definedAxisLocations();
        for (  QList<Ld::PlotFormat::AxisLocation>::const_iterator
                   axisLocationIterator    = axisLocations.constBegin(),
                   axisLocationEndIterator = axisLocations.constEnd()
             ; axisLocationIterator != axisLocationEndIterator
             ; ++axisLocationIterator
            ) {
            Ld::PlotFormat::AxisLocation axisLocation = *axisLocationIterator;
            Ld::ChartAxisFormat          axisFormat   = plotFormat->axisFormat(axisLocation);

            Ld::ChartAxisFormat::AxisScale axisScale            = axisFormat.axisScale();
            Ld::ChartAxisFormat::TickStyle majorTickStyle       = axisFormat.majorTickStyle();
            const Ld::ChartLineStyle&      majorGridLineStyle   = axisFormat.majorTickLineStyle();
            Ld::ChartAxisFormat::TickStyle minorTickStyle       = axisFormat.minorTickStyle();
            const Ld::ChartLineStyle&      minorGridLineStyle   = axisFormat.minorTickLineStyle();
            Ld::FontFormat                 axisTitleFontFormat  = axisFormat.titleFont();
            Ld::FontFormat                 axisNumberFontFormat = axisFormat.numberFont();
            double                         minimumValue         = axisFormat.minimumValue();
            double                         maximumValue         = axisFormat.maximumValue();
            double                         majorStepSize        = axisFormat.majorStepSize();
            double                         minorStepSize        = axisFormat.minorStepSize();
            const QString&                 axisTitle            = plotElement->axisTitle(axisLocation);

            plotFormatDialog->setAxisScale(axisLocation, axisScale);

            plotFormatDialog->setAxisTitle(axisLocation, axisTitle);
            plotFormatDialog->setTitleFont(axisLocation, axisTitleFontFormat.toQFont());
            plotFormatDialog->setTitleFontColor(axisLocation, axisTitleFontFormat.fontColor());
            plotFormatDialog->setTitleFontBackgroundColor(axisLocation, axisTitleFontFormat.fontBackgroundColor());

            plotFormatDialog->setNumberFont(axisLocation, axisNumberFontFormat.toQFont());
            plotFormatDialog->setNumberFontColor(axisLocation, axisNumberFontFormat.fontColor());
            plotFormatDialog->setNumberFontBackgroundColor(axisLocation, axisNumberFontFormat.fontBackgroundColor());

            plotFormatDialog->setMinimumValue(axisLocation, minimumValue);
            plotFormatDialog->setMaximumValue(axisLocation, maximumValue);

            plotFormatDialog->setMajorTickStyle(axisLocation, majorTickStyle);
            plotFormatDialog->setMajorGridLineStyle(axisLocation, majorGridLineStyle.lineStyle());
            plotFormatDialog->setMajorGridLineWidth(axisLocation, majorGridLineStyle.lineWidth());
            plotFormatDialog->setMajorGridLineColor(axisLocation, majorGridLineStyle.lineColor());
            plotFormatDialog->setMajorStepSize(axisLocation, majorStepSize);

            plotFormatDialog->setMinorTickStyle(axisLocation, minorTickStyle);
            plotFormatDialog->setMinorGridLineStyle(axisLocation, minorGridLineStyle.lineStyle());
            plotFormatDialog->setMinorGridLineWidth(axisLocation, minorGridLineStyle.lineWidth());
            plotFormatDialog->setMinorGridLineColor(axisLocation, minorGridLineStyle.lineColor());
            plotFormatDialog->setMinorStepSize(axisLocation, minorStepSize);
        }

        plotFormatDialog->setFirstDataSeriesIsDataLabels(plotFormat->firstSeriesIsDataLabels());
        plotFormatDialog->setBaselineAxis(plotFormat->baselineAxis());

        for (unsigned dataSeries=0 ; dataSeries<numberDataSeries ; ++dataSeries) {
            Ld::PlotSeries plotSeries = plotFormat->plotSeries(dataSeries);

            for (unsigned dataSource=0 ; dataSource<numberDataSourcesPerDataSeries ; ++dataSource) {
                Ld::VariableName              variableName = plotElement->dataSource(dataSeries, dataSource);
                Ld::PlotElement::AxisLocation axisLocation = plotElement->axisLocationForSource(
                    dataSeries,
                    dataSource
                );

                plotFormatDialog->setDataSource(dataSeries, dataSource, variableName);
                plotFormatDialog->setAxisLocation(dataSeries, dataSource, axisLocation);
            }

            plotFormatDialog->setPlotSeries(dataSeries, plotSeries);

            QString seriesLabel = plotElement->legendTitle(dataSeries);
            plotFormatDialog->setPlotSeriesLabel(dataSeries, seriesLabel);
        }
    }

    connectSignals();
}


bool PlotViewProxy::updatePlotElementAndFormat(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat,
        const PlotSettingsWidget*       plotSettingsWidget
    ) {
    bool     elementUpdated                 = false;
    unsigned numberDataSeries               = plotSettingsWidget->numberDataSeries();
    unsigned numberDataSourcesPerDataSeries = plotSettingsWidget->dataSourcesPerDataSeries();

    QString titleText = plotSettingsWidget->titleText();
    if (plotElement->plotTitle() != titleText) {
        plotElement->setPlotTitle(plotSettingsWidget->titleText());
        elementUpdated = true;
    }
    // NOTE: If we ever had a sub-title, we would add it here.

    plotElement->clearAxisTitles();
    plotElement->clearSeriesData();

    plotFormat->clearAxisFormat();
    plotFormat->clearPlotSeries();

    QList<Ld::PlotFormat::AxisLocation> axisLocations = plotSettingsWidget->configuredAxisLocations();
    for (  QList<Ld::PlotFormat::AxisLocation>::const_iterator
               axisLocationIterator    = axisLocations.constBegin(),
               axisLocationEndIterator = axisLocations.constEnd()
         ; axisLocationIterator != axisLocationEndIterator
         ; ++axisLocationIterator
        ) {
        Ld::PlotFormat::AxisLocation axisLocation = *axisLocationIterator;

        Ld::ChartLineStyle majorGridLineStyle(
            plotSettingsWidget->majorGridLineStyle(axisLocation),
            plotSettingsWidget->majorGridLineWidth(axisLocation),
            plotSettingsWidget->majorGridLineColor(axisLocation)
        );

        Ld::ChartLineStyle minorGridLineStyle(
            plotSettingsWidget->minorGridLineStyle(axisLocation),
            plotSettingsWidget->minorGridLineWidth(axisLocation),
            plotSettingsWidget->minorGridLineColor(axisLocation)
        );

        QFont axisTitleFont  = plotSettingsWidget->titleFont(axisLocation);
        QFont axisNumberFont = plotSettingsWidget->numberFont(axisLocation);

        Ld::FontFormat axisTitleFontFormat(
            axisTitleFont.family(),
            axisTitleFont.pointSize(),
            static_cast<Ld::FontFormat::Weight>(axisTitleFont.weight()),
            axisTitleFont.italic()
        );
        axisTitleFontFormat.setUnderline(axisTitleFont.underline());
        axisTitleFontFormat.setUnderline(axisTitleFont.overline());
        axisTitleFontFormat.setStrikeout(axisTitleFont.strikeOut());
        axisTitleFontFormat.setLetterSpacing((axisTitleFont.letterSpacing() / 100.0) - 1.0);
        axisTitleFontFormat.setFontColor(plotSettingsWidget->titleFontColor(axisLocation));
        axisTitleFontFormat.setFontBackgroundColor(plotSettingsWidget->titleFontBackgroundColor(axisLocation));

        Ld::FontFormat axisNumberFontFormat(
            axisNumberFont.family(),
            axisNumberFont.pointSize(),
            static_cast<Ld::FontFormat::Weight>(axisNumberFont.weight()),
            axisNumberFont.italic()
        );
        axisNumberFontFormat.setUnderline(axisNumberFont.underline());
        axisNumberFontFormat.setUnderline(axisNumberFont.overline());
        axisNumberFontFormat.setStrikeout(axisNumberFont.strikeOut());
        axisNumberFontFormat.setLetterSpacing((axisNumberFont.letterSpacing() / 100.0) - 1.0);
        axisNumberFontFormat.setFontColor(plotSettingsWidget->titleFontColor(axisLocation));
        axisNumberFontFormat.setFontBackgroundColor(plotSettingsWidget->titleFontBackgroundColor(axisLocation));

        Ld::ChartAxisFormat chartAxisFormat(
            plotSettingsWidget->axisScale(axisLocation),
            plotSettingsWidget->minimumValue(axisLocation),
            plotSettingsWidget->maximumValue(axisLocation),
            plotSettingsWidget->majorStepSize(axisLocation),
            plotSettingsWidget->minorStepSize(axisLocation),
            plotSettingsWidget->majorTickStyle(axisLocation),
            plotSettingsWidget->minorTickStyle(axisLocation),
            majorGridLineStyle,
            minorGridLineStyle,
            axisTitleFontFormat,
            axisNumberFontFormat
        );

        plotFormat->setAxisFormat(axisLocation, chartAxisFormat);

        QString axisTitle = plotSettingsWidget->axisTitle(axisLocation);
        if (plotElement->axisTitle(axisLocation) != axisTitle) {
            plotElement->setPlotAxisTitle(axisLocation, plotSettingsWidget->axisTitle(axisLocation));
            elementUpdated = true;
        }
    }

    for (unsigned dataSeries=0 ; dataSeries<numberDataSeries ; ++dataSeries) {
        Ld::PlotSeries plotSeries = plotSettingsWidget->plotSeries(dataSeries);

        for (unsigned dataSource=0 ; dataSource<numberDataSourcesPerDataSeries ; ++dataSource) {
            Ld::VariableName              variableName = plotSettingsWidget->dataSource(dataSeries, dataSource);
            Ld::PlotElement::AxisLocation axisLocation = plotSettingsWidget->axisLocation(dataSeries, dataSource);

            if (plotElement->dataSource(dataSeries, dataSource) != variableName            ||
                plotElement->axisLocationForSource(dataSeries, dataSource) != axisLocation    ) {
                plotElement->setDataSource(dataSeries, dataSource, axisLocation, variableName);
                elementUpdated = true;
            }
        }

        plotFormat->setPlotSeries(dataSeries, plotSeries);

        QString seriesLabel = plotSettingsWidget->seriesLabel(dataSeries);
        if (plotElement->legendTitle(dataSeries) != seriesLabel) {
            plotElement->setLegendTitle(dataSeries, plotSettingsWidget->seriesLabel(dataSeries));
            elementUpdated = true;
        }
    }

    QFont titleFont = plotSettingsWidget->titleFont();
    Ld::FontFormat titleFontFormat(
        titleFont.family(),
        titleFont.pointSize(),
        static_cast<Ld::FontFormat::Weight>(titleFont.weight()),
        titleFont.italic()
    );
    titleFontFormat.setUnderline(titleFont.underline());
    titleFontFormat.setOverline(titleFont.overline());
    titleFontFormat.setStrikeout(titleFont.strikeOut());
    titleFontFormat.setLetterSpacing((titleFont.letterSpacing() / 100.0) - 1.0);
    titleFontFormat.setFontColor(plotSettingsWidget->titleFontColor());
    titleFontFormat.setFontBackgroundColor(plotSettingsWidget->titleFontBackgroundColor());

    Ld::ChartLineStyle drawingAreaOutlineStyle(
        plotSettingsWidget->activeAreaBorderLineStyle(),
        plotSettingsWidget->activeAreaBorderLineWidth(),
        plotSettingsWidget->activeAreaBorderLineColor()
    );

    Ld::ChartLineStyle borderLineStyle(
        plotSettingsWidget->borderLineStyle(),
        plotSettingsWidget->borderLineWidth(),
        plotSettingsWidget->borderLineColor()
    );

    QFont legendFont = plotSettingsWidget->legendFont();
    Ld::FontFormat legendFontFormat(
        legendFont.family(),
        legendFont.pointSize(),
        static_cast<Ld::FontFormat::Weight>(legendFont.weight()),
        legendFont.italic()
    );
    legendFontFormat.setUnderline(legendFont.underline());
    legendFontFormat.setOverline(legendFont.overline());
    legendFontFormat.setStrikeout(legendFont.strikeOut());
    legendFontFormat.setLetterSpacing((legendFont.letterSpacing() / 100.0) - 1.0);
    legendFontFormat.setFontColor(plotSettingsWidget->legendFontColor());
    legendFontFormat.setFontBackgroundColor(plotSettingsWidget->legendFontBackgroundColor());

    Ld::ChartLineStyle legendBorderLineStyle(
        plotSettingsWidget->legendLineStyle(),
        plotSettingsWidget->legendLineWidth(),
        plotSettingsWidget->legendLineColor()
    );

    plotFormat->setBaselineAxis(plotSettingsWidget->baselineAxis());
    plotFormat->setFirstSeriesIsDataLabels(plotSettingsWidget->firstDataSeriesIsDataLabels());
    plotFormat->setPlotAreaLeftMargin(0);
    plotFormat->setPlotAreaRightMargin(0);
    plotFormat->setPlotAreaTopMargin(0);
    plotFormat->setPlotAreaBottomMargin(0);
    plotFormat->setChartWidth(plotSettingsWidget->plotWidth());
    plotFormat->setChartHeight(plotSettingsWidget->plotHeight());
    plotFormat->setPlotAreaBackgroundColor(plotSettingsWidget->backgroundColor());
    plotFormat->setPlotDrawingAreaBackgroundColor(plotSettingsWidget->activeAreaColor());
    plotFormat->setTitleFontFormat(titleFontFormat);
    plotFormat->setLegendLocation(plotSettingsWidget->legendLocation());
    plotFormat->setLegendFontFormat(legendFontFormat);
    plotFormat->setLegendBorderLineStyle(legendBorderLineStyle);
    plotFormat->setLegendBackgroundColor(plotSettingsWidget->legendBackgroundColor());
    plotFormat->setDrawingAreaOutlineStyle(drawingAreaOutlineStyle);
    plotFormat->setChartType(plotSettingsWidget->plotStyle()->name());
    plotFormat->setBorderLineStyle(borderLineStyle);

    return elementUpdated;
}


void PlotViewProxy::insertElement(
        Ld::ElementPointer                        newElement,
        bool                                      replaceElement,
        InsertElementCommand::FinalCursorPosition finalCursorPosition
    ) {
    InsertElementCommand* newCommand = new InsertElementCommand(
        newElement,
          replaceElement
        ? InsertElementCommand::InsertionLocation::REPLACE_ELEMENT
        : InsertElementCommand::InsertionLocation::AT_CURSOR,
        finalCursorPosition
    );

    view()->insertCommand(newCommand);
}


void PlotViewProxy::issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement) {
    ViewWidget* view = PlotViewProxy::view();
    view->insertCommand(new UpdateFormatCommand(formatsByElement));
}


void PlotViewProxy::restoreFocus() {
    view()->restoreFocus();
}
