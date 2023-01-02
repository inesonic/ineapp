/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Polar2DPlotEngine class.
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

#include <eqt_polar_2d_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "plot_engine.h"
#include "polar_2d_plot_presentation_data.h"
#include "polar_2d_plot_engine.h"

const Polar2DPlotEngine Polar2DPlotEngine::instance;

Polar2DPlotEngine::Polar2DPlotEngine() {}


Polar2DPlotEngine::~Polar2DPlotEngine() {}


QString Polar2DPlotEngine::name() const {
    return QString("polar2d");
}


QString Polar2DPlotEngine::shortDescription() const {
    return tr("Polar");
}


QString Polar2DPlotEngine::toolTipDescription() const {
    return tr("Polar plotting style, draws points, curves, or lines on a 2-dimensional polar space.");
}


QString Polar2DPlotEngine::whatsThisDescription() const {
    return tr(
        "You can use this button to select the polar plotting style.  You can use this plot style to create plots "
        "points, curves, and/or lines in a 2-dimensional polar space."
    );
}


QIcon Polar2DPlotEngine::icon() const {
    return QIcon(":plot_polar_2d_64.png");
}


PlotEngine::PlotSeriesMode Polar2DPlotEngine::supportedPlotSeriesMode() const {
    return PlotSeriesMode::LINES_AND_MARKERS;
}


unsigned Polar2DPlotEngine::dataSourcesPerSeries() const {
    return numberDataSourcesPerSeries;
}


QSet<PlotEngine::SplineType> Polar2DPlotEngine::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE << SplineType::LINE;
}


PlotEngine::AxisLocationStyle Polar2DPlotEngine::axisLocationStyle() const {
    return AxisLocationStyle::POLAR_2D;
}


QSet<PlotEngine::AxisScale> Polar2DPlotEngine::supportedAxisScales(PlotEngine::AxisLocation axisLocation) const {
    QSet<AxisScale> result;

    switch (axisLocation) {
        case AxisLocation::LEFT_Y_R_RC:
        case AxisLocation::RIGHT_Y_R_RY: {
            result << AxisScale::LINEAR << AxisScale::LOG << AxisScale::DB_10_LOG << AxisScale::DB_20_LOG;
            break;
        }

        case AxisLocation::BOTTOM_X_A_GM:
        case AxisLocation::TOP_X_A_GM:
        case AxisLocation::Z_B_BK: {
            result << AxisScale::LINEAR;
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


PlotPresentationData* Polar2DPlotEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    EQt::Polar2DChartItem*       chartItem        = new EQt::Polar2DChartItem;
    Polar2DPlotPresentationData* presentationData = new Polar2DPlotPresentationData(chartItem);

    QSizeF sizeSceneUnits = toScene(QSizeF(plotFormat->chartWidth(), plotFormat->chartHeight()));
    chartItem->setGeometry(0, 0, sizeSceneUnits.width(), sizeSceneUnits.height());

    dynamic_cast<EQt::GraphicsItemGroup*>(presentationData->graphicsItem())->setForcedGeometry(
        0, 0,
        sizeSceneUnits.width(), sizeSceneUnits.height()
    );

    configureChart(chartItem, presentationData, plotElement, plotFormat);

    return presentationData;
}


Qt::Alignment Polar2DPlotEngine::qtAlignmentForAxis(Polar2DPlotEngine::AxisLocation axisLocation) const {
    Qt::Alignment result;
    switch (axisLocation) {
        case Ld::PlotFormat::AxisLocation::LEFT_Y_R_RC:   { result = Qt::AlignmentFlag::AlignLeft;     break; }
        case Ld::PlotFormat::AxisLocation::BOTTOM_X_A_GM: { result = Qt::AlignmentFlag::AlignBottom;   break; }
        case Ld::PlotFormat::AxisLocation::RIGHT_Y_R_RY:  { result = Qt::AlignmentFlag::AlignLeft;     break; }
        case Ld::PlotFormat::AxisLocation::TOP_X_A_GM:    { result = Qt::AlignmentFlag::AlignBottom;   break; }

        case Ld::PlotFormat::AxisLocation::Z_B_BK: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}
