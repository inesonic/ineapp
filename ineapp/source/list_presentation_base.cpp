/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ListPresentationBase class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QSize>
#include <QRectF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>
#include <QBrush>
#include <QPointF>

#include <algorithm>
#include <cmath>

#include <util_units.h>

#include <eqt_graphics_math_group.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>
#include <ld_list_element_base.h>
#include <ld_visual_with_positional_children.h>
#include <ld_list_placeholder_element.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_operator_format.h>
#include <ld_diagnostic.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "presentation_with_positional_children.h"
#include "presentation_area_tracker.h"
#include "list_presentation_base.h"

ListPresentationBase::ListPresentationBase() {
    currentGraphicsItem = Q_NULLPTR;
}


ListPresentationBase::~ListPresentationBase() {
    removeFromScene();
}


void ListPresentationBase::requestRepositioning(Presentation* /* childPresentation */) {
    Ld::ElementPointer element = ListPresentationBase::element();
    if (!element.isNull()) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPlacementNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPlacementNegotiator != Q_NULLPTR) {
                parentPlacementNegotiator->requestRepositioning(this);
            }
        }
    }
}


void ListPresentationBase::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                minimumTopSpacing,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    QSharedPointer<Ld::ListElementBase> listElement = element().dynamicCast<Ld::ListElementBase>();
    Q_ASSERT(!listElement.isNull());

    Ld::ElementPointer parentElement = listElement->parent();

    QSharedPointer<Ld::FontFormat> format = listElement->format().dynamicCast<Ld::FontFormat>();
    if (format.isNull()) {
        format = Ld::CharacterFormat::applicationDefaultMathFont();
    }

    (void) populatePresentationAreaTrackers(placementTracker, minimumTopSpacing, relativeScale);

    QFont   font            = format->toQFont();
    QColor  color           = format->fontColor();
    QColor  backgroundColor = format->fontBackgroundColor();
    float   fontPointSize   = font.pointSizeF();
    float   fontScaleFactor = Application::fontScaleFactor();

    if (fontScaleFactor != 1.0F || relativeScale != 1.0F) {
        font.setPointSizeF(fontPointSize * fontScaleFactor * relativeScale);
    }

    QPen   fontPen(color.isValid() ? color : QColor(Qt::black));
    QBrush fontBackgroundBrush(backgroundColor.isValid() ? backgroundColor : QColor(255, 255, 255, 0));

    QString                                  separator  = separatorCharacters();
    EQt::GraphicsMathGroup::ParenthesisStyle parenStyle = parenthesisStyle();

    QFontMetricsF fontMetrics(font);
    float fontHeight       = fontMetrics.height();
    float fontAscent       = fontMetrics.ascent();
    float separatorWidth   = fontMetrics.horizontalAdvance(separator);

    float maximumAscent  = fontAscent;
    float maximumDescent = fontHeight - fontAscent;

    calculateMaximumPresentationAreaAscentAndDescent(maximumAscent, maximumDescent);

    float maximumHeight = maximumAscent + maximumDescent;

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, maximumHeight, parenStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, maximumHeight, parenStyle);

    if (currentGraphicsItem == Q_NULLPTR) {
        currentGraphicsItem = new EQt::GraphicsMathGroup;
    } else {
        currentGraphicsItem->clearText();
    }

    currentGraphicsItem->setTextPen(fontPen);
    currentGraphicsItem->setBackgroundBrush(fontBackgroundBrush);

    unsigned long numberChildren = element()->numberChildren();
    float         currentX       = leftParenthesisData.width();
    for (unsigned long childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
        if (childIndex > 0 && listElement->child(childIndex)->typeName() != Ld::ListPlaceholderElement::elementName) {
            currentGraphicsItem->append(separator, font, QPointF(currentX, maximumAscent));
            currentX += separatorWidth;
        }

        const PresentationAreaTracker& presentationAreas       = childPresentationAreas.at(childIndex);
        unsigned long                  numberPresentationAreas = presentationAreas.numberPresentationAreas();

        for (unsigned long areaIndex=0 ; areaIndex<numberPresentationAreas ; ++areaIndex) {
            const PresentationAreaTracker::Entry& entry        = presentationAreas.entry(areaIndex);
            QGraphicsItem*                        graphicsItem = entry.graphicsItem();

            currentGraphicsItem->addToGroup(graphicsItem);
            graphicsItem->setPos(currentX, minimumTopSpacing + maximumAscent - entry.ascent());

            currentX += entry.areaSize().width();
        }
    }

    if (numberChildren == 0) {
        currentX += fontMetrics.horizontalAdvance(tr(" "));
    }

    unsigned parenthesisIndex = currentGraphicsItem->numberTextEntries();
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentGraphicsItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentGraphicsItem, parenthesisIndex);
    currentGraphicsItem->setParenthesisCenterLine(
        maximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    allocateSpaceForThisPresentation(
        parent,
        childIdentifier,
        QSizeF(currentX, minimumTopSpacing + maximumHeight),
        maximumAscent + minimumTopSpacing
    );
}


void ListPresentationBase::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        firstPresentationAreaId,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    if (firstPresentationAreaId == 0) {
        Q_ASSERT(currentGraphicsItem != Q_NULLPTR);
        parent->allocateArea(childIdentifier, 0, currentGraphicsItem->boundingRect().size(), requiredAscent);
    }
}


QSizeF ListPresentationBase::requestArea(
        unsigned long                         /* childIdentifier */,
        ListPresentationBase::SpaceQualifier* spaceQualifier
    ) {
    if (spaceQualifier != Q_NULLPTR) {
        // All operators use the provided space.
        *spaceQualifier = SpaceQualifier::USE_PROVIDED;
    }

    return maximumPossibleSpace;
}


void ListPresentationBase::allocateArea(
        unsigned long childIdentifier,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         ascent,
        bool          /* canStretch */
    ) {
    if (childIdentifier < static_cast<unsigned long>(childPresentationAreas.size())) {
        childPresentationAreas[childIdentifier].allocateArea(presentationAreaId, size, ascent);
    } else {
        Ld::ElementPointer listElement       = element();
        Ld::ElementPointer childElement      = listElement->child(childIdentifier);
        Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());

        PresentationAreaTracker tracker;
        tracker.setChildPresentation(childPresentation);
        tracker.allocateArea(presentationAreaId, size, ascent);

        childPresentationAreas.append(tracker);
    }
}


void ListPresentationBase::areaInsufficient(unsigned long /* childIdentifier */, const QSizeF& /* size */) {
    Q_ASSERT(false); // We should never call this method.
}


void ListPresentationBase::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We should never receive a call to this method.
}


QGraphicsItem* ListPresentationBase::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? currentGraphicsItem : Q_NULLPTR;
}


void ListPresentationBase::resetPlacement() {
    PresentationWithPositionalChildren::resetPlacement();
    removeFromScene();
}


void ListPresentationBase::removeFromScene() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }
}


QList<QRectF> ListPresentationBase::cursorRangeToScene(
        const Ld::ElementCursor& /* endingElementCursor */,
        const Ld::ElementCursor& /* startingElementCursor */
    ) const {
    QList<QRectF> result;

    if (currentGraphicsItem != Q_NULLPTR) {
        QPointF topLeftCorner     = currentGraphicsItem->scenePos();
        QPointF bottomRightCorner = currentGraphicsItem->boundingRect().bottomRight();
        QRectF  boundingRectangle(topLeftCorner, QSizeF(bottomRightCorner.x(), bottomRightCorner.y()));

        result << boundingRectangle;
    }

    return result;
}


EQt::GraphicsMathGroup::ParenthesisStyle ListPresentationBase::parenthesisStyle() const {
    return EQt::GraphicsMathGroup::ParenthesisStyle::NONE;
}


QString ListPresentationBase::separatorCharacters() const {
    return tr(", ");
}


void ListPresentationBase::setGraphicsItem(EQt::GraphicsMathGroup* newGraphicsItem) {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
    }

    currentGraphicsItem = newGraphicsItem;
}


EQt::GraphicsMathGroup* ListPresentationBase::graphicsItem() const {
    return currentGraphicsItem;
}


QList<PresentationAreaTracker>& ListPresentationBase::populatePresentationAreaTrackers(
        PlacementTracker* placementTracker,
        float             minimumTopSpacing,
        float             relativeScale
    ) {
    childPresentationAreas.clear();

    Ld::ElementPointer element = ListPresentationBase::element();
    placementTracker->addNewJobs(element->numberChildren());

    unsigned long numberChildren = element->numberChildren();
    for (unsigned long childIndex=1 ; childIndex<numberChildren ; ++childIndex) {
        unsigned long      childIdentifier     = childIndex - 1;
        Ld::ElementPointer childElement        = element->child(childIdentifier);
        Ld::ElementPointer nextSibling         = element->child(childIndex);
        Presentation*      childPresentation   = dynamic_cast<Presentation*>(childElement->visual());
        Presentation*      siblingPresentation = dynamic_cast<Presentation*>(nextSibling->visual());

        childPresentation->recalculatePlacement(
            placementTracker,
            this,
            childIdentifier,
            siblingPresentation,
            false,
            minimumTopSpacing,
            defaultLineSpacing,
            relativeScale
        );

        placementTracker->completedJob();
    }

    if (numberChildren > 0) {
        Ld::ElementPointer childElement      = element->child(numberChildren - 1);
        Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());

        childPresentation->recalculatePlacement(
            placementTracker,
            this,
            numberChildren - 1,
            Q_NULLPTR,
            false,
            minimumTopSpacing,
            defaultLineSpacing,
            relativeScale
        );

        placementTracker->completedJob();
    }

    return childPresentationAreas;
}


QList<PresentationAreaTracker>& ListPresentationBase::presentationAreaTrackers() {
    return childPresentationAreas;
}


void ListPresentationBase::calculateMaximumPresentationAreaAscentAndDescent(
        float& maximumAscent,
        float& maximumDescent
    ) const {
    unsigned long numberChildren = element()->numberChildren();
    for (unsigned long childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
        const PresentationAreaTracker& presentationAreas = childPresentationAreas.at(childIndex);

        float areaAscent  = presentationAreas.maximumAscent();
        float areaDescent = presentationAreas.maximumDescent();

        if (areaAscent > maximumAscent) {
            maximumAscent = areaAscent;
        }

        if (areaDescent > maximumDescent) {
            maximumDescent = areaDescent;
        }
    }
}


void ListPresentationBase::allocateSpaceForThisPresentation(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        const QSizeF&        newSize,
        float                newAscent
    ) {
    requiredAscent = newAscent;

    bool placed = false;
    do {
        SpaceQualifier spaceQualifier;
        QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
            spaceQualifier == SpaceQualifier::USE_PROVIDED             ||
            (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH &&
             availableSpace.height() >= newSize.height()        )      ||
            (availableSpace.height() >= newSize.height() &&
             availableSpace.width() >= newSize.width()      )             ) {
            parent->allocateArea(
                childIdentifier,
                0,
                newSize,
                newAscent,
                false // canStretch
            );

            placed = true;
        } else {
            parent->areaInsufficient(childIdentifier, availableSpace);
        }
    } while (!placed);
}


void ListPresentationBase::processDataChange() {}


void ListPresentationBase::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder(diagnostic));
        currentGraphicsItem->setToolTip(updateToolTip(diagnostic));
    }
}


void ListPresentationBase::clearDiagnosticDisplay() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder());
        currentGraphicsItem->setToolTip(updateToolTip());
    }
}


void ListPresentationBase::processRemovingChildPresentation(unsigned long, Presentation*) {
    if (!element().isNull()) {
        requestRepositioning(Q_NULLPTR);
    }
}


void ListPresentationBase::processChildPresentationInsertedBefore(unsigned long, Presentation* childPresentation) {
    requestRepositioning(childPresentation);
}


void ListPresentationBase::processChildPresentationInsertedAfter(unsigned long, Presentation* childPresentation) {
    requestRepositioning(childPresentation);
}
