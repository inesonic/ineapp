/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref GroupingOperatorPresentationBase class.
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
#include "grouping_operator_presentation_base.h"

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

GroupingOperatorPresentationBase::GroupingOperatorPresentationBase() {
    currentGraphicsItem = Q_NULLPTR;
}


GroupingOperatorPresentationBase::~GroupingOperatorPresentationBase() {
    removeFromScene();
}


void GroupingOperatorPresentationBase::requestRepositioning(Presentation* /* childPresentation */) {
    // We have only one child so we know which child we care about.

    currentChildPresentationArea.clear();

    Ld::ElementPointer parent = element()->parent();
    if (!parent.isNull()) {
        PlacementNegotiator* parentPlacementNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
        if (parentPlacementNegotiator != Q_NULLPTR) {
            parentPlacementNegotiator->requestRepositioning(this);
        }
    }
}


void GroupingOperatorPresentationBase::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    Ld::ElementPointer element = GroupingOperatorPresentationBase::element();
    Q_ASSERT(!element.isNull());

    if (!currentChildPresentationArea.containsChildData()) {
        Ld::ElementPointer childElement = element->child(0);
        if (!childElement.isNull()) {
            Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
            if (childPresentation != Q_NULLPTR) {
                currentChildPresentationArea.setChildPresentation(childPresentation);
                childPresentation->recalculatePlacement(
                    placementTracker,
                    this,
                    0,
                    Q_NULLPTR,
                    false, // honorLeadingWhitespace
                    imposeNoTopSpacing,
                    defaultLineSpacing,
                    relativeScale
                );
            }
        }
    }

    float childMaximumAscent  = currentChildPresentationArea.maximumAscent();
    float childMaximumDescent = currentChildPresentationArea.maximumDescent();
    float childHeight         = childMaximumAscent + childMaximumDescent;

    if (currentGraphicsItem == Q_NULLPTR) {
        currentGraphicsItem = new EQt::GraphicsMathGroup;
    }

    Ld::FormatPointer format = element->format();
    QFont             font;
    QColor            parenthesisColor;
    QColor            backgroundColor;
    if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();

        font             = fontFormat->toQFont();
        parenthesisColor = fontFormat->fontColor();
        backgroundColor  = fontFormat->fontBackgroundColor();
    } else {
        QSharedPointer<Ld::FontFormat> fontFormat = Ld::CharacterFormat::applicationDefaultMathFont();

        font             = fontFormat->toQFont();
        parenthesisColor = fontFormat->fontColor();
        backgroundColor  = fontFormat->fontBackgroundColor();
    }

    if (!parenthesisColor.isValid()) {
        parenthesisColor = QColor(Qt::black);
    }

    if (!backgroundColor.isValid()) {
        backgroundColor = QColor(255, 255, 255, 0);
    }

    currentGraphicsItem->setBackgroundBrush(QBrush(backgroundColor));
    currentGraphicsItem->setParenthesisPen(QPen(parenthesisColor));

    EQt::GraphicsMathGroup::ParenthesisStyle leftParenStyle  = leftParenthesisStyle(format);
    EQt::GraphicsMathGroup::ParenthesisStyle rightParenStyle = rightParenthesisStyle(format);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, childHeight, leftParenStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, childHeight, rightParenStyle);

    float currentX = leftParenthesisData.width();
    for (  PresentationAreaTracker::Iterator presentationAreaIterator    = currentChildPresentationArea.begin(),
                                             presentationAreaEndIterator = currentChildPresentationArea.end()
         ; presentationAreaIterator != presentationAreaEndIterator
         ; ++presentationAreaIterator
        ) {
        const PresentationAreaTracker::Entry& areaEntry = *presentationAreaIterator;
        QGraphicsItem* childItem   = areaEntry.graphicsItem();
        float          childAscent = areaEntry.ascent();

        if (childItem->parentItem() != currentGraphicsItem) {
            currentGraphicsItem->addToGroup(childItem);
        }

        childItem->setPos(currentX, childMaximumAscent - childAscent);
        currentX += areaEntry.areaSize().width();
    }

    unsigned parenthesisIndex = 0;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentGraphicsItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentGraphicsItem, parenthesisIndex);

    QFontMetricsF fontMetrics(font);
    currentGraphicsItem->setParenthesisCenterLine(
        childMaximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    Ld::DiagnosticPointer diagnostic = element->diagnostic();

    currentGraphicsItem->setBorderPen(updateBorder(diagnostic));
    currentGraphicsItem->setToolTip(updateToolTip(diagnostic));

    requiredSize   = QSizeF(currentX, childHeight);
    requiredAscent = childMaximumAscent;

    bool placed = false;
    do {
        SpaceQualifier spaceQualifier;
        QSizeF         availableSpace = parent->requestArea(childIdentifier, &spaceQualifier);

        if (spaceQualifier == SpaceQualifier::USE_PROVIDED             ||
            spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT ||
            (spaceQualifier == SpaceQualifier::MAXIMUM_WIDTH &&
             availableSpace.height() >= requiredSize.height()   )      ||
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


void GroupingOperatorPresentationBase::redoPlacement(
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


QSizeF GroupingOperatorPresentationBase::requestArea(
        unsigned long                                     /* childIdentifier */,
        GroupingOperatorPresentationBase::SpaceQualifier* spaceQualifier
    ) {
    if (spaceQualifier != Q_NULLPTR) {
        // All operators use the provided space.
        *spaceQualifier = SpaceQualifier::USE_PROVIDED;
    }

    return maximumPossibleSpace;
}


void GroupingOperatorPresentationBase::allocateArea(
        unsigned long childIdentifier,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         ascent,
        bool          /* canStretch */
    ) {
    Q_ASSERT(childIdentifier == 0);
    currentChildPresentationArea.allocateArea(presentationAreaId, size, ascent);
}


void GroupingOperatorPresentationBase::areaInsufficient(unsigned long /* childIdentifier */, const QSizeF& /* size */) {
    // Child math primitives are given as much space as needed when sizing so this method should never be called.
    Q_ASSERT(false);
}


void GroupingOperatorPresentationBase::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We require that all math primitives be unable to stretch.
}


QGraphicsItem* GroupingOperatorPresentationBase::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? currentGraphicsItem : Q_NULLPTR;
}


void GroupingOperatorPresentationBase::resetPlacement() {
    currentChildPresentationArea.resetPlacement();
    PresentationWithFixedChildren::resetPlacement();
}


void GroupingOperatorPresentationBase::removeFromScene() {
    currentChildPresentationArea.removeFromScene();

    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }
}


void GroupingOperatorPresentationBase::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    if (currentGraphicsItem != Q_NULLPTR) {
        QPen newPen = updateBorder(diagnostic);
        currentGraphicsItem->setBorderPen(newPen);

        QString toolTip = updateToolTip(diagnostic);
        currentGraphicsItem->setToolTip(toolTip);
    }
}


void GroupingOperatorPresentationBase::clearDiagnosticDisplay() {
    if (currentGraphicsItem != Q_NULLPTR) {
        QPen newPen = updateBorder(Ld::DiagnosticPointer());
        currentGraphicsItem->setBorderPen(newPen);

        QString toolTip = updateToolTip(Ld::DiagnosticPointer());
        currentGraphicsItem->setToolTip(toolTip);
    }
}


void GroupingOperatorPresentationBase::processPresentationRemoved(unsigned long, Presentation* childPresentation) {
    if (childPresentation != Q_NULLPTR) {
        requestRepositioning(childPresentation);
    } else {
        requestRepositioning(this);
    }
}


void GroupingOperatorPresentationBase::processChildPresentationSet(unsigned long, Presentation* childPresentation) {
    if (childPresentation != Q_NULLPTR) {
        requestRepositioning(childPresentation);
    } else {
        requestRepositioning(this);
    }
}


QList<QRectF> GroupingOperatorPresentationBase::cursorRangeToScene(
        const Ld::ElementCursor& /* endingElementCursor */,
        const Ld::ElementCursor& /* startingElementCursor */
    ) const {
    QList<QRectF> result;

    QRectF boundingRectangle = currentGraphicsItem->boundingRect();
    QPointF topLeft          = currentGraphicsItem->mapToScene(boundingRectangle.topLeft());
    QPointF bottomRight      = currentGraphicsItem->mapToScene(boundingRectangle.bottomRight());

    result << QRectF(topLeft, bottomRight);
    return result;
}


void GroupingOperatorPresentationBase::clearChildPresentationAreas() {
    currentChildPresentationArea.clear();
}
