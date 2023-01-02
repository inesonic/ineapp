/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref UnarySuperscriptOperatorPresentationBase class.
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
#include <QBrush>

#include <util_algorithm.h>

#include <eqt_graphics_math_group.h>

#include <limits>
#include <algorithm>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_operator_format.h>

#include "application.h"
#include "presentation.h"
#include "presentation_with_fixed_children.h"
#include "scene_units.h"
#include "unary_superscript_operator_presentation_base.h"

UnarySuperscriptOperatorPresentationBase::UnarySuperscriptOperatorPresentationBase() {}


UnarySuperscriptOperatorPresentationBase::~UnarySuperscriptOperatorPresentationBase() {}


QString UnarySuperscriptOperatorPresentationBase::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return QString(QChar(0x00BF));
}


EQt::GraphicsMathGroup* UnarySuperscriptOperatorPresentationBase::buildGraphicsItem(
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


EQt::GraphicsMathGroup* UnarySuperscriptOperatorPresentationBase::updateGraphicsItem(
        Ld::FormatPointer                          format,
        EQt::GraphicsMathGroup*                    currentItem,
        OperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    QFont font            = operatorFont(format, relativeScale);
    QFont superscriptFont = font;

    int fontWeight    = superscriptFont.weight();
    int weightAdjust  = static_cast<int>((static_cast<int>(QFont::Black) - fontWeight) * superscriptWeightAdjustment);
    int newFontWeight = fontWeight + weightAdjust;
    superscriptFont.setWeight(static_cast<QFont::Weight>(newFontWeight));
    superscriptFont.setPointSizeF(superscriptFont.pointSizeF() * superscriptSizeAdjustment);

    QFontMetricsF superscriptFontMetrics(font);
    QString opString = QChar(0x200A) + operatorString(format);
    float operatorAscent  = superscriptFontMetrics.ascent();
    float operatorDescent = superscriptFontMetrics.descent();
    float operatorWidth   = superscriptFontMetrics.horizontalAdvance(opString);

    QFontMetricsF fontMetrics(font);
    float positionAdjustment = fontMetrics.height() * superscriptBaseline;
    float superscriptAscent  = fontMetrics.ascent() - positionAdjustment;

    float childY;
    float operatorBaseline;
    if (superscriptAscent < operatorAscent) {
        childY           = operatorAscent - superscriptAscent;
        operatorBaseline = operatorAscent;
    } else {
        childY           = 0;
        operatorBaseline = superscriptAscent;
    }

    float childMaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float childMaximumDescent = childPresentationAreaData[0].maximumDescent();
    float childHeight         = childMaximumAscent + childMaximumDescent;

    float maximumHeight = std::max(childHeight + childY, operatorBaseline + operatorDescent);

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);

    float currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(leftParenthesisData.width(), childY),
        childMaximumAscent
    );

    currentItem->clearText();
    currentItem->append(opString, superscriptFont, QPointF(currentX, operatorBaseline));

    currentX += operatorWidth;

    unsigned parenthesisIndex = 1;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentItem, parenthesisIndex);
    currentItem->setParenthesisCenterLine(
         childMaximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    setColors(currentItem, format);

    requiredSize   = QSizeF(currentX, maximumHeight);
    requiredAscent = childMaximumAscent + childY;

    return currentItem;
}
