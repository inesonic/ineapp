/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Page class.
***********************************************************************************************************************/

#include <QMarginsF>
#include <QList>
#include <QSizeF>
#include <QRectF>
#include <QPolygonF>
#include <QColor>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QLinearGradient>
#include <QGraphicsRectItem>
#include <QSharedPointer>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>

#include <util_units.h>

#include <eqt_graphics_scene.h>
#include <eqt_graphics_item_group.h>

#include <ld_page_format.h>

#include "presentation.h"
#include "page_list.h"
#include "page_list_page.h"

PageList::Page::Page(QSharedPointer<Ld::PageFormat> newPageFormat, QGraphicsItem* parent):QGraphicsItem(parent) {
    currentIndex  = PageList::invalidPageIndex;
    currentFormat = newPageFormat;

    pageBackgroundRect = new QGraphicsRectItem(this);
    pageBackgroundRect->setPen(Qt::NoPen);
    pageBackgroundRect->setBrush(pageContentsBackgroundBrush);

    QLinearGradient rightGradient(0.0, 0.5, 1.0, 0.5);
    rightGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    rightGradient.setColorAt(0.0, shadowColor1);
    rightGradient.setColorAt(1.0, shadowColor2);

    rightPageShadowPolygon = new QGraphicsPolygonItem(this);
    rightPageShadowPolygon->setPen(Qt::NoPen);
    rightPageShadowPolygon->setBrush(QBrush(rightGradient));

    QLinearGradient bottomGradient(0.5, 0.0, 0.5, 1.0);
    bottomGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    bottomGradient.setColorAt(0.0, shadowColor1);
    bottomGradient.setColorAt(1.0, shadowColor2);

    bottomPageShadowPolygon = new QGraphicsPolygonItem(this);
    bottomPageShadowPolygon->setPen(Qt::NoPen);
    bottomPageShadowPolygon->setBrush(QBrush(bottomGradient));

    currentActiveArea = new EQt::GraphicsItemGroup;
    currentActiveArea->setParentItem(this);
    setZValue(Presentation::pageZHeight);

    calculatePageDimensions();
}


PageList::Page::~Page() {
    delete pageBackgroundRect;
    delete rightPageShadowPolygon;
    delete bottomPageShadowPolygon;
    delete currentActiveArea;

    setParentItem(Q_NULLPTR);
    scene()->removeItem(this);
}


QSharedPointer<Ld::PageFormat> PageList::Page::format() const {
    return currentFormat;
}


void PageList::Page::setFormat(QSharedPointer<Ld::PageFormat> newPageFormat) {
    currentFormat = newPageFormat;
    calculatePageDimensions();
}


PageList::Index PageList::Page::index() const {
    return currentIndex;
}


void PageList::Page::setIndex(PageList::Index newPageIndex) {
    currentIndex = newPageIndex;
}


const QRectF& PageList::Page::extentsSceneUnits() const {
    return currentExtentsSceneUnits;
}


const QRectF& PageList::Page::boundarySceneUnits() const {
    return currentBoundarySceneUnits;
}


const QRectF& PageList::Page::activeAreaSceneUnits() const {
    return currentActiveAreaSceneUnits;
}


EQt::GraphicsItemGroup* PageList::Page::activeAreaGroup() const {
    return currentActiveArea;
}


QSizeF PageList::Page::activeAreaSizeSceneUnits() const {
    return currentActiveAreaSceneUnits.size();
}


QRectF PageList::Page::boundingRect() const {
    return currentExtentsSceneUnits;
}


void PageList::Page::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}


void PageList::Page::calculatePageDimensions() {
    QSizeF pageSize = QSizeF(currentFormat->horizontalExtents(), currentFormat->verticalExtents());
    QPointF origin = QPointF(0, 0);
    QMarginsF margins = QMarginsF(
        currentFormat->leftMargin(),
        currentFormat->topMargin(),
        currentFormat->rightMargin(),
        currentFormat->bottomMargin()
    );
    QRectF boundaryPoints = QRectF(origin, pageSize);
    QRectF extentsPoints = QRectF(origin, pageSize + QSizeF(shadowWidthPoints, shadowWidthPoints));
    QRectF activeAreaPoints = boundaryPoints.marginsRemoved(margins);

    currentExtentsSceneUnits    = toScene(extentsPoints);
    currentBoundarySceneUnits   = toScene(boundaryPoints);
    currentActiveAreaSceneUnits = toScene(activeAreaPoints);

    currentActiveArea->setPos(currentActiveAreaSceneUnits.topLeft());

    pageBackgroundRect->setRect(currentBoundarySceneUnits);

    QPointF shadowDiagonal = toScene(QPointF(shadowWidthPoints, shadowWidthPoints));

    QPolygonF rightPolygon;
    rightPolygon << currentBoundarySceneUnits.topRight() + shadowDiagonal
                 << currentBoundarySceneUnits.topRight()
                 << currentBoundarySceneUnits.bottomRight()
                 << currentExtentsSceneUnits.bottomRight()
                 << currentBoundarySceneUnits.topRight() + shadowDiagonal;

    rightPageShadowPolygon->setPolygon(rightPolygon);

    QPolygonF bottomPolygon;
    bottomPolygon << currentExtentsSceneUnits.bottomRight()
                  << currentBoundarySceneUnits.bottomRight()
                  << currentBoundarySceneUnits.bottomLeft()
                  << currentBoundarySceneUnits.bottomLeft() + shadowDiagonal
                  << currentExtentsSceneUnits.bottomRight();

    bottomPageShadowPolygon->setPolygon(bottomPolygon);
}
