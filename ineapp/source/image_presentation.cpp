/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImagePresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QSizeF>
#include <QSize>
#include <QRectF>
#include <QPointF>
#include <QPixmap>
#include <QSvgRenderer>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QTransform>

#include <algorithm>
#include <cmath>

#include <eqt_graphics_pixmap_item.h>
#include <eqt_graphics_svg_item.h>

#include <util_units.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element_cursor.h>
#include <ld_root_element.h>
#include <ld_image_element.h>
#include <ld_image_format.h>
#include <ld_image_visual.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "leaf_presentation.h"
#include "image_presentation.h"

const QSizeF ImagePresentation::errorImageSizePoints(144.0F, 72.0F);
const QSizeF ImagePresentation::defaultDotsPerPoint(96.0F / 72.0F, 96.0F / 72.0F);

ImagePresentation::ImagePresentation() {
    connect(this, SIGNAL(payloadWasChanged()), SLOT(processDataChange()), connectionType);
    connect(this, SIGNAL(payloadFailedToLoad()), SLOT(reportPayloadCouldNotBeLoaded()), connectionType);

    currentGraphicsItem        = Q_NULLPTR;
    currentPrescaledResolution = QSizeF();
    currentImageValid          = false;
}


ImagePresentation::~ImagePresentation() {
    clearGraphicsItem();
}


Ld::Visual* ImagePresentation::creator(const QString&) {
    return new ImagePresentation;
}


void ImagePresentation::recalculatePlacement(
        PlacementTracker*    /* placementTracker */,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    if (currentGraphicsItem == Q_NULLPTR) {
        currentImageValid = loadAndAdjustImage();
    }

    redoPlacement(parent, childIdentifier, 0);
}


void ImagePresentation::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        /* firstPresentationAreaId */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    QSizeF     adjustedImageSize = currentTransformedSize;
    QTransform transform         = currentTransform;
    bool       fits              = false;

    do {
        SpaceQualifier spaceQualifier;
        QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if ((availableSpace.width() >= currentTransformedSize.width()   &&
             availableSpace.height() >= currentTransformedSize.height()    ) ||
            spaceQualifier == SpaceQualifier::USE_PROVIDED                      ) {
            fits = true;
        } else if (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
                   spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH               ) {
            float xScaleFactor = availableSpace.width() / currentTransformedSize.width();
            float yScaleFactor = availableSpace.height() / currentTransformedSize.height();

            float bestScale    = std::min(xScaleFactor, yScaleFactor);

            if (bestScale < 1.0F) {
                transform.scale(bestScale, bestScale);
                adjustedImageSize *= bestScale;
            }

            fits = true;
        } else {
            fits = false;
        }

        if (!fits) {
            parent->areaInsufficient(childIdentifier, availableSpace);
        } else {
            dynamic_cast<QGraphicsItem*>(currentGraphicsItem)->setTransform(transform);
            parent->allocateArea(childIdentifier, 0, adjustedImageSize);
        }
    } while (!fits);
}


void ImagePresentation::applyStretch(unsigned long, float) {}


QGraphicsItem* ImagePresentation::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? dynamic_cast<QGraphicsItem*>(currentGraphicsItem) : Q_NULLPTR;
}


void ImagePresentation::resetPlacement() {
    PresentationWithNoChildren::resetPlacement();
    clearGraphicsItem();
}


void ImagePresentation::removeFromScene() {
    clearGraphicsItem();
}


QList<QRectF> ImagePresentation::cursorRangeToScene(const Ld::ElementCursor&, const Ld::ElementCursor&) const {
    QList<QRectF> boundingRectangles;

    if (currentGraphicsItem != Q_NULLPTR) {
        boundingRectangles << dynamic_cast<QGraphicsItem*>(currentGraphicsItem)->sceneBoundingRect();
    }

    return boundingRectangles;
}


ImagePresentation::ReflowHint ImagePresentation::reflowHint() const {
    return ReflowHint::ALWAYS_REFLOW;
}


QSizeF ImagePresentation::prescaledDotsPerPoint() const {
    return currentPrescaledResolution;
}


QSizeF ImagePresentation::prescaledImageSizePoints() {
    QSizeF result;

    if (currentGraphicsItem == Q_NULLPTR) {
        currentImageValid = loadAndAdjustImage();
    }

    if (currentImageValid) {
        QSizeF scalingFactor =   currentPrescaledResolution.isNull()
                               ? defaultDotsPerPoint
                               : currentPrescaledResolution;

        result = QSizeF(
            currentPrescaledSizePixels.width() / scalingFactor.width(),
            currentPrescaledSizePixels.height() / scalingFactor.height()
        );
    }

    return result;
}


QSizeF ImagePresentation::displayedImageSizePoints() {
    QSizeF result;

    if (currentGraphicsItem == Q_NULLPTR) {
        currentImageValid = loadAndAdjustImage();
    }

    if (currentImageValid) {
        result = fromScene(currentTransformedSize);
    }

    return result;
}


void ImagePresentation::processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    clearGraphicsItem();
    LeafPresentation::processFormatChange(oldFormat, newFormat);
}


void ImagePresentation::processDataChange() {}


void ImagePresentation::processPayloadChange() {
    clearGraphicsItem();
}


void ImagePresentation::reportPayloadCouldNotBeLoaded() {}


void ImagePresentation::payloadChanged() {
    emit payloadWasChanged();
}


void ImagePresentation::payloadCouldNotBeLoaded() {
    emit payloadFailedToLoad();
}


bool ImagePresentation::createGraphicsItem(
        const QByteArray&            imageData,
        const Ld::ImageFormat::Axis& horizontalAxisScaling,
        const Ld::ImageFormat::Axis& verticalAxisScaling,
        Ld::ImageFormat::Rotation    rotation
    ) {
    bool    validImage;
    QString errorString;

    QPixmap unrotated;

    currentPrescaledResolution = QSizeF();
    currentPrescaledSizePixels = QSize();

    if (imageData.isEmpty()) {
        errorString = tr("No image data.");
    } else if (imageData.startsWith("<?xml") && imageData.contains("<svg")) {
        QSvgRenderer renderer(imageData);

        if (renderer.animated()) {
            errorString = tr("Animated .SVG files not supported.");
        } else if (!renderer.isValid()) {
            errorString = tr("Invalid .SVG contents.");
        } else {
            unrotated = QPixmap(renderer.defaultSize());
            QPainter painter(&unrotated);
            renderer.render(&painter);
        }
    } else {
        QImage rawImage;
        validImage = rawImage.loadFromData(imageData);
        if (!validImage) {
            Ld::ElementPointer rawRoot = element()->root();
            if (!rawRoot.isNull() && rawRoot->typeName() == Ld::RootElement::elementName) {
                QSharedPointer<Ld::RootElement> root = rawRoot.dynamicCast<Ld::RootElement>();
                errorString = root->errorString();
            } else {
                errorString = tr("Could not load image.");
            }

            unrotated = QPixmap();
        } else {
            currentPrescaledResolution = QSizeF(
                rawImage.dotsPerMeterX() * Util::Units::mmPerPoint / 1000.0,
                rawImage.dotsPerMeterY() * Util::Units::mmPerPoint / 1000.0
            );

            unrotated = QPixmap::fromImage(rawImage);
        }
    }

    if (unrotated.isNull()) {
        QSizeF   errorPixmapSizeSceneUnits = toScene(errorImageSizePoints);
        unrotated = QPixmap(
            static_cast<int>(errorPixmapSizeSceneUnits.width() + 0.5),
            static_cast<int>(errorPixmapSizeSceneUnits.height() + 0.5)
        );
        QPainter painter(&unrotated);

        QRectF boundingRect(QPointF(0, 0), QSize(unrotated.size().width() - 1, unrotated.size().height() - 1));

        painter.setPen(QPen(QColor(Qt::red)));
        painter.setBrush(QBrush(QColor(Qt::white)));
        painter.drawRect(boundingRect);

        painter.drawText(boundingRect, Qt::AlignCenter | Qt::AlignVCenter, tr("Error\n%1").arg(errorString));

        validImage = false;
    } else {
        validImage = true;
    }

    QPixmap rotated;
    if (validImage && rotation != Ld::ImageFormat::Rotation::NO_ROTATION) {
        QTransform transform;
        switch (rotation) {
            case Ld::ImageFormat::Rotation::ROTATE_CCW_90: {
                currentPrescaledResolution.transpose();
                transform = QTransform(0.0F, -1.0F, 1.0F, 0.0F, 0.0F, 0.0F);
                break;
            }

            case Ld::ImageFormat::Rotation::ROTATE_CW_90: {
                currentPrescaledResolution.transpose();

                transform = QTransform(0.0F, 1.0F, -1.0F, 0.0F, 0.0F, 0.0F);
                break;
            }

            case Ld::ImageFormat::Rotation::FLIP: {
                transform = QTransform(-1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        rotated = unrotated.transformed(transform);
    } else {
        rotated = unrotated;
    }

    EQt::GraphicsPixmapItem* graphicsPixmapItem = new EQt::GraphicsPixmapItem;
    graphicsPixmapItem->setPixmap(rotated);

    if (validImage) {
        Ld::ImageFormat::ImageScalingMode horizontalScalingMode = horizontalAxisScaling.scalingMode();
        Ld::ImageFormat::ImageScalingMode verticalScalingMode   = verticalAxisScaling.scalingMode();

        currentPrescaledSizePixels = rotated.size();
        QSizeF rawImageSizePoints = fromScene(currentPrescaledSizePixels);

        QSizeF scalingFactor =   currentPrescaledResolution.isNull()
                               ? defaultDotsPerPoint
                               : currentPrescaledResolution;

        QSizeF prescaledSizePoints(
            currentPrescaledSizePixels.width() / scalingFactor.width(),
            currentPrescaledSizePixels.height() / scalingFactor.height()
        );

        float xScaling;
        float yScaling;
        if (horizontalScalingMode == Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO) {
            Q_ASSERT(verticalScalingMode != Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO);

            yScaling = calculateScaleFactor(
                verticalAxisScaling,
                rawImageSizePoints.height(),
                prescaledSizePoints.height()
            );
            xScaling = calculateScaleFactor(
                horizontalAxisScaling,
                rawImageSizePoints.width(),
                prescaledSizePoints.width(),
                yScaling
            );
        } else {
            xScaling = calculateScaleFactor(
                horizontalAxisScaling,
                rawImageSizePoints.width(),
                prescaledSizePoints.width()
            );
            yScaling = calculateScaleFactor(
                verticalAxisScaling,
                rawImageSizePoints.height(),
                prescaledSizePoints.height(),
                xScaling
            );
        }

        currentTransformedSize = QSizeF(
            currentPrescaledSizePixels.width() * xScaling,
            currentPrescaledSizePixels.height() * yScaling
        );

        currentTransform.setMatrix(xScaling, 0.0F, 0.0F, 0.0F, yScaling, 0.0F, 0.0F, 0.0F, 1.0F);
    } else {
        currentTransform.reset();
        currentTransformedSize = graphicsPixmapItem->boundingRect().size();
    }

    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater(); // FIXME
    }

    currentGraphicsItem = graphicsPixmapItem;

    return validImage;
}


float ImagePresentation::calculateScaleFactor(
        const Ld::ImageFormat::Axis& axisScaling,
        float                        sizePoints,
        float                        prescaledSize,
        float                        otherScaleFactor
    ) {
    float scaleFactor;

    switch (axisScaling.scalingMode()) {
        case Ld::ImageFormat::ImageScalingMode::IGNORED: {
            scaleFactor = prescaledSize / sizePoints;
            break;
        }

        case Ld::ImageFormat::ImageScalingMode::FIXED_POINTS: {
            scaleFactor = axisScaling.value() / sizePoints;
            break;
        }

        case Ld::ImageFormat::ImageScalingMode::FRACTIONAL: {
            scaleFactor = prescaledSize * axisScaling.value() / sizePoints;
            break;
        }

        case Ld::ImageFormat::ImageScalingMode::ASPECT_RATIO: {
            Q_ASSERT(otherScaleFactor != 0.0F);
            scaleFactor = otherScaleFactor;
            break;
        }

        default: {
            Q_ASSERT(false);
            scaleFactor = 1.0F;
            break;
        }
    }

    return scaleFactor;
}


bool ImagePresentation::loadAndAdjustImage() {
    QSharedPointer<Ld::ImageElement> imageElement = element();
    Q_ASSERT(!imageElement.isNull());

    QByteArray imageData;
    bool success = imageElement->getPayload(imageData);

    QSharedPointer<Ld::ImageFormat> imageFormat = imageElement->format().dynamicCast<Ld::ImageFormat>();
    if (success) {
        Ld::ImageFormat::Axis     horizontalAxis;
        Ld::ImageFormat::Axis     verticalAxis;
        Ld::ImageFormat::Rotation rotation;

        if (imageFormat.isNull()) {
            horizontalAxis = Ld::ImageFormat::Axis(0.0F, Ld::ImageFormat::ImageScalingMode::IGNORED);
            verticalAxis   = Ld::ImageFormat::Axis(0.0F, Ld::ImageFormat::ImageScalingMode::IGNORED);
            rotation       = Ld::ImageFormat::Rotation::NO_ROTATION;
        } else {
            horizontalAxis = imageFormat->horizontalAxis();
            verticalAxis   = imageFormat->verticalAxis();
            rotation       = imageFormat->rotation();
        }

        currentImageValid = createGraphicsItem(imageData, horizontalAxis, verticalAxis, rotation);
    } else {
        createGraphicsItem(
            QByteArray(),
            Ld::ImageFormat::Axis(0.0F, Ld::ImageFormat::ImageScalingMode::IGNORED),
            Ld::ImageFormat::Axis(0.0F, Ld::ImageFormat::ImageScalingMode::IGNORED),
            Ld::ImageFormat::Rotation::NO_ROTATION
        );
    }

    return success;
}


void ImagePresentation::clearGraphicsItem() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }
}
