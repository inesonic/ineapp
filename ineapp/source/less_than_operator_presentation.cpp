/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LessThanOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "less_than_operator_presentation.h"

LessThanOperatorPresentation::LessThanOperatorPresentation() {}


LessThanOperatorPresentation::~LessThanOperatorPresentation() {}


Ld::Visual* LessThanOperatorPresentation::creator(const QString&) {
    return new LessThanOperatorPresentation();
}


QString LessThanOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" < ");
}
