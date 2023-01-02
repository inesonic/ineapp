/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BasicMathMainWindowProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BASIC_MATH_MAIN_WINDOW_PROXY_H
#define BASIC_MATH_MAIN_WINDOW_PROXY_H

#include <QObject>

#include <eqt_programmatic_main_window_proxy.h>

#include <ld_special_characters.h>

namespace EQt {
    class ProgrammaticMainWindow;
}

/**
 * Main window proxy used to support basic math functions.
 */
class BasicMathMainWindowProxy:public EQt::ProgrammaticMainWindowProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] window The main window class we are proxying for.
         */
        BasicMathMainWindowProxy(EQt::ProgrammaticMainWindow* window);

        ~BasicMathMainWindowProxy() override;

        /**
         * Method that is called to perform binding of static actions to slots used to perform those actions.  This
         * method is called once each time a new main window is created.
         */
        void bind() final;

        /**
         * Method that is called after the window is fully configured and before the window first becomes visible.
         *
         * This method will recalculate the new minimum height and width values for the debug dock.
         */
        void windowConfigured() override;

    private slots:
        /**
         * Slot that is triggered when the screen size changes.
         */
        void screenResized();
};

#endif
