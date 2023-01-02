/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NotSubsetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "not_subset_operator_presentation.h"

NotSubsetOperatorPresentation::NotSubsetOperatorPresentation() {}


NotSubsetOperatorPresentation::~NotSubsetOperatorPresentation() {}


Ld::Visual* NotSubsetOperatorPresentation::creator(const QString&) {
    return new NotSubsetOperatorPresentation();
}


QString NotSubsetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2288));
}
