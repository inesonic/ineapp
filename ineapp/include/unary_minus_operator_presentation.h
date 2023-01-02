/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref UnaryMinusOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef UNARY_MINUS_OPERATOR_PRESENTATION_H
#define UNARY_MINUS_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_unary_minus_operator_visual.h>

#include "app_common.h"
#include "unary_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::UnaryMinusOperatorElement class.
 */
class APP_PUBLIC_API UnaryMinusOperatorPresentation:public         UnaryOperatorPresentationBase,
                                                    public virtual Ld::UnaryMinusOperatorVisual {
    Q_OBJECT

    public:
        UnaryMinusOperatorPresentation();

        ~UnaryMinusOperatorPresentation() override;

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
