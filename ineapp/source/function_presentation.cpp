/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FunctionPresentation class.
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
#include <limits>
#include <cmath>

#include <util_units.h>
#include <util_string.h>
#include <util_algorithm.h>

#include <eqt_graphics_math_group.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>
#include <ld_list_element_base.h>
#include <ld_visual_with_positional_children.h>
#include <ld_function_placeholder_element.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_operator_format.h>
#include <ld_diagnostic.h>
#include <ld_element_cursor.h>
#include <ld_function_format.h>
#include <ld_function_element.h>

#include "application.h"
#include "placement_negotiator.h"
#include "placement_tracker.h"
#include "presentation_with_positional_children.h"
#include "presentation_area_tracker.h"
#include "function_presentation.h"

FunctionPresentation::FunctionPresentation() {
    currentGraphicsItem   = Q_NULLPTR;
    currentChildContainer = Q_NULLPTR;
}


FunctionPresentation::~FunctionPresentation() {
    removeFromScene();
}


Ld::Visual* FunctionPresentation::creator(const QString& /* typeName */) {
    return new FunctionPresentation;
}


void FunctionPresentation::requestRepositioning(Presentation* /* childPresentation */) {
    Ld::ElementPointer element = FunctionPresentation::element();
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


void FunctionPresentation::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                minimumTopSpacing,
        float                /* lineSpacing */,
        float                relativeScale
    ) {
    QSharedPointer<Ld::FunctionElement> functionElement = element().dynamicCast<Ld::FunctionElement>();
    Q_ASSERT(!functionElement.isNull());

    QFont  font;
    QColor color;
    QColor backgroundColor;
    bool   hasSubscriptedParameter;

    Ld::FormatPointer format = functionElement->format();
    if (!format.isNull() && format->capabilities().contains(Ld::FunctionFormat::formatName)) {
        QSharedPointer<Ld::FunctionFormat> functionFormat = format.dynamicCast<Ld::FunctionFormat>();
        Q_ASSERT(!functionFormat.isNull());

        font                    = functionFormat->toQFont();
        color                   = functionFormat->fontColor();
        backgroundColor         = functionFormat->fontBackgroundColor();
        hasSubscriptedParameter = functionFormat->subscriptedParameter();
    } else {
        QSharedPointer<Ld::CharacterFormat>
            defaultFontFormat = Ld::CharacterFormat::applicationDefaultMathIdentifierFont();

        font                    = defaultFontFormat->toQFont();
        color                   = defaultFontFormat->fontColor();
        backgroundColor         = defaultFontFormat->fontBackgroundColor();
        hasSubscriptedParameter = false;
    }

    populatePresentationAreaTrackers(
        placementTracker,
        minimumTopSpacing,
        relativeScale,
        hasSubscriptedParameter ? relativeScale * subscriptSizeAdjustment : relativeScale
    );

    QFont subscriptFont = font;

    float fontPointSize            = font.pointSizeF();
    float fontScaleFactor          = Application::fontScaleFactor() * relativeScale;
    float subscriptFontScaleFactor = fontScaleFactor * subscriptSizeAdjustment;

    if (fontScaleFactor != 1.0F) {
        font.setPointSizeF(fontPointSize * fontScaleFactor);
    }

    if (subscriptFontScaleFactor != 1.0F) {
        subscriptFont.setPointSizeF(fontPointSize * subscriptFontScaleFactor);
    }

    QString functionName      = functionElement->text(0);
    QString functionSubscript = functionElement->text(1);

    if (functionName.isEmpty()) {
        functionName = tr("f");
        color        = QColor(Qt::gray);
        font.setItalic(true);
    }

    if (!functionSubscript.isEmpty() && hasSubscriptedParameter) {
        functionSubscript += tr(",");
    }

    QFontMetricsF fontMetrics(font);
    float functionNameWidth   = fontMetrics.horizontalAdvance(functionName);
    float functionNameHeight  = fontMetrics.height();
    float functionNameAscent  = fontMetrics.ascent();
    float functionNameDescent = fontMetrics.descent();

    QFontMetricsF subscriptFontMetrics(subscriptFont);

    float functionSubscriptWidth              = subscriptFontMetrics.horizontalAdvance(functionSubscript);
    float functionSubscriptPositionAdjustment = functionNameHeight * subscriptBaseline;
    float subscriptAscent                     = subscriptFontMetrics.ascent();
    float subscriptDescent                    = subscriptFontMetrics.descent();

    float         maximumParameterAscent      = functionNameAscent;
    float         maximumParameterDescent     = functionNameDescent;
    unsigned long numberPresentationAreas = static_cast<unsigned long>(childPresentationAreas.size());

    unsigned long firstChildPresentationArea = 0;

    if (hasSubscriptedParameter && numberPresentationAreas > 0) {
        firstChildPresentationArea = 1;
    }

    for (unsigned long areaIndex=firstChildPresentationArea ; areaIndex<numberPresentationAreas ; ++areaIndex) {
        const PresentationAreaTracker& area = childPresentationAreas.at(areaIndex);
        float childAscent  = area.maximumAscent();
        float childDescent = area.maximumDescent();

        if (childAscent > maximumParameterAscent) {
            maximumParameterAscent = childAscent;
        }

        if (childDescent > maximumParameterDescent) {
            maximumParameterDescent = childDescent;
        }
    }

    float maximumAscent              = maximumParameterAscent;
    float maximumDescent             = maximumParameterDescent;
    float parametersHeight           = maximumParameterAscent + maximumParameterDescent;
    float baseline                   = maximumAscent;
    float subscriptBaseline          = baseline - functionSubscriptPositionAdjustment;
    float subscriptTop               = subscriptBaseline - subscriptAscent;
    float subscriptTextBottom        = subscriptBaseline + subscriptDescent;
    float subscriptParameterBaseline = 0;
    float subscriptParameterAscent   = 0;
    float subscriptParameterDescent  = 0;

    if (hasSubscriptedParameter && numberPresentationAreas > 0) {
        const PresentationAreaTracker& area = childPresentationAreas.first();
        subscriptParameterAscent   = area.maximumAscent();
        subscriptParameterDescent  = area.maximumDescent();
        subscriptParameterBaseline = subscriptTop + subscriptParameterAscent;

        float subscriptParameterBottom   = subscriptParameterBaseline + subscriptParameterDescent;
        maximumDescent = Util::max(
            maximumDescent,
            subscriptTextBottom - baseline,
            subscriptParameterBottom - baseline
        );
    } else {
        if (!functionSubscript.isEmpty()) {
            maximumDescent = std::max(maximumDescent, subscriptTextBottom - baseline);
        }
    }

    float functionHeight = maximumAscent + maximumDescent;

    EQt::GraphicsMathGroup::ParenthesisStyle
        functionParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(
            functionElement->parenthesisStyle()
        );

    ParenthesisData leftFunctionParenthesisData  = leftParenthesisSize(
        font,
        functionHeight,
        functionParenthesisStyle
    );
    ParenthesisData rightFunctionParenthesisData = rightParenthesisSize(
        font,
        functionHeight,
        functionParenthesisStyle
    );

    EQt::GraphicsMathGroup::ParenthesisStyle
        parameterParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(
            functionElement->parameterParenthesisStyle()
        );

    ParenthesisData leftParameterParenthesisData  = leftParenthesisSize(
        font,
        parametersHeight,
        parameterParenthesisStyle
    );
    ParenthesisData rightParameterParenthesisData = rightParenthesisSize(
        font,
        parametersHeight,
        parameterParenthesisStyle
    );

    if (currentGraphicsItem == Q_NULLPTR) {
        currentGraphicsItem = new EQt::GraphicsMathGroup;
    } else {
        currentGraphicsItem->clearText();
    }

    float currentFunctionX = leftFunctionParenthesisData.width();
    currentGraphicsItem->append(functionName, font, QPointF(currentFunctionX, baseline));
    currentFunctionX += functionNameWidth;

    currentGraphicsItem->append(functionSubscript, subscriptFont, QPointF(currentFunctionX, subscriptBaseline));
    currentFunctionX += functionSubscriptWidth;

    if (hasSubscriptedParameter) {
        currentFunctionX = addPresentationArea(currentGraphicsItem, 0, currentFunctionX, subscriptParameterBaseline);
    }

    currentFunctionX += fontMetrics.horizontalAdvance(tr("%1").arg(QChar(0x2009)));

    if (currentChildContainer == Q_NULLPTR) {
        currentChildContainer = new EQt::GraphicsMathGroup;
    } else {
        currentChildContainer->clearText();
    }

    currentGraphicsItem->addToGroup(currentChildContainer);
    currentChildContainer->setPos(currentFunctionX, baseline - maximumAscent);

    float         currentParameterX = leftParameterParenthesisData.width();
    unsigned      parenthesisIndex  = 0;
    unsigned long numberChildren    = static_cast<unsigned long>(functionElement->numberChildren());

    QString separator      = tr(", ");
    float   separatorWidth = fontMetrics.horizontalAdvance(separator);
    bool    firstChild     = true;

    for (unsigned childIndex=firstChildPresentationArea ; childIndex<numberChildren ; ++childIndex) {
        if (!firstChild) {
            Ld::ElementPointer thisChild = functionElement->child(childIndex);
            if (!thisChild.isNull()                                                                    &&
                (thisChild->typeName() != Ld::FunctionPlaceholderElement::elementName             ||
                 !thisChild.dynamicCast<Ld::FunctionPlaceholderElement>()->fieldString().isEmpty()   )    ) {
                currentChildContainer->append(separator, font, QPointF(currentParameterX, maximumAscent));
                currentParameterX += separatorWidth;
                ++parenthesisIndex;
            }
        } else {
            firstChild = false;
        }

        currentParameterX = addPresentationArea(
            currentChildContainer,
            childIndex,
            currentParameterX,
            maximumAscent
        );
    }

    insertLeftParenthesis(leftParameterParenthesisData, 0, 0, currentChildContainer, parenthesisIndex);
    insertRightParenthesis(
        rightParameterParenthesisData,
        currentParameterX,
        0,
        currentChildContainer,
        parenthesisIndex
    );

    currentParameterX += rightParameterParenthesisData.width();
    currentFunctionX += currentParameterX;

    parenthesisIndex = 2;
    insertLeftParenthesis(leftFunctionParenthesisData, 0, 0, currentGraphicsItem, parenthesisIndex);
    insertRightParenthesis(rightFunctionParenthesisData, currentFunctionX, 0, currentGraphicsItem, parenthesisIndex);
    currentFunctionX += rightFunctionParenthesisData.width();

    if (!color.isValid()) {
        color = QColor(Qt::black);
    }

    if (!backgroundColor.isValid()) {
        backgroundColor = QColor(255, 255, 255, 0);
    }

    currentGraphicsItem->setTextPen(QPen(color));
    currentGraphicsItem->setBackgroundBrush(QBrush(backgroundColor));

    requiredAscent = baseline;
    allocateSpaceForThisPresentation(
        parent,
        childIdentifier,
        QSize(currentFunctionX, functionHeight),
        requiredAscent
    );
}


void FunctionPresentation::redoPlacement(
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


QSizeF FunctionPresentation::requestArea(
        unsigned long                         /* childIdentifier */,
        FunctionPresentation::SpaceQualifier* spaceQualifier
    ) {
    if (spaceQualifier != Q_NULLPTR) {
        // All operators use the provided space.
        *spaceQualifier = SpaceQualifier::USE_PROVIDED;
    }

    return maximumPossibleSpace;
}


void FunctionPresentation::allocateArea(
        unsigned long childIdentifier,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         ascent,
        bool          /* canStretch */
    ) {
    QSharedPointer<Ld::FunctionElement> functionElement = element().dynamicCast<Ld::FunctionElement>();

    Ld::ElementPointer childElement      = functionElement->child(childIdentifier);
    Presentation*      childPresentation = dynamic_cast<Presentation*>(childElement->visual());

    if (childIdentifier < static_cast<unsigned long>(childPresentationAreas.size())) {
        PresentationAreaTracker& tracker = childPresentationAreas[childIdentifier];
        tracker.allocateArea(presentationAreaId, size, ascent);
    } else {
        PresentationAreaTracker tracker;

        tracker.setChildPresentation(childPresentation);
        tracker.allocateArea(presentationAreaId, size, ascent);

        childPresentationAreas.append(tracker);
    }
}


void FunctionPresentation::areaInsufficient(unsigned long /* childIdentifier */, const QSizeF& /* size */) {
    Q_ASSERT(false); // We should never call this method.
}


void FunctionPresentation::applyStretch(unsigned long /* presentationAreaId */, float /* stretchFactor */) {
    Q_ASSERT(false); // We should never receive a call to this method.
}


QGraphicsItem* FunctionPresentation::graphicsItem(unsigned long presentationAreaId) const {
    return presentationAreaId == 0 ? currentGraphicsItem : Q_NULLPTR;
}


void FunctionPresentation::resetPlacement() {
    PresentationWithPositionalChildren::resetPlacement();
    removeFromScene();
}


void FunctionPresentation::removeFromScene() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->deleteLater();
        currentGraphicsItem = Q_NULLPTR;
    }

    if (currentChildContainer != Q_NULLPTR) {
        currentChildContainer->deleteLater();
        currentChildContainer = Q_NULLPTR;
    }
}


QList<QRectF> FunctionPresentation::cursorRangeToScene(
        const Ld::ElementCursor& endingElementCursor,
        const Ld::ElementCursor& startingElementCursor
    ) const {
    QList<QRectF> result;

    if (currentGraphicsItem != Q_NULLPTR) {
        if (endingElementCursor.isValid()) {
            if (startingElementCursor.isValid()) {
                unsigned      startingRegionIndex = 0;
                unsigned long startingTextIndex   = 0;
                unsigned      endingRegionIndex;
                unsigned long endingTextIndex;

                Ld::ElementPointer element = FunctionPresentation::element();
                if (startingElementCursor.element() == element && !startingElementCursor.isWholeElement()) {
                    startingRegionIndex = startingElementCursor.regionIndex();
                    startingTextIndex   = startingElementCursor.textIndex();

                    if (startingRegionIndex > 1) {
                        startingRegionIndex = 1;
                    }

                    unsigned long textLength = static_cast<unsigned long>(element->text(startingRegionIndex).length());
                    if (startingTextIndex > textLength) {
                        startingTextIndex = textLength;
                    }
                }

                if (endingElementCursor.element() == element && !endingElementCursor.isWholeElement()) {
                    endingRegionIndex = endingElementCursor.regionIndex();
                    endingTextIndex   = endingElementCursor.textIndex();

                    if (endingRegionIndex > 1) {
                        endingRegionIndex = 1;
                    }

                    unsigned long textLength = static_cast<unsigned long>(element->text(startingRegionIndex).length());
                    if (startingTextIndex > textLength) {
                        startingTextIndex = textLength;
                    }
                } else {
                    endingRegionIndex = 1;
                    endingTextIndex   = element->text(endingRegionIndex).length();
                }

                if (startingRegionIndex == endingRegionIndex) {
                    QRectF starting = cursorToScene(startingTextIndex, startingRegionIndex);
                    QRectF ending   = cursorToScene(endingTextIndex, endingRegionIndex);

                    result << QRectF(starting.topLeft(), ending.bottomRight());
                } else {
                    QRectF starting1 = cursorToScene(startingTextIndex, startingRegionIndex);
                    QRectF ending1   = cursorToScene(
                        static_cast<unsigned long>(element->text(startingRegionIndex).length()),
                        startingRegionIndex
                    );

                    QRectF starting2 = cursorToScene(0, endingRegionIndex);
                    QRectF ending2   = cursorToScene(endingTextIndex, endingRegionIndex);

                    result << QRectF(starting1.topLeft(), ending1.bottomRight())
                           << QRectF(starting2.topLeft(), ending2.bottomRight());
                }
            } else {
                if (endingElementCursor.isWholeElement()) {
                    result = sceneBoundingRectangles();
                } else {
                    unsigned      regionIndex = endingElementCursor.regionIndex();
                    unsigned long textIndex   = endingElementCursor.textIndex();
                    result << cursorToScene(textIndex, regionIndex);
                }
            }
        }
    }

    return result;
}


void FunctionPresentation::populatePresentationAreaTrackers(
        PlacementTracker* placementTracker,
        float             minimumTopSpacing,
        float             relativeScale,
        float             firstParameterRelativeScale
    ) {
    childPresentationAreas.clear();

    Ld::ElementPointer element = FunctionPresentation::element();
    placementTracker->addNewJobs(element->numberChildren());

    unsigned long numberChildren       = element->numberChildren();
    float         currentRelativeScale = firstParameterRelativeScale;
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
            currentRelativeScale
        );

        currentRelativeScale = relativeScale;
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
            currentRelativeScale
        );
    }

    placementTracker->completedJobs(element->numberChildren());
}


float FunctionPresentation::addPresentationArea(
        EQt::GraphicsMathGroup* graphicsItem,
        unsigned long           presentationAreaIndex,
        float                   leftEdge,
        float                   baseLine
    ) {
    float currentX = leftEdge;

    const PresentationAreaTracker& presentationAreas       = childPresentationAreas.at(presentationAreaIndex);
    unsigned long                  numberPresentationAreas = presentationAreas.numberPresentationAreas();

    for (unsigned long areaIndex=0 ; areaIndex<numberPresentationAreas ; ++areaIndex) {
        const PresentationAreaTracker::Entry& entry             = presentationAreas.entry(areaIndex);
        QGraphicsItem*                        childGraphicsItem = entry.graphicsItem();
        float                                 ascent            = entry.ascent();

        graphicsItem->addToGroup(childGraphicsItem);
        childGraphicsItem->setPos(currentX, baseLine - ascent);

        currentX += entry.areaSize().width();
    }

    return currentX;
}


void FunctionPresentation::allocateSpaceForThisPresentation(
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


unsigned FunctionPresentation::numberDisplayedTextRegions() const {
    return 2;
}


QPair<QString, QFont> FunctionPresentation::textAtDisplayedRegion(unsigned regionIndex) const {
    QPair<QString, QFont> result;

    if (currentGraphicsItem != Q_NULLPTR) {
        Q_ASSERT(regionIndex < numberDisplayedTextRegions());
        const EQt::GraphicsMathGroup::TextEntry& entry = currentGraphicsItem->entry(regionIndex);
        result.first  = entry.text();
        result.second = entry.font();
    }

    return result;
}


QPointF FunctionPresentation::displayedTextPosition(unsigned regionIndex) const {
    QPointF result;


    if (currentGraphicsItem != Q_NULLPTR) {
        Q_ASSERT(regionIndex < numberDisplayedTextRegions());
        const EQt::GraphicsMathGroup::TextEntry& entry = currentGraphicsItem->entry(regionIndex);
        result = currentGraphicsItem->mapToScene(entry.position());
    }

    return result;
}


Ld::ElementCursor FunctionPresentation::convertToElementCursor(
        unsigned long displayedTextOffset,
        unsigned      displayedRegionIndex
    ) const {
    return Ld::ElementCursor(displayedTextOffset, displayedRegionIndex, element());
}


void FunctionPresentation::processDataChange() {
    requestRepositioning(this);
}


void FunctionPresentation::processDiagnostic(Ld::DiagnosticPointer diagnostic) {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder(diagnostic));
        currentGraphicsItem->setToolTip(updateToolTip(diagnostic));
    }
}


void FunctionPresentation::clearDiagnosticDisplay() {
    if (currentGraphicsItem != Q_NULLPTR) {
        currentGraphicsItem->setBorderPen(updateBorder());
        currentGraphicsItem->setToolTip(updateToolTip());
    }
}


void FunctionPresentation::processRemovingChildPresentation(unsigned long, Presentation*) {
    if (!element().isNull()) {
        requestRepositioning(Q_NULLPTR);
    }
}


void FunctionPresentation::processChildPresentationInsertedBefore(unsigned long, Presentation* childPresentation) {
    requestRepositioning(childPresentation);
}


void FunctionPresentation::processChildPresentationInsertedAfter(unsigned long, Presentation* childPresentation) {
    requestRepositioning(childPresentation);
}


QList<QRectF> FunctionPresentation::sceneBoundingRectangles() const {
    QList<QRectF> result;

    if (currentGraphicsItem != Q_NULLPTR) {
        QPointF relativeBottomRight = currentGraphicsItem->boundingRect().bottomRight();

        QPointF sceneTopLeft = currentGraphicsItem->mapToScene(0, 0);
        result << QRectF(sceneTopLeft, QSizeF(relativeBottomRight.x(), relativeBottomRight.y()));
    }

    return result;
}


QRectF FunctionPresentation::cursorToScene(unsigned long textIndex, unsigned regionIndex) const {
    const EQt::GraphicsMathGroup::TextEntry& entry         = currentGraphicsItem->entry(regionIndex);
    QString                                  entryText     = entry.text().left(textIndex);
    QPointF                                  entryPosition = entry.position();

    QFontMetricsF entryFontMetrics(entry.font());
    float         cursorXOffset = entryFontMetrics.horizontalAdvance(entryText);
    float         cursorAscent  = entryFontMetrics.ascent();
    float         cursorHeight  = entryFontMetrics.height();

    QPointF cursorRelativeTopPosition = QPointF(
        entryPosition.x() + cursorXOffset,
        entryPosition.y() - cursorAscent
    );
    QPointF cursorAbsoluteTopPosition = currentGraphicsItem->mapToScene(cursorRelativeTopPosition);

    return QRectF(cursorAbsoluteTopPosition, QSizeF(0, cursorHeight));
}
