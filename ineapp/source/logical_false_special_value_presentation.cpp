/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LogicalFalseSpecialValuePresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_logical_false_special_value_element.h>
#include <ld_logical_false_special_value_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "special_symbol_presentation_base.h"
#include "logical_false_special_value_presentation.h"

LogicalFalseSpecialValuePresentation::LogicalFalseSpecialValuePresentation() {}


LogicalFalseSpecialValuePresentation::~LogicalFalseSpecialValuePresentation() {}


Ld::Visual* LogicalFalseSpecialValuePresentation::creator(const QString&) {
    return new LogicalFalseSpecialValuePresentation();
}


QString LogicalFalseSpecialValuePresentation::specialCharacters() const {
    return tr("false ");
}


QFont LogicalFalseSpecialValuePresentation::specialCharactersFont() const {
    QFont font = SpecialSymbolPresentationBase::specialCharactersFont();
    font.setItalic(true);

    return font;
}
