/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HistogramEngine class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HISTOGRAM_ENGINE_H
#define HISTOGRAM_ENGINE_H

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
#include "plot_engine.h"

#include "plot_2d_categorized_engine_base.h"

class QGraphicsItem;
class QColor;

class HistogramPresentationData;

/**
 * Engine for histograms.
 */
class APP_PUBLIC_API HistogramEngine:public Plot2DCategorizedEngineBase {
    Q_DECLARE_TR_FUNCTIONS(HistogramEngine)

    public:
        /**
         * Global instance.
         */
        static const HistogramEngine instance;

        HistogramEngine();

        ~HistogramEngine() override;

        /**
         * Method that provides an internal name for this plot style.
         *
         * \return Returns the internal name for this plot engine.
         */
        QString name() const override;

        /**
         * Method that provides a very short description for this plot style.
         *
         * \return Returns a very short description for this plot style.  The value is intended to be used in dialogs.
         */
        QString shortDescription() const override;

        /**
         * Method that provides a description suitable for use in tool-tips.
         *
         * \return Returns a description for this plot style.  The value is intended to be used in dialogs for
         *         tool tips.
         */
        QString toolTipDescription() const override;

        /**
         * Method that provides a description suitable for use in what's this fly-over text.
         *
         * \return Returns a description for this plot style.  The value is intended to be used in dialogs for
         *         "what's this" text.
         */
        QString whatsThisDescription() const override;

        /**
         * Method that is called to obtain an icon for this plot style.
         *
         * \return Returns an icon used to define the plotting style.
         */
        QIcon icon() const override;

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
         * Method that is called to determine the axis location style.
         *
         * \return Returns the axis location style.
         */
        AxisLocationStyle axisLocationStyle() const override;

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
         * \return Returns the desired scaling controls mode.
         */
        ScalingControlsMode supportedScalingControlsMode() const override;

        /**
         * Method that is called to determine the maximum number of allowed data series.
         *
         * \return Returns the maximum number of allowed data series.  The default implementation returns the maximum
         *         value that can be represented in a signed integer.
         */
        unsigned maximumNumberAllowedDataSeries() const override;

        /**
         * Method that creates a new plot presentation data instance which includes the plot graphics item.
         *
         * \param[in] plotElement The plot element used to create the plot.
         *
         * \param[in] plotFormat  The plot format instance used to create the plot.
         *
         * \return Returns a newly created \ref PlotPresentationData
         */
        PlotPresentationData* createPresentationData(
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const override;

    private:
        /**
         * Method that configures a QtCharts::QChart object using the supplied element and format.
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
            HistogramPresentationData*      presentationData,
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const;

        /**
         * Method that configures the plot series.
         *
         * \param[in] plotElement      A shared pointer to the plot element.
         *
         * \param[in] plotFormat       A shared pointer to the plot format instance.
         *
         * \param[in] presentationData The \ref HistogramPresentationData instance to be configured.
         *
         * \return Returns a string holding error information.  An empty string indicates no error.
         */
        static QString configureSeries(
            const Ld::PlotElement&     plotElement,
            const Ld::PlotFormat&      plotFormat,
            HistogramPresentationData* presentationData
        );
};

#endif
