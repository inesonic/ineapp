/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LeafPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QSizeF>

#include "presentation.h"
#include "placement_negotiator.h"
#include "presentation_with_no_children.h"
#include "leaf_presentation.h"

LeafPresentation::LeafPresentation() {}


LeafPresentation::~LeafPresentation() {}


void LeafPresentation::requestRepositioning(Presentation*) {
    Q_ASSERT(false);
}


QSizeF LeafPresentation::requestArea(unsigned long, LeafPresentation::SpaceQualifier*) {
    Q_ASSERT(false);
    return QSizeF();
}


void LeafPresentation::allocateArea(unsigned long, unsigned long, const QSizeF&, float, bool) {
    Q_ASSERT(false);
}


void LeafPresentation::areaInsufficient(unsigned long, const QSizeF&) {
    Q_ASSERT(false);
}
