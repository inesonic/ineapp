/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RgbImageEngine class.
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

#include <eqt_graphics_pixmap_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "plot_2d_engine_base.h"
#include "rgb_image_presentation_data.h"
#include "image_render_engine.h"
#include "rgb_image_engine.h"

const RgbImageEngine RgbImageEngine::instance;

RgbImageEngine::RgbImageEngine() {}


RgbImageEngine::~RgbImageEngine() {}


QString RgbImageEngine::name() const {
    return QString("rgb");
}


QString RgbImageEngine::shortDescription() const {
    return tr("RGB Image");
}


QString RgbImageEngine::toolTipDescription() const {
    return tr("RGB Image, draws an RGB image from a triple of matrices.");
}


QString RgbImageEngine::whatsThisDescription() const {
    return tr(
        "You can use this button to select an RGB image, rendered from a triple of matrices.  One matrix per color."
    );
}


QIcon RgbImageEngine::icon() const {
    return QIcon(":plot_image_rgb_64.png");
}


PlotEngine::PlotSeriesMode RgbImageEngine::supportedPlotSeriesMode() const {
    return PlotSeriesMode::COLOR_REGION;
}


QSet<PlotEngine::SplineType> RgbImageEngine::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE;
}


PlotEngine::AxisLocationStyle RgbImageEngine::axisLocationStyle() const {
    return AxisLocationStyle::RGB_COLOR_SPACE;
}


unsigned RgbImageEngine::minimumNumberAllowedDataSeries() const {
    return 3;
}


unsigned RgbImageEngine::maximumNumberAllowedDataSeries() const {
    return 3;
}


PlotPresentationData* RgbImageEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    RgbImagePresentationData* presentationData = new RgbImagePresentationData;
    configureChart(presentationData, plotElement, plotFormat, QPixmap(":plot_image_rgb_64.png"));

    return presentationData;
}
