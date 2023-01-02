/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationAreaTracker class.
***********************************************************************************************************************/

#include <QList>
#include <QSizeF>
#include <QGraphicsItem>

#include "presentation.h"
#include "presentation_area_tracker.h"

/***********************************************************************************************************************
 * PresentationAreaTracker::Entry
 */

PresentationAreaTracker::Entry::Entry(
        Presentation* presentation,
        unsigned long areaId,
        const QSizeF& areaSize,
        float         ascent
    ) {
    currentPresentation = presentation;
    currentAreaId       = areaId;
    currentAreaSize     = areaSize;
    currentAscent       = ascent;
}


PresentationAreaTracker::Entry::Entry() {
    currentPresentation = Q_NULLPTR;
    currentAreaId       = static_cast<unsigned long>(-1);
    currentAreaSize     = QSizeF();
    currentAscent       = 0.0F;
}


PresentationAreaTracker::Entry::Entry(const PresentationAreaTracker::Entry& other) {
    currentPresentation = other.currentPresentation;
    currentAreaId       = other.currentAreaId;
    currentAreaSize     = other.currentAreaSize;
    currentAscent       = other.currentAscent;
}


PresentationAreaTracker::Entry::~Entry() {}


bool PresentationAreaTracker::Entry::isValid() const {
    return currentPresentation != Q_NULLPTR;
}


bool PresentationAreaTracker::Entry::isInvalid() const {
    return currentPresentation == Q_NULLPTR;
}


Presentation* PresentationAreaTracker::Entry::presentation() const {
    return currentPresentation;
}


unsigned long PresentationAreaTracker::Entry::areaId() const {
    return currentAreaId;
}


const QSizeF& PresentationAreaTracker::Entry::areaSize() const {
    return currentAreaSize;
}


float PresentationAreaTracker::Entry::ascent() const {
    return currentAscent;
}


QGraphicsItem* PresentationAreaTracker::Entry::graphicsItem() const {
    return currentPresentation->graphicsItem(currentAreaId);
}


PresentationAreaTracker::Entry& PresentationAreaTracker::Entry::operator=(const PresentationAreaTracker::Entry& other) {
    currentPresentation = other.currentPresentation;
    currentAreaId       = other.currentAreaId;
    currentAreaSize     = other.currentAreaSize;
    currentAscent       = other.currentAscent;

    return *this;
}

/***********************************************************************************************************************
 * PresentationAreaTracker
 */

const PresentationAreaTracker::Entry PresentationAreaTracker::nullEntry;

PresentationAreaTracker::PresentationAreaTracker() {
    clear();
}


PresentationAreaTracker::PresentationAreaTracker(const PresentationAreaTracker& other) {
    currentPresentation   = other.currentPresentation;
    currentEntries        = other.currentEntries;
    currentWidth          = other.currentWidth;
    currentMaximumAscent  = other.currentMaximumAscent;
    currentMaximumDescent = other.currentMaximumDescent;
}


PresentationAreaTracker::~PresentationAreaTracker() {}


void PresentationAreaTracker::setChildPresentation(Presentation* childPresentation) {
    currentEntries.clear();

    currentPresentation   = childPresentation;
    currentWidth          = 0;
    currentMaximumAscent  = 0;
    currentMaximumDescent = 0;
}


void PresentationAreaTracker::clear() {
    currentEntries.clear();

    currentPresentation   = Q_NULLPTR;
    currentWidth          = 0;
    currentMaximumAscent  = 0;
    currentMaximumDescent = 0;
}


void PresentationAreaTracker::allocateArea(unsigned long presentationAreaId, const QSizeF& size, float ascent) {
    currentEntries.append(Entry(currentPresentation, presentationAreaId, size, ascent));

    if (currentMaximumAscent < ascent) {
        currentMaximumAscent = ascent;
    }

    float descent = size.height() - ascent;
    if (currentMaximumDescent < descent) {
        currentMaximumDescent = descent;
    }

    currentWidth += size.width();
}


bool PresentationAreaTracker::containsChildData() const {
    return currentPresentation != Q_NULLPTR;
}


Presentation* PresentationAreaTracker::childPresentation() const {
    return currentPresentation;
}


QSizeF PresentationAreaTracker::boundingRectangle() const {
    return QSizeF(currentWidth, currentMaximumAscent + currentMaximumDescent);
}


float PresentationAreaTracker::width() const {
    return currentWidth;
}


float PresentationAreaTracker::maximumAscent() const {
    return currentMaximumAscent;
}


float PresentationAreaTracker::maximumDescent() const {
    return currentMaximumDescent;
}


void PresentationAreaTracker::resetPlacement() {
    if (currentPresentation != Q_NULLPTR) {
        currentPresentation->resetPlacement();
    }
}


void PresentationAreaTracker::removeFromScene() {
    if (currentPresentation != Q_NULLPTR) {
        currentPresentation->removeFromScene();
        currentPresentation = Q_NULLPTR;
    }
}


unsigned long PresentationAreaTracker::numberPresentationAreas() const {
    return static_cast<unsigned long>(currentEntries.size());
}


PresentationAreaTracker::Entry& PresentationAreaTracker::entry(unsigned index) {
    Q_ASSERT(index < static_cast<unsigned>(currentEntries.size()));
    return currentEntries[index];
}


const PresentationAreaTracker::Entry& PresentationAreaTracker::entry(unsigned index) const {
    Q_ASSERT(index < static_cast<unsigned>(currentEntries.size()));
    return currentEntries.at(index);
}


PresentationAreaTracker::Entry& PresentationAreaTracker::operator[](unsigned index) {
    Q_ASSERT(index < static_cast<unsigned>(currentEntries.size()));
    return currentEntries[index];
}


const PresentationAreaTracker::Entry& PresentationAreaTracker::operator[](unsigned index) const {
    Q_ASSERT(index < static_cast<unsigned>(currentEntries.size()));
    return currentEntries.at(index);
}


PresentationAreaTracker::Iterator PresentationAreaTracker::begin() const {
    return currentEntries.constBegin();
}


PresentationAreaTracker::Iterator PresentationAreaTracker::end() const {
    return currentEntries.constEnd();
}


PresentationAreaTracker& PresentationAreaTracker::operator=(const PresentationAreaTracker& other) {
    currentPresentation   = other.currentPresentation;
    currentEntries        = other.currentEntries;
    currentWidth          = other.currentWidth;
    currentMaximumAscent  = other.currentMaximumAscent;
    currentMaximumDescent = other.currentMaximumDescent;

    return *this;
}
