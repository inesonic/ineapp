/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref EpsilonSpecialValuePresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_epsilon_special_value_element.h>
#include <ld_epsilon_special_value_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "special_symbol_presentation_base.h"
#include "epsilon_special_value_presentation.h"

EpsilonSpecialValuePresentation::EpsilonSpecialValuePresentation() {}


EpsilonSpecialValuePresentation::~EpsilonSpecialValuePresentation() {}


Ld::Visual* EpsilonSpecialValuePresentation::creator(const QString&) {
    return new EpsilonSpecialValuePresentation();
}


QString EpsilonSpecialValuePresentation::specialCharacters() const {
    return tr("%1").arg(QString::fromUtf8("\xF0\x9D\x9B\xA6"));
}
