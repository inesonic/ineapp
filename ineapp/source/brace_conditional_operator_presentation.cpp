/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BraceConditionalOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <util_algorithm.h>

#include <ld_format_structures.h>
#include <ld_brace_conditional_format.h>
#include <ld_brace_conditional_operator_element.h>

#include "application.h"
#include "operator_presentation_base.h"
#include "brace_conditional_operator_presentation.h"

BraceConditionalOperatorPresentation::BraceConditionalOperatorPresentation() {}


BraceConditionalOperatorPresentation::~BraceConditionalOperatorPresentation() {}


Ld::Visual* BraceConditionalOperatorPresentation::creator(const QString&) {
    return new BraceConditionalOperatorPresentation();
}


EQt::GraphicsMathGroup* BraceConditionalOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                          format,
        OperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    return BraceConditionalOperatorPresentation::updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroup,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* BraceConditionalOperatorPresentation::updateGraphicsItem(
        Ld::FormatPointer                          format,
        EQt::GraphicsMathGroup*                    currentItem,
        OperatorPresentationBase::ParenthesisStyle /* parenthesisStyle */,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    QFont font = operatorFont(format, relativeScale);
    QFontMetricsF fontMetrics(font);

    QSharedPointer<Ld::BraceConditionalOperatorElement>
        element = BraceConditionalOperatorPresentation::element().dynamicCast<Ld::BraceConditionalOperatorElement>();

    QString conditionPreambleText;
    QString elseConditionText;
    if (!format.isNull() && format->capabilities().contains(Ld::BraceConditionalFormat::formatName)) {
        QSharedPointer<Ld::BraceConditionalFormat>
            braceConditionalFormat = format.dynamicCast<Ld::BraceConditionalFormat>();

        conditionPreambleText = braceConditionalFormat->conditionPreambleString();
        elseConditionText     = braceConditionalFormat->elseConditionString();
    } else {
        conditionPreambleText = Ld::BraceConditionalFormat::defaultConditionPreambleString;
        elseConditionText     = Ld::BraceConditionalFormat::defaultElseConditionString;
    }

    float conditionPreambleWidth = fontMetrics.horizontalAdvance(conditionPreambleText);
    float fontAscent             = fontMetrics.ascent();
    float fontDescent            = fontMetrics.descent();
    float columnSpacing          = fontMetrics.horizontalAdvance(tr("MM"));
    float leftBraceWidth         = columnSpacing/ 4.0F;

    float         maximumValueWidth          = 0;
    unsigned long numberExplicitConditionals = element->numberExplicitConditions();
    bool          hasExplicitElseCondition   = element->hasExplicitElseCondition();

    for (unsigned long conditionIndex=0 ; conditionIndex<numberExplicitConditionals ; ++conditionIndex) {
        const PresentationAreaTracker& valueEntry = childPresentationAreaData.at(2 * conditionIndex + 0);
        float                          valueWidth = valueEntry.width();

        if (valueWidth > maximumValueWidth) {
            maximumValueWidth = valueWidth;
        }
    }

    unsigned long numberChildren = element->numberChildren();

    const PresentationAreaTracker& elseValueEntry =   hasExplicitElseCondition
                                                    ? childPresentationAreaData.at(numberChildren - 2)
                                                    : childPresentationAreaData.at(numberChildren - 1);
    float                          elseValueWidth = elseValueEntry.width();

    if (elseValueWidth > maximumValueWidth) {
        maximumValueWidth = elseValueWidth;
    }

    currentItem->clearText();

    float currentY     = 0;
    float maximumWidth = 0;
    float conditionPreambleX = leftBraceWidth + maximumValueWidth + columnSpacing;
    float conditionX         = conditionPreambleX + conditionPreambleWidth;
    for (unsigned long conditionIndex=0 ; conditionIndex<numberExplicitConditionals ; ++conditionIndex) {
        const PresentationAreaTracker& valueEntry     = childPresentationAreaData.at(2 * conditionIndex + 0);
        const PresentationAreaTracker& conditionEntry = childPresentationAreaData.at(2 * conditionIndex + 1);

        float valueAscent      = valueEntry.maximumAscent();
        float valueDescent     = valueEntry.maximumDescent();
        float conditionAscent  = conditionEntry.maximumAscent();
        float conditionDescent = conditionEntry.maximumDescent();

        float maximumAscent    = Util::max(valueAscent, fontAscent, conditionAscent);
        float maximumDescent   = Util::max(valueDescent, fontDescent, conditionDescent);
        float maximumHeight    = maximumAscent + maximumDescent;

        addPresentationAreas(
            valueEntry,
            currentItem,
            QPointF(leftBraceWidth, currentY),
            maximumAscent
        );

        currentItem->append(conditionPreambleText, font, QPointF(conditionPreambleX, currentY + maximumAscent));

        float lineMaximumY = addPresentationAreas(
            conditionEntry,
            currentItem,
            QPointF(conditionX, currentY),
            maximumAscent
        );

        if (lineMaximumY > maximumWidth) {
            maximumWidth = lineMaximumY;
        }

        currentY += maximumHeight;
    }

    float elseValueAscent  = elseValueEntry.maximumAscent();
    float elseValueDescent = elseValueEntry.maximumDescent();

    if (hasExplicitElseCondition) {
        const PresentationAreaTracker& elseConditionEntry = childPresentationAreaData.at(numberChildren - 1);

        float elseConditionAscent  = elseConditionEntry.maximumAscent();
        float elseConditionDescent = elseConditionEntry.maximumDescent();

        float maximumElseAscent = Util::max(elseValueAscent, fontAscent, elseConditionAscent);
        float maximumElseHeight = maximumElseAscent + Util::max(elseValueDescent, fontDescent, elseConditionDescent);

        addPresentationAreas(
            elseValueEntry,
            currentItem,
            QPointF(leftBraceWidth, currentY),
            maximumElseAscent
        );

        currentItem->append(conditionPreambleText, font, QPointF(conditionPreambleX, currentY + maximumElseAscent));

        float lineMaximumY = addPresentationAreas(
            elseConditionEntry,
            currentItem,
            QPointF(conditionX, currentY),
            maximumElseAscent
        );

        if (lineMaximumY > maximumWidth) {
            maximumWidth = lineMaximumY;
        }

        currentY += maximumElseHeight;
    } else {
        float maximumElseAscent = std::max(elseValueAscent, fontAscent);
        float maximumElseHeight = maximumElseAscent + std::max(elseValueDescent, fontDescent);

        addPresentationAreas(
            elseValueEntry,
            currentItem,
            QPointF(leftBraceWidth, currentY),
            maximumElseAscent
        );

        float   elseConditionTextWidth = fontMetrics.horizontalAdvance(elseConditionText);

        currentItem->append(elseConditionText, font, QPointF(conditionPreambleX, currentY + maximumElseAscent));

        float lineMaximumY = conditionPreambleX + elseConditionTextWidth;
        if (lineMaximumY > maximumWidth) {
            maximumWidth = lineMaximumY;
        }

        currentY += maximumElseHeight;
    }

    currentItem->setLeftParenthesis(
        EQt::GraphicsMathGroup::ParenthesisStyle::BRACES,
        QRectF(0, 0, leftBraceWidth, currentY)
    );

    currentItem->setRightParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle::NONE);

    float centerLine = currentY / 2.0;
    currentItem->setParenthesisCenterLine(centerLine);

    setColors(currentItem, format);

    requiredSize   = QSizeF(maximumWidth, currentY);
    requiredAscent = centerLine + fontAscent / 4.0;

    return currentItem;
}
