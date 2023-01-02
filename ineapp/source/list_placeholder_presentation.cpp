/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ListPlaceholderPresentation class.
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
#include "list_placeholder_presentation.h"

ListPlaceholderPresentation::ListPlaceholderPresentation() {}


ListPlaceholderPresentation::~ListPlaceholderPresentation() {}


Ld::Visual* ListPlaceholderPresentation::creator(const QString&) {
    return new ListPlaceholderPresentation();
}


QString ListPlaceholderPresentation::specialCharacters() const {
    return QString();
}
