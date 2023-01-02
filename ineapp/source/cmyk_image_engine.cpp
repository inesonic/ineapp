/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CmykImageEngine class.
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
#include "cmyk_image_presentation_data.h"
#include "image_render_engine.h"
#include "cmyk_image_engine.h"

const CmykImageEngine CmykImageEngine::instance;

CmykImageEngine::CmykImageEngine() {}


CmykImageEngine::~CmykImageEngine() {}


QString CmykImageEngine::name() const {
    return QString("cmyk");
}


QString CmykImageEngine::shortDescription() const {
    return tr("CMYK Image");
}


QString CmykImageEngine::toolTipDescription() const {
    return tr("CMYK Image, draws an CMYK image from a triple of matrices.");
}


QString CmykImageEngine::whatsThisDescription() const {
    return tr(
        "You can use this button to select an CMYK image, rendered from a quadriple of matrices.  "
        "One matrix per color."
    );
}


QIcon CmykImageEngine::icon() const {
    return QIcon(":plot_image_cmyk_64.png");
}


PlotEngine::PlotSeriesMode CmykImageEngine::supportedPlotSeriesMode() const {
    return PlotSeriesMode::COLOR_REGION;
}


QSet<PlotEngine::SplineType> CmykImageEngine::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE;
}


PlotEngine::AxisLocationStyle CmykImageEngine::axisLocationStyle() const {
    return AxisLocationStyle::CMYK_COLOR_SPACE;
}


unsigned CmykImageEngine::minimumNumberAllowedDataSeries() const {
    return 4;
}


unsigned CmykImageEngine::maximumNumberAllowedDataSeries() const {
    return 4;
}


PlotPresentationData* CmykImageEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    CmykImagePresentationData* presentationData = new CmykImagePresentationData;
    configureChart(presentationData, plotElement, plotFormat, QPixmap(":plot_image_cmyk_64.png"));

    return presentationData;
}
