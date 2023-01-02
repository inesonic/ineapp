/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ComplexConjugateOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMPLEX_CONJUGATE_OPERATOR_PRESENTATION_H
#define COMPLEX_CONJUGATE_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_complex_conjugate_operator_visual.h>

#include "app_common.h"
#include "unary_superscript_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::ComplexConjugateOperatorElement class.
 */
class APP_PUBLIC_API ComplexConjugateOperatorPresentation:public         UnarySuperscriptOperatorPresentationBase,
                                                          public virtual Ld::ComplexConjugateOperatorVisual {
    Q_OBJECT

    public:
        ComplexConjugateOperatorPresentation();

        ~ComplexConjugateOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref UnaryPlusOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Method that provides the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string.
         */
         QString operatorString(Ld::FormatPointer operatorFormat) const override;
};

#endif
