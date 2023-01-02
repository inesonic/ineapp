/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SubscriptRowColumnOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFontMetricsF>

#include <util_algorithm.h>

#include <ld_format_structures.h>
#include <ld_variable_element.h>
#include <ld_subscript_index_operator_element.h>
#include <ld_subscript_row_column_operator_element.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "subscript_row_column_operator_presentation.h"

SubscriptRowColumnOperatorPresentation::SubscriptRowColumnOperatorPresentation() {}


SubscriptRowColumnOperatorPresentation::~SubscriptRowColumnOperatorPresentation() {}


Ld::Visual* SubscriptRowColumnOperatorPresentation::creator(const QString&) {
    return new SubscriptRowColumnOperatorPresentation();
}


EQt::GraphicsMathGroup* SubscriptRowColumnOperatorPresentation::buildGraphicsItem(
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


EQt::GraphicsMathGroup* SubscriptRowColumnOperatorPresentation::updateGraphicsItem(
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

    QString commaString = tr(",");

    QFont subscriptFont = font;
    subscriptFont.setPointSizeF(font.pointSizeF() * subscriptSizeAdjustment);
    subscriptFont.setWeight(static_cast<QFont::Weight>(font.weight() * subscriptWeightAdjustment));

    QFontMetricsF subscriptFontMetrics(subscriptFont);

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child0Height         = child0MaximumAscent + child0MaximumDescent;

    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();

    float child2MaximumAscent  = childPresentationAreaData[2].maximumAscent();
    float child2MaximumDescent = childPresentationAreaData[2].maximumDescent();

    float subscriptCommaAscent  = subscriptFontMetrics.ascent();
    float subscriptCommaDescent = subscriptFontMetrics.descent();
    float subscriptCommaWidth   = subscriptFontMetrics.horizontalAdvance(commaString);

    float subscriptMaximumAscent  = Util::max(child1MaximumAscent, child2MaximumAscent, subscriptCommaAscent);
    float subscriptMaximumDescent = Util::max(child1MaximumDescent, child2MaximumDescent, subscriptCommaDescent);
    float subscriptHeight         = subscriptMaximumAscent + subscriptMaximumDescent;

    float subscriptBaselineAdjustment = child0Height * subscriptBaseline;

    float child0Top;
    float subscriptTop;
    if (subscriptMaximumAscent + subscriptBaselineAdjustment < child0MaximumAscent) {
        child0Top    = 0;
        subscriptTop = child0MaximumAscent - subscriptBaselineAdjustment - subscriptMaximumAscent;
    } else {
        subscriptTop = 0;
        child0Top    = subscriptMaximumAscent + subscriptBaselineAdjustment - child0MaximumAscent;
    }

    float maximumContentsHeight = std::max(child0Top + child0Height, subscriptTop + subscriptHeight);

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
        currentItem->append(commaString, subscriptFont, QPointF(currentX, subscriptTop + subscriptMaximumAscent));

        currentX += subscriptCommaWidth;
        textFieldIndex = 1;
    } else {
        currentX += fontMetrics.horizontalAdvance(" ") / 4.0;
        textFieldIndex = 0;
    }

    currentX = addPresentationAreas(
        childPresentationAreaData[1],
        currentItem,
        QPointF(currentX, subscriptTop),
        subscriptMaximumAscent
    );

    currentItem->append(commaString, subscriptFont, QPointF(currentX, subscriptTop + subscriptMaximumAscent));
    currentX += subscriptCommaWidth;
    ++textFieldIndex;

    currentX = addPresentationAreas(
        childPresentationAreaData[2],
        currentItem,
        QPointF(currentX, subscriptTop),
        subscriptMaximumAscent
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


float SubscriptRowColumnOperatorPresentation::childRelativeScale(
        float         parentRelativeScale,
        unsigned long childIndex
    ) const {
    return childIndex == 0 ? parentRelativeScale : parentRelativeScale * subscriptSizeAdjustment;
}
