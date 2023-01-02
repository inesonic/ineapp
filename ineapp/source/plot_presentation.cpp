/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotPresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>

#include <eqt_graphics_item.h>

#include <ld_element_structures.h>
#include <ld_plot_element.h>
#include <ld_plot_visual.h>
#include <ld_plot_format.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "plot_engine.h"
#include "plot_presentation_data.h"
#include "special_symbol_presentation_base.h"
#include "plot_presentation.h"

PlotPresentation::PlotPresentation() {
    currentPlotEngine       = Q_NULLPTR;
    currentPresentationData = Q_NULLPTR;
}


PlotPresentation::~PlotPresentation() {
    clearPresentationData();
}


Ld::Visual* PlotPresentation::creator(const QString&) {
    return new PlotPresentation();
}


void PlotPresentation::recalculatePlacement(
        PlacementTracker*    /* placementTracker */,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                minimumTopSpacing,
        float                lineSpacing,
        float                relativeScale
    ) {
    QSharedPointer<Ld::PlotElement> plotElement = element().dynamicCast<Ld::PlotElement>();
    QSharedPointer<Ld::PlotFormat>  plotFormat  = plotElement->format().dynamicCast<Ld::PlotFormat>();

    if (currentPlotEngine == Q_NULLPTR || plotFormat->chartType() != currentPlotEngine->name()) {
        currentPlotEngine = PlotEngine::byName(plotFormat->chartType());
        Q_ASSERT(currentPlotEngine != Q_NULLPTR);

        clearPresentationData();
    }

    if (currentPresentationData == Q_NULLPTR) {
        currentPresentationData = currentPlotEngine->createPresentationData(plotElement, plotFormat);
    } else {
        PlotPresentationData* updatedPresentationData = currentPlotEngine->updatePresentationData(
            currentPresentationData,
            plotElement,
            plotFormat
        );

        if (updatedPresentationData != currentPresentationData) {
            clearPresentationData();
            currentPresentationData = updatedPresentationData;
        }
    }

    redoPlacement(
        parent,
        childIdentifier,
        0,
        minimumTopSpacing,
        lineSpacing,
        relativeScale
    );
}


void PlotPresentation::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        /* firstPresentationAreaId */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    QSizeF     plotSize          = currentPresentationData->boundingRectangle().size();
    QSizeF     adjustedImageSize = plotSize;
    bool       fits              = false;

    do {
        SpaceQualifier spaceQualifier;
        QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if ((availableSpace.width() >= plotSize.width()   &&
             availableSpace.height() >= plotSize.height()    ) ||
            spaceQualifier == SpaceQualifier::USE_PROVIDED                      ) {
            fits = true;
        } else if (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
                   spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH               ) {
            float xScaleFactor = availableSpace.width() / plotSize.width();
            float yScaleFactor = availableSpace.height() / plotSize.height();
            float bestScale    = std::min(xScaleFactor, yScaleFactor);

            if (bestScale < 1.0F) {
                QTransform transform;
                transform.scale(bestScale, bestScale);
                adjustedImageSize *= bestScale;

                currentPresentationData->graphicsItem()->setTransform(transform);
            }

            fits = true;
        } else {
            fits = false;
        }

        if (!fits) {
            parent->areaInsufficient(childIdentifier, availableSpace);
        } else {
            parent->allocateArea(childIdentifier, 0, adjustedImageSize);
        }
    } while (!fits);
}


void PlotPresentation::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We don't support stretch with plots.
}


QGraphicsItem* PlotPresentation::graphicsItem(unsigned long presentationAreaId) const {
    QGraphicsItem* result;
    if (presentationAreaId == 0 && currentPresentationData != Q_NULLPTR) {
        result = currentPresentationData->graphicsItem();
    } else {
        result = Q_NULLPTR;
    }

    return result;
}


void PlotPresentation::resetPlacement() {
    clearPresentationData();
}


void PlotPresentation::removeFromScene() {
    clearPresentationData();
}


QList<QRectF> PlotPresentation::cursorRangeToScene(
        const Ld::ElementCursor& /* endingElementCursor */,
        const Ld::ElementCursor& /* startingElementCursor */
    ) const {
    QList<QRectF> boundingRectangles;

    if (currentPresentationData != Q_NULLPTR && currentPresentationData->graphicsItem() != Q_NULLPTR) {
        boundingRectangles << currentPresentationData->sceneBoundingRectangle();
    }

    return boundingRectangles;
}



PlotPresentation::ReflowHint PlotPresentation::reflowHint() const {
    return ReflowHint::REFLOW_NOT_SUPPORTED;
}


void PlotPresentation::processCalculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue) {
    if (currentPresentationData != Q_NULLPTR) {
        currentPresentationData->calculatedValueUpdated(valueIndex, calculatedValue);
    }
}


void PlotPresentation::processCalculatedValuesCleared() {
    if (currentPresentationData != Q_NULLPTR) {
        currentPresentationData->calculatedValuesCleared();
    }
}


void PlotPresentation::clearPresentationData() {
    if (currentPresentationData != Q_NULLPTR) {
        delete currentPresentationData;
        currentPresentationData = Q_NULLPTR;
    }
}
