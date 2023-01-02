/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Plot2DEngineBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_2D_ENGINE_BASE_H
#define PLOT_2D_ENGINE_BASE_H

#include <QCoreApplication>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QString>
#include <QIcon>
#include <QImage>

#include <eqt_charts.h>

#include <ld_plot_element.h>
#include <ld_chart_line_style.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>

#include "app_common.h"
#include "plot_marker_painter.h"
#include "plot_wrapped_engine_base.h"

class QGraphicsItem;
class QColor;

class PlotPresentationData;
class Plot2DPresentationData;

/**
 * Base class for XY, polar, and similar plots that require multiple axis and markers.
 */
class APP_PUBLIC_API Plot2DEngineBase:public PlotWrappedEngineBase, public PlotMarkerPainter {
    Q_DECLARE_TR_FUNCTIONS(Plot2DEngineBase)

    public:
        Plot2DEngineBase();

        ~Plot2DEngineBase() override;

        /**
         * Method that is called to determine the desired scaling controls mode.
         *
         * \return Returns the desired scaling controls mode.  This version returns
         *         \ref ScalingControlsMode::ALL_SCALING_CONTROLS.
         */
        ScalingControlsMode supportedScalingControlsMode() const override;

    protected:
        /**
         * Value indicating the number of data sources per data series.
         */
        static constexpr unsigned numberDataSourcesPerSeries = 2;

        /**
         * Value indicating the minimum marker size, in points.
         */
        static const float minimumMarkerSizePoints;

        /**
         * Method that configures a QChart object using the supplied element and format.
         *
         * \param[in] chartItem        The chart to be configured.
         *
         * \param[in] presentationData The presentation data instance to be configured with the chart.
         *
         * \param[in] plotElement      The plot element used to create the plot.
         *
         * \param[in] plotFormat       The plot format instance used to create the plot.
         */
        void configureChart(
            QChart*                         chartItem,
            Plot2DPresentationData*         presentationData,
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const;

        /**
         * Method that configures the markers for a series.
         *
         * \param[in] series      The series to be configured.
         *
         * \param[in] markerStyle The marker style to create a painter path for.
         *
         * \param[in] lineWidth   The line width to use as a basis to determine the marker size.
         *
         * \param[in] lineColor   The line color to use for the markers.
         *
         * \return Returns the image used for the markers.
         */
        static void configureMarkers(
            QScatterSeries*             series,
            Ld::PlotSeries::MarkerStyle markerStyle,
            float                       lineWidth,
            const QColor&               lineColor
        );

        /**
         * Method that configures the plot series.
         *
         * \param[in] plotElement      A shared pointer to the plot element.
         *
         * \param[in] plotFormat       A shared pointer to the plot format instance.
         *
         * \param[in] chartItem        The QChart instance to be configured.
         *
         * \param[in] presentationData The \ref Plot2DPresentationData instance to be configured.
         *
         * \param[in] axisByLocation   The Qt axis instances by location.
         *
         * \return Returns a string holding error information.  An empty string indicates no error.
         */
        static QString configureSeries(
            const Ld::PlotElement&                                    plotElement,
            const Ld::PlotFormat&                                     plotFormat,
            QChart*                                                   chartItem,
            Plot2DPresentationData*                                   presentationData,
            const QMap<Ld::PlotFormat::AxisLocation, QAbstractAxis*>& axisByLocation
        );
};

#endif
