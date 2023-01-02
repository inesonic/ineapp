/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref UnaryMinusOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_operator_presentation_base.h"
#include "unary_minus_operator_presentation.h"

UnaryMinusOperatorPresentation::UnaryMinusOperatorPresentation() {}


UnaryMinusOperatorPresentation::~UnaryMinusOperatorPresentation() {}


Ld::Visual* UnaryMinusOperatorPresentation::creator(const QString& /* typeName */) {
    return new UnaryMinusOperatorPresentation;
}


QString UnaryMinusOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("-");
}
