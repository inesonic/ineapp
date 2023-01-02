/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFramePresentation::RowLocation class.
***********************************************************************************************************************/

#include <util_bit_array.h>

#include "table_frame_presentation.h"
#include "table_frame_presentation_row_location.h"

TableFramePresentation::RowLocation::RowLocation() {
    currentPresentationArea = 0;
    currentY                = 0.0F;

    currentCellBorders.clear();
}


TableFramePresentation::RowLocation::RowLocation(
        unsigned long         presentationArea,
        float                 y,
        const Util::BitArray& cellBorders
    ) {
    currentPresentationArea = presentationArea;
    currentY                = y;
    currentCellBorders      = cellBorders;
}


TableFramePresentation::RowLocation::RowLocation(
        const TableFramePresentation::RowLocation& other
    ) {
    currentPresentationArea = other.currentPresentationArea;
    currentY                = other.currentY;
    currentCellBorders      = other.currentCellBorders;
}


unsigned long TableFramePresentation::RowLocation::presentationArea() const {
    return currentPresentationArea;
}


void TableFramePresentation::RowLocation::setPresentationArea(unsigned long newPresentationArea) {
    currentPresentationArea = newPresentationArea;
}


float TableFramePresentation::RowLocation::y() const {
    return currentY;
}


void TableFramePresentation::RowLocation::setY(const float newY) {
    currentY = newY;
}


const Util::BitArray& TableFramePresentation::RowLocation::cellBorders() const {
    return currentCellBorders;
}


void TableFramePresentation::RowLocation::setCellBorders(const Util::BitArray& newCellBorders) {
    currentCellBorders = newCellBorders;
}


void TableFramePresentation::RowLocation::setCellBordersSet(unsigned int columnIndex, unsigned int columnSpan) {
    unsigned endingColumnIndex = columnIndex + columnSpan - 1;

    if (currentCellBorders.length() <= endingColumnIndex) {
        currentCellBorders.resize(endingColumnIndex + 1);
    }

    currentCellBorders.setBits(columnIndex, endingColumnIndex);
}


void TableFramePresentation::RowLocation::setCellBordersCleared(unsigned int columnIndex, unsigned int columnSpan) {
    unsigned endingColumnIndex = columnIndex + columnSpan - 1;

    if (currentCellBorders.length() <= endingColumnIndex) {
        currentCellBorders.resize(endingColumnIndex + 1);
    }

    currentCellBorders.clearBits(columnIndex, endingColumnIndex);
}


TableFramePresentation::RowLocation& TableFramePresentation::RowLocation::operator=(
        const TableFramePresentation::RowLocation& other
    ) {
    currentPresentationArea = other.currentPresentationArea;
    currentY                = other.currentY;
    currentCellBorders      = other.currentCellBorders;

    return *this;

}


bool TableFramePresentation::RowLocation::operator==(const TableFramePresentation::RowLocation& other) const {
    return other.currentPresentationArea == currentPresentationArea && other.currentY == currentY;
}


bool TableFramePresentation::RowLocation::operator!=(const TableFramePresentation::RowLocation& other) const {
    return other.currentPresentationArea != currentPresentationArea || other.currentY != currentY;
}


bool TableFramePresentation::RowLocation::operator<(const TableFramePresentation::RowLocation& other) const {
    return (
           other.currentPresentationArea > currentPresentationArea
        || (other.currentPresentationArea == currentPresentationArea && other.currentY > currentY)
    );
}


bool TableFramePresentation::RowLocation::operator>(const TableFramePresentation::RowLocation& other) const {
    return (
           other.currentPresentationArea < currentPresentationArea
        || (other.currentPresentationArea == currentPresentationArea && other.currentY < currentY)
    );
}


bool TableFramePresentation::RowLocation::operator<=(const TableFramePresentation::RowLocation& other) const {
    return (
           other.currentPresentationArea > currentPresentationArea
        || (other.currentPresentationArea == currentPresentationArea && other.currentY >= currentY)
    );
}


bool TableFramePresentation::RowLocation::operator>=(const TableFramePresentation::RowLocation& other) const {
    return (
           other.currentPresentationArea < currentPresentationArea
        || (other.currentPresentationArea == currentPresentationArea && other.currentY <= currentY)
    );
}
