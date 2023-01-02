/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BarChartPresentationData class.
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
#include "bar_chart_presentation_data.h"

/***********************************************************************************************************************
 * BarChartPresentationData::SeriesData
 */

BarChartPresentationData::SeriesData::SeriesData() {
    currentAxisLocation = AxisLocation::BOTTOM_X_A_GM;
}


BarChartPresentationData::SeriesData::SeriesData(
        BarChartPresentationData::AxisScale    axisScale,
        const Ld::VariableName&                dataSourceName,
        BarChartPresentationData::AxisLocation axisLocation,
        const QString&                         seriesLabel,
        const QColor&                          barColor
    ):Plot2DCategorizedPresentationData::SeriesData(
        axisScale,
        dataSourceName,
        seriesLabel
    ),currentAxisLocation(
        axisLocation
    ),currentBarColor(
        barColor
    ) {}


BarChartPresentationData::SeriesData::SeriesData(
        const SeriesData& other
    ):Plot2DCategorizedPresentationData::SeriesData(
        other
    ),currentAxisLocation(
        other.currentAxisLocation
    ),currentBarColor(
        other.currentBarColor
    ) {}


BarChartPresentationData::SeriesData::~SeriesData() {}


BarChartPresentationData::SeriesData& BarChartPresentationData::SeriesData::operator=(
        const BarChartPresentationData::SeriesData& other
    ) {
    Plot2DCategorizedPresentationData::SeriesData::operator=(other);
    currentAxisLocation   = other.currentAxisLocation;
    currentBarColor       = other.currentBarColor;

    return *this;
}

/***********************************************************************************************************************
 * BarChartPresentationData
 */

BarChartPresentationData::BarChartPresentationData(
        EQt::ChartItem* chartItem
    ):Plot2DCategorizedPresentationData(
        chartItem
    ) {
    currentLabelSeriesIndex = static_cast<unsigned>(-1);
}


BarChartPresentationData::~BarChartPresentationData() {}


void BarChartPresentationData::setBaselineAxisLocation(BarChartPresentationData::AxisLocation baselineAxisLocation) {
    currentBaselineAxisLocation = baselineAxisLocation;
}


void BarChartPresentationData::setLabelSeriesIndex(unsigned int seriesIndex) {
    currentLabelSeriesIndex = seriesIndex;
}


void BarChartPresentationData::clearLabelSeriesIndex() {
    currentLabelSeriesIndex = static_cast<unsigned>(-1);
}


void BarChartPresentationData::setAxisFormat(
        BarChartPresentationData::AxisLocation axisLocation,
        const Ld::ChartAxisFormat&             axisFormat,
        QAbstractAxis*                         axis
    ) {
    axisDataByLocation.insert(axisLocation, AxisData(axisLocation, axisFormat, axis));
}


void BarChartPresentationData::clearSeries() {
    currentSeriesData.clear();
    currentLabelSeriesIndex = static_cast<unsigned>(-1);
}


void BarChartPresentationData::setLabelsSeries(
        unsigned                seriesIndex,
        unsigned                valueIndex,
        const Ld::VariableName& dataSourceName
    ) {
    if (seriesIndex < static_cast<unsigned>(currentSeriesData.size())) {
        currentSeriesData[seriesIndex] = SeriesData(
            AxisScale::LINEAR,
            dataSourceName,
            currentBaselineAxisLocation,
            QString(),
            QColor()
        );
    } else {
        while (seriesIndex > static_cast<unsigned>(currentSeriesData.size())) {
            currentSeriesData.append(SeriesData());
        }

        currentSeriesData.append(
            SeriesData(
                AxisScale::LINEAR,
                dataSourceName,
                currentBaselineAxisLocation,
                QString(),
                QColor()
            )
        );
    }

    mapValueIndex(seriesIndex, 0, valueIndex);
}


QString BarChartPresentationData::setSeries(
        unsigned                               seriesIndex,
        unsigned                               valueIndex,
        const Ld::VariableName&                dataSourceName,
        BarChartPresentationData::AxisLocation axisLocation,
        BarChartPresentationData::AxisScale    axisScale,
        const QString&                         seriesLabel,
        const QColor&                          barColor
    ) {
    QString result;

    if (seriesIndex < static_cast<unsigned>(currentSeriesData.size())) {
        currentSeriesData[seriesIndex] = SeriesData(
            axisScale,
            dataSourceName,
            axisLocation,
            seriesLabel,
            barColor
        );
    } else {
        while (seriesIndex > static_cast<unsigned>(currentSeriesData.size())) {
            currentSeriesData.append(SeriesData());
        }

        currentSeriesData.append(SeriesData(axisScale, dataSourceName, axisLocation, seriesLabel, barColor));
    }

    mapValueIndex(seriesIndex, 0, valueIndex);

    return result;
}


void BarChartPresentationData::performDeferredUpdates() {
    EQt::ChartItem* chart                    = dynamic_cast<EQt::ChartItem*>(chartItem());
    unsigned        numberSeries             = static_cast<unsigned>(currentSeriesData.size());
    unsigned        numberGroups             = static_cast<unsigned>(-1);
    unsigned        numberDataSeries         = numberSeries;
    unsigned        numberProgrammaticLabels = 0;
    QString         errorString;

    QStringList programmaticLabels;
    if (currentLabelSeriesIndex != static_cast<unsigned>(-1)) {
        --numberDataSeries;

        const SeriesData& labelSeriesData = currentSeriesData.at(currentLabelSeriesIndex);

        bool isOk;
        programmaticLabels = labelSeriesData.toStringList(&isOk);

        if (!isOk) {
            errorString = tr("Series %1 does not contain appropriate values for labels.")
                          .arg(currentLabelSeriesIndex + 1);
        }

        numberProgrammaticLabels = static_cast<unsigned>(programmaticLabels.size());
    }

    SeriesMinMax           seriesMinMax;
    QStringList            seriesLabels;
    QList<QVector<double>> valuesBySeries;
    unsigned               seriesIndex = 0;
    while (errorString.isEmpty() && seriesIndex < numberSeries) {
        if (seriesIndex != currentLabelSeriesIndex) {
            bool              isOk;
            const SeriesData& seriesData   = currentSeriesData[seriesIndex];
            QVector<double>   seriesValues = seriesData.toVector(&seriesMinMax, &isOk);

            if (numberGroups == static_cast<unsigned>(-1)) {
                numberGroups = static_cast<unsigned>(seriesValues.size());
                if (numberGroups == 0) {
                    errorString = tr("Data series can-not be empty.");
                }
            } else if (numberGroups != static_cast<unsigned>(seriesValues.size())) {
                errorString = tr("All series must have the same number of values.");
            }

            seriesLabels.append(seriesData.seriesLabel());
            valuesBySeries.append(seriesValues);
        }

        ++seriesIndex;
    }

    if (errorString.isEmpty()) {
        if (seriesMinMax.minimum() < 0) {
            errorString = tr("Bar charts can-not have negative values.");
        } else {
            seriesMinMax.update(0);
        }
    }

    const AxisData&   baselineAxisData  = axisDataByLocation.value(currentBaselineAxisLocation);
    QBarCategoryAxis* baselineAxis      = dynamic_cast<QBarCategoryAxis*>(baselineAxisData.axis());
    AxisLocation      scaleAxisLocation =   currentBaselineAxisLocation == AxisLocation::BOTTOM_X_A_GM
                                                    ? AxisLocation::LEFT_Y_R_RC
                                                    : AxisLocation::BOTTOM_X_A_GM;
    const AxisData&   scaleAxisData     = axisDataByLocation.value(scaleAxisLocation);
    QValueAxis*       scaleAxis         = dynamic_cast<QValueAxis*>(scaleAxisData.axis());

    if (errorString.isEmpty()) {
        SeriesMinMaxAndErrorStatus adjustedRange = configureAxis(scaleAxisData, seriesMinMax);
        errorString = adjustedRange.errorReason();
    }

    if (errorString.isEmpty()) {
        baselineAxis->clear();
        chart->removeAllSeries();

        QFontMetrics fontMetrics(chart->legend()->font());
        unsigned     markerSize = fontMetrics.ascent();

        if (numberGroups == 1 || numberDataSeries == 1) {
            if (numberProgrammaticLabels > 0) {
                seriesLabels = programmaticLabels;
            }

            QAbstractBarSeries* barSeries;
            if (currentBaselineAxisLocation == AxisLocation::BOTTOM_X_A_GM) {
                barSeries = new QBarSeries;
            } else {
                barSeries = new QHorizontalBarSeries;
            }

            if (numberGroups > 1) {
                if (static_cast<unsigned>(seriesLabels.size()) == numberGroups) {
                    // Single dimension based on a single data series.

                    unsigned seriesIndex = 0;
                    while (seriesIndex == currentLabelSeriesIndex) {
                        ++seriesIndex;
                    }

                    const QVector<double>& values       = valuesBySeries.at(0);
                    unsigned               numberPoints = static_cast<unsigned>(values.size());

                    for (unsigned index=0 ; index<numberPoints ; ++index) {
                        const QColor&  barColor    = defaultColor(index);
                        const QString& seriesLabel = seriesLabels.at(index);
                        double         value       = values.at(index);

                        QBarSet* barSet = new QBarSet(seriesLabel);
                        barSet->setBrush(QBrush(barColor));
                        barSet->setBorderColor(barColor);

                        barSet->append(value);

                        barSeries->append(barSet);
                        setMarkerColor(markerSize, chart->legend()->markers(barSeries), barColor);
                    }

                    barSeries->attachAxis(scaleAxis);
//                    baselineAxis->append(seriesLabels);
                    chart->addSeries(barSeries);
                } else {
                    errorString = tr(
                        "Number of labels must match either the number of values in each series or number of series "
                        "providing data."
                    );
                }
            } else {
                if (static_cast<unsigned>(seriesLabels.size()) == numberDataSeries) {
                    // Single dimension based on a independent data series.

                    barSeries->attachAxis(scaleAxis);

                    unsigned valueIndex = 0;
                    for (unsigned seriesIndex=0 ; seriesIndex<numberSeries ; ++seriesIndex) {
                        if (seriesIndex != currentLabelSeriesIndex) {
                            const SeriesData&      seriesData  = currentSeriesData.at(seriesIndex);
                            const QColor&          barColor    = seriesData.barColor();
                            const QString&         seriesLabel = seriesLabels.at(valueIndex);
                            const QVector<double>& values      = valuesBySeries.at(valueIndex);
                            QBarSet*               barSet      = new QBarSet(seriesLabel);

                            barSet->setBrush(QBrush(barColor));
                            barSet->setBorderColor(barColor);
                            barSet->append(values.at(0));
                            barSeries->append(barSet);

                            ++valueIndex;
                        }
                    }

                    chart->addSeries(barSeries);
//                    baselineAxis->append(seriesLabels);
                } else {
                    errorString = tr(
                        "Number of labels must match either the number of values in each series or number of series "
                        "providing data."
                    );
                }
            }
        } else {
            if (numberGroups == static_cast<unsigned>(programmaticLabels.size())) {
                // Multi-dimensional.

                unsigned valueIndex = 0;

                QAbstractBarSeries* barSeries;
                if (currentBaselineAxisLocation == AxisLocation::BOTTOM_X_A_GM) {
                    barSeries = new QBarSeries;
                } else {
                    barSeries = new QHorizontalBarSeries;
                }

                barSeries->attachAxis(scaleAxis);

                for (unsigned seriesIndex=0 ; seriesIndex<numberSeries ; ++seriesIndex) {
                    if (seriesIndex != currentLabelSeriesIndex) {
                        if (valueIndex < static_cast<unsigned>(valuesBySeries.size()) &&
                            valueIndex < static_cast<unsigned>(seriesLabels.size())      ) {
                            const SeriesData&      seriesData  = currentSeriesData.at(seriesIndex);
                            const QColor&          barColor    = seriesData.barColor();
                            const QVector<double>& values      = valuesBySeries.at(valueIndex);
                            const QString&         seriesLabel = seriesLabels.at(valueIndex);

                            QBarSet* barSet = new QBarSet(seriesLabel);

                            barSet->setBrush(QBrush(barColor));
                            barSet->setBorderColor(barColor);

                            for (unsigned groupIndex=0 ; groupIndex<numberGroups ; ++groupIndex) {
                                barSet->append(values.at(groupIndex));
                            }

                            barSeries->append(barSet);
                            setMarkerColor(markerSize, chart->legend()->markers(barSeries), barColor);

                            ++valueIndex;
                        }
                    }
                }

                chart->addSeries(barSeries);
                baselineAxis->append(programmaticLabels);
            } else {
                errorString = tr(
                    "Number of labels must match either the number of values in each series or number of series "
                    "providing data."
                );
            }
        }
    }

    if (!errorString.isEmpty()) {
        showErrorMessage(errorString);
    } else {
        clearErrorMessage();
    }
}


BarChartPresentationData::SeriesData* BarChartPresentationData::seriesData(unsigned valueIndex) {
    return valueIndex < static_cast<unsigned>(currentSeriesData.size()) ? &(currentSeriesData[valueIndex]) : Q_NULLPTR;
}


unsigned BarChartPresentationData::numberSeriesDataValues() const {
    return static_cast<unsigned>(currentSeriesData.size());
}


void BarChartPresentationData::setMarkerColor(
        unsigned                     markerSize,
        const QList<QLegendMarker*>& markerList,
        const QColor&                markerColor
    ) {
    QPixmap  markerPixmap(markerSize, markerSize);
    QPainter painter(&markerPixmap);

    painter.setPen(QColor(Qt::GlobalColor::black));
    painter.setBrush(QBrush(markerColor,Qt::BrushStyle::SolidPattern));
    painter.drawRect(0, 0, markerSize, markerSize);
    painter.end();

    QBrush markerBrush;
    markerBrush.setTexture(markerPixmap);

    for (  QList<QLegendMarker*>::const_iterator it = markerList.constBegin(), end = markerList.constEnd()
         ; it != end
         ; ++it
         ) {
        QLegendMarker* legendMarker = *it;
        legendMarker->setBrush(markerBrush);
    }
}
