/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref AdditionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "addition_operator_presentation.h"

AdditionOperatorPresentation::AdditionOperatorPresentation() {}


AdditionOperatorPresentation::~AdditionOperatorPresentation() {}


Ld::Visual* AdditionOperatorPresentation::creator(const QString&) {
    return new AdditionOperatorPresentation();
}


QString AdditionOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr(" + ");
}
