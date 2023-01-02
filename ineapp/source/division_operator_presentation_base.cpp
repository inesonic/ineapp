/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DivisionOperatorPresentationBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QFontMetricsF>
#include <QPointF>
#include <QSizeF>
#include <QLineF>
#include <QPen>
#include <QBrush>
#include <QColor>

#include <eqt_graphics_math_group.h>
#include <eqt_graphics_math_group_with_line.h>

#include <ld_format_structures.h>
#include <ld_division_operator_element.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "division_operator_presentation_base.h"

DivisionOperatorPresentationBase::DivisionOperatorPresentationBase() {
    lastProcessedDivisionSymbol = Ld::DivisionOperatorFormat::DivisionSymbol::INVALID;
}


DivisionOperatorPresentationBase::~DivisionOperatorPresentationBase() {}


QList<QRectF> DivisionOperatorPresentationBase::cursorRangeToScene(
        const Ld::ElementCursor& endingElementCursor,
        const Ld::ElementCursor& startingElementCursor
    ) const {
    QList<QRectF> result;

    if (currentFractionalBoundingRectangle.isValid()) {
        QGraphicsItem* graphicsItem = DivisionOperatorPresentationBase::graphicsItem(0);
        QPointF        position     = graphicsItem->scenePos();

        result << QRectF(position, currentFractionalBoundingRectangle);
    } else {
        result = BinaryOperatorPresentationBase::cursorRangeToScene(endingElementCursor, startingElementCursor);
    }

    return result;
}


QRectF DivisionOperatorPresentationBase::imageBoundingRectangle() const {
    Ld::ElementPointer                         element        = Presentation::element();
    Ld::FormatPointer                          format         = element->format();
    Ld::DivisionOperatorFormat::DivisionSymbol divisionSymbol = divisionStyle(format);

    QRectF result;

    switch (divisionSymbol) {
        case Ld::DivisionOperatorFormat::DivisionSymbol::OBELUS: {
            result = Presentation::imageBoundingRectangle();
            break;
        }

        case Ld::DivisionOperatorFormat::DivisionSymbol::FRACTION: {
            QRectF fractionBoundingRectangle = Presentation::imageBoundingRectangle();

            const Presentation* child0Presentation = childPresentation(0);
            const Presentation* child1Presentation = childPresentation(1);

            result = fractionBoundingRectangle
                     .united(child0Presentation->imageBoundingRectangle())
                     .united(child1Presentation->imageBoundingRectangle());

            break;
        }

        case Ld::DivisionOperatorFormat::DivisionSymbol::INVALID:
        case Ld::DivisionOperatorFormat::DivisionSymbol::DEFAULT: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


void DivisionOperatorPresentationBase::processFormatChange(
        Ld::FormatPointer oldFormat,
        Ld::FormatPointer newFormat
    ) {
    clearChildPresentationAreas();
    BinaryOperatorPresentationBase::processFormatChange(oldFormat, newFormat);
}


QString DivisionOperatorPresentationBase::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr(" %1 ").arg(QChar(0x00F7));
}


EQt::GraphicsMathGroup* DivisionOperatorPresentationBase::buildGraphicsItem(
        Ld::FormatPointer                                  format,
        DivisionOperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                              relativeScale,
        const QList<PresentationAreaTracker>&              childPresentationAreaData,
        QSizeF&                                            requiredSize,
        float&                                             requiredAscent
    ) {
    EQt::GraphicsMathGroup* result;

    Ld::DivisionOperatorFormat::DivisionSymbol divisionSymbol = divisionStyle(format);
    if (divisionSymbol == Ld::DivisionOperatorFormat::DivisionSymbol::OBELUS) {
        currentFractionalBoundingRectangle = QSizeF();

        result = BinaryOperatorPresentationBase::buildGraphicsItem(
            format,
            parenthesisStyle,
            relativeScale,
            childPresentationAreaData,
            requiredSize,
            requiredAscent
        );
    } else {
        Q_ASSERT(divisionSymbol == Ld::DivisionOperatorFormat::DivisionSymbol::FRACTION);

        result = buildFractionalGraphicsItem(
            format,
            parenthesisStyle,
            relativeScale,
            childPresentationAreaData,
            requiredSize,
            requiredAscent
        );
    }

    lastProcessedDivisionSymbol = divisionSymbol;

    return result;
}


EQt::GraphicsMathGroup* DivisionOperatorPresentationBase::updateGraphicsItem(
        Ld::FormatPointer                                  format,
        EQt::GraphicsMathGroup*                            currentItem,
        DivisionOperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                              relativeScale,
        const QList<PresentationAreaTracker>&              childPresentationAreaData,
        QSizeF&                                            requiredSize,
        float&                                             requiredAscent
    ) {
    EQt::GraphicsMathGroup* result;

    Ld::DivisionOperatorFormat::DivisionSymbol divisionSymbol = divisionStyle(format);
    if (divisionSymbol != lastProcessedDivisionSymbol) {
        currentItem->deleteLater();

        result = buildGraphicsItem(
            format,
            parenthesisStyle,
            relativeScale,
            childPresentationAreaData,
            requiredSize,
            requiredAscent
        );
    } else {
        if (divisionSymbol == Ld::DivisionOperatorFormat::DivisionSymbol::OBELUS) {
            currentFractionalBoundingRectangle = QSizeF();

            result = BinaryOperatorPresentationBase::updateGraphicsItem(
                format,
                currentItem,
                parenthesisStyle,
                relativeScale,
                childPresentationAreaData,
                requiredSize,
                requiredAscent
            );
        } else {
            Q_ASSERT(divisionSymbol == Ld::DivisionOperatorFormat::DivisionSymbol::FRACTION);

            result = updateFractionalGraphicsItem(
                format,
                dynamic_cast<EQt::GraphicsMathGroupWithLine*>(currentItem),
                parenthesisStyle,
                relativeScale,
                childPresentationAreaData,
                requiredSize,
                requiredAscent
            );
        }
    }

    return result;

}


Ld::DivisionOperatorFormat::DivisionSymbol DivisionOperatorPresentationBase::divisionStyle(
        Ld::FormatPointer format
    ) const {
    Ld::DivisionOperatorFormat::DivisionSymbol result;

    if (!format.isNull() && format->capabilities().contains(Ld::DivisionOperatorFormat::formatName)) {
        result = format.dynamicCast<Ld::DivisionOperatorFormat>()->divisionSymbol();

        if (result == Ld::DivisionOperatorFormat::DivisionSymbol::DEFAULT ||
            result == Ld::DivisionOperatorFormat::DivisionSymbol::INVALID    ) {
            result = defaultDivisionSymbol();
        }
    } else {
        result = defaultDivisionSymbol();
    }

    return result;
}


EQt::GraphicsMathGroupWithLine* DivisionOperatorPresentationBase::buildFractionalGraphicsItem(
        Ld::FormatPointer                                  format,
        DivisionOperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                              relativeScale,
        const QList<PresentationAreaTracker>&              childPresentationAreaData,
        QSizeF&                                            requiredSize,
        float&                                             requiredAscent
    ) {
    EQt::GraphicsMathGroupWithLine* result = new EQt::GraphicsMathGroupWithLine;

    return updateFractionalGraphicsItem(
        format,
        result,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroupWithLine* DivisionOperatorPresentationBase::updateFractionalGraphicsItem(
        Ld::FormatPointer                                  format,
        EQt::GraphicsMathGroupWithLine*                    currentItem,
        DivisionOperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                              relativeScale,
        const QList<PresentationAreaTracker>&              childPresentationAreaData,
        QSizeF&                                            requiredSize,
        float&                                             requiredAscent
    ) {
    QFont font  = operatorFont(format, relativeScale);
    QFontMetricsF fontMetrics(font);

    // TODO: Rendering might be improved by setting this value to a small value with some elements for child 1.
    float child1AscentAdjust   = 0; // fontMetrics.ascent() - fontMetrics.capHeight() - fontMetrics.descent();

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent() - child1AscentAdjust;
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();
    float child0Width          = childPresentationAreaData[0].width();
    float child1Width          = childPresentationAreaData[1].width();

    float child0Height         = child0MaximumAscent + child0MaximumDescent;
    float child1Height         = child1MaximumAscent + child1MaximumDescent;

    float lineWidthSceneUnits  = toScene(QSizeF(0.0F, 1.0F)).height();

    float maximumChildWidth    = std::max(child0Width, child1Width);
    float totalHeight          = child0Height + 3.0F * lineWidthSceneUnits + child1Height;

    float linePadding        = std::min(fontMetrics.height() / 2.0F, 0.2 * maximumChildWidth);
    float operatorFontAscent = fontMetrics.ascent();

    float centerLine = child0Height + 1.5F * lineWidthSceneUnits;

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, totalHeight, graphicsItemParenthesisStyle, false);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, totalHeight, graphicsItemParenthesisStyle, false);

    float leftParenthesisWidth = leftParenthesisData.width();
    QPointF child0TopLeft(
        leftParenthesisWidth + linePadding + (maximumChildWidth - child0Width) / 2.0F,
        0.0F
    );
    QPointF child1TopLeft(
        leftParenthesisWidth + linePadding + (maximumChildWidth - child1Width) / 2.0F,
        child0Height + 3.0 * lineWidthSceneUnits
    );

    addPresentationAreas(childPresentationAreaData[0], currentItem, child0TopLeft, child0MaximumAscent);
    addPresentationAreas(childPresentationAreaData[1], currentItem, child1TopLeft, child1MaximumAscent);

    float lineY = child0Height + 1.5F * lineWidthSceneUnits;

    currentItem->setLinePen(QPen(QBrush(operatorColor(format)), lineWidthSceneUnits, Qt::SolidLine, Qt::RoundCap));
    currentItem->setLine(
        leftParenthesisWidth,
        lineY,
        leftParenthesisWidth + 2.0F * linePadding + maximumChildWidth,
        lineY
    );

    float    rightContentsEdge = leftParenthesisWidth + 2.0F * linePadding + maximumChildWidth;
    unsigned parenthesisIndex  = 0;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, rightContentsEdge, 0.0F, currentItem, parenthesisIndex);

    setColors(currentItem, format);

    requiredAscent = centerLine + operatorFontAscent / 4.0F;
    requiredSize   = QSizeF(rightContentsEdge + rightParenthesisData.width(), totalHeight);

    currentFractionalBoundingRectangle = requiredSize;

    return currentItem;
}
