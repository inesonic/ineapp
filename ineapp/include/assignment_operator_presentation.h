/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref AssignmentOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ASSIGNMENT_OPERATOR_PRESENTATION_H
#define ASSIGNMENT_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_assignment_operator_visual.h>

#include "app_common.h"
#include "binary_operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This class provides a visual for the Ld::PlaceholderElement class.
 */
class APP_PUBLIC_API AssignmentOperatorPresentation:public         BinaryOperatorPresentationBase,
                                                    public virtual Ld::AssignmentOperatorVisual {
    Q_OBJECT

    public:
        AssignmentOperatorPresentation();

        ~AssignmentOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref AssignmentOperatorPresentation class.
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
         * Method you can overload to provide the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string.
         */
        QString operatorString(Ld::FormatPointer operatorFormat) const override;

        /**
         * Method that is used to update the border pen.
         *
         * \param[in] diagnostic   The diagnostic used to determine the border.
         *
         * \return Returns the border pen to apply to the border.
         */
        QPen updateBorder(Ld::DiagnosticPointer diagnostic) override;
};

#endif
