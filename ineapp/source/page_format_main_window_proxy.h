/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PageFormatMainWindowProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PAGE_FORMAT_MAIN_WINDOW_PROXY_H
#define PAGE_FORMAT_MAIN_WINDOW_PROXY_H

#include <QObject>
#include <QPoint>

#include <eqt_dimension_line_edit.h>
#include <eqt_programmatic_main_window_proxy.h>

namespace EQt {
    class ProgrammaticMainWindow;
}

/**
 * Main window proxy used to manage the block format functions.  These functions include:
 *
 *     * Basic block format menu items
 *     * Basic block formatting dock widget functions.
 */
class PageFormatMainWindowProxy:public EQt::ProgrammaticMainWindowProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] window The main window class we are proxying for.
         */
        PageFormatMainWindowProxy(EQt::ProgrammaticMainWindow* window);

        ~PageFormatMainWindowProxy() override;

        /**
         * Loads settings to be used by the main window.  The group for the main window will already be set prior
         * to this method being called.
         */
        void loadSettings() final;

        /**
         * Saves settings used by the main window.  The group for the main window will already be set prior to this
         * method being called.
         */
        void saveSettings() final;

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

    private:
        /**
         * Method that is called by \ref PageFormatMainWindowProxy::loadSettings to load settings for a single
         * dimension line edit field.
         *
         * \param[in] widgetName   The name of the widget in the dock.
         *
         * \param[in] defaultValue The default value to use if the setting does not exist.
         */
        void loadSetting(const QString& widgetName, EQt::DimensionLineEdit::Unit defaultValue);

        /**
         * Method that is called by \ref PageFormatMainWindowProxy::saveSettings to load settings for a single
         * dimension line edit field.
         *
         * \param[in] widgetName   The name of the widget in the dock.
         */
        void saveSetting(const QString& widgetName);
};

#endif
