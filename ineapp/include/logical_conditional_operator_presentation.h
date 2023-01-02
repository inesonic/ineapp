/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LogicalConditionalOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LOGICAL_CONDITIONAL_OPERATOR_PRESENTATION_H
#define LOGICAL_CONDITIONAL_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_logical_conditional_operator_visual.h>

#include "app_common.h"
#include "binary_operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::LogicalConditionalOperatorElement class.
 */
class APP_PUBLIC_API LogicalConditionalOperatorPresentation:public         BinaryOperatorPresentationBase,
                                                            public virtual Ld::LogicalConditionalOperatorVisual {
    Q_OBJECT

    public:
        LogicalConditionalOperatorPresentation();

        ~LogicalConditionalOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref LogicalConditionalOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected slots:
        /**
         * Slot that is triggered when and instruction breakpoint has been set or cleared on this element.
         *
         * \param[in] breakpointNowSet If true, a breakpoint on this element has been set.  If false, a breakpoint
         *                             on this element has been cleared.
         */
        void processInstructionBreakpointUpdated(bool breakpointNowSet) override;

    protected:
        /**
         * Method that is used to update the border pen.
         *
         * \param[in] diagnostic   The diagnostic used to determine the border.
         *
         * \return Returns the border pen to apply to the border.
         */
        QPen updateBorder(Ld::DiagnosticPointer diagnostic) override;

        /**
         * Method you can overload to provide the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string
         */
        QString operatorString(Ld::FormatPointer operatorFormat) const override;
};

#endif
