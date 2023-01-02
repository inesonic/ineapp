/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlacementTracker class.
***********************************************************************************************************************/

#include <QObject>

#include "placement_tracker.h"

PlacementTracker::PlacementTracker():currentAbortPlacement(false) {}


PlacementTracker::~PlacementTracker() {}


void PlacementTracker::requestPlacementAborted() {
    currentAbortPlacement = true;
}


void PlacementTracker::clearAbort() {
    currentAbortPlacement = false;
}
