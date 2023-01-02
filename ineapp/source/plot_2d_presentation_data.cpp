/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Plot2DPresentationData class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QChart>
#include <QFont>
#include <QFontMetricsF>
#include <QXYSeries>
#include <QValueAxis>
#include <QLogValueAxis>

#include <algorithm>
#include <cmath>

#include <eqt_charts.h>
#include <eqt_graphics_item.h>
#include <eqt_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <model_api_types.h>
#include <model_tuple.h>
#include <model_matrix_boolean.h>
#include <model_matrix_integer.h>
#include <model_matrix_real.h>
#include <model_matrix_complex.h>
#include <model_variant.h>
#include <m_basic_functions.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include <application.h>
#include <presentation.h>
#include "plot_presentation_data.h"
#include "plot_2d_presentation_data.h"

/***********************************************************************************************************************
 * Plot2DPresentationData::SeriesData
 */

Plot2DPresentationData::SeriesData::SeriesData(
        Plot2DPresentationData::SplineType splineType,
        QXYSeries*                         series1,
        QXYSeries*                         series2
    ):currentSplineType(
        splineType
    ),currentSeries1(
        series1
    ),currentSeries2(
        series2
    ) {
    currentAxisLocations[0]   = AxisLocation::BOTTOM_X_A_GM;
    currentAxisLocations[1]   = AxisLocation::LEFT_Y_R_RC;
    currentNumberSeriesValues = static_cast<unsigned>(-1);
}


Plot2DPresentationData::SeriesData::SeriesData(
        const SeriesData& other
    ):currentSplineType(
        other.currentSplineType
    ),currentSeries1(
        other.currentSeries1
    ),currentSeries2(
        other.currentSeries2
    ),currentNumberSeriesValues(
        other.currentNumberSeriesValues
    ) {
    for (unsigned sourceIndex=0 ; sourceIndex<numberSourcesPerSeries ; ++sourceIndex) {
        currentAxisLocations[sourceIndex]   = other.currentAxisLocations[sourceIndex];
        currentDataSourceNames[sourceIndex] = other.currentDataSourceNames[sourceIndex];
        currentValues[sourceIndex]          = other.currentValues[sourceIndex];
    }
}


Plot2DPresentationData::SeriesData::~SeriesData() {}


void Plot2DPresentationData::SeriesData::setDataSource(
        unsigned                             sourceIndex,
        Plot2DPresentationData::AxisLocation axisLocation,
        Plot2DPresentationData::AxisScale    axisScale,
        const Ld::VariableName&              dataSourceName
    ) {
    Q_ASSERT(sourceIndex < numberSourcesPerSeries);

    currentAxisLocations[sourceIndex]   = axisLocation;
    currentDataSourceNames[sourceIndex] = dataSourceName;
    currentAxisScaling[sourceIndex]     = axisScale;
    currentNumberSeriesValues           = static_cast<unsigned>(-1);
}


void Plot2DPresentationData::SeriesData::setCalculatedValue(
        unsigned                   sourceIndex,
        const Ld::CalculatedValue& calculatedValue
    ) {
    Q_ASSERT(sourceIndex < numberSourcesPerSeries);
    Q_ASSERT(calculatedValue.name() == currentDataSourceNames[sourceIndex]);

    currentValues[sourceIndex] = calculatedValue.variant();
    currentNumberSeriesValues  = static_cast<unsigned>(-1);
}


void Plot2DPresentationData::SeriesData::calculatedValuesCleared() {
    currentValues[0] = Model::Variant();
    currentValues[1] = Model::Variant();
}


Plot2DPresentationData::SeriesUpdateResults Plot2DPresentationData::SeriesData::updateSeries(unsigned seriesIndex) {
    currentSeries1->clear();

    if (currentSeries2 != Q_NULLPTR) {
        currentSeries2->clear();
    }

    const Model::Variant& variant1 = currentValues[0];
    const Model::Variant& variant2 = currentValues[1];

    SeriesMinMax source1MinMax;
    SeriesMinMax source2MinMax;
    QString      errorReason;

    QVector<QPointF> values;
    unsigned long    source1Length = sourceLength(variant1, seriesIndex, errorReason);
    unsigned long    source2Length = errorReason.isEmpty() ? sourceLength(variant2, seriesIndex, errorReason) : 0;

    AxisScale        source1AxisScale = currentAxisScaling[0];
    AxisScale        source2AxisScale = currentAxisScaling[1];

    if (errorReason.isEmpty()) {
        Model::Integer seriesLength = static_cast<Model::Integer>(std::min(source1Length, source2Length));

        values.reserve(seriesLength);

        switch (variant1.valueType()) {
            case Model::ValueType::NONE: {
                break;
            }

            case Model::ValueType::BOOLEAN:
            case Model::ValueType::INTEGER:
            case Model::ValueType::REAL:
            case Model::ValueType::COMPLEX:
            case Model::ValueType::SET: {
                Q_ASSERT(false);
                break;
            }

            case Model::ValueType::TUPLE: {
                const Model::Tuple& tuple = variant1.toTuple();
                bool                ok    = true;
                Model::Integer      index = 1;

                do {
                    const Model::Variant& v = tuple.at(index);
                    double                value;

                    if (v.valueType() == Model::ValueType::COMPLEX) {
                        value = M::abs(v.toComplex());
                    } else {
                        value = v.toReal(&ok);
                    }

                    value = rescaleValue(value, source1AxisScale, ok);

                    values.append(QPointF(value, 0));
                    source1MinMax.update(value);

                    ++index;
                } while (ok && index <= seriesLength);

                if (!ok) {
                    errorReason = tr("Series %1, invalid value in first data source tuple at index %2.")
                                  .arg(seriesIndex+1)
                                  .arg(index - 1);
                }

                break;
            }

            case Model::ValueType::MATRIX_BOOLEAN: {
                const Model::MatrixBoolean& matrix = variant1.toMatrixBoolean();

                bool ok = true;
                for (Model::Integer index=1 ; index<=seriesLength && ok ; ++index) {
                    unsigned value = matrix.at(index) ? 1 : 0;

                    value = rescaleValue(value, source1AxisScale, ok);

                    values.append(QPointF(value, 0));
                    source1MinMax.update(value);
                }

                if (!ok) {
                    errorReason = tr("Series %1, invalid value in first data source.").arg(seriesIndex+1);
                }

                break;
            }

            case Model::ValueType::MATRIX_INTEGER: {
                const Model::MatrixInteger& matrix        = variant1.toMatrixInteger();
                Model::Integer              numberRows    = matrix.numberRows();
                Model::Integer              numberColumns = matrix.numberColumns();

                bool ok = true;
                if (numberRows == 1 || numberColumns == 1) {
                    const Model::Integer* rawData = matrix.data();

                    for (Model::Integer i=0 ; i<seriesLength && ok ; ++i) {
                        double value = static_cast<double>(rawData[i]);

                        value = rescaleValue(value, source1AxisScale, ok);

                        values.append(QPointF(value, 0));
                        source1MinMax.update(value);
                    }
                } else {
                    for (Model::Integer index=1 ; index<=seriesLength && ok ; ++index) {
                        double value = static_cast<double>(matrix.at(index));

                        value = rescaleValue(value, source1AxisScale, ok);

                        values.append(QPointF(value, 0));
                        source1MinMax.update(value);
                    }
                }

                if (!ok) {
                    errorReason = tr("Series %1, invalid value in first data source.").arg(seriesIndex+1);
                }

                break;
            }

            case Model::ValueType::MATRIX_REAL: {
                const Model::MatrixReal& matrix        = variant1.toMatrixReal();
                Model::Integer           numberRows    = matrix.numberRows();
                Model::Integer           numberColumns = matrix.numberColumns();

                bool ok = true;
                if (numberRows == 1 || numberColumns == 1) {
                    const Model::Real* rawData = matrix.data();

                    for (Model::Integer i=0 ; i<seriesLength && ok ; ++i) {
                        double value = rawData[i];

                        value = rescaleValue(value, source1AxisScale, ok);

                        values.append(QPointF(value, 0));
                        source1MinMax.update(value);
                    }
                } else {
                    for (Model::Integer index=1 ; index<=seriesLength && ok ; ++index) {
                        double value = matrix.at(index);

                        value = rescaleValue(value, source1AxisScale, ok);

                        values.append(QPointF(value, 0));
                        source1MinMax.update(value);
                    }
                }

                if (!ok) {
                    errorReason = tr("Series %1, invalid value in first data source.").arg(seriesIndex+1);
                }

                break;
            }

            case Model::ValueType::MATRIX_COMPLEX: {
                const Model::MatrixComplex& matrix        = variant1.toMatrixComplex();
                Model::Integer              numberRows    = matrix.numberRows();
                Model::Integer              numberColumns = matrix.numberColumns();

                bool ok = true;
                if (numberRows == 1 || numberColumns == 1) {
                    const Model::Complex* rawData = matrix.data();

                    for (Model::Integer i=0 ; i<seriesLength ; ++i) {
                        double value = M::abs(rawData[i]);

                        value = rescaleValue(value, source1AxisScale, ok);

                        values.append(QPointF(value, 0));
                        source1MinMax.update(value);
                    }
                } else {
                    for (Model::Integer index=1 ; index<=seriesLength ; ++index) {
                        double value = M::abs(matrix.at(index));

                        value = rescaleValue(value, source1AxisScale, ok);

                        values.append(QPointF(value, 0));
                        source1MinMax.update(value);
                    }
                }

                if (!ok) {
                    errorReason = tr("Series %1, invalid value in first data source.").arg(seriesIndex+1);
                }

                break;
            }

            case Model::ValueType::NUMBER_TYPES: {
                Q_ASSERT(false);
                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        if (errorReason.isEmpty()) {
            switch (variant2.valueType()) {
                case Model::ValueType::NONE: {
                    break;
                }

                case Model::ValueType::BOOLEAN:
                case Model::ValueType::INTEGER:
                case Model::ValueType::REAL:
                case Model::ValueType::COMPLEX:
                case Model::ValueType::SET: {
                    Q_ASSERT(false);
                    break;
                }

                case Model::ValueType::TUPLE: {
                    const Model::Tuple& tuple = variant2.toTuple();
                    bool                ok    = true;
                    Model::Integer      index = 0;

                    do {
                        const Model::Variant& v = tuple.at(index + 1);
                        double                value;

                        if (v.valueType() == Model::ValueType::COMPLEX) {
                            value = M::abs(v.toComplex());
                        } else {
                            value = v.toReal(&ok);
                        }

                        value = rescaleValue(value, source2AxisScale, ok);

                        values[index].setY(value);
                        source2MinMax.update(value);

                        ++index;
                    } while (ok && index < seriesLength);

                    if (!ok) {
                        errorReason = tr("Series %1, invalid value in second data source tuple at index %2.")
                                      .arg(seriesIndex+1)
                                      .arg(index);
                    }

                    break;
                }

                case Model::ValueType::MATRIX_BOOLEAN: {
                    const Model::MatrixBoolean& matrix = variant2.toMatrixBoolean();

                    bool ok = true;
                    for (Model::Integer index=0 ; index<seriesLength && ok ; ++index) {
                        unsigned value = matrix.at(index + 1) ? 1 : 0;

                        value = rescaleValue(value, source2AxisScale, ok);

                        values[index].setY(value);
                        source2MinMax.update(value);
                    }

                    if (!ok) {
                        errorReason = tr("Series %1, invalid value in second data source.").arg(seriesIndex+1);
                    }

                    break;
                }

                case Model::ValueType::MATRIX_INTEGER: {
                    const Model::MatrixInteger& matrix        = variant2.toMatrixInteger();
                    Model::Integer              numberRows    = matrix.numberRows();
                    Model::Integer              numberColumns = matrix.numberColumns();

                    bool ok = true;
                    if (numberRows == 1 || numberColumns == 1) {
                        const Model::Integer* rawData = matrix.data();

                        for (Model::Integer i=0 ; i<seriesLength && ok ; ++i) {
                            double value = static_cast<double>(rawData[i]);

                            value = rescaleValue(value, source2AxisScale, ok);

                            values[i].setY(value);
                            source2MinMax.update(value);
                        }
                    } else {
                        for (Model::Integer index=0 ; index<seriesLength && ok ; ++index) {
                            double value = static_cast<double>(matrix.at(index + 1));

                            value = rescaleValue(value, source2AxisScale, ok);

                            values[index].setY(value);
                            source2MinMax.update(value);
                        }
                    }

                    if (!ok) {
                        errorReason = tr("Series %1, invalid value in second data source.").arg(seriesIndex+1);
                    }

                    break;
                }

                case Model::ValueType::MATRIX_REAL: {
                    const Model::MatrixReal& matrix        = variant2.toMatrixReal();
                    Model::Integer           numberRows    = matrix.numberRows();
                    Model::Integer           numberColumns = matrix.numberColumns();

                    bool ok = true;
                    if (numberRows == 1 || numberColumns == 1) {
                        const Model::Real* rawData = matrix.data();

                        for (Model::Integer i=0 ; i<seriesLength && ok ; ++i) {
                            double value = rawData[i];

                            value = rescaleValue(value, source2AxisScale, ok);

                            values[i].setY(value);
                            source2MinMax.update(value);
                        }
                    } else {
                        for (Model::Integer index=0 ; index<seriesLength && ok ; ++index) {
                            double value = matrix.at(index + 1);

                            value = rescaleValue(value, source2AxisScale, ok);

                            values[index].setY(value);
                            source2MinMax.update(value);
                        }
                    }

                    if (!ok) {
                        errorReason = tr("Series %1, invalid value in second data source.").arg(seriesIndex+1);
                    }

                    break;
                }

                case Model::ValueType::MATRIX_COMPLEX: {
                    const Model::MatrixComplex& matrix        = variant2.toMatrixComplex();
                    Model::Integer              numberRows    = matrix.numberRows();
                    Model::Integer              numberColumns = matrix.numberColumns();

                    bool ok = true;
                    if (numberRows == 1 || numberColumns == 1) {
                        const Model::Complex* rawData = matrix.data();

                        for (Model::Integer i=0 ; i<seriesLength && ok ; ++i) {
                            double value = M::abs(rawData[i]);

                            value = rescaleValue(value, source2AxisScale, ok);

                            values[i].setY(value);
                            source2MinMax.update(value);
                        }
                    } else {
                        for (Model::Integer index=0 ; index<seriesLength && ok ; ++index) {
                            double value = M::abs(matrix.at(index + 1));

                            value = rescaleValue(value, source2AxisScale, ok);

                            values[index].setY(value);
                            source2MinMax.update(value);
                        }
                    }

                    if (!ok) {
                        errorReason = tr("Series %1, invalid value in second data source.").arg(seriesIndex+1);
                    }

                    break;
                }

                case Model::ValueType::NUMBER_TYPES: {
                    Q_ASSERT(false);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }
        }
    }

    if (errorReason.isEmpty()) {
        if (currentSplineType == SplineType::LINEAR_REGRESSION) {
            if (source1AxisScale == AxisScale::LINEAR && source2AxisScale == AxisScale::LINEAR) {
                std::pair<double, double> slopeAndIntercept = calculateOLSLinearRegression(values);
                double m = slopeAndIntercept.first;
                double b = slopeAndIntercept.second;

                currentSeries1->clear();
                currentSeries1->append(source1MinMax.minimum(), m * source1MinMax.minimum() + b);
                currentSeries1->append(source1MinMax.maximum(), m * source1MinMax.maximum() + b);

                currentSeries1->setName(tr("(m=%2, b=%3)").arg(m, 6).arg(b, 6));
            } else {
                errorReason = tr("Linear regressions are only supported for linear scales.");
            }
        } else {
            currentSeries1->replace(values);
        }

        if (currentSeries2 != Q_NULLPTR) {
            currentSeries2->replace(values);
        }
    }

    return SeriesUpdateResults(errorReason, source1MinMax, source2MinMax);
}


Plot2DPresentationData::SeriesData& Plot2DPresentationData::SeriesData::operator=(
        const Plot2DPresentationData::SeriesData& other
    ) {
    currentSplineType = other.currentSplineType;
    currentSeries1    = other.currentSeries1;
    currentSeries2    = other.currentSeries2;

    for (unsigned sourceIndex=0 ; sourceIndex<numberSourcesPerSeries ; ++sourceIndex) {
        currentAxisLocations[sourceIndex]   = other.currentAxisLocations[sourceIndex];
        currentDataSourceNames[sourceIndex] = other.currentDataSourceNames[sourceIndex];
        currentValues[sourceIndex]          = other.currentValues[sourceIndex];
    }

    return *this;
}


unsigned long Plot2DPresentationData::SeriesData::sourceLength(
        const Model::Variant& variant,
        unsigned              seriesIndex,
        QString&              errorReason
    ) {
    unsigned long result = 0;

    switch (variant.valueType()) {
        case Model::ValueType::NONE: {
            break;
        }

        case Model::ValueType::BOOLEAN:
        case Model::ValueType::INTEGER:
        case Model::ValueType::REAL:
        case Model::ValueType::COMPLEX:
        case Model::ValueType::SET: {
            errorReason = tr("Series %1, invalid data type for first data source.").arg(seriesIndex + 1);
            break;
        }

        case Model::ValueType::TUPLE: {
            const Model::Tuple& source1 = variant.toTuple();
            result = static_cast<unsigned long>(source1.size());

            if (result == 0) {
                errorReason = tr("Series %1, empty tuple for first data source.").arg(seriesIndex + 1);
            }

            break;
        }

        case Model::ValueType::MATRIX_BOOLEAN: {
            const Model::MatrixBoolean& matrix = variant.toMatrixBoolean();
            result = static_cast<unsigned long>(matrix.numberCoefficients());

            if (result == 0) {
                errorReason = tr("Series %1, empty matrix for first data source.").arg(seriesIndex + 1);
            }

            break;
        }

        case Model::ValueType::MATRIX_INTEGER: {
            const Model::MatrixInteger& matrix = variant.toMatrixInteger();
            result = static_cast<unsigned long>(matrix.numberCoefficients());

            if (result == 0) {
                errorReason = tr("Series %1, empty matrix for first data source.").arg(seriesIndex + 1);
            }

            break;
        }

        case Model::ValueType::MATRIX_REAL: {
            const Model::MatrixReal& matrix = variant.toMatrixReal();
            result = static_cast<unsigned long>(matrix.numberCoefficients());

            if (result == 0) {
                errorReason = tr("Series %1, empty matrix for first data source.").arg(seriesIndex + 1);
            }

            break;
        }

        case Model::ValueType::MATRIX_COMPLEX: {
            const Model::MatrixComplex& matrix = variant.toMatrixComplex();
            result = static_cast<unsigned long>(matrix.numberCoefficients());

            if (result == 0) {
                errorReason = tr("Series %1, empty matrix for first data source.").arg(seriesIndex + 1);
            }

            break;
        }

        case Model::ValueType::NUMBER_TYPES: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}

/***********************************************************************************************************************
 * Plot2DPlotPresentationData
 */

Plot2DPresentationData::Plot2DPresentationData(
        EQt::GraphicsItem* chartItem
    ):PlotWrappedDataPresentationData(
        chartItem
    ) {}


Plot2DPresentationData::~Plot2DPresentationData() {}


void Plot2DPresentationData::setAxisFormat(
        Plot2DPresentationData::AxisLocation axisLocation,
        const Ld::ChartAxisFormat&           axisFormat,
        QAbstractAxis*                       axis
    ) {
    axisDataByLocation.insert(axisLocation, AxisData(axisLocation, axisFormat, axis));

    axis->setMin(0.000001);
    axis->setMax(1);
}


//void Plot2DPresentationData::clearAxisFormats() {
//    axisDataByLocation.clear();
//}


void Plot2DPresentationData::setSeries(
        unsigned                           seriesIndex,
        Plot2DPresentationData::SplineType splineType,
        QXYSeries*                         series1,
        QXYSeries*                         series2
    ) {
    unsigned numberSeries = static_cast<unsigned>(currentSeriesData.size());
    if (seriesIndex < numberSeries) {
        currentSeriesData[seriesIndex] = SeriesData(splineType, series1, series2);
    } else {
        while (numberSeries < seriesIndex) {
            currentSeriesData.append(SeriesData(SplineType::NONE, Q_NULLPTR, Q_NULLPTR));
            ++numberSeries;
        }

        currentSeriesData.append(SeriesData(splineType, series1, series2));
    }
}


void Plot2DPresentationData::setDataSource(
        unsigned                             valueIndex,
        unsigned                             seriesIndex,
        unsigned                             sourceIndex,
        Plot2DPresentationData::AxisLocation axisLocation,
        const Ld::VariableName&              dataSourceName
    ) {
    Q_ASSERT(seriesIndex < static_cast<unsigned>(currentSeriesData.size()));
    Q_ASSERT(sourceIndex < numberSourcesPerSeries);

    Q_ASSERT(axisDataByLocation.contains(axisLocation));
    const AxisData& axisData = axisDataByLocation.value(axisLocation);
    AxisScale       axisScale = axisData.axisFormat().axisScale();

    currentSeriesData[seriesIndex].setDataSource(sourceIndex, axisLocation, axisScale, dataSourceName);
    mapValueIndex(seriesIndex, sourceIndex, valueIndex);
}


void Plot2DPresentationData::clearDataSources() {}


void Plot2DPresentationData::performDeferredUpdates() {
    SeriesMinMax seriesMinMaxByLocation[static_cast<unsigned>(AxisLocation::NUMBER_AXIS_LOCATIONS)];

    unsigned            numberDataSeries = static_cast<unsigned>(currentSeriesData.size());
    unsigned            seriesIndex      = 0;
    SeriesUpdateResults updateResults;

    while (updateResults.errorReason().isEmpty() && seriesIndex < numberDataSeries) {
        SeriesData& seriesData = currentSeriesData[seriesIndex];
        updateResults = seriesData.updateSeries(seriesIndex);

        SeriesMinMax& series1MinMax = seriesMinMaxByLocation[static_cast<unsigned>(seriesData.axisLocation(0))];
        SeriesMinMax& series2MinMax = seriesMinMaxByLocation[static_cast<unsigned>(seriesData.axisLocation(1))];

        series1MinMax.update(updateResults.series1MinMax());
        series2MinMax.update(updateResults.series2MinMax());

        ++seriesIndex;
    }

    if (updateResults.errorReason().isEmpty()) {
        QString errorReason;

        QHash<AxisLocation, AxisData>::const_iterator axisIterator    = axisDataByLocation.constBegin();
        QHash<AxisLocation, AxisData>::const_iterator axisEndIterator = axisDataByLocation.constEnd();

        while (errorReason.isEmpty() && axisIterator != axisEndIterator) {
            AxisLocation               axisLocation  = axisIterator.key();
            const AxisData&            axisData = axisIterator.value();
            SeriesMinMaxAndErrorStatus result   = configureAxis(
                axisData,
                seriesMinMaxByLocation[static_cast<unsigned>(axisLocation)]
            );

            errorReason = result.errorReason();
            ++axisIterator;
        }

        if (!errorReason.isEmpty()) {
            showErrorMessage(errorReason);
        } else {
            clearErrorMessage();
        }
    } else {
        showErrorMessage(updateResults.errorReason());
    }
}


Plot2DPresentationData::SeriesData* Plot2DPresentationData::seriesData(unsigned valueIndex) {
    SeriesData* result;

    if (valueIndex < static_cast<unsigned>(currentSeriesData.size())) {
        result = &(currentSeriesData[valueIndex]);
    } else {
        result = Q_NULLPTR;
    }

    return result;
}


unsigned Plot2DPresentationData::numberSeriesDataValues() const {
    return static_cast<unsigned>(currentSeriesData.size());
}
