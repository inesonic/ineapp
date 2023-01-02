/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NotEqualToOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "not_equal_to_operator_presentation.h"

NotEqualToOperatorPresentation::NotEqualToOperatorPresentation() {}


NotEqualToOperatorPresentation::~NotEqualToOperatorPresentation() {}


Ld::Visual* NotEqualToOperatorPresentation::creator(const QString&) {
    return new NotEqualToOperatorPresentation();
}


QString NotEqualToOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2260));
}
