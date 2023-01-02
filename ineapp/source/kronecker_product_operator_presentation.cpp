/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref KroneckerProductOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "kronecker_product_operator_presentation.h"

KroneckerProductOperatorPresentation::KroneckerProductOperatorPresentation() {}


KroneckerProductOperatorPresentation::~KroneckerProductOperatorPresentation() {}


Ld::Visual* KroneckerProductOperatorPresentation::creator(const QString&) {
    return new KroneckerProductOperatorPresentation();
}


QString KroneckerProductOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2297));
}
