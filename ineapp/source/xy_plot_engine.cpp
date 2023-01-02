/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref XyPlotEngine class.
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
#include <QLineSeries>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QLegend>
#include <QPainter>
#include <QPainterPath>
#include <QImage>

#include <eqt_chart_item.h>
#include <eqt_graphics_item_group.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "plot_2d_engine_base.h"
#include "xy_plot_presentation_data.h"
#include "xy_plot_engine.h"

const XyPlotEngine XyPlotEngine::instance;

XyPlotEngine::XyPlotEngine() {}


XyPlotEngine::~XyPlotEngine() {}


QString XyPlotEngine::name() const {
    return QString("xy");
}


QString XyPlotEngine::shortDescription() const {
    return tr("XY");
}


QString XyPlotEngine::toolTipDescription() const {
    return tr("X-Y plotting style, draws scatter plots, curves, and lines.");
}


QString XyPlotEngine::whatsThisDescription() const {
    return tr(
        "You can use this button to select the X-Y plotting style.  You can use this plot style to create "
        "scatter plots, curves, and lines."
    );
}


QIcon XyPlotEngine::icon() const {
    return QIcon(":plot_xy_64.png");
}


PlotEngine::PlotSeriesMode XyPlotEngine::supportedPlotSeriesMode() const {
    return PlotSeriesMode::LINES_AND_MARKERS;
}


QSet<PlotEngine::SplineType> XyPlotEngine::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE
                              << SplineType::LINE
                              << SplineType::SPLINE
                              << SplineType::LINEAR_REGRESSION;
}


unsigned XyPlotEngine::dataSourcesPerSeries() const {
    return numberDataSourcesPerSeries;
}


PlotEngine::AxisLocationStyle XyPlotEngine::axisLocationStyle() const {
    return AxisLocationStyle::PLOT_BORDERS;
}


QSet<PlotEngine::AxisScale> XyPlotEngine::supportedAxisScales(PlotEngine::AxisLocation /* axisLocation */) const {
    return QSet<AxisScale>() << AxisScale::LINEAR << AxisScale::LOG << AxisScale::DB_10_LOG << AxisScale::DB_20_LOG;
}


PlotPresentationData* XyPlotEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    EQt::ChartItem*         chartItem        = new EQt::ChartItem;
    XyPlotPresentationData* presentationData = new XyPlotPresentationData(chartItem);

    QSizeF sizeSceneUnits = toScene(QSizeF(plotFormat->chartWidth(), plotFormat->chartHeight()));
    chartItem->setGeometry(0, 0, sizeSceneUnits.width(), sizeSceneUnits.height());

    dynamic_cast<EQt::GraphicsItemGroup*>(presentationData->graphicsItem())->setForcedGeometry(
        0, 0,
        sizeSceneUnits.width(), sizeSceneUnits.height()
    );

    configureChart(chartItem, presentationData, plotElement, plotFormat);

    return presentationData;
}
