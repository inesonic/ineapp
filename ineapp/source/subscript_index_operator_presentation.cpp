/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SubscriptIndexOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFontMetricsF>

#include <ld_format_structures.h>
#include <ld_variable_element.h>
#include <ld_subscript_index_operator_element.h>
#include <ld_subscript_row_column_operator_element.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "subscript_index_operator_presentation.h"

SubscriptIndexOperatorPresentation::SubscriptIndexOperatorPresentation() {}


SubscriptIndexOperatorPresentation::~SubscriptIndexOperatorPresentation() {}


Ld::Visual* SubscriptIndexOperatorPresentation::creator(const QString&) {
    return new SubscriptIndexOperatorPresentation();
}


EQt::GraphicsMathGroup* SubscriptIndexOperatorPresentation::buildGraphicsItem(
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


EQt::GraphicsMathGroup* SubscriptIndexOperatorPresentation::updateGraphicsItem(
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

    Ld::ElementPointer child0       = element()->child(0);
    bool               includeComma = false;

    if (!child0.isNull()) {
        QString typeName = child0->typeName();
        includeComma = (
               (typeName == Ld::VariableElement::elementName && !child0->text(1).isEmpty())
            || typeName == Ld::SubscriptIndexOperatorElement::elementName
            || typeName == Ld::SubscriptRowColumnOperatorElement::elementName
        );
    }

    currentItem->clearText();

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child0Height         = child0MaximumAscent + child0MaximumDescent;

    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();
    float child1Height         = child1MaximumAscent + child1MaximumDescent;

    float child1BaselineAdjustment = child0Height * subscriptBaseline;

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

    unsigned textFieldIndex;
    if (includeComma) {
        QFont commaFont = font;
        commaFont.setPointSizeF(font.pointSizeF() * subscriptSizeAdjustment);
        QFontMetricsF commaFontMetrics(commaFont);

        QString commaText = tr(",");
        currentItem->append(commaText, commaFont, QPointF(currentX, child1Top + child1MaximumAscent));

        currentX += commaFontMetrics.horizontalAdvance(commaText);
        textFieldIndex = 1;
    } else {
        currentX += fontMetrics.horizontalAdvance(" ") / 4.0;
        textFieldIndex = 0;
    }

    currentX = addPresentationAreas(
        childPresentationAreaData[1],
        currentItem,
        QPointF(currentX, child1Top),
        child1MaximumAscent
    );

    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, textFieldIndex);
    insertRightParenthesis(rightParenthesisData, currentX, 0.0F, currentItem, textFieldIndex);
    currentItem->setParenthesisCenterLine(
        child0MaximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    setColors(currentItem, format);

    requiredAscent = child0Top + child0MaximumAscent;
    requiredSize   = QSizeF(currentX, maximumContentsHeight);

    return currentItem;
}


float SubscriptIndexOperatorPresentation::childRelativeScale(
        float         parentRelativeScale,
        unsigned long childIndex
    ) const {
    return childIndex == 0 ? parentRelativeScale : parentRelativeScale * subscriptSizeAdjustment;
}
