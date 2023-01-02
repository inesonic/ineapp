/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NullSetSpecialValuePresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_null_set_special_value_element.h>
#include <ld_null_set_special_value_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "special_symbol_presentation_base.h"
#include "null_set_special_value_presentation.h"

NullSetSpecialValuePresentation::NullSetSpecialValuePresentation() {}


NullSetSpecialValuePresentation::~NullSetSpecialValuePresentation() {}


Ld::Visual* NullSetSpecialValuePresentation::creator(const QString&) {
    return new NullSetSpecialValuePresentation();
}


QString NullSetSpecialValuePresentation::specialCharacters() const {
    return tr("%1").arg(QChar(0x2205));
}
