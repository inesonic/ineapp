/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref CeilingOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "grouping_operator_presentation_base.h"
#include "ceiling_operator_presentation.h"

CeilingOperatorPresentation::CeilingOperatorPresentation() {}


CeilingOperatorPresentation::~CeilingOperatorPresentation() {}


Ld::Visual* CeilingOperatorPresentation::creator(const QString& /* typeName */) {
    return new CeilingOperatorPresentation;
}


GroupingOperatorPresentationBase::ParenthesisStyle CeilingOperatorPresentation::leftParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::CEILING;
}


GroupingOperatorPresentationBase::ParenthesisStyle CeilingOperatorPresentation::rightParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::CEILING;
}
