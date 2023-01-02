/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DisjointUnionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "disjoint_union_operator_presentation.h"

DisjointUnionOperatorPresentation::DisjointUnionOperatorPresentation() {}


DisjointUnionOperatorPresentation::~DisjointUnionOperatorPresentation() {}


Ld::Visual* DisjointUnionOperatorPresentation::creator(const QString&) {
    return new DisjointUnionOperatorPresentation();
}


QString DisjointUnionOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2294));
}
