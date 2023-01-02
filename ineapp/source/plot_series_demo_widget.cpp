/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotSeriesDemoWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QLinearGradient>
#include <QLineF>

#include <cmath>

#include "application.h"
#include "plot_marker_painter.h"
#include "plot_series_demo_widget.h"

PlotSeriesDemoWidget::PlotSeriesDemoWidget(QWidget* parent):QWidget(parent) {
    currentLineStyle      = LineStyle::SOLID_LINE;
    currentLineWidth      = 0;
    currentLineColor      = QColor(Qt::GlobalColor::black);
    currentMarkerStyle    = MarkerStyle::CIRCLE;
    currentSplineType     = SplineType::LINE;
    currentGradientType   = GradientType::INTENSITY;
}


PlotSeriesDemoWidget::~PlotSeriesDemoWidget() {}


PlotSeriesDemoWidget::LineStyle PlotSeriesDemoWidget::lineStyle() const {
    return currentLineStyle;
}


float PlotSeriesDemoWidget::lineWidth() const {
    return currentLineWidth;
}


QColor PlotSeriesDemoWidget::lineColor() const {
    return currentLineColor;
}


PlotSeriesDemoWidget::MarkerStyle PlotSeriesDemoWidget::markerStyle() const {
    return currentMarkerStyle;
}


PlotSeriesDemoWidget::SplineType PlotSeriesDemoWidget::splineType() const {
    return currentSplineType;
}


PlotSeriesDemoWidget::GradientType PlotSeriesDemoWidget::gradientType() const {
    return currentGradientType;
}


void PlotSeriesDemoWidget::paint(
        QPainter*                          painter,
        const QRect&                       boundingRectangle,
        PlotSeriesDemoWidget::LineStyle    lineStyle,
        float                              lineWidth,
        const QColor&                      lineColor,
        PlotSeriesDemoWidget::MarkerStyle  markerStyle,
        PlotSeriesDemoWidget::SplineType   splineType,
        PlotSeriesDemoWidget::GradientType gradientType
    ) {
    if (gradientType != GradientType::NONE) {
        QLineF leftLine(boundingRectangle.topLeft(), boundingRectangle.bottomLeft());
        QLineF rightLine(boundingRectangle.topRight(), boundingRectangle.bottomRight());

        int hue        = lineColor.hue();
        int saturation = lineColor.saturation();
        int value      = lineColor.value();

        if (gradientType == GradientType::INTENSITY) {
            QColor startColor = QColor::fromHsv(hue, saturation, 0);
            QColor finalColor = QColor::fromHsv(hue, saturation, 255);

            QLinearGradient gradient(leftLine.center(), rightLine.center());
            gradient.setColorAt(0, startColor);
            gradient.setColorAt(1, finalColor);

            painter->fillRect(boundingRectangle, gradient);
            painter->setPen(QColor(Qt::GlobalColor::white));
            painter->drawRect(
                boundingRectangle.left() + 2,
                boundingRectangle.top() + 2,
                boundingRectangle.width() - 5,
                boundingRectangle.height() - 5
            );
        } else {
            float hueChangePerLine = static_cast<float>(gradientType) / static_cast<float>(boundingRectangle.width());
            float h                = hue;
            for (unsigned x=boundingRectangle.left() ; x<=static_cast<unsigned>(boundingRectangle.right()) ; ++x) {
                QColor c = QColor::fromHsv(static_cast<int>(h), saturation, value);
                painter->setPen(c);
                painter->setBrush(QBrush(c));
                painter->drawLine(x, boundingRectangle.top(), x, boundingRectangle.bottom());

                h += hueChangePerLine;
                if (h >= 360) {
                    h -= 360;
                } else if (h < 0) {
                    h += 360;
                }
            }
        }
    } else if (markerStyle != Ld::PlotSeries::MarkerStyle::NONE || splineType != Ld::PlotSeries::SplineType::NONE) {
        QPoint markerCenter;

        switch (splineType) {
            case Ld::PlotSeries::SplineType::NONE: {
                markerCenter = boundingRectangle.center();
                break;
            }

            case Ld::PlotSeries::SplineType::LINE: {
                QPen linePen(QBrush(lineColor), lineWidth, static_cast<Qt::PenStyle>(lineStyle));

                QLine leftEdge(boundingRectangle.topLeft(), boundingRectangle.bottomLeft());
                QLine rightEdge(boundingRectangle.topRight(), boundingRectangle.bottomRight());

                painter->setPen(linePen);
                painter->drawLine(leftEdge.center(), rightEdge.center());

                markerCenter = boundingRectangle.center();

                break;
            }

            case Ld::PlotSeries::SplineType::LINEAR_REGRESSION: {
                QPen linePen(QBrush(lineColor), lineWidth, static_cast<Qt::PenStyle>(lineStyle));

                painter->setPen(linePen);
                painter->drawLine(boundingRectangle.bottomLeft(), boundingRectangle.topRight());

                QLine diagonal(boundingRectangle.topLeft(), boundingRectangle.center());
                markerCenter = diagonal.center();

                break;
            }

            case Ld::PlotSeries::SplineType::SPLINE: {
                QPen linePen(QBrush(lineColor), lineWidth, static_cast<Qt::PenStyle>(lineStyle));

                QLine topEdge(boundingRectangle.topLeft(), boundingRectangle.topRight());

                QPainterPath path;
                path.moveTo(boundingRectangle.bottomLeft());
                path.quadTo(topEdge.center(), boundingRectangle.bottomRight());

                painter->setPen(linePen);
                painter->drawPath(path);

                markerCenter = boundingRectangle.center();

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        QSize physicalDpi     = Application::physicalDpi();
        float widthAdjustment = static_cast<float>(physicalDpi.width()) / static_cast<float>(physicalDpi.height());

        unsigned halfMarkerHeight  = static_cast<unsigned>(markerCenter.y() / 2.0);
        unsigned adjustedLineWidth = static_cast<unsigned>(std::max(1.0F, lineWidth + 0.5F));
        QRect markerRectangle(
            markerCenter.x() - widthAdjustment * halfMarkerHeight,
            markerCenter.y() - halfMarkerHeight,
            widthAdjustment * (2 * halfMarkerHeight + adjustedLineWidth),
            2 * halfMarkerHeight + adjustedLineWidth
        );

        paintMarker(painter, markerRectangle, lineWidth, lineColor, markerStyle);
    } else {
        painter->fillRect(boundingRectangle, QBrush(lineColor));
        painter->setPen(QColor(Qt::GlobalColor::white));
        painter->drawRect(
            boundingRectangle.left() + 2,
            boundingRectangle.top() + 2,
            boundingRectangle.width() - 5,
            boundingRectangle.height() - 5
        );
    }
}


void PlotSeriesDemoWidget::setLineStyle(LineStyle newLineStyle) {
    currentLineStyle = newLineStyle;
    update();
}


void PlotSeriesDemoWidget::setLineWidth(float newLineWidth) {
    currentLineWidth = newLineWidth;
    update();
}


void PlotSeriesDemoWidget::setLineColor(const QColor& newColor) {
    currentLineColor = newColor;
    update();
}


void PlotSeriesDemoWidget::setMarkerStyle(PlotSeriesDemoWidget::MarkerStyle newMarkerStyle) {
    currentMarkerStyle = newMarkerStyle;
    update();
}


void PlotSeriesDemoWidget::setSplineType(PlotSeriesDemoWidget::SplineType newSplineType) {
    currentSplineType = newSplineType;
    update();
}


void PlotSeriesDemoWidget::setGradientType(PlotSeriesDemoWidget::GradientType newGradientType) {
    currentGradientType = newGradientType;
    update();
}


void PlotSeriesDemoWidget::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    QRect    boundingRectangle(QPoint(0, 0), size());

    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    painter.fillRect(boundingRectangle, QBrush(Qt::GlobalColor::white));

    paint(
        &painter,
        boundingRectangle,
        currentLineStyle,
        currentLineWidth,
        currentLineColor,
        currentMarkerStyle,
        currentSplineType,
        currentGradientType
    );
}


