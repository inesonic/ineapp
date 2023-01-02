/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RunExportBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QFont>
#include <QColor>
#include <QStringList>
#include <QTimer>
#include <QStatusBar>
#include <QGroupBox>
#include <QMenu>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeySequence>
#include <QTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QFont>

#include <eqt_builder.h>
#include <eqt_progress_bar.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_tool_button.h>
#include <eqt_code_editor.h>
#include <eqt_code_editor_line_number_area.h>
#include <eqt_cpp_syntax_highlighter.h>

#include <ld_cpp_translation_phase.h>

#include "application.h"
#include "page_list.h"
#include "main_window.h"
#include "home_main_window_proxy.h"
#include "console_device.h"
#include "application_status_bar.h"
#include "run_export_main_window_proxy.h"
#include "run_export_view_proxy.h"
#include "run_export_builder.h"

const char RunExportBuilder::builderName[]  = "RunExportBuilder";
const char RunExportBuilder::debugDockName[] = "run_export";
const char RunExportBuilder::irDockName[] = "ir";
const char RunExportBuilder::consoleDockName[] = "console";

RunExportBuilder::RunExportBuilder(QObject* parent):EQt::Builder(parent) {}


RunExportBuilder::~RunExportBuilder() {}


unsigned RunExportBuilder::priority() const {
    return builderPriority;
}


const char* RunExportBuilder::builderIdentifier() const {
    return builderName;
}


const char* RunExportBuilder::plugInName() const {
    return "";
}


void RunExportBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    ApplicationStatusBar* applicationStatusBar = new ApplicationStatusBar(window);
    applicationStatusBar->setNumberBuildPhases(Ld::CppTranslationPhase().numberPhases());
    applicationStatusBar->setHidden(true);
    window->registerWidget(applicationStatusBar, "application_status_bar");
    window->setStatusBar(applicationStatusBar);

    window->addMenu(tr("&Debug"));

    /* File | Export functions */

    window->addMenuAction(tr("&File | &Export | &Image..."), "file_export_image");
    window->setToolTip("file_export_image", tr("Exports an element of your program as an image."));
    window->setWhatsThis(
        "file_export_image",
        tr(
            "You can use this option to export a single element in your program, such as an equation or plot, as "
            "an image."
          )
    );

    window->addMenuAction(tr("&File | &Export | &HTML..."), "file_export_html");
    window->setToolTip("file_export_html", tr("Exports your program as a document in HTML format."));
    window->setWhatsThis(
        "file_export_html",
        tr(
            "You can use this option to export your program as a document in HTML format.  You can export your "
            "program in HTML4 format without using a cascading style sheet or in HTML5 format with a cascading style "
            "sheet.  The exported HTML can also include documentation generated from every external reference in the "
            "order that they're included in your program."
          )
    );

    window->addMenuAction(tr("&File | &Export | &LaTeX..."), "file_export_latex");
    window->setToolTip("file_export_latex", tr("Exports your program as a document in LaTeX format."));
    window->setWhatsThis(
        "file_export_latex",
        tr(
            "You can use this option to export your program as a document in LaTeX format.  You can export your "
            "program either as a single LaTeX source file plus external image files or as three files containing "
            "the LaTeX preamble, your document content, and a top level file that includes the other files.  The "
            "exported LaTeX can also include documentation generated from every external reference in the order that "
            "they're included in your program."
          )
    );

    window->addMenuAction(tr("&File | &Export | &Object..."), "file_export_object");
    window->setToolTip("file_export_object", tr("Exports your program as a binary object."));
    window->setWhatsThis(
        "file_export_object",
        tr(
            "You can use this option to export your program as an object file.  On Windows, the program will have "
            "the extension \".obj\".  On MacOS and Linux, the program will have the extension \".o\".  The object "
            "file will have a standard API giving you access to the full program, along with specific variables and "
            "functions.  Consult the documentation and examples for details on how to make use of this feature."
        )
    );

    #if (defined(Q_OS_WIN))

        window->addMenuAction(tr("&File | &Export | &DLL..."), "file_export_library");

    #elif (defined(Q_OS_LINUX))

        window->addMenuAction(tr("&File | &Export | &Shared Library..."), "file_export_library");

    #elif (defined(Q_OS_DARWIN))

        window->addMenuAction(tr("&File | &Export | &Dynamic Library..."), "file_export_library");

    #else

        #error Unknown platform

    #endif

    window->setToolTip("file_export_library", tr("Exports your program as a dynamic library."));
    window->setWhatsThis(
        "file_export_library",
        tr(
           "You can use this option to export your program as a dynamic library which you can then link into other "
           "programs or embedded environments.  The library will present a standard API giving you access to the "
           "full program, along with specific variables and functions.  Consult the documentation and examples for "
           "details on how to make use of this feature."
        )
    );

    /* Debug functions */

    QAction* debugRun = window->addMenuAction(EQt::Application::icon("debug_run"), tr("&Debug | &Run"), "debug_run");
    debugRun->setToolTip(tr("Run your program"));
    debugRun->setWhatsThis(tr("Use this option to run or debug your program."));

    QAction* debugStop = window->addMenuAction(
        EQt::Application::icon("debug_stop"),
        tr("&Debug | &Stop"),
        "debug_stop"
    );
    debugStop->setToolTip(tr("Stop your program"));
    debugStop->setWhatsThis(tr("Use this option to stop your running program."));

    window->addSeparator(tr("&Debug"));

    QAction* debugPause = window->addMenuAction(
        EQt::Application::icon("debug_pause"),
        tr("&Debug | &Pause"),
        "debug_pause"
    );
    debugPause->setToolTip(tr("Pause your program"));
    debugPause->setWhatsThis(tr("Use this option to pause your program."));

    QAction* debugStep = window->addMenuAction(
        EQt::Application::icon("debug_step"),
        tr("&Debug | Single S&tep"),
        "debug_single_step"
    );
    debugStep->setToolTip(tr("Perform the next operation, then stop"));
    debugStep->setWhatsThis(tr("Use this option to perform the next operation, then stop."));

    QAction* debugRunToCursor = window->addMenuAction(
        tr("&Debug | Run To &Cursor"),
        "debug_run_to_cursor"
    );
    debugRunToCursor->setToolTip(tr("Run to the cursor"));
    debugRunToCursor->setWhatsThis(tr("Use this option to run the program to the cursor location, then stop."));

    window->addSeparator(tr("&Debug"));

    QAction* debugInspector = window->addMenuAction(
        tr("&Debug | &Inspect variable..."),
        "debug_inspect_variable"
    );
    debugInspector->setToolTip(tr("Inspect variable"));
    debugInspector->setWhatsThis(tr("Use this option to view the value of a variable."));

    window->addSeparator(tr("&Debug"));

    QAction* debugToggleInstructionBreakpoint = window->addMenuAction(
        tr("&Debug | Set Instruction &Breakpoint"),
        "debug_toggle_instruction_breakpoint"
    );
    debugToggleInstructionBreakpoint->setToolTip(tr("Toggles an instruction breakpoint on/off"));
    debugToggleInstructionBreakpoint->setWhatsThis(
        tr("Use this option to set or clear an instruction breakpoint.  Your program will pause when it reaches an "
           "instruction breakpoint, allowing you to inspect variables."
        )
    );

    QAction* debugClearAllBreakpoint = window->addMenuAction(
        tr("&Debug | &Clear All Breakpoints"),
        "debug_clear_all_breakpoints"
    );
    debugClearAllBreakpoint->setToolTip(tr("Clears all breakpoints"));
    debugClearAllBreakpoint->setWhatsThis(tr("Use this option to quickly clear all breakpoints."));

    #if (defined(Q_OS_DARWIN))

        debugRun->setShortcut(QKeySequence(tr("Ctrl+Y")));
        debugPause->setShortcut(QKeySequence(tr("Ctrl+Shift+Y")));
        debugToggleInstructionBreakpoint->setShortcut(QKeySequence(tr("Ctrl+\\")));
        debugStep->setShortcut(QKeySequence(tr("Ctrl+Shift+O")));

    #elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        debugRun->setShortcut(QKeySequence(tr("F5", "&Debug | &Run")));
        debugToggleInstructionBreakpoint->setShortcut(QKeySequence(tr("F9", "&Debug | Set Instruction &Breakpoint")));
        debugStep->setShortcut(QKeySequence(tr("F10", "&Debug | Single S&tep")));

    #else

        #error Unknown platform

    #endif
}


void RunExportBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* debugDock = window->addDockWidget(
        debugDockName,
        tr("Debug"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER, "insert", true)
    );
    debugDock->setCloseButtonEnabled();

    QAction* debugDockToggleAction = window->addDockToggleMenuItem(
        debugDockName,
        tr("&View | &Docks"),
        "debug_dock_toggle_action"
    );
    debugDockToggleAction->setToolTip(tr("Show/hide the debug dock"));
    debugDockToggleAction->setWhatsThis(
        tr(
           "You can use this menu item to show or hide the Debug/Run dock.  The Debug/Run dock includes a small "
           "collection of controls used to debug and run your program."
        )
    );

    QHBoxLayout* horizontalDebugLayout = debugDock->newHBoxLayout("horizontal");
    debugDock->setHorizontalLayout(horizontalDebugLayout);

    buildDebugDockHorizontalOrientation(debugDock, horizontalDebugLayout, window);

    QVBoxLayout* verticalDebugLayout = debugDock->newVBoxLayout("vertical");
    debugDock->setVerticalLayout(verticalDebugLayout);

    buildDebugDockVerticalOrientation(debugDock, verticalDebugLayout, window);

    if (Application::debugModeEnabled()) {
        EQt::ProgrammaticDockWidget* irDock = window->addDockWidget(
            irDockName,
            tr("IR"),
            EQt::DockWidgetDefaults(
                EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION,
                consoleDockName,
                true
            )
        );
        irDock->setCloseButtonEnabled();
        //irDock->setAllowSizeAdjustment();

        QAction* irDockToggleAction = window->addDockToggleMenuItem(
            irDockName,
            tr("&View | &Docks"),
            "ir_dock_toggle_action"
        );
        irDockToggleAction->setToolTip(tr("Show/hide the intermediate representation dock"));
        irDockToggleAction->setWhatsThis(
            tr(
               "You can use this menu item to show or hide the intermediate representation dock.  The intermediate "
               "representation dock allows you to view the IR generated by the translation engine prior to "
               "conversion to AST."
              )
        );

        buildIrDock(irDock, window);

        EQt::ProgrammaticDockWidget* consoleDock = window->addDockWidget(
            consoleDockName,
            tr("Console"),
            EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Area::BOTTOM, true)
        );
        consoleDock->setCloseButtonEnabled();
        //consoleDock->setAllowSizeAdjustment();

        QAction* consoleDockToggleAction = window->addDockToggleMenuItem(
            consoleDockName,
            tr("&View | &Docks"),
            "console_dock_toggle_action"
        );
        consoleDockToggleAction->setToolTip(tr("Show/hide the console dock"));
        consoleDockToggleAction->setWhatsThis(
            tr(
               "You can use this menu item to show or hide the debug console dock.  The console dock allows you to "
               "monitor debug output generated by your model during execution."
              )
        );

        buildConsoleDock(consoleDock, window);
    }
}


EQt::ProgrammaticMainWindowProxy* RunExportBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new RunExportMainWindowProxy(window);
}


EQt::ProgrammaticViewProxy* RunExportBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new RunExportViewProxy(view, window);
}


void RunExportBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QMenu* rightClickMenu = mainWindow->widget<QMenu>("right_click_menu");

    QAction* exportImage   = window->action("file_export_image");
    QAction* exportHtml    = window->action("file_export_html");
    QAction* exportLaTeX   = window->action("file_export_latex");
    QAction* exportLibrary = window->action("file_export_library");
    QAction* exportObject = window->action("file_export_object");

    QAction* debugRun                         = window->action("debug_run");
    QAction* debugStop                        = window->action("debug_stop");
    QAction* debugPause                       = window->action("debug_pause");
    QAction* debugStep                        = window->action("debug_single_step");
    QAction* debugRunToCursor                 = window->action("debug_run_to_cursor");
    QAction* debugInspector                   = window->action("debug_inspect_variable");
    QAction* debugToggleInstructionBreakpoint = window->action("debug_toggle_instruction_breakpoint");
    QAction* debugClearAllBreakpoints         = window->action("debug_clear_all_breakpoints");

    rightClickMenu->addSeparator();

    rightClickMenu->addAction(debugRun);
    rightClickMenu->addAction(debugStop);
    rightClickMenu->addAction(debugPause);
    rightClickMenu->addAction(debugStep);
    rightClickMenu->addAction(debugRunToCursor);

    rightClickMenu->addSeparator();
    rightClickMenu->addAction(debugInspector);
    rightClickMenu->addSeparator();

    rightClickMenu->addAction(debugToggleInstructionBreakpoint);
    rightClickMenu->addAction(debugClearAllBreakpoints);

    bool success = mainWindow->setCommand(tr("expimg"), exportImage);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("exphtml"), exportHtml);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("explatex"), exportLaTeX);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("explib"), exportLibrary);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("expobj"), exportObject);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("run"), debugRun);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("kill"), debugStop);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("pause"), debugPause);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("step"), debugStep);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("runtohere"), debugRunToCursor);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("inspect"), debugInspector);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("ibkp"), debugToggleInstructionBreakpoint);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("clrbkp"), debugClearAllBreakpoints);
    Q_ASSERT(success);
}


void RunExportBuilder::buildDebugDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {

    /* Debug menu items */

    QGridLayout* debugLayout = dock->newGridLayout("horizontal_debug");
    horizontalLayout->addLayout(debugLayout);

    EQt::ToolButton* runToolButton = new EQt::ToolButton(window->action("debug_run"));
    dock->registerWidget(runToolButton, "run_tool_button");
    debugLayout->addWidget(runToolButton, 0, 0);

    debugLayout->addWidget(new EQt::ToolButton(window->action("debug_stop")), 0, 1);
    debugLayout->addWidget(new EQt::ToolButton(window->action("debug_pause")), 1, 0);
    debugLayout->addWidget(new EQt::ToolButton(window->action("debug_single_step")), 1, 1);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void RunExportBuilder::buildDebugDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {

    /* Debug menu items */

    QHBoxLayout* debugLayout = dock->newHBoxLayout("vertical_debug");
    verticalLayout->addLayout(debugLayout);

    debugLayout->addWidget(new EQt::ToolButton(window->action("debug_run")));
    debugLayout->addWidget(new EQt::ToolButton(window->action("debug_stop")));
    debugLayout->addWidget(new EQt::ToolButton(window->action("debug_pause")));
    debugLayout->addWidget(new EQt::ToolButton(window->action("debug_single_step")));

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void RunExportBuilder::buildIrDock(EQt::ProgrammaticDockWidget* dock, EQt::ProgrammaticMainWindow* window) {
    EQt::CodeEditor* irViewer = new EQt::CodeEditor(window);
    dock->setSingleWidgetMode(irViewer);

    QFont codeFont = Application::font();

    #if (defined(Q_OS_WIN))

        codeFont.setFamily("Consolas");

    #elif (defined(Q_OS_DARWIN))

        codeFont.setFamily("Monaco");

    #elif (defined(Q_OS_LINUX))

        codeFont.setFamily("DejaVu Sans Mono");

    #else

        #error Unknown platform

    #endif

    codeFont.setStyleHint(QFont::Monospace);

    irViewer->setReadOnly(true);
    irViewer->setWordWrapMode(QTextOption::WrapMode::NoWrap);
    irViewer->document()->setDefaultFont(codeFont);
    irViewer->setLineNumberArea(new EQt::CodeEditorLineNumberArea(irViewer));
    irViewer->lineNumberArea()->setForegroundRole(QPalette::Window);
    irViewer->lineNumberArea()->setBackgroundRole(QPalette::Dark);
    irViewer->lineNumberArea()->setLineNumberFont(codeFont);

    new EQt::CppSyntaxHighlighter(irViewer->document()); // The syntax highlighter is owned by the document.

    window->registerWidget(irViewer, "ir_viewer");
}


void RunExportBuilder::buildConsoleDock(EQt::ProgrammaticDockWidget* dock, EQt::ProgrammaticMainWindow* window) {
    QTextEdit* consoleTextEdit = new QTextEdit(window);
    dock->setSingleWidgetMode(consoleTextEdit);

    QFont codeFont = Application::font();

    #if (defined(Q_OS_WIN))

        codeFont.setFamily("Consolas");

    #elif (defined(Q_OS_DARWIN))

        codeFont.setFamily("Monaco");

    #elif (defined(Q_OS_LINUX))

        codeFont.setFamily("DejaVu Sans Mono");

    #else

        #error Unknown platform

    #endif

    codeFont.setStyleHint(QFont::Monospace);

    consoleTextEdit->setReadOnly(true);
    consoleTextEdit->setWordWrapMode(QTextOption::WrapMode::NoWrap);
    consoleTextEdit->setAcceptRichText(true);
    consoleTextEdit->document()->setDefaultFont(codeFont);

    window->registerWidget(consoleTextEdit, "console_text_edit");
}

