/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationArea class.
***********************************************************************************************************************/

#include <QHash>

#include <util_hash_functions.h>

#include "presentation_area.h"

PresentationArea::PresentationArea(unsigned long newChildIdentifier, unsigned long newPresentationAreaId) {
    currentChildIdentifier    = newChildIdentifier;
    currentPresentationAreaId = newPresentationAreaId;
}


PresentationArea::PresentationArea(const PresentationArea& other) {
    currentChildIdentifier    = other.currentChildIdentifier;
    currentPresentationAreaId = other.currentPresentationAreaId;
}


PresentationArea::~PresentationArea() {}


void PresentationArea::setChildIdentifier(unsigned long newChildIdentifier) {
    currentChildIdentifier = newChildIdentifier;
}


unsigned long PresentationArea::childIdentifier() const {
    return currentChildIdentifier;
}


void PresentationArea::setPresentationAreaId(unsigned long newPresentationAreaId) {
    currentPresentationAreaId = newPresentationAreaId;
}


unsigned long PresentationArea::presentationAreaId() const {
    return currentPresentationAreaId;
}


PresentationArea& PresentationArea::operator=(const PresentationArea& other) {
    currentChildIdentifier    = other.currentChildIdentifier;
    currentPresentationAreaId = other.currentPresentationAreaId;

    return *this;
}


bool PresentationArea::operator==(const PresentationArea& other) const {
    return (currentChildIdentifier == other.currentChildIdentifier       &&
            currentPresentationAreaId == other.currentPresentationAreaId
           );
}


bool PresentationArea::operator!=(const PresentationArea& other) const {
    return (currentChildIdentifier != other.currentChildIdentifier       ||
            currentPresentationAreaId != other.currentPresentationAreaId
           );
}


bool PresentationArea::operator<(const PresentationArea& other) const {
    return (currentChildIdentifier < other.currentChildIdentifier             ||
            (currentChildIdentifier == other.currentChildIdentifier      &&
             currentPresentationAreaId < other.currentPresentationAreaId    )    );
}


bool PresentationArea::operator>(const PresentationArea& other) const {
    return (currentChildIdentifier > other.currentChildIdentifier             ||
            (currentChildIdentifier == other.currentChildIdentifier      &&
             currentPresentationAreaId > other.currentPresentationAreaId    )    );
}


bool PresentationArea::operator<=(const PresentationArea& other) const {
    return (currentChildIdentifier < other.currentChildIdentifier              ||
            (currentChildIdentifier == other.currentChildIdentifier       &&
             currentPresentationAreaId <= other.currentPresentationAreaId    )    );
}


bool PresentationArea::operator>=(const PresentationArea& other) const {
    return (currentChildIdentifier > other.currentChildIdentifier              ||
            (currentChildIdentifier == other.currentChildIdentifier       &&
             currentPresentationAreaId >= other.currentPresentationAreaId    )    );
}


PresentationArea::EncodedValue PresentationArea::encodedValue() const {
    return (static_cast<EncodedValue>(currentChildIdentifier) << 32) | currentPresentationAreaId;
}


Util::HashResult qHash(const PresentationArea& presentationArea, Util::HashSeed seed) {
    return (
          qHash(presentationArea.childIdentifier(), seed)
        ^ qHash(presentationArea.presentationAreaId(), seed)
    );
}
