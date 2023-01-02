/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref FactorialOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_operator_presentation_base.h"
#include "factorial_operator_presentation.h"

FactorialOperatorPresentation::FactorialOperatorPresentation() {}


FactorialOperatorPresentation::~FactorialOperatorPresentation() {}


Ld::Visual* FactorialOperatorPresentation::creator(const QString& /* typeName */) {
    return new FactorialOperatorPresentation;
}


bool FactorialOperatorPresentation::shouldPrecede() const {
    return false;
}


QString FactorialOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("!");
}
