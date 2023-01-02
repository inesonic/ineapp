/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_PRESENTATION_DATA_H
#define PLOT_PRESENTATION_DATA_H

#include <QVector>
#include <QPointF>
#include <QRectF>

#include <ld_chart_axis_format.h>
#include <ld_plot_format.h>

#include "app_common.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsItem;
}

namespace Ld {
    class CalculatedValue;
}

/**
 * You can use this pure virtual class as a common base class for managing plots and plot data.
 */
class APP_PUBLIC_API PlotPresentationData {
    public:
        /**
         * Enumeration of axis scaling values.
         */
        typedef Ld::ChartAxisFormat::AxisScale AxisScale;

        /**
         * Enumeration of supported axis locations.
         */
        typedef Ld::PlotFormat::AxisLocation AxisLocation;

        /**
         * Constructor
         *
         * \param[in] graphicsItem The graphics item to be managed.  This object is expected to be derived from
         *                         EQt::GraphicsItem and QGraphicsItem.
         */
        PlotPresentationData(EQt::GraphicsItem* graphicsItem = Q_NULLPTR);

        virtual ~PlotPresentationData();

        /**
         * Method you can use to set the graphics item used to render the plot.  This class will delete the graphics
         * item when it is destroyed.
         *
         * \param[in] newGraphicsItem The graphics item to be managed.  This object is expected to be derived from
         *                            EQt::GraphicsItem and QGraphicsItem.  If there is already a graphics item
         *                            associated with this class, it will be deleted.
         */
        void setGraphicsItem(EQt::GraphicsItem* newGraphicsItem);

        /**
         * Method you can use to obtain the bounding rectangle for this plot.
         *
         * \return Returns the bounding rectangle for this plot.
         */
        virtual QRectF boundingRectangle() const = 0;

        /**
         * Method you can use to obtain the scene bounding rectangle for this plot.
         *
         * \return Returns the scene bounding rectangle for this plot.
         */
        virtual QRectF sceneBoundingRectangle() const = 0;

        /**
         * Method you can use to obtain the graphics item.
         *
         * \returns the graphics item.
         */
        QGraphicsItem* graphicsItem() const;

        /**
         * Method you can call to report a new calculated value to the plot.
         *
         * \param[in] valueIndex      The index used to reference the calculated value.
         *
         * \param[in] calculatedValue The new calculated value.
         */
        virtual void calculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue) = 0;

        /**
         * Method you can call to clear all the reported calculated values.
         */
        virtual void calculatedValuesCleared() = 0;

    protected:
        /**
         * Trivial class that stores minimum and maximum values.
         */
        class SeriesMinMax {
            public:
                constexpr SeriesMinMax():
                    currentMinimum(+std::numeric_limits<double>::max()),
                    currentMaximum(-std::numeric_limits<double>::min()) {}

                /**
                 * Constructor
                 *
                 * \param[in] newMinimum The new minimum value.
                 *
                 * \param[in] newMaximum The new maximum value.
                 */
                constexpr SeriesMinMax(
                        double newMinimum,
                        double newMaximum
                    ):currentMinimum(
                        newMinimum
                    ),currentMaximum(
                        newMaximum
                    ) {}

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                constexpr SeriesMinMax(
                        const SeriesMinMax& other
                    ):currentMinimum(
                        other.currentMinimum
                    ),currentMaximum(
                        other.currentMaximum
                    ) {}

                ~SeriesMinMax() = default;

                /**
                 * Method you can use to update the current minimum and maximum values.
                 *
                 * \param[in] candidateValue The candidate value to apply.
                 */
                void update(double candidateValue);

                /**
                 * Method you can use to update the current minimum and maximum values.
                 *
                 * \param[in] other The min-max values to compare against this set.
                 */
                void update(const SeriesMinMax& other);

                /**
                 * Method you can use to get the current minimum value.
                 *
                 * \return Returns the current minimum value.
                 */
                inline double minimum() const {
                    return currentMinimum;
                }

                /**
                 * Method you can use to update the minimum value.
                 *
                 * \param[in] newMinimum The new minimum value.
                 */
                void setMinimum(double newMinimum) {
                    currentMinimum = newMinimum;
                }

                /**
                 * Method you can use to get the current maximum value.
                 *
                 * \return Returns the current maximum value.
                 */
                inline double maximum() const {
                    return currentMaximum;
                }

                /**
                 * Method you can use to update the maximum value.
                 *
                 * \param[in] newMaximum The new maximum value.
                 */
                void setMaximum(double newMaximum) {
                    currentMaximum = newMaximum;
                }

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                SeriesMinMax& operator=(const SeriesMinMax& other) {
                    currentMinimum = other.currentMinimum;
                    currentMaximum = other.currentMaximum;

                    return *this;
                }

                /**
                 * Comparison operator
                 *
                 * \param[in] other The instance to compare against this instanhce.
                 *
                 * \return Returns true if the instances are equal.  Returns false if the instances are not equal.
                 */
                bool operator==(const SeriesMinMax& other) const {
                    return currentMinimum == other.currentMinimum && currentMaximum == other.currentMaximum;
                }

                /**
                 * Comparison operator
                 *
                 * \param[in] other The instance to compare against this instanhce.
                 *
                 * \return Returns true if the instances are equal.  Returns false if the instances are not equal.
                 */
                bool operator!=(const SeriesMinMax& other) const {
                    return currentMinimum != other.currentMinimum || currentMaximum != other.currentMaximum;
                }

            private:
                /**
                 * The current minimum value.
                 */
                double currentMinimum;

                /**
                 * The current maximum value.
                 */
                double currentMaximum;
        };

        /**
         * Trivial class that provides return status after updating plot data.
         */
        class SeriesUpdateResults {
            public:
                SeriesUpdateResults() {}

                /**
                 * Constructor
                 *
                 * \param[in] errorReason   The error reason string.  An empty string will indicate no error.
                 *
                 * \param[in] series1MinMax The new minimum and maximum values for series 1.
                 *
                 * \param[in] series2MinMax The new minimum and maximum values for series 2.
                 */
                inline SeriesUpdateResults(
                        const QString&      errorReason,
                        const SeriesMinMax& series1MinMax,
                        const SeriesMinMax& series2MinMax
                    ):currentErrorReason(
                        errorReason
                    ),currentSeries1MinMax(
                        series1MinMax
                    ),currentSeries2MinMax(
                        series2MinMax
                    ) {}

                ~SeriesUpdateResults() = default;

                /**
                 * Method you can use to obtain the current error reason.
                 *
                 * \return Returns the reported error reason.
                 */
                inline const QString& errorReason() const {
                    return currentErrorReason;
                }

                /**
                 * Method you can use to obtain the series 1 min/max values.
                 *
                 * \return Returns the series 1 minimum/maximum values.
                 */
                inline const SeriesMinMax& series1MinMax() const {
                    return currentSeries1MinMax;
                }

                /**
                 * Method you can use to obtain the series 2 min/max values.
                 *
                 * \return Returns the series 2 minimum/maximum values.
                 */
                inline const SeriesMinMax& series2MinMax() const {
                    return currentSeries2MinMax;
                }

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                inline SeriesUpdateResults& operator=(const SeriesUpdateResults& other) {
                    currentErrorReason   = other.currentErrorReason;
                    currentSeries1MinMax = other.currentSeries1MinMax;
                    currentSeries2MinMax = other.currentSeries2MinMax;

                    return *this;
                }

            private:
                /**
                 * The current error reason.
                 */
                QString currentErrorReason;

                /**
                 * The current series 1 minimum and maximum values.
                 */
                SeriesMinMax currentSeries1MinMax;

                /**
                 * The current series 2 minimum and maximum values.
                 */
                SeriesMinMax currentSeries2MinMax;
        };

        /**
         * Trivial class that stores a pair of major/minor step sizes.
         */
        class SeriesStepSizes {
            public:
                constexpr SeriesStepSizes():
                    currentMajorStepSize(0),
                    currentMinorStepSize(0) {}

                /**
                 * Constructor
                 *
                 * \param[in] newMajorStepSize The new major step size.
                 *
                 * \param[in] newMinorStepSize The new minor step size.
                 */
                constexpr SeriesStepSizes(
                        double newMajorStepSize,
                        double newMinorStepSize
                    ):currentMajorStepSize(
                        newMajorStepSize
                    ),currentMinorStepSize(
                        newMinorStepSize
                    ) {}

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                constexpr SeriesStepSizes(
                        const SeriesStepSizes& other
                    ):currentMajorStepSize(
                        other.currentMajorStepSize
                    ),currentMinorStepSize(
                        other.currentMinorStepSize
                    ) {}

                ~SeriesStepSizes() = default;

                /**
                 * Method you can use to set the major step size.
                 *
                 * \param[in] newMajorStepSize The new major step size.
                 */
                inline void setMajorStepSize(double newMajorStepSize) {
                    currentMajorStepSize = newMajorStepSize;
                }

                /**
                 * Method you can use to get the current major step size.
                 *
                 * \return Returns the current major step size.
                 */
                inline double majorStepSize() const {
                    return currentMajorStepSize;
                }

                /**
                 * Method you can use to set the minor step size.
                 *
                 * \param[in] newMinorStepSize The new minor step size.
                 */
                inline void setMinorStepSize(double newMinorStepSize) {
                    currentMinorStepSize = newMinorStepSize;
                }

                /**
                 * Method you can use to get the current minor step size.
                 *
                 * \return Returns the current minor step size.
                 */
                inline double minorStepSize() const {
                    return currentMinorStepSize;
                }

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                SeriesStepSizes& operator=(const SeriesStepSizes& other) {
                    currentMajorStepSize = other.currentMinorStepSize;
                    currentMinorStepSize = other.currentMinorStepSize;

                    return *this;
                }

                /**
                 * Comparison operator
                 *
                 * \param[in] other The instance to compare against this instanhce.
                 *
                 * \return Returns true if the instances are equal.  Returns false if the instances are not equal.
                 */
                bool operator==(const SeriesStepSizes& other) const {
                    return (
                           currentMajorStepSize == other.currentMajorStepSize
                        && currentMinorStepSize == other.currentMinorStepSize
                    );
                }

                /**
                 * Comparison operator
                 *
                 * \param[in] other The instance to compare against this instanhce.
                 *
                 * \return Returns true if the instances are equal.  Returns false if the instances are not equal.
                 */
                bool operator!=(const SeriesStepSizes& other) const {
                    return (
                           currentMajorStepSize != other.currentMajorStepSize
                        || currentMinorStepSize != other.currentMinorStepSize
                    );
                }

            private:
                /**
                 * The current major step size.
                 */
                double currentMajorStepSize;

                /**
                 * The current minor step size.
                 */
                double currentMinorStepSize;
        };

        /**
         * Trivial class used to map a value index to a series and source.
         */
        class SeriesAndSource {
            public:
                /**
                 * Constructor
                 *
                 * \param[in] seriesIndex The zero based series index.
                 *
                 * \param[in] sourceIndex The zero based source index.
                 */
                constexpr SeriesAndSource(
                        unsigned seriesIndex,
                        unsigned sourceIndex
                    ):currentSeriesIndex(
                        seriesIndex
                    ),currentSourceIndex(
                        sourceIndex
                    ) {}

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                constexpr SeriesAndSource(
                        const SeriesAndSource& other
                    ):currentSeriesIndex(
                        other.currentSeriesIndex
                    ),currentSourceIndex(
                        other.currentSourceIndex
                    ) {}

                ~SeriesAndSource() = default;

                /**
                 * Method you can use to get the series index.
                 *
                 * \return Returns the series index.
                 */
                inline unsigned seriesIndex() const {
                    return currentSeriesIndex;
                }

                /**
                 * Method you can use to get the source index.
                 *
                 * \return Returns the source index.
                 */
                inline unsigned sourceIndex() const {
                    return currentSourceIndex;
                }

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                SeriesAndSource& operator=(const SeriesAndSource& other) {
                    currentSeriesIndex = other.currentSeriesIndex;
                    currentSourceIndex = other.currentSourceIndex;

                    return *this;
                }

            private:
                /**
                 * The current series index.
                 */
                unsigned currentSeriesIndex;

                /**
                 * The current source index.
                 */
                unsigned currentSourceIndex;
        };

        /**
         * scale factors used to help quickly find major step size values.  The list ends with a value of 0.
         */
        static const unsigned multiplierScaleFactors[];

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
        virtual SeriesMinMax calculateNiceRange(
            AxisLocation axisLocation,
            double       measuredMinimum,
            double       measuredMaximum,
            AxisScale    axisScale
        ) const;

        /**
         * Convenience method you can use to adjust a minimum and maximum values to a reasonable range.
         *
         * \param[in] axisLocation  The axis we're adjusting our scaling for.
         *
         * \param[in] measuredRange The measured range of values.
         *
         * \param[in] axisScale     The axis scaling mode.  The value is used to determine the algorithm and bounds
         *                          used to calculate a recommended range.
         *
         * \return Returns a pair of values holding the recommended range.
         */
        inline SeriesMinMax calculateNiceRange(
                AxisLocation        axisLocation,
                const SeriesMinMax& measuredRange,
                AxisScale           axisScale
            ) const {
            return calculateNiceRange(axisLocation, measuredRange.minimum(), measuredRange.maximum(), axisScale);
        }

        /**
         * Method that calculates a recommended major and minor step size based on the plot minimum and maximum values.
         *
         * \param[in] axisLocation       The axis we're adjusting our scaling for.  The default implementation ignores
         *                               this value beyond providing it to \ref calculateMinorStepSize.
         *
         * \param[in] span               The span between the minimum and maximum value.
         *
         * \param[in] maximumNumberTicks The maximum number of ticks.
         *
         * \return Returns a pair of value holding the recommended major and minor step sizes.
         */
        virtual SeriesStepSizes calculateNiceStepSizes(
            AxisLocation axisLocation,
            double       span,
            float        maximumNumberTicks
        ) const;

        /**
         * Method that calculates a recommended minor step size based on the a major step size, number of major ticks
         * and maximum number of major ticks.
         *
         * \param[in] axisLocation       The axis we're adjusting our scaling for.  The default implementation ignores
         *                               this value.
         *
         * \param[in] majorStepSize      The major step size.  The minor step size will be an integer fraction of this
         *                               value.
         *
         * \param[in] numberMajorTicks   The number of major ticks on the graph.
         *
         * \param[in] maximumNumberTicks The maximum number of ticks.
         *
         * \return Returns a recommended minor step size.
         */
        virtual double calculateMinorStepSize(
            AxisLocation axisLocation,
            double       majorStepSize,
            unsigned     numberMajorTicks,
            unsigned     maximumNumberTicks
        ) const;

        /**
         * Method you can use to calculate a slope and Y intercept for an ordinary linear regression.
         *
         * \param[in] points A vector of points to calculate the linear regression over.
         *
         * \return Returns a std:pair holding the slope and y intercept as first and second values.
         */
        static std::pair<double, double> calculateOLSLinearRegression(const QVector<QPointF>& points);

    private:
        /**
         * Value used to perform nice scaling.  The value relates roughly to the denominator of the fraction of a plot
         * axis that will be unused, worst case, due to rounding.
         */
        static constexpr double distanceThreshold = 8.0;

        /**
         * The graphics item being tracked by this object.
         */
        QGraphicsItem* currentGraphicsItem;
};

#endif
