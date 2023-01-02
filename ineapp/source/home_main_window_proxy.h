/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HomeMainWindowProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HOME_MAIN_WINDOW_PROXY_H
#define HOME_MAIN_WINDOW_PROXY_H

#include <QObject>
#include <QPoint>
#include <QProgressDialog>

#include <eqt_programmatic_main_window_proxy.h>

namespace EQt {
    class ProgrammaticMainWindow;
}

class Document;
class ViewWidget;

/**
 * Main window proxy used to manage the home functions.  These functions include:
 *
 *     * Basic file menu items
 *     * Basic edit functions
 *     * Basic editor display/zoom functions.
 */
class HomeMainWindowProxy:public EQt::ProgrammaticMainWindowProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] window The main window class we are proxying for.
         */
        HomeMainWindowProxy(EQt::ProgrammaticMainWindow* window);

        ~HomeMainWindowProxy() override;

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
         * Method that is called to request permission to close the window.
         *
         * \return Returns true if the window can be closed.  Returns false if the window should not be closed.
         */
        bool okToClose() final;

        /**
         * Method that is called to inform this proxy that the window is about to close.
         */
        void closing() final;

        /**
         * Method that is called after the window is fully configured and before the window first becomes visible.
         *
         * This method will recalculate the new minimum height and width values for the home dock.
         */
        void windowConfigured() override;

    private slots:
        /**
         * Slot that is triggered when the user requests to exit the application.
         */
        void quit();

        /**
         * Slot that is triggered for the File | New menu item.
         */
        void fileNew();

        /**
         * Slot that is triggered for the File | Open menu item.
         */
        void fileOpenDialog();

        /**
         * Slot that is triggered for the File | Open Recent menu item.
         */
        void fileOpenRecent();

        /**
         * Slot that is triggered for the File | Save menu item.
         */
        void fileSave();

        /**
         * Slot that is triggered for the File | Save As menu item.
         */
        void fileSaveAs();

        /**
         * Slot that is triggered for the File | Close menu item.
         */
        void fileClose();

        /**
         * Slot that is triggered for the File | Close Tab menu item.
         */
        void fileCloseTab();

        #if (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

            /**
             * Slot that is triggered for the View | Enter/Exit Full Screen menu item.
             */
            void viewEnterExitFullScreen();

        #endif

        /**
         * Slot that is tirggered for the View | Docks | Reset to default menu item.
         */
        void resetDocksToDefault();

        /**
         * Slot that is triggered for the Window | New Window menu item.
         */
        void windowNewWindow();

        /**
         * Slot that is triggered for the Window | Close Window menu item.
         */
        void windowCloseWindow();

        /**
         * Slot that is triggered for the Window | New Tab menu item.
         */
        void windowNewTab();

        /**
         * Slot that is triggered for the Window | Close Tab menu item.
         */
        void windowCloseTab();

        /**
         * Slot that is triggered for the Application Preferences.. dialog.
         */
        void appicationPreferences();

        /**
         * Slot that is triggered for the Help | Help menu item.
         */
        void help();

        /**
         * Slot that is triggered for the Help | Getting Started... menu item.
         */
        void helpGettingStarted();

        /**
         * Slot that is triggered for the Help | Examples... menu item.
         */
        void helpOpenExamples();

        /**
         * Slot that is triggered for the Help | Send Feedback... menu item.
         */
        void helpSendFeedback();

        /**
         * Slot that is triggered for the Help | About menu item.
         */
        void helpAbout();

        /**
         * Slot that displays a context menu for a tab.
         *
         * \param[in] location The location where the user clicked.
         *
         * \param[in] tabIndex The index into the tab bar that is of interest.
         */
        void displayTabBarContextMenu(const QPoint& location, unsigned tabIndex);

        #if (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

            /**
             * Slot that is triggered for the View | Enter Full Screen menu item.
             */
            void windowStateChanged(bool nowMinimized, bool nowMaximized, bool nowFullScreen);

        #endif

        /**
         * Slot that is triggered when the clipboard becomes non-empty due to external activity.
         */
        void clipboardUpdated();

        /**
         * Slot that is triggered when we are waiting for a document to abort repositioning, execution, or translation.
         */
        void backgroundOperationAborted();

        /**
         * Slot that is triggered when the screen size changes.
         */
        void screenResized();

    private:
        /**
         * Method that opens a file and places it into an appropriate view in this dialog.
         *
         * \param[in] fileName The name of the file to be opened.
         *
         * \return Returns true on success, returns false on error.
         */
        bool openFile(const QString& fileName);

        /**
         * Method that is triggered from the tab menu when a user requests a file be opened in a new tab.
         *
         * \param[in] index The index of the tab that triggered the request.
         *
         * \return Returns true if the operation occurred.  Returns false if the operation failed or was aborted.
         */
        bool openInNewTab(unsigned index);

        /**
         * Method that is triggered from the tab menu when a user requests a file be opened in a new window.
         *
         * \param[in] index The index of the tab that triggered the request.
         *
         * \return Returns true if the operation occurred.  Returns false if the operation failed or was aborted.
         */
        bool openInNewWindow(unsigned index);

        /**
         * Method that is triggered from the tab menu when a user requests a file be saved from a tab.
         *
         * \param[in] index The index of the tab that triggered the request.
         *
         * \return Returns true if the operation occurred.  Returns false if the operation failed or was aborted.
         */
        bool saveFromTab(unsigned index);

        /**
         * Method that is triggered from the tab menu when a user requests a file be saved under a new name from a tab.
         *
         * \param[in] index The index of the tab that triggered the request.
         *
         * \return Returns true if the operation occurred.  Returns false if the operation failed or was aborted.
         */
        bool saveAsFromTab(unsigned index);

        /**
         * Method that is triggered when the user requests a tab to be closed.
         *
         * \param[in] index The index of the tab to be closed.
         *
         * \return Returns true if the operation occurred.  Returns false if the operation failed or was aborted.
         */
        bool closeTab(unsigned index);

        /**
         * Method that closes a file at a given tab.
         *
         * \param[in] index The index of the tab that triggered the request.
         *
         * \return Returns true if the operation occurred.  Returns false if the operation failed or was aborted.
         */
        bool closeFileFromTab(unsigned index);

        /**
         * Method that is used to close all references to a document.
         *
         * \param[in] document The document to be closed.
         */
        void closeDocument(Document* document);

        /**
         * Method you can call to shut down one or more repositioning or background translation/execute operations.
         *
         * \param[in] documents The documents that do (or may) need to have their rendering operations shut-down.
         *
         * \return Returns the number of pending abort requests.
         */
        unsigned shutdownRenderingTranslationOrExecution(const QList<Document*> documents);

        /**
         * Method you can call to display a progress dialog.
         */
        void enableShutdownStatusDialog();

        /**
         * Method you can use to generate a bulleted list of all new imported documents.
         *
         * \param[in] document The document to obtain the imports for.
         *
         * \return Returns a HTML bulleted list of all new imported documents.  An empty string is returned if there
         *         are no unsaved documents.
         */
        static QString allNewImports(Document* document);

        /**
         * Method you can use to generate a bulleted list of all unsaved imported documents.
         *
         * \param[in] document The document to obtain the imports for.
         *
         * \return Returns a HTML bulleted list of all unsaved imported documents.
         */
        static QString allUnsavedImports(Document* document);

        /**
         * Method you can use to conditionally show the unsaved new imports messagebox.
         *
         * \param[in] document The document to obtain the imports for.
         *
         * \return Returns true if it's OK to save the current document.  Returns false if unsaved imports need to be
         *         saved off first.
         */
        bool checkForUnsavedNewImports(Document* document) const;

        /**
         * Method you can use to conditionally show the unsaved changed imports messagebox.
         *
         * \param[in] document The document to obtain the imports for.
         *
         * \return Returns true if there are no unsaved referenced imports.  Returns false if there are one or more
         *         unsaved referenced imports.
         */
        bool checkForUnsavedChangedImports(Document* document) const;

        /**
         * Enumeration of actions to perform after all document background operations have shut down.
         */
        enum class ActionAfterAborted {
            /**
             * Indicates no action.
             */
            NONE,

            /**
             * Indicates that all application windows should be closed.
             */
            QUIT,

            /**
             * Indicates that we should close a specific open tab.
             */
            CLOSE_TAB,

            /**
             * Indicates that we should close the window.
             */
            CLOSE_WINDOW,

            /**
             * Indicates that we should close the file.
             */
            CLOSE_FILE
        };

        /**
         * A count of the current number of outstanding documents to be shut down.
         */
        unsigned remainingDocumentsToAbortOperation;

        /**
         * Indicates the action to perform after all document rendering is shutdown.
         */
        ActionAfterAborted actionAfterAborted;

        /**
         * Value holding the index of the tab to be closed.
         */
        unsigned indexOfTabToClose;

        /**
         * Pointer to the document to be closed.
         */
        Document* documentToClose;

        /**
         * Flag indicating that we've already prompted for window close and should not prompt again.
         */
        bool forceWindowClose;

        /**
         * A progress dialog used to indicate that an operation is pending.
         */
        QDialog* shutdownStatusDialog;
};

#endif
