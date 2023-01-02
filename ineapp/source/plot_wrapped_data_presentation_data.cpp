/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotWrappedDataPresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QGraphicsItem>
#include <QChart>
#include <QAbstractAxis>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QFontMetricsF>

#include <cmath>

#include <eqt_charts.h>
#include <eqt_graphics_item.h>
#include <eqt_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_calculated_value.h>

#include "application.h"
#include "presentation.h"
#include "plot_wrapped_presentation_data.h"
#include "plot_wrapped_data_presentation_data.h"

/***********************************************************************************************************************
 * PlotWrappedDataPresentationData::SeriesData
 */

PlotWrappedDataPresentationData::SeriesData::SeriesData() {}


PlotWrappedDataPresentationData::SeriesData::~SeriesData() {}


double PlotWrappedDataPresentationData::SeriesData::rescaleValue(double value, AxisScale axisScale, bool& ok) {
    double result = 0;

    if (ok) {
        switch (axisScale) {
            case AxisScale::LINEAR: {
                result = value;
                break;
            }

            case AxisScale::LOG: {
                if (value > 0) {
                    result = value;
                } else if (value == 0) {
                    result = std::numeric_limits<double>::min();
                } else {
                    result = std::numeric_limits<double>::min();
                    ok     = false;
                }

                break;
            }

            case AxisScale::DB_10_LOG: {
                if (value > 0) {
                    result = 10.0 * std::log10(value);
                } else if (value == 0) {
                    result = std::numeric_limits<double>::lowest();
                } else {
                    result = std::numeric_limits<double>::lowest();
                    ok     = false;
                }

                break;
            }

            case AxisScale::DB_20_LOG: {
                if (value > 0) {
                    result = 20.0 * std::log10(value);
                } else if (value == 0) {
                    result = std::numeric_limits<double>::lowest();
                } else {
                    result = std::numeric_limits<double>::lowest();
                    ok     = false;
                }

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }
    } else {
        if (axisScale == AxisScale::LINEAR) {
            result = 0;
        } else {
            result = std::numeric_limits<double>::min();
        }
    }

    return result;
}

/***********************************************************************************************************************
 * PlotWrappedDataPresentationData
 */

PlotWrappedDataPresentationData::PlotWrappedDataPresentationData(
        EQt::GraphicsItem* chartItem
    ):PlotWrappedPresentationData(
        chartItem
    ) {
    updateTimer.setSingleShot(true);
    connect(&updateTimer, &QTimer::timeout, this, &PlotWrappedDataPresentationData::performDeferredUpdates);
}


PlotWrappedDataPresentationData::~PlotWrappedDataPresentationData() {}


void PlotWrappedDataPresentationData::calculatedValueUpdated(
        unsigned                   valueIndex,
        const Ld::CalculatedValue& calculatedValue
    ) {
    unsigned currentNumberValues = static_cast<unsigned>(seriesAndSourceByValueIndex.size());
    Q_ASSERT(valueIndex < currentNumberValues);

    const SeriesAndSource& seriesAndSource = seriesAndSourceByValueIndex.at(valueIndex);

    SeriesData* data = seriesData(seriesAndSource.seriesIndex());
    Q_ASSERT(data != Q_NULLPTR);
    data->setCalculatedValue(seriesAndSource.sourceIndex(), calculatedValue);

    if (!updateTimer.isActive()) {
        updateTimer.start(0);
    }
}


void PlotWrappedDataPresentationData::calculatedValuesCleared() {
    unsigned numberDataSeries = numberSeriesDataValues();
    for (unsigned seriesIndex=0 ; seriesIndex<numberDataSeries ; ++seriesIndex) {
        SeriesData* data = seriesData(seriesIndex);
        Q_ASSERT(data != Q_NULLPTR);
        data->calculatedValuesCleared();
    }

    if (!updateTimer.isActive()) {
        updateTimer.start(0);
    }
}


PlotWrappedDataPresentationData::SeriesMinMaxAndErrorStatus PlotWrappedDataPresentationData::configureAxis(
        const PlotWrappedDataPresentationData::AxisData&     axisData,
        const PlotWrappedDataPresentationData::SeriesMinMax& measuredRange
    ) {
    QRectF plotAreaRectangle = chartItem()->plotArea();

    AxisLocation               axisLocation  = axisData.axisLocation();
    const Ld::ChartAxisFormat& axisFormat    = axisData.axisFormat();
    QAbstractAxis*             axis          = axisData.axis();
    double                     minimum       = axisFormat.minimumValue();
    double                     maximum       = axisFormat.maximumValue();
    double                     majorStepSize = axisFormat.majorStepSize();
    double                     minorStepSize = axisFormat.minorStepSize();

    SeriesMinMax adjustedRange;
    QString      errorReason;

    if (minimum == Ld::ChartAxisFormat::automaticScaling || maximum == Ld::ChartAxisFormat::automaticScaling) {
        if (measuredRange != SeriesMinMax()) {
            auto select = [&](double fromFormat, double measured)->double {
                double result;

                if (fromFormat == Ld::ChartAxisFormat::automaticScaling) {
                    result = measured;
                } else {
                    result = fromFormat;
                }

                return result;
            };

            SeriesMinMax recommendedRange = calculateNiceRange(
                axisLocation,
                select(minimum, measuredRange.minimum()),
                select(maximum, measuredRange.maximum()),
                axisFormat.axisScale()
            );

            adjustedRange = SeriesMinMax(
                select(minimum, recommendedRange.minimum()),
                select(maximum, recommendedRange.maximum())
            );

            if (errorReason.isEmpty()                              &&
                adjustedRange.minimum() >= adjustedRange.maximum() &&
                adjustedRange != SeriesMinMax()                       ) {
                errorReason = tr("Inverted range with automatic scaling -- Can not render plot.");
            }
        }
    } else {
        adjustedRange = SeriesMinMax(minimum, maximum);
    }

    if (errorReason.isEmpty() && adjustedRange != SeriesMinMax()) {
        QFont  numberFont         = axis->labelsFont();
        double displayMinimum     = 0;
        double displayMaximum     = 0;
        float  maximumNumberTicks = 0;

        if (axisFormat.axisScale() != Ld::ChartAxisFormat::AxisScale::LOG) {
            QValueAxis* valueAxis = dynamic_cast<QValueAxis*>(axis);

            if (std::isinf(adjustedRange.minimum()) || std::isinf(adjustedRange.maximum())) {
                if (axisFormat.axisScale() == Ld::ChartAxisFormat::AxisScale::LINEAR) {
                    errorReason = tr("Linear scale does not support infinite axis values.");
                } else {
                    errorReason = tr("Decibel scale does not support infinite axis values.");
                }
            } else if (adjustedRange.minimum() > adjustedRange.maximum()) {
                valueAxis->setReverse(true);
                valueAxis->setRange(adjustedRange.maximum(), adjustedRange.minimum());
            } else if (adjustedRange.minimum() == adjustedRange.maximum()) {
                valueAxis->setReverse(false);
                valueAxis->setRange(adjustedRange.minimum() - 0.5, adjustedRange.maximum() + 0.5);
            } else {
                valueAxis->setReverse(false);
                valueAxis->setRange(adjustedRange.minimum(), adjustedRange.maximum());
            }

            displayMinimum = adjustedRange.minimum();
            displayMaximum = adjustedRange.maximum();
        } else {
            if (adjustedRange.minimum() <= std::numeric_limits<double>::min() ||
                adjustedRange.maximum() <= std::numeric_limits<double>::min()    ) {
                errorReason = tr("Log scale does not support negative or zero values.");
            } else if (adjustedRange.minimum() >= adjustedRange.maximum()) {
                errorReason = tr("Log scale does not support reversed ranges.");
            } else if (std::isinf(adjustedRange.minimum()) || std::isinf(adjustedRange.maximum())) {
                errorReason = tr("Log scale does not support reversed ranges.");
            } else {
                QLogValueAxis* logAxis = dynamic_cast<QLogValueAxis*>(axis);
                logAxis->setRange(adjustedRange.minimum(), adjustedRange.maximum());

                displayMinimum = std::log10(adjustedRange.minimum());
                displayMaximum = std::log10(adjustedRange.maximum());
            }
        }

        if (errorReason.isEmpty()) {
            if (axisFormat.axisScale() != AxisScale::LOG) {
                QFontMetricsF numberFontMetrics(numberFont);

                double span = std::abs(displayMaximum - displayMinimum);
                if (axisLocation == AxisLocation::BOTTOM_X_A_GM || axisLocation == AxisLocation::TOP_X_A_GM) {
                    QString minimumLabel = QString::number(displayMinimum);
                    QString maximumLabel = QString::number(displayMaximum);

                    float labelWidth = std::max(
                        numberFontMetrics.horizontalAdvance(minimumLabel + "MM"),
                        numberFontMetrics.horizontalAdvance(maximumLabel + "MM")
                    );

                    maximumNumberTicks = plotAreaRectangle.width() / labelWidth;

                    bool  rotateLabels;

                    if (majorStepSize != Ld::ChartAxisFormat::automaticScaling) {
                        float requiredNumberTicks = span / std::abs(majorStepSize);
                        rotateLabels = (requiredNumberTicks > maximumNumberTicks);
                    } else {
                        rotateLabels = (maximumNumberTicks < minimumNumberTicks);
                    }

                    if (rotateLabels) {
                        axis->setLabelsAngle(-90);
                        maximumNumberTicks = plotAreaRectangle.width() / (2.0 * numberFontMetrics.height());
                    } else {
                        axis->setLabelsAngle(0);
                    }
                } else {
                    // Not much we can do here to deal with the step size.
                    maximumNumberTicks  = plotAreaRectangle.height() / (2.0 * numberFontMetrics.height());
                }

                QValueAxis* valueAxis = dynamic_cast<QValueAxis*>(axis);

                double adjustedMajorStepSize;
                double adjustedMinorStepSize;
                if (majorStepSize == Ld::ChartAxisFormat::automaticScaling) {
                    SeriesStepSizes recommendedStepSize = calculateNiceStepSizes(
                        axisLocation,
                        span,
                        maximumNumberTicks
                    );

                    adjustedMajorStepSize = recommendedStepSize.majorStepSize();

                    if (minorStepSize == Ld::ChartAxisFormat::automaticScaling) {
                        adjustedMinorStepSize = recommendedStepSize.minorStepSize();
                    } else {
                        adjustedMinorStepSize = std::abs(minorStepSize);
                    }
                } else {
                    adjustedMajorStepSize = std::abs(majorStepSize);

                    if (minorStepSize == Ld::ChartAxisFormat::automaticScaling) {
                        unsigned numberMajorTicks = static_cast<unsigned>(span / adjustedMajorStepSize);
                        adjustedMinorStepSize = calculateMinorStepSize(
                            axisLocation,
                            adjustedMajorStepSize,
                            numberMajorTicks,
                            maximumNumberTicks
                        );
                    } else {
                        adjustedMinorStepSize = std::abs(minorStepSize);
                    }
                }

                if (isModuloAxis(axisLocation)) {
                    unsigned numberMajorTicks = static_cast<unsigned>(
                          1.5
                        + std::abs(displayMaximum - displayMinimum) / adjustedMajorStepSize
                    );
                    valueAxis->setTickCount(numberMajorTicks);
                } else {
                    valueAxis->setTickAnchor(std::min(displayMaximum, displayMaximum));

                    double numberTicks = std::abs(displayMaximum - displayMinimum) / adjustedMajorStepSize;
                    double real;
                    double fractional  = std::modf(numberTicks, &real);

                    if (fractional / real < 1.0E-4) {
                        valueAxis->setTickCount(static_cast<unsigned>(numberTicks) + 1);
                    } else {
                        valueAxis->setTickInterval(adjustedMajorStepSize);
                    }
                }

                int minorTicksBetweenMajorTicks = std::max(
                    static_cast<int>(adjustedMajorStepSize / adjustedMinorStepSize - 0.5),
                    0
                );

                valueAxis->setMinorTickCount(minorTicksBetweenMajorTicks);
            }
        }
    }

    return SeriesMinMaxAndErrorStatus(adjustedRange, errorReason);
}


bool PlotWrappedDataPresentationData::isModuloAxis(AxisLocation /* axisLocation */) const {
    return false;
}


void PlotWrappedDataPresentationData::mapValueIndex(unsigned seriesIndex, unsigned sourceIndex, unsigned valueIndex) {
    unsigned currentNumberValues = static_cast<unsigned>(seriesAndSourceByValueIndex.size());
    if (valueIndex < currentNumberValues) {
        seriesAndSourceByValueIndex[valueIndex] = SeriesAndSource(seriesIndex, sourceIndex);
    } else {
        while(valueIndex > currentNumberValues) {
            seriesAndSourceByValueIndex.append(SeriesAndSource(static_cast<unsigned>(-1), static_cast<unsigned>(-1)));
            ++currentNumberValues;
        }

        seriesAndSourceByValueIndex.append(SeriesAndSource(seriesIndex, sourceIndex));
    }
}
