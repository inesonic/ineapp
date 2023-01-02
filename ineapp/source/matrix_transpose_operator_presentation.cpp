/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref MatrixTransposeOperatorPresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_visual.h>

#include "unary_superscript_operator_presentation_base.h"
#include "matrix_transpose_operator_presentation.h"

MatrixTransposeOperatorPresentation::MatrixTransposeOperatorPresentation() {}


MatrixTransposeOperatorPresentation::~MatrixTransposeOperatorPresentation() {}


Ld::Visual* MatrixTransposeOperatorPresentation::creator(const QString& /* typeName */) {
    return new MatrixTransposeOperatorPresentation;
}


QString MatrixTransposeOperatorPresentation::operatorString(Ld::FormatPointer /* operatorFormat */) const {
    return tr("T");
}
