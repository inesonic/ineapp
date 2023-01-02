/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SetDifferenceOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "set_difference_operator_presentation.h"

SetDifferenceOperatorPresentation::SetDifferenceOperatorPresentation() {}


SetDifferenceOperatorPresentation::~SetDifferenceOperatorPresentation() {}


Ld::Visual* SetDifferenceOperatorPresentation::creator(const QString&) {
    return new SetDifferenceOperatorPresentation();
}


QString SetDifferenceOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x29F5));
}
