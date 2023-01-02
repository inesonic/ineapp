/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref FramePresentationBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QGraphicsItem>
#include <QtAlgorithms>

#include <eqt_graphics_item_group.h>

#include <ld_element_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>

#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "presentation.h"
#include "presentation_with_positional_children.h"
#include "frame_presentation_base.h"

FramePresentationBase::FramePresentationBase() {}


FramePresentationBase::~FramePresentationBase() {
    trimExtraGraphicsItems(0);
}


Ld::Element::ChildPlacement FramePresentationBase::childPlacement() const {
    return Ld::Element::ChildPlacement::POSITIONAL;
}


void FramePresentationBase::requestRepositioning(Presentation*) {
    Ld::ElementPointer parentElement = element()->parent();
    if (!parentElement.isNull()) {
        PlacementNegotiator* parentPlacementNegotiator = dynamic_cast<PlacementNegotiator*>(parentElement->visual());

        if (parentPlacementNegotiator != Q_NULLPTR) {
            parentPlacementNegotiator->requestRepositioning(this);
        }
    }
}


void FramePresentationBase::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    currentParentNegotiator = parent;
    currentChildIdentifier  = childIdentifier;

    Ld::ElementPointer thisElement = element();

    unsigned long numberChildren    = thisElement->numberChildren();
    unsigned long currentChildIndex = 0;
    float         minimumTopSpacing = imposeNoTopSpacing;

    requestNewActiveAreaRectangle(true);
    cursorY = currentActiveArea.top() + topSpacingSceneUnits();

    placementTracker->addNewJobs(numberChildren);

    while (currentChildIndex < numberChildren) {
        Ld::ElementPointer childElement = thisElement->child(currentChildIndex);
        Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());

        if (childPresentation != Q_NULLPTR) {
            if (cursorY == currentActiveArea.top() || currentChildIndex == 0) {
                minimumTopSpacing = imposeNoTopSpacing;
            }

            childPresentation->recalculatePlacement(
                placementTracker,
                this,
                currentChildIndex,
                Q_NULLPTR, // nextSibling -- currently unused
                false,     // alwaysFit
                true,      // honorLeadingWhitespace
                minimumTopSpacing
            );

            minimumTopSpacing = childPresentation->bottomSpacingSceneUnits();
        }

        placementTracker->completedJob();
        ++currentChildIndex;
    }

    currentActiveArea.setBottom(cursorY);
    allocateLastActiveArea();

    trimExtraGraphicsItems(currentActiveAreaId + 1);
}


void FramePresentationBase::redoPlacement(PlacementNegotiator*, unsigned long, unsigned long, float, float, float) {
    // We always request full lines so this method should never be called.
    Q_ASSERT(false);
}


QSizeF FramePresentationBase::requestArea(unsigned long, FramePresentationBase::SpaceQualifier* spaceQualifier) {
    if (spaceQualifier != Q_NULLPTR) {
        *spaceQualifier = currentSpaceQualifier;
    }

    if (currentSpaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT) {
        currentSpaceQualifier = SpaceQualifier::MAXIMUM_WIDTH;
    }

    return QSizeF(currentActiveArea.width(), currentActiveArea.bottom() - cursorY);
}


void FramePresentationBase::allocateArea(
        unsigned long childIdentifier,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         /* ascent */,
        bool          /* canStretch */
    ) {
    Ld::ElementPointer childElement      = element()->child(childIdentifier);
    Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    QGraphicsItem*     graphicsItem      = childPresentation->graphicsItem(presentationAreaId);

    EQt::GraphicsItemGroup* itemGroup = currentGraphicsItemGroups[currentActiveAreaId];
    itemGroup->addToGroup(graphicsItem);
    graphicsItem->setPos(currentActiveArea.left(), cursorY);

    areaInsufficient(childIdentifier, size);
}


void FramePresentationBase::areaInsufficient(unsigned long, const QSizeF& size) {
    cursorY += size.height();

    if (cursorY >= currentActiveArea.bottom()) {
        allocateLastActiveArea();
        requestNewActiveAreaRectangle();
        cursorY = currentActiveArea.top();
    }
}


void FramePresentationBase::applyStretch(unsigned long, float) {
    // We don't flag that we support stretch so this method should never be called.
    Q_ASSERT(false);
}


QGraphicsItem* FramePresentationBase::graphicsItem(unsigned long presentationAreaId) const {
    QGraphicsItem* graphicsItem;
    if (presentationAreaId < static_cast<unsigned long>(currentGraphicsItemGroups.size())) {
        graphicsItem = currentGraphicsItemGroups.at(presentationAreaId);
    } else {
        graphicsItem = Q_NULLPTR;
    }

    return graphicsItem;
}


void FramePresentationBase::resetPlacement() {}


void FramePresentationBase::removeFromScene() {
    currentActiveAreaId = 0;
    trimExtraGraphicsItems(0);
}


void FramePresentationBase::processDataChange() {}


void FramePresentationBase::processDiagnostic(Ld::DiagnosticPointer /* diagnostic */) {}


void FramePresentationBase::clearDiagnosticDisplay() {}


void FramePresentationBase::processRemovingChildPresentation(unsigned long, Presentation*) {
    if (!element().isNull()) {
        requestRepositioning(Q_NULLPTR);
    }
}


void FramePresentationBase::processChildPresentationInsertedBefore(unsigned long, Presentation* childPresentation) {
    requestRepositioning(childPresentation);
}


void FramePresentationBase::processChildPresentationInsertedAfter(unsigned long, Presentation* childPresentation) {
    requestRepositioning(childPresentation);
}


void FramePresentationBase::requestNewActiveAreaRectangle(bool isFirst) {
    Q_ASSERT(currentParentNegotiator != Q_NULLPTR);
    Q_ASSERT(currentChildIdentifier != invalidChildIdentifier);

    QSizeF areaSize;
    do {
        areaSize = currentParentNegotiator->requestArea(currentChildIdentifier, &currentSpaceQualifier);

        if (currentSpaceQualifier == SpaceQualifier::CURRENT_AVAILABLE) {
            currentParentNegotiator->areaInsufficient(currentChildIdentifier, areaSize);
        }
    } while (currentSpaceQualifier == SpaceQualifier::CURRENT_AVAILABLE);

    float currentLeftMarginSceneUnits  = leftMarginSceneUnits();
    float currentRightMarginSceneUnits = rightMarginSceneUnits();

    areaSize.setWidth(areaSize.width() - currentLeftMarginSceneUnits - currentRightMarginSceneUnits);
    currentActiveArea = QRectF(QPointF(currentLeftMarginSceneUnits, 0), areaSize);

    if (isFirst) {
        currentActiveAreaId = 0;
    } else {
        ++currentActiveAreaId;
    }

    if (currentActiveAreaId >= static_cast<unsigned>(currentGraphicsItemGroups.size())) {
        Q_ASSERT(currentActiveAreaId >= static_cast<unsigned>(currentGraphicsItemGroups.size()));
        currentGraphicsItemGroups.append(new EQt::GraphicsItemGroup);
    }
}


void FramePresentationBase::allocateLastActiveArea() {
    currentParentNegotiator->allocateArea(currentChildIdentifier, currentActiveAreaId, currentActiveArea.size());
}


void FramePresentationBase::trimExtraGraphicsItems(unsigned numberActiveAreas) {
    unsigned numberItemGroups = static_cast<unsigned>(currentGraphicsItemGroups.size());

    if (numberItemGroups > numberActiveAreas) {
        unsigned long numberGroups = static_cast<unsigned long>(currentGraphicsItemGroups.size());
        for (unsigned i=numberActiveAreas ; i<numberGroups ; ++i) {
            EQt::GraphicsItemGroup* group = currentGraphicsItemGroups.at(i);
            group->deleteLater();
        }

        currentGraphicsItemGroups.erase(
            currentGraphicsItemGroups.begin() + numberActiveAreas,
            currentGraphicsItemGroups.end()
        );
    }
}
