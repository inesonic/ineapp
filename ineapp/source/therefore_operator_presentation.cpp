/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref ThereforeOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_operator_presentation_base.h"
#include "therefore_operator_presentation.h"

ThereforeOperatorPresentation::ThereforeOperatorPresentation() {}


ThereforeOperatorPresentation::~ThereforeOperatorPresentation() {}


Ld::Visual* ThereforeOperatorPresentation::creator(const QString& /* typeName */) {
    return new ThereforeOperatorPresentation;
}


QString ThereforeOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("%1 ").arg(QChar(0x2234));
}
