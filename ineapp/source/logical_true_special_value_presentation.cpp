/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LogicalTrueSpecialValuePresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_logical_true_special_value_element.h>
#include <ld_logical_true_special_value_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "special_symbol_presentation_base.h"
#include "logical_true_special_value_presentation.h"

LogicalTrueSpecialValuePresentation::LogicalTrueSpecialValuePresentation() {}


LogicalTrueSpecialValuePresentation::~LogicalTrueSpecialValuePresentation() {}


Ld::Visual* LogicalTrueSpecialValuePresentation::creator(const QString&) {
    return new LogicalTrueSpecialValuePresentation();
}


QString LogicalTrueSpecialValuePresentation::specialCharacters() const {
    return tr("true ");
}


QFont LogicalTrueSpecialValuePresentation::specialCharactersFont() const {
    QFont font = SpecialSymbolPresentationBase::specialCharactersFont();
    font.setItalic(true);

    return font;
}
