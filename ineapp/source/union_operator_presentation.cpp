/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref UnionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "union_operator_presentation.h"

UnionOperatorPresentation::UnionOperatorPresentation() {}


UnionOperatorPresentation::~UnionOperatorPresentation() {}


Ld::Visual* UnionOperatorPresentation::creator(const QString&) {
    return new UnionOperatorPresentation();
}


QString UnionOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x222A));
}
