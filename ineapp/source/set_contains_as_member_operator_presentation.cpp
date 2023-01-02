/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SetContainsAsMemberOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "set_contains_as_member_operator_presentation.h"

SetContainsAsMemberOperatorPresentation::SetContainsAsMemberOperatorPresentation() {}


SetContainsAsMemberOperatorPresentation::~SetContainsAsMemberOperatorPresentation() {}


Ld::Visual* SetContainsAsMemberOperatorPresentation::creator(const QString&) {
    return new SetContainsAsMemberOperatorPresentation();
}


QString SetContainsAsMemberOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x220B));
}
