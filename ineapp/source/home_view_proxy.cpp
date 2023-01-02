/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HomeViewProxy class.
***********************************************************************************************************************/

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
#include <QWhatsThis>
#include <QPrinter>
#include <QPrintDialog>
#include <QUrl>
#include <QMenu>
#include <QHash>

#include <QDebug>

#include <cstring>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_zoom_combo_box.h>
#include <eqt_search_line_edit.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_root_import.h>
#include <ld_document_setting.h>
#include <ld_element_cursor.h>
#include <ld_capabilities.h>
#include <ld_special_characters.h>
#include <ld_format.h>
#include <ld_character_format.h>
#include <ld_page_format.h>
#include <ld_code_generator.h>
#include <ld_html_code_generator.h>
#include <ld_latex_code_generator.h>
#include <ld_diagnostic_structures.h>
#include <ld_diagnostic.h>
#include <ld_html_code_generator_diagnostic.h>
#include <ld_latex_code_generator_diagnostic.h>
#include <ld_text_element.h>
#include <ld_list_placeholder_element.h>
#include <ld_placeholder_element.h>
#include <ld_function_placeholder_element.h>
#include <ld_brace_conditional_format.h>

#include "clipboard.h"
#include "application.h"
#include "fixer.h"
#include "desktop_services.h"
#include "imports_dialog.h"
#include "printing_engine.h"
#include "code_generator_export_status_dialog.h"
#include "document_preferences_dialog.h"
#include "printing_status_dialog.h"
#include "pdf_export_dialog.h"
#include "clipboard_mime_data.h"
#include "document.h"
#include "cursor.h"
#include "cursor_position_setting.h"
#include "insert_string_command.h"
#include "insert_element_command.h"
#include "delete_command.h"
#include "cut_copy_command.h"
#include "paste_command.h"
#include "command_container.h"
#include "find_and_replace_dialog.h"
#include "home_builder_initial.h"
#include "zoom_setting.h"
#include "main_window.h"
#include "application_status_bar.h"
#include "view_widget.h"
#include "home_view_proxy.h"

QHash<Ld::ElementPointer, unsigned> HomeViewProxy::allActiveParents;

HomeViewProxy::HomeViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {
    currentFindAndReplaceDialog = Q_NULLPTR;
}


HomeViewProxy::~HomeViewProxy() {}


void HomeViewProxy::loadSettings() {
    MainWindow* window   = HomeViewProxy::window();
    ViewWidget* view     = HomeViewProxy::view();
    QSettings*  settings = Application::settings();

    bool  ok;
    float defaultZoomValue = settings->value("default_zoom", 1.0).toFloat(&ok);
    if (ok && defaultZoomValue > 0 && defaultZoomValue < 10.0) {
        QAction* zoomAction = window->action("zoom_action");
        Q_ASSERT(zoomAction != Q_NULLPTR);

        QString  zoomText = tr("%1%").arg(static_cast<unsigned>(defaultZoomValue * 100.0 + 0.5));
        zoomAction->setData(QVariant::fromValue(zoomText));
        zoomAction->trigger();
        view->setZoom(defaultZoomValue);
    }
}


void HomeViewProxy::saveSettings() {
    ViewWidget* view     = HomeViewProxy::view();
    QSettings*  settings = Application::settings();

    float currentZoom = view->zoom();
    settings->setValue("default_zoom", currentZoom);
}


void HomeViewProxy::bindDynamic() {
    connectSignals();
    view()->restoreFocus();
}


void HomeViewProxy::unbindDynamic() {
    disconnectSignals();
}


void HomeViewProxy::bind() {
    ViewWidget* view = HomeViewProxy::view();

    connect(view, &ViewWidget::addTabKeyPressed, this, &HomeViewProxy::addTabKeyPressed);
    connect(view, &ViewWidget::backspaceKeyPressed, this, &HomeViewProxy::backspaceKeyPressed);
    connect(view, &ViewWidget::closeKeyPressed, this, &HomeViewProxy::closeKeyPressed);
    connect(view, &ViewWidget::deleteKeyPressed, this, &HomeViewProxy::deleteKeyPressed);
    connect(view, &ViewWidget::deleteToEndOfLineKeyPressed, this, &HomeViewProxy::deleteToEndOfLineKeyPressed);
    connect(view, &ViewWidget::deleteToEndOfWordKeyPressed, this, &HomeViewProxy::deleteToEndOfWordKeyPressed);
    connect(view, &ViewWidget::deleteToStartOfWordKeyPressed, this, &HomeViewProxy::deleteToStartOfWordKeyPressed);
    connect(view, &ViewWidget::deleteLineKeyPressed, this, &HomeViewProxy::deleteLineKeyPressed);
    connect(view, &ViewWidget::findNextKeyPressed, this, &HomeViewProxy::findNextKeyPressed);
    connect(view, &ViewWidget::findPreviousKeyPressed, this, &HomeViewProxy::findPreviousKeyPressed);
    connect(view, &ViewWidget::forwardKeyPressed, this, &HomeViewProxy::forwardKeyPressed);
    connect(view, &ViewWidget::helpContentsKeyPressed, this, &HomeViewProxy::helpContentsKeyPressed);
    connect(view, &ViewWidget::insertLineSeparatorKeyPressed, this, &HomeViewProxy::insertLineSeparatorKeyPressed);
    connect(
        view,
        &ViewWidget::insertParagraphSeparatorKeyPressed,
        this,
        &HomeViewProxy::insertParagraphSeparatorKeyPressed
    );
    connect(view, &ViewWidget::nextChildKeyPressed, this, &HomeViewProxy::nextChildKeyPressed);
    connect(view, &ViewWidget::preferencesKeyPressed, this, &HomeViewProxy::preferencesKeyPressed);
    connect(view, &ViewWidget::previousChildKeyPressed, this, &HomeViewProxy::previousChildKeyPressed);
    connect(view, &ViewWidget::printKeyPressed, this, &HomeViewProxy::printKeyPressed);
    connect(view, &ViewWidget::quitKeyPressed, this, &HomeViewProxy::quitKeyPressed);
    connect(view, &ViewWidget::replaceKeyPressed, this, &HomeViewProxy::replaceKeyPressed);
    connect(view, &ViewWidget::deselectKeyPressed, this, &HomeViewProxy::deselectKeyPressed);
    connect(view, &ViewWidget::unknownKeyPressed, this, &HomeViewProxy::unknownKeyPressed);
    connect(view, &ViewWidget::whatsThisKeyPressed, this, &HomeViewProxy::whatsThisKeyPressed);
    connect(view, &ViewWidget::zoomInKeyPressed, this, &HomeViewProxy::zoomInKeyPressed);
    connect(view, &ViewWidget::zoomOutKeyPressed, this, &HomeViewProxy::zoomOutKeyPressed);
    connect(view, &ViewWidget::fullScreenKeyPressed, this, &HomeViewProxy::fullScreenKeyPressed);
    connect(view, &ViewWidget::cancelKeyPressed, this, &HomeViewProxy::cancelKeyPressed);
    connect(view, &ViewWidget::textKeyPressed, this, &HomeViewProxy::textKeyPressed);
    connect(view, &ViewWidget::textKeyPressedOnPlaceholder, this, &HomeViewProxy::textKeyPressedOnPlaceholder);

    connect(view, &ViewWidget::rightMouseButtonClicked, this, &HomeViewProxy::rightMouseButtonClicked);

    connect(view, &ViewWidget::titleChanged, this, &HomeViewProxy::titleChanged);
    connect(view, &ViewWidget::shortformNameChanged, this, &HomeViewProxy::shortformNameChanged);
    connect(view, &ViewWidget::documentModified, this, &HomeViewProxy::documentModified);
}


void HomeViewProxy::aboutToSave(Document*) {
    ViewWidget* view = HomeViewProxy::view();

    QSharedPointer<CursorPositionSetting> cursorPositionSetting(new CursorPositionSetting);
    cursorPositionSetting->setCursorPosition(view->cursor().elementCursor());
    view->addDocumentSetting(cursorPositionSetting);

    QSharedPointer<ZoomSetting> zoomSetting(new ZoomSetting);
    zoomSetting->setZoomValue(view->zoom());
    view->addDocumentSetting(zoomSetting);
}


void HomeViewProxy::loaded(Document* document) {
    MainWindow* window = HomeViewProxy::window();
    ViewWidget* view   = HomeViewProxy::view();

    QSharedPointer<CursorPositionSetting> cursorPositionSetting =
        view->documentSetting<CursorPositionSetting>();
    if (!cursorPositionSetting.isNull()) {
        Ld::ElementCursor elementCursor = cursorPositionSetting->cursorPosition(document->element());
        view->updateCursorPosition(elementCursor);
    }

    QSharedPointer<ZoomSetting> zoomSetting = view->documentSetting<ZoomSetting>();
    if (!zoomSetting.isNull() && zoomSetting->isValid()) {
        float    zoomValue  = zoomSetting->zoomValue();
        QString  zoomText   = tr("%1%").arg(static_cast<unsigned>(zoomValue * 100.0 + 0.5));

        QAction* zoomAction = window->action("zoom_action");
        zoomAction->setData(QVariant::fromValue(zoomText));
        zoomAction->trigger();
    }

    ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
    document->addPlacementStatusNotifierReceiver(applicationStatusBar);
}


void HomeViewProxy::cursorTypeChanged(Cursor::Type newCursorType) {
    MainWindow* window = HomeViewProxy::window();

    QAction* editCutAction = window->action("edit_cut");
    QAction* editCopyAction = window->action("edit_copy");

    bool enableActions = (newCursorType == Cursor::Type::SELECTION || newCursorType == Cursor::Type::ELEMENT);
    editCutAction->setEnabled(enableActions);
    editCopyAction->setEnabled(enableActions);
}


void HomeViewProxy::contentsAreIncoherent() {
    QAction* fileExportPdfAction = window()->action("file_export_pdf");
    fileExportPdfAction->setEnabled(false);
}


void HomeViewProxy::contentsAreCoherent() {
    QAction* fileExportPdfAction = window()->action("file_export_pdf");
    fileExportPdfAction->setEnabled(true);
}


void HomeViewProxy::addTabKeyPressed() {
    MainWindow* window = HomeViewProxy::window();
    QAction*    newTabAction = window->action("window_new_tab");
    newTabAction->trigger();
}


void HomeViewProxy::backspaceKeyPressed() {
    issueCommand(new DeleteCommand(true));
}


void HomeViewProxy::closeKeyPressed() {
    MainWindow* window = HomeViewProxy::window();
    QAction*    closeTabAction = window->action("window_close_tab");
    closeTabAction->trigger();
}


void HomeViewProxy::deleteKeyPressed() {
    issueCommand(new DeleteCommand(false));
}


void HomeViewProxy::deleteToEndOfLineKeyPressed() {
    qDebug() << "deleteToEndOfLineKeyPressed()";
}


void HomeViewProxy::deleteToEndOfWordKeyPressed() {
    qDebug() << "deleteToEndOfWordKeyPressed()";
}


void HomeViewProxy::deleteToStartOfWordKeyPressed() {
    qDebug() << "deleteToStartOfWordKeyPressed()";
}


void HomeViewProxy::deleteLineKeyPressed() {
    qDebug() << "deleteLineKeyPressed()";
}


void HomeViewProxy::findNextKeyPressed() {
    bool searchEnabled = window()->action("edit_search_next");
    if (searchEnabled) {
        searchForwardClicked();
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::findPreviousKeyPressed() {
    bool searchEnabled = window()->action("edit_search_next");
    if (searchEnabled) {
        searchBackwardsClicked();
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::forwardKeyPressed() {
    qDebug() << "forwardKeyPressed()";
}


void HomeViewProxy::helpContentsKeyPressed() {
    MainWindow* window = HomeViewProxy::window();
    QAction*    helpContentsAction = window->action("help");
    helpContentsAction->trigger();
}


void HomeViewProxy::insertLineSeparatorKeyPressed() {
    qDebug() << "insertLineSeparatorKeyPressed()";
}


void HomeViewProxy::insertParagraphSeparatorKeyPressed() {
    ViewWidget*      view                 = HomeViewProxy::view();
    Ld::Capabilities requirementsAtCursor = view->requirementsAtCursor();

    if (requirementsAtCursor.intersects(Ld::Capabilities::newLine)) {
        insertTextAtCursor(QString("\n"));
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::nextChildKeyPressed() {
    MainWindow* window      = HomeViewProxy::window();
    unsigned    numberViews = window->numberViews();

    if (numberViews > 1) {
        unsigned currentView = window->currentViewIndex();

        ++currentView;
        if (currentView >= numberViews) {
            currentView = 0;
        }

        window->setCurrentView(currentView);
        restoreFocus();
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::preferencesKeyPressed() {
    MainWindow* window = HomeViewProxy::window();
    QAction*    applicationPreferencesAction = window->action("application_preferences");
    applicationPreferencesAction->trigger();
}


void HomeViewProxy::previousChildKeyPressed() {
    MainWindow* window      = HomeViewProxy::window();
    unsigned    numberViews = window->numberViews();

    if (numberViews > 1) {
        unsigned currentView = window->currentViewIndex();

        if (currentView > 0) {
            --currentView;
        } else {
            currentView = numberViews - 1;
        }

        window->setCurrentView(currentView);
        restoreFocus();
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::printKeyPressed() {
    qDebug() << "printKeyPressed()";
}


void HomeViewProxy::quitKeyPressed() {
    qDebug() << "quitKeyPressed()";
}


void HomeViewProxy::replaceKeyPressed() {
    if (currentFindAndReplaceDialog == Q_NULLPTR) {
        displayFindReplaceDialog(true);
    } else {
        currentFindAndReplaceDialog->setFindAndReplaceEnabled();
    }
}


void HomeViewProxy::deselectKeyPressed() {
    ViewWidget* view = HomeViewProxy::view();
    if (view->cursorType() == Cursor::Type::SELECTION) {
        view->updateCursorPosition(view->cursor().elementCursor());
    }
}


void HomeViewProxy::unknownKeyPressed(
        int                   keyCode,
        Qt::KeyboardModifiers modifiers,
        unsigned long         nativeModifiers,
        unsigned long         nativeScanCodes,
        unsigned long         nativeVirtualKey,
        const QString&        text
    ) {
    qDebug() << "unknownKeyPressed("
             << keyCode << ", "
             << modifiers << ", "
             << nativeModifiers << ", "
             << nativeScanCodes << ", "
             << nativeVirtualKey << ", "
             << text << ")";
}


void HomeViewProxy::whatsThisKeyPressed() {
    if (!QWhatsThis::inWhatsThisMode()) {
        QWhatsThis::enterWhatsThisMode();
    }
}


void HomeViewProxy::zoomInKeyPressed() {
    zoomIn();
}


void HomeViewProxy::zoomOutKeyPressed() {
    zoomOut();
}


void HomeViewProxy::fullScreenKeyPressed() {
    MainWindow* window = HomeViewProxy::window();
    if (window->isFullScreen()) {
        window->showNormal();
    } else {
        window->showFullScreen();
    }
}


void HomeViewProxy::cancelKeyPressed() {
    qDebug() << "cancelKeyPressed()";
}


void HomeViewProxy::textKeyPressed(
        int                   /* keyCode */,
        Qt::KeyboardModifiers /* modifiers */,
        unsigned long         /* nativeModifiers */,
        unsigned long         /* nativeScanCodes */,
        unsigned long         /* nativeVirtualKey */,
        const QString&        text
    ) {
    if (text == CommandPopupDialog::defaultPrefix) {
        manageCommandPopupWindow();
    } else {
        if (text == QString("\x08") || text == QString("\x7F")) {
            issueCommand(new DeleteCommand(true));
        } else {
            if (text != tr(",") || !processComma()) {
                ViewWidget*      view                 = HomeViewProxy::view();
                Ld::Capabilities requirementsAtCursor = view->requirementsAtCursor();
                if (Ld::SpecialCharacters::textMeetsRequirements(text, requirementsAtCursor)) {
                    insertTextAtCursor(text);
                } else {
                    invalidKeyPressed();
                }
            }
        }
    }
}


void HomeViewProxy::textKeyPressedOnPlaceholder(
        int                   /* keyCode */,
        Qt::KeyboardModifiers /* modifiers */,
        unsigned long         /* nativeModifiers */,
        unsigned long         /* nativeScanCodes */,
        unsigned long         /* nativeVirtualKey */,
        const QString&        text
    ) {
    if (text == CommandPopupDialog::defaultPrefix) {
        manageCommandPopupWindow();
    } else if (text == tr(",")) {
        processComma();
    }
}


void HomeViewProxy::rightMouseButtonClicked(const QPoint& screenPosition) {
    QMenu* rightClickMenu = window()->widget<QMenu>("right_click_menu");
    rightClickMenu->exec(screenPosition);
}


void HomeViewProxy::commandFailed(const CommandContainer& command) {
    if (command.commandType() == Command::CommandType::DELETE_CONTENT) {
        Application::beep();
    } else {
        QMessageBox::information(
            window(),
            tr("Could Not Perform Operation"),
            tr("Could not perform command \"%1\"").arg(command.description())
        );
    }
}


void HomeViewProxy::undoFailed(const CommandContainer& command) {
    QMessageBox::information(
        window(),
        tr("Could Not Perform Undo."),
        tr("Could not undo command \"%1\"").arg(command.description())
    );
}


void HomeViewProxy::redoFailed(const CommandContainer& command) {
    QMessageBox::information(
        window(),
        tr("Could Not Perform Redo."),
        tr("Could not redo command \"%1\"").arg(command.description())
    );
}


void HomeViewProxy::invalidKeyPressed() {
    Application::beep();
}


void HomeViewProxy::undoRedoStackChanged(unsigned long undoStackSize, unsigned long redoStackSize) {
    QAction* undoAction = window()->action("edit_undo");
    QAction* redoAction = window()->action("edit_redo");

    if (undoStackSize > 0 || redoStackSize > 0) {
        ViewWidget* view = HomeViewProxy::view();

        if (undoStackSize > 0) {
            QString description = view->undoRedoStackContentsAt(0).description();
            undoAction->setText(tr("Undo %1").arg(description));
            undoAction->setToolTip(tr("Undo %1").arg(description));
        } else {
            undoAction->setText(tr("Undo"));
            undoAction->setToolTip(tr("Undo"));
        }

        if (redoStackSize > 0) {
            QString description = view->undoRedoStackContentsAt(1).description();
            redoAction->setText(tr("Redo %1").arg(description));
            redoAction->setToolTip(tr("Redo %1").arg(description));
        } else {
            redoAction->setText(tr("Redo"));
            redoAction->setToolTip(tr("Redo"));
        }
    } else {
        undoAction->setText(tr("Undo"));
        undoAction->setToolTip(tr("Undo"));

        redoAction->setText(tr("Redo"));
        redoAction->setToolTip(tr("Redo"));
    }
}


void HomeViewProxy::fileImports() {
    MainWindow*                     window      = HomeViewProxy::window();
    ViewWidget*                     view        = HomeViewProxy::view();
    Document*                       document    = view->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();
    QList<Ld::RootImport>           imports     = rootElement->imports();

    ImportsDialog importsDialog(window);

    QString referenceLocation = rootElement->filename();
    importsDialog.setReferencePath(referenceLocation);
    importsDialog.setImports(imports);

    bool accepted = importsDialog.exec();

    if (accepted) {
       rootElement->setImports(importsDialog.imports());

       QList<Document*> activeDocuments = MainWindow::visibleDocuments();
       Document::purgeUnreferenced(activeDocuments);
    }
}


void HomeViewProxy::fileExportPdf() {
    MainWindow*                     window           = HomeViewProxy::window();
    ViewWidget*                     view             = HomeViewProxy::view();
    Document*                       document         = view->document();
    QSharedPointer<Ld::RootElement> rootElement      = document->element();
    unsigned                        numberPages      = rootElement->numberPages();
    QList<PageList::Index>          pagesUnderCursor = view->pagesUnderCursor();
    unsigned                        currentPage      = pagesUnderCursor.isEmpty() ? 1 : pagesUnderCursor.first() + 1;

    PdfExportDialog pdfExportDialog(window);
    pdfExportDialog.loadSettings("PdfExportDialog");

    pdfExportDialog.setWindowTitle(tr("Export PDF"));
    pdfExportDialog.setCurrentPageNumber(currentPage);
    pdfExportDialog.setNumberPages(numberPages);

    bool accepted = pdfExportDialog.exec();
    if (accepted) {
        pdfExportDialog.saveSettings("PdfExportDialog");

        QString outputFilename = pdfExportDialog.selectedFile();
        if (QFileInfo(outputFilename).suffix().isEmpty()) {
            outputFilename += ".pdf";
        }

        bool doExport = doOverwriteFile(outputFilename);

        if (doExport) {
            QPrinter printer(QPrinter::HighResolution);

            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setDocName(document->shortformName());
            printer.setCreator(Application::applicationName());
            printer.setOutputFileName(outputFilename);

            PrintingStatusDialog statusDialog(window);
            statusDialog.setWindowTitle(tr("Export PDF"));
            statusDialog.setLeaderText(tr("Exporting PDF to:\n%1").arg(pdfExportDialog.selectedFile()));

            PrintingEngine printingEngine(&printer, rootElement, pdfExportDialog.pageList());

            connect(&printingEngine, &PrintingEngine::started, &statusDialog, &PrintingStatusDialog::started);
            connect(
                &printingEngine,
                &PrintingEngine::completedPage,
                &statusDialog,
                &PrintingStatusDialog::completedPage
            );

            connect(&printingEngine, &PrintingEngine::completed, &statusDialog, &PrintingStatusDialog::completed);
            connect(&printingEngine, &PrintingEngine::aborted, &statusDialog, &PrintingStatusDialog::aborted);
            connect(&statusDialog, &PrintingStatusDialog::abort, &printingEngine, &PrintingEngine::abort);

            printingEngine.start();
            statusDialog.exec();

            if (printingEngine.status() == PrintingEngine::Status::IDLE) {
                DesktopServices::openUrl(QUrl(outputFilename));
            }
        }
    }
}


void HomeViewProxy::filePrint() {
    MainWindow*                     window           = HomeViewProxy::window();
    ViewWidget*                     view             = HomeViewProxy::view();
    Document*                       document         = view->document();
    QSharedPointer<Ld::RootElement> rootElement      = document->element();
    unsigned                        numberPages      = rootElement->numberPages();
    QList<PageList::Index>          pagesUnderCursor = view->pagesUnderCursor();
    unsigned                        currentPage      = pagesUnderCursor.isEmpty() ? 1 : pagesUnderCursor.first() + 1;

    QPageSize defaultPageSize = rootElement->defaultPageFormat()->toQPageLayout().pageSize();

    QPrinter printer;
    printer.setPageSize(defaultPageSize);
    printer.setDocName(document->shortformName());
    printer.setCreator(Application::applicationName());

    QPrintDialog printDialog(&printer, window);
    printDialog.setMinMax(1, numberPages);

    printDialog.setOption(QPrintDialog::PrintToFile, false);
    printDialog.setOption(QPrintDialog::PrintCurrentPage, true);
    printDialog.setOption(QPrintDialog::PrintPageRange, true);
    printDialog.setOption(QPrintDialog::PrintCollateCopies, true);

    bool accepted = printDialog.exec();
    if (accepted) {
        PrintingStatusDialog statusDialog(window);
        statusDialog.setWindowTitle(tr("Printing"));
        statusDialog.setAutoCloseWhenDoneEnabled();
        statusDialog.setLeaderText(tr("Printing %1 to %2").arg(document->shortformName()).arg(printer.printerName()));

        PrintingEngine printingEngine(&printer, rootElement);
        printingEngine.setCurrentPageNumber(currentPage);

        connect(&printingEngine, &PrintingEngine::started, &statusDialog, &PrintingStatusDialog::started);
        connect(
            &printingEngine,
            &PrintingEngine::completedPage,
            &statusDialog,
            &PrintingStatusDialog::completedPage
        );

        connect(&printingEngine, &PrintingEngine::completed, &statusDialog, &PrintingStatusDialog::completed);
        connect(&printingEngine, &PrintingEngine::aborted, &statusDialog, &PrintingStatusDialog::aborted);
        connect(&statusDialog, &PrintingStatusDialog::abort, &printingEngine, &PrintingEngine::abort);

        printingEngine.start();
        statusDialog.exec();
    }
}


void HomeViewProxy::editCut() {
    ViewWidget* view = HomeViewProxy::view();
    if (view->cursorType() == Cursor::Type::SELECTION || view->cursorType() == Cursor::Type::ELEMENT) {
        issueCommand(new CutCopyCommand(CutCopyCommand::Mode::CUT, Application::clipboard()));
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::editCopy() {
    ViewWidget* view = HomeViewProxy::view();
    if (view->cursorType() == Cursor::Type::SELECTION || view->cursorType() == Cursor::Type::ELEMENT) {
        issueCommand(new CutCopyCommand(CutCopyCommand::Mode::COPY, Application::clipboard()));
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::editPaste() {
    ViewWidget*  view       = HomeViewProxy::view();
    Cursor::Type cursorType = view->cursorType();

    if (cursorType == Cursor::Type::ELEMENT        ||
        cursorType == Cursor::Type::EMPTY_POSITION ||
        cursorType == Cursor::Type::SELECTION      ||
        cursorType == Cursor::Type::TEXT              ) {
        Clipboard*   clipboard  = Application::clipboard();
        if (clipboard->containsData()) {
            SelectionDataPointer clipboardSelection = clipboard->content();
            Ld::ElementCursor    elementCursor      = view->cursor().elementCursor();

            issueCommand(new PasteCommand(clipboard));
        } else {
            invalidKeyPressed();
        }
    } else {
        invalidKeyPressed();
    }
}


void HomeViewProxy::editSelectAll() {
    ViewWidget* view = HomeViewProxy::view();
    view->selectEntireDocument();
}


void HomeViewProxy::zoom() {
    ViewWidget* view        = HomeViewProxy::view();
    QAction*    zoomAction  = window()->action("zoom_action");
    QVariant    zoomSetting = zoomAction->data();
    QString     zoomText    = zoomSetting.toString();

    if (zoomText == tr("Page Width")) {

    } else if (zoomText == tr("Full Page")) {

    } else {
        int percentLocation = zoomText.indexOf(tr("%"));
        Q_ASSERT(percentLocation > 0);
        QString numericString = zoomText.left(percentLocation);

        bool          ok          = true;
        unsigned long numericZoom = numericString.toULong(&ok);
        Q_ASSERT(ok);

        view->setZoom(numericZoom / 100.0F);
    }
}


void HomeViewProxy::editFind() {
    displayFindReplaceDialog(false);
}


void HomeViewProxy::editFindAndReplace() {
    displayFindReplaceDialog(true);
}


void HomeViewProxy::caseSensitiveActionTriggered(bool nowChecked) {
    QAction* caseSensitiveSearchAction = window()->action("edit_search_case_sensitive");
    if (caseSensitiveSearchAction->isChecked() != nowChecked) {
        caseSensitiveSearchAction->setChecked(nowChecked);
    }

    if (currentFindAndReplaceDialog != Q_NULLPTR                              &&
        currentFindAndReplaceDialog->caseSensitiveModeEnabled() != nowChecked    ) {
        currentFindAndReplaceDialog->setCaseSensitiveModeEnabled(nowChecked);
    }
}


void HomeViewProxy::wholeWordsOnlyActionTriggered(bool nowChecked) {
    QAction* wholeWordsOnlyAction = window()->action("edit_search_whole_words_only");
    if (wholeWordsOnlyAction->isChecked() != nowChecked) {
        wholeWordsOnlyAction->setChecked(nowChecked);
    }

    if (currentFindAndReplaceDialog != Q_NULLPTR                          &&
        currentFindAndReplaceDialog->wholeWordModeEnabled() != nowChecked    ) {
        currentFindAndReplaceDialog->setWholeWordModeEnabled(nowChecked);
    }
}


void HomeViewProxy::regularExpressionActionTriggered(bool nowChecked) {
    MainWindow*                  window = HomeViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(HomeBuilderInitial::homeDockName);

    QAction* caseSensitiveSearchAction     = window->action("edit_search_case_sensitive");
    QAction* wholeWordsOnlyAction          = window->action("edit_search_whole_words_only");
    QAction* regularExpressionSearchAction = window->action("edit_search_regular_expression");

    caseSensitiveSearchAction->setEnabled(!nowChecked);
    wholeWordsOnlyAction->setEnabled(!nowChecked);

    if (regularExpressionSearchAction->isChecked() != nowChecked) {
        regularExpressionSearchAction->setChecked(nowChecked);
    }

    if (currentFindAndReplaceDialog != Q_NULLPTR                                  &&
        currentFindAndReplaceDialog->regularExpressionModeEnabled() != nowChecked    ) {
        currentFindAndReplaceDialog->setRegularExpressionModeEnabled(nowChecked);
    }

    EQt::SearchLineEdit* horizontalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("horizontal_search_line_edit");

    updateSearchControls(horizontalSearchLineEdit->text());
}


void HomeViewProxy::searchTextEntered() {
    bool searchEnabled = window()->action("edit_search_next");
    if (searchEnabled) {
        searchForwardClicked();
        restoreFocus();
    }
}


void HomeViewProxy::searchForwardClicked() {
    MainWindow*                  window = HomeViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(HomeBuilderInitial::homeDockName);

    EQt::SearchLineEdit* horizontalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("horizontal_search_line_edit");

    QAction* caseSensitiveSearchAction     = window->action("edit_search_case_sensitive");
    QAction* wholeWordsOnlyAction          = window->action("edit_search_whole_words_only");
    QAction* regularExpressionSearchAction = window->action("edit_search_regular_expression");

    bool caseSensitiveSearch     = caseSensitiveSearchAction->isChecked();
    bool wholeWordsOnlySearch    = wholeWordsOnlyAction->isChecked();
    bool regularExpressionSearch = regularExpressionSearchAction->isChecked();

    QString searchText = horizontalSearchLineEdit->text();
    searchForward(searchText, caseSensitiveSearch, wholeWordsOnlySearch, regularExpressionSearch);
}


void HomeViewProxy::searchBackwardsClicked() {
    MainWindow*                  window = HomeViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(HomeBuilderInitial::homeDockName);

    EQt::SearchLineEdit* horizontalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("horizontal_search_line_edit");

    QAction* caseSensitiveSearchAction     = window->action("edit_search_case_sensitive");
    QAction* wholeWordsOnlyAction          = window->action("edit_search_whole_words_only");
    QAction* regularExpressionSearchAction = window->action("edit_search_regular_expression");

    bool caseSensitiveSearch     = caseSensitiveSearchAction->isChecked();
    bool wholeWordsOnlySearch    = wholeWordsOnlyAction->isChecked();
    bool regularExpressionSearch = regularExpressionSearchAction->isChecked();

    QString searchText = horizontalSearchLineEdit->text();
    searchBackwards(searchText, caseSensitiveSearch, wholeWordsOnlySearch, regularExpressionSearch);
}


void HomeViewProxy::horizontalSearchTextEdited(const QString& newText) {
    MainWindow*                  window = HomeViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(HomeBuilderInitial::homeDockName);

    EQt::SearchLineEdit* verticalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("vertical_search_line_edit");

    verticalSearchLineEdit->setText(newText);
    updateSearchControls(newText);
    if (currentFindAndReplaceDialog != Q_NULLPTR) {
        currentFindAndReplaceDialog->setSearchText(newText);
    }

    updateSearchControls(newText);
}


void HomeViewProxy::verticalSearchTextEdited(const QString& newText) {
    MainWindow*                  window = HomeViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(HomeBuilderInitial::homeDockName);

    EQt::SearchLineEdit* horizontalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("horizontal_search_line_edit");

    horizontalSearchLineEdit->setText(newText);

    if (currentFindAndReplaceDialog != Q_NULLPTR) {
        currentFindAndReplaceDialog->setSearchText(newText);
    }

    updateSearchControls(newText);
}


void HomeViewProxy::dialogSearchTextEdited(const QString& newText) {
    MainWindow*                  window = HomeViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(HomeBuilderInitial::homeDockName);

    EQt::SearchLineEdit* horizontalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("horizontal_search_line_edit");

    horizontalSearchLineEdit->setText(newText);

    EQt::SearchLineEdit* verticalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("vertical_search_line_edit");

    verticalSearchLineEdit->setText(newText);
    updateSearchControls(newText);
}


void HomeViewProxy::searchForward(
        const QString& searchText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression
    ) {
    ViewWidget* view = HomeViewProxy::view();
    ViewWidget::SearchResult result = view->searchForward(
        searchText,
        caseSensitive,
        wholeWordsOnly,
        regularExpression
    );

    processSearchResult(result);
}


void HomeViewProxy::searchBackwards(
        const QString& searchText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression
    ) {
    ViewWidget* view = HomeViewProxy::view();
    ViewWidget::SearchResult result = view->searchBackward(
        searchText,
        caseSensitive,
        wholeWordsOnly,
        regularExpression
    );

    processSearchResult(result);
}


void HomeViewProxy::replace(
        const QString& searchText,
        const QString& replacementText,
        bool           caseSensitive,
        bool           /* wholeWordsOnly */,
        bool           regularExpression
    ) {
    ViewWidget*  view       = HomeViewProxy::view();
    Cursor::Type cursorType = view->cursorType();

    if (cursorType == Cursor::Type::SELECTION) {
        bool success;
        QString currentText = view->cursor().selectionAsText(&success);
        QString newText;

        if (success) {
            if (regularExpression) {
                QRegularExpression regex(searchText);
                success = (
                       regex.isValid()
                    && regex.match(
                           currentText,
                           0,
                           QRegularExpression::NormalMatch,
                           QRegularExpression::AnchoredMatchOption
                       ).hasMatch()
                );

                if (success) {
                    newText = currentText.replace(regex, replacementText);
                    // Issue insertion command here.
                }
            } else {
                if (caseSensitive) {
                    success = (currentText == searchText);
                } else {
                    success = (currentText.toLower() == searchText.toLower());
                }

                if (success) {
                    newText = replacementText;
                }
            }
        }

        if (success) {
            issueCommand(new InsertStringCommand(newText));
        } else {
            if (currentText.isEmpty()) {
                lowerFindAndReplaceDialog();
                QMessageBox::information(
                    window(),
                    tr("Could Not Perform Replace"),
                    tr("Could not replace\n\"%1\"\nwith:\n\"%2\"").arg(currentText).arg(replacementText)
                );
                raiseFindAndReplaceDialog();
            } else {
                lowerFindAndReplaceDialog();
                QMessageBox::information(
                    window(),
                    tr("Could Not Perform Replace"),
                    tr("Could not replace text with:\n\"%1\"").arg(replacementText)
                );
                raiseFindAndReplaceDialog();
            }
        }
    } else {
        lowerFindAndReplaceDialog();
        QMessageBox::information(
            window(),
            tr("Could Not Perform Replace"),
            tr("No text selected.")
        );
        raiseFindAndReplaceDialog();
    }
}


void HomeViewProxy::replaceAndSearchForward(
        const QString& searchText,
        const QString& replacementText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression
    ) {
    replace(searchText, replacementText, caseSensitive, wholeWordsOnly, regularExpression);
    searchForward(searchText, caseSensitive, wholeWordsOnly, regularExpression);
}


void HomeViewProxy::replaceAndSearchBackward(
        const QString& searchText,
        const QString& replacementText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression
    ) {
    replace(searchText, replacementText, caseSensitive, wholeWordsOnly, regularExpression);
    searchBackwards(searchText, caseSensitive, wholeWordsOnly, regularExpression);
}


void HomeViewProxy::findAndReplaceDialogCloseRequested() {
    MainWindow* window = HomeViewProxy::window();

    QAction* editFindDialog     = window->action("edit_find_dialog");
    QAction* editFindAndReplace = window->action("edit_find_and_replace_dialog");

    editFindDialog->setEnabled(true);
    editFindAndReplace->setEnabled(true);

    currentFindAndReplaceDialog->deleteLater();
    currentFindAndReplaceDialog = Q_NULLPTR;
}


void HomeViewProxy::zoomIn() {
    ViewWidget* view        = HomeViewProxy::view();
    unsigned    currentZoom = static_cast<unsigned>(view->zoom() * 100.0 + 0.5);

    emit zoomInRequested(currentZoom);
}


void HomeViewProxy::zoomOut() {
    ViewWidget* view        = HomeViewProxy::view();
    unsigned    currentZoom = static_cast<unsigned>(view->zoom() * 100.0 + 0.5);

    emit zoomOutRequested(currentZoom);
}


void HomeViewProxy::shortformNameChanged(const QString& newShortformName) {
    MainWindow* window = HomeViewProxy::window();
    ViewWidget* view   = HomeViewProxy::view();

    int index = window->tabIndex(view);
    Q_ASSERT(index >= 0);

    window->setTabText(index, newShortformName);
}


void HomeViewProxy::titleChanged(const QString& newTitle) {
    QString windowTitle = tr("%1 - %2").arg(EQt::ProgrammaticApplication::applicationName()).arg(newTitle);
    emit setMainWindowTitle(windowTitle);
}


void HomeViewProxy::documentModified(bool nowModified) {
    MainWindow* window = HomeViewProxy::window();
    ViewWidget* view   = HomeViewProxy::view();

    int index = window->tabIndex(view);
    Q_ASSERT(index >= 0);

    if (nowModified) {
        window->setTabIcon(index, EQt::ProgrammaticApplication::icon("file_modified"));
    } else {
        window->clearTabIcon(index);
    }
}


void HomeViewProxy::documentPreferencesSelected() {
    DocumentPreferencesDialog documentPreferencesDialog(window());

    Document*                       document    = view()->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    documentPreferencesDialog.setPageFormat(rootElement->defaultPageFormat());

    QSharedPointer<Ld::CharacterFormat> defaultTextFormat           = rootElement->defaultTextFormat();
    QSharedPointer<Ld::CharacterFormat> defaultMathTextFormat       = rootElement->defaultMathTextFormat();
    QSharedPointer<Ld::CharacterFormat> defaultMathIdentifierFormat = rootElement->defaultMathIdentifierFormat();
    QSharedPointer<Ld::CharacterFormat> defaultMathFunctionFormat   = rootElement->defaultMathFunctionFormat();

    QSharedPointer<Ld::BraceConditionalFormat>
        defaultBraceConditionalFormat = rootElement->defaultBraceConditionalFormat();

    Ld::RootElement::RngType rngType         = rootElement->rngType();
    bool                     useSeedAsPreset = rootElement->rngSeedIsPreset();

    Ld::RootElement::RngSeed rngSeed;
    std::memcpy(rngSeed, rootElement->rngSeed(), sizeof(Ld::RootElement::RngSeed));

    documentPreferencesDialog.setTextFont(defaultTextFormat->toQFont());
    documentPreferencesDialog.setMathFont(defaultMathTextFormat->toQFont());
    documentPreferencesDialog.setMathIdentifierFont(defaultMathIdentifierFormat->toQFont());
    documentPreferencesDialog.setMathFunctionFont(defaultMathFunctionFormat->toQFont());

    documentPreferencesDialog.setBraceConditionalFont(defaultBraceConditionalFormat->toQFont());

    documentPreferencesDialog.setRngType(rngType);
    documentPreferencesDialog.setRngSeed(rngSeed);
    documentPreferencesDialog.setUseSeedAsPreset(useSeedAsPreset);

    QString preambleString = defaultBraceConditionalFormat->conditionPreambleString();
    documentPreferencesDialog.setBraceConditionalPreambleString(preambleString);

    QString elseString = defaultBraceConditionalFormat->elseConditionString();
    documentPreferencesDialog.setBraceConditionalElseString(elseString);

    bool braceConditionalElseClauseShown = rootElement->defaultBraceConditionalElseClauseShown();
    documentPreferencesDialog.setBraceConditionalElseClauseIncluded(braceConditionalElseClauseShown);

    connect(
        &documentPreferencesDialog,
        &DocumentPreferencesDialog::applyClicked,
        this,
        &HomeViewProxy::applyDocumentPreferencesDialogUpdates
    );

    int result = documentPreferencesDialog.exec();

    if (result == DocumentPreferencesDialog::Accepted) {
        applyDocumentPreferencesDialogUpdates(&documentPreferencesDialog);
    }
}


void HomeViewProxy::applyDocumentPreferencesDialogUpdates(DocumentPreferencesDialog* dialog) {
    Document*                       document    = view()->document();
    QSharedPointer<Ld::RootElement> rootElement = document->element();

    QSharedPointer<Ld::CharacterFormat> defaultTextFormat           = rootElement->defaultTextFormat();
    QSharedPointer<Ld::CharacterFormat> defaultMathTextFormat       = rootElement->defaultMathTextFormat();
    QSharedPointer<Ld::CharacterFormat> defaultMathIdentifierFormat = rootElement->defaultMathIdentifierFormat();
    QSharedPointer<Ld::CharacterFormat> defaultMathFunctionFormat   = rootElement->defaultMathFunctionFormat();

    if (dialog->pageFormatChanged()) {
        rootElement->setDefaultPageFormat(dialog->pageFormat());
    }

    if (dialog->textFontChanged()) {
        QFont newFont = dialog->textFont();
        QSharedPointer<Ld::CharacterFormat> newFormat = defaultTextFormat->clone()
                                                        .dynamicCast<Ld::CharacterFormat>();

        newFormat->setFamily(newFont.family());
        newFormat->setFontSize(newFont.pointSize());
        newFormat->setItalics(newFont.italic());
        newFormat->setFontWeight(static_cast<Ld::CharacterFormat::Weight>(newFont.weight()));

        rootElement->setDefaultTextFormat(newFormat);
    }

    if (dialog->mathFontChanged()) {
        QFont newFont = dialog->mathFont();
        QSharedPointer<Ld::CharacterFormat> newFormat = defaultMathTextFormat->clone()
                                                        .dynamicCast<Ld::CharacterFormat>();

        newFormat->setFamily(newFont.family());
        newFormat->setFontSize(newFont.pointSize());
        newFormat->setItalics(newFont.italic());
        newFormat->setFontWeight(static_cast<Ld::CharacterFormat::Weight>(newFont.weight()));

        rootElement->setDefaultMathTextFormat(newFormat);
    }

    if (dialog->mathIdentifierFontChanged()) {
        QFont newFont = dialog->mathIdentifierFont();
        QSharedPointer<Ld::CharacterFormat> newFormat = defaultMathIdentifierFormat->clone()
                                                        .dynamicCast<Ld::CharacterFormat>();

        newFormat->setFamily(newFont.family());
        newFormat->setFontSize(newFont.pointSize());
        newFormat->setItalics(newFont.italic());
        newFormat->setFontWeight(static_cast<Ld::CharacterFormat::Weight>(newFont.weight()));

        rootElement->setDefaultMathIdentifierFormat(newFormat);
    }

    if (dialog->mathFunctionFontChanged()) {
        QFont newFont = dialog->mathFunctionFont();
        QSharedPointer<Ld::CharacterFormat> newFormat = defaultMathFunctionFormat->clone()
                                                        .dynamicCast<Ld::CharacterFormat>();

        newFormat->setFamily(newFont.family());
        newFormat->setFontSize(newFont.pointSize());
        newFormat->setItalics(newFont.italic());
        newFormat->setFontWeight(static_cast<Ld::CharacterFormat::Weight>(newFont.weight()));

        rootElement->setDefaultMathFunctionFormat(newFormat);
    }

    bool braceConditionalElseClauseIncluded = dialog->braceConditionalElseClauseIncluded();
    if (dialog->braceConditionalFontChanged()                                                       ||
        braceConditionalElseClauseIncluded != rootElement->defaultBraceConditionalElseClauseShown()    ) {
        QFont newFont = dialog->braceConditionalFont();
        QSharedPointer<Ld::BraceConditionalFormat>
            newFormat = Ld::Format::create(Ld::BraceConditionalFormat::formatName)
                        .dynamicCast<Ld::BraceConditionalFormat>();

        newFormat->setFamily(newFont.family());
        newFormat->setFontSize(newFont.pointSize());
        newFormat->setItalics(newFont.italic());
        newFormat->setFontWeight(static_cast<Ld::BraceConditionalFormat::Weight>(newFont.weight()));
        newFormat->setConditionPreambleString(dialog->braceConditionalPreambleString());
        newFormat->setElseConditionString(dialog->braceConditionalElseString());

        rootElement->setDefaultBraceConditionalFormat(newFormat);
        rootElement->setDefaultBraceConditionalElseClauseShown(braceConditionalElseClauseIncluded);
    }

    Ld::RootElement::RngType rngType         = dialog->rngType();
    bool                     useSeedAsPreset = dialog->useSeedAsPreset();

    Ld::RootElement::RngSeed rngSeed;
    std::memcpy(rngSeed, dialog->rngSeed(), sizeof(Ld::RootElement::RngSeed));

    rootElement->setRngType(rngType);
    rootElement->setRngSeedIsPreset(useSeedAsPreset);
    rootElement->setRngSeed(rngSeed);
}


void HomeViewProxy::connectSignals() {
    MainWindow*                  window   = HomeViewProxy::window();
    ViewWidget*                  view     = HomeViewProxy::view();
    EQt::ProgrammaticDockWidget* dock     = window->dockWidget(HomeBuilderInitial::homeDockName);
    Document*                    document = view->document();

    ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
    document->addPlacementStatusNotifierReceiver(applicationStatusBar);
    applicationStatusBar->renderingContextChanged();

    sessionConnect(view, &ViewWidget::cursorTypeChanged, &HomeViewProxy::cursorTypeChanged);

    QAction* fileSaveAction = window->action("file_save");
    sessionConnect(
        view,
        static_cast<void (ViewWidget::*)(bool)>(&ViewWidget::documentModified),
        fileSaveAction,
        &QAction::setEnabled
    );
    fileSaveAction->setEnabled(view->isModified());

    sessionConnect(window->action("file_imports_dialog"), &QAction::triggered, &HomeViewProxy::fileImports);

    sessionConnect(window->action("file_export_pdf"), &QAction::triggered, &HomeViewProxy::fileExportPdf);
    sessionConnect(window->action("file_print_dialog"), &QAction::triggered, &HomeViewProxy::filePrint);

    QAction* undoAction = window->action("edit_undo");
    sessionConnect(undoAction, &QAction::triggered, view, &ViewWidget::undo);
    sessionConnect(view, &ViewWidget::undoAvailable, undoAction, &QAction::setEnabled);
    undoAction->setEnabled(view->canUndo());

    QAction* redoAction = window->action("edit_redo");
    sessionConnect(redoAction, &QAction::triggered, view, &ViewWidget::redo);
    sessionConnect(view, &ViewWidget::redoAvailable, redoAction, &QAction::setEnabled);
    redoAction->setEnabled(view->canRedo());

    sessionConnect(view, &ViewWidget::commandFailed, &HomeViewProxy::commandFailed);
    sessionConnect(view, &ViewWidget::undoFailed, &HomeViewProxy::undoFailed);
    sessionConnect(view, &ViewWidget::redoFailed, &HomeViewProxy::redoFailed);
    sessionConnect(view, &ViewWidget::undoRedoStackChanged, &HomeViewProxy::undoRedoStackChanged);

    sessionConnect(view, &ViewWidget::contentsAreIncoherent, &HomeViewProxy::contentsAreIncoherent);
    sessionConnect(view, &ViewWidget::contentsAreCoherent, &HomeViewProxy::contentsAreCoherent);
    window->action("file_export_pdf")->setEnabled(view->isDisplayCoherent());

    QAction* editCutAction = window->action("edit_cut");
    QAction* editCopyAction = window->action("edit_copy");
    QAction* editPasteAction = window->action("edit_paste");

    sessionConnect(editCutAction, &QAction::triggered, &HomeViewProxy::editCut);
    sessionConnect(editCopyAction, &QAction::triggered, &HomeViewProxy::editCopy);
    sessionConnect(editPasteAction, &QAction::triggered, &HomeViewProxy::editPaste);

    bool selectionActive = (view->cursorType() == Cursor::Type::SELECTION);
    editCutAction->setEnabled(selectionActive);
    editCopyAction->setEnabled(selectionActive);
    // TODO: Enable/disable paste action

    sessionConnect(window->action("edit_select_all"), &QAction::triggered, &HomeViewProxy::editSelectAll);

    sessionConnect(window->action("edit_find_dialog"), &QAction::triggered, &HomeViewProxy::editFind);
    sessionConnect(
        window->action("edit_find_and_replace_dialog"),
        &QAction::triggered,
        &HomeViewProxy::editFindAndReplace
    );
    sessionConnect(
        window->action("edit_search_case_sensitive"),
        &QAction::triggered,
        &HomeViewProxy::caseSensitiveActionTriggered
    );
    sessionConnect(
        window->action("edit_search_whole_words_only"),
        &QAction::triggered,
        &HomeViewProxy::wholeWordsOnlyActionTriggered
    );
    sessionConnect(
        window->action("edit_search_regular_expression"),
        &QAction::triggered,
        &HomeViewProxy::regularExpressionActionTriggered
    );
    sessionConnect(
        window->action("edit_search_next"),
        &QAction::triggered,
        &HomeViewProxy::searchForwardClicked
    );
    sessionConnect(
        window->action("edit_search_previous"),
        &QAction::triggered,
        &HomeViewProxy::searchBackwardsClicked
    );

    EQt::SearchLineEdit* horizontalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("horizontal_search_line_edit");

    sessionConnect(
        horizontalSearchLineEdit,
        &EQt::SearchLineEdit::returnPressed,
        &HomeViewProxy::searchTextEntered
    );
    sessionConnect(
        horizontalSearchLineEdit,
        &EQt::SearchLineEdit::textEdited,
        &HomeViewProxy::horizontalSearchTextEdited
    );

    EQt::SearchLineEdit* verticalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("vertical_search_line_edit");

    sessionConnect(
        verticalSearchLineEdit,
        &EQt::SearchLineEdit::returnPressed,
        &HomeViewProxy::searchTextEntered
    );
    sessionConnect(
        verticalSearchLineEdit,
        &EQt::SearchLineEdit::textEdited,
        &HomeViewProxy::verticalSearchTextEdited
    );

    // Deal with find and replace

    QString zoomText = tr("%1%").arg(static_cast<unsigned>(view->zoom() * 100.0 + 0.5));
    window->action("zoom_action")->setData(zoomText);
    window->action("zoom_action")->trigger();

    sessionConnect(window->action("zoom_action"), &QAction::triggered, &HomeViewProxy::zoom);
    sessionConnect(window->action("view_zoom_in"), &QAction::triggered, &HomeViewProxy::zoomIn);
    sessionConnect(window->action("view_zoom_out"), &QAction::triggered, &HomeViewProxy::zoomOut);

    EQt::ZoomComboBox* horizontalZoomComboBox =
        dock->EQt::ProgrammaticWidget::widget<EQt::ZoomComboBox>("horizontal_zoom_combo_box");

    sessionConnect(
        this,
        &HomeViewProxy::zoomInRequested,
        horizontalZoomComboBox,
        static_cast<void (EQt::ZoomComboBox::*)(unsigned)>(&EQt::ZoomComboBox::zoomIn)
    );
    sessionConnect(
        this,
        &HomeViewProxy::zoomOutRequested,
        horizontalZoomComboBox,
        static_cast<void (EQt::ZoomComboBox::*)(unsigned)>(&EQt::ZoomComboBox::zoomOut)
    );

    EQt::ZoomComboBox* verticalZoomComboBox =
        dock->EQt::ProgrammaticWidget::widget<EQt::ZoomComboBox>("vertical_zoom_combo_box");

    sessionConnect(
        this,
        &HomeViewProxy::zoomInRequested,
        verticalZoomComboBox,
        static_cast<void (EQt::ZoomComboBox::*)(unsigned)>(&EQt::ZoomComboBox::zoomIn)
    );
    sessionConnect(
        this,
        &HomeViewProxy::zoomOutRequested,
        verticalZoomComboBox,
        static_cast<void (EQt::ZoomComboBox::*)(unsigned)>(&EQt::ZoomComboBox::zoomOut)
    );
    // TODO: Set zoom combo box value.

    QAction* viewMarginsAction = window->action("view_margins");
    sessionConnect(
        viewMarginsAction,
        static_cast<void (QAction::*)(bool)>(&QAction::triggered),
        view,
        static_cast<void (ViewWidget::*)(bool)>(&ViewWidget::setMarginsEnabled)
    );
    viewMarginsAction->setChecked(view->marginsEnabled());

    QAction* viewGuidesAction = window->action("view_guides");
    sessionConnect(
        viewGuidesAction,
        static_cast<void (QAction::*)(bool)>(&QAction::triggered),
        view,
        static_cast<void (ViewWidget::*)(bool)>(&ViewWidget::setGuidesEnabled)
    );
    viewGuidesAction->setChecked(view->guidesEnabled());

    QAction* documentPreferencesAction = window->action("document_preferences");
    sessionConnect(documentPreferencesAction, &QAction::triggered, &HomeViewProxy::documentPreferencesSelected);

    sessionConnect(this, &HomeViewProxy::setMainWindowTitle, window, &MainWindow::setWindowTitle);
    titleChanged(view->title());

    sessionConnect(view, &ViewWidget::requirementsChanged, window, &MainWindow::requirementsChanged);
    window->requirementsChanged(view->requirementsAtCursor());
}


void HomeViewProxy::disconnectSignals() {
    MainWindow* window   = HomeViewProxy::window();
    ViewWidget* view     = HomeViewProxy::view();
    Document*   document = view->document();

    ApplicationStatusBar* applicationStatusBar = window->widget<ApplicationStatusBar>("application_status_bar");
    document->removePlacementStatusNotifierReceiver(applicationStatusBar);

    sessionDisconnectAll();
}


void HomeViewProxy::issueCommand(Command* newCommand) {
    ViewWidget* view  = HomeViewProxy::view();
    view->insertCommand(newCommand);
}


void HomeViewProxy::insertTextAtCursor(const QString& newText) {
    ViewWidget*              view                    = HomeViewProxy::view();
    const Ld::ElementCursor& elementCursor           = view->cursor().elementCursor();
    Ld::FormatPointer        nextTextInsertionFormat = view->nextTextInsertionFormat(elementCursor);
    Command*                 newCommand              = Q_NULLPTR;

    if (!nextTextInsertionFormat.isNull() && view->cursorType() == Cursor::Type::TEXT) {
        Ld::ElementPointer currentElement = elementCursor.element();
        Q_ASSERT(!currentElement.isNull());

        const Fixer* fixer = Fixer::fixer(currentElement->typeName());
        if (fixer != Q_NULLPTR                                                                      &&
            fixer->isSplitAllowed(currentElement, Fixer::SplitReason::FORMAT_CHANGE, elementCursor)    ) {
            Ld::ElementPointer newElement = Ld::Element::create(currentElement->typeName());
            newElement->setFormat(nextTextInsertionFormat);
            newElement->setText(newText);

            newCommand = new InsertElementCommand(
                newElement,
                InsertElementCommand::InsertionLocation::AT_CURSOR,
                InsertElementCommand::FinalCursorPosition::ELEMENT_END_OF_TEXT
            );

            view->clearFormatForNextTextInsertion();
        }
    }

    if (newCommand == Q_NULLPTR) {
        newCommand = new InsertStringCommand(newText);
    }

    issueCommand(newCommand);
}


void HomeViewProxy::manageCommandPopupWindow() {
    ViewWidget*  view       = HomeViewProxy::view();
    Cursor::Type cursorType = view->cursorType();

    if (cursorType == Cursor::Type::EMPTY_POSITION ||
        cursorType == Cursor::Type::TEXT           ||
        cursorType == Cursor::Type::SELECTION      ||
        cursorType == Cursor::Type::ELEMENT           ) {
        QFont fontAtPosition = Application::font();

        if (cursorType == Cursor::Type::TEXT) {
            const Ld::ElementCursor& elementCursor = view->cursor().elementCursor();
            Ld::ElementPointer       element       = elementCursor.element();
            Q_ASSERT(!element.isNull());

            Ld::FormatPointer format = element->format();
            if (!format.isNull() && format->capabilities().contains(Ld::CharacterFormat::formatName)) {
                QSharedPointer<Ld::CharacterFormat> characterFormat = format.dynamicCast<Ld::CharacterFormat>();
                fontAtPosition = characterFormat->toQFont();
            }
        }

        QPoint      globalPosition = view->globalCursorPosition();
        MainWindow* window         = HomeViewProxy::window();
        window->displayCommandPopupDialog(globalPosition, fontAtPosition);
    }
}


void HomeViewProxy::displayFindReplaceDialog(bool showReplaceOptions) {
    MainWindow* window = HomeViewProxy::window();
    currentFindAndReplaceDialog = new FindAndReplaceDialog(window);

    currentFindAndReplaceDialog->setWindowFlags(
        currentFindAndReplaceDialog->windowFlags() | Qt::WindowStaysOnTopHint //| Qt::X11BypassWindowManagerHint
    );

    QAction* caseSensitiveSearchAction     = window->action("edit_search_case_sensitive");
    QAction* wholeWordsOnlyAction          = window->action("edit_search_whole_words_only");
    QAction* regularExpressionSearchAction = window->action("edit_search_regular_expression");
    QAction* editSearch                    = window->action("edit_search");

    currentFindAndReplaceDialog->setCaseSensitiveModeEnabled(caseSensitiveSearchAction->isChecked());
    currentFindAndReplaceDialog->setWholeWordModeEnabled(wholeWordsOnlyAction->isChecked());
    currentFindAndReplaceDialog->setRegularExpressionModeEnabled(regularExpressionSearchAction->isChecked());
    currentFindAndReplaceDialog->setSearchText(editSearch->data().toString());

    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::searchTextEdited,
        this,
        &HomeViewProxy::dialogSearchTextEdited
    );
    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::caseSensitiveCheckboxClicked,
        this,
        &HomeViewProxy::caseSensitiveActionTriggered
    );
    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::wholeWordsOnlyCheckboxClicked,
        this,
        &HomeViewProxy::wholeWordsOnlyActionTriggered
    );
    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::regularExpressionCheckboxClicked,
        this,
        &HomeViewProxy::regularExpressionActionTriggered
    );

    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::searchForward,
        this,
        &HomeViewProxy::searchForward
    );

    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::searchBackwards,
        this,
        &HomeViewProxy::searchBackwards
    );

    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::replaceAndSearchForward,
        this,
        &HomeViewProxy::replaceAndSearchForward
    );

    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::replace,
        this,
        &HomeViewProxy::replace
    );

    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::replaceAndSearchBackward,
        this,
        &HomeViewProxy::replaceAndSearchBackward
    );

    connect(
        currentFindAndReplaceDialog,
        &FindAndReplaceDialog::closeRequested,
        this,
        &HomeViewProxy::findAndReplaceDialogCloseRequested
    );

    currentFindAndReplaceDialog->setFindAndReplaceEnabled(showReplaceOptions);
    currentFindAndReplaceDialog->show();

    QAction* editFindDialog     = window->action("edit_find_dialog");
    QAction* editFindAndReplace = window->action("edit_find_and_replace_dialog");

    editFindDialog->setEnabled(false);
    editFindAndReplace->setEnabled(false);
}


void HomeViewProxy::lowerFindAndReplaceDialog() {
    if (currentFindAndReplaceDialog != Q_NULLPTR) {
        currentFindAndReplaceDialog->hide();
    }
}


void HomeViewProxy::raiseFindAndReplaceDialog() {
    if (currentFindAndReplaceDialog != Q_NULLPTR) {
        currentFindAndReplaceDialog->show();
    }
}


void HomeViewProxy::updateSearchControls(const QString& newText) {
    bool enableControls;

    MainWindow*                  window = HomeViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(HomeBuilderInitial::homeDockName);

    EQt::SearchLineEdit* verticalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("vertical_search_line_edit");

    EQt::SearchLineEdit* horizontalSearchLineEdit =
        dock->EQt::ProgrammaticWidget::widget<EQt::SearchLineEdit>("horizontal_search_line_edit");

    QPalette palette;

    QAction* regularExpressionSearchAction = window->action("edit_search_regular_expression");
    QString  searchToolTip = tr("Locate text");
    if (regularExpressionSearchAction->isChecked()) {
        QRegularExpression regularExpression(newText);
        enableControls = regularExpression.isValid();

        if (!enableControls) {
            palette.setColor(QPalette::Text, Qt::red);
            searchToolTip = regularExpression.errorString();
        } else {
            palette.setColor(QPalette::Text, Qt::black);
        }
    } else {
        enableControls = !newText.isEmpty();
        palette.setColor(QPalette::Text, Qt::black);
    }

    window->action("edit_search")->setToolTip(searchToolTip);

    horizontalSearchLineEdit->setPalette(palette);
    horizontalSearchLineEdit->setToolTip(searchToolTip);

    verticalSearchLineEdit->setPalette(palette);
    verticalSearchLineEdit->setToolTip(searchToolTip);

    window->action("edit_search_next")->setEnabled(enableControls);
    window->action("edit_search_previous")->setEnabled(enableControls);
}


void HomeViewProxy::processSearchResult(ViewWidget::SearchResult searchResult) {
    MainWindow* window = HomeViewProxy::window();

    switch (searchResult) {
        case ViewWidget::SearchResult::FOUND: {
            break;
        }

        case ViewWidget::SearchResult::NOT_FOUND: {
            QMessageBox::information(window, tr("Not Found"), tr("Could not find the requested text."));
            break;
        }

        case ViewWidget::SearchResult::INVALID_STARTING_POSITION: {
            QMessageBox::information(window, tr("Invalid Cursor Position"), tr("No starting cursor position."));
            break;
        }

        case ViewWidget::SearchResult::INVALID_REGULAR_EXPRESSION: {
            QMessageBox::information(
                window,
                tr("Invalid Regular Expression"),
                tr("The regular expression is invalid.")
            );
            break;
        }

        default: {
            Q_ASSERT(false); // Unexpected value.
            break;
        }
    }
}


void HomeViewProxy::restoreFocus() {
    view()->restoreFocus();
}


bool HomeViewProxy::doOverwriteFile(const QString& outputFilename) {
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


bool HomeViewProxy::processComma() {
    bool specialActionTaken = false;

    const Cursor& cursor     = view()->cursor();
    Cursor::Type  cursorType = cursor.cursorType();
    if (cursorType == Cursor::Type::TEXT || cursorType == Cursor::Type::ELEMENT) {
        const Ld::ElementCursor& elementCursor = cursor.elementCursor();
        Ld::ElementPointer       element       = elementCursor.element();

        if (element.isNull() || element->typeName() != Ld::TextElement::elementName) {
            Ld::ElementPointer parent          = elementCursor.parent();
            bool               notFirstPosition = (
                   cursorType == Cursor::Type::TEXT
                && (elementCursor.textIndex() != 0 || elementCursor.regionIndex() != 0)
            );

            while (!parent.isNull() && !parent->isListElement()) {
                element = parent;
                parent  = element->parent();
                if (parent.isNull() || parent->indexOfChild(element) != 0) {
                    notFirstPosition = true;
                }
            }

            if (!parent.isNull() && !element.isNull()) {
                Ld::ElementPointer                      placeholder;
                InsertElementCommand::InsertionLocation insertionLocation;

                QString typeName = element->typeName();
                if (typeName == Ld::FunctionPlaceholderElement::elementName) {
                    QSharedPointer<Ld::FunctionPlaceholderElement>
                        functionPlaceholder = Ld::Element::create(Ld::FunctionPlaceholderElement::elementName)
                                              .dynamicCast<Ld::FunctionPlaceholderElement>();

                    functionPlaceholder->setFieldString(tr("?"));

                    if (notFirstPosition) {
                        unsigned long indexOfElement = parent->indexOfChild(element);
                        if (indexOfElement == (parent->numberChildren() - 1)) {
                            insertionLocation = InsertElementCommand::InsertionLocation::AT_CURSOR;
                        } else {
                            insertionLocation = InsertElementCommand::InsertionLocation::AFTER_CURSOR;
                        }
                    } else {
                        insertionLocation = InsertElementCommand::InsertionLocation::AT_CURSOR;
                    }

                    placeholder = functionPlaceholder;
                } else {
                    placeholder = Ld::Element::create(Ld::PlaceholderElement::elementName);

                    if (element->isPlaceholder()) {
                        if (notFirstPosition && typeName != Ld::ListPlaceholderElement::elementName) {
                            insertionLocation = InsertElementCommand::InsertionLocation::AFTER_CURSOR;
                        } else {
                            insertionLocation = InsertElementCommand::InsertionLocation::AT_CURSOR;
                        }
                    } else {
                        if (notFirstPosition) {
                            insertionLocation = InsertElementCommand::InsertionLocation::AFTER_CURSOR;
                        } else {
                            insertionLocation = InsertElementCommand::InsertionLocation::AT_CURSOR;
                        }
                    }
                }

                ViewWidget*           view    = HomeViewProxy::view();
                InsertElementCommand* command = new InsertElementCommand(
                    placeholder,
                    insertionLocation,
                    InsertElementCommand::FinalCursorPosition::ELEMENT_WHOLE
                );

                view->updateCursorPosition(Ld::ElementCursor(element));
                view->insertCommand(command);

                specialActionTaken = true;
            }
        }
    }

    return specialActionTaken;
}
