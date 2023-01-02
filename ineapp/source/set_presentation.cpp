/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SetPresentation class.
***********************************************************************************************************************/

#include <QString>
#include <QObject>

#include <eqt_graphics_math_group.h>

#include "application.h"
#include "list_presentation_base.h"
#include "set_presentation.h"

SetPresentation::SetPresentation() {}


SetPresentation::~SetPresentation() {}


Ld::Visual* SetPresentation::creator(const QString& /* typeName */) {
    return new SetPresentation;
}


EQt::GraphicsMathGroup::ParenthesisStyle SetPresentation::parenthesisStyle() const {
    return EQt::GraphicsMathGroup::ParenthesisStyle::BRACES;
}
