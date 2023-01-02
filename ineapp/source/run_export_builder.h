/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RunExportBuilder class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef RUN_EXPORT_BUILDER_H
#define RUN_EXPORT_BUILDER_H

#include <QObject>
#include <QList>
#include <QColor>

#include <util_bit_set.h>

#include <eqt_builder.h>

class QHBoxLayout;
class QVBoxLayout;

namespace EQt {
    class ProgrammaticApplication;
    class ProgrammaticMainWindow;
    class ProgrammaticDockWidget;
    class ProgrammaticMainWindowProxy;
    class ProgrammaticViewProxy;
    class ProgrammaticView;
    class ToolButton;
}

/**
 * Builer for the application's home user interface functions.  These functions include:
 *
 *     * Basic file menu items
 *     * Basic edit functions
 *     * Basic editor display/zoom functions.
 */
class RunExportBuilder:public EQt::Builder {
    Q_OBJECT

    public:
        /**
         * The priority for this builder.
         */
        static constexpr unsigned builderPriority = 200;

        /**
         * The builder name.
         */
        static const char builderName[];

        /**
         * Debug dock name.
         */
        static const char debugDockName[];

        /**
         * IR dock name.
         */
        static const char irDockName[];

        /**
         * Console dock name.
         */
        static const char consoleDockName[];

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        explicit RunExportBuilder(QObject* parent = Q_NULLPTR);

        ~RunExportBuilder() override;

        /**
         * Method that returns a priority number used to order this builder with respect to other builders.
         * Builders with lower numbers are executed first.
         *
         * \return The builder priority number.
         */
        unsigned priority() const final;

        /**
         * Method that returns a short identifier for builder.
         *
         * \return Returns a string holding the builder name.
         */
        const char* builderIdentifier() const final;

        /**
         * Method that returns the name of the plug-in associated with this builder.
         *
         * \return Returns a string holding the plug-in name.
         */
        const char* plugInName() const final;

        /**
         * Method that is called to build a main window's GUI.  Note that this method will be called each time a
         * new main window needs to be created.
         *
         * This method should build the menus and toolbars, including support for custom tool-tips, icons, etc.
         *
         * \param[in] window The main window to build the GUI on.
         */
        void buildMainWindowGui(EQt::ProgrammaticMainWindow* window) final;

        /**
         * Method that is called to create docks used by a main window.  Note that this method is called once each
         * time a new main window needs to be created.
         *
         * This method should create actions used by the docks and then create the docking windows.  The docks should
         * be added to a main window.
         *
         * \param[in] window The main window to add the docks to.
         */
        void buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) final;

        /**
         * Method that can be used to build proxy objects for a main window.
         *
         * \param[in] window The window to associate with the proxy.
         *
         * \return Returns a pointer to the proxy object to be used for the main window.
         */
        EQt::ProgrammaticMainWindowProxy* createMainWindowProxy(EQt::ProgrammaticMainWindow* window) final;

        /**
         * Method that can be used to build proxy objects for a view.
         *
         * \param[in] view   The view to be built.
         *
         * \param[in] window The window that contains the view.
         *
         * \return Returns a pointer to the proxy object to be used for the view.  A null pointer can be returned
         *         if no proxy object is needed.
         */
        EQt::ProgrammaticViewProxy* createViewProxy(
            EQt::ProgrammaticView*       view,
            EQt::ProgrammaticMainWindow* window
        ) final;

        /**
         * Method this is called to perform any final configuration for the window.  This method is called once
         * each time a window is created, after all other build* and create* methods are called.
         *
         * The method exists primarily to implement or adjust the application's central widget.
         *
         * \param[in] window The main window to perform any final configuration on.
         */
        void buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) final;

    private:
        /**
         * Progress bar font size adjustment.
         */
        static constexpr int progressBarFontSizeAdjustment = -2;

        /**
         * Builds the debug dock widget's horizontal orientation.
         *
         * \param[in] dock             The dock widget being populated.
         *
         * \param[in] horizontalLayout The QHBoxLayout to use for the horizontal orientation.
         *
         * \param[in] window           Pointer to the main window object.
         */
        void buildDebugDockHorizontalOrientation(
            EQt::ProgrammaticDockWidget* dock,
            QHBoxLayout*                 horizontalLayout,
            EQt::ProgrammaticMainWindow* window
        );

        /**
         * Builds the debug dock widget's vertical orientation.
         *
         * \param[in] dock           The dock widget being populated.
         *
         * \param[in] verticalLayout The QHBoxLayout to use for the vertical orientation.
         *
         * \param[in] window         Pointer to the main window object.
         */
        void buildDebugDockVerticalOrientation(
            EQt::ProgrammaticDockWidget* dock,
            QVBoxLayout*                 verticalLayout,
            EQt::ProgrammaticMainWindow* window
        );

        /**
         * Builds the IR dock.
         *
         * \param[in] dock   The dock widget to be populated.
         *
         * \param[in] window Pointer to the main window object.
         */
        void buildIrDock(EQt::ProgrammaticDockWidget* dock, EQt::ProgrammaticMainWindow* window);

        /**
         * Builds the console dock.
         *
         * \param[in] dock   The dock widget to be populated.
         *
         * \param[in] window Pointer to the main window object.
         */
        void buildConsoleDock(EQt::ProgrammaticDockWidget* dock, EQt::ProgrammaticMainWindow* window);
};

#endif
