/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlaceholderPresentation class.
***********************************************************************************************************************/

#include <QGraphicsItem>
#include <QString>
#include <QSizeF>
#include <QFont>
#include <QFontMetricsF>
#include <QColor>
#include <QGraphicsItem>

#include <eqt_graphics_text_item.h>

#include <ld_element_structures.h>
#include <ld_placeholder_element.h>
#include <ld_placeholder_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "special_symbol_presentation_base.h"
#include "placeholder_presentation.h"

PlaceholderPresentation::PlaceholderPresentation() {}


PlaceholderPresentation::~PlaceholderPresentation() {}


Ld::Visual* PlaceholderPresentation::creator(const QString&) {
    return new PlaceholderPresentation();
}


QString PlaceholderPresentation::specialCharacters() const {
    return tr("?");
}


QColor PlaceholderPresentation::specialCharactersColor() const {
    return QColor(Qt::gray);
}


QColor PlaceholderPresentation::specialCharactersBackgroundColor() const {
    return QColor();
}
