/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RootChildLocation class.
***********************************************************************************************************************/

#include "root_child_location.h"

RootChildLocation::RootChildLocation(unsigned newTopPageIndex, float newTopY, float newMinimumTopSpacing) {
    currentTopPageIndex      = newTopPageIndex;
    currentTopY              = newTopY;
    currentBottomPageIndex   = newTopPageIndex;
    currentBottomY           = newTopY;
    currentMinimumTopSpacing = newMinimumTopSpacing;
}


RootChildLocation::RootChildLocation(const RootChildLocation& other) {
    currentTopPageIndex      = other.currentTopPageIndex;
    currentTopY              = other.currentTopY;
    currentMinimumTopSpacing = other.currentMinimumTopSpacing;
}


RootChildLocation::~RootChildLocation() {}


void RootChildLocation::setTopPageIndex(unsigned newTopPageIndex) {
    currentTopPageIndex = newTopPageIndex;
}


unsigned RootChildLocation::topPageIndex() const {
    return currentTopPageIndex;
}


void RootChildLocation::setTopY(float newTopY) {
    currentTopY = newTopY;
}


float RootChildLocation::topY() const {
    return currentTopY;
}


void RootChildLocation::setBottomPageIndex(unsigned newBottomPageIndex) {
    currentBottomPageIndex = newBottomPageIndex;
}


unsigned RootChildLocation::bottomPageIndex() const {
    return currentBottomPageIndex;
}


void RootChildLocation::setBottomY(float newBottomY) {
    currentBottomY = newBottomY;
}


float RootChildLocation::bottomY() const {
    return currentBottomY;
}


void RootChildLocation::setMinimumTopSpacing(float newMinimumTopSpacing) {
    currentMinimumTopSpacing = newMinimumTopSpacing;
}


float RootChildLocation::minimumTopSpacing() const {
    return currentMinimumTopSpacing;
}


void RootChildLocation::setTopLocation(unsigned newTopPageIndex, float newTopY, float newMinimumTopSpacing) {
    currentTopPageIndex      = newTopPageIndex;
    currentTopY              = newTopY;
    currentMinimumTopSpacing = newMinimumTopSpacing;
}


void RootChildLocation::setBottomLocation(unsigned newBottomPageIndex, float newBottomY) {
    currentBottomPageIndex = newBottomPageIndex;
    currentBottomY         = newBottomY;
}


RootChildLocation& RootChildLocation::operator=(const RootChildLocation& other) {
    currentTopPageIndex      = other.currentTopPageIndex;
    currentTopY              = other.currentTopY;
    currentMinimumTopSpacing = other.currentMinimumTopSpacing;

    return *this;
}


bool RootChildLocation::operator==(const RootChildLocation& other) const {
    return (
           other.currentTopPageIndex == currentTopPageIndex
        && other.currentTopY == currentTopY
        && other.currentMinimumTopSpacing == currentMinimumTopSpacing
    );
}


bool RootChildLocation::operator!=(const RootChildLocation& other) const {
    return (
           other.currentTopPageIndex != currentTopPageIndex
        || other.currentTopY != currentTopY
        || other.currentMinimumTopSpacing != currentMinimumTopSpacing
    );
}


bool RootChildLocation::operator<(const RootChildLocation& other) const {
    return (
           currentTopPageIndex < other.currentTopPageIndex
        || (currentTopPageIndex == other.currentTopPageIndex && currentTopY < other.currentTopY)
        || (   currentTopPageIndex == other.currentTopPageIndex
            && currentTopY == other.currentTopY
            && currentMinimumTopSpacing < other.currentMinimumTopSpacing
           )
    );
}


bool RootChildLocation::operator>(const RootChildLocation& other) const {
    return (
           currentTopPageIndex > other.currentTopPageIndex
        || (currentTopPageIndex == other.currentTopPageIndex && currentTopY > other.currentTopY)
        || (   currentTopPageIndex == other.currentTopPageIndex
            && currentTopY == other.currentTopY
            && currentMinimumTopSpacing > other.currentMinimumTopSpacing
           )
    );
}


bool RootChildLocation::operator<=(const RootChildLocation& other) const {
    return operator<(other) || operator==(other);
}


bool RootChildLocation::operator>=(const RootChildLocation& other) const {
    return operator>(other) || operator==(other);
}
