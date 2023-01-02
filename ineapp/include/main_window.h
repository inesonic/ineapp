/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MainWindow class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QList>
#include <QMutex>
#include <QString>
#include <QPoint>
#include <QFont>
#include <QIcon>

#include <eqt_programmatic_main_window.h>

#include <ld_capabilities.h>

#include "app_common.h"
#include "command_popup_dialog.h"
#include "cursor.h"

class QWidget;
class QCloseEvent;
class QResizeEvent;

class ViewWidget;
class Document;

/**
 * Class that creates the main application window.  Note that the \ref MainWindow class is created with the
 * Qt::WA_DeleteOnClose attribute set and will thus automatically destroy itself.  For this reason, the window does not
 * require or accept a pointer to a parent object.
 *
 * Note that eventually we probably should find a clean way to decouple the cursor state from this window and also find
 * a way to remove the command popup from this window.
 */
class APP_PUBLIC_API MainWindow:public EQt::ProgrammaticMainWindow {
    Q_OBJECT

    public:
        /**
         * Iterator used to walk the list of main windows.
         */
        typedef QList<MainWindow*>::const_iterator Iterator;

        /**
         * Type used to represent a list of executable actions, by user command.
         */
        typedef CommandPopupDialog::ActionsByCommand ActionsByCommand;

        /**
         * Method you can use to obtain a snapshop of the current window list.
         *
         * \return a snapshot of the current list of main windows.
         */
        static QList<MainWindow*> allWindows();

        /**
         * Method you can use to obtain an iterator into the list of windows.
         *
         * \return Returns an iterator to the first known main window.
         */
        static Iterator begin();

        /**
         * Method you can use to obtain an end iterator for the list of main windows.
         */
        static Iterator end();

        /**
         * Method that can be called to close all the application main windows.  Note that this call is not thread
         * safe.
         *
         * \param[in] noPrompt If true, prompts to the user will be suppressed.
         */
        static void closeAllMainWindows(bool noPrompt);

        /**
         * Method that can be called to destroy all the application main windows.  Note that this call is not thread
         * safe.
         */
        static void deleteAllMainWindows();

        MainWindow();

        /**
         * Constructor.
         *
         * \param[in] document The document to include in the first view of the window.
         */
        explicit MainWindow(Document* document);

        ~MainWindow() override;

        /**
         * Method you can use to obtain an index for this window.  The value will match the index of the window in the
         * list returned by \ref MainWindow::allWindows.
         *
         * \return Returns a zero based index for this window in the list of main windows.
         */
        unsigned windowIndex() const;

        /**
         * Loads settings to be used by the main window.  This method will be called once when the window is first
         * created and should restore Window state information.  The method may be called again to restore views.
         *
         * \param[in] groupName The name of the settings group to use for the main window.
         */
        void loadSettings(const QString& groupName) final;

        /**
         * Saves settings used by the main window.
         *
         * \param[in] groupName The name of the settings group to use for the main window.
         */
        void saveSettings(const QString& groupName) final;

        /**
         * Method you can use to associate a set of actions to commands.
         *
         * \param[in] newCommandMap A map of QAction instances indexed by command.  The \ref CommandPopupDialog will
         *                          use this map to tie commands to actions.  The tool-tip for each QAction will be
         *                          used as the description for the command.
         */
        void setCommands(const ActionsByCommand& newCommandMap);

        /**
         * Method you can use to obtain the current map of actions, keyed by commands.
         *
         * \return Returns a map of QAction instances indexed by command.
         */
        const ActionsByCommand& commands() const;

        /**
         * Method you can use to tie a specific action to a command.
         *
         * \param[in] command           The command to tie to the action.
         *
         * \param[in] action            The action to tie to the command.
         *
         * \param[in] overwriteExisting If true, existing commands will be replaced.  If false, the method will refuse
         *                              to overwrite existing commands.
         *
         * \return Returns true if the command was new.  Returns false if the command already existed in the command
         *         map.
         */
        bool setCommand(const QString& command, QAction* action, bool overwriteExisting = false);

        /**
         * Method that returns the views managed by this window.
         *
         * \return Returns a list of views associated with this window.
         */
        QList<ViewWidget*> viewWidgets() const;

        /**
         * Method that returns the current number of tabs in the window.
         *
         * \return Returns the number of views in the window.
         */
        unsigned numberViews() const;

        /**
         * Method that returns the currently active view.
         *
         * \return Returns the zero based index of the currently active view.
         */
        unsigned currentViewIndex() const;

        /**
         * Method that returns the currently active view.
         *
         * \return Returns a pointer to the current view widget.
         */
        ViewWidget* currentView() const;

        /**
         * Method that changes the currently active view.  Note that the MainWindow::currentTabChanged slot will be
         * triggered.
         *
         * \param[in] index The index of the desired tab.
         */
        void setCurrentView(unsigned index);

        /**
         * Method that provides the ViewWidget for a specific tab, by index.
         *
         * \param[in] index The tab index to get the view of.
         *
         * \return Returns the desired view widget.
         */
        ViewWidget* viewWidget(unsigned index) const;

        /**
         * Method that identifies the tab index for a given ViewWidget.
         *
         * \param[in] view the pointer to the view widget to locate.
         *
         * \return Returns the tab index.  A negative value is returned if the tab is not located.
         */
        int tabIndex(ViewWidget* view);

        /**
         * Method that changes the currently active view.  Note that the MainWindow::currentTabChanged slot will be
         * triggered.
         *
         * \param[in] viewWidget The view to be displayed.
         *
         * \return Returns true on success.  Returns false if the View is not in this window.
         */
        bool setCurrentView(ViewWidget* viewWidget);

        /**
         * Creates a new editor view.
         *
         * \param [in] document The document to associated with this view.  A null pointer will cause a view to be
         *                      created that contains an empty/new document.
         *
         * \return Returns the index of the newly added view.
         */
        unsigned addView(Document* document = Q_NULLPTR);

        /**
         * Removes a tab and optionally destroys the underlying document instance.
         *
         * \param[in] index The index of the tab to be removed.
         */
        void removeView(unsigned index);

        /**
         * Method that changes the text associated with a tab.
         *
         * \param[in] index The index of the tab.
         *
         * \param[in] text  The text to place on the tab.
         */
        void setTabText(unsigned index, const QString& text);

        /**
         * Method that places an icon on a tab.
         *
         * \param[in] index The index of the tab.
         *
         * \param[in] icon  The icon to place on the tab.
         */
        void setTabIcon(unsigned index, const QIcon& icon);

        /**
         * Method that removes an icon on a tab.
         *
         * \param[in] index The index of the tab.
         */
        void clearTabIcon(unsigned index);

        /**
         * Method that locates the first view in the window with a view containing a document with modification.  The
         * method then sets that view to the current view.
         *
         * \return Returns true if a modified document was located.  Returns false if the window contains no modified
         *         documents.
         */
        bool displayModifiedDocument();

        /**
         * Method that generates a list of all documents visible within a main window.
         *
         * \return Returns a list of all visible document instances.
         */
        static QList<Document*> visibleDocuments();

    signals:
        /**
         * Signal that is emitted when the user right-clicks on a tab on the window.  You can use this from proxy
         * classes to display a context menu.
         *
         * \param[out] location The location where the user clicked.
         *
         * \param[out] tabIndex The zero based index of the tab.
         */
        void tabBarContextMenuRequested(const QPoint& location, unsigned tabIndex);

        /**
         * Signal that is emitted when the window is maximized or unmaximized.
         *
         * \param[out] nowMinimized  Holds true if the window is now minimized.  Holds false if the window is no longer
         *                           minimized.
         *
         * \param[out] nowMaximized  Holds true if the window is now maximized.  Holds false if the window is no longer
         *                           maximized.
         *
         * \param[out] nowFullScreen Holds true if the window is now full screen.  Holds false if the window is no
         *                           longer full screen.
         */
        void windowStateChanged(bool nowMinimized, bool nowMaximized, bool nowFullScreen);

    public slots:
        /**
         * Slot that is triggered when the requirements at the cursor position has changed.  This slot updates the
         * window's action state which then enables/disables user options.
         *
         * \param[out] newRequirements The new requirements reported by the cursor.
         */
        void requirementsChanged(const Ld::Capabilities& newRequirements);

        /**
         * Slot that displays the command popup dialog for the main window.
         *
         * \param[in] globalPosition The location where the user wants the popup menu.
         *
         * \param[in] fontAtLocation The font at the requested location.
         */
        void displayCommandPopupDialog(const QPoint& globalPosition, const QFont& fontAtLocation);



    protected:
        /**
         * Binds all the actions and signals to slots then binds all the proxies to this class by calling the
         * EQt::ProgrammaticWidgetProxy::bind method in each proxy
         * class.
         */
        void bind() final;

        /**
         * Event handle for window changes.
         *
         * This event handler will check for window maximized events and generate the MainWindow::maximized signal if
         * needed.
         *
         * \param[in] event The event that triggered this call.
         */
        void changeEvent(QEvent* event) override;

        /**
         * Event handler that is triggered when the user attempts to close the window.
         *
         * The event handler will test all the views in this window to determine if the window can close.  After that,
         * the method will call the ProgrammaticMainWindowProxy::okToClose method on each proxy for this window.  The
         * window will be closed only if this method decides the window can close and every proxy returns a value of
         * true.
         *
         * If the window can be closed, the method will call the ProgrammaticMainWindowProxy::closing method to inform
         * the proxy classes that the window is about to be closed.
         *
         * \param[in] event The event that triggered the window to close.
         */
        void closeEvent(QCloseEvent* event) final;

    private slots:
        /**
         * Slot that is triggered when the user selects a new tab.
         *
         * \param[in] index The index of the newly selected tab.
         */
        void currentTabChanged(int index);

        /**
         * Slot that is triggered when the screen size, orientation, or DPI value changes.
         *
         * \param[in] newPhysicalDpi              The new physical screen DPI.
         *
         * \param[in] newLogicalDpi               The new logical screen DPI.
         *
         * \param[in] newToolButtonHeightInPixels The new button icon height, in pixels.
         */
        void screenSettingsChanged(
            const QSize& newPhysicalDpi,
            const QSize& newLogicalDpi,
            unsigned     newToolButtonHeightInPixels
        );

    private:
        /**
         * Value used to calculate the minimum window width, in tool button increments.
         */
        static constexpr unsigned minimumWidthToolButtons = 24;

        /**
         * Value used to calculate the minimum window height, in tool button increments.
         */
        static constexpr unsigned minimumHeightToolButtons = 18;

        /**
         * Method that can be called from the constructor to instruct the builder objects to configure this window.
         */
        void configureWindow();

        /**
         * Method that builds the central widget for the window.
         */
        void buildCentralWidget();

        /**
         * The view widget associated with the currently active tab.
         */
        ViewWidget* activeView;

        /**
         * The last reported parent requirements.  Used to determine if the action state needs to be updated.
         */
        Ld::Capabilities currentParentRequirements;

        /**
         * The command popup dialog tied to this window.
         */
        CommandPopupDialog* currentCommandPopupDialog;

        /**
         * Flag used to indicate if the user should be prompted before closing a window.
         */
        static bool promptOnMainWindowClose;

        /**
         * All the active main windows.  This static list is automatically updated by the class constructors.
         */
        static QList<MainWindow*> mainWindows;
};

#endif
