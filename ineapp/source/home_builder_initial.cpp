/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HomeBuilderInitial class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QStringList>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeySequence>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_recent_files_popup_menu.h>
#include <eqt_search_line_edit.h>
#include <eqt_tool_button.h>
#include <eqt_stacking_layout.h>
#include <eqt_zoom_combo_box.h>

#include "application.h"
#include "main_window.h"
#include "home_main_window_proxy.h"
#include "home_view_proxy.h"
#include "home_builder_initial.h"

const char HomeBuilderInitial::builderName[]    = "HomeBuilderInitial";
const char HomeBuilderInitial::homeDockName[]   = "home";
const char HomeBuilderInitial::insertDockName[] = "insert";

HomeBuilderInitial::HomeBuilderInitial(QObject* parent):EQt::Builder(parent) {}


HomeBuilderInitial::~HomeBuilderInitial() {}


unsigned HomeBuilderInitial::priority() const {
    return builderPriority;
}


const char* HomeBuilderInitial::builderIdentifier() const {
    return builderName;
}


const char* HomeBuilderInitial::plugInName() const {
    return "";
}


void HomeBuilderInitial::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenu(tr("&File"));
    window->addMenu(tr("&Edit"));
    window->addMenu(tr("&View"));
    window->addMenu(tr("&Insert"));
    window->addMenu(tr("F&ormat"));

    /* File menu */

    window->addMenuAction(EQt::Application::icon("file_new"), tr("&File | &New"), "file_new");
    window->setToolTip("file_new", tr("Creates a new program file"));
    window->setShortcut("file_new", QKeySequence::New);
    window->setWhatsThis("file_new", tr("Use this option to create a new, empty, file."));

    window->addMenuAction(EQt::Application::icon("file_open"), tr("&File | &Open..."), "file_open_dialog");
    window->setToolTip("file_open_dialog", tr("Opens an existing program file"));
    window->setShortcut("file_open_dialog", QKeySequence::Open);
    window->setWhatsThis("file_open_dialog", tr("Use this option to open an existing file."));

    QAction* recentDocumentsAction = new QAction(window);
    window->addAction(recentDocumentsAction, "file_open_recent");
    EQt::RecentFilesPopupMenu* recentFilesPopupMenu = new EQt::RecentFilesPopupMenu(
        tr("&File | Open &Recent"),
        EQt::ProgrammaticApplication::recentFilesData(),
        recentDocumentsAction,
        window
    );
    window->addMenu(tr("&File | Open &Recent"), recentFilesPopupMenu);
    window->registerWidget(recentFilesPopupMenu, "recent_files_popup_menu");

    window->addSeparator(tr("&File"));

    window->addMenuAction(
        tr("&File | &Close File"),
        "file_close"
    );
    window->setToolTip("file_close", tr("Closes the current program file"));
    window->setShortcut("file_close", QKeySequence::Close);
    window->setWhatsThis(
        "file_close",
        tr("Use this option to close the current file.  You may be prompted to save your changes.")
    );

    window->addMenuAction(tr("&File | Close &Tab"), "file_close_tab");
    window->setToolTip("file_close_tab", tr("Closes the current tab"));
    window->setWhatsThis(
        "file_close_tab",
        tr(
            "Use this option to close the curernt tab.  You may be prompted to save your work if closing this tab "
            "could cause unsaved changed to be lost."
        )
    );

    window->addMenuAction(EQt::Application::icon("file_save"), tr("&File | &Save"), "file_save");
    window->setToolTip("file_save", tr("Saves the current program"));
    window->setShortcut("file_save", QKeySequence::Save);
    window->setWhatsThis("file_save", tr("Use this option to save the current file under it's current name."));

    window->addMenuAction(tr("&File | Save &As..."), "file_save_as");
    window->setToolTip("file_save_as", tr("Saves the current program under a new name"));
    window->setShortcut("file_save_as", QKeySequence::SaveAs);
    window->setWhatsThis("file_save_as", tr("Use this option to save the current file under a new name."));

    window->addSeparator(tr("&File"));

    window->addMenuAction(
        tr("&File | &Imports..."),
        "file_imports_dialog"
    );
    window->setToolTip("file_imports_dialog", tr("Add, remove, or change program imports"));
    window->setWhatsThis(
        "file_imports_dialog",
        tr(
            "Use this option to add, remove, or change imports from other files.  You can use this to allow your "
            "program to reference functions or variables in other files.  The imported files are compiled together "
            "with this model or algorithm, in order, with this algorithm included last in the compilation.\n\n"
            "You can use this option to: keep commonly used functions or values in a central location that can be "
            "referenced by multiple models, place inputs, implementation, and or results/reporting in distinct "
            "files, or share the same model engine across many projects."
        )
    );

    window->addMenuAction(tr("&File | &Export | &PDF..."), "file_export_pdf");
    window->setToolTip("file_export_pdf", tr("Exports your program as a document in PDF format."));
    window->setWhatsThis(
        "file_export_pdf",
        tr("You can use this option to export your program as a document in PDF format.")
    );

    window->addSeparator(tr("&File"));

    window->addMenuAction(
        EQt::Application::icon("file_print"),
        tr("&File | &Print..."),
        ~Ld::Capabilities::selection,
        "file_print_dialog"
    );
    window->setToolTip("file_print_dialog", tr("Print the current program"));
    window->setShortcut("file_print_dialog", QKeySequence::Print);
    window->setWhatsThis("file_print_dialog", tr("Use this option to print the current file."));

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        window->addSeparator(tr("&File"));
        window->addMenuAction(tr("&File | &Quit"), "quit");

    #elif (defined(Q_OS_DARWIN))

        window->addMenuAction(tr("Quit"), "quit");

    #else

        #error Unknown platform.

    #endif

    window->setToolTip("quit", tr("Quit %1").arg(EQt::Application::applicationName()));
    window->setShortcut("quit", QKeySequence::Quit);
    window->setWhatsThis("quit", tr("Use this option to exit the program."));

    /* Edit menu */

    window->addMenuAction(EQt::Application::icon("edit_undo"), tr("&Edit | &Undo"), "edit_undo");
    window->setToolTip("edit_undo", tr("Undo"));
    window->setShortcut("edit_undo", QKeySequence::Undo);
    window->setWhatsThis("edit_undo", tr("Use this option to undo the last change to the current file."));

    window->addMenuAction(EQt::Application::icon("edit_redo"), tr("&Edit | &Redo"), "edit_redo");
    window->setToolTip("edit_redo", tr("Redo"));
    window->setShortcut("edit_redo", QKeySequence::Redo);
    window->setWhatsThis("edit_redo", tr("Use this option to redo the last undone change."));

    window->addSeparator(tr("&Edit"));

    window->addMenuAction(EQt::Application::icon("edit_cut"), tr("&Edit | C&ut"), "edit_cut");
    window->setToolTip("edit_cut", tr("Cut"));
    window->setShortcut("edit_cut", QKeySequence::Cut);
    window->setWhatsThis("edit_cut", tr("Use this option to cut the current selection, placing it in the clipboard."));

    window->addMenuAction(EQt::Application::icon("edit_copy"), tr("&Edit | &Copy"), "edit_copy");
    window->setToolTip("edit_copy", tr("Copy"));
    window->setShortcut("edit_copy", QKeySequence::Copy);
    window->setWhatsThis("edit_cut", tr("Use this option to copy the current selection to the clipboard."));

    window->addMenuAction(EQt::Application::icon("edit_paste"), tr("&Edit | &Paste"), "edit_paste");
    window->setToolTip("edit_paste", tr("Paste"));
    window->setShortcut("edit_paste", QKeySequence::Paste);
    window->setWhatsThis(
        "edit_paste",
        tr("Use this option to paste the contents of the clipboard into your program at the current cursor postion.")
    );

    window->addMenuAction(tr("&Edit | Select &All"), "edit_select_all");
    window->setToolTip("edit_select_all", tr("Select All"));
    window->setShortcut("edit_select_all", QKeySequence::SelectAll);
    window->setWhatsThis("edit_select_all", tr("Use this option to select the entire file contents."));

    window->addSeparator(tr("&Edit"));

    window->addMenuAction(
        EQt::Application::icon("edit_find"),
        tr("&Edit | &Find..."),
        "edit_find_dialog"
    );
    window->setToolTip("edit_find_dialog", tr("Find"));
    window->setShortcut("edit_find_dialog", QKeySequence::Find);
    window->setWhatsThis(
        "edit_find_dialog",
        tr("Use this option to find and optionally replace text in your document.")
    );

    window->addMenuAction(
        EQt::Application::icon("edit_find"),
        tr("&Edit | &Find And Replace..."),
        "edit_find_and_replace_dialog"
    );
    window->setToolTip("edit_find_and_replace_dialog", tr("Find And Replace"));
    window->setWhatsThis(
        "edit_find_and_replace_dialog",
        tr("Use this option to find and replace text in your document.")
    );

    /* View menu */

//    window->addMenuAction(tr("&View | &Zoom..."), "view_zoom_dialog");
//    window->setToolTip("view_zoom_dialog", tr("Zoom"));
//    window->setWhatsThis("view_zoom_dialog", tr("Use this option to zoom in or out."));

    window->addMenuAction(EQt::Application::icon("view_zoom_in"), tr("&View | Zoom &In"), "view_zoom_in");
    window->setToolTip("view_zoom_in", tr("Zoom In"));
    window->setShortcut("view_zoom_in", QKeySequence::ZoomIn);
    window->setWhatsThis("view_zoom_in", tr("Use this option to zoom in to the next higher magnification factor."));

    window->addMenuAction(EQt::Application::icon("view_zoom_out"), tr("&View | Zoom &Out"), "view_zoom_out");
    window->setToolTip("view_zoom_out", tr("Zoom Out"));
    window->setShortcut("view_zoom_out", QKeySequence::ZoomOut);
    window->setWhatsThis("view_zoom_out", tr("Use this option to zoom out to the next lower magnification factor."));

    window->addSeparator(tr("&View"));

    window->addMenuAction(
        EQt::Application::icon("view_margins"),
        tr("&View | &Margins"),
        "view_margins"
    )->setCheckable(true);
    window->setToolTip("view_margins", tr("Show/Hide Gray Margins"));
    window->setWhatsThis("view_margins", tr("Use this option to enable/disable highlighting page margins."));

    window->addMenuAction(
        EQt::Application::icon("view_guides"),
        tr("&View | &Guides"),
        "view_guides"
    )->setCheckable(true);
    window->setToolTip("view_guides", tr("Show/Hide Margin Guides"));
    window->setWhatsThis(
        "view_guides",
        tr("Use this option to enable/disable guide lines at the four corners of each page's active area.")
    );

    window->addSeparator(tr("&View"));
    window->addMenu(tr("&View | &Docks"));

    #if (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

        window->addMenuAction(tr("&View | Enter Full Screen"), "view_enter_exit_full_screen");
        window->setShortcut("view_enter_exit_full_screen", QKeySequence::FullScreen);
        window->setToolTip("view_enter_exit_full_screen", tr("Make the window full screen"));
        window->setWhatsThis(
            "view_enter_exit_full_screen",
            tr("Use this option to enter or leave full-screen mode.")
        );

    #endif
}


void HomeBuilderInitial::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    window->addAction(new QAction(window), "zoom_action");
    window->setToolTip("zoom_action", tr("Zoom"));
    window->setWhatsThis("zoom_action", tr("Use this combo box to select a zoom level."));

    window->addAction(new QAction(window), "edit_search");
    window->setToolTip("edit_search", tr("Locate text"));
    window->setWhatsThis("edit_search", tr("Use this field to search for text."));

    QAction* editSearchNext     = new QAction(EQt::Application::icon("edit_find"), tr("Search Next"), window);
    QAction* editSearchPrevious = new QAction(
        EQt::Application::icon("edit_find_previous"),
        tr("Search Previous"),
        window
    );

    window->addAction(editSearchNext, "edit_search_next");
    window->addAction(editSearchPrevious, "edit_search_previous");

    editSearchNext->setEnabled(false);
    editSearchPrevious->setEnabled(false);

    QAction* caseSensitiveSearchAction = new QAction(tr("Case sensitive search"), window);
    caseSensitiveSearchAction->setToolTip(tr("Enables/disables case sensitive search."));
    caseSensitiveSearchAction->setWhatsThis(
        tr("You can use this option to enable or disable case sensitive search.")
    );
    caseSensitiveSearchAction->setCheckable(true);
    window->addAction(caseSensitiveSearchAction, "edit_search_case_sensitive");

    QAction* wholeWordsOnlyAction = new QAction(tr("Whole words only"), window);
    wholeWordsOnlyAction->setToolTip(tr("Enables/disables whole word only search."));
    wholeWordsOnlyAction->setWhatsThis(
        tr(
           "You can use this option to enable or disable whole word only search.  If checked, search will ignore "
           "matches that are within words."
        )
    );
    wholeWordsOnlyAction->setCheckable(true);
    window->addAction(wholeWordsOnlyAction, "edit_search_whole_words_only");

    QAction* regularExpressionSearchAction = new QAction(tr("Use regular expressions"), window);
    regularExpressionSearchAction->setToolTip(tr("Enable/disables use of regualr expressions in search."));
    regularExpressionSearchAction->setWhatsThis(
        tr(
            "You can use this option to enable or disable support for regular experssions.  If checked, the search "
            "string will be treated as a regular expression."
        )
    );
    regularExpressionSearchAction->setCheckable(true);
    window->addAction(regularExpressionSearchAction, "edit_search_regular_expression");
}


void HomeBuilderInitial::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    window->setAnimated(true);
    window->setDockNestingEnabled(false);

    window->setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
    window->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    window->setTabPosition(Qt::RightDockWidgetArea, QTabWidget::East);
    window->setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::South);

    EQt::ProgrammaticDockWidget* homeDock = window->addDockWidget(
        homeDockName,
        tr("Home"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Area::TOP, true)
    );
    homeDock->setCloseButtonEnabled();

    QAction* homeDockToggleAction = window->addDockToggleMenuItem(
        homeDockName,
        tr("&View | &Docks"),
        "home_dock_toggle_action"
    );
    homeDockToggleAction->setToolTip(tr("Show/hide the Home dock"));
    homeDockToggleAction->setWhatsThis(
        tr(
           "You can use this menu item to show or hide the Home dock.  The Home dock includes many commonly used "
           "functions such as search, undo/redo, cut, paste, etc."
        )
    );

    QHBoxLayout* horizontalHomeLayout = homeDock->newHBoxLayout("horizontal");
    homeDock->setHorizontalLayout(horizontalHomeLayout);

    buildHomeDockHorizontalOrientation(homeDock, horizontalHomeLayout, window);

    QVBoxLayout* verticalHomeLayout = homeDock->newVBoxLayout("vertical");
    homeDock->setVerticalLayout(verticalHomeLayout);

    buildHomeDockVerticalOrientation(homeDock, verticalHomeLayout, window);

    EQt::ProgrammaticDockWidget* insertDock = window->addDockWidget(
        insertDockName,
        tr("Insert"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER, homeDockName, true)
    );
    insertDock->setCloseButtonEnabled();

    QAction* insertDockToggleAction = window->addDockToggleMenuItem(
        "insert",
        tr("&View | &Docks"),
        "insert_dock_toggle_action"
    );
    insertDockToggleAction->setToolTip(tr("Show/hide the Insert dock"));
    insertDockToggleAction->setWhatsThis(
        tr(
           "You can use this menu item to show or hide the Insert dock.  The Insert dock includes functions you can"
           "use to add widely used program elements."
        )
    );

    QHBoxLayout* horizontalInsertLayout = insertDock->newHBoxLayout("horizontal");
    insertDock->setHorizontalLayout(horizontalInsertLayout);

    buildInsertDockHorizontalOrientation(insertDock, horizontalInsertLayout, window);

    QVBoxLayout* verticalInsertLayout = insertDock->newVBoxLayout("vertical");
    insertDock->setVerticalLayout(verticalInsertLayout);

    buildInsertDockVerticalOrientation(insertDock, verticalInsertLayout, window);
}


EQt::ProgrammaticMainWindowProxy* HomeBuilderInitial::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new HomeMainWindowProxy(window);
}


EQt::ProgrammaticViewProxy* HomeBuilderInitial::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new HomeViewProxy(view, window);
}


void HomeBuilderInitial::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QMenu* rightClickMenu = new QMenu(window);
    window->registerWidget(rightClickMenu, "right_click_menu");

    QAction* exportPdf = window->action("file_export_pdf");

    QAction* editUndo      = window->action("edit_undo");
    QAction* editRedo      = window->action("edit_redo");
    QAction* editCut       = window->action("edit_cut");
    QAction* editCopy      = window->action("edit_copy");
    QAction* editPaste     = window->action("edit_paste");
    QAction* editSelectAll = window->action("edit_select_all");

    rightClickMenu->addAction(editUndo);
    rightClickMenu->addAction(editRedo);
    rightClickMenu->addSeparator();
    rightClickMenu->addAction(editCut);
    rightClickMenu->addAction(editCopy);
    rightClickMenu->addAction(editPaste);
    rightClickMenu->addAction(editSelectAll);

    bool success = mainWindow->setCommand(tr("exppdf"), exportPdf);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("undo"), editUndo);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("redo"), editRedo);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("cut"), editCut);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("cpy"), editCopy);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("pst"), editPaste);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("selall"), editSelectAll);
    Q_ASSERT(success);
}


void HomeBuilderInitial::buildHomeDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    /* File menu items */

    QGridLayout* gridLayout = dock->newGridLayout("horizontal_grid");
    horizontalLayout->addLayout(gridLayout, 1);

    EQt::ToolButton* fileNewToolButton = new EQt::ToolButton(window->action("file_new"));
    dock->registerWidget(fileNewToolButton, "file_new_tool_button");
    gridLayout->addWidget(fileNewToolButton, 0, 0);

    gridLayout->addWidget(new EQt::ToolButton(window->action("file_open_dialog")), 0, 1);
    gridLayout->addWidget(new EQt::ToolButton(window->action("file_save")), 1, 0);
    gridLayout->addWidget(new EQt::ToolButton(window->action("file_print_dialog")), 1, 1);

    gridLayout->setColumnMinimumWidth(2, EQt::ProgrammaticDockWidget::recommendedControlsSpacing);

    /* Edit menu items */

    gridLayout->addWidget(new EQt::ToolButton(window->action("edit_undo")), 0, 3);
    gridLayout->addWidget(new EQt::ToolButton(window->action("edit_redo")), 0, 4);
    gridLayout->setColumnMinimumWidth(2, 6);
    gridLayout->addWidget(new EQt::ToolButton(window->action("edit_cut")), 0, 5);
    gridLayout->addWidget(new EQt::ToolButton(window->action("edit_copy")), 0, 6);
    gridLayout->addWidget(new EQt::ToolButton(window->action("edit_paste")), 0, 7);

    EQt::SearchLineEdit* searchLineEdit = new EQt::SearchLineEdit(window->action("edit_search"), dock);
    searchLineEdit->setCaseSensitiveSearchAction(window->action("edit_search_case_sensitive"));
    searchLineEdit->setWholeWordsOnlyAction(window->action("edit_search_whole_words_only"));
    searchLineEdit->setRegularExpressionAction(window->action("edit_search_regular_expression"));
    searchLineEdit->setDynamicUpdate();

    dock->registerWidget(searchLineEdit, "horizontal_search_line_edit");
    gridLayout->addWidget(searchLineEdit, 1, 3, 1, 3, Qt::AlignJustify);

    gridLayout->addWidget(new EQt::ToolButton(window->action("edit_search_next")), 1, 6);
    gridLayout->addWidget(new EQt::ToolButton(window->action("edit_search_previous")), 1, 7);

    gridLayout->setColumnMinimumWidth(8, EQt::ProgrammaticDockWidget::recommendedControlsSpacing);

    /* View menu items */

    EQt::ZoomComboBox* zoomComboBox = new EQt::ZoomComboBox(window->action("zoom_action"), dock);
    zoomComboBox->setPageWidthStringExcluded();
    zoomComboBox->setFullPageStringExcluded();
    dock->registerWidget(zoomComboBox, "horizontal_zoom_combo_box");
    gridLayout->addWidget(zoomComboBox, 1, 9, 1, 2, Qt::AlignLeft);

    gridLayout->addWidget(new EQt::ToolButton(window->action("view_margins")), 0, 9);
    gridLayout->addWidget(new EQt::ToolButton(window->action("view_guides")), 0, 10);
    gridLayout->addWidget(new EQt::ToolButton(window->action("view_zoom_in")), 0, 11);
    gridLayout->addWidget(new EQt::ToolButton(window->action("view_zoom_out")), 1, 11);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void HomeBuilderInitial::buildHomeDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    /* File menu items */

    QHBoxLayout* fileHorizontalLayout = dock->newHBoxLayout("vertical_file");
    verticalLayout->addLayout(fileHorizontalLayout, 1);

    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_new")));
    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_open_dialog")));
    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_save")));
    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_print_dialog")));

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedControlsSpacing);

    /* Edit menu items */

    QGridLayout* editLayout = dock->newGridLayout("vertical_edit");
    verticalLayout->addLayout(editLayout);

    editLayout->addWidget(new EQt::ToolButton(window->action("edit_undo")), 0, 0);
    editLayout->addWidget(new EQt::ToolButton(window->action("edit_redo")), 0, 1);
    editLayout->setColumnMinimumWidth(2, 6);
    editLayout->addWidget(new EQt::ToolButton(window->action("edit_cut")), 0, 3);
    editLayout->addWidget(new EQt::ToolButton(window->action("edit_copy")), 0, 4);
    editLayout->addWidget(new EQt::ToolButton(window->action("edit_paste")), 0, 5);

    EQt::SearchLineEdit* searchLineEdit = new EQt::SearchLineEdit(window->action("edit_search"), dock);
    searchLineEdit->setCaseSensitiveSearchAction(window->action("edit_search_case_sensitive"));
    searchLineEdit->setWholeWordsOnlyAction(window->action("edit_search_whole_words_only"));
    searchLineEdit->setRegularExpressionAction(window->action("edit_search_regular_expression"));
    searchLineEdit->setDynamicUpdate();

    dock->registerWidget(searchLineEdit, "vertical_search_line_edit");
    editLayout->addWidget(searchLineEdit, 1, 0, 1, 4, Qt::AlignJustify);

    editLayout->addWidget(new EQt::ToolButton(window->action("edit_search_next")), 1, 4);
    editLayout->addWidget(new EQt::ToolButton(window->action("edit_search_previous")), 1, 5);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedControlsSpacing);

    /* View menu items */

    QGridLayout* viewLayout = dock->newGridLayout("vertical_view");
    verticalLayout->addLayout(viewLayout);

    EQt::ZoomComboBox* zoomComboBox = new EQt::ZoomComboBox(window->action("zoom_action"), dock);
    zoomComboBox->setPageWidthStringExcluded();
    zoomComboBox->setFullPageStringExcluded();
    dock->registerWidget(zoomComboBox, "vertical_zoom_combo_box");
    viewLayout->addWidget(zoomComboBox, 1, 3, 1, 2, Qt::AlignCenter);

    viewLayout->addWidget(new EQt::ToolButton(window->action("view_margins")), 0, 0);
    viewLayout->addWidget(new EQt::ToolButton(window->action("view_guides")), 0, 1);
    viewLayout->setColumnMinimumWidth(2, 6);
    viewLayout->addWidget(new EQt::ToolButton(window->action("view_zoom_in")), 0, 3);
    viewLayout->addWidget(new EQt::ToolButton(window->action("view_zoom_out")), 0, 4);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void HomeBuilderInitial::buildInsertDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow*
    ) {
    /* Annotations items */

    EQt::StackingLayout* annotationsStackingLayout = new EQt::StackingLayout(
        EQt::StackingLayout::FixedDimension::HEIGHT,
        2
    );
    horizontalLayout->addLayout(annotationsStackingLayout);
    dock->registerLayout(annotationsStackingLayout, "horizontal_annotations_stacking_layout");

    annotationsStackingLayout->setSpacing(dock->minimumLayoutSpacing());

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void HomeBuilderInitial::buildInsertDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow*
    ) {
    /* Annotations items */

    EQt::StackingLayout* annotationsStackingLayout = new EQt::StackingLayout(
        EQt::StackingLayout::FixedDimension::WIDTH,
        5
    );
    verticalLayout->addLayout(annotationsStackingLayout);
    dock->registerLayout(annotationsStackingLayout, "vertical_annotations_stacking_layout");

    annotationsStackingLayout->setSpacing(dock->minimumLayoutSpacing());

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}
