/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixCombineLeftToRightOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <eqt_graphics_math_group_with_line.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_matrix_combine_left_to_right_operator_element.h>
//#include <ld_matrix_combine_top_to_bottom_operator_element.h>
#include <ld_operator_format.h>

#include "application.h"
#include "operator_presentation_base.h"
#include "matrix_combine_left_to_right_operator_presentation.h"

MatrixCombineLeftToRightOperatorPresentation::MatrixCombineLeftToRightOperatorPresentation() {}


MatrixCombineLeftToRightOperatorPresentation::~MatrixCombineLeftToRightOperatorPresentation() {}


Ld::Visual* MatrixCombineLeftToRightOperatorPresentation::creator(const QString&) {
    return new MatrixCombineLeftToRightOperatorPresentation();
}


EQt::GraphicsMathGroup* MatrixCombineLeftToRightOperatorPresentation::buildGraphicsItem(
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


EQt::GraphicsMathGroup* MatrixCombineLeftToRightOperatorPresentation::updateGraphicsItem(
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
    float fontAscent       = fontMetrics.ascent();
    float valueSpacing     = fontMetrics.horizontalAdvance(tr("M"));
    float parenthesisWidth = parenthesisStyle == ParenthesisStyle::INVALID_OR_NONE ? 0 : valueSpacing / 2.0;

    const PresentationAreaTracker& child0 = childPresentationAreaData.at(0);
    const PresentationAreaTracker& child1 = childPresentationAreaData.at(1);

    float child0Ascent  = child0.maximumAscent();
    float child0Descent = child0.maximumDescent();
    float child0Height  = child0Ascent + child0Descent;

    float child1Ascent  = child1.maximumAscent();
    float child1Descent = child1.maximumDescent();
    float child1Height  = child1Ascent + child1Descent;

    float maximumChildHeight = std::max(child0Height, child1Height);
    float child0y            = (maximumChildHeight - child0Height) / 2.0;
    float child1y            = (maximumChildHeight - child1Height) / 2.0;

    float currentX = addPresentationAreas(child0, graphicsItem, QPointF(parenthesisWidth, child0y), child0Ascent);

    float lineX = currentX + valueSpacing / 2.0;
    graphicsItem->setLine(QLineF(lineX, parenthesisWidth, lineX, maximumChildHeight - parenthesisWidth));
    graphicsItem->setLinePen(QPen(operatorColor(format)));

    currentX = addPresentationAreas(child1, graphicsItem, QPointF(currentX + valueSpacing, child1y), child1Ascent);

    currentItem->setLeftParenthesis(
        static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle),
        QRectF(0, 0, parenthesisWidth, maximumChildHeight)
    );

    currentItem->setRightParenthesis(
        static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle),
        QRectF(currentX, 0, parenthesisWidth, maximumChildHeight)
    );

    currentItem->setParenthesisCenterLine(requiredAscent - fontAscent / 4.0);

    setColors(currentItem, format);

    requiredSize   = QSizeF(currentX + parenthesisWidth, maximumChildHeight);
    requiredAscent = maximumChildHeight / 2.0 + fontAscent / 4.0;

    return currentItem;
}
