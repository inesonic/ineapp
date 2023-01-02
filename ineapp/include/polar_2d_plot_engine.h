/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Polar2DPlotEngine class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef POLAR_2D_PLOT_ENGINE_H
#define POLAR_2D_PLOT_ENGINE_H

#include <QCoreApplication>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QString>
#include <QIcon>
#include <QImage>

#include <ld_plot_element.h>
#include <ld_chart_line_style.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>

#include "app_common.h"
#include "plot_marker_painter.h"
#include "plot_2d_engine_base.h"

class QGraphicsItem;
class QColor;

namespace QtCharts {
    class QXYSeries;
    class QScatterSeries;
    class QAbstractAxis;
    class QLegend;
    class QPolarChart;
}

class PlotPresentationData;
class Polar2DPlotPresentationData;

/**
 * Class that implements an polar plotting engine on 2 dimensions (angle and radius).
 */
class APP_PUBLIC_API Polar2DPlotEngine:public Plot2DEngineBase {
    Q_DECLARE_TR_FUNCTIONS(Polar2DPlotEngine)

    public:
        /**
         * Global instance.
         */
        static const Polar2DPlotEngine instance;

        Polar2DPlotEngine();

        ~Polar2DPlotEngine() override;

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
         * Method that is called to obtain the supported plot series modes.
         *
         * \return Returns a list of the supported plot series modes.
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
         * Method that creates a new plot graphics item.
         *
         * \param[in] plotElement The plot element used to create the plot.
         *
         * \param[in] plotFormat  The plot format instance used to create the plot.
         *
         * \return Returns a newly created graphics item.
         */
        PlotPresentationData* createPresentationData(
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat
        ) const override;

    protected:
        /**
         * Method that determines the Qt axis alignment setting for an axis.
         *
         * \param[in] axisLocation The axis location.
         *
         * \return Returns the Qt alignment setting for the axis.
         */
        Qt::Alignment qtAlignmentForAxis(AxisLocation axisLocation) const override;
};

#endif
