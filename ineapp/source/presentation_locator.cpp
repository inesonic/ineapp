/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationLocator class.
***********************************************************************************************************************/

#include <QPointF>
#include <QSharedData>
#include <QSharedDataPointer>

#include "presentation.h"
#include "presentation_locator.h"

bool PresentationLocator::currentUseLeftToRightOrdering = true;

void PresentationLocator::setLeftToRightOrdering(bool nowLeftToRightOrder) {
    currentUseLeftToRightOrdering = nowLeftToRightOrder;
}


void PresentationLocator::setRightToLeftOrdering(bool nowRightToLeftOrder) {
    setLeftToRightOrdering(!nowRightToLeftOrder);
}


bool PresentationLocator::leftToRightOrdering() {
    return currentUseLeftToRightOrdering;
}


bool PresentationLocator::rightToLeftOrdering() {
    return !leftToRightOrdering();
}


PresentationLocator::PresentationLocator(double newX, double newY) {
    currentX = newX;
    currentY = newY;
}


PresentationLocator::PresentationLocator(const QPointF& newPosition) {
    currentX = newPosition.x();
    currentY = newPosition.y();
}


PresentationLocator::PresentationLocator(const PresentationLocator& other) {
    currentX = other.currentX;
    currentY = other.currentY;
}


PresentationLocator::~PresentationLocator() {}


void PresentationLocator::setPosition(double newX, double newY) {
    currentX = newX;
    currentY = newY;
}


void PresentationLocator::setPosition(const QPointF& newPosition) {
    currentX = newPosition.x();
    currentY = newPosition.y();
}


double PresentationLocator::x() const {
    return currentX;
}


double PresentationLocator::y() const {
    return currentY;
}


QPointF PresentationLocator::position() const {
    return QPointF(currentX, currentY);
}


PresentationLocator& PresentationLocator::operator=(const PresentationLocator& other) {
    currentX = other.currentX;
    currentY = other.currentY;

    return *this;
}


bool PresentationLocator::operator==(const PresentationLocator& other) const {
    return currentX == other.currentX && currentY == other.currentY;
}


bool PresentationLocator::operator!=(const PresentationLocator& other) const {
    return currentX != other.currentX || currentY != other.currentY;
}


bool PresentationLocator::operator<(const PresentationLocator& other) const {
    bool lessThan;

    if (currentY < other.currentY) {
        lessThan = true;
    } else if (currentY == other.currentY) {
        if (( currentUseLeftToRightOrdering && currentX < other.currentX) ||
            (!currentUseLeftToRightOrdering && currentX > other.currentX)   ) {
            lessThan = true;
        } else {
            lessThan = false;
        }
    } else {
        lessThan = false;
    }

    return lessThan;
}


bool PresentationLocator::operator>(const PresentationLocator& other) const {
    bool greaterThan;

    if (currentY > other.currentY) {
        greaterThan = true;
    } else if (currentY == other.currentY) {
        if (( currentUseLeftToRightOrdering && currentX > other.currentX) ||
            (!currentUseLeftToRightOrdering && currentX < other.currentX)   ) {
            greaterThan = true;
        } else {
            greaterThan = false;
        }
    } else {
        greaterThan = false;
    }

    return greaterThan;
}


bool PresentationLocator::operator<=(const PresentationLocator& other) const {
    bool lessOrEqual;

    if (currentY < other.currentY) {
        lessOrEqual = true;
    } else if (currentY == other.currentY) {
        if (( currentUseLeftToRightOrdering && currentX <= other.currentX) ||
            (!currentUseLeftToRightOrdering && currentX >= other.currentX)   ) {
            lessOrEqual = true;
        } else {
            lessOrEqual = false;
        }
    } else {
        lessOrEqual = false;
    }

    return lessOrEqual;
}


bool PresentationLocator::operator>=(const PresentationLocator& other) const {
    bool greaterOrEqual;

    if (currentY > other.currentY) {
        greaterOrEqual = true;
    } else if (currentY == other.currentY) {
        if (( currentUseLeftToRightOrdering && currentX >= other.currentX) ||
            (!currentUseLeftToRightOrdering && currentX <= other.currentX)   ) {
            greaterOrEqual = true;
        } else {
            greaterOrEqual = false;
        }
    } else {
        greaterOrEqual = false;
    }

    return greaterOrEqual;
}


