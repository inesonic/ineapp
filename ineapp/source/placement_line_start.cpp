/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlacementLineStart class.
***********************************************************************************************************************/

#include <QHash>

#include <util_hash_functions.h>

#include "presentation_area.h"
#include "placement_line_start.h"

PlacementLineStart::PlacementLineStart(
        double        newLineY,
        double        newMinimumTopSpacing,
        unsigned long newChildIdentifier,
        unsigned long newPresentationAreaId
    ):PresentationArea(
        newChildIdentifier,
        newPresentationAreaId
    ) {
    currentLineY             = newLineY;
    currentMinimumTopSpacing = newMinimumTopSpacing;
}


PlacementLineStart::PlacementLineStart(const PlacementLineStart& other):PresentationArea(other) {
    currentLineY             = other.currentLineY;
    currentMinimumTopSpacing = other.currentMinimumTopSpacing;
}


PlacementLineStart::~PlacementLineStart() {}


void PlacementLineStart::setLineY(double newLineY) {
    currentLineY = newLineY;
}


double PlacementLineStart::lineY() const {
    return currentLineY;
}


void PlacementLineStart::setMinimumTopSpacing(double newMinimumTopSpacing) {
    currentMinimumTopSpacing = newMinimumTopSpacing;
}


double PlacementLineStart::minimumTopSpacing() const {
    return currentMinimumTopSpacing;
}


PlacementLineStart& PlacementLineStart::operator=(const PlacementLineStart& other) {
    PresentationArea::operator=(other);

    currentLineY             = other.currentLineY;
    currentMinimumTopSpacing = other.currentMinimumTopSpacing;

    return *this;
}


Util::HashResult qHash(const PlacementLineStart& lineStart, Util::HashSeed seed) {
    return (
          qHash(lineStart.lineY(), seed)
        ^ qHash(lineStart.minimumTopSpacing(), seed)
        ^ qHash(static_cast<const PresentationArea&>(lineStart), seed)
    );
}
