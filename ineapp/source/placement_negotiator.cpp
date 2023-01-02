/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlacementNegotiator class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QPointF>
#include <QRectF>
#include <QPolygonF>
#include <QLineF>

#include <limits>

#include "placement_negotiator.h"

const double PlacementNegotiator::imposeNoTopSpacing = -1;

PlacementNegotiator::PlacementNegotiator() {}


PlacementNegotiator::~PlacementNegotiator() {}


float PlacementNegotiator::bottomSpacingSceneUnits() {
    return 0;
}
