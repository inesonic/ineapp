/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LogicalBiconditionalOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "logical_biconditional_operator_presentation.h"

LogicalBiconditionalOperatorPresentation::LogicalBiconditionalOperatorPresentation() {}


LogicalBiconditionalOperatorPresentation::~LogicalBiconditionalOperatorPresentation() {}


Ld::Visual* LogicalBiconditionalOperatorPresentation::creator(const QString&) {
    return new LogicalBiconditionalOperatorPresentation();
}


QString LogicalBiconditionalOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x21D4));
}
