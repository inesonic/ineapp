/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PieChartPresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QPieSlice>
#include <QPieSeries>
#include <QLegend>
#include <QLegendMarker>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QBrush>

#include <cmath>

#include <util_color_functions.h>

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
#include "pie_chart_presentation_data.h"

/***********************************************************************************************************************
 * PieChartPresentationData::SeriesData
 */

PieChartPresentationData::SeriesData::SeriesData() {}


PieChartPresentationData::SeriesData::SeriesData(
        const Ld::VariableName& dataSourceName,
        const QString&          seriesLabel,
        const QColor&           sliceColor
    ):Plot2DCategorizedPresentationData::SeriesData(
        AxisScale::LINEAR,
        dataSourceName,
        seriesLabel
    ),currentSliceColor(
        sliceColor
    ) {}


PieChartPresentationData::SeriesData::SeriesData(
        const SeriesData& other
    ):Plot2DCategorizedPresentationData::SeriesData(
        other
    ),currentSliceColor(
        other.currentSliceColor
    ) {}


PieChartPresentationData::SeriesData::~SeriesData() {}


PieChartPresentationData::SeriesData& PieChartPresentationData::SeriesData::operator=(
        const PieChartPresentationData::SeriesData& other
    ) {
    Plot2DCategorizedPresentationData::SeriesData::operator=(other);
    currentSliceColor = other.currentSliceColor;

    return *this;
}

/***********************************************************************************************************************
 * PieChartPresentationData
 */

PieChartPresentationData::PieChartPresentationData(
        EQt::ChartItem* chartItem
    ):Plot2DCategorizedPresentationData(
        chartItem
    ) {
    currentLabelSeriesIndex = static_cast<unsigned>(-1);
}


PieChartPresentationData::~PieChartPresentationData() {}


void PieChartPresentationData::setLabelSeriesIndex(unsigned int seriesIndex) {
    currentLabelSeriesIndex = seriesIndex;
}


void PieChartPresentationData::clearLabelSeriesIndex() {
    currentLabelSeriesIndex = static_cast<unsigned>(-1);
}


void PieChartPresentationData::setAxisFormat(
        PieChartPresentationData::AxisLocation axisLocation,
        const Ld::ChartAxisFormat&             axisFormat,
        QAbstractAxis*                         axis
    ) {
    axisDataByLocation.insert(axisLocation, AxisData(axisLocation, axisFormat, axis));
}


void PieChartPresentationData::clearSeries() {
    currentSeriesData.clear();
    currentLabelSeriesIndex = static_cast<unsigned>(-1);
}


void PieChartPresentationData::setLabelsSeries(
        unsigned                seriesIndex,
        unsigned                valueIndex,
        const Ld::VariableName& dataSourceName
    ) {
    if (seriesIndex < static_cast<unsigned>(currentSeriesData.size())) {
        currentSeriesData[seriesIndex] = SeriesData(dataSourceName, QString(), QColor());
    } else {
        while (seriesIndex > static_cast<unsigned>(currentSeriesData.size())) {
            currentSeriesData.append(SeriesData());
        }

        currentSeriesData.append(SeriesData(dataSourceName, QString(), QColor()));
    }

    mapValueIndex(seriesIndex, 0, valueIndex);
}


QString PieChartPresentationData::setSeries(
        unsigned                seriesIndex,
        unsigned                valueIndex,
        const Ld::VariableName& dataSourceName,
        const QString&          seriesLabel,
        const QColor&           sliceColor
    ) {
    QString result;

    if (seriesIndex < static_cast<unsigned>(currentSeriesData.size())) {
        currentSeriesData[seriesIndex] = SeriesData(dataSourceName, seriesLabel, sliceColor);
    } else {
        while (seriesIndex > static_cast<unsigned>(currentSeriesData.size())) {
            currentSeriesData.append(SeriesData());
        }

        currentSeriesData.append(SeriesData(dataSourceName, seriesLabel, sliceColor));
    }

    mapValueIndex(seriesIndex, 0, valueIndex);

    return result;
}


void PieChartPresentationData::performDeferredUpdates() {
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
            errorString = tr("Pie charts can-not have negative values.");
        }
    }

    if (errorString.isEmpty()) {
        chart->removeAllSeries();

        QFontMetrics fontMetrics(chart->legend()->font());
        unsigned     markerSize = fontMetrics.ascent();

        if (numberGroups == 1 || numberDataSeries == 1) {
            // Single pie chart

            if (numberProgrammaticLabels > 0) {
                seriesLabels = programmaticLabels;
            }

            QPieSeries* pieSeries = new QPieSeries;
            if (numberGroups > 1) {
                if (static_cast<unsigned>(seriesLabels.size()) == numberGroups) {
                    unsigned seriesIndex = 0;
                    while (seriesIndex == currentLabelSeriesIndex) {
                        ++seriesIndex;
                    }

                    const QVector<double>& values       = valuesBySeries.at(0);
                    unsigned               numberPoints = static_cast<unsigned>(values.size());

                    for (unsigned index=0 ; index<numberPoints ; ++index) {
                        const QColor&  sliceColor  = defaultColor(index);
                        const QString& seriesLabel = seriesLabels.at(index);
                        double         value       = values.at(index);

                        QPieSlice* pieSlice = new QPieSlice(
                            addValueToLabel(seriesLabel, value),
                            value
                        );

                        pieSlice->setColor(sliceColor);
                        pieSeries->append(pieSlice);

                        setMarkerColor(markerSize, chart->legend()->markers(pieSeries), sliceColor);
                    }

                    pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelOutside);
                    pieSeries->setLabelsVisible(!chart->legend()->isVisible());

                    pieSeries->setPieSize(pieRelativeSize(0, 1));
                    pieSeries->setHoleSize(holeRelativeSize(0, 1));

                    chart->addSeries(pieSeries);
                } else {
                    errorString = tr(
                        "Number of labels must match either the number of values in each series or number of series "
                        "providing data."
                    );
                }
            } else /* if (numberDataSeries >= 1) */ {
                if (static_cast<unsigned>(seriesLabels.size()) == numberDataSeries) {
                    unsigned valueIndex = 0;
                    for (unsigned seriesIndex=0 ; seriesIndex<numberSeries ; ++seriesIndex) {
                        if (seriesIndex != currentLabelSeriesIndex) {
                            const SeriesData&      seriesData  = currentSeriesData.at(seriesIndex);
                            const QColor&          sliceColor  = seriesData.sliceColor();
                            const QString&         seriesLabel = seriesLabels.at(valueIndex);
                            const QVector<double>& values      = valuesBySeries.at(valueIndex);
                            double                 value       = values.at(0);

                            QPieSlice* pieSlice = new QPieSlice(
                                addValueToLabel(seriesLabel, value),
                                value
                            );

                            pieSlice->setColor(sliceColor);
                            pieSeries->append(pieSlice);

                            setMarkerColor(markerSize, chart->legend()->markers(pieSeries), sliceColor);

                            ++valueIndex;
                        }
                    }

                    pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelOutside);
                    pieSeries->setLabelsVisible(!chart->legend()->isVisible());

                    pieSeries->setPieSize(pieRelativeSize(0, 1));
                    pieSeries->setHoleSize(holeRelativeSize(0, 1));

                    chart->addSeries(pieSeries);
                } else {
                    errorString = tr(
                        "Number of labels must match either the number of values in each series or number of series "
                        "providing data."
                    );
                }
            }
        } else {
            if (currentLabelSeriesIndex == static_cast<unsigned>(-1)) {
                for (unsigned groupIndex=0 ; groupIndex<numberGroups ; ++groupIndex) {
                    QPieSeries* pieSeries = new QPieSeries;
                    for (unsigned seriesIndex=0 ; seriesIndex<numberSeries ; ++seriesIndex) {
                        const SeriesData& seriesData  = currentSeriesData.at(seriesIndex);
                        const QColor&     sliceColor  = seriesData.sliceColor();
                        const QString&    seriesLabel = seriesData.seriesLabel();
                        double            value       = valuesBySeries.at(seriesIndex).at(groupIndex);

                        QPieSlice* pieSlice = new QPieSlice(
                            addValueToLabel(seriesLabel, value),
                            value
                        );

                        if (groupIndex != 0) {
                            pieSlice->setLabelColor(sliceTextColor(sliceColor));
                        }

                        pieSlice->setColor(sliceColor);
                        pieSeries->append(pieSlice);
                    }


                    if (groupIndex == 0) {
                        pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelOutside);
                    } else {
                        pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelInsideHorizontal);
                    }

                    pieSeries->setLabelsVisible(!chart->legend()->isVisible());

                    pieSeries->setPieSize(pieRelativeSize(groupIndex, numberGroups));
                    pieSeries->setHoleSize(holeRelativeSize(groupIndex, numberGroups));

                    chart->addSeries(pieSeries);
                }
            } else {
                unsigned valueIndex = 0;
                for (unsigned seriesIndex=0 ; seriesIndex<numberSeries ; ++seriesIndex) {
                    if (seriesIndex != currentLabelSeriesIndex) {
                        const SeriesData&      seriesData = currentSeriesData.at(seriesIndex);
                        const QColor&          sliceColor = seriesData.sliceColor();
                        const QVector<double>& values     = valuesBySeries.at(valueIndex);
                        QPieSeries*            pieSeries  = new QPieSeries;

                        for (unsigned groupIndex=0 ; groupIndex<numberGroups ; ++groupIndex) {
                            const QString& label = programmaticLabels.at(groupIndex);
                            double         value = values.at(groupIndex);

                            QPieSlice* pieSlice = new QPieSlice(
                                addValueToLabel(label, value),
                                value
                            );

                            if (valueIndex != 0) {
                                pieSlice->setLabelColor(sliceTextColor(sliceColor));
                            }

                            pieSlice->setColor(sliceColor);
                            pieSeries->append(pieSlice);
                        }

                        if (valueIndex == 0) {
                            pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelOutside);
                        } else {
                            pieSeries->setLabelsPosition(QPieSlice::LabelPosition::LabelInsideHorizontal);
                        }

                        pieSeries->setLabelsVisible(!chart->legend()->isVisible());

                        pieSeries->setPieSize(pieRelativeSize(valueIndex, numberDataSeries));
                        pieSeries->setHoleSize(holeRelativeSize(valueIndex, numberDataSeries));

                        chart->addSeries(pieSeries);

                        ++valueIndex;
                    }
                }
            }
        }
    }

    if (!errorString.isEmpty()) {
        showErrorMessage(errorString);
    } else {
        clearErrorMessage();
    }
}


PieChartPresentationData::SeriesData* PieChartPresentationData::seriesData(unsigned valueIndex) {
    return valueIndex < static_cast<unsigned>(currentSeriesData.size()) ? &(currentSeriesData[valueIndex]) : Q_NULLPTR;
}


unsigned PieChartPresentationData::numberSeriesDataValues() const {
    return static_cast<unsigned>(currentSeriesData.size());
}


void PieChartPresentationData::setMarkerColor(
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


double PieChartPresentationData::pieRelativeSize(unsigned seriesIndex, unsigned numberSeries) const {
    double result;

    if (numberSeries < 2) {
        result = 0.8;
    } else {
        result = 0.40 * (1 + (numberSeries - seriesIndex - 1.0) / (numberSeries - 1.0));
    }

    return result;
}


double PieChartPresentationData::holeRelativeSize(unsigned seriesIndex, unsigned numberSeries) const {
    double result;

    if (numberSeries < 2 || seriesIndex == (numberSeries - 1)) {
        result = 0;
    } else {
        result = 0.40 * (1 + (numberSeries - seriesIndex - 2.0) / (numberSeries - 1.0));
    }

    return result;
}


QColor PieChartPresentationData::sliceTextColor(const QColor& color) {
    QColor result;

    if (Util::luminance(color) > 0.5) {
        result = QColor(Qt::GlobalColor::black);
    } else {
        result = QColor(Qt::GlobalColor::white);
    }

    return result;
}
