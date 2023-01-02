/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BooleanTypePresentation class.
***********************************************************************************************************************/

#include <QString>
#include <QChar>

#include <ld_boolean_type_visual.h>
#include <ld_element_cursor.h>

#include "special_symbol_presentation_base.h"
#include "boolean_type_presentation.h"

BooleanTypePresentation::BooleanTypePresentation() {}


BooleanTypePresentation::~BooleanTypePresentation() {}


Ld::Visual* BooleanTypePresentation::creator(const QString&) {
    return new BooleanTypePresentation();
}


QString BooleanTypePresentation::specialCharacters() const {
    return QString::fromUtf8("\xF0\x9D\x94\xB9");
}
