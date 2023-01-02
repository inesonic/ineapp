/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixCombineTopToBottomOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <eqt_graphics_math_group_with_line.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_matrix_combine_top_to_bottom_operator_element.h>
#include <ld_operator_format.h>

#include "application.h"
#include "scene_units.h"
#include "operator_presentation_base.h"
#include "matrix_combine_top_to_bottom_operator_presentation.h"

MatrixCombineTopToBottomOperatorPresentation::MatrixCombineTopToBottomOperatorPresentation() {}


MatrixCombineTopToBottomOperatorPresentation::~MatrixCombineTopToBottomOperatorPresentation() {}


Ld::Visual* MatrixCombineTopToBottomOperatorPresentation::creator(const QString&) {
    return new MatrixCombineTopToBottomOperatorPresentation();
}


EQt::GraphicsMathGroup* MatrixCombineTopToBottomOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                     format,
        ParenthesisStyle                      parenthesisStyle,
        float                                 relativeScale,
        const QList<PresentationAreaTracker>& childPresentationAreaData,
        QSizeF&                               requiredSize,
        float&                                requiredAscent
    ) {
    return updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroupWithLine,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* MatrixCombineTopToBottomOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                     format,
        EQt::GraphicsMathGroup*               currentItem,
        ParenthesisStyle                      parenthesisStyle,
        float                                 relativeScale,
        const QList<PresentationAreaTracker>& childPresentationAreaData,
        QSizeF&                               requiredSize,
        float&                                requiredAscent
    ) {
    Q_ASSERT(childPresentationAreaData.size() == 2);

    EQt::GraphicsMathGroupWithLine* graphicsItem = dynamic_cast<EQt::GraphicsMathGroupWithLine*>(currentItem);
    QFont                           font         = operatorFont(format, relativeScale);

    QFontMetricsF fontMetrics(font);
    float  fontAscent       = fontMetrics.ascent();
    QSizeF resolution       = sceneUnitsPerPoint();
    float  aspectRatio      = resolution.height() / resolution.width();
    float  valueSpacing     = fontMetrics.horizontalAdvance(tr("M")) * aspectRatio;
    float  parenthesisWidth = parenthesisStyle == ParenthesisStyle::INVALID_OR_NONE ? 0 : valueSpacing / 2.0;

    const PresentationAreaTracker& child0 = childPresentationAreaData.at(0);
    const PresentationAreaTracker& child1 = childPresentationAreaData.at(1);

    float child0Ascent  = child0.maximumAscent();
    float child0Descent = child0.maximumDescent();
    float child0Height  = child0Ascent + child0Descent;
    float child0Width   = child0.width();

    float child1Ascent  = child1.maximumAscent();
    float child1Descent = child1.maximumDescent();
    float child1Height  = child1Ascent + child1Descent;
    float child1Width   = child1.width();

    float maximumChildWidth = std::max(child0Width, child1Width);
    float child0x           = (maximumChildWidth - child0Width) / 2.0;
    float child1x           = (maximumChildWidth - child1Width) / 2.0;

    (void) addPresentationAreas(child0, graphicsItem, QPointF(parenthesisWidth + child0x, 0), child0Ascent);

    float lineY = child0Height + valueSpacing / 2.0;
    graphicsItem->setLine(QLineF(parenthesisWidth, lineY, maximumChildWidth + parenthesisWidth, lineY));
    graphicsItem->setLinePen(QPen(operatorColor(format)));

    (void) addPresentationAreas(
        child1,
        graphicsItem,
        QPointF(parenthesisWidth + child1x, child0Height + valueSpacing),
        child1Ascent
    );

    float totalHeight = child0Height + valueSpacing + child1Height;

    currentItem->setLeftParenthesis(
        static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle),
        QRectF(0, 0, parenthesisWidth, totalHeight)
    );

    currentItem->setRightParenthesis(
        static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle),
        QRectF(parenthesisWidth + maximumChildWidth, 0, parenthesisWidth, totalHeight)
    );

    currentItem->setParenthesisCenterLine(totalHeight / 2.0);

    setColors(currentItem, format);

    requiredSize   = QSizeF(maximumChildWidth + 2.0 * parenthesisWidth, totalHeight);
    requiredAscent = totalHeight / 2.0 + fontAscent / 4.0;

    return currentItem;
}
