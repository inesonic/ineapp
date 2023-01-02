/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ControlFlowViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CONTROL_FLOW_VIEW_PROXY_H
#define CONTROL_FLOW_VIEW_PROXY_H

#include <QObject>

#include <ld_element_structures.h>

#include "page_list.h"
#include "update_format_command.h"
#include "view_widget.h"
#include "math_view_proxy_base.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

class InsertElementCommand;

/**
 * View proxy used to manage functions related to control flow.
 */
class ControlFlowViewProxy:public MathViewProxyBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        ControlFlowViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~ControlFlowViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

    private slots:
        /**
         * Slot that is triggered when the for all in operator button is clicked.
         */
        void forAllInOperatorButtonClicked();

        /**
         * Slot that is triggered when the while operator button is clicked.
         */
        void whileOperatorButtonClicked();

        /**
         * Slot that is triggered when the compound statement operator button is clicked.
         */
        void compoundStatementOperatorButtonClicked();

        /**
         * Slot that is triggered when the if operator button is clicked.
         */
        void ifOperatorButtonClicked();

        /**
         * Slot that is triggered when the else-if operator button is clicked.
         */
        void elseIfOperatorButtonClicked();

        /**
         * Slot that is triggered when the else operator button is clicked.
         */
        void elseOperatorButtonClicked();

        /**
         * Slot that is is triggered when the therefore operator button is clicked.
         */
        void thereforeOperatorButtonClicked();

        /**
         * Slot that is triggered when the brace conditional operator button is clicked.
         */
        void braceConditionalOperatorButtonClicked();
};

#endif
