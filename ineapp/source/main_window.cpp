/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MainWindow class.
***********************************************************************************************************************/

#include <QWidget>
#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QFont>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QList>
#include <QIcon>
#include <QFileInfo>
#include <QBitArray>
#include <QMainWindow>

#include <eqt_recent_files_data.h>
#include <eqt_recent_files_popup_menu.h>
#include <eqt_tab_widget.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_tool_button.h>

#include <ld_capabilities.h>

#include "desktop_services.h"
#include "document_file_dialog.h"
#include "about_dialog.h"
#include "application_preferences_dialog.h"
#include "application.h"
#include "view_widget.h"
#include "command_popup_dialog.h"
#include "editor.h"
#include "cursor.h"
#include "document.h"
#include "main_window.h"

QList<MainWindow*> MainWindow::mainWindows;
bool               MainWindow::promptOnMainWindowClose = true;

QList<MainWindow*> MainWindow::allWindows() {
    return QList<MainWindow*>(mainWindows); // Let's be really explicit about the copy.
}


MainWindow::Iterator MainWindow::begin() {
    return mainWindows.constBegin();
}


MainWindow::Iterator MainWindow::end() {
    return mainWindows.constEnd();
}


void MainWindow::closeAllMainWindows(bool noPrompt) {
    promptOnMainWindowClose = !noPrompt;

    QList<MainWindow*> runningList = mainWindows;
    for (QList<MainWindow*>::const_iterator it=runningList.constBegin(),end=runningList.constEnd() ; it!=end ; ++it) {
        MainWindow* window = *it;
        window->close();
    }

    promptOnMainWindowClose = true;
}


void MainWindow::deleteAllMainWindows() {
    while (!mainWindows.isEmpty()) {
        MainWindow* mainWindow = mainWindows.last();
        delete mainWindow; // Deleting the main window removes it from the list.
    }
}


MainWindow::MainWindow() {
    configureWindow();
    runBuilders();
    addView();
}


MainWindow::MainWindow(Document* document) {
    configureWindow();
    runBuilders();
    addView(document);
}


MainWindow::~MainWindow() {
    unsigned numberRemoved = static_cast<unsigned>(mainWindows.removeAll(this));
    Q_ASSERT(numberRemoved == 1);
}


unsigned MainWindow::windowIndex() const {
    int index = mainWindows.indexOf(const_cast<MainWindow*>(this));
    Q_ASSERT(index >= 0);

    return static_cast<unsigned>(index);
}


void MainWindow::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    restoreGeometry(settings->value("geometry").toByteArray());
    restoreState(settings->value("windowState").toByteArray());

    settings->endGroup();

    ProgrammaticMainWindow::loadSettings(groupName);
}


void MainWindow::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());

    settings->endGroup();

    ProgrammaticMainWindow::saveSettings(groupName);

    QList<ViewWidget*> viewWidgets       = MainWindow::viewWidgets();
    unsigned           numberViewWidgets = static_cast<unsigned>(viewWidgets.size());
    for (unsigned viewIndex=0 ; viewIndex<numberViewWidgets ; ++viewIndex) {
        ViewWidget* viewWidget = viewWidgets.at(viewIndex);
        viewWidget->saveSettings(QString("View%1").arg(viewIndex));
    }
}


void MainWindow::setCommands(const MainWindow::ActionsByCommand& newCommandMap) {
    currentCommandPopupDialog->setCommands(newCommandMap);
}


const MainWindow::ActionsByCommand& MainWindow::commands() const {
    return currentCommandPopupDialog->commands();
}


bool MainWindow::setCommand(const QString& command, QAction* action, bool overwriteExisting) {
    return currentCommandPopupDialog->setCommand(command, action, overwriteExisting);
}


QList<ViewWidget*> MainWindow::viewWidgets() const {
    unsigned           count = numberViews();
    QList<ViewWidget*> views;

    for (unsigned i=0 ; i<count ; ++i) {
        views.append(viewWidget(i));
    }

    return views;
}


unsigned MainWindow::numberViews() const {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");
    return tabWidget->count();
}


unsigned MainWindow::currentViewIndex() const {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");
    return tabWidget->currentIndex();
}


ViewWidget* MainWindow::currentView() const {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");
    return dynamic_cast<ViewWidget*>(tabWidget->currentWidget());
}


void MainWindow::setCurrentView(unsigned index) {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");
    tabWidget->setCurrentIndex(index);
    currentTabChanged(index);
}


bool MainWindow::setCurrentView(ViewWidget* viewWidget) {
    int index = tabIndex(viewWidget);
    if (index >= 0) {
        setCurrentView(index);
        return true;
    } else {
        return false;
    }
}


int MainWindow::tabIndex(ViewWidget* view) {
    unsigned index = 0;

    while (index < numberViews() && viewWidget(index) != view) {
        ++index;
    }

    if (index >= numberViews()) {
        index = -1;
    }

    return index;
}


ViewWidget* MainWindow::viewWidget(unsigned index) const {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");
    return dynamic_cast<ViewWidget*>(tabWidget->widget(index));
}


unsigned MainWindow::addView(Document* document) {
    EQt::TabWidget*  tabWidget = widget<EQt::TabWidget>("tab_widget");
    ViewWidget*      view      =   document == Q_NULLPTR
                                 ? new ViewWidget(this, tabWidget)
                                 : new ViewWidget(document, this, tabWidget);

    // NOTE: We make several connections to slots in the main window in the ViewWidget::bind method.

    unsigned index = static_cast<unsigned>(tabWidget->addTab(view, view->shortformName()));

    view->loadSettings(QString("View%1").arg(index));

    if (view->isModified()) {
        setTabIcon(index, Application::icon("file_modified"));
    } else {
        setTabIcon(index, QIcon());
    }

    setCurrentView(index); // triggers call to currentTabChanged(index);

    return index;
}


void MainWindow::removeView(unsigned index) {
    EQt::TabWidget*  tabWidget  = widget<EQt::TabWidget>("tab_widget");
    ViewWidget*      viewWidget = dynamic_cast<ViewWidget*>(tabWidget->widget(index));

    viewWidget->saveSettings(QString("View%1").arg(index));

    tabWidget->removeTab(index);

    if (activeView == viewWidget) {
        activeView->unbindDynamic();
        activeView = dynamic_cast<ViewWidget*>(tabWidget->currentWidget());
        if (activeView != Q_NULLPTR) {
            activeView->bindDynamic();
        }
    }

    // When the underlying Editor instance is deleted, the document that the editor is referencing may also be deleted
    // if there are no other Editors using it and no references to it.

    delete viewWidget;

    if (tabWidget->count() == 0) {
        addView();
    }
}


void MainWindow::setTabText(unsigned index, const QString& text) {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");
    tabWidget->setTabText(index, text);
}


void MainWindow::setTabIcon(unsigned index, const QIcon& icon) {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");
    tabWidget->setTabIcon(index, icon);
}


void MainWindow::clearTabIcon(unsigned index) {
    setTabIcon(index, QIcon());
}


void MainWindow::requirementsChanged(const Ld::Capabilities& newParentRequirements) {
    if (newParentRequirements != currentParentRequirements) {
        currentParentRequirements = newParentRequirements;
        setActionState(newParentRequirements);
    }
}


void MainWindow::displayCommandPopupDialog(const QPoint& globalPosition, const QFont& fontAtLocation) {
    currentCommandPopupDialog->setFont(fontAtLocation);
    currentCommandPopupDialog->move(globalPosition);

    currentCommandPopupDialog->execute();
}


void MainWindow::bind() {
    EQt::TabWidget* tabWidget = widget<EQt::TabWidget>("tab_widget");

    connect(tabWidget, &EQt::TabWidget::currentChanged, this, &MainWindow::currentTabChanged);
    connect(tabWidget, &EQt::TabWidget::customContextMenuRequested, this, &MainWindow::tabBarContextMenuRequested);

    ProgrammaticMainWindow::bind();
}


void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent* stateChangeEvent = dynamic_cast<QWindowStateChangeEvent*>(event);
        Q_ASSERT(stateChangeEvent != Q_NULLPTR);

        Qt::WindowStates oldState = stateChangeEvent->oldState();
        Qt::WindowStates newState = windowState();

        Qt::WindowStates changed  = oldState ^ newState;
        if ((changed & Qt::WindowFullScreen) || (changed & Qt::WindowMaximized) || (changed & Qt::WindowMinimized)) {
            bool nowMinimized  = newState & Qt::WindowMinimized;
            bool nowMaximized  = newState & Qt::WindowMaximized;
            bool nowFullScreen = newState & Qt::WindowFullScreen;

            emit windowStateChanged(nowMinimized, nowMaximized, nowFullScreen);
        }
    }

    ProgrammaticMainWindow::changeEvent(event);
}


void MainWindow::closeEvent(QCloseEvent* event) {
    bool closeWindow;

    if (promptOnMainWindowClose) {
        closeWindow = okToClose();
    } else {
        closeWindow = true;
    }

    if (closeWindow) {
        closing();
        event->accept();
    } else {
        event->ignore();
    }
}


void MainWindow::currentTabChanged(int index) {
    if (numberViews() > 0) {
        ViewWidget* newActiveView = viewWidget(index);

        if (newActiveView != activeView) {
            if (activeView != Q_NULLPTR) {
                activeView->viewInactive();
                activeView->unbindDynamic();
            }

            activeView = newActiveView;

            activeView->bindDynamic();
            activeView->viewActive();
        }
    }
}


void MainWindow::screenSettingsChanged(
        const QSize& /* newPhysicalDpi */,
        const QSize& /* newLogicalDpi */,
        unsigned     newToolButtonHeightPixels
    ) {
    setMinimumSize(
        newToolButtonHeightPixels * minimumWidthToolButtons,
        newToolButtonHeightPixels * minimumHeightToolButtons
    );
}


bool MainWindow::displayModifiedDocument() {
    int count = numberViews();
    int index = 0;

    while (index < count && viewWidget(index)->isNotModified()) {
        ++index;
    }

    if (index < count) {
        setCurrentView(index);
        return true;
    } else {
        return false;
    }
}


QList<Document*> MainWindow::visibleDocuments() {
    QSet<Document*> allVisibleDocuments;

    for (MainWindow::Iterator it=MainWindow::begin(),end=MainWindow::end() ; it!=end ; ++it) {
        MainWindow* mainWindow = *it;
        unsigned    numberViews = mainWindow->numberViews();
        for (unsigned viewIndex=0 ; viewIndex<numberViews ; ++viewIndex) {
            ViewWidget* view         = mainWindow->viewWidget(viewIndex);
            Document*   viewDocument = view->document();
            allVisibleDocuments.insert(viewDocument);
        }
    }

    return allVisibleDocuments.values();
}


void MainWindow::configureWindow() {
    // Main window instances do not have a parent and thus nobody to delete them when they're closed.
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowState(Qt::WindowMaximized | Qt::WindowActive);

    mainWindows.append(this);

    screenSettingsChanged(Application::physicalDpi(), Application::logicalDpi(), EQt::ToolButton::globalSize());
    connect(Application::instance(), &Application::screenSettingsChanged, this, &MainWindow::screenSettingsChanged);

    buildCentralWidget();

    currentCommandPopupDialog = new CommandPopupDialog(this);

    currentParentRequirements = Ld::Capabilities();
}


void MainWindow::buildCentralWidget() {
    EQt::TabWidget* tabWidget = new EQt::TabWidget(this);
    setCentralWidget(tabWidget);

    tabWidget->setMovable(true);
    tabWidget->setTabBarAutoHide(true);
    tabWidget->setTabPosition(QTabWidget::South);
    tabWidget->setTabShape(QTabWidget::Rounded);
    tabWidget->setTabsClosable(false);
    tabWidget->setDocumentMode(true);

    registerWidget(tabWidget, "tab_widget");

    activeView = Q_NULLPTR;
}
