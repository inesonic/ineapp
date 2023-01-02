/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Plot2DEngineBase class.
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

#include <eqt_charts.h>
#include <eqt_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "plot_2d_presentation_data.h"
#include "plot_engine.h"
#include "plot_2d_engine_base.h"

const float Plot2DEngineBase::minimumMarkerSizePoints = 9.0F;

Plot2DEngineBase::Plot2DEngineBase() {}


Plot2DEngineBase::~Plot2DEngineBase() {}


Plot2DEngineBase::ScalingControlsMode Plot2DEngineBase::supportedScalingControlsMode() const {
    return ScalingControlsMode::ALL_SCALING_CONTROLS;
}


void Plot2DEngineBase::configureChart(
        QChart*                         chartItem,
        Plot2DPresentationData*         presentationData,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    PlotWrappedEngineBase::configureChart(chartItem, plotElement, plotFormat);

    const Ld::ChartLineStyle activeAreaBorderLineStyle = plotFormat->drawingAreaOutlineStyle();

    QList<Ld::PlotFormat::AxisLocation>                axisLocations = plotFormat->definedAxisLocations();
    QMap<Ld::PlotFormat::AxisLocation, QAbstractAxis*> axisByLocation;

    for (  QList<Ld::PlotFormat::AxisLocation>::const_iterator axisLocationIterator    = axisLocations.constBegin(),
                                                               axisLocationEndIterator = axisLocations.constEnd()
         ; axisLocationIterator != axisLocationEndIterator
         ; ++axisLocationIterator
        ) {
        Ld::PlotFormat::AxisLocation axisLocation = *axisLocationIterator;
        Ld::ChartAxisFormat          axisFormat   = plotFormat->axisFormat(axisLocation);

        QAbstractAxis* axis = configureAxis(
            axisFormat,
            plotElement->axisTitle(axisLocation),
            activeAreaBorderLineStyle
        );

        chartItem->addAxis(axis, qtAlignmentForAxis(axisLocation));
        presentationData->setAxisFormat(axisLocation, axisFormat, axis);

        axisByLocation.insert(axisLocation, axis);
    }

    configureLegend(chartItem->legend(), *plotFormat);

    QString errorReason = configureSeries(*plotElement, *plotFormat, chartItem, presentationData, axisByLocation);

    if (!errorReason.isEmpty()) {
        presentationData->showErrorMessage(errorReason);
    } else {
        presentationData->clearErrorMessage();
    }
}


void Plot2DEngineBase::configureMarkers(
        QScatterSeries*             series,
        Ld::PlotSeries::MarkerStyle markerStyle,
        float                       lineWidth,
        const QColor&               lineColor
    ) {
    if (markerStyle == Ld::PlotSeries::MarkerStyle::NONE) {
        series->setBrush(QBrush());
    } else {
        unsigned markerEdgePoints = static_cast<unsigned>(0.5 + std::max(8 * lineWidth, minimumMarkerSizePoints));
        QSizeF   markerSizeFloat  = toScene(QSizeF(markerEdgePoints, markerEdgePoints));
        unsigned markerEdge       = static_cast<unsigned>((markerSizeFloat.width() + markerSizeFloat.height()) / 2.0);

        series->setMarkerSize(markerEdge);

        if (markerStyle == Ld::PlotSeries::MarkerStyle::CIRCLE) {
            series->setMarkerShape(QScatterSeries::MarkerShape::MarkerShapeCircle);
            series->setPen(QPen(QBrush(lineColor), 0));
            series->setBrush(QBrush());
        } else if (markerStyle == Ld::PlotSeries::MarkerStyle::SQUARE) {
            series->setMarkerShape(QScatterSeries::MarkerShape::MarkerShapeRectangle);
            series->setPen(QPen(QBrush(lineColor), 0));
            series->setBrush(QBrush());
        } else if (markerStyle == Ld::PlotSeries::MarkerStyle::DOT) {
            series->setMarkerShape(QScatterSeries::MarkerShape::MarkerShapeCircle);
            series->setPen(QPen(QBrush(lineColor), 0));
            series->setBrush(QBrush(lineColor));
        } else {
            QSize  markerSize(markerEdge, markerEdge);
            QImage markerImage(markerSize, QImage::Format::Format_ARGB32);

            markerImage.fill(Qt::GlobalColor::transparent);
            QPainter painter(&markerImage);

            paintMarker(&painter, QRect(QPoint(0, 0), markerSize), lineWidth, lineColor, markerStyle);
            painter.end();

            series->setBrush(markerImage);
            series->setPen(QPen(Qt::PenStyle::NoPen));
        }
    }
}


QString Plot2DEngineBase::configureSeries(
        const Ld::PlotElement&                                    plotElement,
        const Ld::PlotFormat&                                     plotFormat,
        QChart*                                                   chartItem,
        Plot2DPresentationData*                                   presentationData,
        const QMap<Ld::PlotFormat::AxisLocation, QAbstractAxis*>& axisByLocation
    ) {
    QString  errorReason;

    unsigned numberDataSeries     = plotElement.numberDataSeries();
    unsigned seriesIndex          = 0;
    unsigned calculatedValueIndex = 0;

    while (errorReason.isEmpty() && seriesIndex < numberDataSeries) {
        QString                              seriesLegendTitle = plotElement.legendTitle(seriesIndex);
        const Ld::PlotSeries&                plotSeries        = plotFormat.plotSeries(seriesIndex);
        Ld::PlotSeries::LineStyle            lineStyle         = plotSeries.lineStyle();
        float                                lineWidth         = plotSeries.lineWidth();
        QColor                               lineColor         = validateForegroundColor(plotSeries.lineColor());
        Ld::PlotSeries::MarkerStyle          markerStyle       = plotSeries.markerStyle();
        Ld::PlotSeries::SplineType           splineType        = plotSeries.splineType();

        QXYSeries*      lineSeries;
        QScatterSeries* scatterSeries;

        if (markerStyle != Ld::PlotSeries::MarkerStyle::NONE) {
            scatterSeries = new QScatterSeries;
            configureMarkers(scatterSeries, markerStyle, lineWidth, lineColor);
            scatterSeries->setName(seriesLegendTitle);

            chartItem->addSeries(scatterSeries);
        } else {
            scatterSeries = Q_NULLPTR;
        }

        if (lineStyle != Ld::ChartLineStyle::LineStyle::NO_LINE && splineType != Ld::PlotSeries::SplineType::NONE) {
            switch (splineType) {
                case Ld::PlotSeries::SplineType::NONE: {
                    lineSeries = Q_NULLPTR;
                    break;
                }

                case Ld::PlotSeries::SplineType::LINE: {
                    lineSeries = new QLineSeries;
                    lineSeries->setPen(QPen(QBrush(lineColor), lineWidth, static_cast<Qt::PenStyle>(lineStyle)));
                    lineSeries->setName(seriesLegendTitle);

                    chartItem->addSeries(lineSeries);

                    break;
                }

                case Ld::PlotSeries::SplineType::SPLINE: {
                    lineSeries = new QSplineSeries;
                    lineSeries->setPen(QPen(QBrush(lineColor), lineWidth, static_cast<Qt::PenStyle>(lineStyle)));
                    lineSeries->setName(seriesLegendTitle);

                    chartItem->addSeries(lineSeries);

                    break;
                }

                case Ld::PlotSeries::SplineType::LINEAR_REGRESSION :{
                    lineSeries = new QLineSeries;
                    lineSeries->setPen(QPen(QBrush(lineColor), lineWidth, static_cast<Qt::PenStyle>(lineStyle)));
                    lineSeries->setName(tr("(m=?, b=?)"));

                    chartItem->addSeries(lineSeries);

                    break;
                }

                default: {
                    lineSeries = Q_NULLPTR; // Make MSVC happy.
                    Q_ASSERT(false);
                    break;
                }
            }
        } else {
            lineSeries = Q_NULLPTR;
        }

        if (lineSeries != Q_NULLPTR) {
            presentationData->setSeries(
                seriesIndex,
                splineType,
                lineSeries,
                scatterSeries
            );
        } else {
            presentationData->setSeries(
                seriesIndex,
                Ld::PlotSeries::SplineType::NONE,
                scatterSeries
            );
        }

        unsigned numberDataSources = plotElement.numberDataSources(seriesIndex);
        if (numberDataSources == numberDataSourcesPerSeries) {
            unsigned sourceIndex = 0;
            while (errorReason.isEmpty() && sourceIndex < numberDataSourcesPerSeries) {
                Ld::VariableName              dataSource   = plotElement.dataSource(seriesIndex, sourceIndex);
                Ld::PlotElement::AxisLocation axisLocation = plotElement.axisLocationForSource(seriesIndex, sourceIndex);

                if (axisByLocation.contains(axisLocation)) {
                    QAbstractAxis* abstractAxis = axisByLocation.value(axisLocation);

                    if (lineSeries != Q_NULLPTR) {
                        lineSeries->attachAxis(abstractAxis);
                    }

                    if (scatterSeries != Q_NULLPTR) {
                        scatterSeries->attachAxis(abstractAxis);
                    }

                    presentationData->setDataSource(
                        calculatedValueIndex,
                        seriesIndex,
                        sourceIndex,
                        axisLocation,
                        dataSource
                    );

                    ++calculatedValueIndex;
                } else {
                    delete lineSeries;
                    delete scatterSeries;

                    errorReason = tr("Mismatched axis locations, series %1.").arg(seriesIndex + 1);
                }

                ++sourceIndex;
            }

            ++seriesIndex;
        } else {
            delete lineSeries;
            delete scatterSeries;

            errorReason = tr("Incorrect number of data sources for series %1.").arg(seriesIndex + 1);
        }
    }

    return errorReason;
}
