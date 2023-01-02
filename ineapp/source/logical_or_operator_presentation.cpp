/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LogicalOrOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "logical_or_operator_presentation.h"

LogicalOrOperatorPresentation::LogicalOrOperatorPresentation() {}


LogicalOrOperatorPresentation::~LogicalOrOperatorPresentation() {}


Ld::Visual* LogicalOrOperatorPresentation::creator(const QString&) {
    return new LogicalOrOperatorPresentation();
}


QString LogicalOrOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2228));
}
