/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref GroupingBracketsOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_parenthesis_format.h>
#include <ld_visual.h>

#include "simple_grouping_operator_presentation_base.h"
#include "grouping_brackets_operator_presentation.h"

GroupingBracketsOperatorPresentation::GroupingBracketsOperatorPresentation() {}


GroupingBracketsOperatorPresentation::~GroupingBracketsOperatorPresentation() {}


Ld::Visual* GroupingBracketsOperatorPresentation::creator(const QString& /* typeName */) {
    return new GroupingBracketsOperatorPresentation;
}


Ld::ParenthesisFormat::ParenthesisStyle GroupingBracketsOperatorPresentation::defaultParenthesisStyle() const {
    return Ld::ParenthesisFormat::ParenthesisStyle::BRACKETS;
}
