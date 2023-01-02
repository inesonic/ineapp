/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref SquareRootOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QPen>
#include <QBrush>
#include <QFontMetricsF>
#include <QPainterPath>
#include <QPointF>
#include <QPolygonF>

#include <eqt_graphics_math_group.h>
#include <eqt_graphics_math_group_with_painter_path.h>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_operator_presentation_base.h"
#include "square_root_operator_presentation.h"

SquareRootOperatorPresentation::SquareRootOperatorPresentation() {}


SquareRootOperatorPresentation::~SquareRootOperatorPresentation() {}


Ld::Visual* SquareRootOperatorPresentation::creator(const QString& /* typeName */) {
    return new SquareRootOperatorPresentation;
}


EQt::GraphicsMathGroup* SquareRootOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                                format,
        SquareRootOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                            relativeScale,
        const QList<PresentationAreaTracker>&            childPresentationAreaData,
        QSizeF&                                          requiredSize,
        float&                                           requiredAscent
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


EQt::GraphicsMathGroup* SquareRootOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                                format,
        EQt::GraphicsMathGroup*                          currentItem,
        SquareRootOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                            relativeScale,
        const QList<PresentationAreaTracker>&            childPresentationAreaData,
        QSizeF&                                          requiredSize,
        float&                                           requiredAscent
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


EQt::GraphicsMathGroup* SquareRootOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                                format,
        EQt::GraphicsMathGroupWithPainterPath*           currentItem,
        SquareRootOperatorPresentation::ParenthesisStyle parenthesisStyle,
        float                                            relativeScale,
        const QList<PresentationAreaTracker>&            childPresentationAreaData,
        QSizeF&                                          requiredSize,
        float&                                           requiredAscent
    ) {
    QFont font  = operatorFont(format, relativeScale);

    float childMaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float childMaximumDescent = childPresentationAreaData[0].maximumDescent();
    float childWidth          = childPresentationAreaData[0].width();
    float childHeight         = childMaximumAscent + childMaximumDescent;

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, childHeight, graphicsItemParenthesisStyle, false);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, childHeight, graphicsItemParenthesisStyle, false);

    ChildPlacementData childPlacementData = drawRootSymbol(
        currentItem,
        format,
        relativeScale,
        QPointF(leftParenthesisData.width(), 0.0F),
        QSizeF(childWidth, childHeight)
    );

    addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        childPlacementData.child0TopLeft(),
        childMaximumAscent
    );

    float    rightContentsEdge = childPlacementData.child0Left() + childWidth;
    unsigned parenthesisIndex  = 0;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, rightContentsEdge, 0.0F, currentItem, parenthesisIndex);

    setColors(currentItem, format);

    requiredAscent = childMaximumAscent;
    requiredSize   = QSizeF(rightContentsEdge + rightParenthesisData.width(), childHeight);

    return currentItem;
}
