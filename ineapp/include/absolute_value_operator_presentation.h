/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref AbsoluteValueOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ABSOLUTE_VALUE_OPERATOR_PRESENTATION_H
#define ABSOLUTE_VALUE_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_absolute_value_operator_visual.h>

#include "app_common.h"
#include "grouping_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::AbsoluteValueOperatorElement class.
 */
class APP_PUBLIC_API AbsoluteValueOperatorPresentation:public         GroupingOperatorPresentationBase,
                                                    public virtual Ld::AbsoluteValueOperatorVisual {
    Q_OBJECT

    public:
        AbsoluteValueOperatorPresentation();

        ~AbsoluteValueOperatorPresentation() override;

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
         * Method that provides the left parenthesis style.
         *
         * \param[in] format The format instance tied to this operator.
         *
         * \return Returns the style to apply to the left parenthesis.
         */
        ParenthesisStyle leftParenthesisStyle(Ld::FormatPointer format) const override;

        /**
         * Method that provides the right parenthesis style.
         *
         * \param[in] format The format instance tied to this operator.
         *
         * \return Returns the style to apply to the right parenthesis.
         */
        ParenthesisStyle rightParenthesisStyle(Ld::FormatPointer format) const override;
};

#endif
