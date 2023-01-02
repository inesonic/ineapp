/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImageRenderEngine class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMAGE_RENDER_ENGINE_H
#define IMAGE_RENDER_ENGINE_H

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
#include "plot_engine.h"

namespace EQt {
    class GraphicsPixmapItem;
    class GraphicsRectItem;
}

class PlotPresentationData;
class ImageRenderPresentationData;

/**
 * Class you can use as a basis for plot-like image rendering engines.
 */
class APP_PUBLIC_API ImageRenderEngine:public PlotEngine {
    Q_DECLARE_TR_FUNCTIONS(ImageRenderEngine)

    public:
        ImageRenderEngine();

        ~ImageRenderEngine() override;

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
         * \return Returns the desired scaling controls mode.
         */
        ScalingControlsMode supportedScalingControlsMode() const override;

        /**
         * Method that is called to determine if the active area background settings can be controlled.
         *
         * \return Returns true if the active area background settings can be changed.  Returns false if the active
         *         area background settings can-not be changed.  This version returns false.
         */
        bool activeAreaBackgroundAdjustable() const override;

        /**
         * Method that is called to determine if this plot style supports a legend.
         *
         * \return Returns true if this plot style supports a legend.  Returns false if this plot style does not
         *         support a legend.  The default implementation always returns true.
         */
        bool supportsPlotLegend() const override;

        /**
         * Method that is called to determine if this plot style has a controllable active plot area.
         *
         * \return Returns true if this plot style has a distinct/controllable active plot area.  Returns false if
         *         this plot style does not have a distinct controllable active plot area.  The default implemetnation
         *         returns true.
         */
        bool hasDistinctActiveArea() const override;

        /**
         * Method that is called to determine if this plot style includes a plot title.
         *
         * \return Returns true if this plot style has a plot title.  Returns false if this plot style does not have a
         *         plot title.  The default implementation returns true.
         */
        bool supportsPlotTitle() const override;

        /**
         * Method that is called to determine if this plot style has a viewable background area that can be controlled.
         *
         * \return Returns true if this plot style has a viewable and controllable background area.  Returns false if
         *         this plot style does not have a viewable and controllable background area.  The default
         *         implementation returns true.
         */
        bool supportsControllableBackgroundArea() const override;

    protected:
        /**
         * Method that configures a QtCharts::QChart object using the supplied element and format.
         *
         * \param[in] presentationData The presentation data instance to be configured with the chart.
         *
         * \param[in] plotElement      The plot element used to create the plot.
         *
         * \param[in] plotFormat       The plot format instance used to create the plot.
         *
         * \param[in] defaultImage     The default image.
         */
        void configureChart(
            ImageRenderPresentationData*    presentationData,
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat,
            const QPixmap&                  defaultImage
        ) const;

    private:
        /**
         * Value indicating the number of data sources per data series.
         */
        static constexpr unsigned numberDataSourcesPerSeries = 1;
};

#endif
