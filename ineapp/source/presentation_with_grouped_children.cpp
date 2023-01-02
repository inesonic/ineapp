/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationWithGroupedChildren class.
***********************************************************************************************************************/

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual_with_grouped_children.h>

#include "presentation.h"
#include "presentation_with_grouped_children.h"

PresentationWithGroupedChildren::PresentationWithGroupedChildren() {
    connect(
        this,
        SIGNAL(removingChildFromGroup(unsigned long, unsigned, unsigned long, Presentation*)),
        SLOT(processAboutToRemoveChildFromGroup(unsigned long, unsigned, unsigned long, Presentation*)),
        connectionType
    );

    connect(
        this,
        SIGNAL(removingChildrenFromGroup(unsigned long, unsigned long, unsigned)),
        SLOT(processAboutToRemoveChildrenFromGroup(unsigned long,unsigned long,unsigned)),
        connectionType
    );

    connect(
        this,
        SIGNAL(childWasInsertedIntoGroupBefore(unsigned long, unsigned, unsigned long, Presentation*)),
        SLOT(processChildInsertedIntoGroupBefore(unsigned long, unsigned, unsigned long, Presentation*)),
        connectionType
    );

    connect(
        this,
        SIGNAL(childWasInsertedIntoGroupAfter(unsigned long,unsigned,unsigned long,Presentation*)),
        SLOT(processChildInsertedIntoGroupAfter(unsigned long,unsigned,unsigned long,Presentation*)),
        connectionType
    );

    connect(
        this,
        SIGNAL(groupsWereRemoved(unsigned,unsigned)),
        SLOT(processGroupsRemoved(unsigned,unsigned)),
        connectionType
    );

    connect(
        this,
        SIGNAL(groupsWereInserted(unsigned,unsigned)),
        SLOT(processGroupsInserted(unsigned,unsigned)),
        connectionType
    );
}


PresentationWithGroupedChildren::~PresentationWithGroupedChildren() {}


Ld::Element::ChildPlacement PresentationWithGroupedChildren::childPlacement() const {
    return Ld::Element::ChildPlacement::GROUPED;
}



void PresentationWithGroupedChildren::processAboutToRemoveChildFromGroup(
        unsigned long,
        unsigned,
        unsigned long,
        Presentation*
    ) {}


void PresentationWithGroupedChildren::processAboutToRemoveChildrenFromGroup(
        unsigned long,
        unsigned long,
        unsigned
    ) {}


void PresentationWithGroupedChildren::processChildInsertedIntoGroupBefore(
        unsigned long,
        unsigned,
        unsigned long,
        Presentation*
    ) {}


void PresentationWithGroupedChildren::processChildInsertedIntoGroupAfter(
        unsigned long,
        unsigned,
        unsigned long,
        Presentation*
    ) {}


void PresentationWithGroupedChildren::processGroupsRemoved(unsigned, unsigned) {}


void PresentationWithGroupedChildren::processGroupsInserted(unsigned, unsigned) {}


void PresentationWithGroupedChildren::aboutToRemoveChildFromGroup(
        unsigned long      childIndex,
        unsigned           groupIndex,
        unsigned long      childIndexInGroup,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit removingChildFromGroup(childIndex, groupIndex, childIndexInGroup, childPresentation);
}


void PresentationWithGroupedChildren::aboutToRemoveChildrenFromGroup(
        unsigned long  startingChildIndex,
        unsigned long  numberChildren,
        unsigned       groupIndex
    ) {
    emit removingChildrenFromGroup(startingChildIndex, numberChildren, groupIndex);
}


void PresentationWithGroupedChildren::childInsertedIntoGroupBefore(
        unsigned long      childIndex,
        unsigned           groupIndex,
        unsigned long      childIndexInGroup,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childWasInsertedIntoGroupBefore(childIndex, groupIndex, childIndexInGroup, childPresentation);
}


void PresentationWithGroupedChildren::childInsertedIntoGroupAfter(
        unsigned long      childIndex,
        unsigned           groupIndex,
        unsigned long      childIndexInGroup,
        Ld::ElementPointer childElement
    ) {
    Presentation* childPresentation = dynamic_cast<Presentation*>(childElement->visual());
    emit childWasInsertedIntoGroupAfter(childIndex, groupIndex, childIndexInGroup, childPresentation);
}


void PresentationWithGroupedChildren::groupsRemoved(unsigned startingGroupIndex, unsigned numberGroups) {
    emit groupsWereRemoved(startingGroupIndex, numberGroups);
}


void PresentationWithGroupedChildren::groupsInserted(unsigned groupIndex, unsigned numberGroups) {
    emit groupsWereInserted(groupIndex, numberGroups);
}
