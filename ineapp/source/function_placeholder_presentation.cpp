/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FunctionPlaceholderPresentation class.
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
#include <ld_function_placeholder_element.h>
#include <ld_function_placeholder_visual.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "placement_negotiator.h"
#include "special_symbol_presentation_base.h"
#include "function_placeholder_presentation.h"

FunctionPlaceholderPresentation::FunctionPlaceholderPresentation() {}


FunctionPlaceholderPresentation::~FunctionPlaceholderPresentation() {}


Ld::Visual* FunctionPlaceholderPresentation::creator(const QString&) {
    return new FunctionPlaceholderPresentation();
}


QString FunctionPlaceholderPresentation::specialCharacters() const {
    QString result;

    Ld::ElementPointer element = FunctionPlaceholderPresentation::element();
    if (!element.isNull() && element->typeName() == Ld::FunctionPlaceholderElement::elementName) {
        QSharedPointer<Ld::FunctionPlaceholderElement>
            functionPlaceholderElement = element.dynamicCast<Ld::FunctionPlaceholderElement>();

        result = functionPlaceholderElement->fieldString();
    }

    return result;
}


QColor FunctionPlaceholderPresentation::specialCharactersColor() const {
    return QColor(Qt::gray);
}


QColor FunctionPlaceholderPresentation::specialCharactersBackgroundColor() const {
    return QColor();
}
