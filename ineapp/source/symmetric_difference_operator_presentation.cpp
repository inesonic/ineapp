/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SymmetricDifferenceOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "symmetric_difference_operator_presentation.h"

SymmetricDifferenceOperatorPresentation::SymmetricDifferenceOperatorPresentation() {}


SymmetricDifferenceOperatorPresentation::~SymmetricDifferenceOperatorPresentation() {}


Ld::Visual* SymmetricDifferenceOperatorPresentation::creator(const QString&) {
    return new SymmetricDifferenceOperatorPresentation();
}


QString SymmetricDifferenceOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x25B3));
}
