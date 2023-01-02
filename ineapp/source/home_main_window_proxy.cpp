/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HomeMainWindowProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QPoint>
#include <QMessageBox>
#include <QDialog>
#include <QFileDialog>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QGridLayout>

#include <ud_usage_data.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_color_tool_button.h>
#include <eqt_tab_widget.h>
#include <eqt_recent_files_data.h>

#include <ld_document_setting.h>
#include <ld_environment.h>
#include <ld_element.h>
#include <ld_root_element.h>

#include "document.h"
#include "application.h"
#include "application_settings.h"
#include "build_execute_state_machine.h"
#include "clipboard.h"
#include "main_window.h"
#include "view_widget.h"
#include "application_preferences_dialog.h"
#include "desktop_services.h"
#include "about_dialog.h"
#include "document_file_dialog.h"
#include "home_builder_initial.h"
#include "home_main_window_proxy.h"

HomeMainWindowProxy::HomeMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticMainWindowProxy(
        window
    ) {
    remainingDocumentsToAbortOperation = 0;
    actionAfterAborted                 = ActionAfterAborted::NONE;
    indexOfTabToClose                  = static_cast<unsigned>(-1);
    forceWindowClose                   = false;

    shutdownStatusDialog = Q_NULLPTR;
}


HomeMainWindowProxy::~HomeMainWindowProxy() {
    if (shutdownStatusDialog != Q_NULLPTR) {
        delete shutdownStatusDialog;
    }
}


void HomeMainWindowProxy::loadSettings() {}


void HomeMainWindowProxy::saveSettings() {}


bool HomeMainWindowProxy::okToClose() {
    bool        okToCloseWindow = true;

    if (!forceWindowClose) {
        MainWindow* window      = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
        unsigned    numberViews = window->numberViews();
        unsigned    viewIndex   = 0;

        BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
        Document*                 documentBeingBuilt       = Q_NULLPTR;

        if (buildExecuteStateMachine->isActive()) {
            documentBeingBuilt = dynamic_cast<Document*>(buildExecuteStateMachine->rootElement()->visual());
        }

        // FIXME:
        //   Code below is a bit of a kludge as it forces the Editor class to know what type of object contains it.
        //   We can probably clean this up quite a bit.

        QSet<Document*> destroyedDocuments;
        while (viewIndex < numberViews) {
            ViewWidget* view     = window->viewWidget(viewIndex);
            Document*   document = view->document();

            if (view->isModified() || document == documentBeingBuilt) {
                bool documentInAnotherWindow = false;

                MainWindow::Iterator it  = MainWindow::begin();
                MainWindow::Iterator end = MainWindow::end();
                while (it != end && !documentInAnotherWindow) {
                    MainWindow* alternateWindow = *it;
                    if (alternateWindow != window) {
                        unsigned numberViewsAlternateWindow = alternateWindow->numberViews();
                        unsigned alternateViewIndex         = 0;
                        while (alternateViewIndex < numberViewsAlternateWindow && !documentInAnotherWindow) {
                            ViewWidget* alternateView = alternateWindow->viewWidget(alternateViewIndex);
                            documentInAnotherWindow = (alternateView->document() == document);
                            ++alternateViewIndex;
                        }
                    }

                    ++it;
                }

                if (!documentInAnotherWindow) {
                    destroyedDocuments.insert(document);
                }

                if (!documentInAnotherWindow) {
                    okToCloseWindow = false;
                }
            }

            ++viewIndex;
        }

        if (!okToCloseWindow) {
            QMessageBox::StandardButton result = QMessageBox::warning(
                window,
                tr("Warning"),
                tr("Unsaved changes will be lost.\nDo you really want to close this window ?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );

            okToCloseWindow = (result == QMessageBox::Yes);
        }

        if (okToCloseWindow) {
            actionAfterAborted = ActionAfterAborted::CLOSE_WINDOW;
            if (shutdownRenderingTranslationOrExecution(destroyedDocuments.values()) != 0) {
                okToCloseWindow  = false;
            }

            forceWindowClose = true;
        }
    }

    return okToCloseWindow;
}


void HomeMainWindowProxy::closing() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    window->saveSettings(QString("mainWindow%1").arg(window->windowIndex()));
}


void HomeMainWindowProxy::windowConfigured() {
    EQt::ProgrammaticDockWidget* homeDock   = window()->dockWidget(HomeBuilderInitial::homeDockName);
    EQt::ProgrammaticDockWidget* insertDock = window()->dockWidget(HomeBuilderInitial::insertDockName);

    // Structure of both the home dock and the insert dock are basically the same so we calculate the geometry for
    // just the home dock and then apply to both docks.

    QGridLayout*     gridLayout = homeDock->EQt::ProgrammaticWidget::layout<QGridLayout>("horizontal_grid");
    QGridLayout*     editLayout = homeDock->EQt::ProgrammaticWidget::layout<QGridLayout>("vertical_edit");

    QSize toolButtonSize = EQt::ToolButton::sizePixels();

    unsigned minimumHeightHorizontalOrientation = std::max(
        static_cast<unsigned>(
           1 * gridLayout->spacing()
         + 2 * toolButtonSize.height()
         + gridLayout->contentsMargins().top()
         + gridLayout->contentsMargins().bottom()
        ),
        homeDock->minimumRecommendedHorizontalOrientationTitleBarHeight()
    );

    unsigned homeMinimumWidthVerticalOrientation = (
          5 * editLayout->spacing()
        + 5 * toolButtonSize.width()
        + 6 // There's a small gap in place of a column.
        + editLayout->contentsMargins().left()
        + editLayout->contentsMargins().right()
    );
    unsigned insertMinimumWidthVerticalOrientation = (
          4 * editLayout->spacing()
        + 5 * toolButtonSize.width()
        + editLayout->contentsMargins().left()
        + editLayout->contentsMargins().right()
    );

    homeDock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    homeDock->setMinimumDockWidth(homeMinimumWidthVerticalOrientation);

    insertDock->setMinimumDockHeight(minimumHeightHorizontalOrientation);
    insertDock->setMinimumDockWidth(insertMinimumWidthVerticalOrientation);
}


void HomeMainWindowProxy::bind() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());

    connect(window->action("quit"), &QAction::triggered, this, &HomeMainWindowProxy::quit);
    connect(window->action("file_new"), &QAction::triggered, this, &HomeMainWindowProxy::fileNew);
    connect(window->action("file_open_dialog"), &QAction::triggered, this, &HomeMainWindowProxy::fileOpenDialog);
    connect(window->action("file_open_recent"), &QAction::triggered, this, &HomeMainWindowProxy::fileOpenRecent);
    connect(window->action("file_save"), &QAction::triggered, this, &HomeMainWindowProxy::fileSave);
    connect(window->action("file_save_as"), &QAction::triggered, this, &HomeMainWindowProxy::fileSaveAs);
    connect(window->action("file_close"), &QAction::triggered, this, &HomeMainWindowProxy::fileClose);
    connect(window->action("file_close_tab"), &QAction::triggered, this, &HomeMainWindowProxy::fileCloseTab);

    window->action("edit_paste")->setEnabled(Application::clipboard()->containsData());

    #if (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

        connect(
            window->action("view_enter_exit_full_screen"),
            &QAction::triggered,
            this,
            &HomeMainWindowProxy::viewEnterExitFullScreen
        );

    #endif

    connect(
        window->action("reset_docks_to_default_action"),
        &QAction::triggered,
        this,
        &HomeMainWindowProxy::resetDocksToDefault
    );

    connect(window->action("window_new"), &QAction::triggered, this, &HomeMainWindowProxy::windowNewWindow);
    connect(window->action("window_close"), &QAction::triggered, this, &HomeMainWindowProxy::windowCloseWindow);
    connect(window->action("window_new_tab"), &QAction::triggered, this, &HomeMainWindowProxy::windowNewTab);
    connect(window->action("window_close_tab"), &QAction::triggered, this, &HomeMainWindowProxy::windowCloseTab);
    connect(
        window->action("application_preferences"),
        &QAction::triggered,
        this,
        &HomeMainWindowProxy::appicationPreferences
    );
    connect(window->action("help"), &QAction::triggered, this, &HomeMainWindowProxy::help);
    connect(
        window->action("help_getting_started"),
        &QAction::triggered,
        this,
        &HomeMainWindowProxy::helpGettingStarted
    );
    connect(window->action("help_examples"), &QAction::triggered, this, &HomeMainWindowProxy::helpOpenExamples);
    connect(window->action("help_send_feedback"), &QAction::triggered, this, &HomeMainWindowProxy::helpSendFeedback);
    connect(window->action("help_about"), &QAction::triggered, this, &HomeMainWindowProxy::helpAbout);

    connect(window, &MainWindow::tabBarContextMenuRequested, this, &HomeMainWindowProxy::displayTabBarContextMenu);

    #if (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

        connect(window, &MainWindow::windowStateChanged, this, &HomeMainWindowProxy::windowStateChanged);

    #endif

    connect(
        Application::clipboard(),
        static_cast<void (Clipboard::*)()>(&Clipboard::clipboardUpdated),
        this,
        &HomeMainWindowProxy::clipboardUpdated
    );

    connect(
        Application::instance(),
        &Application::screenResized,
        this,
        &HomeMainWindowProxy::screenResized
    );
}


void HomeMainWindowProxy::displayTabBarContextMenu(const QPoint& location, unsigned tabIndex) {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    ViewWidget* view   = window->viewWidget(tabIndex);

    window->setCurrentView(tabIndex);

    QMenu menu;

    QAction* closeTabAction = menu.addAction(tr("&Close Tab"));
    QAction* closeFileAction = menu.addAction(tr("C&lose File"));

    menu.addSeparator();

    QAction* openInNewTabAction = menu.addAction(tr("Open In New &Tab"));
    QAction* openInNewWindowAction = menu.addAction(tr("Open In New &Window"));

    menu.addSeparator();

    QAction* saveAction = menu.addAction(tr("&Save"));
    QAction* saveAsAction = menu.addAction(tr("Save &As"));

    saveAction->setEnabled(view->isModified());

    QAction* selectedAction = menu.exec(location);

    if (selectedAction == closeTabAction) {
        closeTab(tabIndex);
    } else if (selectedAction == closeFileAction) {
        closeFileFromTab(tabIndex);
    } else if (selectedAction == openInNewTabAction) {
        openInNewTab(tabIndex);
    } else if (selectedAction == openInNewWindowAction) {
        openInNewWindow(tabIndex);
    } else if (selectedAction == saveAction) {
        saveFromTab(tabIndex);
    } else if (selectedAction == saveAsAction) {
        saveAsFromTab(tabIndex);
    }
}


#if (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

    void HomeMainWindowProxy::windowStateChanged(bool, bool nowMaximized, bool nowFullScreen) {
        MainWindow* window           = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
        QAction*    fullScreenAction = window->action("view_enter_exit_full_screen");

        if (nowMaximized || nowFullScreen) {
            fullScreenAction->setText(tr("Exit Full Screen"));
            fullScreenAction->setToolTip(tr("Leave full screen mode"));
        } else {
            fullScreenAction->setText(tr("Enter Full Screen"));
            fullScreenAction->setToolTip(tr("Make the window full screen"));
        }
    }

#endif


void HomeMainWindowProxy::clipboardUpdated() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    window->action("edit_paste")->setEnabled(true);
}


void HomeMainWindowProxy::backgroundOperationAborted() {
    if (remainingDocumentsToAbortOperation > 0) {
        --remainingDocumentsToAbortOperation;
    }

    if (remainingDocumentsToAbortOperation == 0) {
        switch (actionAfterAborted) {
            case ActionAfterAborted::NONE: {
                break;
            }

            case ActionAfterAborted::QUIT: {
                MainWindow::closeAllMainWindows(true);
                break;
            }

            case ActionAfterAborted::CLOSE_TAB: {
                if (indexOfTabToClose != static_cast<unsigned>(-1)) {
                    dynamic_cast<MainWindow*>(window())->removeView(indexOfTabToClose);
                    indexOfTabToClose = static_cast<unsigned>(-1);

                    QList<Document*> activeDocuments = MainWindow::visibleDocuments();
                    Document::purgeUnreferenced(activeDocuments);
                }

                break;
            }

            case ActionAfterAborted::CLOSE_WINDOW: {
                window()->close();
                break;
            }

            case ActionAfterAborted::CLOSE_FILE: {
                if (documentToClose != Q_NULLPTR) {
                    closeDocument(documentToClose);
                    documentToClose = Q_NULLPTR;
                }

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        actionAfterAborted = ActionAfterAborted::NONE;

        if (shutdownStatusDialog != Q_NULLPTR) {
            shutdownStatusDialog->deleteLater();
            shutdownStatusDialog = Q_NULLPTR;
        }
    }
}


void HomeMainWindowProxy::screenResized() {
    windowConfigured();
}


void HomeMainWindowProxy::quit() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());

    QList<Document*>                 documents = Document::documents();
    QList<Document*>::const_iterator it        = documents.constBegin();
    QList<Document*>::const_iterator end       = documents.constEnd();
    while (it != end && (*it)->isPristine()) {
        ++it;
    }

    bool okToExit;
    if (it != end) {
        QMessageBox::StandardButton result = QMessageBox::warning(
            window,
            tr("Warning"),
            tr("There are unsaved changes.\nDo you want really want to quit %1 ?").arg(Application::applicationName()),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        okToExit = (result == QMessageBox::Yes);
    } else {
        okToExit = true;
    }

    if (okToExit) {
        actionAfterAborted = ActionAfterAborted::QUIT;
        if (shutdownRenderingTranslationOrExecution(Document::documents()) == 0) {
            MainWindow::closeAllMainWindows(true); // Windows destroy themselves automatically on close.
        }
    } else {
        bool success = window->displayModifiedDocument();

        if (!success) {
            MainWindow::Iterator it  = MainWindow::begin();
            MainWindow::Iterator end = MainWindow::end();

            while (it != end && !(*it)->displayModifiedDocument()) {
                ++it;
            }

            Q_ASSERT(it != end);
        }
    }
}


void HomeMainWindowProxy::fileNew() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());

    unsigned index = 0;
    unsigned numberViews = window->numberViews();
    while (index < numberViews && window->viewWidget(index)->isNotReplaceable()) {
        ++index;
    }

    if (index < numberViews) {
        window->setCurrentView(index);
    } else {
        window->addView();
    }
}


void HomeMainWindowProxy::fileOpenDialog() {
    DocumentFileDialog* fileDialog = Application::documentFileDialog();
    fileDialog->configureForFileOpen();

    bool accepted = fileDialog->exec();
    if (accepted) {
        Q_ASSERT(fileDialog->selectedFiles().count() == 1);
        openFile(fileDialog->selectedFiles()[0]);
    }
}


void HomeMainWindowProxy::fileOpenRecent() {
    QAction* recentFilesAction = window()->action("file_open_recent");
    openFile(recentFilesAction->data().toString());
}


void HomeMainWindowProxy::fileSave() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    saveFromTab(window->currentViewIndex());
}


void HomeMainWindowProxy::fileSaveAs() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    saveAsFromTab(window->currentViewIndex());
}


void HomeMainWindowProxy::fileClose() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    closeFileFromTab(window->currentViewIndex());
}


void HomeMainWindowProxy::fileCloseTab() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    closeTab(window->currentViewIndex());
}


#if (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

    void HomeMainWindowProxy::viewEnterExitFullScreen() {
        MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
        if (window->isFullScreen()) {
            window->showNormal();
        } else {
            window->showFullScreen();
        }
    }

#endif


void HomeMainWindowProxy::resetDocksToDefault() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    window->resetDocksToDefaultState();
}


void HomeMainWindowProxy::windowNewWindow() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    openInNewWindow(window->currentViewIndex());
}


void HomeMainWindowProxy::windowCloseWindow() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    window->close();
}


void HomeMainWindowProxy::windowNewTab() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    openInNewTab(window->currentViewIndex());
}


void HomeMainWindowProxy::windowCloseTab() {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    closeTab(window->currentViewIndex());
}


void HomeMainWindowProxy::appicationPreferences() {
    ApplicationPreferencesDialog applicationPreferencesDialog(window());
    applicationPreferencesDialog.exec();
}


void HomeMainWindowProxy::help() {
    bool success = DesktopServices::openHelp();
    if (!success) {
        QMessageBox::warning(
            window(),
            tr("Unable to provide help"),
            tr("Could not launch web browser.")
        );
    }
}


void HomeMainWindowProxy::helpGettingStarted() {
    bool success = DesktopServices::openGettingStarted();
    if (!success) {
        QMessageBox::warning(
            window(),
            tr("Unable to provide help"),
            tr("Could not launch web browser.")
        );
    }
}


void HomeMainWindowProxy::helpOpenExamples() {
    QFileDialog fileDialog(
        window(),
        tr("Select an example to be opened"),
        Ld::Environment::examplesDirectory()
    );

    QStringList filters;
    filters << tr("%1 documents (*.%2)").arg(Application::applicationName()).arg(FILE_EXTENSION)
            << tr("All files (*)");
    fileDialog.setNameFilters(filters);
    fileDialog.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setWindowTitle(tr("Load Example File"));

    bool accepted = fileDialog.exec();
    if (accepted) {
        Q_ASSERT(fileDialog.selectedFiles().count() == 1);
        openFile(fileDialog.selectedFiles()[0]);
    }
}


void HomeMainWindowProxy::helpSendFeedback() {
    unsigned long long customerId = Application::customerId();

    bool success = DesktopServices::sendFeedback(customerId);
    if (!success) {
        QMessageBox::warning(
            window(),
            tr("Unable to provide help"),
            tr("Could not launch web browser.")
        );
    }
}


void HomeMainWindowProxy::helpAbout() {
    AboutDialog aboutDialog(window());
    aboutDialog.exec();
}


bool HomeMainWindowProxy::openFile(const QString& fileName) {
    bool        success;
    MainWindow* window   = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());

    QFileInfo fileInformation(fileName);
    Document*   document = Document::document(fileInformation);

    if (document == Q_NULLPTR) {
        QSharedPointer<Ld::RootElement> rootElement = Ld::Element::create(Ld::RootElement::elementName)
                                                      .dynamicCast<Ld::RootElement>();
        Ld::RootElement::registerRootElement(rootElement);
        document = dynamic_cast<Document*>(rootElement->visual());

        success = document->loadDocument(fileName);
        if (!success) {
            QMessageBox::information(
                window,
                tr("Error"),
                tr("Could not load %1\n%2").arg(fileName, document->lastError())
            );

            Ld::RootElement::unregisterRootElement(rootElement);
            document = Q_NULLPTR;
        }
    } else {
        success = true;
    }

    if (document != Q_NULLPTR) {
        unsigned numberViews = window->numberViews();
        unsigned index       = 0;

        while (index < numberViews && window->viewWidget(index)->isNotReplaceable()) {
            ++index;
        }

        if (index < numberViews) {
            window->viewWidget(index)->setDocument(document);
            window->setCurrentView(index);

            QList<Document*> visibleDocuments = MainWindow::visibleDocuments();
            Document::purgeUnreferenced(visibleDocuments);
        } else {
            window->addView(document);
        }

        window->currentView()->loaded(document);
        Application::recentFilesData()->update(document->filename());
    }

    return success;
}


bool HomeMainWindowProxy::openInNewTab(unsigned index) {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    ViewWidget* view   = window->viewWidget(index);

    Q_ASSERT(view != Q_NULLPTR);

    Document* document = view->document();
    Q_ASSERT(document != Q_NULLPTR);

    window->addView(document);

    return true;
}


bool HomeMainWindowProxy::openInNewWindow(unsigned index) {
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    ViewWidget* view   = window->viewWidget(index);

    Q_ASSERT(view != Q_NULLPTR);

    Document* document = view->document();
    Q_ASSERT(document != Q_NULLPTR);

    MainWindow* newMainWindow = new MainWindow(document);
    newMainWindow->show();

    return true;
}


bool HomeMainWindowProxy::saveFromTab(unsigned index) {
    bool        success;
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    ViewWidget* view   = window->viewWidget(index);
    Q_ASSERT(view != Q_NULLPTR);

    Document* document = view->document();
    Q_ASSERT(document != Q_NULLPTR);

    if (document->filename().isEmpty()) {
        success = saveAsFromTab(index);
    } else if (checkForUnsavedNewImports(document)) {
        view->aboutToSave(document);
        success = document->saveDocument();
        if (!success) {
            QMessageBox::warning(window, tr("Warning"), tr("Unable to save file.\n%1").arg(document->lastError()));
        } else {
            checkForUnsavedChangedImports(document);
        }
    } else {
        success = false;
    }

    return success;
}


bool HomeMainWindowProxy::saveAsFromTab(unsigned index) {
    bool        success;
    MainWindow* window = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    ViewWidget* view   = window->viewWidget(index);
    Q_ASSERT(view != Q_NULLPTR);

    Document* document = view->document();
    Q_ASSERT(document != Q_NULLPTR);

    DocumentFileDialog* fileDialog = Application::documentFileDialog();
    fileDialog->configureForFileSave();

    success = fileDialog->exec();

    if (success) {
        Q_ASSERT(fileDialog->selectedFiles().count() == 1);
        QString fileName = fileDialog->selectedFiles()[0];

        if (checkForUnsavedNewImports(document)) {
            view->aboutToSave(document);
            success = document->saveDocument(fileName);
            if (!success) {
                QMessageBox::warning(window, tr("Warning"), tr("Unable to save file.\n%1").arg(document->lastError()));
            } else {
                Application::recentFilesData()->update(document->filename());
                checkForUnsavedChangedImports(document);
            }
        }
    }

    return success;
}


bool HomeMainWindowProxy::closeTab(unsigned index) {
    bool        success   = false;
    MainWindow* window    = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    ViewWidget* view      = window->viewWidget(index);
    Document*   document  = view->document();

    unsigned numberEditors = document->countEditors();
    if (numberEditors == 1) {
        if (document->isModified()) {
            QMessageBox::StandardButton response = QMessageBox::question(
                window,
                tr("Question"),
                tr(
                    "Document contains local modifications.\n",
                    "Do you want to save the document, discard your modifications, or cancel the close operation ?"
                ),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                QMessageBox::Cancel
            );

            if (response == QMessageBox::Save) {
                success = saveFromTab(index);
                if (success) {
                    window->removeView(index);

                    QList<Document*> activeDocuments = MainWindow::visibleDocuments();
                    Document::purgeUnreferenced(activeDocuments);

                    success = true;
                }
            } else if (response == QMessageBox::Discard) {
                actionAfterAborted = ActionAfterAborted::CLOSE_TAB;
                indexOfTabToClose           = index;
                if (shutdownRenderingTranslationOrExecution(QList<Document*>() << document) == 0) {
                    window->removeView(index);

                    QList<Document*> activeDocuments = MainWindow::visibleDocuments();
                    Document::purgeUnreferenced(activeDocuments);
                }

                success = true;
            } else {
                Q_ASSERT(response == QMessageBox::Cancel);
            }
        } else {
            BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
            if (buildExecuteStateMachine->isActive()) {
                Document* builtDocument = dynamic_cast<Document*>(buildExecuteStateMachine->rootElement()->visual());
                if (builtDocument == document) {
                    actionAfterAborted = ActionAfterAborted::CLOSE_TAB;
                    indexOfTabToClose           = index;
                    if (shutdownRenderingTranslationOrExecution(QList<Document*>() << document) == 0) {
                        window->removeView(index);

                        QList<Document*> activeDocuments = MainWindow::visibleDocuments();
                        Document::purgeUnreferenced(activeDocuments);
                    }

                    success = true;
                } else {

                }
            }
        }
    } else {
        window->removeView(index);
        success = true;
    }

    return success;
}


bool HomeMainWindowProxy::closeFileFromTab(unsigned index) {
    MainWindow* window   = dynamic_cast<MainWindow*>(HomeMainWindowProxy::window());
    ViewWidget* view     = window->viewWidget(index);
    Document*   document = view->document();
    bool        success;

    if (document->isModified()) {
        QMessageBox::StandardButton response = QMessageBox::question(
            window,
            tr("Question"),
            tr(
                "Document contains local modifications.\n",
                "Do you want to save the document, discard your modifications, or cancel the close operation ?"
            ),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Cancel
        );

        if (response == QMessageBox::Save) {
            success = saveFromTab(index);
        } else if (response == QMessageBox::Discard) {
            success = true;
        } else {
            success = false;
        }
    } else {
        success = true;
    }

    if (success) {
        documentToClose    = document;
        actionAfterAborted = ActionAfterAborted::CLOSE_FILE;
        if (shutdownRenderingTranslationOrExecution(QList<Document*>() << document) == 0) {
            closeDocument(document);
        }
    }

    return success;
}


void HomeMainWindowProxy::closeDocument(Document* document) {
    QSet<QString> allRequiredDependencies;

    for (MainWindow::Iterator it=MainWindow::begin(),end=MainWindow::end() ; it!=end ; ++it) {
        MainWindow* mainWindow = *it;
        unsigned    numberViews = mainWindow->numberViews();
        unsigned    index       = 0;
        while (index < numberViews) {
            ViewWidget* view         = mainWindow->viewWidget(index);
            Document*   viewDocument = view->document();
            if (viewDocument == document) {
                mainWindow->removeView(index);
                --numberViews;
            } else {
                QSharedPointer<Ld::RootElement> rootElement = viewDocument->element();
                QString                         identifier  = rootElement->identifier();

                allRequiredDependencies.insert(identifier);
                Ld::RootElement::RootElementList allDependencies = rootElement->allDependencies();
                for (  Ld::RootElement::RootElementList::const_iterator it  = allDependencies.constBegin(),
                                                                        end = allDependencies.constEnd()
                     ; it != end
                     ; ++it
                    ) {
                    QSharedPointer<Ld::RootElement> dependency = *it;
                    QString                         identifier = dependency->identifier();
                    allRequiredDependencies.insert(identifier);
                }

                ++index;
            }
        }
    }

    QList<Document*> activeDocuments = MainWindow::visibleDocuments();
    Document::purgeUnreferenced(activeDocuments);
}


unsigned HomeMainWindowProxy::shutdownRenderingTranslationOrExecution(const QList<Document*> documents) {
    remainingDocumentsToAbortOperation = 0;

    BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
    Document*                 documentBeingBuilt       = Q_NULLPTR;

    if (buildExecuteStateMachine->isActive()) {
        QSharedPointer<Ld::RootElement> rootElement = buildExecuteStateMachine->rootElement();
        if (rootElement != Q_NULLPTR) {
            documentBeingBuilt = dynamic_cast<Document*>(rootElement->visual());
        }
    }

    for (QList<Document*>::const_iterator it=documents.constBegin(),end=documents.constEnd() ; it != end ; ++it) {
        Document* document = *it;
        if (!document->isDisplayCoherent()) {
            ++remainingDocumentsToAbortOperation;

            connect(
                document,
                &Document::presentationUpdatesCompleted,
                this,
                &HomeMainWindowProxy::backgroundOperationAborted,
                Qt::QueuedConnection
            );

            document->abortRepositioning();
        }

        if (document == documentBeingBuilt) {
            ++remainingDocumentsToAbortOperation;

            connect(
                buildExecuteStateMachine,
                &BuildExecuteStateMachine::nowShutdown,
                this,
                &HomeMainWindowProxy::backgroundOperationAborted,
                Qt::QueuedConnection
            );

            buildExecuteStateMachine->shutdownNow();
            enableShutdownStatusDialog();
        }
    }

    return remainingDocumentsToAbortOperation;
}


void HomeMainWindowProxy::enableShutdownStatusDialog() {
    if (shutdownStatusDialog == Q_NULLPTR) {
        shutdownStatusDialog = new QDialog(window(), Qt::Dialog | Qt::WindowStaysOnTopHint);
        shutdownStatusDialog->setWindowModality(Qt::WindowModal);

        QVBoxLayout* shutdownStatusDialogLayout = new QVBoxLayout;
        shutdownStatusDialog->setLayout(shutdownStatusDialogLayout);
        shutdownStatusDialogLayout->addWidget(new QLabel(tr("Waiting for background operations to terminate...")));

        QProgressBar* progressBar = new QProgressBar;
        progressBar->setRange(0, 0);
        progressBar->setValue(0);
        shutdownStatusDialogLayout->addWidget(progressBar);

        QTimer* displayTimer = new QTimer(shutdownStatusDialog);
        displayTimer->setSingleShot(true);

        connect(displayTimer, &QTimer::timeout, shutdownStatusDialog, &QDialog::show);
        displayTimer->start(1000);
    }
}


QString HomeMainWindowProxy::allNewImports(Document* document) {
    Ld::RootElement::RootElementList allDependencies = document->element()->allDependencies();
    QString                          result;

    for (  Ld::RootElement::RootElementList::const_iterator dependencyIt = allDependencies.constBegin(),
                                                            dependencyEndIt = allDependencies.constEnd()
         ; dependencyIt != dependencyEndIt
         ; ++dependencyIt
        ) {
        QSharedPointer<Ld::RootElement> dependency = *dependencyIt;
        if (dependency->isNew()) {
            result += tr("<li>%1</li>").arg(dependency->shortformName());
        }
    }

    return result;
}


QString HomeMainWindowProxy::allUnsavedImports(Document* document) {
    Ld::RootElement::RootElementList allDependencies = document->element()->allDependencies();
    QString                          result;

    for (  Ld::RootElement::RootElementList::const_iterator dependencyIt = allDependencies.constBegin(),
                                                            dependencyEndIt = allDependencies.constEnd()
         ; dependencyIt != dependencyEndIt
         ; ++dependencyIt
        ) {
        QSharedPointer<Ld::RootElement> dependency = *dependencyIt;
        if (dependency->isModified()) {
            result += tr("<li>%1</li>").arg(dependency->shortformName());
        }
    }

    return result;
}


bool HomeMainWindowProxy::checkForUnsavedNewImports(Document* document) const {
    bool    result;
    QString newImports = allNewImports(document);

    if (!newImports.isEmpty()) {
        QString message = tr(
            "<p>You have the following new imports referenced by this document.</p>"
            "<ul>%1</ul>"
            "<p>Either save these documents to media or remove them as imports before saving this document.</p>"
        ).arg(newImports);

        QMessageBox::information(window(), tr("Unsaved Dependencies"), message, QMessageBox::StandardButton::Ok);
        result = false;
    } else {
        result = true;
    }

    return result;
}


bool HomeMainWindowProxy::checkForUnsavedChangedImports(Document* document) const {
    bool    result;
    QString changedImports = allUnsavedImports(document);
    if (!changedImports.isEmpty()) {
        QString message = tr(
            "<p>Your document has been saved,</p>"
            "<p>You have the following unsaved imports referenced by this document:</p>"
            "<ul>%1</ul>"
            "<p>Be sure to save any changes to the documents as well.</p>"
        ).arg(changedImports);

        QMessageBox::information(
            window(),
            tr("Unsaved Dependencies"),
            message,
            QMessageBox::StandardButton::Ok
        );

        result = false;
    } else {
        result = true;
    }

    return result;
}
