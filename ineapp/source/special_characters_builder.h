/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SpecialCharactersBuilder class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SPECIAL_CHARACTERS_BUILDER_H
#define SPECIAL_CHARACTERS_BUILDER_H

#include <QObject>
#include <QList>
#include <QColor>
#include <QFont>

#include <util_bit_set.h>

#include <ld_special_characters.h>

#include <eqt_builder.h>

class QGridLayout;

namespace EQt {
    class ProgrammaticMainWindow;
    class ProgrammaticDockWidget;
    class ProgrammaticMainWindowProxy;
    class ProgrammaticViewProxy;
    class ProgrammaticView;
    class ToolButton;
}

/**
 * Builder for the special characters docks and controls.
 */
class SpecialCharactersBuilder:public EQt::Builder, public Ld::SpecialCharacters {
    Q_OBJECT

    public:
        /**
         * The priority for this builder.
         */
        static constexpr unsigned builderPriority = 20000;

        /**
         * The builder name.
         */
        static const char builderName[];

        /**
         * The name of the greek characters dock.
         */
        static const char greekCharactersDockName[];

        /**
         * The name of the special characters dock.
         */
        static const char specialCharactersDockName[];

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        explicit SpecialCharactersBuilder(QObject* parent = Q_NULLPTR);

        ~SpecialCharactersBuilder() override;

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
         * Number of rows in the horizontal layout.  This value is platform specific.
         */
        static const unsigned numberHorizontalRows;

        /**
         * Number of columns in the vertical layout.  This value is platform specific.
         */
        static const unsigned numberVerticalColumns;

        /**
         * The button scaling factor.  The space required by the font will be multiplied against this value.
         * The padding will then be added to this value.
         */
        static constexpr float buttonScaleFactor = 1.0F;

        /**
         * Fixed padding to account for the button's borders and shadows.  This value is platform specific.
         */
        static const unsigned buttonPadding;

        /**
         * The font family to be used for the buttons.  This parameter is platform specific.
         */
        static const char buttonFontFamily[];

        /**
         * The font size to be used for the buttons.  This parameter is platform specific.
         */
        static const unsigned buttonFontSize;

        /**
         * Method that calculates maximum size required for buttons in the current button font.
         *
         * \return Returns the edge size required for the button using the currently selected font.
         */
        unsigned buttonEdgeSize() const;

        /**
         * Method that builds the greek characters dock.
         *
         * \param[in] window The application main window.
         */
        void buildGreekCharactersDock(EQt::ProgrammaticMainWindow* window);

        /**
         * Method that builds the special characters dock.
         *
         * \param[in] window The application main window.
         */
        void buildSpecialCharactersDock(EQt::ProgrammaticMainWindow* window);

        /**
         * Method that builds the marshallers for the main window.
         *
         * \param[in] window The main window instance.
         */
        void buildMarshallers(EQt::ProgrammaticMainWindow* window);

        /**
         * Method that adds special character buttons based on a list of unicode values.
         *
         * \param[in]     window             The window that ultimately contains the dock widget.
         *
         * \param[in]     dock               The dock widget containing the buttons.
         *
         * \param[in]     characterList      A list of unicode values to be added.
         *
         * \param[in,out] horizontalLayout   The horizontal layout for the buttons.
         *
         * \param[in,out] verticalLayout     The vertical layout for the buttons.
         */
        void addButtons(
            EQt::ProgrammaticMainWindow* window,
            EQt::ProgrammaticDockWidget* dock,
            const QList<UnicodeValue>&   characterList,
            QGridLayout*                 horizontalLayout,
            QGridLayout*                 verticalLayout
        );

        /**
         * The button dimension, per side.
         */
        unsigned buttonEdgeWidth;

        /**
         * The font to be used on special buttons.
         */
        QFont specialButtonFont;
};

#endif
