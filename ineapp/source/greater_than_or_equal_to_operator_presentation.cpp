/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref GreaterThanOrEqualToOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "greater_than_or_equal_to_operator_presentation.h"

GreaterThanOrEqualToOperatorPresentation::GreaterThanOrEqualToOperatorPresentation() {}


GreaterThanOrEqualToOperatorPresentation::~GreaterThanOrEqualToOperatorPresentation() {}


Ld::Visual* GreaterThanOrEqualToOperatorPresentation::creator(const QString&) {
    return new GreaterThanOrEqualToOperatorPresentation();
}


QString GreaterThanOrEqualToOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2265));
}
