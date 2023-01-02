/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Plot2DCategoryEngineBase class.
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
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "plot_2d_categorized_presentation_data.h"
#include "plot_engine.h"
#include "plot_2d_categorized_engine_base.h"

Plot2DCategorizedEngineBase::Plot2DCategorizedEngineBase() {}


Plot2DCategorizedEngineBase::~Plot2DCategorizedEngineBase() {}


Plot2DCategorizedEngineBase::PlotSeriesMode Plot2DCategorizedEngineBase::supportedPlotSeriesMode() const {
    return PlotSeriesMode::COLOR_REGION;
}


QSet<Plot2DCategorizedEngineBase::SplineType> Plot2DCategorizedEngineBase::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE;
}


unsigned Plot2DCategorizedEngineBase::dataSourcesPerSeries() const {
    return numberDataSourcesPerSeries;
}


QSet<Plot2DCategorizedEngineBase::AxisScale> Plot2DCategorizedEngineBase::supportedAxisScales(
        Plot2DCategorizedEngineBase::AxisLocation /* axisLocation */
    ) const {
    return QSet<AxisScale>() << AxisScale::LINEAR << AxisScale::LOG << AxisScale::DB_10_LOG << AxisScale::DB_20_LOG;
}


Plot2DCategorizedEngineBase::ScalingControlsMode Plot2DCategorizedEngineBase::supportedScalingControlsMode() const {
    return ScalingControlsMode::NO_SCALING_CONTROLS;
}


void Plot2DCategorizedEngineBase::configureChart(
        QChart*                            chartItem,
        QSharedPointer<Ld::PlotElement>    plotElement,
        QSharedPointer<Ld::PlotFormat>     plotFormat
    ) const {
    PlotWrappedEngineBase::configureChart(chartItem, plotElement, plotFormat);
}
