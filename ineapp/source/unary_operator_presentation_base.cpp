/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref UnaryOperatorPresentationBase class.
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
#include "unary_operator_presentation_base.h"

UnaryOperatorPresentationBase::UnaryOperatorPresentationBase() {}


UnaryOperatorPresentationBase::~UnaryOperatorPresentationBase() {}


bool UnaryOperatorPresentationBase::shouldPrecede() const {
    return true;
}


QString UnaryOperatorPresentationBase::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return QString(QChar(0x00BF));
}


EQt::GraphicsMathGroup* UnaryOperatorPresentationBase::buildGraphicsItem(
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


EQt::GraphicsMathGroup* UnaryOperatorPresentationBase::updateGraphicsItem(
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

    QString opString = operatorString(format);

    float operatorAscent  = fontMetrics.ascent();
    float operatorDescent = fontMetrics.descent();
    float operatorWidth   = fontMetrics.horizontalAdvance(opString);

    float childMaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float childMaximumDescent = childPresentationAreaData[0].maximumDescent();

    float maximumAscent  = Util::max(childMaximumAscent, operatorAscent);
    float maximumDescent = Util::max(childMaximumDescent, operatorDescent);

    float maximumHeight = maximumAscent + maximumDescent;

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);

    bool  operatorPrecedes = shouldPrecede();
    float currentX         = leftParenthesisData.width();

    if (!operatorPrecedes) {
        currentX = addPresentationAreas(
            childPresentationAreaData[0],
            currentItem,
            QPointF(currentX, 0.0),
            maximumAscent
        );
    }

    if (currentItem->numberTextEntries() > 0) {
        EQt::GraphicsMathGroup::TextEntry& operatorEntry = currentItem->entry(operatorIndex);
        if (operatorEntry.font() != font || operatorEntry.text() != opString) {
            operatorEntry.setFont(font);
            operatorEntry.setText(opString);
        }

        operatorEntry.setPosition(currentX, maximumAscent);
    } else {
        currentItem->append(opString, font, QPointF(currentX, maximumAscent));
    }

    currentX += operatorWidth;

    if (operatorPrecedes) {
        currentX = addPresentationAreas(
            childPresentationAreaData[0],
            currentItem,
            QPointF(currentX, 0.0),
            maximumAscent
        );
    }

    unsigned parenthesisIndex = operatorIndex + 1;
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
