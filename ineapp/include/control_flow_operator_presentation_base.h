/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ControlFlowOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CONTROL_FLOW_OPERATOR_PRESENTATION_BASE_H
#define CONTROL_FLOW_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>

#include "app_common.h"
#include "operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This is a common base class for control flow operators.  The class add support for breakpoints.
 */
class APP_PUBLIC_API ControlFlowOperatorPresentationBase:public OperatorPresentationBase {
    Q_OBJECT

    public:
        ControlFlowOperatorPresentationBase();

        ~ControlFlowOperatorPresentationBase() override;

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
};

#endif
