/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MultiplicationOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_multiplication_operator_format.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "multiplication_operator_presentation.h"

MultiplicationOperatorPresentation::MultiplicationOperatorPresentation() {}


MultiplicationOperatorPresentation::~MultiplicationOperatorPresentation() {}


Ld::Visual* MultiplicationOperatorPresentation::creator(const QString&) {
    return new MultiplicationOperatorPresentation();
}


Ld::MultiplicationOperatorFormat::MultiplicationSymbol MultiplicationOperatorPresentation::defaultSymbol() const {
    return Ld::MultiplicationOperatorFormat::MultiplicationSymbol::SPACE;
}
