/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImageRenderPresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QList>
#include <QRectF>
#include <QFont>
#include <QFontMetricsF>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QBrush>
#include <QPen>
#include <QTransform>

#include <eqt_graphics_multi_text_group.h>
#include <eqt_graphics_pixmap_item.h>

#include <util_units.h>

#include <model_api_types.h>
#include <model_variant.h>
#include <model_matrix_boolean.h>
#include <model_matrix_integer.h>
#include <model_matrix_real.h>
#include <model_matrix_complex.h>
#include <m_basic_functions.h>

#include <eqt_graphics_pixmap_item.h>
#include <eqt_graphics_item_group.h>

#include "application.h"
#include "presentation.h"
#include "plot_wrapped_presentation_data.h"
#include "image_render_presentation_data.h"

/***********************************************************************************************************************
 * ImageRenderPresentationData::SeriesData
 */

ImageRenderPresentationData::SeriesData::SeriesData() {
    currentAxisLocation = AxisLocation::LEFT_Y_R_RC;
    currentSeriesIndex  = 0;
    currentValueIndex   = 0;
}


ImageRenderPresentationData::SeriesData::SeriesData(
        AxisLocation               axisLocation,
        unsigned                   seriesIndex,
        unsigned                   valueIndex,
        const Ld::CalculatedValue& calculatedValue
    ):currentAxisLocation(
        axisLocation
    ),currentSeriesIndex(
        seriesIndex
    ),currentValueIndex(
        valueIndex
    ),currentCalculatedValue(
        calculatedValue
    ) {}


ImageRenderPresentationData::SeriesData::SeriesData(
        const ImageRenderPresentationData::SeriesData& other
    ):currentAxisLocation(
        other.currentAxisLocation
    ),currentSeriesIndex(
        other.currentSeriesIndex
    ),currentValueIndex(
        other.currentValueIndex
    ),currentCalculatedValue(
        other.currentCalculatedValue
    ) {}


ImageRenderPresentationData::SeriesData::~SeriesData() {}


ImageRenderPresentationData::SeriesData& ImageRenderPresentationData::SeriesData::operator=(
        const ImageRenderPresentationData::SeriesData& other
    ) {
    currentAxisLocation    = other.currentAxisLocation;
    currentSeriesIndex     = other.currentSeriesIndex;
    currentValueIndex      = other.currentValueIndex;
    currentCalculatedValue = other.currentCalculatedValue;

    return *this;
}

/***********************************************************************************************************************
 * ImageRenderPresentationData
 */

ImageRenderPresentationData::ImageRenderPresentationData() {
    updateTimer.setSingleShot(true);
    connect(&updateTimer, &QTimer::timeout, this, &ImageRenderPresentationData::performUpdate);
}


ImageRenderPresentationData::~ImageRenderPresentationData() {
    if (currentPixmapItem != Q_NULLPTR) {
        currentPixmapItem->deleteLater();
    }
}


void ImageRenderPresentationData::configure(
        EQt::GraphicsMultiTextGroup* groupItem,
        EQt::GraphicsPixmapItem*     pixmapItem,
        const QSizeF&                sizeInPoints,
        const QPen&                  borderPen,
        const QPixmap&               defaultImage
    ) {
    setGraphicsItem(groupItem);

    currentBorderPen     = borderPen;
    currentPixmapItem    = pixmapItem;
    currentSizeInPoints  = sizeInPoints;
    currentDefaultPixmap = defaultImage;

    drawDefaultImage();
}


void ImageRenderPresentationData::calculatedValueUpdated(
        unsigned                   valueIndex,
        const Ld::CalculatedValue& calculatedValue
    ) {
    if (valueIndex < static_cast<unsigned>(seriesDataByValueIndex.size())) {
        seriesDataByValueIndex[valueIndex].setCalculatedValue(calculatedValue);
        updateTimer.start(0);
    }
}


void ImageRenderPresentationData::calculatedValuesCleared() {
    unsigned numberCalculatedValues = static_cast<unsigned>(seriesDataByValueIndex.size());
    for (unsigned i=0 ; i<numberCalculatedValues ; ++i) {
        seriesDataByValueIndex[i].setCalculatedValue(Ld::CalculatedValue());
    }

    drawDefaultImage();
}


QRectF ImageRenderPresentationData::boundingRectangle() const {
    return currentPixmapItem->boundingRect();
}


QRectF ImageRenderPresentationData::sceneBoundingRectangle() const {
    return currentPixmapItem->sceneBoundingRect();
}


void ImageRenderPresentationData::showErrorMessage(const QString& errorMessage) {
    QPixmap defaultPixmap(100, 100);
    QSizeF  pixelsPerPoint(
        100 / (defaultPixmap.widthMM() * Util::Units::pointsPerMm),
        100 / (defaultPixmap.heightMM() * Util::Units::pointsPerMm)
    );
    QSizeF realPixmapSize = toScene(currentSizeInPoints);
    QSize pixmapSize(
        static_cast<unsigned>(0.5 + realPixmapSize.width()),
        static_cast<unsigned>(0.5 + realPixmapSize.height())
    );
    QSize imageSize = currentDefaultPixmap.size();

    if (imageSize.width() <= pixmapSize.width() && imageSize.height() <= pixmapSize.height()) {
        QPixmap  pixmap(pixmapSize);
        pixmap.fill(QColor(Qt::GlobalColor::white));

        QPainter painter(&pixmap);

        QPoint imageTopLeft(
            (pixmapSize.width() - imageSize.width()) / 2,
            (pixmapSize.height() - imageSize.height()) / 2
        );
        painter.drawPixmap(imageTopLeft, currentDefaultPixmap);
        painter.end();

        currentPixmapItem->resetTransform();
        currentPixmapItem->setPixmap(pixmap);
    } else {
        QPixmap pixmap = currentDefaultPixmap.scaled(pixmapSize, Qt::AspectRatioMode::KeepAspectRatio);
        currentPixmapItem->resetTransform();
        currentPixmapItem->setPixmap(pixmap);
    }

    EQt::GraphicsMultiTextGroup* group = dynamic_cast<EQt::GraphicsMultiTextGroup*>(graphicsItem());

    QFont messageFont = Application::font();
    group->setTextPen(QPen(Presentation::errorBorderColor));
    group->setBorderPen(QPen(Presentation::errorBorderColor));

    messageFont.setPointSizeF((messageFont.pointSizeF() - 2) * Application::fontScaleFactor());
    QFontMetricsF messageFontMetrics(messageFont);
    float messageWidth  = messageFontMetrics.horizontalAdvance(errorMessage);
    float messageAscent = messageFontMetrics.ascent();
    float messageHeight = messageFontMetrics.height();

    QRectF  boundingRectangle = group->boundingRect();
    QPointF textLocation;
    if (boundingRectangle.isValid()) {
        textLocation = QPointF(
            std::max(0.0F, static_cast<float>(boundingRectangle.width() - messageWidth) / 2.0F),
            std::max(0.0F, static_cast<float>(boundingRectangle.height() - messageHeight) / 2.0F) + messageAscent
        );
    } else {
        textLocation = QPointF(0.0, messageAscent);
    }

    if (group->numberTextEntries() == 0) {
        group->append(errorMessage, messageFont, textLocation);
    } else {
        group->entry(0).setText(errorMessage);
    }
}


void ImageRenderPresentationData::clearErrorMessage() {
    EQt::GraphicsMultiTextGroup* group = dynamic_cast<EQt::GraphicsMultiTextGroup*>(graphicsItem());
    group->clearText();
    group->setBorderPen(currentBorderPen);
}


void ImageRenderPresentationData::setSeries(
        unsigned                                  seriesIndex,
        unsigned                                  valueIndex,
        ImageRenderPresentationData::AxisLocation axisLocation
    ) {
    if (static_cast<unsigned>(seriesDataByValueIndex.size()) <= valueIndex) {
        while (static_cast<unsigned>(seriesDataByValueIndex.size()) < valueIndex) {
            seriesDataByValueIndex.append(SeriesData());
        }

        seriesDataByValueIndex.append(SeriesData(axisLocation, seriesIndex, valueIndex));
    } else {
        seriesDataByValueIndex[valueIndex] = SeriesData(axisLocation, seriesIndex, valueIndex);
    }
}


void ImageRenderPresentationData::performUpdate() {
    unsigned long long expectedNumberRows    = static_cast<unsigned long long>(-1);
    unsigned long long expectedNumberColumns = static_cast<unsigned long long>(-1);
    unsigned           numberValues          = static_cast<unsigned>(seriesDataByValueIndex.size());
    bool               hasNoneValue          = false;
    bool               hasDecodableValue     = false;

    QMap<AxisLocation, Model::MatrixInteger> integerMatrices;
    QMap<AxisLocation, Model::MatrixReal>    realMatrices;

    QString  errorString;
    unsigned index = 0;

    while (errorString.isEmpty() && index < numberValues) {
        const SeriesData&          seriesData      = seriesDataByValueIndex.at(index);
        AxisLocation               axisLocation    = seriesData.axisLocation();
        const Ld::CalculatedValue& calculatedValue = seriesData.calculatedValue();
        Model::Variant             variant         = calculatedValue.variant();
        Model::ValueType           valueType       = variant.valueType();

        if (integerMatrices.contains(axisLocation) || realMatrices.contains(axisLocation)) {
            errorString = tr("Value for color defined multiple times.");
        } else {
            unsigned numberRows    = 0;
            unsigned numberColumns = 0;
            switch (valueType) {
                case Model::ValueType::NONE: {
                    hasNoneValue = true;
                    errorString  = tr("Unknown data type in image plot.");
                    break;
                }

                case Model::ValueType::BOOLEAN: {
                    hasDecodableValue = true;
                    errorString       = tr("Can-not generate image from boolean value.");
                    break;
                }

                case Model::ValueType::INTEGER: {
                    hasDecodableValue = true;
                    errorString       = tr("Can-not generate image from integer value.");
                    break;
                }

                case Model::ValueType::REAL: {
                    hasDecodableValue = true;
                    errorString       = tr("Can-not generate image from real value.");
                    break;
                }

                case Model::ValueType::COMPLEX: {
                    hasDecodableValue = true;
                    errorString       = tr("Can-not generate image from complex value.");
                    break;
                }

                case Model::ValueType::SET: {
                    hasDecodableValue = true;
                    errorString       = tr("Can-not generate image from set.");
                    break;
                }

                case Model::ValueType::TUPLE: {
                    hasDecodableValue = true;
                    errorString       = tr("Can-not generate image from tuple.");
                    break;
                }

                case Model::ValueType::MATRIX_BOOLEAN:
                case Model::ValueType::MATRIX_INTEGER: {
                    hasDecodableValue = true;

                    bool                 ok;
                    Model::MatrixInteger integerMatrix = variant.toMatrixInteger(&ok);
                    Q_ASSERT(ok);

                    numberRows    = integerMatrix.numberRows();
                    numberColumns = integerMatrix.numberColumns();

                    integerMatrices.insert(axisLocation, integerMatrix);
                    break;
                }


                case Model::ValueType::MATRIX_REAL: {
                    hasDecodableValue = true;

                    bool              ok;
                    Model::MatrixReal realMatrix = variant.toMatrixReal(&ok);
                    Q_ASSERT(ok);

                    numberRows    = realMatrix.numberRows();
                    numberColumns = realMatrix.numberColumns();

                    realMatrices.insert(axisLocation, realMatrix);
                    break;
                }

                case Model::ValueType::MATRIX_COMPLEX: {
                    hasDecodableValue = true;
                    errorString       = tr("Can-not generate image from complex matrices.");
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            if (expectedNumberColumns == static_cast<unsigned long long>(-1)) {
                expectedNumberRows    = numberRows;
                expectedNumberColumns = numberColumns;
            } else if (expectedNumberRows != numberRows || expectedNumberColumns != numberColumns) {
                errorString = tr("Matrices must be the same size and type.");
            }
        }

        ++index;
    }

    QImage image;
    if (errorString.isEmpty()) {
        if (static_cast<unsigned>(integerMatrices.size()) != 0) {
            if (static_cast<unsigned>(realMatrices.size()) != 0) {
                errorString = tr("Matrices must be the same type.");
            } else if (static_cast<unsigned>(integerMatrices.size()) != numberValues) {
                errorString = tr("Incorrect number of matrices.");
            } else {
                image = render(integerMatrices, errorString);
            }
        } else {
            if (static_cast<unsigned>(realMatrices.size()) != numberValues) {
                errorString = tr("Incorrect number of matrices.");
            } else {
                image = render(realMatrices, errorString);
            }
        }
    }

    if (errorString.isEmpty()) {
        QSizeF requiredSizeInSceneUnits = toScene(currentSizeInPoints);
        QSizeF currentSizeInSceneUnits  = image.size();
        double widthScaleFactor         = requiredSizeInSceneUnits.width() / currentSizeInSceneUnits.width();
        double heightScaleFactor        = requiredSizeInSceneUnits.height() / currentSizeInSceneUnits.height();

        QTransform transform(
            widthScaleFactor, 0,                 0,
            0,                heightScaleFactor, 0,
            0,                0,                 1
        );

        QImage scaledImage = image.transformed(transform, Qt::TransformationMode::SmoothTransformation);
        currentPixmapItem->setPixmap(QPixmap::fromImage(scaledImage));
        clearErrorMessage();
    } else {
        if (!hasDecodableValue && hasNoneValue) {
            drawDefaultImage();
        } else {
            showErrorMessage(errorString);
        }
    }
}


void ImageRenderPresentationData::drawDefaultImage() {
    QPixmap defaultPixmap(100, 100);
    QSizeF  pixelsPerPoint(
        100 / (defaultPixmap.widthMM() * Util::Units::pointsPerMm),
        100 / (defaultPixmap.heightMM() * Util::Units::pointsPerMm)
    );
    QSizeF realPixmapSize = toScene(currentSizeInPoints);
    QSize pixmapSize(
        static_cast<unsigned>(0.5 + realPixmapSize.width()),
        static_cast<unsigned>(0.5 + realPixmapSize.height())
    );
    QSize imageSize = currentDefaultPixmap.size();

    if (imageSize.width() <= pixmapSize.width() && imageSize.height() <= pixmapSize.height()) {
        QPixmap  pixmap(pixmapSize);
        pixmap.fill(QColor(Qt::GlobalColor::white));

        QPainter painter(&pixmap);

        QPoint imageTopLeft(
            (pixmapSize.width() - imageSize.width()) / 2,
            (pixmapSize.height() - imageSize.height()) / 2
        );
        painter.drawPixmap(imageTopLeft, currentDefaultPixmap);
        painter.end();

        currentPixmapItem->resetTransform();
        currentPixmapItem->setPixmap(pixmap);
    } else {
        QPixmap pixmap = currentDefaultPixmap.scaled(pixmapSize, Qt::AspectRatioMode::KeepAspectRatio);
        currentPixmapItem->resetTransform();
        currentPixmapItem->setPixmap(pixmap);
    }
}
