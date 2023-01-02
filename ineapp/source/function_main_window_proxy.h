/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FunctionMainWindowProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FUNCTION_MAIN_WINDOW_PROXY_H
#define FUNCTION_MAIN_WINDOW_PROXY_H

#include <QObject>
#include <QPoint>

#include <eqt_dimension_line_edit.h>
#include <eqt_programmatic_main_window_proxy.h>

namespace EQt {
    class ProgrammaticMainWindow;
}

namespace Ld {
    class FunctionData;
}

/**
 * Main window proxy used to manage functions.
 */
class FunctionMainWindowProxy:public EQt::ProgrammaticMainWindowProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] window The main window class we are proxying for.
         */
        FunctionMainWindowProxy(EQt::ProgrammaticMainWindow* window);

        ~FunctionMainWindowProxy() override;

        /**
         * Method that is called to perform binding of static actions to slots used to perform those actions.  This
         * method is called once each time a new main window is created.
         */
        void bind() override;

        /**
         * Method that is called after the window is fully configured and before the window first becomes visible.
         *
         * This method will recalculate the new minimum height and width values for the debug dock.
         */
        void windowConfigured() override;

    private slots:
        /**
         * Slot that is triggered when the user requests help for a function.
         *
         * \param[in] functionData The information about the function the user wishes help for.
         */
        void helpRequested(const Ld::FunctionData& functionData);

        /**
         * Slot that is triggered when the screen size changes.
         */
        void screenResized();
};

#endif
