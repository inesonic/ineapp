/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BarChartPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BAR_CHART_PRESENTATION_DATA_H
#define BAR_CHART_PRESENTATION_DATA_H

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
 * The \ref PlotPresentationData instance to use for bar charts.
 */
class APP_PUBLIC_API BarChartPresentationData:public Plot2DCategorizedPresentationData {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The chart item containing the plot.
         */
        BarChartPresentationData(EQt::ChartItem* graphicsItem);

        ~BarChartPresentationData() override;

        /**
         * Method that assigns the baseline axis for the plot.  The baseline axis determines the orientation for the
         * bars.
         *
         * \param[in] baselineAxisLocation The baseline axis location.
         */
        void setBaselineAxisLocation(AxisLocation baselineAxisLocation);

        /**
         * Method that indicates the series used for labels.
         *
         * \param[in] seriesIndex The series index of the series defining the labels.
         */
        void setLabelSeriesIndex(unsigned seriesIndex);

        /**
         * Method you can use to indicate no series contains labels.
         */
        void clearLabelSeriesIndex();

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
         * Method you can use to indicate the series index tied to labels.
         *
         * \param[in] seriesIndex    The zero based index of the data source to use for labels.
         *
         * \param[in] valueIndex     The value index to tie to the series labels.
         *
         * \param[in] dataSourceName The name of the data source for labels.
         */
        void setLabelsSeries(unsigned seriesIndex, unsigned valueIndex, const Ld::VariableName& dataSourceName);

        /**
         * Method that configures a data series.  You can use this method to set both the data source for the series
         * and the series look/feel.
         *
         * \param[in] seriesIndex    The zero based series index to be configured.
         *
         * \param[in] valueIndex     The zero based value index that references this series.
         *
         * \param[in] dataSourceName The name of the data source.  This value is primarily used for debugging purposes
         *                           only and can be removed.
         *
         * \param[in] axisLocation   The axis used to determine the length of the bar.
         *
         * \param[in] axisScale      The axis scale to be used.
         *
         * \param[in] seriesLabel    The name for the data series.
         *
         * \param[in] barColor       The color to assign to the series.
         *
         * \return Returns an error string if the supplied series is invalid.
         */
        QString setSeries(
            unsigned                seriesIndex,
            unsigned                valueIndex,
            const Ld::VariableName& dataSourceName,
            AxisLocation            axisLocation,
            AxisScale               axisScale,
            const QString&          seriesLabel,
            const QColor&           barColor
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
                 * \param[in] axisScale      The scaling being applied to this axis.
                 *
                 * \param[in] dataSourceName The name of the data source feeding this series.
                 *
                 * \param[in] axisLocation   The axis location that determines scaling for this bar.
                 *
                 * \param[in] seriesLabel    The label associated with this data source.
                 *
                 * \param[in] barColor       The color to use for this bar.
                 */
                SeriesData(
                    AxisScale               axisScale,
                    const Ld::VariableName& dataSourceName,
                    AxisLocation            axisLocation,
                    const QString&          seriesLabel,
                    const QColor&           barColor
                );

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                SeriesData(const SeriesData& other);

                ~SeriesData() override;

                /**
                 * Method you can use to obtain the axis location.
                 *
                 * \return Returns the axis location.
                 */
                AxisLocation axisLocation() const {
                    return currentAxisLocation;
                }

                /**
                 * Method you can use to obtain the bar color.
                 *
                 * \return Returns the bar color.
                 */
                const QColor& barColor() const {
                    return currentBarColor;
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
                 * The axis location.
                 */
                AxisLocation currentAxisLocation;

                /**
                 * The series bar color.
                 */
                QColor currentBarColor;
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
         * Method that updates the marker color for one or more markers.
         *
         * \param[in] markerSize  The size of the marker, in pixels.
         *
         * \param[in] markerList  The list of markers to be updated.
         *
         * \param[in] markerColor The marker color.
         */
        static void setMarkerColor(
            unsigned                     markerSize,
            const QList<QLegendMarker*>& markerList,
            const QColor&                markerColor
        );

        /**
         * The series index tied to labels.  Set to static_cast<unsigned>(-1) if there is no series index used to
         * indicate programmatically generated labels.
         */
        unsigned currentLabelSeriesIndex;

        /**
         * List of variants holding the provided calculated values.
         */
        QList<SeriesData> currentSeriesData;

        /**
         * Map of axis locations for this plot.
         */
        QHash<AxisLocation, AxisData> axisDataByLocation;

        /**
         * The current baseline axis location for the chart.
         */
        AxisLocation currentBaselineAxisLocation;

        /**
         * The current list of plot series.
         */
        QList<QBarSeries*> currentBarSeries;
};

#endif
