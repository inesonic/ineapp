/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref GroupingBracketsOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef GROUPING_BRACKETS_OPERATOR_PRESENTATION_H
#define GROUPING_BRACKETS_OPERATOR_PRESENTATION_H

#include <QObject>

#include <ld_format_structures.h>
#include <ld_paragraph_format.h>
#include <ld_grouping_brackets_operator_visual.h>

#include "app_common.h"
#include "simple_grouping_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::GroupingBracketsOperatorElement class.
 */
class APP_PUBLIC_API GroupingBracketsOperatorPresentation:public         SimpleGroupingOperatorPresentationBase,
                                                          public virtual Ld::GroupingBracketsOperatorVisual {
    Q_OBJECT

    public:
        GroupingBracketsOperatorPresentation();

        ~GroupingBracketsOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref GroupingBracketsOperatorPresentation class.
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
