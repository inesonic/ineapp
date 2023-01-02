/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FactorialOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FACTORIAL_OPERATOR_PRESENTATION_H
#define FACTORIAL_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_factorial_operator_visual.h>

#include "app_common.h"
#include "unary_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::FactorialOperatorElement class.
 */
class APP_PUBLIC_API FactorialOperatorPresentation:public         UnaryOperatorPresentationBase,
                                                   public virtual Ld::FactorialOperatorVisual {
    Q_OBJECT

    public:
        FactorialOperatorPresentation();

        ~FactorialOperatorPresentation() override;

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
         * Method that indicates whether the operator should precede or follow the child element.
         * element.
         *
         * \return Returns true if the operator should precede.  This version always returns false.
         */
        bool shouldPrecede() const override;

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
