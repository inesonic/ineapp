/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MultiplicationOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MULTIPLICATION_OPERATOR_PRESENTATION_BASE_H
#define MULTIPLICATION_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_multiplication_operator_format.h>
#include <ld_multiplication_operator_visual.h>

#include "app_common.h"
#include "binary_operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * Base class for presentations for multiplication operators.
 */
class APP_PUBLIC_API MultiplicationOperatorPresentationBase:public BinaryOperatorPresentationBase {
    Q_OBJECT

    public:
        MultiplicationOperatorPresentationBase();

        ~MultiplicationOperatorPresentationBase() override;

    protected:
        /**
         * Method you can overload to provide the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string
         */
        QString operatorString(Ld::FormatPointer operatorFormat) const override;

        /**
         * Method that returns the default symbol for this operation.
         *
         * \return Returns the default symbol for this operation.
         */
        virtual Ld::MultiplicationOperatorFormat::MultiplicationSymbol defaultSymbol() const = 0;
};

#endif
