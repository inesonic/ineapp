/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref ParagraphPresentationBase class.
***********************************************************************************************************************/

#include <QString>
#include <QSharedPointer>
#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QFont>
#include <QColor>
#include <QBrush>
#include <QFontMetricsF>

#include <algorithm>

#include <eqt_graphics_item_group.h>
#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual.h>
#include <ld_element_with_positional_children.h>
#include <ld_paragraph_element.h>
#include <ld_visual_with_positional_children.h>
#include <ld_paragraph_format.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "placement_line_start.h"
#include "presentation.h"
#include "presentation_with_positional_children.h"
#include "paragraph_presentation_base.h"

ParagraphPresentationBase::ParagraphPresentationBase() {
    currentPendingRepositioning = false;
    currentReflowHint           = ReflowHint::REFLOW_NOT_SUPPORTED;
    currentParentNegotiator     = Q_NULLPTR;
    currentChildIdentifier      = invalidChildIdentifier;
    currentListTextItem         = Q_NULLPTR;
}


ParagraphPresentationBase::~ParagraphPresentationBase() {
    truncateActiveAreasStartingAt(0);
}


void ParagraphPresentationBase::requestRepositioning(Presentation* childPresentation) {
    Ld::ElementPointer   parentElement = element()->parent();
    PlacementNegotiator* parentNegotiator = dynamic_cast<PlacementNegotiator*>(parentElement->visual());
    if (parentNegotiator != Q_NULLPTR) {
        currentPendingRepositioning = true;

        ReflowHint childReflowHint = childPresentation->reflowHint();
        if (childReflowHint > currentReflowHint) {
            currentReflowHint = childReflowHint;
        }

        parentNegotiator->requestRepositioning(this);
    }
}


void ParagraphPresentationBase::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadngWhitespace */,
        float                minimumTopSpacing,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    currentPendingRepositioning = false;
    currentParentNegotiator     = parent;
    currentChildIdentifier      = childIdentifier;

    QSharedPointer<Ld::ElementWithPositionalChildren> thisElement = element()
                                                                    .dynamicCast<Ld::ElementWithPositionalChildren>();

    clearLineData();

    if (currentListTextItem != Q_NULLPTR) {
        currentListTextItem->deleteLater();
        currentListTextItem = Q_NULLPTR;
    }

    QFont   listFont;
    QColor  listFontColor;
    QColor  listFontBackgroundColor;
    QString listText = listString(&listFont, &listFontColor, &listFontBackgroundColor);
    if (!listText.isEmpty()) {
        float pointSize = listFont.pointSizeF();
        listFont.setPointSizeF(pointSize * relativeScale * Application::fontScaleFactor());

        currentListTextItem = new EQt::GraphicsTextItem(listText, listFont);

        if (listFontColor.isValid()) {
            currentListTextItem->setBrush(QBrush(listFontColor));
        }

        if (listFontBackgroundColor.isValid()) {
            currentListTextItem->setBackgroundBrush(QBrush(listFontBackgroundColor));
        }
    }

    unsigned long numberChildren = thisElement->numberChildren();
    unsigned long childIndex     = 0;

    placementTracker->addNewJobs(numberChildren);

    currentListIndentation = listIndentationSceneUnits();

    currentAreaIdentifier = 0;
    currentLineIndex      = 0;
    currentAreaRectangle  = activeAreaRectangle(currentAreaIdentifier);
    cursorX               = (
          currentAreaRectangle.left()
        + leftMarginSceneUnits()
        + firstLineLeftMarginSceneUnits()
        + currentListIndentation
    );

    if (minimumTopSpacing < 0) {
        cursorY = currentAreaRectangle.top();
    } else {
        cursorY = currentAreaRectangle.top() + std::max(minimumTopSpacing, topSpacingSceneUnits());
    }

    currentMinimumTopSpacing = imposeNoTopSpacing;
    nextMinimumTopSpacing    = 0;

    currentReflowHint = ReflowHint::REFLOW_NOT_SUPPORTED;
    while (childIndex < numberChildren) {
        Ld::ElementPointer childElement      = thisElement->child(childIndex);
        Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());

        childPresentation->recalculatePlacement(
            placementTracker,
            this,
            childIndex,
            Q_NULLPTR, // nextSibling -- currently unused
            childIndex == 0, // honorLeadingWhitespace
            currentMinimumTopSpacing,
            lineSpacing(),
            relativeScale
        );

        // We do this after repositioning so that an aborted repositioning can still properly update the reflow hint.
        ReflowHint childReflowHint = childPresentation->reflowHint();
        if (childReflowHint > currentReflowHint) {
            currentReflowHint = childReflowHint;
        }

        nextMinimumTopSpacing = std::max(nextMinimumTopSpacing, childPresentation->bottomSpacingSceneUnits());
        ++childIndex;

        placementTracker->completedJob();
    }

    positionPresentationAreas();

    currentAreaRectangle.setBottom(cursorY + currentMaximumHeight);
    activeAreaClosed(currentAreaIdentifier, currentAreaRectangle);
    truncateActiveAreasStartingAt(currentAreaIdentifier + 1);
}


void ParagraphPresentationBase::redoPlacement(PlacementNegotiator*, unsigned long, unsigned long, float, float, float) {
    // This method should never be called.
    Q_ASSERT(false);
}


QSizeF ParagraphPresentationBase::requestArea(
        unsigned long,
        ParagraphPresentationBase::SpaceQualifier* spaceQualifier
    ) {
    double leftMargin = currentLineIndex == 0 ? firstLineLeftMarginSceneUnits() : leftMarginSceneUnits();
    double leftEdge   = currentAreaRectangle.left() + leftMargin;
    double rightEdge  = currentAreaRectangle.right() - rightMarginSceneUnits();

    if (spaceQualifier != Q_NULLPTR) {
        if (currentSpaceQualifier == SpaceQualifier::MAXIMUM_WIDTH            ||
            currentSpaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT    ) {
            *spaceQualifier = (cursorX == leftEdge) ? currentSpaceQualifier : SpaceQualifier::CURRENT_AVAILABLE;
        } else {
            *spaceQualifier = currentSpaceQualifier;
        }
    }

    if (currentSpaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT) {
        currentSpaceQualifier = SpaceQualifier::MAXIMUM_WIDTH;
    }

    return QSizeF(rightEdge - cursorX, currentAreaRectangle.bottom() - cursorY);
}


void ParagraphPresentationBase::allocateArea(
        unsigned long childIdentifier,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         ascent,
        bool          canStretch
    ) {
    PlacementLineData lineData(childIdentifier, presentationAreaId, size.width(), size.height(), ascent, canStretch);
    currentLinePresentations.append(lineData);

    if (size.height() > currentMaximumHeight) {
        currentMaximumHeight = size.height();
    }

    if (ascent > currentMaximumAscent) {
        currentMaximumAscent = ascent;
    }

    if (canStretch) {
        currentTotalStretchableWidth += size.width();
    }

    adjustCursor(size, true);
}


void ParagraphPresentationBase::areaInsufficient(unsigned long, const QSizeF& size) {
    adjustCursor(size, false);
}


void ParagraphPresentationBase::applyStretch(unsigned long, float) {
    // Frames are fixed size and can-not be stretched.  This method should never be called.
    Q_ASSERT(false);
}


QGraphicsItem* ParagraphPresentationBase::graphicsItem(unsigned long presentationAreaId) const {
    return   presentationAreaId < static_cast<unsigned long>(activeAreas.size())
           ? activeAreas.at(presentationAreaId)
           : Q_NULLPTR;
}


void ParagraphPresentationBase::resetPlacement() {
    PresentationWithPositionalChildren::resetPlacement();
    truncateActiveAreasStartingAt(0);
}


void ParagraphPresentationBase::removeFromScene() {
    truncateActiveAreasStartingAt(0);
}


double ParagraphPresentationBase::distanceToClosestPresentationArea(
        const QPointF&       location,
        const Presentation** bestPresentation,
        unsigned long*       presentationAreaId,
        QPointF*             closestPoint
    ) const {
    return distanceToClosestChild(location, bestPresentation, presentationAreaId, closestPoint);
}


bool ParagraphPresentationBase::cursorCanHighlight() const {
    return false;
}


bool ParagraphPresentationBase::pendingRepositioning() const {
    return currentPendingRepositioning;
}


void ParagraphPresentationBase::flagPendingRepositioning() {
    currentPendingRepositioning = true;
}


ParagraphPresentationBase::ReflowHint ParagraphPresentationBase::reflowHint() const {
    return currentReflowHint;
}


ParagraphPresentationBase::Justification ParagraphPresentationBase::lineJustification() {
    return Justification::LEFT;
}


float ParagraphPresentationBase::leftMarginSceneUnits() {
    return 0;
}


float ParagraphPresentationBase::firstLineLeftMarginSceneUnits() {
    return 0;
}


float ParagraphPresentationBase::rightMarginSceneUnits() {
    return 0;
}


float ParagraphPresentationBase::topSpacingSceneUnits() {
    return 0;
}


float ParagraphPresentationBase::listIndentationSceneUnits() {
    return 20;
}


QString ParagraphPresentationBase::listString(QFont* font, QColor* fontColor, QColor* fontBackgroundColor) {
    if (font != Q_NULLPTR) {
        *font = Application::font();
    }

    if (fontColor != Q_NULLPTR) {
        *fontColor = QColor();
    }

    if (fontBackgroundColor != Q_NULLPTR) {
        *fontBackgroundColor = QColor();
    }

    return QString();
}


float ParagraphPresentationBase::lineSpacing() {
    return 1.0;
}


void ParagraphPresentationBase::processRemovingChildPresentation(unsigned long, Presentation* childPresentation) {
    if (graftedToRoot()) {
        requestRepositioning(childPresentation);
    }
}


void ParagraphPresentationBase::processChildPresentationInsertedBefore(
        unsigned long,
        Presentation* childPresentation
    ) {
    if (graftedToRoot()) {
        requestRepositioning(childPresentation);
    }
}


void ParagraphPresentationBase::processChildPresentationInsertedAfter(unsigned long, Presentation* childPresentation) {
    if (graftedToRoot()) {
        requestRepositioning(childPresentation);
    }
}


void ParagraphPresentationBase::clearLineData() {
    currentLinePresentations.clear();
    currentMaximumHeight         = 0;
    currentMaximumAscent         = 0;
    currentTotalStretchableWidth = 0;
}


void ParagraphPresentationBase::adjustCursor(const QSizeF& size, bool usableAdjustment) {
    double nextCursorX = cursorX + size.width();

    if (usableAdjustment) {
        cursorX = nextCursorX;
    }

    bool startNewLine = false;

    double rightEdge  = currentAreaRectangle.right() - rightMarginSceneUnits();

    if (nextCursorX >= rightEdge) {
        positionPresentationAreas();

        if (currentMaximumHeight > 0) {
            cursorY += currentMaximumHeight;
        } else {
            cursorY += size.height();
        }

        startNewLine = true;
    }

    if (cursorY >= currentAreaRectangle.bottom()) {
        activeAreaClosed(currentAreaIdentifier, currentAreaRectangle);
        ++currentAreaIdentifier;
        currentAreaRectangle = activeAreaRectangle(currentAreaIdentifier);
        cursorY = currentAreaRectangle.top();
        startNewLine = true;
    }

    if (startNewLine) {
        double leftMargin = currentLineIndex == 0 ? firstLineLeftMarginSceneUnits() : leftMarginSceneUnits();
        cursorX = currentAreaRectangle.left() + leftMargin + currentListIndentation;

        clearLineData();
    } else if (!usableAdjustment) {
        cursorX = nextCursorX;
    }
}


void ParagraphPresentationBase::positionPresentationAreas() {
    float  left          = 0;
    double stretchFactor = 0;

    float  normalLeftMargin = leftMarginSceneUnits();
    if (!currentLinePresentations.isEmpty()) {
        if (currentLineIndex == 0 && currentListTextItem != Q_NULLPTR) {
            QFont font = currentListTextItem->font();
            QFontMetricsF fontMetrics(font);
            float height = fontMetrics.height();
            float ascent = fontMetrics.ascent();

            if (currentMaximumHeight < height) {
                currentMaximumHeight = height;
            }

            if (currentMaximumAscent < ascent) {
                currentMaximumAscent = ascent;
            }

            activeAreas.at(currentAreaIdentifier)->addToGroup(currentListTextItem);
            currentListTextItem->setPos(normalLeftMargin, cursorY + currentMaximumAscent - ascent);
        }

        float leftMargin =   currentLineIndex == 0
                           ? firstLineLeftMarginSceneUnits() + normalLeftMargin
                           : normalLeftMargin;
        float leftEdge   = currentAreaRectangle.left() + leftMargin + currentListIndentation;

        ++currentLineIndex;

        switch(lineJustification()) {
            case Justification::LEFT: {
                left          = leftEdge;
                stretchFactor = 0;

                break;
            }

            case Justification::RIGHT: {
                double textWidth  = cursorX - leftEdge;

                left = currentAreaRectangle.right() - rightMarginSceneUnits() - textWidth;
                stretchFactor = 0;

                break;
            }

            case Justification::CENTER: {
                double textWidth  = cursorX - leftEdge;
                double rightEdge  = currentAreaRectangle.right() - rightMarginSceneUnits();

                left          = (leftEdge + rightEdge - textWidth) / 2.0;
                stretchFactor = 0;

                break;
            }

            case Justification::JUSTIFY: {
                left = leftEdge;

                if (currentTotalStretchableWidth > 0) {
                    double textWidth     = cursorX - leftEdge;
                    double rightEdge     = currentAreaRectangle.right() - rightMarginSceneUnits();
                    double areaWidth     = rightEdge - leftEdge;
                    double stretchAmount = areaWidth - textWidth;

                    stretchFactor = stretchAmount / currentTotalStretchableWidth;

                    if (stretchFactor > maximumStretchFactor) {
                        stretchFactor = 0;
                    }
                } else {
                    stretchFactor = 0;
                }

                break;

            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        QSharedPointer<Ld::ElementWithPositionalChildren> thisElement = element()
                                                                        .dynamicCast<Ld::ElementWithPositionalChildren>();

        for (  QList<PlacementLineData>::const_iterator it  = currentLinePresentations.constBegin(),
                                                        end = currentLinePresentations.constEnd()
             ; it != end
             ; ++it
            ) {
            unsigned long      childIndex         = it->childIdentifier();
            unsigned long      presentationAreaId = it->presentationAreaId();
            double             width              = it->width();
            bool               canStretch         = it->canStretch();
            double             ascent             = it->ascent();
            Ld::ElementPointer childElement       = thisElement->child(childIndex);
            Presentation*      childPresentation  = dynamic_cast<Presentation*>(childElement->visual());
            QGraphicsItem*     graphicsItem       = childPresentation->graphicsItem(presentationAreaId);
            double             presentationAreaY  = ascent == 0 ? cursorY : cursorY + currentMaximumAscent - ascent;

            if (stretchFactor > 0 && canStretch) {
                childPresentation->applyStretch(presentationAreaId, stretchFactor);
                width *= 1.0 + stretchFactor;
            }

            activeAreas.at(currentAreaIdentifier)->addToGroup(graphicsItem);
            graphicsItem->setPos(left, presentationAreaY);

            left += width;
        }

        currentMinimumTopSpacing = nextMinimumTopSpacing;
        nextMinimumTopSpacing    = 0;
    }
}


QRectF ParagraphPresentationBase::activeAreaRectangle(unsigned long areaIdentifier) {
    Q_ASSERT(currentParentNegotiator != Q_NULLPTR);
    Q_ASSERT(currentChildIdentifier != invalidChildIdentifier);

    QSizeF areaSize;

    do {
        areaSize = currentParentNegotiator->requestArea(currentChildIdentifier, &currentSpaceQualifier);

        if (currentSpaceQualifier == SpaceQualifier::CURRENT_AVAILABLE) {
            currentParentNegotiator->areaInsufficient(currentChildIdentifier, areaSize);
        }
    } while (currentSpaceQualifier == SpaceQualifier::CURRENT_AVAILABLE);

    if (areaIdentifier >= static_cast<unsigned long>(activeAreas.size())) {
        Q_ASSERT(areaIdentifier == static_cast<unsigned long>(activeAreas.size()));
        activeAreas.append(new EQt::GraphicsItemGroup);
    }

    return QRectF(QPointF(0, 0), areaSize);
}


void ParagraphPresentationBase::activeAreaClosed(unsigned long areaIdentifier, const QRectF& activeRectangle) {
    Q_ASSERT(currentParentNegotiator != nullptr);
    Q_ASSERT(currentChildIdentifier != invalidChildIdentifier);

    currentParentNegotiator->allocateArea(currentChildIdentifier, areaIdentifier, activeRectangle.size());
}


void ParagraphPresentationBase::truncateActiveAreasStartingAt(unsigned long newListSize) {
    if (newListSize == 0 && currentListTextItem != Q_NULLPTR) {
        currentListTextItem->deleteLater();
        currentListTextItem = Q_NULLPTR;
    }

    unsigned long numberAreas = static_cast<unsigned long>(activeAreas.size());
    if (newListSize < numberAreas) {
        for (unsigned long index=newListSize ; index<numberAreas ; ++index) {
            EQt::GraphicsItemGroup* activeArea = activeAreas.at(index);
            activeArea->deleteLater();
        }

        activeAreas.erase(activeAreas.begin() + newListSize, activeAreas.end());
    }
}
