/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref EulersNumberSpecialValuePresentation class.
***********************************************************************************************************************/

#include <QString>

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_eulers_number_special_value_element.h>
#include <ld_eulers_number_special_value_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "special_symbol_presentation_base.h"
#include "eulers_number_special_value_presentation.h"

EulersNumberSpecialValuePresentation::EulersNumberSpecialValuePresentation() {}


EulersNumberSpecialValuePresentation::~EulersNumberSpecialValuePresentation() {}


Ld::Visual* EulersNumberSpecialValuePresentation::creator(const QString&) {
    return new EulersNumberSpecialValuePresentation();
}


QString EulersNumberSpecialValuePresentation::specialCharacters() const {
    return tr("%1").arg(QString::fromUtf8("\xF0\x9D\x91\x92"));
}
