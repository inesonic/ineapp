/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ProperSupersetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "proper_superset_operator_presentation.h"

ProperSupersetOperatorPresentation::ProperSupersetOperatorPresentation() {}


ProperSupersetOperatorPresentation::~ProperSupersetOperatorPresentation() {}


Ld::Visual* ProperSupersetOperatorPresentation::creator(const QString&) {
    return new ProperSupersetOperatorPresentation();
}


QString ProperSupersetOperatorPresentation::operatorString(Ld::FormatPointer /* format */) const {
    return tr(" %1 ").arg(QChar(0x2283));
}
