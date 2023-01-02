/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationWithFixedChildren class.
***********************************************************************************************************************/

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_fixed_children.h>

#include "presentation.h"
#include "presentation_with_fixed_children.h"

PresentationWithFixedChildren::PresentationWithFixedChildren() {
    connect(
        this,
        SIGNAL(childPresentationRemoved(unsigned long,Presentation*)),
        SLOT(processPresentationRemoved(unsigned long,Presentation*)),
        connectionType
    );

    connect(
        this,
        SIGNAL(childPresentationSet(unsigned long,Presentation*)),
        SLOT(processChildPresentationSet(unsigned long,Presentation*)),
        connectionType
    );
}


PresentationWithFixedChildren::~PresentationWithFixedChildren() {}


Ld::Element::ChildPlacement PresentationWithFixedChildren::childPlacement() const {
    return Ld::Element::ChildPlacement::FIXED;
}


void PresentationWithFixedChildren::processPresentationRemoved(unsigned long, Presentation*) {
    // Trigger repositioning needed.
}


void PresentationWithFixedChildren::processChildPresentationSet(unsigned long, Presentation*) {
    // Trigger repositioning needed.
}


void PresentationWithFixedChildren::aboutToRemoveChild(unsigned long childIndex, Ld::ElementPointer childElement) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationRemoved(childIndex, childPresentation);
}


void PresentationWithFixedChildren::childSet(unsigned long childIndex, Ld::ElementPointer childElement) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childPresentationSet(childIndex, childPresentation);
}
