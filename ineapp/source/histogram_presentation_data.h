/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HistogramPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HISTOGRAM_PRESENTATION_DATA_H
#define HISTOGRAM_PRESENTATION_DATA_H

#include <QObject>
#include <QList>
#include <QLegendMarker>

#include <eqt_charts.h>

#include "app_common.h"
#include "plot_2d_categorized_presentation_data.h"

namespace EQt {
    class ChartItem;
}

namespace Ld {
    class VariableName;
    class CalculatedValue;
}

/**
 * The \ref PlotPresentationData instance to use for histograms.
 */
class APP_PUBLIC_API HistogramPresentationData:public Plot2DCategorizedPresentationData {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The chart item containing the plot.
         */
        HistogramPresentationData(EQt::ChartItem* graphicsItem);

        ~HistogramPresentationData() override;

        /**
         * Method that assigns the baseline axis for the plot.  The baseline axis determines the orientation for the
         * bars.
         *
         * \param[in] baselineAxisLocation The baseline axis location.
         */
        void setBaselineAxisLocation(AxisLocation baselineAxisLocation);

        /**
         * Method that assigns a format to an axis.  Be sure to set the axis format before adding any series or
         * data sources so that the axis scaling can be properly handled.
         *
         * \param[in] axisLocation The location of the axis.
         *
         * \param[in] axisFormat   The axis format to be assigned.
         *
         * \param[in] axis         The Qt axis associated with the format.
         */
        void setAxisFormat(
            AxisLocation               axisLocation,
            const Ld::ChartAxisFormat& axisFormat,
            QAbstractAxis*             axis
        );

        /**
         * Method you can use to clear all the series data.
         */
        void clearSeries();

        /**
         * Method that configures a data series.  You can use this method to set both the data source for the series
         * and the series look/feel.
         *
         * \param[in] dataSourceName The name of the data source.  This value is primarily used for debugging purposes
         *                           only and can be removed.
         *
         * \param[in] axisScale      The scaling for the values.
         *
         * \param[in] seriesFormat   The format for the data series.
         *
         * \param[in] seriesLabel    The name for the data series.
         *
         * \return Returns an error string if the supplied series is invalid.
         */
        QString setSeries(
            const Ld::VariableName& dataSourceName,
            AxisScale               axisScale,
            const Ld::PlotSeries&   seriesFormat,
            const QString&          seriesLabel
        );

    protected slots:
        /**
         * Slot that is triggered in our event loop after we receive updates.
         */
        void performDeferredUpdates() override;

    protected:
        /**
         * Class used internally to track series data.
         */
        class SeriesData:public Plot2DCategorizedPresentationData::SeriesData {
            public:
                SeriesData();

                /**
                 * Constructor
                 *
                 * \param[in] dataSourceName The name of the data source feeding this series.
                 *
                 * \param[in] axisScale      The scaling for the series vertical axis.
                 *
                 * \param[in] seriesFormat   The series format tied to this series.
                 *
                 * \param[in] seriesLabel    The label associated with this data source.
                 */
                SeriesData(
                    const Ld::VariableName& dataSourceName,
                    AxisScale               axisScale,
                    const Ld::PlotSeries&   seriesFormat,
                    const QString&          seriesLabel
                );

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                SeriesData(const SeriesData& other);

                ~SeriesData() override;

                /**
                 * Method you can use to set the plot series format tied to this series.
                 *
                 * \param[in] newSeriesFormat The new series format to be applied.
                 */
                void setFormat(const Ld::PlotSeries& newSeriesFormat) {
                    currentSeriesFormat = newSeriesFormat;
                }

                /**
                 * Method you can use to obtain the series format.
                 *
                 * \return Returns the series format tied to this series.
                 */
                const Ld::PlotSeries& format() const {
                    return currentSeriesFormat;
                }

                /**
                 * Assignment operator.
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                SeriesData& operator=(const SeriesData& other);

            private:
                /**
                 * The series format tied to this series.
                 */
                Ld::PlotSeries currentSeriesFormat;
        };

        /**
         * Method that provides the series data instance for a given value index.
         *
         * \param[in] valueIndex The zero based index to the data series.
         *
         * \return Returns a pointer to the requested series data instance.   A null pointer is returned on error.
         */
        SeriesData* seriesData(unsigned valueIndex) override;

        /**
         * Method you should overload to provide the number of series data values.
         *
         * \return Returns the number of series data values.
         */
        unsigned numberSeriesDataValues() const override;

        /**
         * List of variants holding the provided calculated values.
         */
        SeriesData currentSeriesData;

        /**
         * Map of axis locations for this plot.
         */
        QHash<AxisLocation, AxisData> axisDataByLocation;

        /**
         * The current baseline axis location for the chart.
         */
        AxisLocation currentBaselineAxisLocation;
};

#endif
