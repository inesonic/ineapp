/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RootOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFontMetricsF>

#include <eqt_graphics_math_group.h>
#include <eqt_graphics_math_group_with_painter_path.h>

#include <ld_format_structures.h>

#include "application.h"
#include "root_operator_presentation_base.h"
#include "root_operator_presentation.h"

RootOperatorPresentation::RootOperatorPresentation() {}


RootOperatorPresentation::~RootOperatorPresentation() {}


Ld::Visual* RootOperatorPresentation::creator(const QString&) {
    return new RootOperatorPresentation();
}


EQt::GraphicsMathGroup* RootOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                          format,
        RootOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    return updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroupWithPainterPath,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* RootOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                          format,
        EQt::GraphicsMathGroup*                    currentItem,
        RootOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    return updateGraphicsItem(
        format,
        dynamic_cast<EQt::GraphicsMathGroupWithPainterPath*>(currentItem),
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* RootOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                          format,
        EQt::GraphicsMathGroupWithPainterPath*     currentItem,
        RootOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    QFont font = operatorFont(format, relativeScale);

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child0Width          = childPresentationAreaData[0].width();
    float child0Height         = child0MaximumAscent + child0MaximumDescent;

    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();
    float child1Width          = childPresentationAreaData[1].width();
    float child1Height         = child1MaximumAscent + child1MaximumDescent;

    if (parenthesisStyle != RootOperatorPresentation::ParenthesisStyle::INVALID_OR_NONE) {
        ChildPlacementData childPlacementData = drawRootSymbol(
            Q_NULLPTR,
            format,
            relativeScale,
            QPointF(0.0F, 0.0F),
            QSizeF(child0Width, child0Height),
            QSizeF(child1Width, child1Height)
        );

        float totalHeight = childPlacementData.child0Bottom();

        EQt::GraphicsMathGroup::ParenthesisStyle
            graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

        ParenthesisData leftParenthesisData  = leftParenthesisSize(
            font,
            totalHeight,
            graphicsItemParenthesisStyle,
            false
        );

        ParenthesisData rightParenthesisData = rightParenthesisSize(
            font,
            totalHeight,
            graphicsItemParenthesisStyle,
            false
        );

        childPlacementData = drawRootSymbol(
            currentItem,
            format,
            relativeScale,
            QPointF(leftParenthesisData.width(), 0.0F),
            QSizeF(child0Width, child0Height),
            QSizeF(child1Width, child1Height)
        );

        addPresentationAreas(
            childPresentationAreaData[0],
            currentItem,
            childPlacementData.child0TopLeft(),
            child0MaximumAscent
        );

        addPresentationAreas(
            childPresentationAreaData[1],
            currentItem,
            childPlacementData.child1TopLeft(),
            child1MaximumAscent
        );

        QFontMetricsF fontMetrics(font);
        unsigned parenthesisIndex = 0;
        insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
        insertRightParenthesis(
            rightParenthesisData,
            childPlacementData.child0Right(),
            0.0F,
            currentItem,
            parenthesisIndex
        );
        currentItem->setParenthesisCenterLine(
            child0MaximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
        );

        requiredAscent = child0MaximumAscent + childPlacementData.child0Top();
        requiredSize   = QSizeF(
            childPlacementData.child0Right() + rightParenthesisData.width(),
            childPlacementData.child0Bottom()
        );
    } else {
        ChildPlacementData childPlacementData = drawRootSymbol(
            currentItem,
            format,
            relativeScale,
            QPointF(0.0F, 0.0F),
            QSizeF(child0Width, child0Height),
            QSizeF(child1Width, child1Height)
        );

        addPresentationAreas(
            childPresentationAreaData[0],
            currentItem,
            childPlacementData.child0TopLeft(),
            child0MaximumAscent
        );

        addPresentationAreas(
            childPresentationAreaData[1],
            currentItem,
            childPlacementData.child1TopLeft(),
            child1MaximumAscent
        );

        requiredAscent = child0MaximumAscent + childPlacementData.child0Top();
        requiredSize   = QSizeF(childPlacementData.child0Right(), childPlacementData.child0Bottom());
    }

    setColors(currentItem, format);

    return currentItem;
}


float RootOperatorPresentation::childRelativeScale(float parentRelativeScale, unsigned long childIndex) const {
    return childIndex == 0 ? parentRelativeScale : parentRelativeScale * superscriptSizeAdjustment;
}
