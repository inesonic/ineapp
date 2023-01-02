/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TuplePresentation class.
***********************************************************************************************************************/

#include <QString>
#include <QObject>

#include <eqt_graphics_math_group.h>

#include "application.h"
#include "list_presentation_base.h"
#include "tuple_presentation.h"

TuplePresentation::TuplePresentation() {}


TuplePresentation::~TuplePresentation() {}


Ld::Visual* TuplePresentation::creator(const QString& /* typeName */) {
    return new TuplePresentation;
}


EQt::GraphicsMathGroup::ParenthesisStyle TuplePresentation::parenthesisStyle() const {
    return EQt::GraphicsMathGroup::ParenthesisStyle::PARENTHESIS;
}
