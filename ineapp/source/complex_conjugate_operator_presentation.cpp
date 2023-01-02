/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref ComplexConjugateOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_superscript_operator_presentation_base.h"
#include "complex_conjugate_operator_presentation.h"

ComplexConjugateOperatorPresentation::ComplexConjugateOperatorPresentation() {}


ComplexConjugateOperatorPresentation::~ComplexConjugateOperatorPresentation() {}


Ld::Visual* ComplexConjugateOperatorPresentation::creator(const QString& /* typeName */) {
    return new ComplexConjugateOperatorPresentation;
}


QString ComplexConjugateOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("*");
}
