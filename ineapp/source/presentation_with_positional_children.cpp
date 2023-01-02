/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationWithPositionalChildren class.
***********************************************************************************************************************/

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_positional_children.h>

#include "presentation.h"
#include "presentation_with_positional_children.h"

PresentationWithPositionalChildren::PresentationWithPositionalChildren() {
    connect(
        this,
        SIGNAL(removingChildPresentation(unsigned long, Presentation*)),
        SLOT(processRemovingChildPresentation(unsigned long, Presentation*)),
        connectionType
    );

    connect(
        this,
        SIGNAL(childPresentationNowInsertedBefore(unsigned long, Presentation*)),
        SLOT(processChildPresentationInsertedBefore(unsigned long ,Presentation*)),
        connectionType
    );

    connect(
        this,
        SIGNAL(childPresentationNowInsertedAfter(unsigned long, Presentation*)),
        SLOT(processChildPresentationInsertedAfter(unsigned long, Presentation*)),
        connectionType
    );
}


PresentationWithPositionalChildren::~PresentationWithPositionalChildren() {}


Ld::Element::ChildPlacement PresentationWithPositionalChildren::childPlacement() const {
    return Ld::Element::ChildPlacement::POSITIONAL;
}


void PresentationWithPositionalChildren::processRemovingChildPresentation(unsigned long, Presentation*) {}


void PresentationWithPositionalChildren::processChildPresentationInsertedBefore(unsigned long, Presentation*) {}


void PresentationWithPositionalChildren::processChildPresentationInsertedAfter(unsigned long, Presentation*) {}


void PresentationWithPositionalChildren::aboutToRemoveChild(
        unsigned long      childIndex,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit removingChildPresentation(childIndex, childPresentation);
}


void PresentationWithPositionalChildren::childInsertedBefore(
        unsigned long      childIndex,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationNowInsertedBefore(childIndex, childPresentation);
}


void PresentationWithPositionalChildren::childInsertedAfter(
        unsigned long      childIndex,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationNowInsertedAfter(childIndex, childPresentation);
}
