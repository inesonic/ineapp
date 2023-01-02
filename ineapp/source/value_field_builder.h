/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This header defines the \ref ValueFieldBuilder class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VALUE_FIELD_BUILDER_H
#define VALUE_FIELD_BUILDER_H

#include <QObject>
#include <QList>
#include <QColor>
#include <QDoubleValidator>

#include <util_bit_set.h>

#include <eqt_builder.h>

class QHBoxLayout;
class QVBoxLayout;
class QAction;

namespace EQt {
    class ProgrammaticApplication;
    class ProgrammaticMainWindow;
    class ProgrammaticDockWidget;
    class ProgrammaticMainWindowProxy;
    class ProgrammaticViewProxy;
    class ProgrammaticView;
    class ToolButton;
    class SpinBox;
    class ComboBox;
    class LineEdit;
}

/**
 * Builer for the application's value field and related output display.
 */
class ValueFieldBuilder:public EQt::Builder {
    Q_OBJECT

    public:
        /**
         * The priority for this builder.
         */
        static constexpr unsigned builderPriority = 7000;

        /**
         * The builder name.
         */
        static const char builderName[];

        /**
         * Name of the page format dock widget.
         */
        static const char valueFieldFormatDockName[];

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        explicit ValueFieldBuilder(QObject* parent = Q_NULLPTR);

        ~ValueFieldBuilder() override;

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
         * The double precision number validator.
         */
        static QDoubleValidator doubleValidator;

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
         * Method that configures and returns a configured integer number digits spin box.
         *
         * \param[in] action The action to be tied to the spin box.
         *
         * \return Returns a pointer to a newly created spin box.
         */
        static EQt::SpinBox* newIntegerNumberDigitsSpinBox(QAction* action);

        /**
         * Method that configures and returns a configured number base spin box.
         *
         * \param[in] action The action to be tied to the spin box.
         *
         * \return Returns a pointer to a newly created spin box.
         */
        static EQt::SpinBox* newIntegerNumberBaseSpinBox(QAction* action);

        /**
         * Method that configures and returns a configured integer style combo box.
         *
         * \param[in] action The action to be tied to the combo box.
         *
         * \return Returns a pointer to a newly created combo box.
         */
        static EQt::ComboBox* newIntegerNumberStyleComboBox(QAction* action);

        /**
         * Method that configures and returns a configured real number precision spin box.
         *
         * \param[in] action The action to be tied to the spin box.
         *
         * \return Returns a pointer to a newly created spin box.
         */
        static EQt::SpinBox* newRealNumberPrecisionSpinBox(QAction* action);

        /**
         * Method that configures and returns a configured integer style combo box.
         *
         * \param[in] action The action to be tied to the combo box.
         *
         * \return Returns a pointer to a newly created combo box.
         */
        static EQt::ComboBox* newRealNumberStyleComboBox(QAction* action);

        /**
         * Method that configures and returns a configured multiplier line editor.
         *
         * \param[in] action The action to be tied to the line editor.
         *
         * \return Returns a pointer to a newly created combo box.
         */
        static EQt::LineEdit* newRealNumberMultiplierLineEdit(QAction* action);
};

#endif
