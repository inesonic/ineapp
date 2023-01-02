/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref SimpleGroupingOperatorPresentationBase class.
***********************************************************************************************************************/

#include <QObject>

#include <ld_format_structures.h>
#include <ld_parenthesis_format.h>

#include "application.h"
#include "grouping_operator_presentation_base.h"
#include "simple_grouping_operator_presentation_base.h"

SimpleGroupingOperatorPresentationBase::SimpleGroupingOperatorPresentationBase() {}


SimpleGroupingOperatorPresentationBase::~SimpleGroupingOperatorPresentationBase() {}


GroupingOperatorPresentationBase::ParenthesisStyle SimpleGroupingOperatorPresentationBase::leftParenthesisStyle(
        Ld::FormatPointer format
    ) const {
    return parenthesisStyle(format);
}


GroupingOperatorPresentationBase::ParenthesisStyle SimpleGroupingOperatorPresentationBase::rightParenthesisStyle(
        Ld::FormatPointer format
    ) const {
    return parenthesisStyle(format);
}


GroupingOperatorPresentationBase::ParenthesisStyle SimpleGroupingOperatorPresentationBase::parenthesisStyle(
        Ld::FormatPointer format
    ) const {
    Ld::ParenthesisFormat::ParenthesisStyle result;

    if (!format.isNull() && format->capabilities().contains(Ld::ParenthesisFormat::formatName)) {
        QSharedPointer<Ld::ParenthesisFormat> parenthesisFormat = format.dynamicCast<Ld::ParenthesisFormat>();
        result = parenthesisFormat->parenthesisStyle();

        if (result == Ld::ParenthesisFormat::ParenthesisStyle::DEFAULT) {
            result = defaultParenthesisStyle();
        }
    } else {
        result = defaultParenthesisStyle();
    }

    return static_cast<ParenthesisStyle>(result); // Note enums are designed to match values.
}
