/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ElementOfSetOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>

#include "application.h"
#include "binary_operator_presentation_base.h"
#include "element_of_set_operator_presentation.h"

ElementOfSetOperatorPresentation::ElementOfSetOperatorPresentation() {}


ElementOfSetOperatorPresentation::~ElementOfSetOperatorPresentation() {}


Ld::Visual* ElementOfSetOperatorPresentation::creator(const QString&) {
    return new ElementOfSetOperatorPresentation();
}


QString ElementOfSetOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr(" %1 ").arg(QChar(0x2208));
}
