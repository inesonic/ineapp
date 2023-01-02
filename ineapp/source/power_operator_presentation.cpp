/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PowerOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFontMetricsF>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "power_operator_presentation.h"

PowerOperatorPresentation::PowerOperatorPresentation() {}


PowerOperatorPresentation::~PowerOperatorPresentation() {}


Ld::Visual* PowerOperatorPresentation::creator(const QString&) {
    return new PowerOperatorPresentation();
}


EQt::GraphicsMathGroup* PowerOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                          format,
        OperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    return updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroup,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* PowerOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                          format,
        EQt::GraphicsMathGroup*                    currentItem,
        OperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    QFont font = operatorFont(format, relativeScale);
    QFontMetricsF fontMetrics(font);

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child0Height         = child0MaximumAscent + child0MaximumDescent;

    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();
    float child1Height         = child1MaximumAscent + child1MaximumDescent;

    float child1BaselineAdjustment = child0Height * superscriptBaseline;

    float child0Top;
    float child1Top;
    if (child1MaximumAscent + child1BaselineAdjustment < child0MaximumAscent) {
        child0Top = 0;
        child1Top = child0MaximumAscent - child1BaselineAdjustment - child1MaximumAscent;
    } else {
        child1Top = 0;
        child0Top = child1MaximumAscent + child1BaselineAdjustment - child0MaximumAscent;
    }

    float maximumContentsHeight = std::max(child0Top + child0Height, child1Top + child1Height);

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData = leftParenthesisSize(
        font,
        maximumContentsHeight,
        graphicsItemParenthesisStyle
    );

    ParenthesisData rightParenthesisData = rightParenthesisSize(
        font,
        maximumContentsHeight,
        graphicsItemParenthesisStyle
    );

    float currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(leftParenthesisData.width(), child0Top),
        child0MaximumAscent
    );

    currentX += fontMetrics.horizontalAdvance(" ") / 4.0;

    currentX = addPresentationAreas(
        childPresentationAreaData[1],
        currentItem,
        QPointF(currentX, child1Top),
        child1MaximumAscent
    );

    unsigned parenthesisIndex = 0;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentItem, parenthesisIndex);
    currentItem->setParenthesisCenterLine(
        child0MaximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    setColors(currentItem, format);

    requiredAscent = child0Top + child0MaximumAscent;
    requiredSize   = QSizeF(currentX, maximumContentsHeight);

    return currentItem;
}


float PowerOperatorPresentation::childRelativeScale(float parentRelativeScale, unsigned long childIndex) const {
    return childIndex == 0 ? parentRelativeScale : parentRelativeScale * superscriptSizeAdjustment;
}
