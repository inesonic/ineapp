/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref GreaterThanOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "greater_than_operator_presentation.h"

GreaterThanOperatorPresentation::GreaterThanOperatorPresentation() {}


GreaterThanOperatorPresentation::~GreaterThanOperatorPresentation() {}


Ld::Visual* GreaterThanOperatorPresentation::creator(const QString&) {
    return new GreaterThanOperatorPresentation();
}


QString GreaterThanOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" > ");
}
