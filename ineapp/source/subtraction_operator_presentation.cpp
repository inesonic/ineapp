/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SubtractionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "subtraction_operator_presentation.h"

SubtractionOperatorPresentation::SubtractionOperatorPresentation() {}


SubtractionOperatorPresentation::~SubtractionOperatorPresentation() {}


Ld::Visual* SubtractionOperatorPresentation::creator(const QString&) {
    return new SubtractionOperatorPresentation();
}


QString SubtractionOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr(" - ");
}
