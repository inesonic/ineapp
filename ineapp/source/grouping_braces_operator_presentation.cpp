/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref GroupingBracesOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_parenthesis_format.h>
#include <ld_visual.h>

#include "simple_grouping_operator_presentation_base.h"
#include "grouping_braces_operator_presentation.h"

GroupingBracesOperatorPresentation::GroupingBracesOperatorPresentation() {}


GroupingBracesOperatorPresentation::~GroupingBracesOperatorPresentation() {}


Ld::Visual* GroupingBracesOperatorPresentation::creator(const QString& /* typeName */) {
    return new GroupingBracesOperatorPresentation;
}


Ld::ParenthesisFormat::ParenthesisStyle GroupingBracesOperatorPresentation::defaultParenthesisStyle() const {
    return Ld::ParenthesisFormat::ParenthesisStyle::BRACES;
}
