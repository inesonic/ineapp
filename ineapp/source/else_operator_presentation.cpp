/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ElseOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <util_algorithm.h>

#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_diagnostic.h>
#include <ld_else_operator_element.h>

#include "application.h"
#include "operator_presentation_base.h"
#include "else_operator_presentation.h"

ElseOperatorPresentation::ElseOperatorPresentation() {}


ElseOperatorPresentation::~ElseOperatorPresentation() {}


Ld::Visual* ElseOperatorPresentation::creator(const QString&) {
    return new ElseOperatorPresentation();
}


EQt::GraphicsMathGroup* ElseOperatorPresentation::buildGraphicsItem(
        Ld::FormatPointer                          format,
        OperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                      relativeScale,
        const QList<PresentationAreaTracker>&      childPresentationAreaData,
        QSizeF&                                    requiredSize,
        float&                                     requiredAscent
    ) {
    return ElseOperatorPresentation::updateGraphicsItem(
        format,
        new EQt::GraphicsMathGroup,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* ElseOperatorPresentation::updateGraphicsItem(
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

    QString operatorString = tr("else: ");

    float operatorAscent  = fontMetrics.ascent();
    float operatorDescent = fontMetrics.descent();
    float operatorWidth   = fontMetrics.horizontalAdvance(operatorString);

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();

    float maximumAscent  = Util::max(operatorAscent, child0MaximumAscent);
    float maximumDescent = Util::max(operatorDescent, child0MaximumDescent);
    float maximumHeight  = maximumAscent + maximumDescent;

    if (currentItem->numberTextEntries() > 0) {
        EQt::GraphicsMathGroup::TextEntry& entry = currentItem->entry(0);
        entry.setFont(font);
        entry.setPosition(0, maximumAscent);
    } else {
        currentItem->append(operatorString, font, QPointF(0, maximumAscent));
    }

    float currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(operatorWidth, 0),
        maximumAscent
    );

    setColors(currentItem, format);

    requiredSize   = QSizeF(currentX, maximumHeight);
    requiredAscent = maximumAscent;

    return currentItem;
}
