/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RulerScene class.
***********************************************************************************************************************/

#include <QSize>
#include <QSizeF>
#include <QPalette>
#include <QFont>
#include <QLocale>
#include <QFontMetrics>
#include <QRect>
#include <QVector>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>
#include <QLineF>
#include <QGraphicsScene>

#include <cmath>

#include <util_units.h>

#include "application.h"
#include "page_list.h"
#include "application.h"
#include "ruler_scene.h"

RulerScene::RulerScene(QObject* parent):QGraphicsScene(parent) {
    currentLengths.clear();
    currentZoomFactor       = defaultZoomFactor;
    currentImperialUnits    = (Application::locale()->measurementSystem() == QLocale::ImperialSystem);
    cursorLine              = Q_NULLPTR;
    currentHeight           = 0;

    redraw();
}


RulerScene::~RulerScene() {}


QList<double> RulerScene::lengths() const {
    return currentLengths;
}


double RulerScene::zoomFactor() const {
    return currentZoomFactor;
}


bool RulerScene::imperialUnits() const {
    return currentImperialUnits;
}


bool RulerScene::cursorBarIsVisible() const {
    return cursorLine->isVisible();
}


int RulerScene::height() const {
    return currentHeight;
}


void RulerScene::setLengthsAndPositions(
        QList<double> newRulerLength,
        QList<double> newRulerPositions,
        double        newLeftEdge,
        double        newRightEdge
    ) {
    currentLengths = newRulerLength;
    currentPositions = newRulerPositions;

    redraw();

    QRectF sceneRectangle = sceneRect();
    sceneRectangle.setLeft(newLeftEdge);
    sceneRectangle.setRight(newRightEdge);
    setSceneRect(sceneRectangle);
}


void RulerScene::setZoomFactor(double newZoomFactor) {
    currentZoomFactor = newZoomFactor;
    redraw();
}


void RulerScene::setImperialUnits(bool nowUsingImperialUnits) {
    currentImperialUnits = nowUsingImperialUnits;
    redraw();
}


void RulerScene::setCursorBarVisible(bool nowVisible) {
    cursorLine->setVisible(nowVisible);
}


void RulerScene::setCursorPointPosition(unsigned column, double newPosition) {
    QLineF line = cursorLine->line();

    double y1 = line.y1();
    double y2 = line.y2();
    double x;

    double rulerPosition = newPosition + currentPositions[column];
    if (currentImperialUnits) {
        x = pixelsPerUnit * rulerPosition / Util::Units::pointsPerInch;
    } else {
        x = pixelsPerUnit * rulerPosition / Util::Units::pointsPerMm;
    }

    cursorLine->setLine(x, y1, x, y2);
}


void RulerScene::setCursorSceneCoordinatePosition(double newPosition) {
    QLineF line = cursorLine->line();

    double y1 = line.y1();
    double y2 = line.y2();

    cursorLine->setLine(newPosition, y1, newPosition, y2);
}


QFont RulerScene::optimalFont(double lastHorizontalUnit, double pixelsPerUnit, unsigned& numberSkipInterval) {
    int numberDigits = std::ceil(std::log10(lastHorizontalUnit));
    if (numberDigits < 1) {
        numberDigits = 1;
    }

    QString largestNumber;
    while (largestNumber.length() < numberDigits) {
        largestNumber.append(tr("8")); // Widest digit for locale.
    }

    QFont currentFont = Application::font();

    numberSkipInterval = 1;

    unsigned pointSize        = maximumFontSize;
    unsigned maximumFontWidth = pixelsPerUnit / 6;
    unsigned fontWidth;

    bool success = false;
    do {
        currentFont.setPointSize(pointSize);
        QFontMetrics metrics(currentFont);

        fontWidth = metrics.width(largestNumber);

        if (fontWidth > maximumFontWidth * numberSkipInterval) {
            if (pointSize <= minimumFontSize) {
                ++numberSkipInterval;
                pointSize = maximumFontSize;
            } else {
                --pointSize;
            }
        } else {
            success = true;
        }
    } while (!success);

    return currentFont;
}


RulerScene::VerticalPositions RulerScene::calculateImperialVerticalPositions(const QFont& font) {
    VerticalPositions vp;

    QFontMetrics metrics(font);

    unsigned fontHeight = metrics.height();
    unsigned tickDelta  = fontHeight / 4;
    unsigned fontBottom = fontSpacing + fontHeight;
    unsigned height     = fontBottom + fontSpacing + numberImperialTicks * tickDelta;

    vp.fontTop     = fontSpacing;
    vp.fontBottom  = fontBottom;
    vp.divisor     = imperialDivisor;

    vp.lineStarts.resize(imperialDivisor);
    vp.lineStarts[0] = height - numberImperialTicks * tickDelta;

    for (unsigned masked=1 ; masked<imperialDivisor ; ++masked) {
        unsigned shifted = masked;
        unsigned length  = 1;
        while ((shifted & 1) == 0) {
            ++length;
            shifted >>= 1;
        }

        vp.lineStarts[masked] = height - length * tickDelta;
    }

    emit heightChanged(height);
    currentHeight = height;

    return vp;
}


RulerScene::VerticalPositions RulerScene::calculateMetricVerticalPositions(const QFont& font) {
    VerticalPositions vp;

    QFontMetrics metrics(font);

    unsigned fontHeight = metrics.height();
    unsigned tickDelta  = fontHeight / 4;
    unsigned fontBottom = fontSpacing + fontHeight;
    unsigned height     = fontBottom + fontSpacing + 3 * tickDelta;

    vp.fontTop     = fontSpacing;
    vp.fontBottom  = fontBottom;
    vp.divisor     = metricDivisor;

    vp.lineStarts.resize(metricDivisor);
    vp.lineStarts[0] = height - 2 * tickDelta;

    for (unsigned masked=1 ; masked<metricDivisor ; ++masked) {
        vp.lineStarts[masked] = height - 1 * tickDelta;
    }

    emit heightChanged(height);
    currentHeight = height;

    return vp;
}


void RulerScene::redraw() {
    // Note that the positioning and alignment of primitives must closely match those setup in the Page class.  We rely
    // on both sets of GraphicsItems centering identically across the screen.

    bool   cursorLineIsVisible = true;
    double cursorLinePosition  = 0;

    if (cursorLine != Q_NULLPTR) {
        cursorLineIsVisible = cursorLine->isVisible();
        cursorLinePosition  = cursorLine->x();
    }

    clear();

    QSizeF dpi = QSizeF(Application::physicalDpi());
    if (currentImperialUnits) {
        pixelsPerUnit = dpi.width() * currentZoomFactor;
    } else {
        pixelsPerUnit = dpi.width() * currentZoomFactor * Util::Units::inchesPerCm;
    }

    unsigned numberRulers = currentPositions.size();
    for (unsigned rulerNumber=0 ; rulerNumber<numberRulers ; ++rulerNumber) {
        // First we figure out what font we're going to use.

        double horizontalUnits;

        if (currentImperialUnits) {
            horizontalUnits = currentLengths[rulerNumber] / Util::Units::pointsPerInch;
        } else {
            horizontalUnits = currentLengths[rulerNumber] / Util::Units::pointsPerCm;
        }

        double leftEdge          =   currentPositions[rulerNumber] * dpi.width() * Util::Units::inchesPerPoint;
        double shadowWidthPixels = PageList::shadowWidthPoints * dpi.width() * Util::Units::inchesPerPoint;

        double rightEdge = horizontalUnits * pixelsPerUnit + leftEdge;

        unsigned numberSkipInterval;
        QFont    currentFont = optimalFont(horizontalUnits, pixelsPerUnit, numberSkipInterval);

        setFont(currentFont);

        VerticalPositions positions =   currentImperialUnits
                                      ? calculateImperialVerticalPositions(currentFont)
                                      : calculateMetricVerticalPositions(currentFont);

        double   pixelsPerTick = static_cast<double>(pixelsPerUnit) / static_cast<double>(positions.divisor);
        unsigned endingTick    = horizontalUnits * positions.divisor;

        // Rectangle below won't be visible and exists to force the ruler to center identically with the page.  The area
        // this rectangle takes up matches the space required for the shadows.

        addRect(
            0 + leftEdge,
            0,
            pixelsPerUnit * horizontalUnits,
            currentHeight - 1,
            QPen(Qt::NoPen),
            PageList::pageContentsBackgroundBrush
        );

        for (unsigned tick=0 ; tick<=endingTick ; ++tick) {
            double   x = tick * pixelsPerTick + leftEdge;
            unsigned r = tick % positions.divisor;
            addLine(x, positions.lineStarts[r], x, currentHeight - 1);
        }

        QFontMetrics metrics(currentFont);

        double labelX;
        double width;
        for (unsigned major=0 ; major<=horizontalUnits ; major += numberSkipInterval) {
            QString label = QString::number(major);

            width  = metrics.width(label);
            labelX = pixelsPerUnit * major - width / 2.0 + leftEdge;

            if (labelX < leftEdge) {
                labelX = leftEdge;
            }

            if (labelX > rightEdge - width + shadowWidthPixels) {
                labelX = rightEdge - width + shadowWidthPixels;
            }

            QGraphicsSimpleTextItem* textItem = addSimpleText(label, currentFont);
            textItem->setPos(labelX, positions.fontTop);
        }

        // Only put a units label on if there is room.

        unsigned rightMostLabelEdge = labelX + width + 1 + shadowWidthPixels;
        QString  unitsLabel         = currentImperialUnits ? tr("in") : tr("cm");
        unsigned unitsWidth         = metrics.width(unitsLabel);
        unsigned margin             = metrics.width(tr(" "));

        double   unitsLabelX        = rightEdge - unitsWidth;
        double   unitsLabelY        = positions.fontTop;

        if (unitsLabelX >= rightMostLabelEdge + margin) {
            QGraphicsSimpleTextItem* unitsLabelItem = addSimpleText(unitsLabel, currentFont);
            unitsLabelItem->setPos(unitsLabelX, unitsLabelY);
        }
    }

    cursorLine = addLine(
        cursorLinePosition,
        0,
        cursorLinePosition,
        currentHeight - 1
    );

    cursorLine->setVisible(cursorLineIsVisible);
}
