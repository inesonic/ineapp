/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref InfinitySpecialValuePresentation class.
***********************************************************************************************************************/

#include <QString>

#include <ld_element_structures.h>
#include <ld_infinity_special_value_element.h>
#include <ld_infinity_special_value_visual.h>

#include "application.h"
#include "special_symbol_presentation_base.h"
#include "infinity_special_value_presentation.h"

InfinitySpecialValuePresentation::InfinitySpecialValuePresentation() {}


InfinitySpecialValuePresentation::~InfinitySpecialValuePresentation() {}


Ld::Visual* InfinitySpecialValuePresentation::creator(const QString&) {
    return new InfinitySpecialValuePresentation();
}


QString InfinitySpecialValuePresentation::specialCharacters() const {
    return tr("%1").arg(QChar(0x221E));
}
