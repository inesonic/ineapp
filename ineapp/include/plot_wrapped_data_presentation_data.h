/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotWrappedDataPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_WRAPPED_DATA_PRESENTATION_DATA_H
#define PLOT_WRAPPED_DATA_PRESENTATION_DATA_H

#include <QObject>
#include <QString>
#include <QTimer>

#include "app_common.h"
#include "plot_wrapped_presentation_data.h"

class QGraphicsItem;

namespace QtCharts {
    class QChart;
    class QAbstractAxis;
};

namespace EQt {
    class GraphicsItem;
    class ChartItem;
}

namespace Ld {
    class ChartAxisFormat;
}

/**
 * Pure virtual base class you can use to manage plots.  The class extends \ref PlotWrappedPresentationData to provide
 * handling of multiple calculated data fields with deferred updates.
 */
class APP_PUBLIC_API PlotWrappedDataPresentationData:public QObject, public PlotWrappedPresentationData {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The graphics item to be managed.  This instance must be derived from both
         *                         EQt::GraphicsItem and QtCharts::QChart
         */
        PlotWrappedDataPresentationData(EQt::GraphicsItem* graphicsItem = Q_NULLPTR);

        ~PlotWrappedDataPresentationData() override;

        /**
         * Method you can call to report a new calculated value to the plot.
         *
         * \param[in] valueIndex      The index used to reference the calculated value.
         *
         * \param[in] calculatedValue The new calculated value.
         */
        void calculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue) override;

        /**
         * Method you can call to clear all the reported calculated values.
         */
        void calculatedValuesCleared() override;

    protected slots:
        /**
         * Slot that is triggered in our event loop after we receive updates.
         */
        virtual void performDeferredUpdates() = 0;

    protected:
        /**
         * Trivial class that provides a minimum, maximum value and error status.
         */
        class SeriesMinMaxAndErrorStatus:public SeriesMinMax {
            public:
                SeriesMinMaxAndErrorStatus():SeriesMinMax() {}

                /**
                 * Constructor
                 *
                 * \param[in] newMinimum  The new minimum value.
                 *
                 * \param[in] newMaximum  The new maximum value.
                 *
                 * \param[in] errorReason The error reason message.  An empty string indicates no error.
                 */
                SeriesMinMaxAndErrorStatus(
                        double         newMinimum,
                        double         newMaximum,
                        const QString& errorReason = QString()
                    ):SeriesMinMax(
                        newMinimum,
                        newMaximum
                    ),currentErrorReason(
                        errorReason
                    ) {}

                /**
                 * Constructor
                 *
                 * \param[in] newMinMax   The new minimum and maximum values.
                 *
                 * \param[in] errorReason The error reason message.  An empty string indicates no error.
                 */
                SeriesMinMaxAndErrorStatus(
                        const SeriesMinMax& newMinMax,
                        const QString&      errorReason = QString()
                    ):SeriesMinMax(
                        newMinMax
                    ),currentErrorReason(
                        errorReason
                    ) {}

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                SeriesMinMaxAndErrorStatus(
                        const SeriesMinMaxAndErrorStatus& other
                    ):SeriesMinMax(
                        other
                    ),currentErrorReason(
                        other.currentErrorReason
                    ) {}

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                SeriesMinMaxAndErrorStatus(
                        const SeriesMinMax& other
                    ):SeriesMinMax(
                        other
                    ) {}

                ~SeriesMinMaxAndErrorStatus() = default;

                /**
                 * Method you can use to determine if an error was reported.
                 *
                 * \return Returns true if an error was reported.  Returns false if no error was reported.
                 */
                bool errorReported() const {
                    return !currentErrorReason.isEmpty();
                }

                /**
                 * Method you can use to obtain the current error reason.
                 *
                 * \return Returns the current error reason.
                 */
                QString errorReason() const {
                    return currentErrorReason;
                }

                /**
                 * Method you can use to update the error resaon.
                 *
                 * \param[in] newErrorReason The new error message.
                 */
                void setErrorReason(const QString& newErrorReason) {
                    currentErrorReason = newErrorReason;
                }

                /**
                 * Method you can use to clear the current error reason.
                 */
                void clearError() {
                    currentErrorReason.clear();
                }

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                SeriesMinMaxAndErrorStatus& operator=(const SeriesMinMaxAndErrorStatus& other) {
                    SeriesMinMax::operator=(other);
                    currentErrorReason = other.currentErrorReason;

                    return *this;
                }

            private:
                /**
                 * The current error message.
                 */
                QString currentErrorReason;
        };

        /**
         * Pure virtual base class for all series data.
         */
        class SeriesData {
            public:
                SeriesData();

                virtual ~SeriesData();

                /**
                 * Method you can use to set the calculated value for a data source.
                 *
                 * \param[in] sourceIndex        The zero based source index.
                 *
                 * \param[in] newCalculatedValue The newly calculated value.
                 */
                virtual void setCalculatedValue(
                    unsigned                   sourceIndex,
                    const Ld::CalculatedValue& newCalculatedValue
                )= 0;

                /**
                 * Method you can use to clear the calculated values.
                 */
                virtual void calculatedValuesCleared() = 0;

            protected:
                /**
                 * Method used internally to adjust the plot value based on axis scaling.
                 *
                 * \param[in]     value     The input value.
                 *
                 * \param[in]     axisScale The axis scaling.
                 *
                 * \param[in,out] ok        Reference to a boolean value that will be set to false if the provided
                 *                          value is invalid.  This method will do nothing if the input value is false.
                 *
                 * \return Returns the properly adjusted value.  A reasonable value for the scaling will be returned
                 *         if ok is false.
                 */
                static double rescaleValue(double value, AxisScale axisScale, bool& ok);
        };

        /**
         * Method you can use to configure this axis based on a supplied set of minimum and maximum values.
         *
         * \param[in] axisData      The axis data instance to be configured.
         *
         * \param[in] measuredRange The measured range of values.  The value is ignored if the format fully specifies
         *                          the minimum, maximum, and step size values.
         *
         * \return Returns a \ref PlotPresentationData::SeriesMinMax instance holding the actual axis minimum
         *         and maximum value used.
         */
        SeriesMinMaxAndErrorStatus configureAxis(const AxisData& axisData, const SeriesMinMax& measuredRange);

        /**
         * Method you should overload to indicate whether an axis is modulo or not.
         *
         * \param[in] axisLocation The axis location of interest.
         *
         * \return Returns true if the axis is a modulo axis.  Returns false if the axis is not a modulo axis.  The
         *         default implementation always returns false.
         */
        virtual bool isModuloAxis(AxisLocation axisLocation) const;

        /**
         * Method you should overload to provide the series data instance for a given value index.
         *
         * \param[in] valueIndex The zero based index to the data value.
         *
         * \return Returns a pointer to the requested series data instance.   A null pointer is returned on error.
         */
        virtual SeriesData* seriesData(unsigned valueIndex) = 0;

        /**
         * Method you should overload to provide the number of series data values.
         *
         * \return Returns the number of series data values.
         */
        virtual unsigned numberSeriesDataValues() const = 0;

        /**
         * Method you can use to map a data series and data source to a series index.
         *
         * \param[in] seriesIndex The zero based data series index.
         *
         * \param[in] sourceIndex The zero based source index.
         *
         * \param[in] valueIndex  The zero based value index.
         */
        void mapValueIndex(unsigned seriesIndex, unsigned sourceIndex, unsigned valueIndex);

    private:
        /**
         * Timer used to defer updates.
         */
        QTimer updateTimer;

        /**
         * List used to map a value index to a series and source index.
         */
        QList<SeriesAndSource> seriesAndSourceByValueIndex;
};

#endif
