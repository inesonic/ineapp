/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref LogicalUnaryNotOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_logical_unary_not_operator_format.h>
#include <ld_visual.h>

#include "logical_not_operator_presentation_base.h"
#include "logical_unary_not_operator_presentation.h"

LogicalUnaryNotOperatorPresentation::LogicalUnaryNotOperatorPresentation() {}


LogicalUnaryNotOperatorPresentation::~LogicalUnaryNotOperatorPresentation() {}


Ld::Visual* LogicalUnaryNotOperatorPresentation::creator(const QString& /* typeName */) {
    return new LogicalUnaryNotOperatorPresentation;
}


Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol
    LogicalUnaryNotOperatorPresentation::defaultLogicalNotSymbol() const {
    return Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol::NEGATION;
}
