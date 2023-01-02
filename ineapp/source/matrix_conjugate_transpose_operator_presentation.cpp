/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref MatrixConjugateTransposeOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_superscript_operator_presentation_base.h"
#include "matrix_conjugate_transpose_operator_presentation.h"

MatrixConjugateTransposeOperatorPresentation::MatrixConjugateTransposeOperatorPresentation() {}


MatrixConjugateTransposeOperatorPresentation::~MatrixConjugateTransposeOperatorPresentation() {}


Ld::Visual* MatrixConjugateTransposeOperatorPresentation::creator(const QString& /* typeName */) {
    return new MatrixConjugateTransposeOperatorPresentation;
}


QString MatrixConjugateTransposeOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("H");
}
