/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFramePresentation::Rectangle class.
***********************************************************************************************************************/

#include <QRectF>
#include <QPointF>
#include <QSizeF>

#include "table_frame_presentation.h"
#include "table_frame_presentation_rectangle.h"

TableFramePresentation::Rectangle::Rectangle() {
    currentPresentationAreaId = 0;
}


TableFramePresentation::Rectangle::Rectangle(
            unsigned long  presentationAreaId,
            const QPointF& topLeft,
            const QSizeF&  size
        ):QRectF(
            topLeft,
            size
        ) {
    currentPresentationAreaId = presentationAreaId;
}


TableFramePresentation::Rectangle::Rectangle(
        unsigned long presentationAreaId,
        const QPointF& topLeft,
        const QPointF& bottomRight
    ):QRectF(
        topLeft,
        bottomRight
    ) {
    currentPresentationAreaId = presentationAreaId;
}


TableFramePresentation::Rectangle::Rectangle(
        unsigned long presentationAreaId,
        float x,
        float y,
        float width,
        float height
    ):QRectF(
         x,
         y,
         width,
         height
    ) {
    currentPresentationAreaId = presentationAreaId;
}


TableFramePresentation::Rectangle::Rectangle(const TableFramePresentation::Rectangle& other):QRectF(other) {
    currentPresentationAreaId = other.currentPresentationAreaId;
}


TableFramePresentation::Rectangle::~Rectangle() {}


unsigned long TableFramePresentation::Rectangle::presentationAreaId() const {
    return currentPresentationAreaId;
}


void TableFramePresentation::Rectangle::setPresentationAreaId(unsigned long newPresentationAreaId) {
    currentPresentationAreaId = newPresentationAreaId;
}


TableFramePresentation::Rectangle& TableFramePresentation::Rectangle::operator=(
        const TableFramePresentation::Rectangle& other
    ) {
    QRectF::operator=(other);
    currentPresentationAreaId = other.currentPresentationAreaId;

    return *this;
}


bool TableFramePresentation::Rectangle::operator==(const TableFramePresentation::Rectangle& other) const {
    return static_cast<const QRectF&>(other) == *this && other.currentPresentationAreaId == currentPresentationAreaId;
}


bool TableFramePresentation::Rectangle::operator!=(const TableFramePresentation::Rectangle& other) const {
    return static_cast<const QRectF&>(other) != *this || other.currentPresentationAreaId != currentPresentationAreaId;
}

