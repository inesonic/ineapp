/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref NaryUnionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_large_operator_presentation_base.h"
#include "nary_union_operator_presentation.h"

NaryUnionOperatorPresentation::NaryUnionOperatorPresentation() {}


NaryUnionOperatorPresentation::~NaryUnionOperatorPresentation() {}


Ld::Visual* NaryUnionOperatorPresentation::creator(const QString& /* typeName */) {
    return new NaryUnionOperatorPresentation;
}


QString NaryUnionOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("%1").arg(QChar(0x22C3));
}
