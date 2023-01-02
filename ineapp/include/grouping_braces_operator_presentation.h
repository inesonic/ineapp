/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref GroupingBracesOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef GROUPING_BRACES_OPERATOR_PRESENTATION_H
#define GROUPING_BRACES_OPERATOR_PRESENTATION_H

#include <QObject>

#include <ld_format_structures.h>
#include <ld_paragraph_format.h>
#include <ld_grouping_braces_operator_visual.h>

#include "app_common.h"
#include "simple_grouping_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::GroupingBracesOperatorElement class.
 */
class APP_PUBLIC_API GroupingBracesOperatorPresentation:public         SimpleGroupingOperatorPresentationBase,
                                                        public virtual Ld::GroupingBracesOperatorVisual {
    Q_OBJECT

    public:
        GroupingBracesOperatorPresentation();

        ~GroupingBracesOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref GroupingBracesOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Methot ath indicates the default parenthesis style.
         *
         * \return Returns the default parenthesis style.
         */
        Ld::ParenthesisFormat::ParenthesisStyle defaultParenthesisStyle() const override;
};

#endif
