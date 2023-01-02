/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref AssignmentOperatorPresentation class.
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
#include <ld_format_structures.h>
#include <ld_element_cursor.h>
#include <ld_assignment_operator_element.h>
#include <ld_diagnostic_structures.h>

#include "application.h"
#include "placement_negotiator.h"
#include "binary_operator_presentation_base.h"
#include "assignment_operator_presentation.h"

AssignmentOperatorPresentation::AssignmentOperatorPresentation() {}


AssignmentOperatorPresentation::~AssignmentOperatorPresentation() {}


Ld::Visual* AssignmentOperatorPresentation::creator(const QString&) {
    return new AssignmentOperatorPresentation();
}


void AssignmentOperatorPresentation::processInstructionBreakpointUpdated(bool) {
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


QString AssignmentOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr(" = ");
}


QPen AssignmentOperatorPresentation::updateBorder(Ld::DiagnosticPointer diagnostic) {
    QPen result = BinaryOperatorPresentationBase::updateBorder(diagnostic);

    if (result.style() == Qt::NoPen) {
        QSharedPointer<Ld::AssignmentOperatorElement> element = AssignmentOperatorPresentation::element()
                                                                .dynamicCast<Ld::AssignmentOperatorElement>();

        if (!element.isNull() && element->instructionBreakpointSet()) {
            result = QPen(breakpointBorderColor);
        }
    }

    return result;
}
