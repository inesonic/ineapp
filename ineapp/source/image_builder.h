/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImageBuilder class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMAGE_BUILDER_H
#define IMAGE_BUILDER_H

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
 * Builer for the application's image functions.  These functions include:
 *
 *     * Support for inserting images from a file.
 *     * Support for modifying the image format.
 */
class ImageBuilder:public EQt::Builder {
    Q_OBJECT

    public:
        /**
         * The priority for this builder.
         */
        static constexpr unsigned builderPriority = 5000;

        /**
         * The builder name.
         */
        static const char builderName[];

        /**
         * Name of the page format dock widget.
         */
        static const char imageFormatDockName[];

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        explicit ImageBuilder(QObject* parent = Q_NULLPTR);

        ~ImageBuilder() override;

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
         * Method that is called to build additional actions used by a main window's GUI but not tied to a specific
         * menu action.
         *
         * \param[in] window The main window to build the GUI on.
         */
        void buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) final;

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
         * Method this is called to perform any final configuration for the window.  This method is called once
         * each time a window is created, after all other build* and create* methods are called.
         *
         * The method exists primarily to implement or adjust the application's central widget.
         *
         * \param[in] window The main window to perform any final configuration on.
         */
        void buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) final;

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

    private:
        /**
         * Value representing the minimum scaling factor in the image dock.
         */
        static constexpr unsigned minimumScaleFactor = 10;

        /**
         * Value representing the maximum scaling factor in the image dock.
         */
        static constexpr unsigned maximumScaleFactor = 1000;

        /**
         * Value that represents the height and width to use for the image scaling mode buttons.
         */
        static constexpr unsigned scalingModeButtonSize = 24;

        /**
         * Creates menus and actions used to change the horizontal image scaling mode.
         *
         * \param[in] window Pointer to the main window object.
         */
        void buildScalingModeMenusAndActions(EQt::ProgrammaticMainWindow* window);

        /**
         * Builds the image format dock widget's horizontal orientation.
         *
         * \param[in] dock             The dock widget being populated.
         *
         * \param[in] horizontalLayout The QHBoxLayout to use for the horizontal orientation.
         *
         * \param[in] window           Pointer to the main window object.
         */
        void buildDockHorizontalOrientation(
            EQt::ProgrammaticDockWidget* dock,
            QHBoxLayout*                 horizontalLayout,
            EQt::ProgrammaticMainWindow* window
        );

        /**
         * Builds the image format dock widget's vertical orientation.
         *
         * \param[in] dock           The dock widget being populated.
         *
         * \param[in] verticalLayout The QHBoxLayout to use for the vertical orientation.
         *
         * \param[in] window         Pointer to the main window object.
         */
        void buildDockVerticalOrientation(
            EQt::ProgrammaticDockWidget* dock,
            QVBoxLayout*                 verticalLayout,
            EQt::ProgrammaticMainWindow* window
        );

        /**
         * Updates the insert dock widget to include a button to add images.
         *
         * \param[in] window Pointer to the main window object.
         */
        void updateInsertDock(EQt::ProgrammaticMainWindow* window);

        /**
         * Method you can use to obtain a sorted list of supported image formats.
         *
         * \return Returns a sorted list of image formats.
         */
        static QStringList supportedImageFormats();

        /**
         * Method that returns a nicely formatted string of supported image formats.
         *
         * \return Returns a string listing the supported image formats in comma separated format.
         */
        static QString supportedImageFormatString();
};

#endif