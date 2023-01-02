/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref GroupingParenthesisOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_parenthesis_format.h>
#include <ld_visual.h>

#include "simple_grouping_operator_presentation_base.h"
#include "grouping_parenthesis_operator_presentation.h"

GroupingParenthesisOperatorPresentation::GroupingParenthesisOperatorPresentation() {}


GroupingParenthesisOperatorPresentation::~GroupingParenthesisOperatorPresentation() {}


Ld::Visual* GroupingParenthesisOperatorPresentation::creator(const QString& /* typeName */) {
    return new GroupingParenthesisOperatorPresentation;
}


Ld::ParenthesisFormat::ParenthesisStyle GroupingParenthesisOperatorPresentation::defaultParenthesisStyle() const {
    return Ld::ParenthesisFormat::ParenthesisStyle::PARENTHESIS;
}
