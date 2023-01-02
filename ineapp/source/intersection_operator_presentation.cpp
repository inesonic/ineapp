/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref IntersectionOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "intersection_operator_presentation.h"

IntersectionOperatorPresentation::IntersectionOperatorPresentation() {}


IntersectionOperatorPresentation::~IntersectionOperatorPresentation() {}


Ld::Visual* IntersectionOperatorPresentation::creator(const QString&) {
    return new IntersectionOperatorPresentation();
}


QString IntersectionOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2229));
}
