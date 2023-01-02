/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageBreakPresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QString>
#include <QSizeF>
#include <QSize>
#include <QRectF>
#include <QPointF>

#include <eqt_graphics_rect_item.h>

#include <ld_page_break_element.h>
#include <ld_page_break_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "leaf_presentation.h"
#include "page_break_presentation.h"

PageBreakPresentation::PageBreakPresentation() {
    currentGraphicsItem = Q_NULLPTR;
}


PageBreakPresentation::~PageBreakPresentation() {
    clearGraphicsItem();
}


Ld::Visual* PageBreakPresentation::creator(const QString&) {
    return new PageBreakPresentation();
}


void PageBreakPresentation::recalculatePlacement(
        PlacementTracker*    /* placementTracker */,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    clearGraphicsItem();

    QSizeF         currentActiveAreaSize;
    SpaceQualifier spaceQualifier;

    do {
        currentActiveAreaSize = parent->requestArea(childIdentifier, &spaceQualifier);

        if (spaceQualifier == SpaceQualifier::CURRENT_AVAILABLE) {
            parent->areaInsufficient(childIdentifier, currentActiveAreaSize);
        }
    } while (spaceQualifier == SpaceQualifier::CURRENT_AVAILABLE);

    currentGraphicsItem = new EQt::GraphicsRectItem();
    currentGraphicsItem->setBrush(QBrush(Qt::NoBrush));
    currentGraphicsItem->setPen(QPen(Qt::NoPen));
    currentGraphicsItem->setRect(QRectF(QPointF(0, 0), currentActiveAreaSize));

    parent->allocateArea(childIdentifier, 0, currentActiveAreaSize);
}


void PageBreakPresentation::redoPlacement(
        PlacementNegotiator*,
        unsigned long,
        unsigned long,
        float,
        float,
        float
    ) {
    // We always request full width regions so this method should never be called.
    Q_ASSERT(false);
}


void PageBreakPresentation::applyStretch(unsigned long, float) {
    // Stretch is flagged not allowed for this item when we allocate an area so we should never see this method called.
    Q_ASSERT(false);
}


QGraphicsItem* PageBreakPresentation::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? currentGraphicsItem : Q_NULLPTR;
}


void PageBreakPresentation::resetPlacement() {
    PresentationWithNoChildren::resetPlacement();
    clearGraphicsItem();
}


void PageBreakPresentation::removeFromScene() {
    clearGraphicsItem();
}


QList<QRectF> PageBreakPresentation::cursorRangeToScene(
        const Ld::ElementCursor& endingElementCursor,
        const Ld::ElementCursor&
    ) const {
    Q_ASSERT(endingElementCursor.isValid());
    QList<QRectF> boundingRectangles;

    if (currentGraphicsItem != Q_NULLPTR) {
        boundingRectangles << currentGraphicsItem->sceneBoundingRect();
    }

    return boundingRectangles;
}


void PageBreakPresentation::clearGraphicsItem() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }
}
