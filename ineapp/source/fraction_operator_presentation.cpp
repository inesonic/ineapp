/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FractionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_division_operator_format.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "fraction_operator_presentation.h"

FractionOperatorPresentation::FractionOperatorPresentation() {}


FractionOperatorPresentation::~FractionOperatorPresentation() {}


Ld::Visual* FractionOperatorPresentation::creator(const QString&) {
    return new FractionOperatorPresentation();
}


Ld::DivisionOperatorFormat::DivisionSymbol FractionOperatorPresentation::defaultDivisionSymbol() const {
    return Ld::DivisionOperatorFormat::DivisionSymbol::FRACTION;
}
