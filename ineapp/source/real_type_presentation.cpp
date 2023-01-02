/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RealTypePresentation class.
***********************************************************************************************************************/

#include <QString>
#include <QChar>

#include <eqt_graphics_text_item.h>

#include <ld_real_type_visual.h>
#include <ld_element_cursor.h>

#include "special_symbol_presentation_base.h"
#include "real_type_presentation.h"

RealTypePresentation::RealTypePresentation() {}


RealTypePresentation::~RealTypePresentation() {}


Ld::Visual* RealTypePresentation::creator(const QString&) {
    return new RealTypePresentation();
}


QString RealTypePresentation::specialCharacters() const {
    return QString(QChar(0x211D));
}
