/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HomeBuilderFinal class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QStringList>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QList>
#include <QMap>
#include <QColor>
#include <QPalette>
#include <QFont>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_recent_files_popup_menu.h>
#include <eqt_tool_button.h>

#include "main_window.h"
#include "home_builder_final.h"

const char HomeBuilderFinal::builderName[] = "HomeBuilderFinal";

HomeBuilderFinal::HomeBuilderFinal(QObject* parent):EQt::Builder(parent) {}


HomeBuilderFinal::~HomeBuilderFinal() {}


unsigned HomeBuilderFinal::priority() const {
    return builderPriority;
}


const char* HomeBuilderFinal::builderIdentifier() const {
    return builderName;
}


const char* HomeBuilderFinal::plugInName() const {
    return "";
}


void HomeBuilderFinal::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    /* Document and Application preferences (location depends on platform) */

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        window->addSeparator(tr("T&ools"));
        window->addMenuAction(tr("T&ools | &Document Preferences..."), "document_preferences");
        window->addMenuAction(tr("T&ools | Application &Preferences..."), "application_preferences");

    #elif (defined(Q_OS_DARWIN))

        window->addSeparator(tr("F&ormat"));
        window->addMenuAction(tr("F&ormat | Document Preferences..."), "document_preferences");
        window->addMenuAction(tr("Preferences..."), "application_preferences");

    #else

        #error Unknown platform.

    #endif

    window->action("document_preferences")->setToolTip(tr("Configure document-global settings"));
    window->action("document_preferences")->setWhatsThis(
        tr("You can use this menu option to configure document-global settings.")
    );

    window->action("application_preferences")->setToolTip(tr("Configure the application"));
    window->action("application_preferences")->setWhatsThis(
        tr("You can use this menu option to configure application-global settings.")
    );

    /* Window menu */

    window->addMenuAction(tr("&Window | &New Window"), "window_new");
    window->setToolTip("window_new", tr("Creates a new window"));
    window->setWhatsThis(
        "window_new",
        tr(
            "Use this option to create a new, separate, window showing another view into the current file.  "
            "Different parts of the same file can be viewed and edited from each window."
        )
    );

    window->addMenuAction(tr("&Window | &Close Window"), "window_close");
    window->setToolTip("window_close", "Closes this window");
    window->setWhatsThis(
       "window_close",
       tr(
           "Use this option to close the current window.  You may be prompted to save one or more files if closing "
           "this window could cause unsaved changes to be lost."
       )
    );

    window->addSeparator(tr("&Window"));

    window->addMenuAction(tr("&Window | New &Tab"), "window_new_tab");
    window->setToolTip("window_new_tab", tr("Creates a new tab"));
    window->setWhatsThis(
        "window_new_tab",
        tr(
            "Use this option to create a new tab in the current window showing another view into the the current "
            "file.  Different parts of the same file can be viewed and edited from each tab."
        )
    );

    window->addMenuAction(tr("&Window | C&lose Tab"), "window_close_tab");
    window->setToolTip("window_close_tab", tr("Close the current tab"));
    window->setWhatsThis(
        "window_close_tab",
        tr(
            "Use this option to close the curernt tab.  You may be prompted to save your work if closing this tab "
            "could cause unsaved changed to be lost."
        )
    );

    /* Help */

    /* TODO: Add search widget. */

    window->addMenuAction(tr("&Help | %1 &Help...").arg(EQt::ProgrammaticApplication::applicationName()), "help");
    window->setToolTip("help", tr("Help"));
    window->setWhatsThis("help", tr("Use this option to access detailed documentation."));

    window->addMenuAction(tr("&Help | Getting &Started..."), "help_getting_started");
    window->setToolTip("help_getting_started", tr("Start here !"));
    window->setWhatsThis(
        "help_getting_started",
        tr("Use this option to access a tutorial on %1").arg(EQt::ProgrammaticApplication::applicationName())
    );

    window->addMenuAction(tr("&Help | &Examples..."), "help_examples");
    window->setToolTip("help_examples", tr("Open an example"));
    window->setWhatsThis(
        "help_examples",
        tr("Use this option to open an example to use as a starting point or as inspiration.")
    );

    window->addSeparator(tr("&Help"));

    window->addMenuAction(tr("&Help | Send &Feedback..."), "help_send_feedback");
    window->setToolTip("help_send_feedback", tr("Tell us how to make you more successful"));
    window->setWhatsThis(
        "help_send_feedback",
        tr(
           "Your feedback, good or bad, is greatly appreciated.  You can use this option to send us your thoughts "
           "on %1.  We will use your feedback to identify areas for improvement.  In order to serve you better, "
           "please do not use this option to send us bug reports."
        ).arg(EQt::ProgrammaticApplication::applicationName())
    );

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        window->addMenuAction(
            tr("&Help | &About %1...").arg(EQt::ProgrammaticApplication::applicationName()),
            "help_about"
        );

    #elif (defined(Q_OS_DARWIN))

        window->addMenuAction(tr("About"), "help_about");

    #else

        #error Unknown platform

    #endif

    window->setToolTip("help_about", tr("About %1").arg(EQt::ProgrammaticApplication::applicationName()));
    window->setWhatsThis(
        "help_about",
        tr(
            "Use this option to view information about your license as well as information on installed "
            "plug-ins."
        )
    );
}


void HomeBuilderFinal::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    // We add the View | Docks | Reset default default action here so that it ends up being truly last
    // in the menu.

    window->addSeparator(tr("&View | &Docks"));

    QAction* resetDocksToDefaultAction = window->addMenuAction(
        tr("&View | &Docks | &Reset to default"),
        "reset_docks_to_default_action"
    );
    resetDocksToDefaultAction->setToolTip(tr("Restores all docks to a default state."));
    resetDocksToDefaultAction->setWhatsThis(
        tr("Use this option to cause all dock windows to be restored to their initial defualt state")
    );
}


void HomeBuilderFinal::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* homeDockToggleAction = mainWindow->action("home_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("hdock"), homeDockToggleAction);
    Q_ASSERT(success);

    QAction* insertDockToggleAction = mainWindow->action("insert_dock_toggle_action");
    success = mainWindow->setCommand(tr("insdock"), insertDockToggleAction);
    Q_ASSERT(success);

//    mainWindow->dockWidget("home")->raise();
    mainWindow->resetDocksToDefaultState();
    mainWindow->loadSettings(QString("mainWindow%1").arg(mainWindow->windowIndex()));

    QAction* quitAction = mainWindow->action("quit");
    success = mainWindow->setCommand(tr("quit"), quitAction);
    Q_ASSERT(success);

    QAction* fileNewAction = mainWindow->action("file_new");
    success = mainWindow->setCommand(tr("fnew"), fileNewAction);
    Q_ASSERT(success);

    QAction* fileOpenAction = mainWindow->action("file_open_dialog");
    success = mainWindow->setCommand("fopen", fileOpenAction);
    Q_ASSERT(success);

    QAction* fileSaveAction = mainWindow->action("file_save");
    success = mainWindow->setCommand("fsave", fileSaveAction);
    Q_ASSERT(success);

    QAction* fileSaveAsAction = mainWindow->action("file_save_as");
    success = mainWindow->setCommand(tr("fsaveas"), fileSaveAsAction);
    Q_ASSERT(success);

    QAction* fileCloseAction = mainWindow->action("file_close");
    success = mainWindow->setCommand(tr("fclose"), fileCloseAction);
    Q_ASSERT(success);

    QAction* fileCloseTabAction = mainWindow->action("file_close_tab");
    success = mainWindow->setCommand(tr("tabclose"), fileCloseTabAction);
    Q_ASSERT(success);

    QAction* windowNewAction = mainWindow->action("window_new");
    success = mainWindow->setCommand(tr("wnew"), windowNewAction);
    Q_ASSERT(success);

    QAction* windowCloseAction = mainWindow->action("window_close");
    success = mainWindow->setCommand(tr("wclose"), windowCloseAction);
    Q_ASSERT(success);

    QAction* windowNewTabAction = mainWindow->action("window_new_tab");
    success = mainWindow->setCommand(tr("tabnew"), windowNewTabAction);
    Q_ASSERT(success);

    QAction* applicationPreferencesAction = mainWindow->action("application_preferences");
    success = mainWindow->setCommand(tr("preferences"), applicationPreferencesAction);
    Q_ASSERT(success);

    QAction* helpAction = mainWindow->action("help");
    success = mainWindow->setCommand(tr("help"), helpAction);
    Q_ASSERT(success);

    QAction* helpGettingStartedAction = mainWindow->action("help_getting_started");
    success = mainWindow->setCommand(tr("getstarted"), helpGettingStartedAction);
    Q_ASSERT(success);

    QAction* helpExamplesAction = mainWindow->action("help_examples");
    success = mainWindow->setCommand(tr("examples"), helpExamplesAction);
    Q_ASSERT(success);

    QAction* helpSendFeedbackAction = mainWindow->action("help_send_feedback");
    success = mainWindow->setCommand(tr("feedback"), helpSendFeedbackAction);
    Q_ASSERT(success);

    QAction* helpAboutAction = mainWindow->action("help_about");
    success = mainWindow->setCommand(tr("about"), helpAboutAction);
    Q_ASSERT(success);
}
