/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NotSetContainsAsMemberOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "not_set_contains_as_member_operator_presentation.h"

NotSetContainsAsMemberOperatorPresentation::NotSetContainsAsMemberOperatorPresentation() {}


NotSetContainsAsMemberOperatorPresentation::~NotSetContainsAsMemberOperatorPresentation() {}


Ld::Visual* NotSetContainsAsMemberOperatorPresentation::creator(const QString&) {
    return new NotSetContainsAsMemberOperatorPresentation();
}


QString NotSetContainsAsMemberOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x220C));
}
