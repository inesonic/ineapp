/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SimpleGroupingOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SIMPLE_GROUPING_OPERATOR_PRESENTATION_BASE_H
#define SIMPLE_GROUPING_OPERATOR_PRESENTATION_BASE_H

#include <QObject>

#include <ld_format_structures.h>
#include <ld_parenthesis_format.h>

#include "app_common.h"
#include "grouping_operator_presentation_base.h"

/**
 * You can use this class as a base class for simple grouping operator presentations that simply encapsulate a child
 * element.
 */
class APP_PUBLIC_API SimpleGroupingOperatorPresentationBase:public GroupingOperatorPresentationBase {
    Q_OBJECT

    public:
        SimpleGroupingOperatorPresentationBase();

        ~SimpleGroupingOperatorPresentationBase() override;

    protected:
        /**
         * Pure virtual method you can overload to indicate the default parenthesis style.
         *
         * \return Returns the default parenthesis style.
         */
        virtual Ld::ParenthesisFormat::ParenthesisStyle defaultParenthesisStyle() const = 0;

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

    private:
        /**
         * Method that determines the parenthesis style to be used.
         *
         * \param[in] format The format instance tied to this operator.
         *
         * \return Returns the parenthesis style to be used.
         */
        ParenthesisStyle parenthesisStyle(Ld::FormatPointer format) const;
};

#endif

