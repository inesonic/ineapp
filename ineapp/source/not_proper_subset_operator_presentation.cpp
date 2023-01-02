/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NotProperSubsetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "not_proper_subset_operator_presentation.h"

NotProperSubsetOperatorPresentation::NotProperSubsetOperatorPresentation() {}


NotProperSubsetOperatorPresentation::~NotProperSubsetOperatorPresentation() {}


Ld::Visual* NotProperSubsetOperatorPresentation::creator(const QString&) {
    return new NotProperSubsetOperatorPresentation();
}


QString NotProperSubsetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2284));
}
