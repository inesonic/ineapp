/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MatrixOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MATRIX_OPERATOR_PRESENTATION_BASE_H
#define MATRIX_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_matrix_combine_left_to_right_operator_visual.h>

#include "app_common.h"
#include "operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * Base class for operators that present matrices.  This method extends \ref OperatorPresentationBase to modify how
 * and when the matrix parenthesis are displayed.  Specifically, this method will force parenthesis to always be
 * displayed unless the parent object is an Ld::MatrixCombineLeftToRightOperatorElement or a
 * Ld::MatrixCombineTopToBottomOperatorElement.
 */
class APP_PUBLIC_API MatrixOperatorPresentationBase:public OperatorPresentationBase {
    Q_OBJECT

    public:
        MatrixOperatorPresentationBase();

        ~MatrixOperatorPresentationBase() override;
};

#endif
