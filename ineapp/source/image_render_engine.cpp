/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImageRenderEngine class.
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
#include <eqt_graphics_rect_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "plot_engine.h"
#include "image_render_presentation_data.h"
#include "image_render_engine.h"

ImageRenderEngine::ImageRenderEngine() {}


ImageRenderEngine::~ImageRenderEngine() {}


QSet<PlotEngine::SplineType> ImageRenderEngine::supportedSplineTypes() const {
    return QSet<SplineType>() << SplineType::NONE;
}


unsigned ImageRenderEngine::dataSourcesPerSeries() const {
    return numberDataSourcesPerSeries;
}


QSet<PlotEngine::AxisScale> ImageRenderEngine::supportedAxisScales(PlotEngine::AxisLocation /* axisLocation */) const {
    return QSet<AxisScale>() << AxisScale::LINEAR;
}


ImageRenderEngine::ScalingControlsMode ImageRenderEngine::supportedScalingControlsMode() const {
    return ScalingControlsMode::MANUAL_SCALING_CONTROLS;
}


bool ImageRenderEngine::activeAreaBackgroundAdjustable() const {
    return false;
}


bool ImageRenderEngine::supportsPlotLegend() const {
    return false;
}


bool ImageRenderEngine::hasDistinctActiveArea() const {
    return false;
}


bool ImageRenderEngine::supportsPlotTitle() const {
    return false;
}


bool ImageRenderEngine::supportsControllableBackgroundArea() const {
    return false;
}


void ImageRenderEngine::configureChart(
        ImageRenderPresentationData*    presentationData,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat,
        const QPixmap&                  defaultImage
    ) const {
    EQt::GraphicsMultiTextGroup* graphicsItem = new EQt::GraphicsMultiTextGroup;
    EQt::GraphicsPixmapItem*     graphicsPixmapItem = new EQt::GraphicsPixmapItem;
    graphicsItem->addToGroup(graphicsPixmapItem);

    const Ld::ChartLineStyle& borderLineStyle = plotFormat->borderLineStyle();
    QColor                    lineColor       = borderLineStyle.lineColor();
    QPen borderPen(
        lineColor.isValid() ? lineColor : QColor(Qt::GlobalColor::transparent),
        borderLineStyle.lineWidth(),
        static_cast<Qt::PenStyle>(borderLineStyle.lineStyle())
    );

    QSizeF sizePoints(plotFormat->chartWidth(), plotFormat->chartHeight());
    presentationData->configure(graphicsItem, graphicsPixmapItem, sizePoints, borderPen, defaultImage);

    QString  errorString;
    unsigned numberRequiredValues  = presentationData->numberSeries();
    unsigned numberAvailableValues = plotElement->numberDataSeries();

    if (numberAvailableValues != numberRequiredValues) {
        errorString = tr("Invalid number of data series for this type of image.");
    } else {
        unsigned seriesIndex = 0;
        while(errorString.isEmpty() && seriesIndex < numberRequiredValues) {
            if (plotElement->numberDataSources(seriesIndex) != 1) {
                errorString = tr("Invalid number of data sources in series %1.").arg(seriesIndex + 1);
            } else {
                AxisLocation axisLocation = plotElement->axisLocationForSource(seriesIndex, 0);
                presentationData->setSeries(seriesIndex, seriesIndex, axisLocation);

                ++seriesIndex;
            }
        }
    }

    if (!errorString.isEmpty()) {
        presentationData->showErrorMessage(errorString);
    }
}
