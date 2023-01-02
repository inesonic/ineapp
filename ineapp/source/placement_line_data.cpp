/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlacementLineData class.
***********************************************************************************************************************/

#include "placement_line_data.h"

PlacementLineData::PlacementLineData() {
    currentChildIdentifier    = 0;
    currentPresentationAreaId = 0;
    currentWidth              = 0;
    currentHeight             = 0;
    currentAscent             = 0;
    currentlyCanStretch       = false;
}


PlacementLineData::PlacementLineData(
        unsigned long newChildIdentifier,
        unsigned long newPresentationAreaId,
        double        newWidth,
        double        newHeight,
        double        newAscent,
        bool          canStretch
    ) {
    currentChildIdentifier    = newChildIdentifier;
    currentPresentationAreaId = newPresentationAreaId;
    currentWidth              = newWidth;
    currentHeight             = newHeight;
    currentAscent             = newAscent;
    currentlyCanStretch       = canStretch;
}


PlacementLineData::PlacementLineData(const PlacementLineData& other) {
    currentChildIdentifier    = other.currentChildIdentifier;
    currentPresentationAreaId = other.currentPresentationAreaId;
    currentWidth              = other.currentWidth;
    currentHeight             = other.currentHeight;
    currentAscent             = other.currentAscent;
    currentlyCanStretch       = other.currentlyCanStretch;
}


PlacementLineData::~PlacementLineData() {}


void PlacementLineData::setChildIdentifier(unsigned long newChildIdentifier) {
    currentChildIdentifier = newChildIdentifier;
}


unsigned long PlacementLineData::childIdentifier() const {
    return currentChildIdentifier;
}


void PlacementLineData::setPresentationAreaId(unsigned long newPresentationAreaId) {
    currentPresentationAreaId = newPresentationAreaId;
}


unsigned long PlacementLineData::presentationAreaId() const {
    return currentPresentationAreaId;
}


void PlacementLineData::setWidth(double newWidth) {
    currentWidth = newWidth;
}


double PlacementLineData::width() const {
    return currentWidth;
}


void PlacementLineData::setHeight(double newHeight) {
    currentHeight = newHeight;
}


double PlacementLineData::height() const {
    return currentHeight;
}


void PlacementLineData::setAscent(double newAscent) {
    currentAscent = newAscent;
}


double PlacementLineData::ascent() const {
    return currentAscent;
}


void PlacementLineData::setStretchable(bool nowCanStretch) {
    currentlyCanStretch = nowCanStretch;
}


bool PlacementLineData::canStretch() const {
    return currentlyCanStretch;
}


PlacementLineData& PlacementLineData::operator=(const PlacementLineData& other) {
    currentChildIdentifier    = other.currentChildIdentifier;
    currentPresentationAreaId = other.currentPresentationAreaId;
    currentWidth              = other.currentWidth;
    currentHeight             = other.currentHeight;
    currentAscent             = other.currentAscent;
    currentlyCanStretch       = other.currentlyCanStretch;

    return *this;
}
