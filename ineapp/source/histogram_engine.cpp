/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HistogramEngine class.
***********************************************************************************************************************/

#include <QCoreApplication>
#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QStringList>
#include <QPen>
#include <QBrush>
#include <QAbstractAxis>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QXYSeries>
#include <QLineSeries>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QLegend>
#include <QPainter>
#include <QPainterPath>
#include <QImage>

#include <eqt_charts.h>
#include <eqt_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "histogram_presentation_data.h"
#include "plot_engine.h"
#include "plot_2d_categorized_engine_base.h"
#include "histogram_engine.h"

const HistogramEngine HistogramEngine::instance;

HistogramEngine::HistogramEngine() {}


HistogramEngine::~HistogramEngine() {}


QString HistogramEngine::name() const {
    return QString("histogram");
}


QString HistogramEngine::shortDescription() const {
    return tr("Histogram");
}


QString HistogramEngine::toolTipDescription() const {
    return tr("Histogram plot style.");
}


QString HistogramEngine::whatsThisDescription() const {
    return tr("You can use this button to select a histogram chart.");
}


QIcon HistogramEngine::icon() const {
    return QIcon(":plot_histogram_64.png");
}


HistogramEngine::PlotSeriesMode HistogramEngine::supportedPlotSeriesMode() const {
    return PlotSeriesMode::COLOR_REGION;
}


QSet<HistogramEngine::SplineType> HistogramEngine::supportedSplineTypes() const {
    return QSet<SplineType>();
}


unsigned HistogramEngine::dataSourcesPerSeries() const {
    return 1;
}


HistogramEngine::AxisLocationStyle HistogramEngine::axisLocationStyle() const {
    return AxisLocationStyle::FIXED;
}


QSet<HistogramEngine::AxisScale> HistogramEngine::supportedAxisScales(AxisLocation /* axisLocation */) const {
    return QSet<AxisScale>() << AxisScale::LINEAR << AxisScale::DB_10_LOG << AxisScale::DB_20_LOG;
}


HistogramEngine::ScalingControlsMode HistogramEngine::supportedScalingControlsMode() const {
    return ScalingControlsMode::ALL_SCALING_CONTROLS;
}


unsigned HistogramEngine::maximumNumberAllowedDataSeries() const {
    return 1;
}


PlotPresentationData* HistogramEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    EQt::ChartItem*            chartItem        = new EQt::ChartItem;
    HistogramPresentationData* presentationData = new HistogramPresentationData(chartItem);

    QSizeF sizeSceneUnits = toScene(QSizeF(plotFormat->chartWidth(), plotFormat->chartHeight()));
    chartItem->setGeometry(0, 0, sizeSceneUnits.width(), sizeSceneUnits.height());

    dynamic_cast<EQt::GraphicsItemGroup*>(presentationData->graphicsItem())->setForcedGeometry(
        0, 0,
        sizeSceneUnits.width(), sizeSceneUnits.height()
    );

    configureChart(chartItem, presentationData, plotElement, plotFormat);

    return presentationData;
}


void HistogramEngine::configureChart(
        QChart*                         chartItem,
        HistogramPresentationData*      presentationData,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    PlotWrappedEngineBase::configureChart(chartItem, plotElement, plotFormat);

    const Ld::ChartLineStyle activeAreaBorderLineStyle = plotFormat->drawingAreaOutlineStyle();
    AxisLocation             baselineAxisLocation      = plotFormat->baselineAxis();
    Ld::ChartAxisFormat      baselineAxisFormat        = plotFormat->axisFormat(baselineAxisLocation);
    QAbstractAxis*           baselineAxis              = configureAxis(
        baselineAxisFormat,
        plotElement->axisTitle(baselineAxisLocation),
        activeAreaBorderLineStyle
    );

    presentationData->setAxisFormat(baselineAxisLocation, baselineAxisFormat, baselineAxis);
    chartItem->addAxis(baselineAxis, qtAlignmentForAxis(baselineAxisLocation));

    presentationData->setBaselineAxisLocation(baselineAxisLocation);

    AxisLocation      scaleAxisLocation;
    Qt::AlignmentFlag scaleAxisAlignment;

    if (baselineAxisLocation == AxisLocation::BOTTOM_X_A_GM) {
        scaleAxisLocation  = AxisLocation::LEFT_Y_R_RC;
        scaleAxisAlignment = Qt::AlignmentFlag::AlignLeft;
    } else {
        Q_ASSERT(baselineAxisLocation == AxisLocation::LEFT_Y_R_RC);

        scaleAxisLocation  = AxisLocation::BOTTOM_X_A_GM;
        scaleAxisAlignment = Qt::AlignmentFlag::AlignBottom;
    }

    Ld::ChartAxisFormat scaleAxisFormat = plotFormat->axisFormat(scaleAxisLocation);
    QAbstractAxis*      scaleAxis       = configureAxis(
        scaleAxisFormat,
        plotElement->axisTitle(scaleAxisLocation),
        activeAreaBorderLineStyle
    );

    chartItem->addAxis(scaleAxis, scaleAxisAlignment);
    presentationData->setAxisFormat(scaleAxisLocation, scaleAxisFormat, scaleAxis);

    configureLegend(chartItem->legend(), *plotFormat, false);

    QString errorReason = configureSeries(*plotElement, *plotFormat, presentationData);

    if (!errorReason.isEmpty()) {
        presentationData->showErrorMessage(errorReason);
    } else {
        presentationData->clearErrorMessage();
    }
}


QString HistogramEngine::configureSeries(
        const Ld::PlotElement&     plotElement,
        const Ld::PlotFormat&      plotFormat,
        HistogramPresentationData* presentationData
    ) {
    Q_ASSERT(numberDataSourcesPerSeries == 1);

    QString  errorReason;

    unsigned numberDataSeries = plotElement.numberDataSeries();

    if (numberDataSeries == 1) {
        // This style of plot supports exactly 1 data series with exactly 1 data source.

        AxisLocation          baselineAxisLocation = plotFormat.baselineAxis();
        Ld::VariableName      dataSourceName       = plotElement.dataSource(0, 0);
        QString               seriesLegendTitle    = plotElement.legendTitle(0);
        const Ld::PlotSeries& plotSeries           = plotFormat.plotSeries(0);
        AxisScale             axisScale            = plotFormat.axisFormat(baselineAxisLocation).axisScale();

        errorReason = presentationData->setSeries(
            dataSourceName,
            axisScale,
            plotSeries,
            seriesLegendTitle
        );
    } else {
        presentationData->clearSeries();
        errorReason = tr("Insufficient number data series for plot.");
    }

    return errorReason;
}
