/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref RootOperatorPresentationBase class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QFontMetricsF>
#include <QPainterPath>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QPolygonF>

#include <eqt_graphics_math_group_with_painter_path.h>

#include <ld_format_structures.h>
#include <ld_operator_format.h>
#include <ld_visual.h>

#include "unary_operator_presentation_base.h"
#include "root_operator_presentation_base.h"

/***********************************************************************************************************************
 * RootOperatorPresentationBase::ChildPlacementData
 */

RootOperatorPresentationBase::ChildPlacementData::ChildPlacementData(
        const QRectF& child0Rectangle,
        const QRectF& child1Rectangle
    ):currentChild0BoundingRectangle(
        child0Rectangle
    ),currentChild1BoundingRectangle(
        child1Rectangle
    ) {}


RootOperatorPresentationBase::ChildPlacementData::ChildPlacementData(
        const RootOperatorPresentationBase::ChildPlacementData& other
    ) {
    currentChild0BoundingRectangle = other.currentChild0BoundingRectangle;
    currentChild1BoundingRectangle = other.currentChild1BoundingRectangle;
}


RootOperatorPresentationBase::ChildPlacementData::~ChildPlacementData() {}


void RootOperatorPresentationBase::ChildPlacementData::setChild0BoundingRectangle(const QRectF& position) {
    currentChild0BoundingRectangle = position;
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0TopLeft(const QPointF& position) {
    currentChild0BoundingRectangle.moveTopLeft(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0TopLeft(float x, float y) {
    currentChild0BoundingRectangle.moveTopLeft(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0TopRight(const QPointF& position) {
    currentChild0BoundingRectangle.moveTopRight(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0TopRight(float x, float y) {
    currentChild0BoundingRectangle.moveTopRight(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0BottomLeft(const QPointF& position) {
    currentChild0BoundingRectangle.moveBottomLeft(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0BottomLeft(float x, float y) {
    currentChild0BoundingRectangle.moveBottomLeft(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0BottomRight(const QPointF& position) {
    currentChild0BoundingRectangle.moveBottomRight(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0BottomRight(float x, float y) {
    currentChild0BoundingRectangle.moveBottomRight(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0Size(const QSizeF& size) {
    currentChild0BoundingRectangle.setSize(size);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0Size(float width, float height) {
    currentChild0BoundingRectangle.setSize(QSizeF(width, height));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0Width(float width) {
    currentChild0BoundingRectangle.setWidth(width);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild0Height(float height) {
    currentChild0BoundingRectangle.setHeight(height);
}


const QRectF& RootOperatorPresentationBase::ChildPlacementData::child0BoundingRectangle() const {
    return currentChild0BoundingRectangle;
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child0TopLeft() const {
    return currentChild0BoundingRectangle.topLeft();
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child0TopRight() const {
    return currentChild0BoundingRectangle.topRight();
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child0BottomLeft() const {
    return currentChild0BoundingRectangle.bottomLeft();
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child0BottomRight() const {
    return currentChild0BoundingRectangle.bottomRight();
}


float RootOperatorPresentationBase::ChildPlacementData::child0Left() const {
    return currentChild0BoundingRectangle.left();
}


float RootOperatorPresentationBase::ChildPlacementData::child0Top() const {
    return currentChild0BoundingRectangle.top();
}


float RootOperatorPresentationBase::ChildPlacementData::child0Right() const {
    return currentChild0BoundingRectangle.right();
}


float RootOperatorPresentationBase::ChildPlacementData::child0Bottom() const {
    return currentChild0BoundingRectangle.bottom();
}


QSizeF RootOperatorPresentationBase::ChildPlacementData::child0Size() const {
    return currentChild0BoundingRectangle.size();
}


float RootOperatorPresentationBase::ChildPlacementData::child0Width() {
    return currentChild0BoundingRectangle.width();
}


float RootOperatorPresentationBase::ChildPlacementData::child0Height() {
    return currentChild0BoundingRectangle.height();
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1BoundingRectangle(const QRectF& position) {
    currentChild1BoundingRectangle = position;
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1TopLeft(const QPointF& position) {
    currentChild1BoundingRectangle.moveTopLeft(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1TopLeft(float x, float y) {
    currentChild1BoundingRectangle.moveTopLeft(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1TopRight(const QPointF& position) {
    currentChild1BoundingRectangle.moveTopRight(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1TopRight(float x, float y) {
    currentChild1BoundingRectangle.moveTopRight(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1BottomLeft(const QPointF& position) {
    currentChild1BoundingRectangle.moveBottomLeft(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1BottomLeft(float x, float y) {
    currentChild1BoundingRectangle.moveBottomLeft(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1BottomRight(const QPointF& position) {
    currentChild1BoundingRectangle.moveBottomRight(position);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1BottomRight(float x, float y) {
    currentChild1BoundingRectangle.moveBottomRight(QPointF(x, y));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1Size(const QSizeF& size) {
    currentChild1BoundingRectangle.setSize(size);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1Size(float width, float height) {
    currentChild1BoundingRectangle.setSize(QSizeF(width, height));
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1Width(float width) {
    currentChild1BoundingRectangle.setWidth(width);
}


void RootOperatorPresentationBase::ChildPlacementData::setChild1Height(float height) {
    currentChild1BoundingRectangle.setHeight(height);
}


const QRectF& RootOperatorPresentationBase::ChildPlacementData::child1BoundingRectangle() const {
    return currentChild1BoundingRectangle;
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child1TopLeft() const {
    return currentChild1BoundingRectangle.topLeft();
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child1TopRight() const {
    return currentChild1BoundingRectangle.topRight();
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child1BottomLeft() const {
    return currentChild1BoundingRectangle.bottomLeft();
}


QPointF RootOperatorPresentationBase::ChildPlacementData::child1BottomRight() const {
    return currentChild1BoundingRectangle.bottomRight();
}


float RootOperatorPresentationBase::ChildPlacementData::child1Left() const {
    return currentChild1BoundingRectangle.left();
}


float RootOperatorPresentationBase::ChildPlacementData::child1Top() const {
    return currentChild1BoundingRectangle.top();
}


float RootOperatorPresentationBase::ChildPlacementData::child1Right() const {
    return currentChild1BoundingRectangle.right();
}


float RootOperatorPresentationBase::ChildPlacementData::child1Bottom() const {
    return currentChild1BoundingRectangle.bottom();
}


QSizeF RootOperatorPresentationBase::ChildPlacementData::child1Size() const {
    return currentChild1BoundingRectangle.size();
}


float RootOperatorPresentationBase::ChildPlacementData::child1Width() {
    return currentChild1BoundingRectangle.width();
}


float RootOperatorPresentationBase::ChildPlacementData::child1Height() {
    return currentChild1BoundingRectangle.height();
}


RootOperatorPresentationBase::ChildPlacementData& RootOperatorPresentationBase::ChildPlacementData::operator=(
        const RootOperatorPresentationBase::ChildPlacementData& other
    ) {
    currentChild0BoundingRectangle = other.currentChild0BoundingRectangle;
    currentChild1BoundingRectangle = other.currentChild1BoundingRectangle;

    return *this;
}

/***********************************************************************************************************************
 * RootOperatorPresentationBase
 */

RootOperatorPresentationBase::RootOperatorPresentationBase() {}


RootOperatorPresentationBase::~RootOperatorPresentationBase() {}


RootOperatorPresentationBase::ChildPlacementData RootOperatorPresentationBase::drawRootSymbol(
        EQt::GraphicsMathGroupWithPainterPath* graphicsItem,
        Ld::FormatPointer                      format,
        float                                  relativeScale,
        const QPointF&                         basePosition,
        const QSizeF&                          child0Size,
        const QSizeF&                          child1Size
    ) {
    ChildPlacementData result;

    result.setChild0Size(child0Size);
    result.setChild1Size(child1Size);

    QFontMetricsF fontMetrics(operatorFont(format, relativeScale));
    float fontHeight    = fontMetrics.height();
    float fontAscent    = fontMetrics.ascent();
    float fontCapHeight = fontMetrics.capHeight();

    float lineWidth       = toScene(QSizeF(0.0F, 0.5F)).height();
    float totalHeight     = child0Size.height();
    float totalRootHeight = totalHeight - (fontAscent - fontCapHeight) / 4.0F;

    float m1 = -(0.10 * totalRootHeight) / (0.14 * fontHeight); // slope of left-most upward sloping nib.
    float m2 = -(totalRootHeight) / (0.46 * fontHeight);        // slope of main vertical line.
    float dx = 0.05 * fontHeight;                               // width of the trapazioid

    // left-most nib, ignoring offsets
    QPointF rl1p1(0.0F,              0.60 * totalRootHeight);
    QPointF rl1p2(0.14 * fontHeight, 0.50 * totalRootHeight);

    // main vertial line end points, ignoring offsets.
    QPointF rl2p1(0.30 * fontHeight, totalRootHeight);
    QPointF rl2p2(0.76 * fontHeight, 0.0F);

    QPointF relativeChild1BottomRight(rl2p1.x() + (rl1p2.y() - rl2p1.y()) / m2, rl1p2.y());
    QPointF relativeChild1TopLeft = relativeChild1BottomRight - QPointF(child1Size.width(), child1Size.height());
    QPointF base(
        basePosition.x() - std::min(0.0, relativeChild1TopLeft.x()),
        basePosition.y() - std::min(0.0, relativeChild1TopLeft.y())
    );

    result.setChild0TopLeft(base + rl2p2);
    result.setChild1TopLeft(base + relativeChild1TopLeft);

    if (graphicsItem != Q_NULLPTR) {
        QPainterPath painterPath;
        painterPath.setFillRule(Qt::WindingFill);

        // left-most nib
        QPointF l1p1 = base + rl1p1;
        QPointF l1p2 = base + rl1p2;

        // main vertial line
        QPointF l2p1 = base + rl2p1;
        QPointF l2p2 = base + rl2p2;

        // top horizontal line
        QPointF l3p1 = l2p2;
        QPointF l3p2(l2p2.x() + child0Size.width(), l2p2.y());

        // trapezoid forming thick line.
        QPointF t1p1 = l1p2;
        QPointF t1p2(t1p1.x() - dx, t1p1.y() - dx * m1);
        QPointF t1p3 = l2p1;
        QPointF t1p4(t1p3.x() + dx, t1p3.y() + dx * m2);

        painterPath.moveTo(l1p1);
        painterPath.lineTo(l1p2);
        painterPath.addPolygon(QPolygonF() << t1p1 << t1p2 << t1p3 << t1p4 << t1p1);
        painterPath.moveTo(l2p1);
        painterPath.lineTo(l2p2);
        painterPath.moveTo(l3p1);
        painterPath.lineTo(l3p2);

        graphicsItem->setPainterPath(painterPath);

        QColor pathColor = operatorColor(format);
        graphicsItem->setPainterPathPen(QPen(pathColor, lineWidth));
        graphicsItem->setPainterPathBrush(QBrush(pathColor));
    }

    return result;
}
