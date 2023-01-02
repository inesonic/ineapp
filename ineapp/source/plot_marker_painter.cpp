/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotMarkerPainter class.
***********************************************************************************************************************/

#include <QPainter>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QPainterPath>

#include <cmath>

#include "plot_marker_painter.h"

static const float pi            = 4.0F * std::atan(1.0F);
static const float fifthRotation = 2.0F * pi / 5.0F;

const float PlotMarkerPainter::starA = std::abs(std::cos(pi/2.0F - fifthRotation));
const float PlotMarkerPainter::starB = std::abs(std::sin(pi/2.0F - fifthRotation));
const float PlotMarkerPainter::starC = std::abs(std::cos(pi/2.0F + 2.0F * fifthRotation));
const float PlotMarkerPainter::starD = std::abs(std::sin(pi/2.0F + 2.0F * fifthRotation));

const float PlotMarkerPainter::oneOverSqrt2 = 1.0F / std::sqrt(2.0F);

PlotMarkerPainter::PlotMarkerPainter() {}


PlotMarkerPainter::~PlotMarkerPainter() {}


void PlotMarkerPainter::paintMarker(
        QPainter*                      painter,
        const QRect&                   boundingRectangle,
        float                          lineWidth,
        const QColor&                  lineColor,
        PlotMarkerPainter::MarkerStyle markerStyle
    ) {
    if (markerStyle != Ld::PlotSeries::MarkerStyle::NONE) {
        QPen markerPen(QBrush(lineColor), lineWidth);

        unsigned lineAdjust = static_cast<unsigned>((lineWidth + 1.0) / 2.0);

        switch (markerStyle) {
            case Ld::PlotSeries::MarkerStyle::NONE: {
                Q_ASSERT(false);
                break;
            }

            case Ld::PlotSeries::MarkerStyle::DOT: {
                QRect markerRect(
                    QPoint(boundingRectangle.left() + lineAdjust, boundingRectangle.top() + lineAdjust),
                    QPoint(boundingRectangle.right() - lineAdjust, boundingRectangle.bottom() - lineAdjust)
                );

                painter->setPen(markerPen);
                painter->setBrush(QBrush(lineColor));
                painter->drawEllipse(markerRect);

                break;
            }

            case Ld::PlotSeries::MarkerStyle::CIRCLE: {
                QRect markerRect(
                    QPoint(boundingRectangle.left() + lineAdjust, boundingRectangle.top() + lineAdjust),
                    QPoint(boundingRectangle.right() - lineAdjust, boundingRectangle.bottom() - lineAdjust)
                );

                painter->setPen(markerPen);
                painter->drawEllipse(markerRect);

                break;
            }

            case Ld::PlotSeries::MarkerStyle::SQUARE: {
                QPoint   markerCenter = boundingRectangle.center();
                unsigned halfWidth    = static_cast<unsigned>(0.5 + oneOverSqrt2 * boundingRectangle.width() / 2);
                unsigned halfHeight   = static_cast<unsigned>(0.5 + oneOverSqrt2 * boundingRectangle.height() / 2);

                QRect square(
                    QPoint(markerCenter.x() - halfWidth + lineAdjust, markerCenter.y() - halfHeight + lineAdjust),
                    QPoint(markerCenter.x() + halfWidth - lineAdjust, markerCenter.y() + halfHeight - lineAdjust)
                );

                painter->setPen(markerPen);
                painter->setBrush(QBrush());
                painter->drawRect(square);

                break;
            }

            case Ld::PlotSeries::MarkerStyle::DIAMOND: {
                QPoint   markerCenter = boundingRectangle.center();
                unsigned halfWidth    = markerCenter.x() - boundingRectangle.left();
                unsigned halfHeight   = markerCenter.y() - boundingRectangle.top();

                QPoint corners[4] = {
                    QPoint(markerCenter.x() - halfWidth + lineAdjust, markerCenter.y()),
                    QPoint(markerCenter.x(), markerCenter.y() + halfHeight - lineAdjust),
                    QPoint(markerCenter.x() + halfWidth - lineAdjust, markerCenter.y()),
                    QPoint(markerCenter.x(), markerCenter.y() - halfHeight + lineAdjust)
                };

                painter->setPen(markerPen);
                painter->setBrush(QBrush());
                painter->drawConvexPolygon(corners, 4);

                break;
            }

            case Ld::PlotSeries::MarkerStyle::STAR: {
                QPoint markerCenter = boundingRectangle.center();

                float  x            = markerCenter.x();
                float  y            = markerCenter.y();
                float  heightOver2  = y - boundingRectangle.top();
                float  widthOver2   = x - boundingRectangle.left();

                // Star end points.
                QPointF p1(x, y - heightOver2);
                QPointF p2(x - starA * widthOver2 - lineWidth, y - starB * heightOver2 - lineWidth);
                QPointF p3(x - starC * widthOver2 - lineWidth, y + starD * heightOver2);
                QPointF p4(x + starC * widthOver2, y + starD * heightOver2);
                QPointF p5(x + starA * widthOver2 - lineWidth, y - starB * heightOver2);

                // Crossing lines.
                QLineF l13(p1, p3);
                QLineF l24(p2, p4);
                QLineF l35(p3, p5);
                QLineF l41(p4, p1);
                QLineF l52(p5, p2);

                // Star interior points.
                QPointF p12;
                QPointF p23;
                QPointF p34;
                QPointF p45;
                QPointF p51;

                l13.intersects(l52, &p12);
                l24.intersects(l13, &p23);
                l35.intersects(l24, &p34);
                l41.intersects(l35, &p45);
                l52.intersects(l41, &p51);

                QPainterPath path;
                path.moveTo(p1);
                path.lineTo(p12);
                path.lineTo(p2);
                path.lineTo(p23);
                path.lineTo(p3);
                path.lineTo(p34);
                path.lineTo(p4);
                path.lineTo(p45);
                path.lineTo(p5);
                path.lineTo(p51);
                path.lineTo(p1);

                painter->setPen(markerPen);
                painter->setBrush(QBrush());
                painter->drawPath(path);

                break;
            }

            case Ld::PlotSeries::MarkerStyle::X: {
                QPoint   markerCenter = boundingRectangle.center();
                unsigned halfWidth    = static_cast<unsigned>(0.5 + oneOverSqrt2 * boundingRectangle.width() / 2);
                unsigned halfHeight   = static_cast<unsigned>(0.5 + oneOverSqrt2 * boundingRectangle.height() / 2);

                QPoint p1(markerCenter.x() - halfWidth, markerCenter.y() - halfHeight);
                QPoint p2(markerCenter.x() + halfWidth, markerCenter.y() + halfHeight);
                QPoint p3(markerCenter.x() - halfWidth, markerCenter.y() + halfHeight);
                QPoint p4(markerCenter.x() + halfWidth, markerCenter.y() - halfHeight);

                painter->setPen(markerPen);
                painter->setBrush(QBrush());
                painter->drawLine(p1, p2);
                painter->drawLine(p3, p4);

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }
    }
}
