/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref LogicalNotOperatorPresentationBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFontMetricsF>
#include <QColor>
#include <QGraphicsScene>
#include <QPen>

#include <eqt_graphics_math_group_with_line.h>

#include <ld_format_structures.h>
#include <ld_font_format.h>
#include <ld_logical_unary_not_operator_format.h>
#include <ld_visual.h>

#include "unary_operator_presentation_base.h"
#include "logical_not_operator_presentation_base.h"

LogicalNotOperatorPresentationBase::LogicalNotOperatorPresentationBase() {}


LogicalNotOperatorPresentationBase::~LogicalNotOperatorPresentationBase() {}


void LogicalNotOperatorPresentationBase::processFormatChange(
        Ld::FormatPointer oldFormat,
        Ld::FormatPointer newFormat
    ) {
    clearChildPresentationAreas();
    UnaryOperatorPresentationBase::processFormatChange(oldFormat, newFormat);
}


EQt::GraphicsMathGroup* LogicalNotOperatorPresentationBase::buildGraphicsItem(
        Ld::FormatPointer                                    format,
        LogicalNotOperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                                relativeScale,
        const QList<PresentationAreaTracker>&                childPresentationAreaData,
        QSizeF&                                              requiredSize,
        float&                                               requiredAscent
    ) {
    EQt::GraphicsMathGroup*                             result;
    EQt::GraphicsMathGroupWithLine*                     graphicsItem     = new EQt::GraphicsMathGroupWithLine;
    Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol logicalNotSymbol = logicalNotStyle(format);

    if (logicalNotSymbol == Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol::OVERBAR) {
        result = LogicalNotOperatorPresentationBase::updateOverbarGraphicsItem(
            format,
            graphicsItem,
            parenthesisStyle,
            relativeScale,
            childPresentationAreaData,
            requiredSize,
            requiredAscent
        );
    } else {
        graphicsItem->setLinePen(QPen(Qt::NoPen));
        result = UnaryOperatorPresentationBase::updateGraphicsItem(
            format,
            graphicsItem,
            parenthesisStyle,
            relativeScale,
            childPresentationAreaData,
            requiredSize,
            requiredAscent
        );
    }

    return result;
}


EQt::GraphicsMathGroup* LogicalNotOperatorPresentationBase::updateGraphicsItem(
        Ld::FormatPointer                                    format,
        EQt::GraphicsMathGroup*                              currentItem,
        LogicalNotOperatorPresentationBase::ParenthesisStyle parenthesisStyle,
        float                                                relativeScale,
        const QList<PresentationAreaTracker>&                childPresentationAreaData,
        QSizeF&                                              requiredSize,
        float&                                               requiredAscent
    ) {
    EQt::GraphicsMathGroup*         result;
    EQt::GraphicsMathGroupWithLine* graphicsItem = dynamic_cast<EQt::GraphicsMathGroupWithLine*>(currentItem);

    Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol logicalNotSymbol = logicalNotStyle(format);
    if (logicalNotSymbol == Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol::OVERBAR) {
        result = updateOverbarGraphicsItem(
            format,
            graphicsItem,
            parenthesisStyle,
            relativeScale,
            childPresentationAreaData,
            requiredSize,
            requiredAscent
        );
    } else {
        graphicsItem->setLinePen(QPen(Qt::NoPen));
        result = dynamic_cast<EQt::GraphicsMathGroupWithLine*>(
            UnaryOperatorPresentationBase::updateGraphicsItem(
                format,
                graphicsItem,
                parenthesisStyle,
                relativeScale,
                childPresentationAreaData,
                requiredSize,
                requiredAscent
            )
        );
    }

    return result;
}


QString LogicalNotOperatorPresentationBase::operatorString(Ld::FormatPointer format) const {
    QString result;

    if (logicalNotStyle(format) == Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol::TILDE) {
        result = tr("~");
    } else {
        result = tr("%1%2").arg(QChar(0x00AC)).arg(QChar(0x2009));
    }

    return result;
}


EQt::GraphicsMathGroupWithLine* LogicalNotOperatorPresentationBase::updateOverbarGraphicsItem(
        Ld::FormatPointer                     format,
        EQt::GraphicsMathGroupWithLine*       currentItem,
        ParenthesisStyle                      parenthesisStyle,
        float                                 relativeScale,
        const QList<PresentationAreaTracker>& childPresentationAreaData,
        QSizeF&                               requiredSize,
        float&                                requiredAscent
    ) {
    QFont font = operatorFont(format, relativeScale);
    QFontMetricsF fontMetrics(font);

    float childMaximumAscent  = childPresentationAreaData[0].maximumAscent();
    float childMaximumDescent = childPresentationAreaData[0].maximumDescent();
    float childHeight         = childMaximumAscent + childMaximumDescent;

    float overbarLineWidth        = toScene(QSizeF(0.0, 0.5 * relativeScale)).height();
    float overbarVerticalPosition = (fontMetrics.ascent() - fontMetrics.capHeight()) / 4.0F;

    EQt::GraphicsMathGroup::ParenthesisStyle
        graphicsItemParenthesisStyle = static_cast<EQt::GraphicsMathGroup::ParenthesisStyle>(parenthesisStyle);

    ParenthesisData leftParenthesisData  = leftParenthesisSize(font, childHeight, graphicsItemParenthesisStyle);
    ParenthesisData rightParenthesisData = rightParenthesisSize(font, childHeight, graphicsItemParenthesisStyle);

    float currentX = addPresentationAreas(
        childPresentationAreaData[0],
        currentItem,
        QPointF(leftParenthesisData.width(), 0),
        childMaximumAscent
    );

    if (currentItem->numberTextEntries() > 0) {
        currentItem->clearText();
    }

    unsigned parenthesisIndex = 0;
    insertLeftParenthesis(leftParenthesisData, 0.0F, 0.0F, currentItem, parenthesisIndex);
    insertRightParenthesis(leftParenthesisData, currentX, 0.0F, currentItem, parenthesisIndex);
    currentItem->setParenthesisCenterLine(
        childMaximumAscent - fontMetrics.ascent() / 4.0 - leftParenthesisData.relativePosition()
    );

    currentX += rightParenthesisData.width();

    QColor fontColor;
    if (!format.isNull() && format->capabilities().contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();
        fontColor = fontFormat->fontColor();
    }

    if (!fontColor.isValid()) {
        fontColor = QColor(Qt::black);
    }

    QPen linePen(fontColor);
    linePen.setWidthF(overbarLineWidth);

    currentItem->setLinePen(linePen);
    currentItem->setLine(0, overbarVerticalPosition, currentX, overbarVerticalPosition);

    setColors(currentItem, format);

    requiredSize   = QSizeF(currentX, childHeight);
    requiredAscent = childMaximumAscent;

    return currentItem;
}


Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol LogicalNotOperatorPresentationBase::logicalNotStyle(
        Ld::FormatPointer format
    ) const {
    Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol result;

    if (format->capabilities().contains(Ld::LogicalUnaryNotOperatorFormat::formatName)) {
        QSharedPointer<Ld::LogicalUnaryNotOperatorFormat>
            logicalNotFormat = format.dynamicCast<Ld::LogicalUnaryNotOperatorFormat>();

        result = logicalNotFormat->logicalNotSymbol();

        if (result == Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol::DEFAULT) {
            result = defaultLogicalNotSymbol();
        }
    } else {
        result = defaultLogicalNotSymbol();
    }

    return result;
}
