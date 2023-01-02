/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref NearestIntegerOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "grouping_operator_presentation_base.h"
#include "nearest_integer_operator_presentation.h"

NearestIntegerOperatorPresentation::NearestIntegerOperatorPresentation() {}


NearestIntegerOperatorPresentation::~NearestIntegerOperatorPresentation() {}


Ld::Visual* NearestIntegerOperatorPresentation::creator(const QString& /* typeName */) {
    return new NearestIntegerOperatorPresentation;
}


GroupingOperatorPresentationBase::ParenthesisStyle NearestIntegerOperatorPresentation::leftParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::FLOOR;
}


GroupingOperatorPresentationBase::ParenthesisStyle NearestIntegerOperatorPresentation::rightParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::CEILING;
}
