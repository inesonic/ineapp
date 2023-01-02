/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NotProperSupersetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "not_proper_superset_operator_presentation.h"

NotProperSupersetOperatorPresentation::NotProperSupersetOperatorPresentation() {}


NotProperSupersetOperatorPresentation::~NotProperSupersetOperatorPresentation() {}


Ld::Visual* NotProperSupersetOperatorPresentation::creator(const QString&) {
    return new NotProperSupersetOperatorPresentation();
}


QString NotProperSupersetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2285));
}
