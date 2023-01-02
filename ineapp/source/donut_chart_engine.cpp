/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DonutChartEngine class.
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

#include <eqt_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "donut_chart_presentation_data.h"
#include "plot_engine.h"
#include "plot_2d_categorized_engine_base.h"
#include "pie_chart_engine.h"
#include "donut_chart_engine.h"

const DonutChartEngine DonutChartEngine::instance;

DonutChartEngine::DonutChartEngine() {}


DonutChartEngine::~DonutChartEngine() {}


QString DonutChartEngine::name() const {
    return QString("donut");
}


QString DonutChartEngine::shortDescription() const {
    return tr("Donut Chart");
}


QString DonutChartEngine::toolTipDescription() const {
    return tr("Donut chart style.");
}


QString DonutChartEngine::whatsThisDescription() const {
    return tr("You can use this button to select a donut chart.");
}


QIcon DonutChartEngine::icon() const {
    return QIcon(":plot_donut_chart_64.png");
}


DonutChartEngine::AxisLocationStyle DonutChartEngine::axisLocationStyle() const {
    return AxisLocationStyle::UNCATEGORIZED;
}


PlotPresentationData* DonutChartEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    EQt::ChartItem*             chartItem        = new EQt::ChartItem;
    DonutChartPresentationData* presentationData = new DonutChartPresentationData(chartItem);

    configureChart(chartItem, presentationData, plotElement, plotFormat);

    return presentationData;
}
