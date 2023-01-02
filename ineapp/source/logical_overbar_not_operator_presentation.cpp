/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref LogicalOverbarNotOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_logical_unary_not_operator_format.h>
#include <ld_visual.h>

#include "logical_not_operator_presentation_base.h"
#include "logical_overbar_not_operator_presentation.h"

LogicalOverbarNotOperatorPresentation::LogicalOverbarNotOperatorPresentation() {}


LogicalOverbarNotOperatorPresentation::~LogicalOverbarNotOperatorPresentation() {}


Ld::Visual* LogicalOverbarNotOperatorPresentation::creator(const QString& /* typeName */) {
    return new LogicalOverbarNotOperatorPresentation;
}


Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol
    LogicalOverbarNotOperatorPresentation::defaultLogicalNotSymbol() const {
    return Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol::OVERBAR;
}
