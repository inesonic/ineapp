/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Editor class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QWidget>
#include <QOpenGLWidget>
#include <QResizeEvent>
#include <QList>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QRectF>
#include <QScrollBar>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QKeySequence>
#include <QGraphicsView>
#include <QPointF>
#include <QPoint>

#include <limits>

#include <eqt_signal_aggregator.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_text_element.h>
#include <ld_placeholder_element.h>
#include <ld_list_placeholder_element.h>
#include <ld_format.h>
#include <ld_character_format.h>
#include <ld_capabilities.h>
#include <ld_special_characters.h>
#include <ld_document_setting.h>

#include "application.h"
#include "command.h"
#include "command_container.h"
#include "insert_string_command.h"
#include "insert_element_command.h"
#include "delete_command.h"
#include "command_popup_dialog.h"
#include "view_widget.h"
#include "page_list.h"
#include "presentation.h"
#include "document.h"
#include "cursor.h"
#include "page_list.h"
#include "editor.h"

Editor::Editor(ViewWidget* parent):QGraphicsView(parent) {
    QSharedPointer<Ld::RootElement> rootElement = Ld::Element::create(Ld::RootElement::elementName)
                                                  .dynamicCast<Ld::RootElement>();
    Ld::RootElement::registerRootElement(rootElement);

    Document* document = dynamic_cast<Document*>(rootElement->visual());
    document->fillEmptyDocument();

    configureWidget(document);
}


Editor::Editor(Document* newDocument, ViewWidget* parent):QGraphicsView(parent) {
    configureWidget(newDocument);
}


Editor::~Editor() {
    Document* oldDocument = document();

    // At this point the disposition of the document is already decided if nothing else references it.

    if (oldDocument != Q_NULLPTR) {
        if (oldDocument->countEditors() == 1 && oldDocument->referencedBy().count() == 0) {
            oldDocument->deleteLater(); // Triggers the document to be deleted after we exit the destructor.
        } else {
            oldDocument->unregisterEditor(this);
            oldDocument->removeCursor(currentCursor);
        }
    }
}


void Editor::setDocument(Document* newDocument) {
    Document* oldDocument = document();

    newDocument->registerEditor(this);
    newDocument->addCursor(currentCursor);

    connect(newDocument, &Document::pageFormatHasChanged, this, &Editor::pageFormatChanged);
    connect(newDocument, &Document::descendantFormatWasChanged, this, &Editor::descendantFormatWasChanged);
    connect(newDocument, &Document::descendantFormatWasUpdated, this, &Editor::descendantFormatWasChanged);
    connect(newDocument, &Document::filenameChanged, this, &Editor::filenameChanged);
    connect(newDocument, &Document::shortformNameChanged, this, &Editor::shortformNameChanged);
    connect(newDocument, &Document::modificationChanged, this, &Editor::documentModified);
    connect(newDocument, &Document::programNowChanged, this, &Editor::documentChanged);
    connect(newDocument, &Document::undoAvailable, this, &Editor::undoAvailable);
    connect(newDocument, &Document::redoAvailable, this, &Editor::redoAvailable);
    connect(newDocument, &Document::undoRedoStackChanged, this, &Editor::undoRedoStackChanged);
    connect(newDocument, &Document::commandFailed, this, &Editor::commandFailed);
    connect(newDocument, &Document::undoFailed, this, &Editor::undoFailed);
    connect(newDocument, &Document::redoFailed, this, &Editor::redoFailed);
    connect(newDocument, &Document::presentationUpdatesPending, this, &Editor::contentsAreIncoherent);
    connect(newDocument, &Document::presentationUpdatesCompleted, this, &Editor::presentationUpdatesCompleted);
    connect(newDocument, &Document::presentationUpdatesRestarted, this, &Editor::presentationUpdatesRestarted);
    connect(newDocument, &Document::maximumExtentsChanged, this, &Editor::documentMaximumHorizontalExtentChanged);
    connect(newDocument, &Document::sceneRectChanged, this, &Editor::documentSceneRectangleChanged);
    connect(newDocument, &Document::changed, this, &Editor::sceneAreasChanged);


    QString       newFilename                 = newDocument->filename();
    QString       newShortformName            = newDocument->shortformName();
    bool          newDocumentModified         = newDocument->isModified();
    bool          newUndoAvailable            = newDocument->canUndo();
    bool          newRedoAvailable            = newDocument->canRedo();
    QList<double> newPageColumnWidthPoints    = newDocument->maximumHorizontalExtentPoints();
    QList<double> newPageColumnLeftEdgePoints = newDocument->pageColumnLeftEdgePoints();
    QRectF        newSceneRect                = newDocument->sceneRect();

    if (oldDocument != Q_NULLPTR) {
        disconnect(oldDocument, &Document::pageFormatHasChanged, this, &Editor::pageFormatChanged);
        disconnect(oldDocument, &Document::descendantFormatWasChanged, this, &Editor::descendantFormatWasChanged);
        disconnect(oldDocument, &Document::descendantFormatWasUpdated, this, &Editor::descendantFormatWasChanged);
        disconnect(oldDocument, &Document::filenameChanged, this, &Editor::filenameChanged);
        disconnect(oldDocument, &Document::shortformNameChanged, this, &Editor::shortformNameChanged);
        disconnect(oldDocument, &Document::modificationChanged, this, &Editor::documentModified);
        disconnect(oldDocument, &Document::programNowChanged, this, &Editor::documentChanged);
        disconnect(oldDocument, &Document::undoAvailable, this, &Editor::undoAvailable);
        disconnect(oldDocument, &Document::redoAvailable, this, &Editor::redoAvailable);
        disconnect(oldDocument, &Document::undoRedoStackChanged, this, &Editor::undoRedoStackChanged);
        disconnect(oldDocument, &Document::commandFailed, this, &Editor::commandFailed);
        disconnect(oldDocument, &Document::undoFailed, this, &Editor::undoFailed);
        disconnect(oldDocument, &Document::redoFailed, this, &Editor::redoFailed);
        disconnect(oldDocument, &Document::presentationUpdatesPending, this, &Editor::contentsAreIncoherent);
        disconnect(oldDocument, &Document::presentationUpdatesCompleted, this, &Editor::presentationUpdatesCompleted);
        disconnect(oldDocument, &Document::presentationUpdatesRestarted, this, &Editor::presentationUpdatesRestarted);
        disconnect(
            oldDocument,
            &Document::maximumExtentsChanged,
            this,
            &Editor::documentMaximumHorizontalExtentChanged
        );
        disconnect(oldDocument, &Document::sceneRectChanged, this, &Editor::documentSceneRectangleChanged);
        disconnect(oldDocument, &Document::changed, this, &Editor::sceneAreasChanged);

        QString       oldFilename                 = oldDocument->filename();
        QString       oldShortformName            = oldDocument->shortformName();
        bool          oldDocumentModified         = oldDocument->isModified();
        bool          oldUndoAvailable            = oldDocument->canUndo();
        bool          oldRedoAvailable            = oldDocument->canRedo();
        QList<double> oldPageColumnWidthPoints    = oldDocument->maximumHorizontalExtentPoints();
        QList<double> oldPageColumnLeftEdgePoints = oldDocument->pageColumnLeftEdgePoints();
        QRectF        oldSceneRect                = oldDocument->sceneRect();

        // At this point the disposition of the document is already decided if nothing else references it.

        if (oldDocument->countEditors() == 1 && oldDocument->referencedBy().count() == 0) {
            delete oldDocument;
        } else {
            oldDocument->unregisterEditor(this);
            oldDocument->removeCursor(currentCursor);
        }

        setScene(newDocument);

        if (newFilename != oldFilename) {
            emit filenameChanged(newFilename);
        }

        if (newShortformName != oldShortformName) {
            emit shortformNameChanged(newShortformName);
        }

        if (newDocumentModified != oldDocumentModified) {
            emit documentModified(newDocumentModified);
        }

        if (newUndoAvailable != oldUndoAvailable) {
            emit undoAvailable(newUndoAvailable);
        }

        if (newRedoAvailable != oldRedoAvailable) {
            emit redoAvailable(newRedoAvailable);
        }

        // Emit no matter what since this indicates other likely changes.
        emit undoRedoStackChanged(newDocument->undoStackSize(), newDocument->redoStackSize());

        if (newPageColumnWidthPoints != oldPageColumnWidthPoints       ||
            newPageColumnLeftEdgePoints != oldPageColumnLeftEdgePoints ||
            newSceneRect.left() != oldSceneRect.left()                 ||
            newSceneRect.right() != oldSceneRect.right()                 ) {
            emit maximumHorizontalExtentChanged(
                newPageColumnWidthPoints,
                newPageColumnLeftEdgePoints,
                newSceneRect.left(),
                newSceneRect.right()
            );
        }
    } else {
        setScene(newDocument);

        emit filenameChanged(newFilename);
        emit shortformNameChanged(newShortformName);
        emit documentModified(newDocumentModified);
        emit undoAvailable(newUndoAvailable);
        emit redoAvailable(newRedoAvailable);
        emit undoRedoStackChanged(newDocument->undoStackSize(), newDocument->redoStackSize());
        emit maximumHorizontalExtentChanged(
            newPageColumnWidthPoints,
            newPageColumnLeftEdgePoints,
            newSceneRect.left(),
            newSceneRect.right()
        );
    }

    currentCursor->setRootPresentation(newDocument);
    currentCursor->updateFromElementCursor(Ld::ElementCursor(newDocument->element(), 0));

    cursorUpdatePending         = false;
    abortRepositioningRequested = false;

    emit pageFormatsChanged();
    emit elementFormatsChanged();

    updateAllowedActions(true);
}


Document* Editor::document() const {
    return dynamic_cast<Document*>(scene());
}


QString Editor::filename() const {
    return document()->filename();
}


QString Editor::shortformName() const {
    return document()->shortformName();
}


QString Editor::identifier() const {
    return document()->identifier();
}


bool Editor::isEmpty() const {
    return document()->isEmpty();
}


bool Editor::isModified() const {
    return document()->isModified();
}


bool Editor::isReplaceable() const {
    return document()->isReplaceable();
}


unsigned long Editor::undoStackSize() const {
    return document()->undoStackSize();
}


unsigned long Editor::redoStackSize() const {
    return document()->redoStackSize();
}


bool Editor::canUndo() const {
    return document()->canUndo();
}


bool Editor::canRedo() const {
    return document()->canRedo();
}


const CommandContainer& Editor::undoRedoStackContentsAt(long index) const {
    return document()->undoRedoStackContentsAt(index);
}


float Editor::zoom() const {
    return QGraphicsView::transform().m11();
}


QList<double> Editor::maximumHorizontalExtentPoints() const {
    return document()->maximumHorizontalExtentPoints();
}


QList<double> Editor::pageColumnLeftEdgePoints() const {
    return document()->pageColumnLeftEdgePoints();
}


double Editor::sceneLeftEdge() const {
    return document()->sceneRect().left();
}


double Editor::sceneRightEdge() const {
    return document()->sceneRect().right();
}


bool Editor::marginsEnabled() const {
    return marginsAreEnabled;
}


bool Editor::guidesEnabled() const {
    return guidesAreEnabled;
}


const Cursor& Editor::cursor() const {
    return *currentCursor;
}


void Editor::updateCursorPosition(const Ld::ElementCursor& elementCursor) {
    currentCursor->updateFromElementCursor(elementCursor);
}


Ld::Capabilities Editor::requirementsAtCursor() const {
    return currentParentRequirements;
}


Cursor::Type Editor::cursorType() const {
    return currentCursorType;
}


void Editor::selectEntireDocument() {
    currentCursor->selectAll();
    updateAllowedActions();
}


Ld::ElementPointerList Editor::elementsUnderCursor(bool includeAncestors) const {
    return currentCursor->elementsUnderCursor(includeAncestors);
}


QList<Ld::FormatPointer> Editor::formatsAtCursor() const {
    Ld::ElementPointerList            elements    = currentCursor->elementsUnderCursor();
    Document*                         document    = Editor::document();
    QSharedPointer<Ld::RootElement>   rootElement = document->element();

    QSet<PageList::Index>    pageIndexes;
    QList<Ld::FormatPointer> formats;

    for (  Ld::ElementPointerList::const_iterator elementIterator    = elements.constBegin(),
                                                  elementEndIterator = elements.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        if (!element.isNull()) {
            Ld::FormatPointer format = element->format();
            if (!format.isNull()) {
                formats.append(format);
            }

            if (element->parent() == rootElement) {
                Presentation* presentation = dynamic_cast<Presentation*>(element->visual());
                if (presentation != Q_NULLPTR) {
                    QSet<PageList::Index> newPages = document->pagesContainingPresentation(presentation);
                    pageIndexes.unite(newPages);
                }
            }
        }
    }

    for (  QSet<PageList::Index>::const_iterator pageIndexIterator    = pageIndexes.constBegin(),
                                                 pageIndexEndIterator = pageIndexes.constEnd()
         ; pageIndexIterator != pageIndexEndIterator
         ; ++pageIndexIterator
        ) {
        PageList::Index pageIndex = *pageIndexIterator;
        Ld::FormatPointer format = rootElement->pageFormat(pageIndex);
        if (!format.isNull()) {
            formats.prepend(format);
        }
    }

    return formats;
}


QList<PageList::Index> Editor::pagesUnderCursor() const {
    QList<PageList::Index> result;
    Document*              document = Editor::document();

    if (currentCursor->cursorType() == Cursor::Type::TEXT) {
        QPointF         cursorCenter = currentCursor->boundingRectangle().center();
        PageList::Index pageIndex    = document->pageAt(cursorCenter);

        if (pageIndex != PageList::invalidPageIndex) {
            result.append(pageIndex);
        }
    } else {
        Ld::ElementPointerList          elements    = currentCursor->elementsUnderCursor();
        QSharedPointer<Ld::RootElement> rootElement = document->element();

        QSet<PageList::Index> pageIndexes;

        for (  Ld::ElementPointerList::const_iterator elementIterator    = elements.constBegin(),
                                                      elementEndIterator = elements.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (!element.isNull()) {
                Presentation* presentation = dynamic_cast<Presentation*>(element->visual());
                if (presentation != Q_NULLPTR) {
                    QSet<PageList::Index> newPages = document->pagesContainingPresentation(presentation);
                    pageIndexes.unite(newPages);
                }
            }
        }

        result = pageIndexes.values();
    }

   return result;
}


QPoint Editor::globalCursorPosition() const {
    return mapToGlobal(mapFromScene(currentCursor->cursorPosition()));
}


QSharedPointer<Ld::PageFormat> Editor::pageFormat(PageList::Index pageIndex) const {
    Document*                          document    = Editor::document();
    QSharedPointer<Ld::RootElement>    rootElement = document->element();

    return rootElement->pageFormat(pageIndex);
}


void Editor::setFormatForNextTextInsertion(Ld::FormatPointer newFormat) {
    currentNextTextInsertionFormat        = newFormat;
    currentNextTextInsertionElementCursor = currentCursor->elementCursor();
}


void Editor::clearFormatForNextTextInsertion() {
    currentNextTextInsertionFormat.clear();
    currentNextTextInsertionElementCursor.setInvalid();
}


Ld::FormatPointer Editor::nextTextInsertionFormat(const Ld::ElementCursor& elementCursor) const {
    Ld::FormatPointer result;

    if (currentNextTextInsertionElementCursor == elementCursor) {
        result = currentNextTextInsertionFormat;
    }

    return result;
}


QSharedPointer<Ld::DocumentSetting> Editor::documentSettingByName(const QString& settingName) const {
    return document()->documentSettingByName(settingName);
}


void Editor::addDocumentSetting(QSharedPointer<Ld::DocumentSetting> newSetting) {
    document()->addDocumentSetting(newSetting);
}


bool Editor::isRepositioning() const {
    return document()->isRepositioning();
}


bool Editor::isDisplayCoherent() const {
    return document()->isDisplayCoherent();
}


void Editor::setMarginsEnabled(bool nowEnabled) {
    marginsAreEnabled = nowEnabled;
    viewport()->update();
}


void Editor::setGuidesEnabled(bool nowEnabled) {
    guidesAreEnabled = nowEnabled;
    viewport()->update();
}


void Editor::insertCommand(Command* newCommand) {
    currentNextTextInsertionFormat.clear();
    currentNextTextInsertionElementCursor.setInvalid();

    newCommand->setCursor(currentCursor);
    document()->insertCommand(newCommand);
}


void Editor::insertCommand(QSharedPointer<Command> newCommand) {
    currentNextTextInsertionFormat.clear();
    currentNextTextInsertionElementCursor.setInvalid();

    newCommand->setCursor(currentCursor);
    document()->insertCommand(newCommand);
}


void Editor::insertCommand(const CommandContainer& newCommand) {
    currentNextTextInsertionFormat.clear();
    currentNextTextInsertionElementCursor.setInvalid();

    CommandContainer container = newCommand;
    container->setCursor(currentCursor);

    document()->insertCommand(newCommand);
}


void Editor::undo() {
    document()->undo();
}


void Editor::redo() {
    document()->redo();
}


Editor::SearchResult Editor::searchForward(
        const QString& searchText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression,
        bool           okToLoop
    ) {
    SearchResult result = currentCursor->searchForward(
        searchText,
        caseSensitive,
        wholeWordsOnly,
        regularExpression,
        okToLoop
    );

    updateAllowedActions();

    return result;
}


Editor::SearchResult Editor::searchBackward(
        const QString& searchText,
        bool           caseSensitive,
        bool           wholeWordsOnly,
        bool           regularExpression,
        bool           okToLoop
    ) {
    SearchResult result = currentCursor->searchBackward(
        searchText,
        caseSensitive,
        wholeWordsOnly,
        regularExpression,
        okToLoop
    );

    updateAllowedActions();

    return result;
}


void Editor::setZoom(float newZoomFactor) {
    setTransform(QTransform(newZoomFactor, 0, 0, newZoomFactor, 0, 0));

    if (document()->arePresentationUpdatesPending()) {
        cursorUpdatePending = true;
    } else {
        QRectF cursorBoundingRectangle = currentCursor->boundingRectangle();
        ensureVisible(cursorBoundingRectangle, cursorMarginX, cursorMarginY);
    }
}


void Editor::abortRepositioning() {
    if (document()->isRepositioning()) {
        abortRepositioningRequested = true;
        document()->abortRepositioning();
    } else {
        emit repositioningAborted();
    }
}


void Editor::drawForeground(QPainter *painter, const QRectF& rect) {
    Document* document = Editor::document();
    painter->save();
    if (marginsAreEnabled || guidesAreEnabled) {
        document->drawPageDecorations(rect, painter, marginsAreEnabled, guidesAreEnabled);
    }

    if (document->isDisplayCoherent()) {
        currentCursor->drawCursor(painter, rect, currentShowCursor);
    }

    painter->restore();
}


void Editor::dragEnterEvent(QDragEnterEvent* event) {
    QGraphicsView::dragEnterEvent(event);
}


void Editor::dragLeaveEvent(QDragLeaveEvent* event) {
    QGraphicsView::dragLeaveEvent(event);
}


void Editor::dragMoveEvent(QDragMoveEvent* event) {
    QGraphicsView::dragMoveEvent(event);
}


void Editor::dropEvent(QDropEvent* event) {
    QGraphicsView::dropEvent(event);
}


void Editor::focusInEvent(QFocusEvent* event) {
    blinkCursor();
    QGraphicsView::focusInEvent(event);
}


bool Editor::focusNextPrevChild(bool next) {
    return QGraphicsView::focusNextPrevChild(next);
}


void Editor::focusOutEvent(QFocusEvent* event) {
    cursorTimer->stop();
    currentShowCursor = true;
    updateScene(QList<QRectF>() << previousCursorBoundingRectangle);

    QGraphicsView::focusOutEvent(event);
}


// void Editor::inputMethodEvent(QInputMethodEvent* event) {}


void Editor::keyPressEvent(QKeyEvent* event) {
    if        (event->matches(QKeySequence::AddTab)) {
        emit addTabKeyPressed();
    } else if (event->matches(QKeySequence::Back)) {
        #if (defined(Q_OS_WIN))

            emit backspaceKeyPressed();

        #elif (!defined(Q_OS_LINUX) && !defined(Q_OS_DARWIN))

            #error Unknown Platform

        #endif
    } else if (event->matches(QKeySequence::Backspace)) {
        emit backspaceKeyPressed();
    } else if (event->matches(QKeySequence::Bold)) {
        emit boldKeyPressed();
    } else if (event->matches(QKeySequence::Close)) {
        emit closeKeyPressed();
    } else if (event->matches(QKeySequence::Copy)) {
        emit copyKeyPressed();
    } else if (event->matches(QKeySequence::Cut)) {
        emit cutKeyPressed();
    } else if (event->matches(QKeySequence::Delete)) {
        emit deleteKeyPressed();
    } else if (event->matches(QKeySequence::DeleteEndOfLine)) {
        emit deleteToEndOfLineKeyPressed();
    } else if (event->matches(QKeySequence::DeleteEndOfWord)) {
        emit deleteToEndOfWordKeyPressed();
    } else if (event->matches(QKeySequence::DeleteStartOfWord)) {
        emit deleteToStartOfWordKeyPressed();
    } else if (event->matches(QKeySequence::DeleteCompleteLine)) {
        emit deleteLineKeyPressed();
    } else if (event->matches(QKeySequence::Find)) {
        emit findKeyPressed();
    } else if (event->matches(QKeySequence::FindNext)) {
        emit findNextKeyPressed();
    } else if (event->matches(QKeySequence::FindPrevious)) {
        emit findPreviousKeyPressed();
    } else if (event->matches(QKeySequence::Forward)) {
        emit forwardKeyPressed();
    } else if (event->matches(QKeySequence::HelpContents)) {
        emit helpContentsKeyPressed();
    } else if (event->matches(QKeySequence::InsertLineSeparator)) {
        emit insertLineSeparatorKeyPressed();
    } else if (event->matches(QKeySequence::InsertParagraphSeparator)) {
        emit insertParagraphSeparatorKeyPressed();
    } else if (event->matches(QKeySequence::Italic)) {
        emit italicsKeyPressed();
    } else if (event->matches(QKeySequence::MoveToEndOfBlock)) {
        emit moveToEndOfBlockKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveForwardByElement();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToEndOfDocument)) {
        emit moveToEndOfDocumentKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveToEndOfDocumentView();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToEndOfLine)) {
        emit moveToEndOfLineKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveToEndOfLine();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToNextChar)) {
        emit moveToNextCharacterKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveForwardByCharacter();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToNextLine)) {
        emit moveToNextLineKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveForwardByLine();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToNextPage)) {
        emit moveToNextPageKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        pageMove(event);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToNextWord)) {
        emit moveToNextWordKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveForwardByWhitespace();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToPreviousChar)) {
        emit moveToPreviousCharacterKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveBackwardByCharacter();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToPreviousLine)) {
        emit moveToPreviousLineKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveBackwardByLine();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToPreviousPage)) {
        emit moveToPreviousPageKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        pageMove(event);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToPreviousWord)) {
        emit moveToPreviousWordKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveBackwardByWhitespace();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToStartOfBlock)) {
        emit moveToStartOfBlockKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveBackwardByElement();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToStartOfDocument)) {
        emit moveToStartOfDocumentKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveToStartOfDocumentView();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::MoveToStartOfLine)) {
        emit moveToStartOfLineKeyPressed(false);
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }

        currentCursor->moveToStartOfLine();
        updateAllowedActions();
    } else if (event->matches(QKeySequence::New)) {
        emit newKeyPressed();
    } else if (event->matches(QKeySequence::NextChild)) {
        emit nextChildKeyPressed();
    } else if (event->matches(QKeySequence::Open)) {
        emit openKeyPressed();
    } else if (event->matches(QKeySequence::Paste)) {
        emit pasteKeyPressed();
    } else if (event->matches(QKeySequence::Preferences)) {
        emit preferencesKeyPressed();
    } else if (event->matches(QKeySequence::PreviousChild)) {
        emit previousChildKeyPressed();
    } else if (event->matches(QKeySequence::Print)) {
        emit printKeyPressed();
    } else if (event->matches(QKeySequence::Quit)) {
        emit quitKeyPressed();
    } else if (event->matches(QKeySequence::Redo)) {
        emit redoKeyPressed();
    } else if (event->matches(QKeySequence::Refresh)) {
        emit refreshKeyPressed();
    } else if (event->matches(QKeySequence::Replace)) {
        emit replaceKeyPressed();
    } else if (event->matches(QKeySequence::SaveAs)) {
        emit saveAsKeyPressed();
    } else if (event->matches(QKeySequence::Save)) {
        emit saveKeyPressed();
    } else if (event->matches(QKeySequence::SelectAll)) {
        emit selectAllKeyPressed();
    } else if (event->matches(QKeySequence::Deselect)) {
        emit deselectKeyPressed();
        if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
            currentCursor->clearSelection();
        }
    } else if (event->matches(QKeySequence::SelectEndOfBlock)) {
        emit moveToEndOfBlockKeyPressed(true);
        currentCursor->moveForwardByElement(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectEndOfDocument)) {
        emit moveToEndOfDocumentKeyPressed(true);
        currentCursor->moveToEndOfDocumentView(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectEndOfLine)) {
        emit moveToEndOfLineKeyPressed(true);
        currentCursor->moveToEndOfLine(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectNextChar)) {
        emit moveToNextCharacterKeyPressed(true);
        currentCursor->moveForwardByCharacter(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectNextLine)) {
        emit moveToNextLineKeyPressed(true);
        currentCursor->moveForwardByLine(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectNextPage)) {
        emit moveToNextPageKeyPressed(true);
        pageMove(event);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectNextWord)) {
        emit moveToNextWordKeyPressed(true);
        currentCursor->moveForwardByWhitespace(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectPreviousChar)) {
        emit moveToPreviousCharacterKeyPressed(true);
        currentCursor->moveBackwardByCharacter(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectPreviousLine)) {
        emit moveToPreviousLineKeyPressed(true);
        currentCursor->moveBackwardByLine(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectPreviousPage)) {
        emit moveToPreviousPageKeyPressed(true);
        pageMove(event);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectPreviousWord)) {
        emit moveToPreviousWordKeyPressed(true);
        currentCursor->moveBackwardByWhitespace(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectStartOfBlock)) {
        emit moveToStartOfBlockKeyPressed(true);
        currentCursor->moveBackwardByElement(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectStartOfDocument)) {
        emit moveToStartOfDocumentKeyPressed(true);
        currentCursor->moveToStartOfDocumentView(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::SelectStartOfLine)) {
        emit moveToStartOfLineKeyPressed(true);
        currentCursor->moveToStartOfLine(true);
        updateAllowedActions();
    } else if (event->matches(QKeySequence::Underline)) {
        emit underlineKeyPressed();
    } else if (event->matches(QKeySequence::Undo)) {
        emit undoKeyPressed();
    } else if (event->matches(QKeySequence::UnknownKey)) {
        emit unknownKeyPressed(
            event->key(),
            event->modifiers(),
            event->nativeModifiers(),
            event->nativeScanCode(),
            event->nativeVirtualKey(),
            event->text()
        );
    } else if (event->matches(QKeySequence::WhatsThis)) {
        emit whatsThisKeyPressed();
    } else if (event->matches(QKeySequence::ZoomIn)) {
        emit zoomInKeyPressed();
    } else if (event->matches(QKeySequence::ZoomOut)) {
        emit zoomOutKeyPressed();
    } else if (event->matches(QKeySequence::FullScreen)) {
        emit fullScreenKeyPressed();
    } else if (event->matches(QKeySequence::Cancel)) {
        emit cancelKeyPressed();
    } else if (!event->text().isEmpty()) {
        if (currentCursorType == Cursor::Type::ELEMENT) {
            Ld::ElementPointer element = currentCursor->elementCursor().element();
            if (!element.isNull() && element->isPlaceholder()) {
                emit textKeyPressedOnPlaceholder(
                    event->key(),
                    event->modifiers(),
                    event->nativeModifiers(),
                    event->nativeScanCode(),
                    event->nativeVirtualKey(),
                    event->text()
                );
            } else {
                emit textKeyPressed(
                    event->key(),
                    event->modifiers(),
                    event->nativeModifiers(),
                    event->nativeScanCode(),
                    event->nativeVirtualKey(),
                    event->text()
                );
            }
        } else {
            emit textKeyPressed(
                event->key(),
                event->modifiers(),
                event->nativeModifiers(),
                event->nativeScanCode(),
                event->nativeVirtualKey(),
                event->text()
            );
        }
    }
}


void Editor::mouseDoubleClickEvent(QMouseEvent* event) {
    QGraphicsView::mouseDoubleClickEvent(event);
}


void Editor::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton) {
        QPoint  viewportPosition = event->pos();
        QPointF scenePosition    = mapToScene(viewportPosition);

        currentCursor->moveToPosition(scenePosition, true);
    }

    QGraphicsView::mouseMoveEvent(event);
}


void Editor::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        Qt::KeyboardModifiers modifiers = Application::keyboardModifiers();
        bool                  shiftDown = ((modifiers & Qt::ShiftModifier) != 0);

        // Qt::KeyboardModifiers QApplication::keyboardModifiers()
        QPoint  viewportPosition = event->pos();
        QPointF scenePosition    = mapToScene(viewportPosition);

        if (shiftDown) {
            currentCursor->moveToPosition(scenePosition, true);
        } else {
            if (currentCursor->cursorType() == Cursor::Type::SELECTION) {
                currentCursor->clearSelection();
            }

            currentCursor->moveToPosition(scenePosition, false);
        }
        updateAllowedActions();
    } else if (event->button() == Qt::RightButton) {
        QPoint viewportPosition = event->pos();
        QPoint globalPosition   = mapToGlobal(viewportPosition);

        emit rightMouseButtonClicked(globalPosition);
    }

    QGraphicsView::mousePressEvent(event);
}


void Editor::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        const Ld::ElementCursor& elementCursor   = currentCursor->elementCursor();
        const Ld::ElementCursor& selectionCursor = currentCursor->selectionCursor();

        if (elementCursor == selectionCursor) {
            currentCursor->clearSelection();
        }

        updateAllowedActions();
    }

    QGraphicsView::mouseReleaseEvent(event);
}


void Editor::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    viewport()->update(); // Force the viewport to always update after we resize.
}


void Editor::presentationUpdatesCompleted() {
    if (cursorUpdatePending) {
        cursorUpdatePending = false;
        updateCursorPosition();
    }

    updateAllowedActions(false);

    emit contentsAreCoherent();
    if (abortRepositioningRequested) {
        emit repositioningAborted();
        abortRepositioningRequested = false;
    }
}


void Editor::presentationUpdatesRestarted() {
    currentCursor->forceScreenUpdate(true);
 }


void Editor::documentMaximumHorizontalExtentChanged(
        QList<double> newMaximumExtentsPoints,
        QList<double> newPageColumnLeftEdgePoints,
        double        newLeftEdge,
        double        newRightEdge
    ) {
    emit maximumHorizontalExtentChanged(
        newMaximumExtentsPoints,
        newPageColumnLeftEdgePoints,
        newLeftEdge,
        newRightEdge
    );
}


void Editor::documentSceneRectangleChanged(const QRectF& rectangle) {
    QList<double> maximumExtentsPoints = document()->maximumHorizontalExtentPoints();
    QList<double> leftEdgePoints       = document()->pageColumnLeftEdgePoints();

    emit maximumHorizontalExtentChanged(maximumExtentsPoints, leftEdgePoints, rectangle.left(), rectangle.right());
}


void Editor::sceneAreasChanged(const QList<QRectF>& regionList) {
    // TODO: With other stuff we're doing, do we really need this function now ?

    if (!cursorUpdatePending) {
        QRectF cursorBoundingRectangle = currentCursor->boundingRectangle();

        if (cursorBoundingRectangle.width() == 0) {
            // The QRectF::intersects method will not detect intersections with zero width rectangles.  To overcome
            // this, we give the rectangle a very small width if the width is reported as 0.  Note that a value that is
            // too small but still non-zero causes the QRectF::intersects method to still fail.

            cursorBoundingRectangle.setWidth(0.1);
        }

        QList<QRectF>::const_iterator it  = regionList.constBegin();
        QList<QRectF>::const_iterator end = regionList.constEnd();

        while (it != end && !it->intersects(cursorBoundingRectangle)) {
            ++it;
        }

        if (it != end) {
            // TODO: Do we want to defer updates if this editor does not have focus ?
            currentCursor->forceScreenUpdate(true);
        }
    }
}


void Editor::pageFormatChanged(unsigned long, QSharedPointer<Ld::PageFormat>, QSharedPointer<Ld::PageFormat>) {
    pageFormatsSignalAggregator->forceTrigger<Editor>(this);
}


void Editor::descendantFormatWasChanged(Ld::ElementPointer, Ld::FormatPointer) {
    elementFormatsSignalAggregator->forceTrigger<Editor>(this);
}


void Editor::processCursorUpdate() {
    if (document()->arePresentationUpdatesPending()) {
        cursorUpdatePending = true;
    } else {
        updateCursorPosition();
    }
}


void Editor::blinkCursor() {
    int cursorFlashInterval = Application::cursorFlashTime();

    if (cursorFlashInterval > 0) {
        cursorTimer->start(cursorFlashInterval / 2.0);
        currentShowCursor = !currentShowCursor;
    } else {
        currentShowCursor = true;
    }

    updateScene(QList<QRectF>() << previousCursorBoundingRectangle);
}


void Editor::configureWidget(Document* newDocument) {
    marginsAreEnabled = defaultMarginsAreVisible;
    guidesAreEnabled  = defaultGuidesAreVisible;

    /*
     * If we ever enable OpenGL support, we should do so here using the line:
     *
     *     setViewport(new QOpenGLWidget);
     *
     * Appears that using OpenGL at this time has several downsides:
     *
     *     - Using OpenGL with Qt5.7 causes refresh to fail on windows.  Forcing a refresh does not fix the problem.
     *     - Using OpenGL on MacOS provides for very smooth scrolling; however, OpenGL seems to cause the composition
     *       modes to fail which breaks the cursor implementation.  While it would be nice to fix this eventually,
     *       I've oped to simply avoid OpenGL completely, for now.
     */

    setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    setRenderHints(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QPalette updatedPalette = palette();
    updatedPalette.setColor(QPalette::Base, PageList::shadowColor2);
    setPalette(updatedPalette);
    setAutoFillBackground(true);

    pageFormatsSignalAggregator = new EQt::SignalAggregator(this);
    pageFormatsSignalAggregator->registerConnection(this, this, &Editor::pageFormatsChanged);

    elementFormatsSignalAggregator = new EQt::SignalAggregator(this);
    elementFormatsSignalAggregator->registerConnection(this, this, &Editor::elementFormatsChanged);

    cursorTimer = new QTimer();
    cursorTimer->setSingleShot(true);

    connect(cursorTimer, &QTimer::timeout, this, &Editor::blinkCursor);

    currentShowCursor = true;

    currentCursor.reset(new Cursor(newDocument));
    connect(currentCursor.data(), &Cursor::cursorUpdated,       this, &Editor::processCursorUpdate);
    connect(currentCursor.data(), &Cursor::elementStackChanged, this, &Editor::elementStackChanged);
    cursorUpdatePending = false;

    setDocument(newDocument);
}


void Editor::pageMove(QKeyEvent* event) {
    bool selection;

    if (event->matches(QKeySequence::MoveToNextPage) || event->matches(QKeySequence::MoveToPreviousPage)) {
        selection = false;
    } else if (event->matches(QKeySequence::SelectNextPage) || event->matches(QKeySequence::SelectPreviousPage)) {
        selection = true;
    } else {
        selection = false;
        Q_ASSERT(false); // Method called from an invalid key event.
    }

    QPointF cursorLocationInScene = currentCursor->cursorPosition();
    QPoint  cursorLocationInView  = mapFromScene(cursorLocationInScene);

    QGraphicsView::keyPressEvent(event); // Perform the page movement operation.

    QRectF  visibleSceneRect           = mapToScene(frameRect()).boundingRect();
    QPointF idealCursorLocationInScene = mapToScene(cursorLocationInView);

    currentCursor->moveNearPosition(visibleSceneRect, idealCursorLocationInScene, selection);
}


void Editor::updateAllowedActions(bool alwaysGenerateSignals) {
    const Ld::Capabilities newParentRequirements = currentCursor->requirementsAtCursor();
    Cursor::Type           newCursorType         = currentCursor->cursorType();

    if (alwaysGenerateSignals || currentCursorType != newCursorType) {
        currentCursorType = newCursorType;

        emit cursorTypeChanged(newCursorType);

        bool makeVisible = (newCursorType == Cursor::Type::TEXT);
        emit cursorBarVisible(makeVisible);
    }

    if (alwaysGenerateSignals || currentParentRequirements != newParentRequirements) {
        currentParentRequirements = newParentRequirements;
        emit requirementsChanged(newParentRequirements);
    }
}


void Editor::updateCursorPosition() {
    QRectF cursorBoundingRectangle = currentCursor->boundingRectangle().marginsAdded(QMarginsF(1, 1, 1, 1));
    QRectF sceneBoundingRectangle;

    ensureVisible(cursorBoundingRectangle, cursorMarginX, cursorMarginY);

    if (previousCursorBoundingRectangle.isNull()) {
        sceneBoundingRectangle = cursorBoundingRectangle;
    } else {
        sceneBoundingRectangle = cursorBoundingRectangle.united(previousCursorBoundingRectangle);
    }

    previousCursorBoundingRectangle = cursorBoundingRectangle;

    updateScene(QList<QRectF>() << sceneBoundingRectangle);

    currentShowCursor = true;

    int cursorFlashInterval = Application::cursorFlashTime();
    if (cursorFlashInterval > 0 && hasFocus()) {
        cursorTimer->start(cursorFlashInterval / 2.0);
    }

    emit cursorSceneCoordinatePositionChanged(cursorBoundingRectangle.left());
    emit cursorUpdated();
}
