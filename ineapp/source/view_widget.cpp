/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ViewWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QAction>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QScrollBar>
#include <QSize>
#include <QMessageBox>
#include <QPalette>
#include <QTimer>
#include <QResizeEvent>
#include <QList>
#include <QSharedPointer>

#include <eqt_programmatic_view.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_aggregations_by_capability.h>
#include <ld_page_format.h>
#include <ld_capabilities.h>

#include "main_window.h"
#include "editor.h"
#include "cursor.h"
#include "document.h"
#include "page_list.h"
#include "command.h"
#include "command_container.h"
#include "insert_string_command.h"
#include "view_proxy.h"
#include "view_widget.h"

ViewWidget::ViewWidget(MainWindow* mainWindow, QWidget* parent):EQt::ProgrammaticView(mainWindow, parent) {
    configureWidget();
    runBuilders();
}


ViewWidget::ViewWidget(
        Document*   newDocument,
        MainWindow* mainWindow,
        QWidget*    parent
    ):ProgrammaticView(
        mainWindow,
        parent
    ) {
    configureWidget(newDocument);
    runBuilders();
}


ViewWidget::~ViewWidget() {}


bool ViewWidget::isActiveView() const {
    MainWindow* mainWindow = window();
    return (mainWindow->currentView() == this);
}


MainWindow* ViewWidget::window() const {
    return dynamic_cast<MainWindow*>(ProgrammaticView::window());
}


void ViewWidget::setDocument(Document* newDocument) {
    widget<Editor>("editor")->setDocument(newDocument);
}


Document* ViewWidget::document() const {
    return widget<Editor>("editor")->document();
}


QString ViewWidget::filename() const {
    return document()->filename();
}


QString ViewWidget::shortformName() const {
    return document()->shortformName();
}


QString ViewWidget::title() const {
    QString result;

    QString filename = document()->filename();
    if (filename.isEmpty()) {
        result = document()->shortformName();
    } else {
        QFileInfo fileInformation(filename);
        result = fileInformation.baseName();
    }

    return result;
}


QString ViewWidget::identifier() const {
    return document()->identifier();
}


bool ViewWidget::isModified() const {
    return document()->isModified();
}


bool ViewWidget::isReplaceable() const {
    return document()->isReplaceable();
}


unsigned long ViewWidget::undoStackSize() const {
    return document()->undoStackSize();
}


unsigned long ViewWidget::redoStackSize() const {
    return document()->redoStackSize();
}


bool ViewWidget::canUndo() const {
    return document()->canUndo();
}


bool ViewWidget::canRedo() const {
    return document()->canRedo();
}


const CommandContainer& ViewWidget::undoRedoStackContentsAt(long index) const {
    return document()->undoRedoStackContentsAt(index);
}


float ViewWidget::zoom() const {
    Editor* editor = widget<Editor>("editor");
    return editor->zoom();
}


bool ViewWidget::marginsEnabled() const {
    return widget<Editor>("editor")->marginsEnabled();
}


bool ViewWidget::guidesEnabled() const {
    return widget<Editor>("editor")->guidesEnabled();
}


const Cursor& ViewWidget::cursor() const {
    return widget<Editor>("editor")->cursor();
}


void ViewWidget::updateCursorPosition(const Ld::ElementCursor& elementCursor) {
    widget<Editor>("editor")->updateCursorPosition(elementCursor);
}


Ld::Capabilities ViewWidget::requirementsAtCursor() const {
    return widget<Editor>("editor")->requirementsAtCursor();
}


Cursor::Type ViewWidget::cursorType() const {
    return widget<Editor>("editor")->cursorType();
}


void ViewWidget::selectEntireDocument() {
    widget<Editor>("editor")->selectEntireDocument();
}


Ld::ElementPointerList ViewWidget::elementsUnderCursor(bool includeAncestors) const {
    return widget<Editor>("editor")->elementsUnderCursor(includeAncestors);
}


QList<Ld::FormatPointer> ViewWidget::formatsAtCursor() const {
    return widget<Editor>("editor")->formatsAtCursor();
}


Ld::AggregationsByCapability ViewWidget::aggregationsByCapabilityWithAncestors() const {
    if (currentAggregationsByCapabilityWithAncestors.isEmpty()) {
        generateAggregationsByCapability();
    }

    return currentAggregationsByCapabilityWithAncestors;
}


Ld::AggregationsByCapability ViewWidget::aggregationsByCapabilityWithoutAncestors() const {
    if (currentAggregationsByCapabilityWithoutAncestors.isEmpty()) {
        generateAggregationsByCapability();
    }

    return currentAggregationsByCapabilityWithoutAncestors;
}


const QList<PageList::Index>& ViewWidget::pagesUnderCursor() const {
    if (currentPagesUnderCursor.isEmpty()) {
        currentPagesUnderCursor = widget<Editor>("editor")->pagesUnderCursor();
    }

    return currentPagesUnderCursor;
}


QPoint ViewWidget::globalCursorPosition() const {
    return widget<Editor>("editor")->globalCursorPosition();
}


QSharedPointer<Ld::PageFormat> ViewWidget::pageFormat(PageList::Index pageIndex) const {
    return widget<Editor>("editor")->pageFormat(pageIndex);
}


void ViewWidget::setFormatForNextTextInsertion(Ld::FormatPointer newFormat) {
    widget<Editor>("editor")->setFormatForNextTextInsertion(newFormat);
}


void ViewWidget::clearFormatForNextTextInsertion() {
    widget<Editor>("editor")->clearFormatForNextTextInsertion();
}


Ld::FormatPointer ViewWidget::nextTextInsertionFormat(const Ld::ElementCursor& elementCursor) const {
    return widget<Editor>("editor")->nextTextInsertionFormat(elementCursor);
}


void ViewWidget::aboutToSave(Document* document) {
    ProxyList proxies = ViewWidget::proxies();
    for (ProxyList::const_iterator it=proxies.constBegin(),end=proxies.constEnd() ; it!=end ; ++it) {
        ViewProxy* proxy = dynamic_cast<ViewProxy*>(*it);
        if (proxy != Q_NULLPTR) {
            proxy->aboutToSave(document);
        }
    }
}


void ViewWidget::loaded(Document* document) {
    ProxyList proxies = ViewWidget::proxies();
    for (ProxyList::const_iterator it=proxies.constBegin(),end=proxies.constEnd() ; it!=end ; ++it) {
        ViewProxy* proxy = dynamic_cast<ViewProxy*>(*it);
        if (proxy != Q_NULLPTR) {
            proxy->loaded(document);
        }
    }
}


QSharedPointer<Ld::DocumentSetting> ViewWidget::documentSettingByName(const QString& settingName) const {
    return widget<Editor>("editor")->documentSettingByName(settingName);
}


void ViewWidget::addDocumentSetting(QSharedPointer<Ld::DocumentSetting> newSetting) {
    widget<Editor>("editor")->addDocumentSetting(newSetting);
}


bool ViewWidget::isRepositioning() const {
    return widget<Editor>("editor")->isRepositioning();
}


bool ViewWidget::isDisplayCoherent() const {
    return widget<Editor>("editor")->isDisplayCoherent();
}


void ViewWidget::bindDynamic() {
    ProgrammaticView::bindDynamic();
}


void ViewWidget::unbindDynamic() {
    ProgrammaticView::unbindDynamic();
}


void ViewWidget::viewActive() {
    widget<Editor>("editor")->abortRepositioning();
}


void ViewWidget::viewInactive() {}


void ViewWidget::insertCommand(Command *newCommand) {
    Editor* editor = widget<Editor>("editor");
    editor->insertCommand(newCommand);
}


void ViewWidget::insertCommand(QSharedPointer<Command> newCommand) {
    Editor* editor = widget<Editor>("editor");
    editor->insertCommand(newCommand);
}


void ViewWidget::insertCommand(const CommandContainer& newCommand) {
    Editor* editor = widget<Editor>("editor");
    editor->insertCommand(newCommand);
}


void ViewWidget::undo() {
    Editor* editor = widget<Editor>("editor");
    editor->undo();
}


void ViewWidget::redo() {
    Editor* editor = widget<Editor>("editor");
    editor->redo();
}


ViewWidget::SearchResult ViewWidget::searchForward(
        const QString& searchText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression,
        bool           okToLoop
    ) {
    Editor* editor = widget<Editor>("editor");
    return editor->searchForward(searchText, caseSensitive, wholeWordsOnly, regularExpression, okToLoop);
}


ViewWidget::SearchResult ViewWidget::searchBackward(
        const QString& searchText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression,
        bool           okToLoop
    ) {
    Editor* editor = widget<Editor>("editor");
    return editor->searchBackward(searchText, caseSensitive, wholeWordsOnly, regularExpression, okToLoop);
}


void ViewWidget::setZoom(float newZoomFactor) {
    Editor* editor = widget<Editor>("editor");
    editor->setZoom(newZoomFactor);
}


void ViewWidget::setMarginsEnabled(bool nowEnabled) {
    widget<Editor>("editor")->setMarginsEnabled(nowEnabled);
}


void ViewWidget::setGuidesEnabled(bool nowEnabled) {
    widget<Editor>("editor")->setGuidesEnabled(nowEnabled);
}


void ViewWidget::restoreFocus() {
    if (isActiveView()) {
        widget<Editor>("editor")->setFocus(Qt::OtherFocusReason);
    }
}


void ViewWidget::forceVisibleArea(const QRectF& visibleRectangle) {
    widget<Editor>("editor")->ensureVisible(visibleRectangle, Editor::cursorMarginX, Editor::cursorMarginY);

}


void ViewWidget::abortRepositioning() {
    widget<Editor>("editor")->abortRepositioning();
}


void ViewWidget::reportTitleChanged(const QString&) {
    emit titleChanged(title());
}


void ViewWidget::processElementStackChange() {
    generateAggregationsByCapability();
    emit formatsAtCursorChanged(
        currentAggregationsByCapabilityWithAncestors,
        currentAggregationsByCapabilityWithoutAncestors
    );
}


void ViewWidget::processCursorUpdate() {
    QList<PageList::Index> newPagesUnderCursor = widget<Editor>("editor")->pagesUnderCursor();

    if (newPagesUnderCursor != currentPagesUnderCursor) {
        currentPagesUnderCursor = newPagesUnderCursor;
        emit pagesUnderCursorChanged(currentPagesUnderCursor);
    }
}


void ViewWidget::pageFormatsChanged() {
    QList<PageList::Index> newPagesUnderCursor = widget<Editor>("editor")->pagesUnderCursor();
    currentPagesUnderCursor = newPagesUnderCursor;
    emit pagesUnderCursorChanged(currentPagesUnderCursor);
}


void ViewWidget::elementFormatsChanged() {
    generateAggregationsByCapability();
    emit formatsAtCursorChanged(
        currentAggregationsByCapabilityWithAncestors,
        currentAggregationsByCapabilityWithoutAncestors
    );
}


void ViewWidget::configureWidget(Document* newDocument) {
    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    mainLayout->setSpacing(1);

    Editor* editor = (newDocument == Q_NULLPTR ? new Editor(this) : new Editor(newDocument, this));
    registerWidget(editor, "editor");

    mainLayout->addWidget(editor);
}


void ViewWidget::bind() {
    Editor* editor = widget<Editor>("editor");

    connect(editor, &Editor::addTabKeyPressed, this, &ViewWidget::addTabKeyPressed);
    connect(editor, &Editor::backspaceKeyPressed, this, &ViewWidget::backspaceKeyPressed);
    connect(editor, &Editor::boldKeyPressed, this, &ViewWidget::boldKeyPressed);
    connect(editor, &Editor::closeKeyPressed, this, &ViewWidget::closeKeyPressed);
    connect(editor, &Editor::copyKeyPressed, this, &ViewWidget::copyKeyPressed);
    connect(editor, &Editor::cutKeyPressed, this, &ViewWidget::cutKeyPressed);
    connect(editor, &Editor::deleteKeyPressed, this, &ViewWidget::deleteKeyPressed);
    connect(editor, &Editor::deleteToEndOfLineKeyPressed, this, &ViewWidget::deleteToEndOfLineKeyPressed);
    connect(editor, &Editor::deleteToEndOfWordKeyPressed, this, &ViewWidget::deleteToEndOfWordKeyPressed);
    connect(editor, &Editor::deleteToStartOfWordKeyPressed, this, &ViewWidget::deleteToStartOfWordKeyPressed);
    connect(editor, &Editor::deleteLineKeyPressed, this, &ViewWidget::deleteLineKeyPressed);
    connect(editor, &Editor::findKeyPressed, this, &ViewWidget::findKeyPressed);
    connect(editor, &Editor::findNextKeyPressed, this, &ViewWidget::findNextKeyPressed);
    connect(editor, &Editor::findPreviousKeyPressed, this, &ViewWidget::findPreviousKeyPressed);
    connect(editor, &Editor::forwardKeyPressed, this, &ViewWidget::forwardKeyPressed);
    connect(editor, &Editor::helpContentsKeyPressed, this, &ViewWidget::helpContentsKeyPressed);
    connect(editor, &Editor::insertLineSeparatorKeyPressed, this, &ViewWidget::insertLineSeparatorKeyPressed);
    connect(
        editor,
        &Editor::insertParagraphSeparatorKeyPressed,
        this,
        &ViewWidget::insertParagraphSeparatorKeyPressed
    );
    connect(editor, &Editor::italicsKeyPressed, this, &ViewWidget::italicsKeyPressed);
    connect(editor, &Editor::moveToEndOfBlockKeyPressed, this, &ViewWidget::moveToEndOfBlockKeyPressed);
    connect(editor, &Editor::moveToEndOfDocumentKeyPressed, this, &ViewWidget::moveToEndOfDocumentKeyPressed);
    connect(editor, &Editor::moveToEndOfLineKeyPressed, this, &ViewWidget::moveToEndOfLineKeyPressed);
    connect(editor, &Editor::moveToNextCharacterKeyPressed, this, &ViewWidget::moveToNextCharacterKeyPressed);
    connect(editor, &Editor::moveToNextLineKeyPressed, this, &ViewWidget::moveToNextLineKeyPressed);
    connect(editor, &Editor::moveToNextPageKeyPressed, this, &ViewWidget::moveToNextPageKeyPressed);
    connect(editor, &Editor::moveToNextWordKeyPressed, this, &ViewWidget::moveToNextWordKeyPressed);
    connect(editor, &Editor::moveToPreviousCharacterKeyPressed, this, &ViewWidget::moveToPreviousCharacterKeyPressed);
    connect(editor, &Editor::moveToPreviousLineKeyPressed, this, &ViewWidget::moveToPreviousLineKeyPressed);
    connect(editor, &Editor::moveToPreviousPageKeyPressed, this, &ViewWidget::moveToPreviousPageKeyPressed);
    connect(editor, &Editor::moveToPreviousWordKeyPressed, this, &ViewWidget::moveToPreviousWordKeyPressed);
    connect(editor, &Editor::moveToStartOfBlockKeyPressed, this, &ViewWidget::moveToStartOfBlockKeyPressed);
    connect(editor, &Editor::moveToStartOfDocumentKeyPressed, this, &ViewWidget::moveToStartOfDocumentKeyPressed);
    connect(editor, &Editor::moveToStartOfLineKeyPressed, this, &ViewWidget::moveToStartOfLineKeyPressed);
    connect(editor, &Editor::newKeyPressed, this, &ViewWidget::newKeyPressed);
    connect(editor, &Editor::nextChildKeyPressed, this, &ViewWidget::nextChildKeyPressed);
    connect(editor, &Editor::openKeyPressed, this, &ViewWidget::openKeyPressed);
    connect(editor, &Editor::pasteKeyPressed, this, &ViewWidget::pasteKeyPressed);
    connect(editor, &Editor::preferencesKeyPressed, this, &ViewWidget::preferencesKeyPressed);
    connect(editor, &Editor::previousChildKeyPressed, this, &ViewWidget::previousChildKeyPressed);
    connect(editor, &Editor::printKeyPressed, this, &ViewWidget::printKeyPressed);
    connect(editor, &Editor::quitKeyPressed, this, &ViewWidget::quitKeyPressed);
    connect(editor, &Editor::redoKeyPressed, this, &ViewWidget::redoKeyPressed);
    connect(editor, &Editor::refreshKeyPressed, this, &ViewWidget::refreshKeyPressed);
    connect(editor, &Editor::replaceKeyPressed, this, &ViewWidget::replaceKeyPressed);
    connect(editor, &Editor::saveAsKeyPressed, this, &ViewWidget::saveAsKeyPressed);
    connect(editor, &Editor::saveKeyPressed, this, &ViewWidget::saveKeyPressed);
    connect(editor, &Editor::selectAllKeyPressed, this, &ViewWidget::selectAllKeyPressed);
    connect(editor, &Editor::deselectKeyPressed, this, &ViewWidget::deselectKeyPressed);
    connect(editor, &Editor::underlineKeyPressed, this, &ViewWidget::underlineKeyPressed);
    connect(editor, &Editor::undoKeyPressed, this, &ViewWidget::undoKeyPressed);
    connect(editor, &Editor::unknownKeyPressed, this, &ViewWidget::unknownKeyPressed);
    connect(editor, &Editor::whatsThisKeyPressed, this, &ViewWidget::whatsThisKeyPressed);
    connect(editor, &Editor::zoomInKeyPressed, this, &ViewWidget::zoomInKeyPressed);
    connect(editor, &Editor::zoomOutKeyPressed, this, &ViewWidget::zoomOutKeyPressed);
    connect(editor, &Editor::fullScreenKeyPressed, this, &ViewWidget::fullScreenKeyPressed);
    connect(editor, &Editor::cancelKeyPressed, this, &ViewWidget::cancelKeyPressed);
    connect(editor, &Editor::textKeyPressed, this, &ViewWidget::textKeyPressed);
    connect(editor, &Editor::textKeyPressedOnPlaceholder, this, &ViewWidget::textKeyPressedOnPlaceholder);

    connect(editor, &Editor::rightMouseButtonClicked, this, &ViewWidget::rightMouseButtonClicked);

    connect(editor, &Editor::filenameChanged, this, &ViewWidget::filenameChanged);
    connect(editor, &Editor::shortformNameChanged, this, &ViewWidget::shortformNameChanged);
    connect(editor, &Editor::documentModified, this, &ViewWidget::documentModified);
    connect(editor, &Editor::documentChanged, this, &ViewWidget::documentChanged);
    connect(editor, &Editor::undoAvailable, this, &ViewWidget::undoAvailable);
    connect(editor, &Editor::redoAvailable, this, &ViewWidget::redoAvailable);

    connect(editor, &Editor::filenameChanged, this, &ViewWidget::reportTitleChanged);

    connect(editor, &Editor::undoRedoStackChanged, this, &ViewWidget::undoRedoStackChanged);
    connect(editor, &Editor::commandFailed, this, &ViewWidget::commandFailed);
    connect(editor, &Editor::undoFailed, this, &ViewWidget::undoFailed);
    connect(editor, &Editor::redoFailed, this, &ViewWidget::redoFailed);

    connect(editor, &Editor::cursorTypeChanged, this, &ViewWidget::cursorTypeChanged);
    connect(editor, &Editor::requirementsChanged, this, &ViewWidget::requirementsChanged);

    connect(editor, &Editor::cursorUpdated, this, &ViewWidget::cursorUpdated);
    connect(editor, &Editor::cursorUpdated, this, &ViewWidget::processCursorUpdate);
    connect(editor, &Editor::elementStackChanged, this, &ViewWidget::elementStackChanged);
    connect(editor, &Editor::elementStackChanged, this, &ViewWidget::processElementStackChange);
    connect(editor, &Editor::pageFormatsChanged, this, &ViewWidget::pageFormatsChanged);
    connect(editor, &Editor::elementFormatsChanged, this, &ViewWidget::elementFormatsChanged);

    connect(editor, &Editor::contentsAreIncoherent, this, &ViewWidget::contentsAreIncoherent);
    connect(editor, &Editor::contentsAreCoherent, this, &ViewWidget::contentsAreCoherent);
    connect(editor, &Editor::repositioningAborted, this, &ViewWidget::repositioningAborted);

    ProgrammaticView::bind();
}


void ViewWidget::generateAggregationsByCapability() const {
    Ld::ElementPointerList elements = elementsUnderCursor(false);

    currentAggregationsByCapabilityWithoutAncestors.clear();

    Ld::ElementPointerSet elementsWithAncestors;
    for (  Ld::ElementPointerList::const_iterator elementIterator    = elements.constBegin(),
                                                  elementEndIterator = elements.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        if (!element.isNull()) {
            currentAggregationsByCapabilityWithoutAncestors.addFormat(element);
            elementsWithAncestors << element;

            Ld::ElementPointerList ancestorList = element->ancestors();
            elementsWithAncestors |= Ld::ElementPointerSet(ancestorList.begin(), ancestorList.end());
        }
    }

    currentAggregationsByCapabilityWithAncestors.clear();
    for (  Ld::ElementPointerSet::const_iterator allElementsIterator    = elementsWithAncestors.constBegin(),
                                                 allElementsEndIterator = elementsWithAncestors.constEnd()
         ; allElementsIterator != allElementsEndIterator
         ; ++allElementsIterator
        ) {
        currentAggregationsByCapabilityWithAncestors.addFormat(*allElementsIterator);
    }
}
