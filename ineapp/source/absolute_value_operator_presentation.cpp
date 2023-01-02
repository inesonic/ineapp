/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref AbsoluteValueOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "grouping_operator_presentation_base.h"
#include "absolute_value_operator_presentation.h"

AbsoluteValueOperatorPresentation::AbsoluteValueOperatorPresentation() {}


AbsoluteValueOperatorPresentation::~AbsoluteValueOperatorPresentation() {}


Ld::Visual* AbsoluteValueOperatorPresentation::creator(const QString& /* typeName */) {
    return new AbsoluteValueOperatorPresentation;
}


GroupingOperatorPresentationBase::ParenthesisStyle AbsoluteValueOperatorPresentation::leftParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::ABSOLUTE_VALUE;
}


GroupingOperatorPresentationBase::ParenthesisStyle AbsoluteValueOperatorPresentation::rightParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::ABSOLUTE_VALUE;
}
