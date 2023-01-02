/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LessThanOrEqualToOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "less_than_or_equal_to_operator_presentation.h"

LessThanOrEqualToOperatorPresentation::LessThanOrEqualToOperatorPresentation() {}


LessThanOrEqualToOperatorPresentation::~LessThanOrEqualToOperatorPresentation() {}


Ld::Visual* LessThanOrEqualToOperatorPresentation::creator(const QString&) {
    return new LessThanOrEqualToOperatorPresentation();
}


QString LessThanOrEqualToOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2264));
}
