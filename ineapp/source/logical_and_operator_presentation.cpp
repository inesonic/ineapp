/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LogicalAndOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "logical_and_operator_presentation.h"

LogicalAndOperatorPresentation::LogicalAndOperatorPresentation() {}


LogicalAndOperatorPresentation::~LogicalAndOperatorPresentation() {}


Ld::Visual* LogicalAndOperatorPresentation::creator(const QString&) {
    return new LogicalAndOperatorPresentation();
}


QString LogicalAndOperatorPresentation::operatorString(Ld::FormatPointer) const {
    return tr(" %1 ").arg(QChar(0x2227));
}
