/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref WhileOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <util_algorithm.h>

#include <ld_format_structures.h>
#include <ld_while_operator_element.h>

#include "application.h"
#include "operator_presentation_base.h"
#include "control_flow_operator_presentation_base.h"
#include "while_operator_presentation.h"

WhileOperatorPresentation::WhileOperatorPresentation() {}


WhileOperatorPresentation::~WhileOperatorPresentation() {}


Ld::Visual* WhileOperatorPresentation::creator(const QString&) {
    return new WhileOperatorPresentation();
}


EQt::GraphicsMathGroup* WhileOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                          format,
        OperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    return WhileOperatorPresentation::updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroup,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* WhileOperatorPresentation::updateGraphicsItem(
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

    QSharedPointer<Ld::WhileOperatorElement>
        element = WhileOperatorPresentation::element().dynamicCast<Ld::WhileOperatorElement>();

    QString s1 = tr("while ");
    QString s2 = tr(" : ");

    float operatorAscent  = fontMetrics.ascent();
    float operatorDescent = fontMetrics.descent();
    float operatorWidth1  = fontMetrics.horizontalAdvance(s1);
    float operatorWidth2  = fontMetrics.horizontalAdvance(s2);

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();

    float maximumAscent  = Util::max(operatorAscent, child0MaximumAscent, child1MaximumAscent);
    float maximumDescent = Util::max(operatorDescent, child0MaximumDescent, child1MaximumDescent);
    float maximumHeight  = maximumAscent + maximumDescent;

    if (currentItem->numberTextEntries() > 0) {
        EQt::GraphicsMathGroup::TextEntry& entry = currentItem->entry(0);
        entry.setFont(font);
        entry.setPosition(0, maximumAscent);
    } else {
        currentItem->append(s1, font, QPointF(0, maximumAscent));
    }

    float currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(operatorWidth1, 0),
        maximumAscent
    );

    if (currentItem->numberTextEntries() > 1) {
        EQt::GraphicsMathGroup::TextEntry& entry = currentItem->entry(1);
        entry.setFont(font);
        entry.setPosition(currentX, maximumAscent);
    } else {
        currentItem->append(s2, font, QPointF(currentX, maximumAscent));
    }

    currentX = addPresentationAreas(
        childPresentationAreaData[1],
        currentItem,
        QPointF(currentX + operatorWidth2, 0),
        maximumAscent
    );

    setColors(currentItem, format);

    requiredSize   = QSizeF(currentX, maximumHeight);
    requiredAscent = maximumAscent;

    return currentItem;
}
