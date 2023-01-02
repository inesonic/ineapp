/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HeatChartEngine class.
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
#include <QXYSeries>
#include <QScatterSeries>
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
#include "plot_2d_engine_base.h"
#include "heat_chart_presentation_data.h"
#include "heat_chart_engine.h"

const HeatChartEngine HeatChartEngine::instance;

HeatChartEngine::HeatChartEngine() {}


HeatChartEngine::~HeatChartEngine() {}


QString HeatChartEngine::name() const {
    return QString("heat");
}


QString HeatChartEngine::shortDescription() const {
    return tr("Heat Chart");
}


QString HeatChartEngine::toolTipDescription() const {
    return tr("Heat Chart, draws heat charts representing the contents of a matrix.");
}


QString HeatChartEngine::whatsThisDescription() const {
    return tr("You can use this button to select a heat chart plotting style.");
}


QIcon HeatChartEngine::icon() const {
    return QIcon(":plot_heat_chart_64.png");
}


PlotEngine::PlotSeriesMode HeatChartEngine::supportedPlotSeriesMode() const {
    return PlotSeriesMode::GRADIENT;
}


QSet<PlotEngine::SplineType> HeatChartEngine::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE;
}


unsigned HeatChartEngine::dataSourcesPerSeries() const {
    return numberDataSourcesPerSeries;
}


PlotEngine::AxisLocationStyle HeatChartEngine::axisLocationStyle() const {
    return AxisLocationStyle::FIXED;
}


QSet<PlotEngine::AxisScale> HeatChartEngine::supportedAxisScales(PlotEngine::AxisLocation /* axisLocation */) const {
    return QSet<AxisScale>() << AxisScale::LINEAR;
}


HeatChartEngine::ScalingControlsMode HeatChartEngine::supportedScalingControlsMode() const {
    return ScalingControlsMode::MANUAL_SCALING_CONTROLS;
}


unsigned HeatChartEngine::maximumNumberAllowedDataSeries() const {
    return 1;
}


bool HeatChartEngine::activeAreaBackgroundAdjustable() const {
    return false;
}


PlotPresentationData* HeatChartEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    EQt::ChartItem*            chartItem        = new EQt::ChartItem;
    HeatChartPresentationData* presentationData = new HeatChartPresentationData(chartItem);

    QSizeF sizeSceneUnits = toScene(QSizeF(plotFormat->chartWidth(), plotFormat->chartHeight()));
    chartItem->setGeometry(0, 0, sizeSceneUnits.width(), sizeSceneUnits.height());

    dynamic_cast<EQt::GraphicsItemGroup*>(presentationData->graphicsItem())->setForcedGeometry(
        0, 0,
        sizeSceneUnits.width(), sizeSceneUnits.height()
    );

    configureChart(chartItem, presentationData, plotElement, plotFormat);

    return presentationData;
}


void HeatChartEngine::configureChart(
        QChart*                         chartItem,
        HeatChartPresentationData*      presentationData,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    PlotWrappedEngineBase::configureChart(chartItem, plotElement, plotFormat);

    PlotWrappedEngineBase::configureLegend(
        chartItem->legend(),
        *plotFormat,
        true
    );

    const Ld::ChartLineStyle activeAreaBorderLineStyle = plotFormat->drawingAreaOutlineStyle();

    Ld::ChartAxisFormat bottomAxisFormat = plotFormat->axisFormat(AxisLocation::BOTTOM_X_A_GM);
    QValueAxis*         bottomAxis       = new QValueAxis;
    QString             bottomAxisTitle  = plotElement->axisTitle(AxisLocation::BOTTOM_X_A_GM);

    chartItem->addAxis(bottomAxis, Qt::AlignmentFlag::AlignBottom);
    PlotWrappedEngineBase::configureAxisHelper(
        bottomAxis,
        bottomAxisFormat,
        bottomAxisTitle,
        activeAreaBorderLineStyle
    );

    Ld::ChartAxisFormat leftAxisFormat = plotFormat->axisFormat(AxisLocation::LEFT_Y_R_RC);
    QValueAxis*         leftAxis       = new QValueAxis;
    QString             leftAxisTitle  = plotElement->axisTitle(AxisLocation::LEFT_Y_R_RC);

    chartItem->addAxis(leftAxis, Qt::AlignmentFlag::AlignLeft);
    PlotWrappedEngineBase::configureAxisHelper(
        leftAxis,
        leftAxisFormat,
        leftAxisTitle,
        activeAreaBorderLineStyle
    );

    presentationData->setSeries(
        plotElement->dataSource(0, 0),
        plotElement->legendTitle(0),
        plotFormat->plotSeries(0).lineColor(),
        plotFormat->plotSeries(0).gradientType()
    );

    presentationData->setLeftAxisFormat(leftAxis, leftAxisFormat);
    presentationData->setBottomAxisFormat(bottomAxis, bottomAxisFormat);
}
