/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HadamardProductOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_multiplication_operator_format.h>

#include "application.h"
#include "multiplication_operator_presentation_base.h"
#include "hadamard_product_operator_presentation.h"

HadamardProductOperatorPresentation::HadamardProductOperatorPresentation() {}


HadamardProductOperatorPresentation::~HadamardProductOperatorPresentation() {}


Ld::Visual* HadamardProductOperatorPresentation::creator(const QString&) {
    return new HadamardProductOperatorPresentation();
}


Ld::MultiplicationOperatorFormat::MultiplicationSymbol HadamardProductOperatorPresentation::defaultSymbol() const {
    return Ld::MultiplicationOperatorFormat::MultiplicationSymbol::CIRCLE;
}
