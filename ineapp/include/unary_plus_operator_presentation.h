/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref UnaryPlusOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef UNARY_PLUS_OPERATOR_PRESENTATION_H
#define UNARY_PLUS_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_unary_plus_operator_visual.h>

#include "app_common.h"
#include "unary_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * Presentation for Ld::UnaryPlusOperatorElement class.
 */
class APP_PUBLIC_API UnaryPlusOperatorPresentation:public         UnaryOperatorPresentationBase,
                                                   public virtual Ld::UnaryPlusOperatorVisual {
    Q_OBJECT

    public:
        UnaryPlusOperatorPresentation();

        ~UnaryPlusOperatorPresentation() override;

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
         * Method you can overload to provide the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string.  The default implementation returns an inverted question mark.
         */
         QString operatorString(Ld::FormatPointer operatorFormat) const override;
};

#endif
