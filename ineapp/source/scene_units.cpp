/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SceneUnits class.
***********************************************************************************************************************/

#include <QSize>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QLineF>

#include <util_units.h>

#include "scene_units.h"

QSizeF SceneUnits::currentSceneUnitsPerPoint;

void SceneUnits::update(const QSize& physicalDpi) {
    currentSceneUnitsPerPoint = QSizeF(physicalDpi) * Util::Units::inchesPerPoint;
}


const QSizeF& SceneUnits::sceneUnitsPerPoint() {
    return currentSceneUnitsPerPoint;
}


QSizeF SceneUnits::toScene(const QSizeF& size) {
    return QSizeF(size.width() * currentSceneUnitsPerPoint.width(), size.height() * currentSceneUnitsPerPoint.height());
}


QPointF SceneUnits::toScene(const QPointF& point) {
    return QPointF(point.x() * currentSceneUnitsPerPoint.width(), point.y() * currentSceneUnitsPerPoint.height());
}


QRectF SceneUnits::toScene(const QRectF& rect) {
    return QRectF(toScene(rect.topLeft()), toScene(rect.size()));
}


QLineF SceneUnits::toScene(const QLineF& line) {
    return QLineF(toScene(line.p1()), toScene(line.p2()));
}


QSizeF SceneUnits::fromScene(const QSizeF& size) {
    return QSizeF(size.width() / currentSceneUnitsPerPoint.width(), size.height() / currentSceneUnitsPerPoint.height());
}


QPointF SceneUnits::fromScene(const QPointF& point) {
    return QPointF(point.x() / currentSceneUnitsPerPoint.width(), point.y() / currentSceneUnitsPerPoint.height());
}


QRectF SceneUnits::fromScene(const QRectF& rect) {
    return QRectF(fromScene(rect.topLeft()), fromScene(rect.size()));
}


QLineF SceneUnits::fromScene(const QLineF& line) {
    return QLineF(fromScene(line.p1()), fromScene(line.p2()));
}
