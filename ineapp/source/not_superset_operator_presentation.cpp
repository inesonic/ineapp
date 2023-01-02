/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NotSupersetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "not_superset_operator_presentation.h"

NotSupersetOperatorPresentation::NotSupersetOperatorPresentation() {}


NotSupersetOperatorPresentation::~NotSupersetOperatorPresentation() {}


Ld::Visual* NotSupersetOperatorPresentation::creator(const QString&) {
    return new NotSupersetOperatorPresentation();
}


QString NotSupersetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2289));
}
