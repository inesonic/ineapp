/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NotElementOfSetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "not_element_of_set_operator_presentation.h"

NotElementOfSetOperatorPresentation::NotElementOfSetOperatorPresentation() {}


NotElementOfSetOperatorPresentation::~NotElementOfSetOperatorPresentation() {}


Ld::Visual* NotElementOfSetOperatorPresentation::creator(const QString&) {
    return new NotElementOfSetOperatorPresentation();
}


QString NotElementOfSetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2209));
}
