/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref OperatorPresentationBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QList>
#include <QBitArray>
#include <QFontMetricsF>
#include <QGraphicsItem>
#include <QFont>
#include <QPen>

#include <util_algorithm.h>

#include <eqt_graphics_item_group.h>
#include <eqt_graphics_math_group.h>

#include <cmath>
#include <limits>
#include <algorithm>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_format.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_operator_format.h>

#include "application.h"
#include "placement_tracker.h"
#include "presentation.h"
#include "presentation_with_fixed_children.h"
#include "scene_units.h"
#include "operator_presentation_base.h"

static_assert(
       Ld::OperatorFormat::ParenthesisStyle::INVALID_OR_NONE
       == static_cast<Ld::OperatorFormat::ParenthesisStyle>(EQt::GraphicsMathGroup::ParenthesisStyle::INVALID)
    && Ld::OperatorFormat::ParenthesisStyle::PARENTHESIS
       == static_cast<Ld::OperatorFormat::ParenthesisStyle>(EQt::GraphicsMathGroup::ParenthesisStyle::PARENTHESIS)
    && Ld::OperatorFormat::ParenthesisStyle::BRACKETS
       == static_cast<Ld::OperatorFormat::ParenthesisStyle>(EQt::GraphicsMathGroup::ParenthesisStyle::BRACKETS)
    && Ld::OperatorFormat::ParenthesisStyle::BRACES
       == static_cast<Ld::OperatorFormat::ParenthesisStyle>(EQt::GraphicsMathGroup::ParenthesisStyle::BRACES),
    "Mismatched between Ld::OperatorFormat::ParenthesisStyle and EQt::GraphicsMathGroup::ParenthesisStyle"
);

OperatorPresentationBase::OperatorPresentationBase() {
    currentGraphicsItem = Q_NULLPTR;
}


OperatorPresentationBase::~OperatorPresentationBase() {
    removeFromScene();
}


void OperatorPresentationBase::requestRepositioning(Presentation* childPresentation) {
    Ld::ElementPointer element        = OperatorPresentationBase::element();
    unsigned long      numberChildren = element->numberChildren();

    reallocateTrackers();

    if (!element.isNull()) {
        for (unsigned childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
            Ld::ElementPointer childElement = element->child(childIndex);
            if (!childElement.isNull()) {
                Presentation* presentation = dynamic_cast<Presentation*>(childElement->visual());
                if (presentation != Q_NULLPTR && childPresentation == presentation) {
                    currentChildPresentationAreas[childIndex].clear();
                }
            }
        }

        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPlacementNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPlacementNegotiator != Q_NULLPTR) {
                parentPlacementNegotiator->requestRepositioning(this);
            }
        }
    }
}


void OperatorPresentationBase::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    Ld::ElementPointer element = OperatorPresentationBase::element();
    Q_ASSERT(!element.isNull());

    Ld::FormatPointer format     = element->format();
    ParenthesisStyle  parenStyle = parenthesisStyle(element);

    populateTrackers(placementTracker, relativeScale);

    if (currentGraphicsItem == Q_NULLPTR) {
        currentGraphicsItem = buildGraphicsItem(
            format,
            parenStyle,
            relativeScale,
            currentChildPresentationAreas,
            requiredSize,
            requiredAscent
        );
    } else {
        currentGraphicsItem = updateGraphicsItem(
            format,
            currentGraphicsItem,
            parenStyle,
            relativeScale,
            currentChildPresentationAreas,
            requiredSize,
            requiredAscent
        );
    }

    Ld::DiagnosticPointer diagnostic = element->diagnostic();

    QPen borderPen = updateBorder(diagnostic);
    currentGraphicsItem->setBorderPen(borderPen);

    QString toolTip = updateToolTip(diagnostic);
    currentGraphicsItem->setToolTip(toolTip);

    bool placed = false;
    do {
        SpaceQualifier spaceQualifier;
        QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
            spaceQualifier == SpaceQualifier::USE_PROVIDED             ||
            (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH  &&
             availableSpace.height() >= requiredSize.height()    )     ||
            (availableSpace.height() >= requiredSize.height() &&
             availableSpace.width() >= requiredSize.width()      )        ) {
            parent->allocateArea(
                childIdentifier,
                0,
                requiredSize,
                requiredAscent,
                false // canStretch
            );

            placed = true;
        } else {
            parent->areaInsufficient(childIdentifier, availableSpace);
        }
    } while (!placed);
}


void OperatorPresentationBase::redoPlacement(
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        unsigned long        firstPresentationAreaId,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    if (firstPresentationAreaId == 0) {
        Q_ASSERT(currentGraphicsItem != Q_NULLPTR);
        parent->allocateArea(childIdentifier, 0, requiredSize, requiredAscent, false);
    }
}


QSizeF OperatorPresentationBase::requestArea(
        unsigned long                             /* childIdentifier */,
        OperatorPresentationBase::SpaceQualifier* spaceQualifier
    ) {
    if (spaceQualifier != Q_NULLPTR) {
        // All operators use the provided space.
        *spaceQualifier = SpaceQualifier::USE_PROVIDED;
    }

    return maximumPossibleSpace;
}


void OperatorPresentationBase::allocateArea(
        unsigned long childIdentifier,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         ascent,
        bool          /* canStretch */
    ) {
    currentChildPresentationAreas[childIdentifier].allocateArea(presentationAreaId, size, ascent);
}


void OperatorPresentationBase::areaInsufficient(unsigned long /* childIdentifier */, const QSizeF& /* size */) {
    // Child math primitives are given as much space as needed when sizing so this method should never be called.
    Q_ASSERT(false);
}


void OperatorPresentationBase::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We require that all math primitives be unable to stretch.
}


QGraphicsItem* OperatorPresentationBase::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? currentGraphicsItem : Q_NULLPTR;
}


void OperatorPresentationBase::resetPlacement() {
    Ld::ElementPointer element        = OperatorPresentationBase::element();
    unsigned long      numberChildren = element->numberChildren();

    for (unsigned index=0 ; index<numberChildren ; ++index) {
        currentChildPresentationAreas[index].resetPlacement();
    }

    PresentationWithFixedChildren::resetPlacement();
}


void OperatorPresentationBase::removeFromScene() {
    Ld::ElementPointer element = OperatorPresentationBase::element();
    if (!element.isNull()) {
        unsigned long numberChildren = element->numberChildren();

        for (unsigned index=0 ; index<numberChildren ; ++index) {
            currentChildPresentationAreas[index].removeFromScene();
        }
    }

    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }
}


void OperatorPresentationBase::tiedToElement(Ld::ElementPointer element) {
    reallocateTrackers();
    PresentationWithFixedChildren::tiedToElement(element);
}


void OperatorPresentationBase::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    if (currentGraphicsItem != Q_NULLPTR) {
        QPen newPen = updateBorder(diagnostic);
        currentGraphicsItem->setBorderPen(newPen);

        QString toolTip = updateToolTip(diagnostic);
        currentGraphicsItem->setToolTip(toolTip);
    }
}


void OperatorPresentationBase::clearDiagnosticDisplay() {
    if (currentGraphicsItem != Q_NULLPTR) {
        QPen newPen = updateBorder(Ld::DiagnosticPointer());
        currentGraphicsItem->setBorderPen(newPen);

        QString toolTip = updateToolTip(Ld::DiagnosticPointer());
        currentGraphicsItem->setToolTip(toolTip);
    }
}


void OperatorPresentationBase::processPresentationRemoved(unsigned long, Presentation* childPresentation) {
    if (childPresentation != Q_NULLPTR) {
        requestRepositioning(childPresentation);
    } else {
        requestRepositioning(this);
    }
}


void OperatorPresentationBase::processChildPresentationSet(unsigned long, Presentation* childPresentation) {
    if (childPresentation != Q_NULLPTR) {
        requestRepositioning(childPresentation);
    } else {
        requestRepositioning(this);
    }
}


QList<QRectF> OperatorPresentationBase::cursorRangeToScene(
        const Ld::ElementCursor& /* endingElementCursor */,
        const Ld::ElementCursor& /* startingElementCursor */
    ) const {
    QList<QRectF> result;

    if (currentGraphicsItem != Q_NULLPTR) {
        QRectF boundingRectangle = currentGraphicsItem->boundingRect();
        QPointF topLeft          = currentGraphicsItem->mapToScene(boundingRectangle.topLeft());
        QPointF bottomRight      = currentGraphicsItem->mapToScene(boundingRectangle.bottomRight());

        result << QRectF(topLeft, bottomRight);
    }

    return result;
}


QFont OperatorPresentationBase::operatorFont(Ld::FormatPointer format, float relativeScale) const {
    QFont result;

    if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
        Q_ASSERT(!fontFormat.isNull());

        result = fontFormat->toQFont();
    } else {
        result = Ld::CharacterFormat::applicationDefaultMathFont()->toQFont();
    }

    float fontScaleFactor = Application::fontScaleFactor() * relativeScale;
    if (fontScaleFactor != 1.0F) {
        float fontPointSize = result.pointSizeF();
        result.setPointSizeF(fontPointSize * fontScaleFactor);
    }

    return result;
}


QColor OperatorPresentationBase::operatorColor(Ld::FormatPointer format) const {
    QColor result;

    if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
        Q_ASSERT(!fontFormat.isNull());

        result = fontFormat->fontColor();
    }

    return result;
}


QColor OperatorPresentationBase::backgroundColor(Ld::FormatPointer format) const {
    QColor result;

    if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
        Q_ASSERT(!fontFormat.isNull());

        result = fontFormat->fontBackgroundColor();
    }

    return result;
}


OperatorPresentationBase::ParenthesisStyle OperatorPresentationBase::parenthesisStyle(
        Ld::ElementPointer element
    ) const {
    return element->parenthesisStyle();
}


float OperatorPresentationBase::childRelativeScale(float parentRelativeScale, unsigned long /* childIndex */) const {
    return parentRelativeScale;
}


void OperatorPresentationBase::setColors(EQt::GraphicsMathGroup* graphicsItem, Ld::FormatPointer format) {
    QColor currentOperatorColor = operatorColor(format);
    if (!currentOperatorColor.isValid()) {
        currentOperatorColor = QColor(Qt::black);
    }

    if (graphicsItem->textPen().color() != currentOperatorColor) {
        QPen textPen(currentOperatorColor);
        graphicsItem->setTextPen(textPen);
        graphicsItem->setParenthesisPen(textPen);
        graphicsItem->setParenthesisBrush(QBrush(currentOperatorColor));
    }

    QColor currentBackgroundColor = backgroundColor(format);
    if (!currentBackgroundColor.isValid()) {
        currentBackgroundColor = QColor(255, 255, 255, 0);
    }

    if (graphicsItem->backgroundBrush().color() != currentBackgroundColor) {
        graphicsItem->setBackgroundBrush(QBrush(currentBackgroundColor));
    }
}


float OperatorPresentationBase::addPresentationAreas(
        const PresentationAreaTracker& presentationAreas,
        EQt::GraphicsItemGroup*        graphicsItemGroup,
        const QPointF&                 topLeftCorner,
        float                          maximumAscent
    ) {
    float currentX = topLeftCorner.x();
    float topY     = topLeftCorner.y();

    for (  PresentationAreaTracker::Iterator presentationAreaIterator    = presentationAreas.begin(),
                                             presentationAreaEndIterator = presentationAreas.end()
         ; presentationAreaIterator != presentationAreaEndIterator
         ; ++presentationAreaIterator
        ) {
        const PresentationAreaTracker::Entry& areaEntry = *presentationAreaIterator;
        QGraphicsItem* childItem   = areaEntry.graphicsItem();
        float          childAscent = areaEntry.ascent();

        if (childItem->parentItem() != graphicsItemGroup) {
            graphicsItemGroup->addToGroup(childItem);
        }

        childItem->setPos(currentX, topY + maximumAscent - childAscent);

        currentX += areaEntry.areaSize().width();
    }

    return currentX;
}


void OperatorPresentationBase::clearChildPresentationAreas() {
    Ld::ElementPointer element        = OperatorPresentationBase::element();
    unsigned long      numberChildren = element->numberChildren();

    for (unsigned childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
        currentChildPresentationAreas[childIndex].clear();
    }
}


void OperatorPresentationBase::populateTrackers(PlacementTracker* placementTracker, float relativeScale) {
    Ld::ElementPointer element = OperatorPresentationBase::element();
    Q_ASSERT(!element.isNull());

    placementTracker->addNewJobs(element->numberChildren());

    reallocateTrackers();

    unsigned long numberChildren = element->numberChildren();
    for (unsigned childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
        if (!currentChildPresentationAreas[childIndex].containsChildData()) {
            Ld::ElementPointer childElement = element->child(childIndex);
            if (!childElement.isNull()) {
                Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
                if (childPresentation != Q_NULLPTR) {
                    currentChildPresentationAreas[childIndex].setChildPresentation(childPresentation);

                    Presentation* nextSiblingPresentation = Q_NULLPTR;

                    if (childIndex < (numberChildren - 1)) {
                        Ld::ElementPointer nextSiblingElement = element->child(childIndex + 1);
                        if (!nextSiblingElement.isNull()) {
                            nextSiblingPresentation = dynamic_cast<Presentation*>(nextSiblingElement->visual());
                        }
                    }

                    childPresentation->recalculatePlacement(
                        placementTracker,
                        this,
                        childIndex,
                        nextSiblingPresentation,
                        false, // honorLeadingWhitespace
                        imposeNoTopSpacing,
                        defaultLineSpacing,
                        childRelativeScale(relativeScale, childIndex)
                    );
                }
            }
        }
    }

    placementTracker->completedJobs(element->numberChildren());
}


void OperatorPresentationBase::reallocateTrackers() {
    unsigned long numberChildren = element()->numberChildren();
    if (numberChildren != static_cast<unsigned long>(currentChildPresentationAreas.size())) {
        while (numberChildren > static_cast<unsigned long>(currentChildPresentationAreas.size())) {
            currentChildPresentationAreas.append(PresentationAreaTracker());
        }

        if (numberChildren < static_cast<unsigned long>(currentChildPresentationAreas.size())) {
            currentChildPresentationAreas.erase(
                currentChildPresentationAreas.begin() + numberChildren,
                currentChildPresentationAreas.end()
            );
        }
    }
}
