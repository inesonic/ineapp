/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationWithFloatingChildren class.
***********************************************************************************************************************/

#include <QObject>
#include <QPointF>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_location.h>
#include <ld_visual_with_floating_children.h>

#include "presentation.h"
#include "presentation_with_floating_children.h"

PresentationWithFloatingChildren::PresentationWithFloatingChildren() {
    connect(
        this,
        SIGNAL(removingChildPresentation(const QPointF&, Presentation*)),
        SLOT(processRemovingChildPresentation(const QPointF&, Presentation*)),
        connectionType
    );

    connect(
        this,
        SIGNAL(childPresentationNowInserted(Presentation*, const QPointF&)),
        SLOT(processChildPresentationInserted(Presentation*, const QPointF&)),
        connectionType
    );

    connect(
        this,
        SIGNAL(childPresentationNowRelocated(Presentation*, const QPointF&, const QPointF&)),
        SLOT(processChildPresentationRelocated(Presentation*,const QPointF&, const QPointF&)),
        connectionType
    );
}


PresentationWithFloatingChildren::~PresentationWithFloatingChildren() {}


Ld::Element::ChildPlacement PresentationWithFloatingChildren::childPlacement() const {
    return Ld::Element::ChildPlacement::FLOATING;
}


void PresentationWithFloatingChildren::processRemovingChildPresentation(const QPointF&, Presentation*) {}


void PresentationWithFloatingChildren::processChildPresentationInserted(Presentation*, const QPointF&) {}


void PresentationWithFloatingChildren::processChildPresentationRelocated(
        Presentation*,
        const QPointF&,
        const QPointF&
    ) {}


void PresentationWithFloatingChildren::aboutToRemoveChild(
        const Ld::Location& location,
        Ld::ElementPointer  childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit removingChildPresentation(location.toQPointF(), childPresentation);
}


void PresentationWithFloatingChildren::childInserted(Ld::ElementPointer childElement, const Ld::Location& location) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationNowInserted(childPresentation, location.toQPointF());
}


void PresentationWithFloatingChildren::childRelocated(
        Ld::ElementPointer  childElement,
        const Ld::Location& oldLocation,
        const Ld::Location& newLocation
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationNowRelocated(childPresentation, oldLocation.toQPointF(), newLocation.toQPointF());
}
