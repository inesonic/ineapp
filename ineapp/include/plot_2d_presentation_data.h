/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Plot2DPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_2D_PRESENTATION_DATA_H
#define PLOT_2D_PRESENTATION_DATA_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QTimer>
#include <QVariant>

#include <eqt_charts.h>

#include <ld_plot_element.h>
#include <ld_plot_series.h>

#include "app_common.h"
#include "plot_wrapped_data_presentation_data.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsItem;
    class ChartItem;
}

namespace Ld {
    class CalculatedValue;
}

/**
 * Pure virtual base class you can use to manage plots and plot data on two dimensions.
 */
class APP_PUBLIC_API Plot2DPresentationData:public PlotWrappedDataPresentationData {
    public:
        /**
         * Typedef indicating the axis locations.
         */
        typedef Ld::PlotElement::AxisLocation AxisLocation;

        /**
         * Typedef indicating the axis plot series.
         */
        typedef Ld::PlotSeries::SplineType SplineType;

        /**
         * Constructor
         *
         * \param[in] graphicsItem The graphics item to be managed.  This instance must be derived from both
         *                         EQt::GraphicsItem and QChart
         */
        Plot2DPresentationData(EQt::GraphicsItem* graphicsItem = Q_NULLPTR);

        ~Plot2DPresentationData() override;

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
        virtual void setAxisFormat(
            AxisLocation               axisLocation,
            const Ld::ChartAxisFormat& axisFormat,
            QAbstractAxis*             axis
        );

        /**
         * Method that assigns a new series.
         *
         * \param[in] seriesIndex The zero based series index.
         *
         * \param[in] splineType  The spline type being used.  The value will be applied to series 1.
         *
         * \param[in] series1     The Qt series.
         *
         * \param[in] series2     The second Qt series.
         */
        virtual void setSeries(
            unsigned   seriesIndex,
            SplineType splineType,
            QXYSeries* series1,
            QXYSeries* series2 = Q_NULLPTR
        );

        /**
         * Method that assigns a data source to series.
         *
         * \param[in] valueIndex     The zero based index that will be used to assign a calculated value to this
         *                           series.
         *
         * \param[in] seriesIndex    The zero based series index.
         *
         * \param[in] sourceIndex    The zero based source index.
         *
         * \param[in] axisLocation   The axis associated with the data source.
         *
         * \param[in] dataSourceName The data source name for this data source.
         */
        virtual void setDataSource(
            unsigned                valueIndex,
            unsigned                seriesIndex,
            unsigned                sourceIndex,
            AxisLocation            axisLocation,
            const Ld::VariableName& dataSourceName
        );

        /**
         * Method that clears all the source data.
         */
        void clearDataSources();

    protected slots:
        /**
         * Slot that is triggered in our event loop after we receive updates.
         */
        void performDeferredUpdates() override;

    protected:
        /**
         * Value holding the number of source per series.
         */
        static constexpr unsigned numberSourcesPerSeries = 2;

        /**
         * Class used internally to track series data.
         */
        class SeriesData:public PlotWrappedDataPresentationData::SeriesData {
            public:
                /**
                 * Constructor
                 *
                 * \param[in] splineType The spline type used for series 1.
                 *
                 * \param[in] series1    The first Qt series.
                 *
                 * \param[in] series2    The second Qt series.
                 */
                SeriesData(SplineType splineType, QXYSeries* series1, QXYSeries* series2);

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                SeriesData(const SeriesData& other);

                ~SeriesData() override;

                /**
                 * Method you can use to assign a data source to this series.
                 *
                 * \param[in] sourceIndex    The source index within this series.  The value must be 0 or 1.  This
                 *                           method will assert for all other values.
                 *
                 * \param[in] axisLocation   The axis tied to the data source.
                 *
                 * \param[in] axisScale      The axis scaling value.
                 *
                 * \param[in] dataSourceName The name of the data source.
                 */
                void setDataSource(
                    unsigned                sourceIndex,
                    AxisLocation            axisLocation,
                    AxisScale               axisScale,
                    const Ld::VariableName& dataSourceName
                );

                /**
                 * Method you can use to set the calculated value for a data source.
                 *
                 * \param[in] sourceIndex        The zero based source index.
                 *
                 * \param[in] newCalculatedValue The newly calculated value.
                 */
                void setCalculatedValue(unsigned sourceIndex, const Ld::CalculatedValue& newCalculatedValue) override;

                /**
                 * Method you can use to clear the calculated values.
                 */
                void calculatedValuesCleared() override;

                /**
                 * Method you can use to get the spline type.
                 *
                 * \return Returns the selected spline type.
                 */
                inline SplineType splineType() const {
                    return currentSplineType;
                }

                /**
                 * Method you can use to get the first series.
                 *
                 * \return Returns the selected series.
                 */
                inline QXYSeries* series1() const {
                    return currentSeries1;
                }

                /**
                 * Method you can use to get the second series.
                 *
                 * \return Returns the selected series.
                 */
                inline QXYSeries* series2() const {
                    return currentSeries2;
                }

                /**
                 * Method you can use to get the axis location for a data source.
                 *
                 * \param[in] sourceIndex The source index of interest.  This method will assert if the value is not
                 *                        0 or 1.
                 *
                 * \return Returns the axis tied to this data source.
                 */
                AxisLocation axisLocation(unsigned sourceIndex) const {
                    Q_ASSERT(sourceIndex < 2);
                    return currentAxisLocations[sourceIndex];
                }

                /**
                 * Method you can use to get the name of the data source.
                 *
                 * \param[in] sourceIndex The source index of interest.  This method will assert if the value is not
                 *                        0 or 1.
                 *
                 * \return Returns the data source name.
                 */
                const Ld::VariableName& dataSourceName(unsigned sourceIndex) const {
                    Q_ASSERT(sourceIndex < 2);
                    return currentDataSourceNames[sourceIndex];
                }

                /**
                 * Method you can use to update the data series values.
                 *
                 * \param[in] seriesIndex The zero based series index.  The value is used for error reporting.
                 *
                 * \return Returns a class holding the series update result.
                 */
                SeriesUpdateResults updateSeries(unsigned seriesIndex);

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
                 * Method used internally to determine the number of points in a data source.
                 *
                 * \param[in]  variant     The variant to analyze.
                 *
                 * \param[in]  seriesIndex The zero based index of the series.
                 *
                 * \param[out] errorReason A string that will be populated with any error reason code.
                 *
                 * \return Returns the length of the source, in entries.
                 */
                static unsigned long sourceLength(
                    const Model::Variant& variant,
                    unsigned              seriesIndex,
                    QString&              errorReason
                );

                /**
                 * The spline type associated with this series.
                 */
                SplineType currentSplineType;

                /**
                 * The first series.
                 */
                QXYSeries* currentSeries1;

                /**
                 * The second series.
                 */
                QXYSeries* currentSeries2;

                /**
                 * The axis location for each source.
                 */
                AxisLocation currentAxisLocations[2];

                /**
                 * The current data source names.
                 */
                Ld::VariableName currentDataSourceNames[2];

                /**
                 * The axis scaling for the data source.
                 */
                AxisScale currentAxisScaling[2];

                /**
                 * The currently reported calculated values.
                 */
                Model::Variant currentValues[2];

                /**
                 * The number of series points.  A value of static_cast<unsigned>(-1) indicates that the value needs
                 * to be calculated.
                 */
                mutable unsigned currentNumberSeriesValues;
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
        QList<SeriesData> currentSeriesData;

        /**
         * Map of axis locations for this plot.
         */
        QHash<AxisLocation, AxisData> axisDataByLocation;
};

#endif
