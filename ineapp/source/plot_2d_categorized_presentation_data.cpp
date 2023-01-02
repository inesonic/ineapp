/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Plot2DCategorizedPresentationData class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>
#include <QGraphicsItem>
#include <QChart>
#include <QFont>
#include <QFontMetricsF>
#include <QXYSeries>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QVector>

#include <algorithm>
#include <cmath>

#include <util_string.h>

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
#include "plot_2d_categorized_presentation_data.h"

/***********************************************************************************************************************
 * Plot2DCategorizedPresentationData:SeriesData
 */

Plot2DCategorizedPresentationData::SeriesData::SeriesData() {}


Plot2DCategorizedPresentationData::SeriesData::SeriesData(
        Plot2DCategorizedPresentationData::AxisScale axisScale,
        const Ld::VariableName&                      dataSourceName,
        const QString&                               seriesLabel
    ):currentAxisScale(
        axisScale
    ),currentDataSourceName(
        dataSourceName
    ),currentSeriesLabel(
        seriesLabel
    ) {}


Plot2DCategorizedPresentationData::SeriesData::SeriesData(
        const Plot2DCategorizedPresentationData::SeriesData& other
    ):currentAxisScale(
        other.currentAxisScale
    ),currentDataSourceName(
        other.currentDataSourceName
    ),currentSeriesLabel(
        other.currentSeriesLabel
    ),currentCalculatedValue(
        other.currentCalculatedValue
    ) {}


Plot2DCategorizedPresentationData::SeriesData::~SeriesData() {}


void Plot2DCategorizedPresentationData::SeriesData::setCalculatedValue(
        unsigned                   sourceIndex,
        const Ld::CalculatedValue& calculatedValue
    ) {
    Q_ASSERT(sourceIndex == 0);
    currentCalculatedValue = calculatedValue;
}


void Plot2DCategorizedPresentationData::SeriesData::calculatedValuesCleared() {
    currentCalculatedValue = Ld::CalculatedValue();
}


QStringList Plot2DCategorizedPresentationData::SeriesData::toStringList(bool* ok) const {
    bool        isOk;
    QStringList result;

    const Model::Variant& variant   = currentCalculatedValue.variant();
    Model::ValueType      valueType = variant.valueType();

    switch (valueType) {
        case Ld::DataType::ValueType::NONE: {
            isOk = false;
            break;
        }

        case Ld::DataType::ValueType::BOOLEAN:
        case Ld::DataType::ValueType::INTEGER:
        case Ld::DataType::ValueType::REAL:
        case Ld::DataType::ValueType::COMPLEX: {
            isOk = false;
            break;
        }

        case Ld::DataType::ValueType::SET: {
            const Model::Set& vl = variant.toSet(&isOk);
            if (isOk) {
                Model::Set::ConstIterator it = vl.constBegin();
                Model::Set::ConstIterator end = vl.constEnd();
                while (isOk && it != end) {
                    QString label = variantToString(*it, &isOk);
                    if (isOk) {
                        result.append(label);
                        ++it;
                    }
                }
            }

            break;
        }

        case Ld::DataType::ValueType::TUPLE: {
            const Model::Tuple& vl = variant.toTuple(&isOk);
            if (isOk) {
                unsigned listSize = static_cast<unsigned>(vl.size());
                unsigned index    = 1;

                while (isOk && index <= listSize) {
                    QString label = variantToString(vl.at(index), &isOk);
                    if (isOk) {
                        result.append(label);
                        ++index;
                    }
                }
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_BOOLEAN: {
            const Model::MatrixBoolean& vl = variant.toMatrixBoolean(&isOk);
            if (isOk) {
                unsigned listSize = static_cast<unsigned>(vl.numberCoefficients());
                unsigned index    = 1;

                while (isOk && index < listSize) {
                    QString label = Util::booleanToUnicodeString(
                        static_cast<bool>(vl.at(index)),
                        Util::BooleanStyle::LOWER_CASE_TRUE_FALSE
                    );

                    result.append(label);
                    ++index;
                }
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_INTEGER: {
            const Model::MatrixInteger& vl = variant.toMatrixInteger(&isOk);
            if (isOk) {
                unsigned listSize = static_cast<unsigned>(vl.numberCoefficients());
                unsigned index    = 1;

                while (isOk && index < listSize) {
                    QString label = Util::longLongIntegerToUnicodeString(vl.at(index), Util::IntegerNumberStyle::NONE);

                    result.append(label);
                    ++index;
                }
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_REAL: {
            const Model::MatrixReal& vl = variant.toMatrixReal(&isOk);
            if (isOk) {
                unsigned listSize = static_cast<unsigned>(vl.numberCoefficients());
                unsigned index    = 1;

                while (isOk && index < listSize) {
                    QString label = Util::longDoubleToUnicodeString(
                        vl.at(index),
                        Util::RealNumberStyle::CONCISE,
                        Util::inferPrecision
                    );

                    result.append(label);
                    ++index;
                }
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_COMPLEX: {
            const Model::MatrixComplex& vl = variant.toMatrixReal(&isOk);
            if (isOk) {
                unsigned listSize = static_cast<unsigned>(vl.numberCoefficients());
                for (unsigned index=1 ; index<=listSize ; ++index) {
                    QString label = complexToString(vl.at(index));
                    result.append(label);
                }
            }

            break;
        }

        case Ld::DataType::ValueType::NUMBER_TYPES: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    if (ok != Q_NULLPTR) {
        *ok = isOk;
    }

    return result;
}


QVector<double> Plot2DCategorizedPresentationData::SeriesData::toVector(
        Plot2DCategorizedPresentationData::SeriesMinMax* seriesMinMax,
        bool*                                            ok
    ) const {
    return variantToVector(currentCalculatedValue.variant(), currentAxisScale, seriesMinMax, ok);
}


Plot2DCategorizedPresentationData::SeriesData& Plot2DCategorizedPresentationData::SeriesData::operator=(
        const Plot2DCategorizedPresentationData::SeriesData& other
    ) {
    currentAxisScale       = other.currentAxisScale;
    currentDataSourceName  = other.currentDataSourceName;
    currentSeriesLabel     = other.currentSeriesLabel;
    currentCalculatedValue = other.currentCalculatedValue;

    return *this;
}


QVector<double> Plot2DCategorizedPresentationData::SeriesData::variantToVector(
        const Model::Variant&                            variant,
        Plot2DCategorizedPresentationData::AxisScale     axisScale,
        Plot2DCategorizedPresentationData::SeriesMinMax* seriesMinMax,
        bool*                                            ok
    ) {
    bool            isOk;
    QVector<double> result;

    Model::ValueType valueType = variant.valueType();
    switch (valueType) {
        case Ld::DataType::ValueType::NONE: {
            isOk = false;
            break;
        }

        case Ld::DataType::ValueType::BOOLEAN: {
            double v = rescaleValue(static_cast<double>(variant.toBoolean(&isOk)), axisScale, isOk);
            result.append(v);

            if (seriesMinMax != Q_NULLPTR) {
                seriesMinMax->update(v);
            }

            break;
        }

        case Ld::DataType::ValueType::INTEGER: {
            double v = rescaleValue(static_cast<double>(variant.toInteger(&isOk)), axisScale, isOk);
            result.append(v);

            if (seriesMinMax != Q_NULLPTR) {
                seriesMinMax->update(v);
            }

            break;
        }

        case Ld::DataType::ValueType::REAL: {
            double v = rescaleValue(static_cast<double>(variant.toReal(&isOk)), axisScale, isOk);
            result.append(v);

            if (seriesMinMax != Q_NULLPTR) {
                seriesMinMax->update(v);
            }

            break;
        }

        case Ld::DataType::ValueType::COMPLEX: {
            double v = rescaleValue(M::abs(variant.toComplex(&isOk)), axisScale, isOk);
            result.append(v);

            if (seriesMinMax != Q_NULLPTR) {
                seriesMinMax->update(v);
            }

            break;
        }

        case Ld::DataType::ValueType::SET: {
            Model::Set s = variant.toSet(&isOk);
            Q_ASSERT(isOk);

            result.reserve(s.size());

            Model::Set::ConstIterator it  = s.constBegin();
            Model::Set::ConstIterator end = s.constEnd();
            while (isOk && it != end) {
                const Model::Variant& v = it.constReference();
                result += variantToVector(v, axisScale, seriesMinMax, &isOk);
                ++it;
            }

            break;
        }

        case Ld::DataType::ValueType::TUPLE: {
            Model::Tuple t = variant.toTuple(&isOk);
            Q_ASSERT(isOk);

            Model::Integer tupleSize = t.size();
            Model::Integer index     = 1;

            result.reserve(tupleSize);

            while (isOk && index <= tupleSize) {
                const Model::Variant& v = t.at(index);
                result += variantToVector(v, axisScale, seriesMinMax, &isOk);
                ++index;
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_BOOLEAN: {
            Model::MatrixBoolean m = variant.toMatrixBoolean(&isOk);
            Q_ASSERT(isOk);

            Model::Integer numberCoefficients = m.numberCoefficients();
            result.reserve(numberCoefficients);

            Model::Integer index = 1;
            while (isOk && index <= numberCoefficients) {
                double v = rescaleValue(static_cast<double>(m.at(index)), axisScale, isOk);
                result.append(v);

                if (seriesMinMax != Q_NULLPTR) {
                    seriesMinMax->update(v);
                }

                ++index;
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_INTEGER: {
            Model::MatrixInteger m = variant.toMatrixInteger(&isOk);
            Q_ASSERT(isOk);

            Model::Integer numberCoefficients = m.numberCoefficients();
            result.reserve(numberCoefficients);

            Model::Integer index = 1;
            while (isOk && index <= numberCoefficients) {
                double v = rescaleValue(static_cast<double>(m.at(index)), axisScale, isOk);
                result.append(v);

                if (seriesMinMax != Q_NULLPTR) {
                    seriesMinMax->update(v);
                }

                ++index;
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_REAL: {
            Model::MatrixReal m = variant.toMatrixReal(&isOk);
            Q_ASSERT(isOk);

            Model::Integer numberCoefficients = m.numberCoefficients();
            result.reserve(numberCoefficients);

            Model::Integer index = 1;
            while (isOk && index <= numberCoefficients) {
                double v = rescaleValue(static_cast<double>(m.at(index)), axisScale, isOk);
                result.append(v);

                if (seriesMinMax != Q_NULLPTR) {
                    seriesMinMax->update(v);
                }

                ++index;
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_COMPLEX: {
            Model::MatrixComplex m = variant.toMatrixComplex(&isOk);
            Q_ASSERT(isOk);

            Model::Integer numberCoefficients = m.numberCoefficients();
            result.reserve(numberCoefficients);

            Model::Integer index = 1;
            while(isOk && index <= numberCoefficients) {
                double v = rescaleValue(M::abs(m.at(index)), axisScale, isOk);
                result.append(v);

                if (seriesMinMax != Q_NULLPTR) {
                    seriesMinMax->update(v);
                }

                ++index;
            }

            break;
        }

        case Ld::DataType::ValueType::NUMBER_TYPES: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    if (ok != Q_NULLPTR) {
        *ok = isOk;
    }

    return result;
}


QString Plot2DCategorizedPresentationData::SeriesData::variantToString(const Model::Variant& variant, bool* ok) {
    bool    isOk;
    QString result;

    Model::ValueType valueType = variant.valueType();
    switch (valueType) {
        case Ld::DataType::ValueType::NONE: {
            isOk = false;
            break;
        }

        case Ld::DataType::ValueType::BOOLEAN: {
            bool v = variant.toBoolean(&isOk);
            if (isOk) {
                result = Util::booleanToUnicodeString(v, Util::BooleanStyle::LOWER_CASE_TRUE_FALSE);
            }

            break;
        }

        case Ld::DataType::ValueType::INTEGER: {
            Model::Integer v = variant.toInteger(&isOk);
            if (isOk) {
                result = Util::longLongIntegerToUnicodeString(v, Util::IntegerNumberStyle::NONE);
            }

            break;
        }

        case Ld::DataType::ValueType::REAL: {
            Model::Real v = variant.toReal(&isOk);
            if (isOk) {
                result = Util::longDoubleToUnicodeString(
                    v,
                    Util::RealNumberStyle::CONCISE,
                    Util::inferPrecision
                );
            }

            break;
        }

        case Ld::DataType::ValueType::COMPLEX: {
            Model::Complex v = variant.toComplex(&isOk);
            if (isOk) {
                result = complexToString(v);
            }

            break;
        }

        case Ld::DataType::ValueType::SET: {
            isOk = false;
            break;
        }

        case Ld::DataType::ValueType::TUPLE: {
            const Model::Tuple& v = variant.toTuple(&isOk);
            if (isOk) {
                char* s = v.toString();
                if (s != Q_NULLPTR) {
                    result = QString::fromUtf8(s);
                    delete[] s;
                } else {
                    isOk = false;
                }
            }

            break;
        }

        case Ld::DataType::ValueType::MATRIX_BOOLEAN:
        case Ld::DataType::ValueType::MATRIX_INTEGER:
        case Ld::DataType::ValueType::MATRIX_REAL:
        case Ld::DataType::ValueType::MATRIX_COMPLEX: {
            isOk = false;
            break;
        }

        case Ld::DataType::ValueType::NUMBER_TYPES: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    if (ok != Q_NULLPTR) {
        *ok = isOk;
    }

    return result;
}


QString Plot2DCategorizedPresentationData::SeriesData::complexToString(const Model::Complex& value) {
    QString           result;
    Model::Complex::T rp = value.real();
    Model::Complex::T ip = value.imag();

    if (rp == 0) {
        if (ip == 0) {
            result = tr("0");
        } else {
            result = tr("%1i").arg(
                Util::longDoubleToUnicodeString(
                    ip,
                    Util::RealNumberStyle::FLOATING_POINT,
                    Util::inferPrecision
                )
            );
        }
    } else if (rp != 0) {
        if (ip == 0) {
            result = Util::longDoubleToUnicodeString(
                rp,
                Util::RealNumberStyle::CONCISE,
                Util::inferPrecision
            );
        } else if (ip > 0) {
            result = tr("%1+%2i")
                     .arg(
                         Util::longDoubleToUnicodeString(
                             rp,
                             Util::RealNumberStyle::CONCISE,
                             Util::inferPrecision
                         )
                     )
                     .arg(
                         Util::longDoubleToUnicodeString(
                             ip,
                             Util::RealNumberStyle::CONCISE,
                             Util::inferPrecision
                         )
                     );
        } else /* if (ip < 0) */ {
            result = tr("%1%2i")
                     .arg(
                         Util::longDoubleToUnicodeString(
                             rp,
                             Util::RealNumberStyle::CONCISE,
                             Util::inferPrecision
                         )
                     )
                     .arg(
                         Util::longDoubleToUnicodeString(
                             ip,
                             Util::RealNumberStyle::CONCISE,
                             Util::inferPrecision
                         )
                     );
        }
    }

    return result;
}

/***********************************************************************************************************************
 * Plot2DPlotPresentationData
 */

const QColor Plot2DCategorizedPresentationData::defaultColors[Plot2DCategorizedPresentationData::numberColors] = {
    QColor(Qt::GlobalColor::blue),
    QColor(Qt::GlobalColor::red),
    QColor(Qt::GlobalColor::darkYellow),
    QColor(Qt::GlobalColor::black),
    QColor(Qt::GlobalColor::darkBlue),
    QColor(Qt::GlobalColor::darkRed),
    QColor(Qt::GlobalColor::darkGray),
    QColor(Qt::GlobalColor::cyan),
    QColor(Qt::GlobalColor::magenta)
};


Plot2DCategorizedPresentationData::Plot2DCategorizedPresentationData(
        EQt::GraphicsItem* chartItem
    ):PlotWrappedDataPresentationData(
        chartItem
    ) {
    EQt::GraphicsMultiTextGroup* group = new EQt::GraphicsMultiTextGroup;
    group->setBorderPen(QPen(Qt::PenStyle::NoPen));

    QGraphicsItem* graphicsItem = dynamic_cast<QGraphicsItem*>(chartItem);
    if (graphicsItem != Q_NULLPTR) {
        group->addToGroup(graphicsItem);
        graphicsItem->setPos(0, 0);
    }

    setGraphicsItem(group);
}


Plot2DCategorizedPresentationData::~Plot2DCategorizedPresentationData() {}


const QColor& Plot2DCategorizedPresentationData::defaultColor(unsigned index) {
    return defaultColors[index % numberColors];
}


QString Plot2DCategorizedPresentationData::addValueToLabel(const QString& label, double value) {
    QString result = label;

    if (label.contains("%v")) {
        result.replace("%v", QString::number(value));
    }

    return result;
}
