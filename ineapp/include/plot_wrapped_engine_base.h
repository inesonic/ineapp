/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotWrappedEngineBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_WRAPPED_ENGINE_BASE_H
#define PLOT_WRAPPED_ENGINE_BASE_H

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

class QGraphicsItem;
class QColor;

class PlotPresentationData;
class Plot2DPresentationData;

/**
 * Base class for most plots based on QtChart::QChart.  This class extends \ref PlotEngine to include support for
 * basic configuration settings common to all QtChart::QChart instances.
 */
class APP_PUBLIC_API PlotWrappedEngineBase:public PlotEngine {
    Q_DECLARE_TR_FUNCTIONS(PlotWrappedEngineBase)

    public:
        /**
         * Value indicating the minimum marker size, in points.
         */
        static const float minimumMarkerSizePoints;

        PlotWrappedEngineBase();

        ~PlotWrappedEngineBase() override;

    protected:
        /**
         * Method that configures a QChart object using the supplied element and format.  This method will configure
         * chart borders, background colors, and the plot title.
         *
         * \param[in] chartItem   The chart to be configured.
         *
         * \param[in] plotElement The plot element used to create the plot.
         *
         * \param[in] plotFormat  The plot format instance used to create the plot.
         */
        void configureChart(
            QChart*                         chartItem,
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const;

        /**
         * Method that configures a Qt axis from a Ld::ChartAxisFormat instance.
         *
         * \param[in] axisFormat              The axis format to use to configure the axis.
         *
         * \param[in] axisTitle               The axis title text.
         *
         * \param[in] plotAreaBorderLineStyle The line style to use for the plot area border.
         *
         * \return Returns a newly created Qt axis instance.
         */
        static QAbstractAxis* configureAxis(
            const Ld::ChartAxisFormat& axisFormat,
            const QString&             axisTitle,
            const Ld::ChartLineStyle&  plotAreaBorderLineStyle
        );

        /**
         * Helper method that configures a Qt axis from a Ld::ChartAxisFormat instance.  This method configures the
         * visual appearance of the axis.
         *
         * \param[in] axis                    The Qt axis to be confignured.
         *
         * \param[in] axisFormat              The axis format to use to configure the axis.
         *
         * \param[in] axisTitle               The axis title text.
         *
         * \param[in] plotAreaBorderLineStyle The line style to use for the plot area border.
         *
         * \return Returns a newly created Qt axis instance.
         */
        static void configureAxisHelper(
            QAbstractAxis*             axis,
            const Ld::ChartAxisFormat& axisFormat,
            const QString&             axisTitle,
            const Ld::ChartLineStyle&  plotAreaBorderLineStyle
        );

        /**
         * Method that configures the plot legend
         *
         * \param[in] legend           The Qt plot legend.
         *
         * \param[in] plotFormat       The plot format instance providing the configuration data for the legend.
         *
         * \param[in] honorMarkerShape If true, the marker shape will be set to match the series.
         */
        static void configureLegend(
            QLegend*              legend,
            const Ld::PlotFormat& plotFormat,
            bool                  honorMarkerShape = true
        );

        /**
         * Method that determines the Qt axis alignment setting for an axis.
         *
         * \param[in] axisLocation The axis location.
         *
         * \return Returns the Qt alignment setting for the axis.
         */
        virtual Qt::Alignment qtAlignmentForAxis(AxisLocation axisLocation) const;

        /**
         * Method that handles invalid colors, converting them to black.
         *
         * \param[in] inputColor The color to be processed.
         *
         * \return Returns the clarified color.
         */
        static QColor validateForegroundColor(const QColor& inputColor);

        /**
         * Method that handles invalid colors, converting them to transparent.
         *
         * \param[in] inputColor The color to be processed.
         *
         * \return Returns the clarified color.
         */
        static QColor validateBackgroundColor(const QColor& inputColor);
};

#endif
