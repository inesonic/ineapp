/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HeatChartPresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QList>
#include <QRectF>
#include <QFont>
#include <QFontMetricsF>
#include <QScatterSeries>
#include <QAbstractAxis>
#include <QValueAxis>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QBrush>
#include <QPen>

#include <cmath>

#include <model_api_types.h>
#include <model_variant.h>
#include <model_matrix_boolean.h>
#include <model_matrix_integer.h>
#include <model_matrix_real.h>
#include <model_matrix_complex.h>
#include <m_basic_functions.h>

#include <eqt_charts.h>
#include <eqt_chart_item.h>
#include <eqt_graphics_pixmap_item.h>
#include <eqt_graphics_item_group.h>

#include "presentation.h"
#include "plot_wrapped_presentation_data.h"
#include "heat_chart_presentation_data.h"

template<typename T> static inline QColor toColor(
        T      v,
        double multiplier,
        T      minimum,
        int    hue,
        int    saturation,
        int    value
    ) {
    int h = hue + static_cast<int>(multiplier * (v - minimum) + 0.5);
    if (h >= 360) {
        h -= 360;
    } else if (h < 0) {
        h += 360;
    }

    return QColor::fromHsv(h, saturation, value);
}


HeatChartPresentationData::HeatChartPresentationData(
        EQt::ChartItem* chartItem
    ):PlotWrappedPresentationData(
        chartItem
    ) {
    dummySeries       = Q_NULLPTR;
    currentPixmapItem = new EQt::GraphicsPixmapItem;

    dynamic_cast<EQt::GraphicsItemGroup*>(graphicsItem())->addToGroup(currentPixmapItem);
    currentPixmapItem->setZValue(Presentation::maximumEmbellishmentsZHeight);
    currentPixmapItem->setShapeMode(QGraphicsPixmapItem::ShapeMode::BoundingRectShape);
    currentPixmapItem->setTransformationMode(Qt::TransformationMode::SmoothTransformation);
}


HeatChartPresentationData::~HeatChartPresentationData() {
    if (currentPixmapItem != Q_NULLPTR) {
        currentPixmapItem->deleteLater();
    }
}


void HeatChartPresentationData::calculatedValueUpdated(
        unsigned                   valueIndex,
        const Ld::CalculatedValue& calculatedValue
    ) {
    if (valueIndex == 0) {
        currentCalculatedValue = calculatedValue;
        updatePlot();
    }
}


void HeatChartPresentationData::calculatedValuesCleared() {
    currentCalculatedValue = Ld::CalculatedValue();
}


void HeatChartPresentationData::setSeries(
        const Ld::VariableName&                 variableName,
        const QString&                          seriesName,
        const QColor&                           seriesColor,
        HeatChartPresentationData::GradientType gradientType
    ) {
    currentSeriesVariableName = variableName;
    currentSeriesLabel        = seriesName;
    currentSeriesColor        = seriesColor;
    currentGradientType       = gradientType;
}


void HeatChartPresentationData::setLeftAxisFormat(QValueAxis* axis, const Ld::ChartAxisFormat& axisFormat) {
    currentleftAxis       = axis;
    currentLeftAxisFormat = axisFormat;
}


void HeatChartPresentationData::setBottomAxisFormat(
        QValueAxis*                axis,
        const Ld::ChartAxisFormat& axisFormat
    ) {
    currentBottomAxis       = axis;
    currentBottomAxisFormat = axisFormat;
}


void HeatChartPresentationData::updatePlot() {
    QChart* chart = chartItem();

    if (dummySeries != Q_NULLPTR) {
        chart->removeAllSeries();
    }

    dummySeries = new QScatterSeries;
    chart->addSeries(dummySeries);

    dummySeries->setName(currentSeriesLabel);
    dummySeries->setMarkerShape(QScatterSeries::MarkerShape::MarkerShapeRectangle);
    dummySeries->setMarkerSize(1.0);

    dummySeries->setColor(currentSeriesColor);

    dummySeries->attachAxis(currentBottomAxis);
    dummySeries->attachAxis(currentleftAxis);

    dummySeries->append(-0.1, -0.1);
    dummySeries->append(0.1, 0.1);

    configureAxis(currentBottomAxis, AxisLocation::BOTTOM_X_A_GM, currentBottomAxisFormat);
    configureAxis(currentleftAxis, AxisLocation::LEFT_Y_R_RC, currentLeftAxisFormat);

    QRectF rectangle = chart->plotArea();
    currentPixmapItem->setPos(rectangle.topLeft());

    const Model::Variant& variant = currentCalculatedValue.variant();
    Model::ValueType      valueType = variant.valueType();
    QString               errorString;
    QImage                image;

    switch (valueType) {
        case Model::ValueType::NONE:
        case Model::ValueType::BOOLEAN:
        case Model::ValueType::INTEGER:
        case Model::ValueType::REAL:
        case Model::ValueType::COMPLEX:
        case Model::ValueType::SET:
        case Model::ValueType::TUPLE: {
            errorString = tr("Heat charts can only be generated from matrix types.");
            break;
        }

        case Model::ValueType::MATRIX_BOOLEAN: {
            bool                 ok;
            Model::MatrixBoolean matrix = variant.toMatrixBoolean(&ok);
            Q_ASSERT(ok);

            Model::Integer numberRows    = matrix.numberRows();
            Model::Integer numberColumns = matrix.numberColumns();

            if (numberRows > 0 && numberColumns > 0) {
                image = matrixToPixmap(matrix, currentSeriesColor, currentGradientType);
            } else {
                errorString = tr("Can not generate a heat map from an empty matrix.");
            }

            break;
        }

        case Model::ValueType::MATRIX_INTEGER: {
            bool                 ok;
            Model::MatrixInteger matrix = variant.toMatrixInteger(&ok);
            Q_ASSERT(ok);

            Model::Integer numberRows    = matrix.numberRows();
            Model::Integer numberColumns = matrix.numberColumns();

            if (numberRows > 0 && numberColumns > 0) {
                image = matrixToPixmap(matrix, currentSeriesColor, currentGradientType);
            } else {
                errorString = tr("Can not generate a heat map from an empty matrix.");
            }

            break;
        }

        case Model::ValueType::MATRIX_REAL: {
            bool              ok;
            Model::MatrixReal matrix = variant.toMatrixReal(&ok);
            Q_ASSERT(ok);

            Model::Integer numberRows    = matrix.numberRows();
            Model::Integer numberColumns = matrix.numberColumns();

            if (numberRows > 0 && numberColumns > 0) {
                image = matrixToPixmap(matrix, currentSeriesColor, currentGradientType);
            } else {
                errorString = tr("Can not generate a heat map from an empty matrix.");
            }

            break;
        }

        case Model::ValueType::MATRIX_COMPLEX: {
            bool                 ok;
            Model::MatrixComplex matrix = variant.toMatrixComplex(&ok);
            Q_ASSERT(ok);

            Model::Integer numberRows    = matrix.numberRows();
            Model::Integer numberColumns = matrix.numberColumns();

            if (numberRows > 0 && numberColumns > 0) {
                image = matrixToPixmap(matrix, currentSeriesColor, currentGradientType);
            } else {
                errorString = tr("Can not generate a heat map from an empty matrix.");
            }

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    QSize  rawImageSizeInSceneUnits = image.size();
    QSizeF requiredSizeInSceneUnits = rectangle.size();
    double xScaleFactor             = requiredSizeInSceneUnits.width() / rawImageSizeInSceneUnits.width();
    double yScaleFactor             = requiredSizeInSceneUnits.height() / rawImageSizeInSceneUnits.height();
    QTransform transform(
        xScaleFactor, 0,            0,
        0,            yScaleFactor, 0,
        0,            0,            1
    );
    QImage scaledImage = image.transformed(transform, Qt::TransformationMode::FastTransformation);
    currentPixmapItem->setPixmap(QPixmap::fromImage(scaledImage));
}


QImage HeatChartPresentationData::matrixToPixmap(
        const Model::MatrixBoolean&             matrix,
        const QColor&                           color,
        HeatChartPresentationData::GradientType gradientType
    ) {
    Model::Integer numberRows    = matrix.numberRows();
    Model::Integer numberColumns = matrix.numberColumns();

    int hue        = color.hue();
    int saturation = color.saturation();

    QColor zeroColor  = QColor::fromHsv(hue, saturation, 26);
    QColor oneColor   = QColor::fromHsv(hue, saturation, 229);
    if (gradientType == GradientType::NONE || gradientType == GradientType::INTENSITY) {
        zeroColor  = QColor::fromHsv(hue, saturation, 0);
        oneColor   = QColor::fromHsv(hue, saturation, 255);
    } else {
        int    value        = color.value();
        double maximumAngle = static_cast<double>(gradientType);

        zeroColor = color;
        oneColor  = toColor(1, maximumAngle, 0, hue, saturation, value);
    }

    QImage image(static_cast<int>(numberColumns), static_cast<int>(numberRows), QImage::Format::Format_RGB32);
    for (Model::Integer columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
        for (Model::Integer rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
            Model::Boolean b = matrix.at(rowIndex + 1, columnIndex + 1);
            image.setPixelColor(
                static_cast<unsigned>(columnIndex),
                static_cast<unsigned>(rowIndex),
                b ? oneColor : zeroColor
            );
        }
    }

    return image;
}


QImage HeatChartPresentationData::matrixToPixmap(
        const Model::MatrixInteger&             matrix,
        const QColor&                           color,
        HeatChartPresentationData::GradientType gradientType
    ) {
    Model::Integer numberRows    = matrix.numberRows();
    Model::Integer numberColumns = matrix.numberColumns();

    Model::Integer minimumValue = std::numeric_limits<Model::Integer>::max();
    Model::Integer maximumValue = std::numeric_limits<Model::Integer>::min();

    int hue        = color.hue();
    int saturation = color.saturation();

    for (Model::Integer columnIndex=1 ; columnIndex<=numberColumns ; ++columnIndex) {
        for (Model::Integer rowIndex=1 ; rowIndex<=numberRows ; ++rowIndex) {
            Model::Integer v = matrix.at(rowIndex, columnIndex);
            if (v > maximumValue) {
                maximumValue = v;
            }

            if (v < minimumValue) {
                minimumValue = v;
            }
        }
    }

    QImage image(
        static_cast<int>(numberColumns),
        static_cast<int>(numberRows),
        QImage::Format::Format_RGB32
    );

    if (gradientType == GradientType::NONE || gradientType == GradientType::INTENSITY) {
        double multiplier;
        if (maximumValue >= 0x7FFFFFFFFFFFFFFFLL || minimumValue <= -0x7FFFFFFFFFFFFFFF) {
            multiplier = 127.0 / (maximumValue / 2 - minimumValue / 2);
        } else {
            multiplier = 255.0 / (maximumValue - minimumValue);
        }

        for (Model::Integer columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            for (Model::Integer rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                Model::Integer i = matrix.at(rowIndex + 1, columnIndex + 1);
                QColor         c = QColor::fromHsv(hue, saturation, static_cast<int>(multiplier * (i - minimumValue)));
                image.setPixelColor(static_cast<unsigned>(columnIndex), static_cast<unsigned>(rowIndex), c);
            }
        }
    } else {
        int    value        = color.value();
        double maximumAngle = static_cast<double>(gradientType);
        double multiplier;
        if (maximumValue >= 0x7FFFFFFFFFFFFFFFLL || minimumValue <= -0x7FFFFFFFFFFFFFFF) {
            multiplier = (maximumAngle / 2) / (maximumValue / 2 - minimumValue / 2);
        } else {
            multiplier = maximumAngle / (maximumValue - minimumValue);
        }

        for (Model::Integer columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            for (Model::Integer rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                Model::Integer i = matrix.at(rowIndex + 1, columnIndex + 1);
                QColor         c = toColor(i, multiplier, minimumValue, hue, saturation, value);
                image.setPixelColor(static_cast<unsigned>(columnIndex), static_cast<unsigned>(rowIndex), c);
            }
        }
    }

    return image;
}


QImage HeatChartPresentationData::matrixToPixmap(
        const Model::MatrixReal&                matrix,
        const QColor&                           color,
        HeatChartPresentationData::GradientType gradientType
    ) {
    Model::Integer numberRows    = matrix.numberRows();
    Model::Integer numberColumns = matrix.numberColumns();

    Model::Real minimumValue = std::numeric_limits<Model::Real>::max();
    Model::Real maximumValue = std::numeric_limits<Model::Real>::lowest();

    int hue        = color.hue();
    int saturation = color.saturation();

    for (Model::Integer columnIndex=1 ; columnIndex<=numberColumns ; ++columnIndex) {
        for (Model::Integer rowIndex=1 ; rowIndex<=numberRows ; ++rowIndex) {
            Model::Real v = matrix.at(rowIndex, columnIndex);
            if (v > maximumValue) {
                maximumValue = v;
            }

            if (v < minimumValue) {
                minimumValue = v;
            }
        }
    }

    QImage image(
        static_cast<int>(numberColumns),
        static_cast<int>(numberRows),
        QImage::Format::Format_RGB32
    );

    if (gradientType == GradientType::NONE || gradientType == GradientType::INTENSITY) {
        double multiplier = 255.0 / (maximumValue - minimumValue);
        for (Model::Integer columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            for (Model::Integer rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                Model::Real r = matrix.at(rowIndex + 1, columnIndex + 1);
                QColor      c = QColor::fromHsv(hue, saturation, static_cast<int>(multiplier * (r - minimumValue)));
                image.setPixelColor(static_cast<unsigned>(columnIndex), static_cast<unsigned>(rowIndex), c);
            }
        }
    } else {
        int    value        = color.value();
        double maximumAngle = static_cast<double>(gradientType);
        double multiplier   = maximumAngle / (maximumValue - minimumValue);
        for (Model::Integer columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            for (Model::Integer rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                Model::Real r = matrix.at(rowIndex + 1, columnIndex + 1);
                QColor      c = toColor(r, multiplier, minimumValue, hue, saturation, value);
                image.setPixelColor(static_cast<unsigned>(columnIndex), static_cast<unsigned>(rowIndex), c);
            }
        }
    }

    return image;
}


QImage HeatChartPresentationData::matrixToPixmap(
        const Model::MatrixComplex&             matrix,
        const QColor&                           color,
        HeatChartPresentationData::GradientType gradientType
    ) {
    Model::Integer numberRows    = matrix.numberRows();
    Model::Integer numberColumns = matrix.numberColumns();

    Model::Real minimumValue = std::numeric_limits<Model::Real>::max();
    Model::Real maximumValue = std::numeric_limits<Model::Real>::lowest();

    int hue        = color.hue();
    int saturation = color.saturation();

    Model::MatrixReal realMatrix(numberRows, numberColumns);

    for (Model::Integer columnIndex=1 ; columnIndex<=numberColumns ; ++columnIndex) {
        for (Model::Integer rowIndex=1 ; rowIndex<=numberRows ; ++rowIndex) {
            Model::Complex c = matrix.at(rowIndex, columnIndex);
            Model::Real    r = M::abs(c);

            realMatrix.update(rowIndex + 1, columnIndex + 1, r);

            if (r > maximumValue) {
                maximumValue = r;
            }

            if (r < minimumValue) {
                minimumValue = r;
            }
        }
    }

    QImage image(
        static_cast<int>(numberColumns),
        static_cast<int>(numberRows),
        QImage::Format::Format_RGB32
    );

    if (gradientType == GradientType::NONE || gradientType == GradientType::INTENSITY) {
        double      multiplier  = 255.0 * (maximumValue - minimumValue);
        for (Model::Integer columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            for (Model::Integer rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                Model::Real r = realMatrix.at(rowIndex + 1, columnIndex + 1);
                QColor      c = QColor::fromHsv(hue, saturation, static_cast<int>(multiplier * (r - minimumValue)));
                image.setPixelColor(static_cast<unsigned>(columnIndex), static_cast<unsigned>(rowIndex), c);
            }
        }
    } else {
        int    value        = color.value();
        double maximumAngle = static_cast<double>(gradientType);
        double multiplier   = maximumAngle / (maximumValue - minimumValue);
        for (Model::Integer columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            for (Model::Integer rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
                Model::Real r = realMatrix.at(rowIndex + 1, columnIndex + 1);
                QColor      c = toColor(r, multiplier, minimumValue, hue, saturation, value);
                image.setPixelColor(static_cast<unsigned>(columnIndex), static_cast<unsigned>(rowIndex), c);
            }
        }
    }

    return image;
}


void HeatChartPresentationData::configureAxis(
        QValueAxis*                             axis,
        HeatChartPresentationData::AxisLocation axisLocation,
        const Ld::ChartAxisFormat&              axisFormat
    ) {
    QRectF plotAreaRectangle = chartItem()->plotArea();

    double minimum       = axisFormat.minimumValue();
    double maximum       = axisFormat.maximumValue();
    double majorStepSize = axisFormat.majorStepSize();
    double minorStepSize = axisFormat.minorStepSize();

    if (std::isinf(minimum)) {
        if (std::isinf(maximum)) {
            minimum = 0;
            maximum = 1;
        } else {
            minimum = maximum - 1.0;
        }
    } else if (std::isinf(maximum)) {
        maximum = minimum + 1;
    }

    SeriesMinMax adjustedRange(minimum, maximum);
    QString      errorReason;

    QFont  numberFont         = axis->labelsFont();
    float  maximumNumberTicks = 0;

    if (minimum > maximum) {
        axis->setReverse(true);
        axis->setRange(maximum, minimum);
    } else if (minimum == maximum) {
        axis->setReverse(false);
        axis->setRange(minimum - 0.5, maximum + 0.5);
    } else {
        axis->setReverse(false);
        axis->setRange(minimum, maximum);
    }

    QFontMetricsF numberFontMetrics(numberFont);

    float  requiredNumberTicks = 0;
    double span                = std::abs(maximum - minimum);
    if (axisLocation == AxisLocation::BOTTOM_X_A_GM) {
        QString minimumLabel = QString::number(minimum);
        QString maximumLabel = QString::number(maximum);

        float labelWidth = std::max(
            numberFontMetrics.horizontalAdvance(minimumLabel + "MM"),
            numberFontMetrics.horizontalAdvance(maximumLabel + "MM")
        );

        maximumNumberTicks = plotAreaRectangle.width() / labelWidth;

        bool rotateLabels;

        if (majorStepSize != Ld::ChartAxisFormat::automaticScaling) {
            requiredNumberTicks = span / std::abs(majorStepSize);
            rotateLabels        = (requiredNumberTicks > maximumNumberTicks);
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
        requiredNumberTicks = span / std::abs(majorStepSize);
        maximumNumberTicks  = plotAreaRectangle.height() / (2.0 * numberFontMetrics.height());
    }

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

    axis->setTickAnchor(std::min(maximum, maximum));

    double numberTicks = std::abs(maximum - minimum) / adjustedMajorStepSize;
    double real;
    double fractional  = std::modf(numberTicks, &real);

    if (fractional / real < 1.0E-4) {
        axis->setTickCount(static_cast<unsigned>(numberTicks) + 1);
    } else {
        axis->setTickInterval(adjustedMajorStepSize);
    }

    int minorTicksBetweenMajorTicks = std::max(
        static_cast<int>(adjustedMajorStepSize / adjustedMinorStepSize - 0.5),
        0
    );

    axis->setMinorTickCount(minorTicksBetweenMajorTicks);
}
