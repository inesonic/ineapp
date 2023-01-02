/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Range3Presentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QFontMetricsF>

#include <util_algorithm.h>

#include <eqt_graphics_math_group.h>

#include <ld_format_structures.h>
#include <ld_range_3_element.h>
#include <ld_range_3_visual.h>

#include "application.h"
#include "trinary_operator_presentation_base.h"
#include "range_3_presentation.h"

Range3Presentation::Range3Presentation() {}


Range3Presentation::~Range3Presentation() {}


Ld::Visual* Range3Presentation::creator(const QString&) {
    return new Range3Presentation();
}


EQt::GraphicsMathGroup* Range3Presentation::buildGraphicsItem(
        Ld::FormatPointer                     format,
        Range3Presentation::ParenthesisStyle  parenthesisStyle,
        float                                 relativeScale,
        const QList<PresentationAreaTracker>& childPresentationAreaData,
        QSizeF&                               requiredSize,
        float&                                requiredAscent
    ) {
    EQt::GraphicsMathGroup* result = new EQt::GraphicsMathGroup;
    return updateGraphicsItem(
        format,
        result,
        parenthesisStyle,
        relativeScale,
        childPresentationAreaData,
        requiredSize,
        requiredAscent
    );
}


EQt::GraphicsMathGroup* Range3Presentation::updateGraphicsItem(
        Ld::FormatPointer                     format,
        EQt::GraphicsMathGroup*               currentItem,
        Range3Presentation::ParenthesisStyle  parenthesisStyle,
        float                                 relativeScale,
        const QList<PresentationAreaTracker>& childPresentationAreaData,
        QSizeF&                               requiredSize,
        float&                                requiredAscent
    ) {
    currentItem->clearText();

    QFont font = operatorFont(format, relativeScale);
    QFontMetricsF fontMetrics(font);

    QString commaSeparator   = tr(", ");
    QString elipsesSeparator = tr(",%1, ").arg(QChar(0x2026));

    float fontAscent            = fontMetrics.ascent();
    float fontDescent           = fontMetrics.descent();
    float commaWidth            = fontMetrics.horizontalAdvance(commaSeparator);
    float elipsesSeparatorWidth = fontMetrics.horizontalAdvance(elipsesSeparator);

    float child0MaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float child1MaximumAscent  = childPresentationAreaData[1].maximumAscent();
    float child2MaximumAscent  = childPresentationAreaData[2].maximumAscent();
    float child0MaximumDescent = childPresentationAreaData[0].maximumDescent();
    float child1MaximumDescent = childPresentationAreaData[1].maximumDescent();
    float child2MaximumDescent = childPresentationAreaData[2].maximumDescent();

    float maximumAscent  = Util::max(child0MaximumAscent, child1MaximumAscent, child2MaximumAscent, fontAscent);
    float maximumDescent = Util::max(child0MaximumDescent, child1MaximumDescent, child2MaximumDescent, fontDescent);

    float maximumHeight  = maximumAscent + maximumDescent;

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, maximumHeight, graphicsItemParenthesisStyle);

    float currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(leftParenthesisData.width(), 0.0),
        maximumAscent
    );

    currentItem->append(commaSeparator, font, QPointF(currentX, maximumAscent));
    currentX += commaWidth;

    currentX = addPresentationAreas(
        childPresentationAreaData[1],
        currentItem,
        QPointF(currentX, 0.0),
        maximumAscent
    );

    currentItem->append(elipsesSeparator, font, QPointF(currentX, maximumAscent));
    currentX += elipsesSeparatorWidth;

    currentX = addPresentationAreas(
        childPresentationAreaData[2],
        currentItem,
        QPointF(currentX, 0.0),
        maximumAscent
    );

    unsigned parenthesisIndex = 2;
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
