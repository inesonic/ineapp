/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ProperSubsetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "proper_subset_operator_presentation.h"

ProperSubsetOperatorPresentation::ProperSubsetOperatorPresentation() {}


ProperSubsetOperatorPresentation::~ProperSubsetOperatorPresentation() {}


Ld::Visual* ProperSubsetOperatorPresentation::creator(const QString&) {
    return new ProperSubsetOperatorPresentation();
}


QString ProperSubsetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2282));
}
