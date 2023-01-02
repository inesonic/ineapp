/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ControlFlowOperatorPresentationBase class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>
#include <QPen>

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_element_with_fixed_children.h>
#include <ld_diagnostic_structures.h>

#include "application.h"
#include "placement_negotiator.h"
#include "operator_presentation_base.h"
#include "control_flow_operator_presentation_base.h"

ControlFlowOperatorPresentationBase::ControlFlowOperatorPresentationBase() {}


ControlFlowOperatorPresentationBase::~ControlFlowOperatorPresentationBase() {}


void ControlFlowOperatorPresentationBase::processInstructionBreakpointUpdated(bool) {
    if (graftedToRoot()) {
        Ld::ElementPointer parent = element()->parent();
        if (!parent.isNull()) {
            PlacementNegotiator* parentPlacementNegotiator = dynamic_cast<PlacementNegotiator*>(parent->visual());
            if (parentPlacementNegotiator != Q_NULLPTR) {
                parentPlacementNegotiator->requestRepositioning(this);
            }
        }
    }
}


QPen ControlFlowOperatorPresentationBase::updateBorder(Ld::DiagnosticPointer diagnostic) {
    QPen result = OperatorPresentationBase::updateBorder(diagnostic);

    if (result.style() == Qt::NoPen) {
        Ld::ElementPointer element = ControlFlowOperatorPresentationBase::element();

        if (!element.isNull() && element->instructionBreakpointSet()) {
            result = QPen(breakpointBorderColor);
        }
    }

    return result;
}
