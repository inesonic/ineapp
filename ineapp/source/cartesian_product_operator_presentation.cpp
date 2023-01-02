/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CartesianProductOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "cartesian_product_operator_presentation.h"

CartesianProductOperatorPresentation::CartesianProductOperatorPresentation() {}


CartesianProductOperatorPresentation::~CartesianProductOperatorPresentation() {}


Ld::Visual* CartesianProductOperatorPresentation::creator(const QString&) {
    return new CartesianProductOperatorPresentation();
}


QString CartesianProductOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x00D7));
}
