/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotWrappedPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_WRAPPED_PRESENTATION_DATA_H
#define PLOT_WRAPPED_PRESENTATION_DATA_H

#include <QString>
#include <QRectF>

#include <eqt_charts.h>
#include "app_common.h"
#include "plot_presentation_data.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsItem;
    class ChartItem;
}

namespace Ld {
    class ChartAxisFormat;
}

/**
 * Pure virtual base class you can use to manage plots.  The class wraps the actual chart instance within a group
 * allowing for the displaying of error messages as well as error borders.
 */
class APP_PUBLIC_API PlotWrappedPresentationData:public PlotPresentationData {
    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The graphics item to be managed.  This instance must be derived from both
         *                         EQt::GraphicsItem and QChart
         */
        PlotWrappedPresentationData(EQt::GraphicsItem* graphicsItem = Q_NULLPTR);

        ~PlotWrappedPresentationData() override;

        /**
         * Method you can use to obtain the bounding rectangle for this plot.
         *
         * \return Returns the bounding rectangle for this plot.  This version returns the bounding rectangle of the
         *         chart item.
         */
        QRectF boundingRectangle() const override;

        /**
         * Method you can use to obtain the scene bounding rectangle for this plot.
         *
         * \return Returns the scene bounding rectangle for this plot.  This version returns the scene bounding
         *         rectangle of the chart item.
         */
        QRectF sceneBoundingRectangle() const override;

        /**
         * Method you can use to set an error message.
         *
         * \param[in] errorMessage The error message to be displayed.
         */
        void showErrorMessage(const QString& errorMessage);

        /**
         * Method you can use to clear any reported error message.
         */
        void clearErrorMessage();

    protected:
        /**
         * Value holds the minimum number of ticks we try to support on any given axis.
         */
        static constexpr unsigned minimumNumberTicks = 4;

        /**
         * Class that tracks data about an axis.
         */
        class AxisData {
            public:
                AxisData();

                /**
                 * Constructor
                 *
                 * \param[in] axisLocation The location of the axis.
                 *
                 * \param[in] axisFormat   The axis format to be assigned.
                 *
                 * \param[in] axis         The Qt axis associated with the format.
                 */
                AxisData(
                    AxisLocation               axisLocation,
                    const Ld::ChartAxisFormat& axisFormat,
                    QAbstractAxis*             axis
                );

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                AxisData(const AxisData& other);

                ~AxisData();

                /**
                 * Method you can use to determine the location of this axis.
                 *
                 * \return Returns the axis location.
                 */
                inline AxisLocation axisLocation() const {
                    return currentAxisLocation;
                }

                /**
                 * Method you can use to determine the axis format.
                 *
                 * \return Returns the axis format.
                 */
                inline const Ld::ChartAxisFormat& axisFormat() const {
                    return currentAxisFormat;
                }

                /**
                 * Method you can use to determine the Qt axis instance.
                 *
                 * \return Returns a pointer to the Qt axis instance.
                 */
                inline QAbstractAxis* axis() const {
                    return currentAxis;
                }

                /**
                 * Assignment operator.
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                AxisData& operator=(const AxisData& other);

            private:
                /**
                 * The current axis location.
                 */
                AxisLocation currentAxisLocation;

                /**
                 * The current axis format.
                 */
                Ld::ChartAxisFormat currentAxisFormat;

                /**
                 * The current Qt axis instance.
                 */
                QAbstractAxis* currentAxis;
        };

        /**
         * Method that creates a new graphics item.
         *
         * \param[in] chartItem The chart to be assigned.
         */
        void setChartItem(EQt::GraphicsItem* chartItem);

        /**
         * Method you can use to get the underlying chart item.
         *
         * \return Returns a pointer to the chart item.
         */
        QChart* chartItem() const;

    private:
        /**
         * The current chart item.
         */
        EQt::GraphicsItem* currentChartGraphicsItem;

        /**
         * The current chart item, cast as a QtChart::QChart instance.
         */
        QChart* currentChartItem;
};

#endif
