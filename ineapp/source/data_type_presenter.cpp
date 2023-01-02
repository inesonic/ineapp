/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DataTypePreenter class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QSizeF>

#include "data_type_presenter.h"

/***********************************************************************************************************************
 * DataTypePresenter::PresentationArea
 */

DataTypePresenter::PresentationArea::PresentationArea() {}


DataTypePresenter::PresentationArea::PresentationArea(
        QGraphicsItem* graphicsItem,
        const QSizeF&  size,
        float          ascent
    ):currentGraphicsItem(
        graphicsItem
    ),currentSize(
        size
    ),currentAscent(
        ascent
    ) {}


DataTypePresenter::PresentationArea::PresentationArea(const DataTypePresenter::PresentationArea& other) {
    currentGraphicsItem = other.currentGraphicsItem;
    currentSize         = other.currentSize;
    currentAscent       = other.currentAscent;
}


DataTypePresenter::PresentationArea::~PresentationArea() {}


void DataTypePresenter::PresentationArea::setGraphicsItem(QGraphicsItem* newGraphicsItem) {
    currentGraphicsItem = newGraphicsItem;
}


QGraphicsItem* DataTypePresenter::PresentationArea::graphicsItem() const {
    return currentGraphicsItem;
}


void DataTypePresenter::PresentationArea::setSize(const QSizeF& newSize) {
    currentSize = newSize;
}


const QSizeF& DataTypePresenter::PresentationArea::size() const {
    return currentSize;
}


void DataTypePresenter::PresentationArea::setAscent(float newAscent) {
    currentAscent = newAscent;
}


float DataTypePresenter::PresentationArea::ascent() const {
    return currentAscent;
}


DataTypePresenter::PresentationArea& DataTypePresenter::PresentationArea::operator=(
        const DataTypePresenter::PresentationArea& other
    ) {
    currentGraphicsItem = other.currentGraphicsItem;
    currentSize         = other.currentSize;
    currentAscent       = other.currentAscent;

    return *this;
}

/***********************************************************************************************************************
 * DataTypePresenter
 */

DataTypePresenter::DataTypePresenter() {}


DataTypePresenter::~DataTypePresenter() {}
