/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RunExportViewProxy class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QChar>
#include <QMessageBox>
#include <QAction>
#include <QVariant>
#include <QList>
#include <QSettings>
#include <QRegularExpression>
#include <QFont>
#include <QTimer>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QFile>
#include <QByteArray>

#include <eqt_file_dialog.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_progress_bar.h>
#include <eqt_code_editor.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_variable_element.h>
#include <ld_calculated_value.h>
#include <ld_root_import.h>
#include <ld_document_setting.h>
#include <ld_element_cursor.h>
#include <ld_capabilities.h>
#include <ld_code_generator.h>
#include <ld_html_code_generator.h>
#include <ld_latex_code_generator.h>
#include <ld_cpp_code_generator_output_types.h>
#include <ld_cpp_code_generator.h>
#include <ld_diagnostic_structures.h>
#include <ld_diagnostic.h>
#include <ld_cpp_code_generator_diagnostic.h>
#include <ld_html_code_generator_diagnostic.h>
#include <ld_latex_code_generator_diagnostic.h>

#include "application.h"
#include "code_generator_export_status_dialog.h"
#include "image_file_dialog.h"
#include "html_export_dialog.h"
#include "latex_export_dialog.h"
#include "object_export_dialog.h"
#include "library_export_dialog.h"
#include "root_presentation.h"
#include "document.h"
#include "build_execute_state_machine.h"
#include "presentation.h"
#include "cursor.h"
#include "cursor_position_setting.h"
#include "desktop_services.h"
#include "inspector_dialog.h"
#include "run_export_builder.h"
#include "main_window.h"
#include "application_status_bar.h"
#include "view_widget.h"
#include "run_export_view_proxy.h"

RunExportViewProxy::RunExportViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ),CppCodeGeneratorVisual::Client(
        true
    ) {}


RunExportViewProxy::~RunExportViewProxy() {}


void RunExportViewProxy::bindDynamic() {
    connectSignals();
}


void RunExportViewProxy::unbindDynamic() {
    disconnectSignals();
}


void RunExportViewProxy::bind() {}


void RunExportViewProxy::elementStackChanged() {
    MainWindow*  window     = RunExportViewProxy::window();
    ViewWidget*  viewWidget = view();
    Cursor::Type cursorType = viewWidget->cursorType();

    QAction* exportImage                            = window->action("file_export_image");
    QAction* debugRunToCursorAction                 = window->action("debug_run_to_cursor");
    QAction* debugToggleInstructionBreakpointAction = window->action("debug_toggle_instruction_breakpoint");
    QAction* debugInspector                         = window->action("debug_inspect_variable");

    if (cursorType == Cursor::Type::TEXT || cursorType == Cursor::Type::ELEMENT) {
        const Ld::ElementCursor& elementCursor      = viewWidget->cursor().elementCursor();
        Ld::ElementPointer       elementUnderCursor = elementCursor.element();

        if (!elementUnderCursor.isNull()) {
            bool canExportImage = elementUnderCursor->exportImageCapability() != Ld::Element::ExportImageCapability::NONE;
            exportImage->setEnabled(canExportImage);

            bool canSetInstructionBreakpoint = (
                   !elementUnderCursor.isNull()
                && elementUnderCursor->supportsInstructionBreakpoints()
            );

            debugRunToCursorAction->setEnabled(canSetInstructionBreakpoint);
            debugToggleInstructionBreakpointAction->setEnabled(canSetInstructionBreakpoint);

            if (canSetInstructionBreakpoint && elementUnderCursor->instructionBreakpointSet()) {
                debugToggleInstructionBreakpointAction->setText(tr("Clear Instruction &Breakpoint"));
            } else {
                debugToggleInstructionBreakpointAction->setText(tr("Set Instruction &Breakpoint"));
            }

            if (elementUnderCursor->typeName() == Ld::VariableElement::elementName) {
                Ld::CalculatedValue calculatedValue = elementUnderCursor->calculatedValue();
                debugInspector->setEnabled(calculatedValue.valueType() != Ld::DataType::ValueType::NONE);
            } else {
                debugInspector->setEnabled(false);
            }
        } else {
            exportImage->setEnabled(false);

            debugRunToCursorAction->setEnabled(false);
            debugInspector->setEnabled(false);
            debugToggleInstructionBreakpointAction->setEnabled(false);
            debugToggleInstructionBreakpointAction->setText(tr("Set Instruction &Breakpoint"));
        }
    } else {
        exportImage->setEnabled(false);

        debugRunToCursorAction->setEnabled(false);
        debugInspector->setEnabled(false);
        debugToggleInstructionBreakpointAction->setEnabled(false);
        debugToggleInstructionBreakpointAction->setText(tr("Set Instruction &Breakpoint"));
    }
}


void RunExportViewProxy::fileExportImage() {
    ViewWidget*  viewWidget = view();
    Cursor::Type cursorType = viewWidget->cursorType();

    if (cursorType == Cursor::Type::TEXT || cursorType == Cursor::Type::ELEMENT) {
        const Ld::ElementCursor& elementCursor      = viewWidget->cursor().elementCursor();
        Ld::ElementPointer       elementUnderCursor = elementCursor.element();

        if (elementUnderCursor->exportImageCapability() != Ld::Element::ExportImageCapability::NONE) {
            ImageFileDialog* fileDialog = Application::imageFileDialog();
            fileDialog->configureMode(ImageFileDialog::DialogMode::EXPORT_IMAGE);

            bool accepted = fileDialog->exec();
            if (accepted) {
                Q_ASSERT(fileDialog->selectedFiles().count() == 1);
                QString filepath = fileDialog->selectedFiles().at(0);

                QByteArray imageData = elementUnderCursor->exportImage(300.0);
                if (!imageData.isEmpty()) {
                    QPixmap pixmap;
                    pixmap.loadFromData(imageData);
                    bool success = pixmap.save(filepath);
                    if (!success) {
                        QMessageBox::information(
                            window(),
                            tr("Could Not Export Image"),
                            tr("Could not write to file \"%1\".").arg(filepath),
                            QMessageBox::StandardButton::Ok
                        );
                    }
               } else {
                    QMessageBox::information(
                        window(),
                        tr("Could Not Export Image"),
                        tr("Image could not be created."),
                        QMessageBox::StandardButton::Ok
                    );
                }
            }
        } else {
            QMessageBox::information(
                window(),
                tr("Unexpected State"),
                tr("Element does not support image export capability."),
                QMessageBox::StandardButton::Ok
            );
        }
    } else {
        QMessageBox::information(
            window(),
            tr("Unexpected State"),
            tr("Unexpected cursor state."),
            QMessageBox::StandardButton::Ok
        );
    }
}


void RunExportViewProxy::fileExportHtml() {
    MainWindow* window = RunExportViewProxy::window();

    HtmlExportDialog htmlExportDialog(window);
    htmlExportDialog.loadSettings("HtmlExportDialog");

    bool accepted = htmlExportDialog.exec();
    if (accepted) {
        htmlExportDialog.saveSettings("HtmlExportDialog");

        QString exportDirectory = htmlExportDialog.selectedDirectory();
        bool    doExport        = doExportMarkup(exportDirectory, tr("HTML"));
        if (doExport) {
            HtmlExportDialog::MathMode          mathMode          = htmlExportDialog.mathMode();
            HtmlExportDialog::HtmlStyle         htmlStyle         = htmlExportDialog.htmlStyle();
            HtmlExportDialog::ImageHandlingMode imageHandlingMode = htmlExportDialog.imageHandlingMode();
            bool                                includeImports    = htmlExportDialog.includeImports();

            ViewWidget*                     view        = RunExportViewProxy::view();
            Document*                       document    = view->document();
            QSharedPointer<Ld::RootElement> rootElement = document->element();

            QFileInfo exportDirectoryInformation(exportDirectory);

            CodeGeneratorExportStatusDialog exportStatusDialog(window);
            exportStatusDialog.setWindowTitle(tr("Export HTML"));
            exportStatusDialog.setLeaderText(
                tr("Exporting HTML into:\n%1").arg(exportDirectoryInformation.absoluteFilePath())
            );

            QSharedPointer<Ld::HtmlCodeGenerator>
                codeGenerator = Ld::CodeGenerator::codeGenerator(Ld::HtmlCodeGenerator::codeGeneratorName)
                                .dynamicCast<Ld::HtmlCodeGenerator>();

            Q_ASSERT(!codeGenerator.isNull());
            codeGenerator->setVisual(&exportStatusDialog);
            codeGenerator->setMathMode(mathMode);
            codeGenerator->setHtmlStyle(htmlStyle);
            codeGenerator->setImageHandlingMode(imageHandlingMode);
            codeGenerator->setProcessImports(includeImports);
            codeGenerator->setReportMissingPerElementTranslators();

            QString primaryFile = exportDirectoryInformation.absoluteFilePath() + "/index.html";
            bool success = codeGenerator->translate(
                rootElement,
                primaryFile,
                Ld::CodeGeneratorOutputType::ExportMode::EXPORT_AS_DIRECTORY
            );

            if (!success) {
                QMessageBox::warning(window, tr("HTML Export Failed"), tr("HTML export failed"), QMessageBox::Ok);
            } else {
                exportStatusDialog.exec();
            }

            codeGenerator->setVisual(Q_NULLPTR);

            if (success && codeGenerator->reportedDiagnostics().isEmpty()) {
                DesktopServices::openUrl(QUrl(primaryFile));
            }
        }
    }
}


void RunExportViewProxy::fileExportLaTeX() {
    MainWindow* window = RunExportViewProxy::window();

    LaTeXExportDialog latexExportDialog(window);
    latexExportDialog.loadSettings("LaTeXExportDialog");

    bool accepted = latexExportDialog.exec();
    if (accepted) {
        latexExportDialog.saveSettings("LaTeXExportDialog");

        QString  exportDirectory = latexExportDialog.selectedDirectory();
        bool     doExport        = doExportMarkup(exportDirectory, tr("LaTeX"));
        if (doExport) {
            LaTeXExportDialog::ImageMode   imageMode              = latexExportDialog.imageMode();
            LaTeXExportDialog::UnicodeMode unicodeTranslationMode = latexExportDialog.unicodeTranslationMode();
            bool                           exportToSingleFile     = latexExportDialog.singleFile();
            bool                           includeImports         = latexExportDialog.includeImports();

            ViewWidget*                     view        = RunExportViewProxy::view();
            Document*                       document    = view->document();
            QSharedPointer<Ld::RootElement> rootElement = document->element();

            QFileInfo exportDirectoryInformation(exportDirectory);

            CodeGeneratorExportStatusDialog exportStatusDialog(window);
            exportStatusDialog.setWindowTitle(tr("Export LaTeX"));
            exportStatusDialog.setLeaderText(
                tr("Exporting LaTeX into:\n%1").arg(exportDirectoryInformation.absoluteFilePath())
            );

            QSharedPointer<Ld::LaTeXCodeGenerator>
                codeGenerator = Ld::CodeGenerator::codeGenerator(Ld::LaTeXCodeGenerator::codeGeneratorName)
                                .dynamicCast<Ld::LaTeXCodeGenerator>();

            Q_ASSERT(!codeGenerator.isNull());
            codeGenerator->setVisual(&exportStatusDialog);
            codeGenerator->setImageMode(imageMode);
            codeGenerator->setSingleFile(exportToSingleFile);
            codeGenerator->setCopyrightIncluded();
            codeGenerator->setUnicodeTranslationMode(unicodeTranslationMode);
            codeGenerator->setProcessImports(includeImports);
            codeGenerator->setReportMissingPerElementTranslators();

            QString outputFilename;
            if (exportToSingleFile) {
                outputFilename = Ld::LaTeXCodeGenerator::latexTopFilename;
            } else {
                outputFilename = Ld::LaTeXCodeGenerator::latexBodyFilename;
            }

            QString primaryFile = exportDirectoryInformation.absoluteFilePath() + "/" + outputFilename;
            bool success = codeGenerator->translate(
                rootElement,
                primaryFile,
                Ld::CodeGeneratorOutputType::ExportMode::EXPORT_AS_DIRECTORY
            );

            if (!success) {
                QMessageBox::warning(window, tr("LaTeX Export Failed"), tr("LaTeX export failed"), QMessageBox::Ok);
            } else {
                exportStatusDialog.exec();
            }

            codeGenerator->setVisual(Q_NULLPTR);
        }
    }
}


void RunExportViewProxy::fileExportLibrary() {
    MainWindow* window = RunExportViewProxy::window();

    LibraryExportDialog libraryExportDialog(window);

    #if (defined(Q_OS_WIN))

        libraryExportDialog.loadSettings("LibraryExportDialog");

    #elif (defined(Q_OS_LINUX) || defined(Q_OS_DARWIN))

        libraryExportDialog.loadSettings();

    #else

        #error Unknown QBlittablePlatformPixmap

    #endif

    bool accepted = libraryExportDialog.exec();
    if (accepted) {
        #if (defined(Q_OS_WIN))

            libraryExportDialog.saveSettings("LibraryExportDialog");
            QString runtimeLibrary = libraryExportDialog.windowsImportLibrary();

        #elif (defined(Q_OS_LINUX) || defined(Q_OS_DARWIN))

            libraryExportDialog.saveSettings();
            QString runtimeLibrary;

        #else

            #error Unknown platform.

        #endif

        QString   libraryFile = libraryExportDialog.selectedFiles().first();
        QFileInfo libraryFileInfo(libraryFile);
        if (libraryFileInfo.suffix().isEmpty()) {
            #if (defined(Q_OS_WIN))

                libraryFile += tr(".dll");

            #elif (defined(Q_OS_LINUX))

                libraryFile += tr(".so");

            #elif (defined(Q_OS_DARWIN))

                libraryFile += tr(".dylib");

            #else

                #error Unknown platform

            #endif
        }

        ViewWidget*                     view        = RunExportViewProxy::view();
        Document*                       document    = view->document();
        QSharedPointer<Ld::RootElement> rootElement = document->element();

        translate(rootElement, libraryFile, Client::ExportMode::EXPORT_AS_FILE, runtimeLibrary);
    }
}


void RunExportViewProxy::fileExportObject() {
    MainWindow* window = RunExportViewProxy::window();

    ObjectExportDialog objectExportDialog(window);
    objectExportDialog.loadSettings();

    bool accepted = objectExportDialog.exec();
    if (accepted) {
        objectExportDialog.saveSettings();

        QString   objectFile = objectExportDialog.selectedFiles().first();
        QFileInfo objectFileInfo(objectFile);
        if (objectFileInfo.suffix().isEmpty()) {
            #if (defined(Q_OS_WIN))

                objectFile += tr(".obj");

            #elif (defined(Q_OS_LINUX) || defined(Q_OS_DARWIN))

                objectFile += tr(".o");

            #else

                #error Unknown platform

            #endif
        }

        ViewWidget*                     view        = RunExportViewProxy::view();
        Document*                       document    = view->document();
        QSharedPointer<Ld::RootElement> rootElement = document->element();

        Ld::CodeGeneratorOutputTypeContainer objectFileOutputType(new Ld::CppObjectFileOutputType);
        translate(rootElement, objectFile, objectFileOutputType, Client::ExportMode::EXPORT_AS_FILE);
    }
}


void RunExportViewProxy::debugRun() {
    ViewWidget*                     viewWidget               = RunExportViewProxy::view();
    Document*                       document                 = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement              = document->element();
    BuildExecuteStateMachine*       buildExecuteStateMachine = Application::buildExecuteStateMachine();

    if (buildExecuteStateMachine->isReady()) {
        buildExecuteStateMachine->run(rootElement, true);
    } else {
        buildExecuteStateMachine->resume();
    }
}


void RunExportViewProxy::debugStop() {
    BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
    buildExecuteStateMachine->abort();
}


void RunExportViewProxy::debugPause() {
    BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
    buildExecuteStateMachine->pause();
}


void RunExportViewProxy::debugSingleStep() {
    ViewWidget*                     viewWidget               = RunExportViewProxy::view();
    Document*                       document                 = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement              = document->element();
    BuildExecuteStateMachine*       buildExecuteStateMachine = Application::buildExecuteStateMachine();

    buildExecuteStateMachine->step(rootElement);
}


void RunExportViewProxy::debugRunToCursor() {
    ViewWidget* viewWidget = RunExportViewProxy::view();
    Cursor::Type cursorType = viewWidget->cursorType();

    if (cursorType == Cursor::Type::ELEMENT || cursorType == Cursor::Type::TEXT) {
        const Ld::ElementCursor& elementCursor = viewWidget->cursor().elementCursor();
        Ld::ElementPointer       element       = elementCursor.element();

        if (element->supportsInstructionBreakpoints()) {
            Document*                       document                 = viewWidget->document();
            QSharedPointer<Ld::RootElement> rootElement              = document->element();
            BuildExecuteStateMachine*       buildExecuteStateMachine = Application::buildExecuteStateMachine();

            if (buildExecuteStateMachine->isReady()) {
                buildExecuteStateMachine->run(rootElement, true, element);
            } else if (buildExecuteStateMachine->state() == BuildExecuteStateMachine::State::PAUSED) {
                buildExecuteStateMachine->resume(element);
            }
        }
    }
}


void RunExportViewProxy::debugInspectVariable() {
    ViewWidget*              viewWidget         = view();
    const Ld::ElementCursor& elementCursor      = viewWidget->cursor().elementCursor();
    Ld::ElementPointer       elementUnderCursor = elementCursor.element();
    Ld::CalculatedValue      calculatedValue    = elementUnderCursor->calculatedValue();

    InspectorDialog inspectorDialog(calculatedValue, window());
    int result = inspectorDialog.exec();
    if (result == InspectorDialog::Accepted) {
        BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
        bool success = buildExecuteStateMachine->updateVariable(inspectorDialog.calculatedValue());
        if (!success) {
            QString message;

            if (calculatedValue.name2().isEmpty()) {
                message = tr("Could not update variable %1").arg(calculatedValue.name1());
            } else {
                message = tr("Could not update variable %1<sub>%2</sub>.")
                          .arg(calculatedValue.name1())
                          .arg(calculatedValue.name2());
            }

            QMessageBox::warning(window(), tr("Could Not Update Variable"), message);
        }
    }
}


void RunExportViewProxy::debugToggleInstructionBreakpoint() {
    MainWindow* window      = RunExportViewProxy::window();
    ViewWidget* viewWidget  = view();
    Cursor::Type cursorType = viewWidget->cursorType();

    if (cursorType == Cursor::Type::TEXT || cursorType == Cursor::Type::ELEMENT) {
        const Ld::ElementCursor& elementCursor      = viewWidget->cursor().elementCursor();
        Ld::ElementPointer       elementUnderCursor = elementCursor.element();
        if (!elementUnderCursor.isNull() && elementUnderCursor->supportsInstructionBreakpoints()) {
            bool nowSet = !elementUnderCursor->instructionBreakpointSet();
            elementUnderCursor->setInstructionBreakpoint(nowSet);

            BuildExecuteStateMachine*       buildExecuteStateMachine = Application::buildExecuteStateMachine();
            Document*                       document                 = viewWidget->document();
            QSharedPointer<Ld::RootElement> rootElement              = document->element();

            if (buildExecuteStateMachine->isRunning(rootElement)) {
                Ld::ElementPointer element = elementUnderCursor;
                Ld::ElementPointer parent  = element->parent();
                while(!parent.isNull() && parent->supportsInstructionBreakpoints()) {
                    element = parent;
                    parent  = element->parent();
                }

                // TODO: Set/clear the breakpoint in the active model here.
            }

            QAction* debugToggleInstructionBreakpointAction = window->action("debug_toggle_instruction_breakpoint");

            if (nowSet) {
                debugToggleInstructionBreakpointAction->setText(tr("Clear Instruction &Breakpoint"));
            } else {
                debugToggleInstructionBreakpointAction->setText(tr("Set Instruction &Breakpoint"));
            }
        }
    }
}


void RunExportViewProxy::debugClearAllBreakpoints() {
    MainWindow*                     window      = RunExportViewProxy::window();
    ViewWidget*                     viewWidget  = view();
    Document*                       document    = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    int result = QMessageBox::question(
        window,
        tr("Are you sure ?"),
        tr("Are you sure you want to clear all breakpoints ?"),
        QMessageBox::Yes | QMessageBox::No
    );

    if (result == QMessageBox::Yes) {
        BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
        bool                      isRunning                = buildExecuteStateMachine->isRunning(rootElement);
        Ld::ElementPointerSet     allElements              = rootElement->descendants();

        for (  Ld::ElementPointerSet::const_iterator elementIterator    = allElements.constBegin(),
                                                     elementEndIterator = allElements.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (element->instructionBreakpointSet()) {
                element->clearInstructionBreakpoint();

                if (isRunning) {
                    // TODO: Clear the breakpoint in the active model here.
                }
            }
        }
    }
}


void RunExportViewProxy::debugEngineReady(bool nowReady) {
    MainWindow* window           = RunExportViewProxy::window();
    QAction*    debugRunAction   = window->action("debug_run");
    QAction*    debugStopAction  = window->action("debug_stop");
    QAction*    debugPauseAction = window->action("debug_pause");

    if (nowReady) {
        debugRunAction->setEnabled(true);
        debugStopAction->setEnabled(false);
        debugPauseAction->setEnabled(true);
    } else {
        ViewWidget*                     viewWidget  = RunExportViewProxy::view();
        Document*                       document    = viewWidget->document();
        QSharedPointer<Ld::RootElement> rootElement = document->element();

        BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();
        bool isRunning = buildExecuteStateMachine->isRunning(rootElement);

        debugRunAction->setEnabled(false);
        debugStopAction->setEnabled(isRunning);
        debugPauseAction->setEnabled(isRunning);
    }
}


void RunExportViewProxy::modelRunning(QSharedPointer<Ld::RootElement> runningRootElement, bool /* debugMode */) {
    MainWindow*                     window      = RunExportViewProxy::window();
    ViewWidget*                     viewWidget  = RunExportViewProxy::view();
    Document*                       document    = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();
    bool                            canControl  = (runningRootElement == rootElement);

    QAction* debugRunAction   = window->action("debug_run");
    QAction* debugStopAction  = window->action("debug_stop");
    QAction* debugPauseAction = window->action("debug_pause");

    debugRunAction->setEnabled(false);
    debugStopAction->setEnabled(canControl);
    debugPauseAction->setEnabled(canControl);

    if (canControl) {
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->setRunning();
    }
}


void RunExportViewProxy::modelStopped(QSharedPointer<Ld::RootElement> stoppedRootElement) {
    ViewWidget*                     viewWidget  = RunExportViewProxy::view();
    Document*                       document    = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    if (rootElement == stoppedRootElement) {
        MainWindow*           window               = RunExportViewProxy::window();
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->operationCompleted();
    }
}


void RunExportViewProxy::modelAborted(QSharedPointer<Ld::RootElement> abortedRootElement) {
    ViewWidget*                     viewWidget  = RunExportViewProxy::view();
    Document*                       document    = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    if (rootElement == abortedRootElement) {
        MainWindow*           window               = RunExportViewProxy::window();
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->setAborted();
    }
}


void RunExportViewProxy::modelPausedOnUserRequest(
        QSharedPointer<Ld::RootElement> pausedRootElement,
        Ld::ElementPointer              pausedAt
    ) {
    modelPaused(pausedRootElement, pausedAt, tr("User Request"));
}


void RunExportViewProxy::modelPausedAtInstructionBreakpoint(
        QSharedPointer<Ld::RootElement> pausedRootElement,
        Ld::ElementPointer              pausedAt
    ) {
    modelPaused(pausedRootElement, pausedAt, tr("Instruction Breakpoint"));
}


void RunExportViewProxy::modelPausedOnVariableUpdate(
        QSharedPointer<Ld::RootElement> pausedRootElement,
        Ld::ElementPointer              pausedAt,
        const Ld::IdentifierContainer&  /* identifierData */
    ) {
    modelPaused(pausedRootElement, pausedAt, tr("Data Breakpoint"));
}


void RunExportViewProxy::modelResumed(QSharedPointer<Ld::RootElement> resumedRootElement) {
    MainWindow*                     window      = RunExportViewProxy::window();
    ViewWidget*                     viewWidget  = RunExportViewProxy::view();
    Document*                       document    = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();
    bool                            canControl  = (resumedRootElement == rootElement);

    QAction* debugRunAction   = window->action("debug_run");
    QAction* debugStopAction  = window->action("debug_stop");
    QAction* debugPauseAction = window->action("debug_pause");

    debugRunAction->setEnabled(false);
    debugStopAction->setEnabled(canControl);
    debugPauseAction->setEnabled(canControl);

    if (canControl) {
        MainWindow*           window               = RunExportViewProxy::window();
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->setRunning();
    }
}


void RunExportViewProxy::documentChanged() {
    BuildExecuteStateMachine*       buildExecuteStateMachine = Application::buildExecuteStateMachine();
    ViewWidget*                     viewWidget               = RunExportViewProxy::view();
    Document*                       document                 = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement              = document->element();
    QSharedPointer<Ld::RootElement> activeRootElement        = buildExecuteStateMachine->rootElement();

    if (!activeRootElement.isNull()) {
        if (activeRootElement == rootElement) {
            buildExecuteStateMachine->rootElementChanged(rootElement);
        } else {
            Ld::RootElement::RootElementSet indirectlyImports = rootElement->indirectlyImportedBy();
            if (indirectlyImports.contains(activeRootElement)) {
                buildExecuteStateMachine->rootElementChanged(activeRootElement);
            }
        }
    }
}


void RunExportViewProxy::sendOutputToDevice(Model::Device device, const Model::Variant& value) {
    (void) device;
    (void) value;
}


void RunExportViewProxy::directoryIsReadOnly(const QString& directoryPath) {
    MainWindow* window = RunExportViewProxy::window();

    QMessageBox::information(
        window,
        tr("Directory Is Read Only"),
        tr(
            "The directory\n"
            "\n"
            "%1\n"
            "\n"
            "is read-only.  Please save your work to a writable\n"
            "directory before running or debugging it."
        ).arg(directoryPath)
    );

    ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
    applicationStatusBar->operationCompleted();
}


void RunExportViewProxy::clientOwnsGenerator() {
    MainWindow* window = RunExportViewProxy::window();
    QAction* fileExportLibrary = window->action("file_export_library");
    fileExportLibrary->setEnabled(false);

    QAction* fileExportObject = window->action("file_export_object");
    fileExportObject->setEnabled(false);
}


void RunExportViewProxy::generatorBusy() {
    MainWindow* window = RunExportViewProxy::window();

    QAction* fileExportLibrary = window->action("file_export_library");
    fileExportLibrary->setEnabled(false);

    QAction* fileExportObject = window->action("file_export_object");
    fileExportObject->setEnabled(false);
}


void RunExportViewProxy::generatorIdle() {
    MainWindow* window = RunExportViewProxy::window();

    QAction* fileExportLibrary = window->action("file_export_library");
    fileExportLibrary->setEnabled(true);

    QAction* fileExportObject = window->action("file_export_object");
    fileExportObject->setEnabled(true);
}


void RunExportViewProxy::translationStarted(QSharedPointer<Ld::RootElement> rootElement) {
    ViewWidget*                     viewWidget   = RunExportViewProxy::view();
    Document*                       document     = viewWidget->document();
    QSharedPointer<Ld::RootElement> documentRoot = document->element();

    if (documentRoot == rootElement) {
        MainWindow*           window               = RunExportViewProxy::window();
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->setBuildPhase(0);
    }
}


void RunExportViewProxy::translationPhaseStarted(
        QSharedPointer<Ld::RootElement> rootElement,
        unsigned                        newPhaseNumber,
        unsigned                        /* numberPhases */,
        const QString&                  /* phaseName */
    ) {
    ViewWidget*                     viewWidget   = RunExportViewProxy::view();
    Document*                       document     = viewWidget->document();
    QSharedPointer<Ld::RootElement> documentRoot = document->element();

    if (documentRoot == rootElement) {
        MainWindow*           window               = RunExportViewProxy::window();
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->setBuildPhase(newPhaseNumber);
    }
}


void RunExportViewProxy::translationCompleted(QSharedPointer<Ld::RootElement> rootElement, bool /* success */) {
    ViewWidget*                     viewWidget   = RunExportViewProxy::view();
    Document*                       document     = viewWidget->document();
    QSharedPointer<Ld::RootElement> documentRoot = document->element();

    if (documentRoot == rootElement) {
        MainWindow* window = RunExportViewProxy::window();
        if (!Application::buildExecuteStateMachine()->isRunning()) {
            ApplicationStatusBar* statusBar = window->widget<ApplicationStatusBar>("application_status_bar");
            statusBar->operationCompleted();
        }

        if (Application::debugModeEnabled()) {
            EQt::CodeEditor* irViewer = window->widget<EQt::CodeEditor>("ir_viewer");
            irViewer->setPlainText(intermediateRepresentation());
        }
    }
}


void RunExportViewProxy::translationAborted(QSharedPointer<Ld::RootElement> rootElement) {
    ViewWidget*                     viewWidget   = RunExportViewProxy::view();
    Document*                       document     = viewWidget->document();
    QSharedPointer<Ld::RootElement> documentRoot = document->element();

    if (documentRoot == rootElement) {
        MainWindow*           window               = RunExportViewProxy::window();
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->setBuildError(tr("Build Aborted"));
    }
}


void RunExportViewProxy::translationErrorDetected(
        QSharedPointer<Ld::RootElement> rootElement,
        Ld::DiagnosticPointer           diagnostic
    ) {
    ViewWidget*                     viewWidget   = RunExportViewProxy::view();
    Document*                       document     = viewWidget->document();
    QSharedPointer<Ld::RootElement> documentRoot = document->element();

    if (documentRoot == rootElement) {
        MainWindow*           window               = RunExportViewProxy::window();
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");

        Ld::Diagnostic::Type diagnosticType    = diagnostic->diagnosticType();
        QString              diagnosticMessage = diagnostic->diagnosticMessage();

        if (diagnosticType == Ld::Diagnostic::Type::WARNING || diagnosticType == Ld::Diagnostic::Type::INFORMATION) {
            applicationStatusBar->setBuildWarning(diagnosticMessage);
        } else {
            applicationStatusBar->setBuildError(diagnosticMessage);
        }
    }
}


void RunExportViewProxy::connectSignals() {
    MainWindow*                     window      = RunExportViewProxy::window();
    ViewWidget*                     viewWidget  = RunExportViewProxy::view();
    Document*                       document    = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    sessionConnect(viewWidget, &ViewWidget::elementStackChanged, &RunExportViewProxy::elementStackChanged);
    sessionConnect(viewWidget, &ViewWidget::documentChanged, &RunExportViewProxy::documentChanged);

    sessionConnect(window->action("file_export_image"), &QAction::triggered, &RunExportViewProxy::fileExportImage);
    sessionConnect(window->action("file_export_html"), &QAction::triggered, &RunExportViewProxy::fileExportHtml);
    sessionConnect(window->action("file_export_latex"), &QAction::triggered, &RunExportViewProxy::fileExportLaTeX);
    sessionConnect(window->action("file_export_library"), &QAction::triggered, &RunExportViewProxy::fileExportLibrary);
    sessionConnect(window->action("file_export_object"), &QAction::triggered, &RunExportViewProxy::fileExportObject);

    QAction* debugRunAction                         = window->action("debug_run");
    QAction* debugStopAction                        = window->action("debug_stop");
    QAction* debugPauseAction                       = window->action("debug_pause");
    QAction* debugSingleStepAction                  = window->action("debug_single_step");
    QAction* debugRunToCursorAction                 = window->action("debug_run_to_cursor");
    QAction* debugInspector                         = window->action("debug_inspect_variable");
    QAction* debugToggleInstructionBreakpointAction = window->action("debug_toggle_instruction_breakpoint");
    QAction* debugClearAllBreakpointsAction         = window->action("debug_clear_all_breakpoints");

    sessionConnect(debugRunAction, &QAction::triggered, &RunExportViewProxy::debugRun);
    sessionConnect(debugStopAction, &QAction::triggered, &RunExportViewProxy::debugStop);
    sessionConnect(debugPauseAction, &QAction::triggered, &RunExportViewProxy::debugPause);
    sessionConnect(debugSingleStepAction, &QAction::triggered, &RunExportViewProxy::debugSingleStep);
    sessionConnect(debugRunToCursorAction, &QAction::triggered, &RunExportViewProxy::debugRunToCursor);
    sessionConnect(debugInspector, &QAction::triggered, &RunExportViewProxy::debugInspectVariable);
    sessionConnect(
        debugToggleInstructionBreakpointAction,
        &QAction::triggered,
        &RunExportViewProxy::debugToggleInstructionBreakpoint
    );
    sessionConnect(debugClearAllBreakpointsAction, &QAction::triggered, &RunExportViewProxy::debugClearAllBreakpoints);

    BuildExecuteStateMachine* buildExecuteStateMachine = Application::buildExecuteStateMachine();

    sessionConnect(
        buildExecuteStateMachine,
        &BuildExecuteStateMachine::ready,
        &RunExportViewProxy::debugEngineReady
    );
    sessionConnect(buildExecuteStateMachine, &BuildExecuteStateMachine::running, &RunExportViewProxy::modelRunning);
    sessionConnect(buildExecuteStateMachine, &BuildExecuteStateMachine::stopped, &RunExportViewProxy::modelStopped);
    sessionConnect(buildExecuteStateMachine, &BuildExecuteStateMachine::aborted, &RunExportViewProxy::modelAborted);
    sessionConnect(
        buildExecuteStateMachine,
        &BuildExecuteStateMachine::pausedOnUserRequest,
        &RunExportViewProxy::modelPausedOnUserRequest
    );
    sessionConnect(
        buildExecuteStateMachine,
        &BuildExecuteStateMachine::pausedAtInstructionBreakpoint,
        &RunExportViewProxy::modelPausedAtInstructionBreakpoint
    );
    sessionConnect(
        buildExecuteStateMachine,
        &BuildExecuteStateMachine::pausedOnVariableUpdate,
        &RunExportViewProxy::modelPausedOnVariableUpdate
    );
    sessionConnect(buildExecuteStateMachine, &BuildExecuteStateMachine::resumed, &RunExportViewProxy::modelResumed);
    sessionConnect(
        buildExecuteStateMachine,
        &BuildExecuteStateMachine::sendOutputToDevice,
        &RunExportViewProxy::sendOutputToDevice
    );
    sessionConnect(
        buildExecuteStateMachine,
        &BuildExecuteStateMachine::directoryIsReadOnly,
        &RunExportViewProxy::directoryIsReadOnly
    );

    elementStackChanged();

    bool isReady   = buildExecuteStateMachine->isReady();
    bool isRunning = buildExecuteStateMachine->isRunning(rootElement);

    debugRunAction->setEnabled(isReady);
    debugStopAction->setEnabled(isRunning);
    debugPauseAction->setEnabled(isRunning || isReady);
}


void RunExportViewProxy::disconnectSignals() {
    sessionDisconnectAll();
}


void RunExportViewProxy::modelPaused(
        QSharedPointer<Ld::RootElement> pausedRootElement,
        Ld::ElementPointer              pausedAt,
        const QString&                  /* pauseReason */
    ) {
    MainWindow*                     window      = RunExportViewProxy::window();
    ViewWidget*                     viewWidget  = RunExportViewProxy::view();
    Document*                       document    = viewWidget->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();
    bool                            canControl  = (pausedRootElement == rootElement);

    QAction* debugRunAction   = window->action("debug_run");
    QAction* debugStopAction  = window->action("debug_stop");
    QAction* debugPauseAction = window->action("debug_pause");

    debugRunAction->setEnabled(canControl);
    debugStopAction->setEnabled(canControl);
    debugPauseAction->setEnabled(false);

    if (canControl) {
        ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
        applicationStatusBar->setPaused();

        if (!pausedAt.isNull()) {
            Presentation* presentation = dynamic_cast<Presentation*>(pausedAt->visual());
            if (presentation != Q_NULLPTR) {
                Ld::ElementCursor cursor(pausedAt);
                QList<QRectF> boundingRectangles = presentation->cursorRangeToScene(cursor);
                if (!boundingRectangles.isEmpty()) {
                    QRectF boundingRectangle = boundingRectangles.first();
                    if (boundingRectangle.width() == 0) {
                        boundingRectangle.setWidth(1);
                    }

                    if (boundingRectangle.height() == 0) {
                        boundingRectangle.setHeight(1);
                    }

                    for (  QList<QRectF>::const_iterator it  = boundingRectangles.constBegin(),
                                                         end = boundingRectangles.constEnd()
                         ; it != end
                         ; ++it
                        ) {
                        boundingRectangle |= *it;
                    }

                    viewWidget->forceVisibleArea(boundingRectangle);
                }
            }
        }
    }
}


bool RunExportViewProxy::doExportMarkup(const QString& exportDirectory, const QString& exportedContent) {
    bool doExport;

    MainWindow* window = RunExportViewProxy::window();

    QFileInfo exportDirectoryInformation(exportDirectory);
    if (exportDirectoryInformation.exists()) {
        if (exportDirectoryInformation.isDir()) {
            QMessageBox::StandardButton result;
            if (QDir(exportDirectory).isEmpty()) {
                result = QMessageBox::question(
                    window,
                    tr("Directory Exists"),
                    tr(
                        "You've selected an existing, empty, directory.\n"
                        "Do you want to export %1 into this directory ?"
                    ).arg(exportedContent),
                    QMessageBox::Yes | QMessageBox::No
                );
            } else {
                result = QMessageBox::warning(
                    window,
                    tr("Directory Exists"),
                    tr(
                        "You've selected an existing directory that already contains one or more files.\n"
                        "Do you really want to export %1 into this directory ?"
                    ).arg(exportedContent),
                    QMessageBox::Yes | QMessageBox::No
                );
            }

            doExport = (result == QMessageBox::Yes);
        } else {
            QMessageBox::information(
                window,
                tr("Can't Export %1").arg(exportedContent),
                tr("You've selected an existing file.\nYou can only export %1 into a directory.").arg(exportedContent),
                QMessageBox::Ok
            );
            doExport = false;
        }
    } else {
        doExport = true;
    }

    return doExport;
}


bool RunExportViewProxy::doOverwriteFile(const QString& outputFilename) {
    bool    doExport;
    QFileInfo selectedFileInformation(outputFilename);
    if (selectedFileInformation.exists()) {
        if (selectedFileInformation.isWritable()) {
            QMessageBox::StandardButton result = QMessageBox::question(
                window(),
                tr("File Exists"),
                tr("The file:\n%1\nexists, do you want to overwrite this file ?").arg(outputFilename),
                QMessageBox::Yes | QMessageBox::No
            );

            doExport = (result == QMessageBox::Yes);
        } else {
            QMessageBox::information(
                window(),
                tr("File Exists"),
                tr("The file\n%1\nexists and can not be overwritten.").arg(outputFilename)
            );

            doExport = false;
        }
    } else {
        doExport = true;
    }

    return doExport;
}
