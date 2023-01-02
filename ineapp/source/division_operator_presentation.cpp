/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DivisionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_division_operator_format.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "division_operator_presentation.h"

DivisionOperatorPresentation::DivisionOperatorPresentation() {}


DivisionOperatorPresentation::~DivisionOperatorPresentation() {}


Ld::Visual* DivisionOperatorPresentation::creator(const QString&) {
    return new DivisionOperatorPresentation();
}


Ld::DivisionOperatorFormat::DivisionSymbol DivisionOperatorPresentation::defaultDivisionSymbol() const {
    return Ld::DivisionOperatorFormat::DivisionSymbol::OBELUS;
}
