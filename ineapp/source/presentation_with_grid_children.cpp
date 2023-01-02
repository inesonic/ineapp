/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationWithGridChildren class.
***********************************************************************************************************************/

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_grid_children.h>

#include "presentation.h"
#include "presentation_with_grid_children.h"

PresentationWithGridChildren::PresentationWithGridChildren() {
    connect(
        this,
        SIGNAL(geometryWasChanged(unsigned long,unsigned long,unsigned long,unsigned long)),
        this,
        SLOT(processGeometryChanged(unsigned long, unsigned long, unsigned long, unsigned long)),
        connectionType
    );

    connect(
        this,
        SIGNAL(aboutToRemoveRowCalled(unsigned long)),
        this,
        SLOT(processAboutToRemoveRow(unsigned long)),
        connectionType
    );

    connect(
        this,
        SIGNAL(aboutToRemoveColumnCalled(unsigned long)),
        this,
        SLOT(processAboutToRemoveColumn(unsigned long)),
        connectionType
    );

    connect(
        this,
        SIGNAL(rowWasInserted(unsigned long)),
        this,
        SLOT(processRowInserted(unsigned long)),
        connectionType
    );

    connect(
        this,
        SIGNAL(columnWasInserted(unsigned long)),
        this,
        SLOT(processColumnInserted(unsigned long)),
        connectionType
    );
}


PresentationWithGridChildren::~PresentationWithGridChildren() {}


Ld::Element::ChildPlacement PresentationWithGridChildren::childPlacement() const {
    return Ld::Element::ChildPlacement::GRID;
}


void PresentationWithGridChildren::processGeometryChanged(
        unsigned long /* oldNumberRows */,
        unsigned long /* oldNumberColumns */,
        unsigned long /* newNumberRows */,
        unsigned long /* newNumberColumns */
    ) {}


void PresentationWithGridChildren::processAboutToRemoveRow(unsigned long /* rowIndex */) {}


void PresentationWithGridChildren::processAboutToRemoveColumn(unsigned long /* columnIndex */) {}


void PresentationWithGridChildren::processRowInserted(unsigned long /* rowIndex */) {}


void PresentationWithGridChildren::processColumnInserted(unsigned long /* columnIndex */) {}


void PresentationWithGridChildren::geometryChanged(
        unsigned long oldNumberRows,
        unsigned long oldNumberColumns,
        unsigned long newNumberRows,
        unsigned long newNumberColumns
    ) {
    emit geometryWasChanged(oldNumberRows, oldNumberColumns, newNumberRows, newNumberColumns);
}


void PresentationWithGridChildren::aboutToRemoveRow(unsigned long rowIndex) {
    emit aboutToRemoveRowCalled(rowIndex);
}


void PresentationWithGridChildren::aboutToRemoveColumn(unsigned long columnIndex) {
    emit aboutToRemoveColumnCalled(columnIndex);
}


void PresentationWithGridChildren::rowInserted(unsigned long rowIndex) {
    emit rowWasInserted(rowIndex);
}


void PresentationWithGridChildren::columnInserted(unsigned long columnIndex) {
    emit columnWasInserted(columnIndex);
}
