/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref GrayscaleImageEngine class.
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
#include "grayscale_image_presentation_data.h"
#include "image_render_engine.h"
#include "grayscale_image_engine.h"

const GrayscaleImageEngine GrayscaleImageEngine::instance;

GrayscaleImageEngine::GrayscaleImageEngine() {}


GrayscaleImageEngine::~GrayscaleImageEngine() {}


QString GrayscaleImageEngine::name() const {
    return QString("grayscale");
}


QString GrayscaleImageEngine::shortDescription() const {
    return tr("Grayscale Image");
}


QString GrayscaleImageEngine::toolTipDescription() const {
    return tr("Grayscale Image, draws an grayscale image from a triple of matrices.");
}


QString GrayscaleImageEngine::whatsThisDescription() const {
    return tr(
        "You can use this button to select an grayscale image, rendered from a matrix."
    );
}


QIcon GrayscaleImageEngine::icon() const {
    return QIcon(":plot_image_grayscale_64.png");
}


PlotEngine::PlotSeriesMode GrayscaleImageEngine::supportedPlotSeriesMode() const {
    return PlotSeriesMode::COLOR_REGION;
}


QSet<PlotEngine::SplineType> GrayscaleImageEngine::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE;
}


PlotEngine::AxisLocationStyle GrayscaleImageEngine::axisLocationStyle() const {
    return AxisLocationStyle::GRAYSCALE_COLOR_SPACE;
}


unsigned GrayscaleImageEngine::minimumNumberAllowedDataSeries() const {
    return 1;
}


unsigned GrayscaleImageEngine::maximumNumberAllowedDataSeries() const {
    return 1;
}


PlotPresentationData* GrayscaleImageEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    GrayscaleImagePresentationData* presentationData = new GrayscaleImagePresentationData;
    configureChart(presentationData, plotElement, plotFormat, QPixmap(":plot_image_grayscale_64.png"));

    return presentationData;
}
