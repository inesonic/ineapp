/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ComplexTypePresentation class.
***********************************************************************************************************************/

#include <QString>
#include <QChar>

#include <ld_complex_type_visual.h>
#include <ld_element_cursor.h>

#include "special_symbol_presentation_base.h"
#include "complex_type_presentation.h"

ComplexTypePresentation::ComplexTypePresentation() {}


ComplexTypePresentation::~ComplexTypePresentation() {}


Ld::Visual* ComplexTypePresentation::creator(const QString&) {
    return new ComplexTypePresentation();
}


QString ComplexTypePresentation::specialCharacters() const {
    return QString(QChar(0x2102));
}
