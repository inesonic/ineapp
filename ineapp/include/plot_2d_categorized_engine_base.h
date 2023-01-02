/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Plot2DCategorizedEngineBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_2D_CATEGORIZED_PLOT_ENGINE_H
#define PLOT_2D_CATEGORIZED_PLOT_ENGINE_H

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
class Plot2DCategorizedPresentationData;

/**
 * Base class for 2D bar charts, pie charts, and donut charts.
 */
class APP_PUBLIC_API Plot2DCategorizedEngineBase:public PlotWrappedEngineBase {
    Q_DECLARE_TR_FUNCTIONS(Plot2DCategorizedEngineBase)

    public:
        Plot2DCategorizedEngineBase();

        ~Plot2DCategorizedEngineBase() override;

        /**
         * Method that is called to obtain the supported plot series mode.
         *
         * \return Returns the supported plot series mode.
         */
        PlotSeriesMode supportedPlotSeriesMode() const override;

        /**
         * Method that is called to obtain a set of supported spline types.
         *
         * \return Returns a set of supported spline types.
         */
        QSet<SplineType> supportedSplineTypes() const override;

        /**
         * Method that is called to determine the number of data sources per data series.
         *
         * \return Returns the number of data sources required to represent a single data series.
         */
        unsigned dataSourcesPerSeries() const override;

        /**
         * Method that is called to determine the supported axis scaling modes.
         *
         * \param[in] axisLocation The axis location to query the supported styles for.
         *
         * \return Returns a set of supported axis scaling modes.
         */
        QSet<AxisScale> supportedAxisScales(AxisLocation axisLocation) const override;

        /**
         * Method that is called to determine the desired scaling controls mode.
         *
         * \return Returns the desired scaling controls mode.  This version returns
         *         \ref ScalingControlsMode::NO_SCALING_CONTROLS.
         */
        ScalingControlsMode supportedScalingControlsMode() const override;

    protected:
        /**
         * Value indicating the number of data sources per data series.
         */
        static constexpr unsigned numberDataSourcesPerSeries = 1;

        /**
         * Method that configures a QChart object using the supplied element and format.
         *
         * \param[in] chartItem        The chart to be configured.
         *
         * \param[in] plotElement      The plot element used to create the plot.
         *
         * \param[in] plotFormat       The plot format instance used to create the plot.
         */
        void configureChart(
            QChart*                         chartItem,
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const;
};

#endif
