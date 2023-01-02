/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Document class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QObject>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QBitArray>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <QList>
#include <QTransform>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QLineF>
#include <QTimer>
#include <QGraphicsView>

#include <ld_element_structures.h>
#include <ld_element.h>
#include "ld_paragraph_element.h"
#include "ld_paragraph_format.h"
#include "ld_text_element.h"
#include "ld_character_format.h"
#include <ld_root_element.h>

#include "application.h"
#include "plug_in_manager.h"
#include "cursor.h"
#include "command.h"
#include "command_container.h"
#include "command_queue.h"
#include "editor.h"
#include "scene_units.h"
#include "document.h"

Document::Document(QObject* parent):RootPresentation(parent) {
    currentCommandQueue = new CommandQueue(CommandQueue::infiniteStackDepth, this);

    connect(currentCommandQueue, &CommandQueue::undoIsAvailable, this, &Document::undoAvailable);
    connect(currentCommandQueue, &CommandQueue::redoIsAvailable, this, &Document::redoAvailable);
    connect(
        currentCommandQueue,
        static_cast<void (CommandQueue::*)(unsigned long, unsigned long)>(&CommandQueue::queueChanged),
        this,
        static_cast<void (Document::*)(unsigned long, unsigned long)>(&Document::undoRedoStackChanged)
    );
    connect(currentCommandQueue, &CommandQueue::commandHasFailed, this, &Document::commandFailed);
    connect(currentCommandQueue, &CommandQueue::undoHasFailed, this, &Document::undoFailed);
    connect(currentCommandQueue, &CommandQueue::redoHasFailed, this, &Document::redoFailed);

    currentMaximumHorizontalExtentPoints = 0;
    modified                             = false;

    earlyRepositionElementTimer = new QTimer(this);
    lateRepositionElementTimer  = new QTimer(this);

    earlyRepositionElementTimer->setSingleShot(true);
    lateRepositionElementTimer->setSingleShot(true);

    connect(earlyRepositionElementTimer, SIGNAL(timeout()), this, SLOT(performEarlyRepositioning()));
    connect(lateRepositionElementTimer, SIGNAL(timeout()), this, SLOT(performLateRepositioning()));

    // Debug starts

    earlyRepositionElementTimer->start(1000);
    debugInsertCount = 0;

    // Debug ends
}


Document::~Document() {
    QSharedPointer<Ld::RootElement> rootElement = element();
    if (!rootElement.isNull() && rootElement->openMode() != Ld::RootElement::OpenMode::CLOSED) {
        rootElement->close();
    }
}


Ld::Visual* Document::creator(const QString&) {
    return new Document();
}


void Document::fillEmptyDocument() {
    QSharedPointer<Ld::RootElement> rootElement = element();
    if (rootElement->isEmpty()) {
        QSharedPointer<Ld::ParagraphElement> paragraph = Ld::Element::create(
            Ld::ParagraphElement::elementName
        ).dynamicCast<Ld::ParagraphElement>();

        paragraph->setFormat(Ld::Format::create(Ld::ParagraphFormat::formatName).dynamicCast<Ld::ParagraphFormat>());

        rootElement->append(paragraph, nullptr);

        QSharedPointer<Ld::TextElement> textElement = Ld::Element::create(
            Ld::TextElement::elementName
        ).dynamicCast<Ld::TextElement>();

        textElement->setFormat(rootElement->defaultTextFormat()->clone());
        paragraph->append(textElement, nullptr);

        rootElement->markPristine();
    }
}


void Document::setCommandQueue(CommandQueue* newCommandQueue) {
    if (currentCommandQueue != Q_NULLPTR) {
        delete currentCommandQueue;
    }

    currentCommandQueue = newCommandQueue;
    if (currentCommandQueue != Q_NULLPTR) {
        currentCommandQueue->setParent(this);
    }
}


CommandQueue* Document::commandQueue() const {
    return currentCommandQueue;
}


bool Document::addCursor(CursorPointer newCursor) {
    return currentCommandQueue->addCursor(newCursor);
}


bool Document::removeCursor(CursorPointer oldCursor) {
    return currentCommandQueue->removeCursor(oldCursor);
}


bool Document::isEmpty() const {
    bool result = false;

    QSharedPointer<Ld::RootElement> rootElement = element();
    if (rootElement->numberChildren() == 0) {
        result = true; // We should not see this case.
    } else if (rootElement->numberChildren() == 1) {
        // Check if we have a single paragraph with a single empty text element.

        Ld::ElementPointer element = rootElement->child(0);
        if (element->typeName() == Ld::ParagraphElement::elementName) {
            QSharedPointer<Ld::ParagraphElement> paragraph = element.dynamicCast<Ld::ParagraphElement>();
            if (paragraph->numberChildren() == 1) {
                element = paragraph->child(0);
                if (element->typeName() == Ld::TextElement::elementName) {
                    QSharedPointer<Ld::TextElement> textElement = element.dynamicCast<Ld::TextElement>();
                    result = textElement->text().isEmpty();
                }
            }
        }
    }

    return result;
}


bool Document::isNotEmpty() const {
    return !isEmpty();
}


bool Document::isModified() const {
    return modified;
}


bool Document::isPristine() const {
    return !isModified();
}


bool Document::isReplaceable() const {
    return element()->isNew() && isEmpty();
}


unsigned long Document::undoStackSize() const {
    return currentCommandQueue->undoStackSize();
}


unsigned long Document::redoStackSize() const {
    return currentCommandQueue->redoStackSize();
}


bool Document::canUndo() const {
    return currentCommandQueue->canUndo();
}


bool Document::canRedo() const {
    return currentCommandQueue->canRedo();
}


const CommandContainer& Document::undoRedoStackContentsAt(long index) const {
    return currentCommandQueue->at(index);
}


bool Document::loadDocument(const QString& filename) {
    bool                            success     = true;
    QSharedPointer<Ld::RootElement> rootElement = element();

    if (rootElement->openMode() != Ld::RootElement::OpenMode::CLOSED) {
        success = rootElement->close();
    }

    if (success) {
        Ld::PlugInsByName plugInsByName = Application::plugInManager()->plugInsByName();
        success = rootElement->openExisting(filename, false, plugInsByName);
    }

    fillEmptyDocument(); // This exists here in case the load fails.

    return success;
}


bool Document::saveDocument() {
    QSharedPointer<Ld::RootElement> rootElement = element();
    return rootElement->save();
}


bool Document::saveDocument(const QString& newFilename) {
    QSharedPointer<Ld::RootElement> rootElement = element();
    bool success = rootElement->saveAs(newFilename);

    return success;
}


QString Document::filename() const {
    return element()->filename();
}


QString Document::shortformName() const {
    return element()->shortformName();
}


QString Document::identifier() const {
    return element()->identifier();
}


QString Document::lastError() const {
    QSharedPointer<Ld::RootElement> rootElement = element();
    return rootElement->errorString();
}


Document::EditorIterator Document::beginEditors() {
    return currentEditors.begin();
}


Document::EditorIterator Document::endEditors() {
    return currentEditors.end();
}


unsigned Document::countEditors() const {
    return currentEditors.count();
}


void Document::registerEditor(Editor* editor) {
    currentEditors.insert(editor);
}


void Document::unregisterEditor(Editor* editor) {
    currentEditors.remove(editor);
}


QList<Document*> Document::referencedBy() const {
    return QList<Document*>(); // FIXME
}


QList<Document*> Document::referencing() const {
    return QList<Document*>(); // FIXME
}


bool Document::isDocumentRoot() const {
    return true;
}


Document::Identifier Document::identifier(const QFileInfo& fileInformation) {
    QString name = fileInformation.canonicalFilePath();

    if (name.isEmpty()) {
        name = fileInformation.absoluteFilePath();
    }

    return name;
}


Document* Document::document(const QFileInfo& fileInformation) {
    return document(identifier(fileInformation));
}


Document* Document::document(const Document::Identifier& documentIdentifier) {
    Document*                       result      = Q_NULLPTR;
    QSharedPointer<Ld::RootElement> rootElement = Ld::RootElement::byIdentifier(documentIdentifier);

    if (!rootElement.isNull()) {
        result = dynamic_cast<Document*>(rootElement->visual());
    }

    return result;
}


QList<Document*> Document::documents() {
    QList<Document*> result;

    Ld::RootElement::RootElementList roots = Ld::RootElement::rootElements();
    for (  Ld::RootElement::RootElementList::const_iterator it = roots.constBegin(), end = roots.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::RootElement> rootElement = *it;
        Document*                       document    = dynamic_cast<Document*>(rootElement->visual());
        if (document != Q_NULLPTR) {
            result.append(document);
        }
    }

    return result;
}


void Document::purgeUnreferenced(const QList<Document*>& referencedDocuments) {
    QSet<QString> allRequiredDependencies;
    for (  QList<Document*>::const_iterator documentIterator    = referencedDocuments.constBegin(),
                                            documentEndIterator = referencedDocuments.constEnd()
         ; documentIterator != documentEndIterator
         ; ++documentIterator
        ) {
        Document*                        document        = *documentIterator;
        QSharedPointer<Ld::RootElement>  rootElement     = document->element();
        Ld::RootElement::RootElementList allDependencies = rootElement->allDependencies();

        allRequiredDependencies.insert(rootElement->identifier());
        for (  Ld::RootElement::RootElementList::const_iterator dependencyIterator    = allDependencies.constBegin(),
                                                                dependencyEndIterator = allDependencies.constEnd()
             ; dependencyIterator != dependencyEndIterator
             ; ++dependencyIterator
            ) {
            QSharedPointer<Ld::RootElement> rootElement = *dependencyIterator;
            QString                         identifier  = rootElement->identifier();
            allRequiredDependencies.insert(identifier);
        }
    }

    Ld::RootElement::RootElementList allRootElements = Ld::RootElement::rootElements();
    for (  Ld::RootElement::RootElementList::const_iterator rootIterator    = allRootElements.constBegin(),
                                                            rootEndIterator = allRootElements.constEnd()
         ; rootIterator != rootEndIterator
         ; ++rootIterator
        ) {
        QSharedPointer<Ld::RootElement> rootElement = *rootIterator;
        QString                         identifier  = rootElement->identifier();

        if (!allRequiredDependencies.contains(identifier)) {
            Ld::RootElement::unregisterRootElement(rootElement);
        }
    }
}


void Document::insertCommand(Command* newCommand) {
    currentCommandQueue->insertCommand(newCommand);
}


void Document::insertCommand(QSharedPointer<Command> newCommand) {
    currentCommandQueue->insertCommand(newCommand);
}


void Document::insertCommand(const CommandContainer& newCommand) {
    currentCommandQueue->insertCommand(newCommand);
}


void Document::undo() {
    currentCommandQueue->undo();
}


void Document::redo() {
    currentCommandQueue->redo();
}


void Document::processNowPristine() {
    setModified(false);
}


void Document::processNowModified() {
    setModified(true);
}


void Document::processNowChanged() {} // TODO: Do we need to do anything here ?


void Document::performEarlyRepositioning() {
}


void Document::performLateRepositioning() {
//    earlyRepositionElementTimer->stop();
//    performRepositioning(topmostLocation);
}


void Document::setModified(bool nowModified) {
    if (nowModified != modified) {
        modified = nowModified;
        emit modificationChanged(nowModified);
    }
}
