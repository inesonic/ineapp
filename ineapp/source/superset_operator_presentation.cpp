/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SupersetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "superset_operator_presentation.h"

SupersetOperatorPresentation::SupersetOperatorPresentation() {}


SupersetOperatorPresentation::~SupersetOperatorPresentation() {}


Ld::Visual* SupersetOperatorPresentation::creator(const QString&) {
    return new SupersetOperatorPresentation();
}


QString SupersetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2287));
}
