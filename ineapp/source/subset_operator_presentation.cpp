/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SubsetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "subset_operator_presentation.h"

SubsetOperatorPresentation::SubsetOperatorPresentation() {}


SubsetOperatorPresentation::~SubsetOperatorPresentation() {}


Ld::Visual* SubsetOperatorPresentation::creator(const QString&) {
    return new SubsetOperatorPresentation();
}


QString SubsetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2286));
}
