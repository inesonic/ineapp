/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref NaryDisjointUnionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_large_operator_presentation_base.h"
#include "nary_disjoint_union_operator_presentation.h"

NaryDisjointUnionOperatorPresentation::NaryDisjointUnionOperatorPresentation() {}


NaryDisjointUnionOperatorPresentation::~NaryDisjointUnionOperatorPresentation() {}


Ld::Visual* NaryDisjointUnionOperatorPresentation::creator(const QString& /* typeName */) {
    return new NaryDisjointUnionOperatorPresentation;
}


QString NaryDisjointUnionOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("%1").arg(QChar(0x2A06));
}
