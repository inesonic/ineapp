/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FunctionViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CONTROL_FLOW_VIEW_PROXY_H
#define CONTROL_FLOW_VIEW_PROXY_H

#include <QObject>
#include <QString>
#include <QStringList>

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

namespace Ld {
    class FunctionVariant;
    class FunctionData;
}

/**
 * View proxy used to manage functions controls.
 */
class FunctionViewProxy:public MathViewProxyBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        FunctionViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~FunctionViewProxy() override;

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
         * Slot that is triggered when the function operator button is clicked.
         */
        void functionOperatorButtonClicked();

        /**
         * Slot that is triggered when a new standard function needs to be inserted.  This version will insert the
         * first variant of the function.
         *
         * \param[in] functionData The information about the standard function to be inserted.
         */
        void insertFunction(const Ld::FunctionData& functionData);

        /**
         * Slot that is triggered when a new standard function needs to be inserted.
         *
         * \param[in] functionData          The information about the standard function to be inserted.
         *
         * \param[in] parameterDescriptions The desired function variant.
         */
        void insertFunction(const Ld::FunctionData& functionData, const QStringList& parameterDescriptions);
};

#endif
