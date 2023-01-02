/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref IntegerTypePresentation class.
***********************************************************************************************************************/

#include <QString>
#include <QChar>

#include <ld_integer_type_visual.h>
#include <ld_element_cursor.h>

#include "special_symbol_presentation_base.h"
#include "integer_type_presentation.h"

IntegerTypePresentation::IntegerTypePresentation() {}


IntegerTypePresentation::~IntegerTypePresentation() {}


Ld::Visual* IntegerTypePresentation::creator(const QString&) {
    return new IntegerTypePresentation();
}


QString IntegerTypePresentation::specialCharacters() const {
    return QString(QChar(0x2124));
}
