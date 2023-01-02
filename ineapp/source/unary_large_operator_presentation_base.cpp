/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref UnaryLargeOperatorPresentationBase class.
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
#include "unary_large_operator_presentation_base.h"

UnaryLargeOperatorPresentationBase::UnaryLargeOperatorPresentationBase() {}


UnaryLargeOperatorPresentationBase::~UnaryLargeOperatorPresentationBase() {}


QString UnaryLargeOperatorPresentationBase::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return QString(QChar(0x00BF));
}


EQt::GraphicsMathGroup* UnaryLargeOperatorPresentationBase::buildGraphicsItem(
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


EQt::GraphicsMathGroup* UnaryLargeOperatorPresentationBase::updateGraphicsItem(
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
    float fontHeight = fontMetrics.height();

    QString opString = operatorString(format);

    float childMaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float childMaximumDescent = childPresentationAreaData[0].maximumDescent();
    float childHeight         = childMaximumAscent + childMaximumDescent;

    float maximumHeight = Util::max(childHeight, fontHeight);

    QFont scaledFont    = font;
    QChar lastCharacter = opString.at(opString.length() - 1);
    float operatorHeight;
    float operatorAscent;
    float operatorWidth;
    if (maximumHeight > fontHeight) {
        scaledFont.setPointSizeF(font.pointSizeF() * maximumHeight / fontHeight);
        QFontMetricsF scaledFontMetrics(scaledFont);
        operatorHeight = scaledFontMetrics.height();
        operatorAscent = scaledFontMetrics.ascent();
        operatorWidth  = scaledFontMetrics.horizontalAdvance(opString) + scaledFontMetrics.rightBearing(lastCharacter);
    } else {
        operatorHeight = fontHeight;
        operatorAscent = fontMetrics.ascent();
        operatorWidth  = fontMetrics.horizontalAdvance(opString) + fontMetrics.rightBearing(lastCharacter);
    }

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);

    float currentX = leftParenthesisData.width();

    if (currentItem->numberTextEntries() == 0) {
        currentItem->append(opString, scaledFont, QPointF(currentX, operatorAscent));
    } else {
        EQt::GraphicsMathGroup::TextEntry& entry = currentItem->entry(operatorIndex);
        entry.setText(opString);
        entry.setFont(scaledFont);
        entry.setPosition(currentX, operatorAscent);
    }

    currentX += operatorWidth;

    float childTop = maximumHeight - childHeight;
    currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(currentX, childTop),
        childMaximumAscent + childTop
    );

    unsigned parenthesisIndex = operatorIndex + 1;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentItem, parenthesisIndex);
    currentItem->setParenthesisCenterLine(
         childMaximumAscent + childTop - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    setColors(currentItem, format);

    requiredSize   = QSizeF(currentX, Util::max(operatorHeight, childHeight));
    requiredAscent = childMaximumAscent + childTop;

    return currentItem;
}
