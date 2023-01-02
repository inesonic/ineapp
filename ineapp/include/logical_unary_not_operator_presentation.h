/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LogicalUnaryNotOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LOGICAL_UNARY_NOT_OPERATOR_PRESENTATION_H
#define LOGICAL_UNARY_NOT_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_logical_unary_not_operator_visual.h>

#include "app_common.h"
#include "logical_not_operator_presentation_base.h"

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::LogicalUnaryNotOperatorElement class.
 */
class APP_PUBLIC_API LogicalUnaryNotOperatorPresentation:public         LogicalNotOperatorPresentationBase,
                                                         public virtual Ld::LogicalUnaryNotOperatorVisual {
    Q_OBJECT

    public:
        LogicalUnaryNotOperatorPresentation();

        ~LogicalUnaryNotOperatorPresentation() override;

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
         * Method you can overload to specify the default format for this logical not operation.
         *
         * \return Returns the default logical not style to use for this operation.
         */
        Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol defaultLogicalNotSymbol() const override;
};

#endif
