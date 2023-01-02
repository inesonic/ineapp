/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LogicalConditionalOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QPen>

#include <ld_format_structures.h>
#include <ld_element_structures.h>
#include <ld_logical_conditional_operator_element.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "logical_conditional_operator_presentation.h"

LogicalConditionalOperatorPresentation::LogicalConditionalOperatorPresentation() {}


LogicalConditionalOperatorPresentation::~LogicalConditionalOperatorPresentation() {}


Ld::Visual* LogicalConditionalOperatorPresentation::creator(const QString&) {
    return new LogicalConditionalOperatorPresentation();
}


void LogicalConditionalOperatorPresentation::processInstructionBreakpointUpdated(bool) {
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


QPen LogicalConditionalOperatorPresentation::updateBorder(Ld::DiagnosticPointer diagnostic) {
    QPen result = OperatorPresentationBase::updateBorder(diagnostic);

    if (result.style() == Qt::NoPen) {
        Ld::ElementPointer element = LogicalConditionalOperatorPresentation::element();

        if (!element.isNull() && element->instructionBreakpointSet()) {
            result = QPen(breakpointBorderColor);
        }
    }

    return result;
}


QString LogicalConditionalOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2192));
}
