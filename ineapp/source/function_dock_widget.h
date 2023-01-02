/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FunctionDockWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FUNCTION_DOCK_WIDGET_H
#define FUNCTION_DOCK_WIDGET_H

#include <QString>

#include <eqt_programmatic_dock_widget.h>

#include "app_common.h"

class QAction;
class QModelIndex;

namespace EQt {
    class ToolButton;
    class ProgrammaticMainWindow;
}

namespace Ld {
    class FunctionData;
    class Capabilities;
}

class FunctionBrowserModel;

/**
 * Class derived from EQt::ProgrammaticDockWidget that provides a dock for the function browser.
 */
class APP_PUBLIC_API FunctionDockWidget:public EQt::ProgrammaticDockWidget {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this window.
         *
         * \param[in] flags  Flags that can be used to modify the behavior of this dock widget.
         */
        FunctionDockWidget(EQt::ProgrammaticMainWindow* parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

        ~FunctionDockWidget() override;

        /**
         * Method that is called by the main window after all the actions are created to configure this dock widget.
         *
         * \param[in] mainWindow Pointer to the main window object.
         */
        void configureDockWidget(EQt::ProgrammaticMainWindow* mainWindow) override;

        /**
         * Method that is called to set the desired height adjustment for the dock widget.
         *
         * \param[in] heightAdjustment The non-negative adjustment from the minimum height required to make the
         *                             dock widget fit nicely.
         */
        void setHeightAdjustment(unsigned heightAdjustment) override;

        /**
         * Method that is called to set the desired height adjustment for the dock widget.
         *
         * \param[in] widthAdjustment The non-negative adjustment from the minimum width required to make the dock
         *                            widget fit nicely.
         */
        void setWidthAdjustment(unsigned widthAdjustment) override;

    signals:
        /**
         * Signal that is emitted when a user selects a new function.
         *
         * \param[out] functionData The function data instance that was recently selected.
         */
        void functionSelected(const Ld::FunctionData& functionData);

        /**
         * Signal that is emitted when a user selects a new function.
         *
         * \param[out] functionData          The function data instance that was recently selected.
         *
         * \param[out] parameterDescriptions A list of strings holding the descriptions for the parameters.
         */
        void functionSelected(const Ld::FunctionData& functionData, const QStringList& parameterDescriptions);

        /**
         * Signal that is emitted when help on a function is requested.
         *
         * \param[out] functionData The function data instance that help is being requested for.
         */
        void helpRequested(const Ld::FunctionData& functionData);

    public slots:
        /**
         * Slot that is triggered when the current requirements have changed.
         *
         * \param[out] newRequirements The new requirements reported by the cursor.
         */
        void requirementsChanged(const Ld::Capabilities& newRequirements);

    protected slots:
        /**
         * Slot that is triggered when the dock is made to float or is docked.  You can overload this slot if you
         * need to receive notation that the dock has been made floating or docked.  This version calls the base class
         * method and then enables or disables resizing capabilities.
         *
         * \param[in] topLevel Holds true if the dock is now floating.
         */
        void topLevelChanged(bool topLevel) override;

    private slots:
        /**
         * Slot that is triggered when the user clicks on an item in the horizontal table view.
         *
         * \param[in] index The index indicating the location where the user pressed the mouse button.
         */
        void horizontalTableClicked(const QModelIndex& index);

        /**
         * Slot that is triggered when the horizontal table view is double clicked.
         *
         * \param[in] index The index indicating the location where the table was clicked.
         */
        void horizontalTableDoubleClicked(const QModelIndex& index);

        /**
         * Slot that is triggered when the user clicks on an item in the vertical table view.
         *
         * \param[in] index The index indicating the location where the user pressed the mouse button.
         */
        void verticalTableClicked(const QModelIndex& index);

        /**
         * Slot that is triggered when the vertical table view is double clicked.
         *
         * \param[in] index The index indicating the location where the table was clicked.
         */
        void verticalTableDoubleClicked(const QModelIndex& index);

        /**
         * Slot that is triggered when the horizontal insert standard function button is clicked.
         */
        void horizontalInsertStandardFunctionButtonClicked();

        /**
         * Slot that is triggered when the vertical insert standard function button is clicked.
         */
        void verticalInsertStandardFunctionButtonClicked();

        /**
         * Slot that is triggered when the horizontal function help button is clicked.
         */
        void horizontalFunctionHelpButtonClicked();

        /**
         * Slot that is triggered when the vertical function help button is clicked.
         */
        void verticalFunctionHelpButtonClicked();

        /**
         * Slot that is triggered when the horizontal line editor is updated.
         *
         * \param[in] newText The new line editor text.
         */
        void horizontalLineEditorEdited(const QString& newText);

        /**
         * Slot that is triggered when the horizontal line editor is updated.
         *
         * \param[in] newText The new line editor text.
         */
        void verticalLineEditorEdited(const QString& newText);

    private:
        /**
         * Minimum allowed width for the tree view.
         */
        static constexpr unsigned minimumTreeViewWidth = 100;

        /**
         * Builds the widget's horizontal orientation.
         *
         * \param[in] mainWindow           Pointer to the main window object.
         *
         * \param[in] functionBrowserModel The function browser model shared by both function views.
         */
        void buildHorizontalOrientation(
            EQt::ProgrammaticMainWindow* mainWindow,
            FunctionBrowserModel*        functionBrowserModel
        );

        /**
         * Builds the widget's vertical orientation.
         *
         * \param[in] mainWindow           Pointer to the main window object.
         *
         * \param[in] functionBrowserModel The function browser model shared by both function views.
         */
        void buildVerticalOrientation(
            EQt::ProgrammaticMainWindow* mainWindow,
            FunctionBrowserModel*        functionBrowserModel
        );

        /**
         * Method that is used to insert a new function.
         *
         * \param[in] modelIndex The model index into the tree view.
         */
        void emitFunctionDataEvent(const QModelIndex& modelIndex);

        /**
         * Method that is used to display function help data.
         *
         * \param[in] modelIndex The model index into the tree view.
         */
        void displayFunctionHelp(const QModelIndex& modelIndex);
};

#endif
