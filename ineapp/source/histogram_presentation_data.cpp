/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HistogramPresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QBarSet>
#include <QBarSeries>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QHorizontalBarSeries>
#include <QLegend>
#include <QLegendMarker>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QBrush>

#include <cmath>

#include <eqt_charts.h>
#include <eqt_chart_item.h>

#include <model_tuple.h>
#include <model_matrix_boolean.h>
#include <model_matrix_integer.h>
#include <model_matrix_real.h>
#include <model_matrix_complex.h>
#include <m_basic_functions.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>
#include <ld_data_type.h>

#include "plot_2d_categorized_presentation_data.h"
#include "histogram_presentation_data.h"

/***********************************************************************************************************************
 * HistogramPresentationData::SeriesData
 */

HistogramPresentationData::SeriesData::SeriesData() {}


HistogramPresentationData::SeriesData::SeriesData(
        const Ld::VariableName&              dataSourceName,
        HistogramPresentationData::AxisScale axisScale,
        const Ld::PlotSeries&                seriesFormat,
        const QString&                       seriesLabel
    ):Plot2DCategorizedPresentationData::SeriesData(
        axisScale,
        dataSourceName,
        seriesLabel
    ),currentSeriesFormat(
        seriesFormat
    ) {}


HistogramPresentationData::SeriesData::SeriesData(
        const SeriesData& other
    ):Plot2DCategorizedPresentationData::SeriesData(
        other
    ),currentSeriesFormat(
        other.currentSeriesFormat
    ) {}


HistogramPresentationData::SeriesData::~SeriesData() {}


HistogramPresentationData::SeriesData& HistogramPresentationData::SeriesData::operator=(
        const HistogramPresentationData::SeriesData& other
    ) {
    Plot2DCategorizedPresentationData::SeriesData::operator=(other);
    currentSeriesFormat = other.currentSeriesFormat;

    return *this;
}

/***********************************************************************************************************************
 * HistogramPresentationData
 */

HistogramPresentationData::HistogramPresentationData(
        EQt::ChartItem* chartItem
    ):Plot2DCategorizedPresentationData(
        chartItem
    ) {}


HistogramPresentationData::~HistogramPresentationData() {}


void HistogramPresentationData::setBaselineAxisLocation(HistogramPresentationData::AxisLocation baselineAxisLocation) {
    currentBaselineAxisLocation = baselineAxisLocation;
}


void HistogramPresentationData::setAxisFormat(
        HistogramPresentationData::AxisLocation axisLocation,
        const Ld::ChartAxisFormat&              axisFormat,
        QAbstractAxis*                          axis
    ) {
    axisDataByLocation.insert(axisLocation, AxisData(axisLocation, axisFormat, axis));
}


void HistogramPresentationData::clearSeries() {
    currentSeriesData = SeriesData();
}


QString HistogramPresentationData::setSeries(
        const Ld::VariableName&              dataSourceName,
        HistogramPresentationData::AxisScale axisScale,
        const Ld::PlotSeries&                seriesFormat,
        const QString&                       seriesLabel
    ) {
    currentSeriesData = SeriesData(dataSourceName, axisScale, seriesFormat, seriesLabel);
    mapValueIndex(0, 0, 0);

    return QString();
}


void HistogramPresentationData::performDeferredUpdates() {
    EQt::ChartItem* chart = dynamic_cast<EQt::ChartItem*>(chartItem());

    SeriesMinMax    baselineSeriesMinMax;
    QVector<double> sampleData = currentSeriesData.toVector(&baselineSeriesMinMax);

    const AxisData&            baselineAxisData   = axisDataByLocation.value(currentBaselineAxisLocation);
    SeriesMinMaxAndErrorStatus baselineAxisMinMax = configureAxis(baselineAxisData, baselineSeriesMinMax);
    QString                    errorString        = baselineAxisMinMax.errorReason();

    if (errorString.isEmpty()) {
        const Ld::ChartAxisFormat& baselineAxisFormat = baselineAxisData.axisFormat();
        if (baselineAxisFormat.axisScale() == AxisScale::LOG) {
            errorString = tr("Log scale is not supported for historgrams.");
        }
    }

    if (errorString.isEmpty() && baselineAxisMinMax.minimum() >= baselineAxisMinMax.maximum()) {
        errorString = tr("Histograms do not support reversed axis scales.");
    }

    if (errorString.isEmpty()) {
        QVector<double> bucketCenters;
        QVector<double> buckets;

        QValueAxis* baselineAxis   = dynamic_cast<QValueAxis*>(baselineAxisData.axis());
        double      lower          = baselineAxisMinMax.minimum();
        double      upper          = baselineAxisMinMax.maximum();
        unsigned    minorTickCount = static_cast<unsigned>(baselineAxis->minorTickCount());

        unsigned numberBuckets;
        double   bucketWidth;
        double   majorTickInterval = baselineAxis->tickInterval();
        if (majorTickInterval != 0 && !std::isnan(majorTickInterval)) {
            bucketWidth = majorTickInterval / (minorTickCount + 1);
            numberBuckets = static_cast<unsigned>(0.5 + std::ceil(std::abs(upper - lower) / bucketWidth));
        } else {
            unsigned numberMajorTicks = baselineAxis->tickCount();
            numberBuckets = (numberMajorTicks - 1) * (minorTickCount + 1);
            bucketWidth = std::abs(upper - lower) / numberBuckets;
        }

        double bucketHalfWidth = bucketWidth / 2.0;

        buckets.resize(numberBuckets);
        for (unsigned index=0 ; index<numberBuckets ; ++index) {
            bucketCenters.append(lower + bucketHalfWidth + bucketWidth * index);
        }

        unsigned long numberSamples = static_cast<unsigned long>(sampleData.size());
        double        maximumValue  = 0;
        for (unsigned index=0 ; index<numberSamples ; ++index) {
            double v = sampleData.at(index);
            if (v >= lower && v <= upper) {
                int bucketIndex = static_cast<int>(std::floor((v - lower) / bucketWidth));
                if (static_cast<unsigned>(bucketIndex) == numberBuckets) {
                    bucketIndex = numberBuckets - 1;
                }

                double value = buckets.at(bucketIndex) + 1;
                if (value > maximumValue) {
                    maximumValue = value;
                }
                buckets[bucketIndex] = value;
            }
        }

        AxisLocation scaleAxisLocation;

        if (currentBaselineAxisLocation == AxisLocation::BOTTOM_X_A_GM) {
            scaleAxisLocation = AxisLocation::LEFT_Y_R_RC;
        } else {
            scaleAxisLocation = AxisLocation::BOTTOM_X_A_GM;
        }

        const AxisData&            scaleAxisData = axisDataByLocation.value(scaleAxisLocation);
        SeriesMinMaxAndErrorStatus scaleMinMax   = configureAxis(scaleAxisData, SeriesMinMax(0, maximumValue));
        errorString = scaleMinMax.errorReason();

        if (errorString.isEmpty()) {
            chart->removeAllSeries();

            QAbstractBarSeries* barSeries;
            if (currentBaselineAxisLocation == AxisLocation::BOTTOM_X_A_GM) {
                barSeries = new QBarSeries;
            } else {
                barSeries = new QHorizontalBarSeries;
            }

            barSeries->attachAxis(scaleAxisData.axis());

            QBarSet* barSet = new QBarSet("");
            for (unsigned index=0 ; index<numberBuckets ; ++index) {
                barSet->append(buckets.at(index));
            }

            barSet->setBrush(QColor(currentSeriesData.format().lineColor()));
            barSeries->append(barSet);

            chart->addSeries(barSeries);
        }
    }

    if (!errorString.isEmpty()) {
        showErrorMessage(errorString);
    } else {
        clearErrorMessage();
    }
}


HistogramPresentationData::SeriesData* HistogramPresentationData::seriesData(unsigned valueIndex) {
    return valueIndex == 0 ? &currentSeriesData : Q_NULLPTR;
}


unsigned HistogramPresentationData::numberSeriesDataValues() const {
    return 1;
}
