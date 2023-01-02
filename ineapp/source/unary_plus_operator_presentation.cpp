/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref UnaryPlusOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_operator_presentation_base.h"
#include "unary_plus_operator_presentation.h"

UnaryPlusOperatorPresentation::UnaryPlusOperatorPresentation() {}


UnaryPlusOperatorPresentation::~UnaryPlusOperatorPresentation() {}


Ld::Visual* UnaryPlusOperatorPresentation::creator(const QString& /* typeName */) {
    return new UnaryPlusOperatorPresentation;
}


QString UnaryPlusOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("+");
}
