/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref FloorOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "grouping_operator_presentation_base.h"
#include "floor_operator_presentation.h"

FloorOperatorPresentation::FloorOperatorPresentation() {}


FloorOperatorPresentation::~FloorOperatorPresentation() {}


Ld::Visual* FloorOperatorPresentation::creator(const QString& /* typeName */) {
    return new FloorOperatorPresentation;
}


GroupingOperatorPresentationBase::ParenthesisStyle FloorOperatorPresentation::leftParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::FLOOR;
}


GroupingOperatorPresentationBase::ParenthesisStyle FloorOperatorPresentation::rightParenthesisStyle(
        Ld::FormatPointer /* format */
    ) const {
    return ParenthesisStyle::FLOOR;
}
