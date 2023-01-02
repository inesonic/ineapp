/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Ruler class.
***********************************************************************************************************************/

#include <QScrollArea>
#include <QHBoxLayout>
#include <QWidget>
#include <QScrollBar>
#include <QRectF>

#include "page_list.h"
#include "ruler_scene.h"
#include "ruler.h"

Ruler::Ruler(QWidget* parent):QGraphicsView(parent) {
    configureWidget();
}


Ruler::~Ruler() {}


QList<double> Ruler::lengths() const {
    return rulerScene->lengths();
}


double Ruler::zoomFactor() const {
    return rulerScene->zoomFactor();
}


bool Ruler::imperialUnits() const {
    return rulerScene->imperialUnits();
}


bool Ruler::cursorBarIsVisible() const {
    return rulerScene->cursorBarIsVisible();
}


void Ruler::setLengthsAndPositions(
        QList<double> newRulerLengths,
        QList<double> newRulerPositions,
        double        newLeftEdge,
        double        newRightEdge
    ) {
    rulerScene->setLengthsAndPositions(newRulerLengths, newRulerPositions, newLeftEdge, newRightEdge);
}


void Ruler::setZoomFactor(double newZoomFactor) {
    rulerScene->setZoomFactor(newZoomFactor);
}


void Ruler::setHorizontalPosition(int newPosition) {
    QScrollBar* scrollBar = horizontalScrollBar();
    scrollBar->setValue(newPosition);
}


void Ruler::setImperialUnits(bool nowUsingImperialUnits) {
    rulerScene->setImperialUnits(nowUsingImperialUnits);
}


void Ruler::setHeight(int newHeight) {
    setFixedHeight(newHeight);
}


void Ruler::setCursorPointPosition(unsigned column, double newPosition) {
    rulerScene->setCursorPointPosition(column, newPosition);
}


void Ruler::setCursorSceneCoordinatePosition(double newPosition) {
    rulerScene->setCursorSceneCoordinatePosition(newPosition);
}


void Ruler::setCursorBarVisible(bool nowVisible) {
    rulerScene->setCursorBarVisible(nowVisible);
}


void Ruler::setSceneWidth(double leftEdge, double rightEdge) {
    QRectF newSceneRect = rulerScene->sceneRect();

    newSceneRect.setLeft(leftEdge);
    newSceneRect.setRight(rightEdge);

    rulerScene->setSceneRect(newSceneRect);
}


void Ruler::configureWidget() {
    rulerScene = new RulerScene(this);
    setScene(rulerScene);

    setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    setBackgroundBrush(QBrush(PageList::shadowColor2));

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFixedHeight(rulerScene->height());
    connect(rulerScene, SIGNAL(heightChanged(int)), this, SLOT(setHeight(int)));
}
