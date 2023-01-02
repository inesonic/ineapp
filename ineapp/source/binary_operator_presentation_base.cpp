/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref BinaryOperatorPresentationBase class.
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
#include "binary_operator_presentation_base.h"

BinaryOperatorPresentationBase::BinaryOperatorPresentationBase() {}


BinaryOperatorPresentationBase::~BinaryOperatorPresentationBase() {}


QString BinaryOperatorPresentationBase::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return QString(QChar(0x00BF));
}


EQt::GraphicsMathGroup* BinaryOperatorPresentationBase::buildGraphicsItem(
        Ld::FormatPointer                          format,
        OperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    return BinaryOperatorPresentationBase::updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroup,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* BinaryOperatorPresentationBase::updateGraphicsItem(
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

    // Note that the order that we insert into the GraphicsMathGroup matters as we depend on the order later.

    QString op              = operatorString(format);
    float   operatorAscent  = fontMetrics.ascent();
    float   operatorDescent = fontMetrics.descent();
    float   operatorWidth   = fontMetrics.horizontalAdvance(op);

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();

    float maximumAscent  = Util::max(child0MaximumAscent, child1MaximumAscent, operatorAscent);
    float maximumDescent = Util::max(child0MaximumDescent, child1MaximumDescent, operatorDescent);

    float maximumHeight = maximumAscent + maximumDescent;

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);

    float currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(leftParenthesisData.width(), 0),
        maximumAscent
    );

    if (operatorIndex < currentItem->numberTextEntries()) {
        EQt::GraphicsMathGroup::TextEntry& entry = currentItem->entry(operatorIndex);
        entry.setText(op);
        entry.setFont(font);
        entry.setPosition(currentX, maximumAscent);
    } else {
        currentItem->append(op, font, QPointF(currentX, maximumAscent));
    }

    currentX += operatorWidth;

    currentX = addPresentationAreas(
        childPresentationAreaData[1],
        currentItem,
        QPointF(currentX, 0),
        maximumAscent
    );

    unsigned parenthesisIndex = parenthesisStartingIndex;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentItem, parenthesisIndex);
    currentItem->setParenthesisCenterLine(
        maximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    setColors(currentItem, format);

    requiredSize   = QSizeF(currentX, maximumHeight);
    requiredAscent = maximumAscent;

    return currentItem;
}
