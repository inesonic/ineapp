/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Document class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QSharedPointer>
#include <QBitArray>
#include <QMutex>
#include <QFileInfo>
#include <QSet>
#include <QMap>
#include <QList>
#include <QBitArray>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QLineF>
#include <QObject>

#include <ld_element.h>
#include <ld_root_element.h>

#include "app_common.h"
#include "cursor.h"
#include "scene_units.h"
#include "command.h"
#include "command_container.h"
#include "root_presentation.h"

class QTimer;

class Editor;
class CommandQueue;

/**
 * Class that tracks the contents of a single document.  This class acts as a wrapper for both the root element and the
 * root presentation.  Key methods of the root presentation are exposed through inheritance and key methods of the root
 * element are marshalled by this class.
 *
 * The class will automatically instantiate an instance of the root element, largely hiding the functionality of the
 * element tree.
 *
 * A document can be viewed and manipulated using the associated \ref Editor class.
 *
 * Note that you should check for a non-empty error string after instantiating a new document instance as that
 * indicates that needed temporary files could not be created.
 */
class APP_PUBLIC_API Document:public RootPresentation {
    Q_OBJECT

    public:
        /**
         * Type used to identify documents.
         */
        typedef QString Identifier;

        /**
         * Iterator object used to traverse the set of editors.
         */
        typedef QSet<Editor*>::iterator EditorIterator;

        /**
         * The maximum length to allow for a shortform name.
         */
        static constexpr unsigned maximumShortformNameLength = 30;

        /**
         * Constructor.
         *
         * \param[in] parent Pointer to the parent object.
         */
        explicit Document(QObject* parent = Q_NULLPTR);

        ~Document() override;

        /**
         * Static method that creates an instance of a \ref RootPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

        /**
         * Method that populates the current document with a default set of elements if the document is completely
         * empty.  The method will also mark the document as pristine after populating it.
         */
        void fillEmptyDocument();

        /**
         * Method you can use to set the command queue to be used by this class.  Note that a \ref CommandQueue
         * instance will be instantiated by default.
         *
         * Note that changing the command queue will clear the undo/redo stack.
         *
         * \param[in] newCommandQueue The new command queue.  Note that this class will take ownership of the command
         *                            queue.
         */
        void setCommandQueue(CommandQueue* newCommandQueue);

        /**
         * Method you can use to access the current command queue.
         *
         * \return Returns a pointer to the current command queue.
         */
        CommandQueue* commandQueue() const;

        /**
         * Convenience method you can use to notify the command queue about a new cursor.
         *
         * \param[in] newCursor Pointer to the new cursor to be modified during operations.
         *
         * \return Returns true if the cursor is new.  Returns false if the cursor is already being tracked.
         */
        bool addCursor(CursorPointer newCursor);

        /**
         * Convenience method you can use to notify the command queue that a cursor is not longer to be modified.
         *
         * \param[in] oldCursor Pointer to the new cursor to be modified during operations.
         *
         * \return Returns true if the cursor was removed from the list.  Returns false if the cursor was not
         *         being updated by commands.
         */
        bool removeCursor(CursorPointer oldCursor);

        /**
         * Method that can be used to determine if the document is empty.  An empty document is defined as having only a
         * single page containing no other items.
         *
         * \return Returns true if the document is empty.  Returns false if the document has content.
         */
        bool isEmpty() const;

        /**
         * Method that can be used to determine if the document is not empty.
         *
         * \return Returns true if the document has content.  Returns false if the document is empty.
         */
        bool isNotEmpty() const;

        /**
         * Method that can be used to determine if the document is modified and should be saved to media.
         *
         * \return Returns true if the document is modified, returns false if the document is pristine.
         */
        bool isModified() const;

        /**
         * Method that can be used to determine if the document is pristine and unmodified.
         *
         * \return Returns true if the document is pristine, returns false if the document is modified.
         */
        bool isPristine() const;

        /**
         * Method that determines if this document can be safely replaced within a view.
         *
         * \return Returns true if the document can be safely swapped out.  Returns false if the document should not be
         *         replaced.
         */
        bool isReplaceable() const;

        /**
         * Method that returns the current size of the underlying undo stack.
         *
         * \return Returns the size of the undo stack.
         */
        unsigned long undoStackSize() const;

        /**
         * Method that returns the current size of the redo stack.
         *
         * \return Returns the size of the redo stack.
         */
        unsigned long redoStackSize() const;

        /**
         * Method you can use to determine if undo operations are available.
         *
         * \return Returns true if undo operations are available.  Returns false if no undo operations are available.
         */
        bool canUndo() const;

        /**
         * Method you can use to determine if redo operations are available.
         *
         * \return Returns true if undo operations are available.  Returns false if no undo operations are available.
         */
        bool canRedo() const;

        /**
         * Method that returns a specific command in the undo or redo stack.
         *
         * \param[in] index The index to the desired command.  Entry 0 is the most recently completed command.  Values
         *                  less than 0 return older executed commands.  Values greater than zero return commands in the
         *                  redo stack.
         *
         * \return Returns a reference to a command container representing the command.  An invalid command container
         *         is returned if the index value is invalid.
         */
        const CommandContainer& undoRedoStackContentsAt(long index) const;

        /**
         * Loads this document from a file.
         *
         * \param[in] filename The filename of the file containing this document.
         */
        bool loadDocument(const QString& filename);

        /**
         * Saves the document.
         *
         * \return Returns true on success, false on failure.
         */
        bool saveDocument();

        /**
         * Saves the document under a new name.
         *
         * \param[in] newFilename The new name to assign to the document.
         *
         * \return Returns true on success, false on failure.
         */
        bool saveDocument(const QString& newFilename);

        /**
         * Determines the filename associated with this document.
         *
         * \return Returns the document filename.  Note that, if this document is new and has not been saved, an empty
         *         string is returned.
         */
        QString filename() const;

        /**
         * Determines the short form name of this document to present on tabs and other user interface items.
         *
         * \return Returns the document filename.  Note that, if this document is new and has not been saved, a default
         *         name will be constructed.
         */
        QString shortformName() const;

        /**
         * Calculates a unique identifier for this document based on the document's filename or the document's current
         * default name.
         *
         * \return Returns a unique identifier for the document.
         */
        Identifier identifier() const;

        /**
         * Returns a string holding the last reported error.
         *
         * \return Returns the last reported error.
         */
        QString lastError() const;

        /**
         * Method that provides an iterator to the first editor that is presenting this document.
         *
         * \return Returns an instance of \ref Document::EditorIterator
         */
        EditorIterator beginEditors();

        /**
         * Method that provides an iterator past the last editor that is presenting this document.
         *
         * \return Returns an instance of \ref Document::EditorIterator that can be used to determine when the last
         *         \ref Editor instance has been passed.
         */
        EditorIterator endEditors();

        /**
         * Method that determines the number of editors that are presenting this document.
         *
         * \return Returns a count of the number of editors viewing this document.
         */
        unsigned countEditors() const;

        /**
         * Method that registers an editor with this document.
         *
         * \param[in] editor The editor to be registered with the document.
         */
        void registerEditor(Editor* editor);

        /**
         * Method that unregisters an editor with the document.
         *
         * \param[in] editor The editor to be unregistered from the document.
         */
        void unregisterEditor(Editor* editor);

        /**
         * Method that returns a list of documents referencing this document.
         *
         * \return Returns a list of documents referencing this document.
         */
        QList<Document*> referencedBy() const;

        /**
         * Method that returns a list of documents that this document references.
         *
         * \return Returns a list of documents that this document is currently referencing.
         */
        QList<Document*> referencing() const;

        /**
         * Method that indicates if this element is a top level element.
         *
         * \return Returns true if the element is at the top of the Element heirarchy.  The root object is of type
         *         \ref Document and has no parent.  This version always returns true.
         */
        bool isDocumentRoot() const;

        /**
         * Calculates a unique identifier for the document that would be associated with a specified file.
         *
         * \param[in] fileInformation The file information instance to calculate a unique identifier for.  Note that the
         *                            file information structure must point to a valid file.
         *
         * \return Returns a unique identifier that will apply to the document, once opened.
         */
        static Identifier identifier(const QFileInfo& fileInformation);

        /**
         * Method that locates a document by file information.
         *
         * \param[in] fileInformation The file information to use to locate the document.  A null pointer is returned if
         *                            the document does not exist.
         */
        static Document* document(const QFileInfo& fileInformation);

        /**
         * Method that locates a document by identifier.
         *
         * \param[in] documentIdentifier The document identifier used to locate the document.
         */
        static Document* document(const Identifier& documentIdentifier);

        /**
         * Method that returns a list containing all of the currently loaded documents.
         *
         * \return Returns a QList of documents.
         */
        static QList<Document*> documents();

        /**
         * Method that purges all unreferenced documents from the system.
         *
         * \param[in] referencedDocuments A list of documents that are known to be referenced.  This list is used as
         *                                a starting point to walk dependencies in order to identify other referenced
         *                                documents.  Documents and the related root elements will be purged if their
         *                                not referenced in this list and are not dependencies to this list.
         */
        static void purgeUnreferenced(const QList<Document*>& referencedDocuments);

    signals:
        /**
         * Signal that is emitted when the document filename is updated.  This signal is emitted by the
         * \ref Document::saveDocument(const QString&) and \ref Document::loadDocument methods.
         *
         * \param[out] newFilename The new filename associated with this document.
         */
        void filenameChanged(const QString& newFilename);

        /**
         * Signal that is emitted when the document's shortform name is updated.  This signal is emitted by the
         * \ref Document::saveDocument(const QString&) and \ref Document::loadDocument methods.
         *
         * \param[out] newShortformName The new filename associated with this document.
         */
        void shortformNameChanged(const QString& newShortformName);

        /**
         * Signal that is emitted when the identifier for a file changes.    This signal is emitted by the
         * \ref Document::saveDocument(const QString&) and \ref Document::loadDocument methods.
         *
         * \param[out] oldIdentifier The old document identifier.
         *
         * \param[out] newIdentifier The new document identifier.
         */
        void identifierChanged(const Document::Identifier& oldIdentifier, const Document::Identifier& newIdentifier);

        /**
         * Signal that is emitted when an element is modified or when the modification status is reset.
         *
         * \param[out] nowModified If true, one or more elements are marked as modified.  If false, all elements are now
         *                         pristine.
         */
        void modificationChanged(bool nowModified);

        /**
         * Signal that is emitted whenever undo status changes.
         *
         * \param[in] nowCanUndo If true, we can now perform an undo operation.  If false, there are no operations to
         *                       be undone.
         */
        void undoAvailable(bool nowCanUndo);

        /**
         * Signal that is emitted whenever redo status changes.
         *
         * \param[in] nowCanRedo If true, we can now perform an redo operation.  If false, there are no operations to
         *                       be redone.
         */
        void redoAvailable(bool nowCanRedo);

        /**
         * Signal that is emitted when the undo/redo stack changes.
         *
         * \param[out] undoStackSize The new size of the undo stack.
         *
         * \param[out] redoStackSize The new size of the redo stack.
         */
        void undoRedoStackChanged(unsigned long undoStackSize, unsigned long redoStackSize);

        /**
         * Signal that is emitted when a command fails.
         *
         * \param[out] failedCommand The command that could not be performed.
         */
        void commandFailed(const CommandContainer& failedCommand);

        /**
         * Signal that is emitted when an undo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.
         */
        void undoFailed(const CommandContainer& failedCommand);

        /**
         * Signal that is emitted when a redo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.
         */
        void redoFailed(const CommandContainer& failedCommand);

    public slots:
        /**
         * Slot that inserts a new command into the command queue.
         *
         * \param[in] newCommand The command to be inserted into the command queue.  The class will take ownership of
         *                       the command.
         */
        void insertCommand(Command* newCommand);

        /**
         * Slot that inserts a new command into the command queue.
         *
         * \param[in] newCommand The command to be inserted into the command queue.
         */
        void insertCommand(QSharedPointer<Command> newCommand);

        /**
         * Slot that inserts a new command into the command queue.
         *
         * \param[in] newCommand The command to be inserted into the command queue.
         */
        void insertCommand(const CommandContainer& newCommand);

        /**
         * Slot that performs an undo operation.
         */
        void undo();

        /**
         * Slot that performs a redo operation.
         */
        void redo();

    private slots:
        /**
         * Slot that can be triggered to mark all elements as pristine.
         */
        void processNowPristine() final;

        /**
         * Slot that can be triggered to mark one or more elements as modified.
         */
        void processNowModified() final;

        /**
         * Slot that is triggered whenever the root element or any child is modified, including changes to references.
         * This slot will not be triggered when the root element is loaded or saved.
         */
        void processNowChanged() final;

        /**
         * Slot that is triggered by the early repositioning timer.
         */
        void performEarlyRepositioning();

        /**
         * Slot that is triggered by the late repositioning timer.
         */
        void performLateRepositioning();

    private:
        /**
         * Value used to indicate an invalid document number.
         */
        static constexpr unsigned invalidDocumentNumber = static_cast<unsigned>(-1);

        /**
         * Minimum delay between a repositioning request and the start of the repositioning operation.  The value is in
         * mSec.
         */
        static constexpr unsigned earlyRepositionDelayMSec = 200;

        /**
         * Maximum delay between a repositioning request and the start of the repositioning operation.  The value is in
         * mSec.
         */
        static constexpr unsigned lateRepositioningDelayMSec = 1000;

        /**
         * Method used internally to flag that one or more elements have been modified or to mark that all elements are
         * now prisine.
         *
         * \param[in] nowModified If true, the document will now report non-pristine/modified status.  If false, the
         *                        document will now report pristine status.
         */
        void setModified(bool nowModified);

        /**
         * Editors that are displaying this document.
         */
        QSet<Editor*> currentEditors;

        /**
         * Boolean used to indicate if the document is modified.
         */
        bool modified;

        /**
         * The file information structure used to associate this document with a file.  An empty/null value indicates
         * that the document is new and has not been assigned a filename.
         */
        QFileInfo currentFileInformation;

        /**
         * Timer used to delay repositioning events.  This timer allows this class to reduce the number of times
         * elements get repositioned during operation.  This timer will be reset every time repositioning is requested.
         */
        QTimer* earlyRepositionElementTimer;

        /**
         * Value used for debugging to slowing insert text.  Used to test the underlying repositioing engine.
         */
        unsigned debugInsertCount;

        /**
         * Timer used to delay repositioning events.  This timer allows this class to reduce the number of times
         * elements get repositioned during operation.  This timer exists to guarantee that we reposition elements at
         * least ocassionally.  The timer will not be reset once it's started and will be started with a longer timeout
         * than the early timer.
         */
        QTimer* lateRepositionElementTimer;

        /**
         * The maximum horizontal extent across all the pages managed by this document.  The value is used to
         * report the required ruler length.
         */
        double currentMaximumHorizontalExtentPoints;

        /**
         * The command queue for commands associated with this document.
         */
        CommandQueue* currentCommandQueue;
};

#endif
