/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Polar2DPlotPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef POLAR_2D_PLOT_PRESENTATION_DATA_H
#define POLAR_2D_PLOT_PRESENTATION_DATA_H

#include <QObject>

#include <utility>

#include "app_common.h"
#include "plot_2d_presentation_data.h"

namespace EQt {
    class ChartItem;
    class Polar2DChartItem;
}

/**
 * The \ref PlotPresentationData instance to use for 2-D polar plots.
 */
class APP_PUBLIC_API Polar2DPlotPresentationData:public Plot2DPresentationData {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The chart item containing the plot.
         */
        Polar2DPlotPresentationData(EQt::Polar2DChartItem* graphicsItem);

        ~Polar2DPlotPresentationData() override;

    protected:
        /**
         * Method you can use to adjust a minimum and maximum values to a reasonable range.
         *
         * \param[in] axisLocation    The axis we're adjusting our scaling for.  The default implementation ignores
         *                            this value.
         *
         * \param[in] measuredMinimum The measured minimum value.  The returned value will be slightly lower than this.
         *
         * \param[in] measuredMaximum The measured maximum value.  This is used to modify the selection of the minimum.
         *                            value.
         *
         * \param[in] axisScale       The axis scaling mode.  The value is used to determine the algorithm and bounds
         *                            used to calculate a recommended range.
         *
         * \return Returns a pair of values holding the recommended range.
         */
        SeriesMinMax calculateNiceRange(
            AxisLocation axisLocation,
            double       measuredMinimum,
            double       measuredMaximum,
            AxisScale    axisScale
        ) const override;

        /**
         * Method that calculates a recommended major and minor step size based on the plot minimum and maximum values.
         *
         * \param[in] axisLocation       The axis we're adjusting our scaling for.  The default implementation ignores
         *                               this value beyond providing it to
         *                               \ref PlotPresentationData::calculateMinorStepSize.
         *
         * \param[in] span               The span between the minimum and maximum value.
         *
         * \param[in] maximumNumberTicks The maximum number of ticks.
         *
         * \return Returns a pair of value holding the recommended major and minor step sizes.
         */
        SeriesStepSizes calculateNiceStepSizes(
            AxisLocation axisLocation,
            double       span,
            float        maximumNumberTicks
        ) const override;

        /**
         * Method you should overload to indicate whether an axis is modulo or not.
         *
         * \param[in] axisLocation The axis location of interest.
         *
         * \return Returns true if the axis is a modulo axis.  Returns false if the axis is not a modulo axis.  The
         *         default implementation always returns false.
         */
        bool isModuloAxis(AxisLocation axisLocation) const override;
};

#endif
