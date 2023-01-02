/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PresentationWithNoChildren class.
***********************************************************************************************************************/

#include <QObject>
#include <QSizeF>

#include <ld_element.h>
#include <ld_visual_with_no_children.h>

#include "presentation.h"
#include "presentation_with_no_children.h"

PresentationWithNoChildren::PresentationWithNoChildren() {}


PresentationWithNoChildren::~PresentationWithNoChildren() {}


Ld::Element::ChildPlacement PresentationWithNoChildren::childPlacement() const {
    return Ld::Element::ChildPlacement::NONE;
}
