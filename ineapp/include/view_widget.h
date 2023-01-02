/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ViewWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VIEW_WIDGET_H
#define VIEW_WIDGET_H

#include <QObject>
#include <QSize>
#include <QResizeEvent>
#include <QTimer>
#include <QList>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QHash>

#include <eqt_programmatic_view.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_capabilities.h>
#include <ld_format.h>
#include <ld_aggregations_by_capability.h>

#include "app_common.h"
#include "command.h"
#include "command_container.h"

#include "cursor.h"
#include "page_list.h"

class QKeyEvent;

namespace Ld {
    class Element;
    class Format;
    class PageFormat;
    class DocumentSetting;
    class ElementCursor;
}

class MainWindow;
class Document;
class RootPresentation;

/**
 * Class that manages document views within the application \ref MainWindow.
 */
class APP_PUBLIC_API ViewWidget:public EQt::ProgrammaticView {
    Q_OBJECT

    public:
        /**
         * Type used to represent the result from a search.
         */
        typedef Cursor::SearchResult SearchResult;

        /**
         * Constuctor
         *
         * \param[in] mainWindow The main window that this view is within.
         *
         * \param[in] parent     The parent object that owns this widget.
         */
        explicit ViewWidget(MainWindow* mainWindow, QWidget* parent = Q_NULLPTR);

        /**
         * Constuctor
         *
         * \param[in] newDocument The document that should be managed by this view.
         *
         * \param[in] mainWindow  The main window that this view is within.
         *
         * \param[in] parent      The parent object that owns this widget.
         */
        ViewWidget(Document* newDocument, MainWindow* mainWindow, QWidget* parent = Q_NULLPTR);

        ~ViewWidget() override;

        /**
         * Method you can use to determine if this view is the currently active view.
         *
         * \return Returns true if this view is the currently active view.  Returns false if this view is not the
         *         currently active view.
         */
        bool isActiveView() const;

        /**
         * Method that returns the main window associated with this view.
         *
         * \return Returns a pointer to the main window.
         */
        MainWindow* window() const;

        /**
         * Sets the document being managed by this view.  Note that the previous document will be decoupled from the
         * underlying editor window.  This method will not make any attempt to determine the disposition of the
         * previously displayed document.
         *
         * \param[in] newDocument the document to be managed by this view.
         */
        void setDocument(Document* newDocument);

        /**
         * Determines the document instance being managed by this view.
         *
         * \return Returns a pointer to the document being managed by this view.
         */
        Document* document() const;

        /**
         * Obtains the current document filename.
         *
         * \return Returns the current document filename.
         */
        QString filename() const;

        /**
         * Obtains the current document short form name.
         *
         * \return Returns the current document short form name.
         */
        QString shortformName() const;

        /**
         * Obtains a title for this view that is suitable for use in tabs and window titles.  The title will either be
         * the basename of the filename or the shortform name if the filename is empty.
         *
         * \return Returns the title for this view widget.
         */
        QString title() const;

        /**
         * Obtains the current document's identifier.
         *
         * \return Returns the current document identifier.
         */
        QString identifier() const;

        /**
         * Method that determines if the underlying document is in the modified or unmodified state.
         *
         * \return Returns true if the document is in the modified state.  Returns false if the document is in the
         *         unmodified state.
         */
        bool isModified() const;

        /**
         * Method that can be used to determine if the underlying document is pristine and unmodified.
         *
         * \return Returns true if the document is pristine, returns false if the document is modified.
         */
        inline bool isNotModified() const {
            return !isModified();
        }

        /**
         * Method that determines if the currently loaded document is replaceable.
         *
         * \return Returns true if the document is replaceable.  Returns false if the document is not repleaceable.
         */
        bool isReplaceable() const;

        /**
         * Method that determines if the underlying document should be saved before being replaced within a view.
         *
         * \return Returns true if the document contains critical unsaved changes.  Returns false if the document can be
         *         safely swapped out and deleted.
         */
        inline bool isNotReplaceable() const {
            return !isReplaceable();
        }

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
         * Determines the current zoom factor.
         *
         * \return Returns the current zoom factor.  A value of 1 indicates 100%.
         */
        float zoom() const;

        /**
         * Method that determines if the margins on pages are being shown.
         *
         * \return Returns true if the margins are shown.  Returns false if the margins are hidden.
         */
        bool marginsEnabled() const;

        /**
         * Method that determines if the margins on pages are being hidden.
         *
         * \return Returns true if the margins are hidden.  Returns false if the margins are shown.
         */
        inline bool marginsDisabled() const {
            return !marginsEnabled();
        }

        /**
         * Method that determines if the guides on pages are being shown.
         *
         * \return Returns true if the guides are shown.  Returns false if the guides are hidden.
         */
        bool guidesEnabled() const;

        /**
         * Method that determines if the guides on pages are being hidden.
         *
         * \return Returns true if the guides are hidden.  Returns false if the guides are shown.
         */
        inline bool guidesDisabled() const {
            return !guidesEnabled();
        }

        /**
         * Method you can use to access the cursor.
         *
         * \return Returns a reference to the current cursor.
         */
        const Cursor& cursor() const;

        /**
         * Method you can use to update the cursor position.
         *
         * \param[in] elementCursor The new cursor position.  No adjustment of the cursor position is performed if the
         *                          cursor position is invalid.
         */
        void updateCursorPosition(const Ld::ElementCursor& elementCursor);

        /**
         * Method that returns the parent requirements at the current cursor position.
         *
         * \return Returns the current parent requirements.
         */
        Ld::Capabilities requirementsAtCursor() const;

        /**
         * Method that returns the cursor type at the current cursor position.
         *
         * \return Returns the current cursor type.
         */
        Cursor::Type cursorType() const;

        /**
         * Method you can use to select the entire document contents.
         */
        void selectEntireDocument();

        /**
         * Method you can use to obtain a list of every element at the given cursor position.  The returned list
         * will vary depending on whether or not there is a selection.
         *
         * \param[in] includeAncestors If true, we will include ancestors.  If false, ancestors will be excluded.
         *
         * \return Returns a list of every element under the cursor.  If there is a selection, the method will
         *         return every selected element in arbitrary order.  If there is no selection, the method will
         *         return the stack of elements starting at the root element and working downward.
         */
        Ld::ElementPointerList elementsUnderCursor(bool includeAncestors = true) const;

        /**
         * Method you can use to obtain a list of every format at the given cursor position.  If there is no
         * selection, this method will return the formats from the root down to the deepest element.  If there is
         * a selection, this method will return a list of all the format objects over the span of the selection.
         *
         * This method will include all page formats over the expected span.
         *
         * \return Returns all the formats at a given position.  An empty list is returned if the position is
         *         invalid.
         */
        QList<Ld::FormatPointer> formatsAtCursor() const;

        /**
         * Method you can use to obtain a format aggregation of element element under the cursor organized by the
         * capabilities supported by each format.
         *
         * \return Returns a map of formats keyed by format type where each entry a a list of shared pointers to
         *         elements.  The returned aggregation will include ancestors.
         */
        Ld::AggregationsByCapability aggregationsByCapabilityWithAncestors() const;

        /**
         * Method you can use to obtain a format aggregation of element element under the cursor organized by the
         * capabilities supported by each format.
         *
         * \return Returns a map of formats keyed by format type where each entry a a list of shared pointers to
         *         elements.  The returned aggregation will exclude ancestors.
         */
        Ld::AggregationsByCapability aggregationsByCapabilityWithoutAncestors() const;

        /**
         * Method you can use to obtain the page index number of pages under the cursor.
         *
         * \return Returns a list of page index values for pages currently under the cursor.
         */
        const QList<PageList::Index>& pagesUnderCursor() const;

        /**
         * Method you can use to determine the current cursor position on the screen.
         *
         * \return Returns the editor's global cursor position.
         */
        QPoint globalCursorPosition() const;

        /**
         * Method you can use to obtain the format for a given page by page index.
         *
         * \param[in] pageIndex The zero based index of the page we need the format information for.
         *
         * \return Returns a shared pointer to the desired page format.
         */
        QSharedPointer<Ld::PageFormat> pageFormat(PageList::Index pageIndex) const;

        /**
         * Method you can use to inform the view widget of the format to use at the next text insertion.
         *
         * \param[in] newFormat The new format to be applied.  This method will take ownership of this format.
         */
        void setFormatForNextTextInsertion(Ld::FormatPointer newFormat);

        /**
         * Method you can call to clear the format used for the next text insertion.
         */
        void clearFormatForNextTextInsertion();

        /**
         * Method that provides the cached format to be applied at the next text insertion.
         *
         * \param[in] elementCursor The cursor location for the format of interest.
         *
         * \return Returns a shared pointer to the format that is expected to be applied.  A null pointer will be
         *         returned if there is no cached format for the indicated location.
         */
        Ld::FormatPointer nextTextInsertionFormat(const Ld::ElementCursor& elementCursor) const;

        /**
         * Method you can use to inform the proxies that document under this view is about to be saved.
         *
         * \param[in] document Pointer to the document instance that is about to be saved.
         */
        void aboutToSave(Document* document);

        /**
         * Method you can use to inform the proxies that the document under this view was newly loaded.
         *
         * \param[in] document Pointer to the newly loaded document instance.
         */
        void loaded(Document* document);

        /**
         * Method you can use to obtain a document setting, by name.
         *
         * \param[in] settingName The name of the requested setting.
         *
         * \return Returns a shared pointer to the requested setting.  A null pointer is returned if the setting is not
         *         currently defined.
         */
        QSharedPointer<Ld::DocumentSetting> documentSettingByName(const QString& settingName) const;

        /**
         * Template method you can use to obtain a document setting cast to the correct type.
         *
         * \param[in] T The class we want to obtain the settings for.
         *
         * \return Returns a shared pointer to the requested settings instance.  The pointer will be properly cast.
         */
        template<class T> QSharedPointer<T> documentSetting() const {
            QSharedPointer<Ld::DocumentSetting> setting = documentSettingByName(T::settingName);
            return setting.dynamicCast<T>();
        }

        /**
         * Method you can use to add a new document setting.  If the setting exists, it will be replaced.
         *
         * \param[in] newSetting A shared pointer to the requested document setting.
         */
        void addDocumentSetting(QSharedPointer<Ld::DocumentSetting> newSetting);

        /**
         * Method you can call to determine if the document contents are currently being positioned.
         *
         * \return Returns true if the contents are being repositioned.  Returns false if the display of the contents
         *         are not being changed.
         */
        bool isRepositioning() const;

        /**
         * Method you can call to determine if the document contents are currently correct.
         *
         * \return Returns true if the contents are being correctly displayed.  Returns false if either the
         *         display of contents needs to be updated or the contents are actively being updated.
         */
        bool isDisplayCoherent() const;

    signals:
        /**
         * Signal that is emitted when the add tab key sequence is pressed.
         */
        void addTabKeyPressed();

        /**
         * Signal that is emitted when the backspace or back key is pressed.
         */
        void backspaceKeyPressed();

        /**
         * Signal that is emitted when the bold key sequence is pressed.
         */
        void boldKeyPressed();

        /**
         * Signal that is emitted when the close key sequence is pressed.
         */
        void closeKeyPressed();

        /**
         * Signal that is emitted when the copy key sequence is pressed.
         */
        void copyKeyPressed();

        /**
         * Signal that is emitted when the cut key sequence is pressed.
         */
        void cutKeyPressed();

        /**
         * Signal that is emitted when the "Delete" key sequence is pressed.
         */
        void deleteKeyPressed();

        /**
         * Signal that is emitted when the delete to end of line key sequence is pressed.
         */
        void deleteToEndOfLineKeyPressed();

        /**
         * Signal that is emitted when the delete to end of word key pressed.
         */
        void deleteToEndOfWordKeyPressed();

        /**
         * Signal that is emitted when the delete to start of word key pressed.
         */
        void deleteToStartOfWordKeyPressed();

        /**
         * Signal that is emitted when the delete complete line key pressed.
         */
        void deleteLineKeyPressed();

        /**
         * Signal that is emitted when the find key is pressed.
         */
        void findKeyPressed();

        /**
         * Signal that is emitted when the find next key is pressed.
         */
        void findNextKeyPressed();

        /**
         * Signal that is emitted when the find previous key is pressed.
         */
        void findPreviousKeyPressed();

        /**
         * Signal that is emitted when the forward key is pressed.
         */
        void forwardKeyPressed();

        /**
         * Signal that is emitted when the help-contents key pressed.
         */
        void helpContentsKeyPressed();

        /**
         * Signal that is emitted when the insert line separator key is pressed.
         */
        void insertLineSeparatorKeyPressed();

        /**
         * Signal that is emitted when the insert paragraph separator key is pressed.
         */
        void insertParagraphSeparatorKeyPressed();

        /**
         * Signal that is emitted when the italics key is pressed.
         */
        void italicsKeyPressed();

        /**
         * Signal that is emitted when the move to end of block key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToEndOfBlockKeyPressed(bool selection);

        /**
         * Signal that is emitted when the end of document key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToEndOfDocumentKeyPressed(bool selection);

        /**
         * Signal that is emitted when the end of line key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToEndOfLineKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move forward by character key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextCharacterKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to next line key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextLineKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to next page key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextPageKeyPressed(bool selection);

        /**
         * Signal that is emitted when the mvoe to next word key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextWordKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous character key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousCharacterKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous line key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousLineKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous page key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousPageKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous word key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousWordKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to start of block key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToStartOfBlockKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to start of document key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToStartOfDocumentKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to start of line key is pressed.
         *
         * \param[out] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToStartOfLineKeyPressed(bool selection);

        /**
         * Signal that is emitted when the "new" key is pressed.
         */
        void newKeyPressed();

        /**
         * Signal that is emitted when the "next child" key is pressed.
         */
        void nextChildKeyPressed();

        /**
         * Signal that is emitted when the "open" key is pressed.
         */
        void openKeyPressed();

        /**
         * Signal that is emitted when the paste key is pressed.
         */
        void pasteKeyPressed();

        /**
         * Signal that is emitted when the preferences key is pressed.
         */
        void preferencesKeyPressed();

        /**
         * Signal that is emitted when the "previous child" key is pressed.
         */
        void previousChildKeyPressed();

        /**
         * Signal that is emitted when the print key is pressed.
         */
        void printKeyPressed();

        /**
         * Signal that is emitted when the quit key is pressed.
         */
        void quitKeyPressed();

        /**
         * Signal that is emitted when the redo key is pressed.
         */
        void redoKeyPressed();

        /**
         * Signal that is emitted when the refresh key is pressed.
         */
        void refreshKeyPressed();

        /**
         * Signal that is emitted when the replace key is pressed.
         */
        void replaceKeyPressed();

        /**
         * Signal that is emitted when the save-as key is pressed.
         */
        void saveAsKeyPressed();

        /**
         * Signal that is emitted when the save key is pressed.
         */
        void saveKeyPressed();

        /**
         * Signal that is emitted when the select-all key is pressed.
         */
        void selectAllKeyPressed();

        /**
         * Signal that is emitted when the deselect key is pressed.
         */
        void deselectKeyPressed();

        /**
         * Signal that is emitted when the underline key is pressed.
         */
        void underlineKeyPressed();

        /**
         * Signal that is emitted when the undo key is pressed.
         */
        void undoKeyPressed();

        /**
         * Signal that is emitted when an unbound key is pressed.
         *
         * \param[out] keyCode          The key code reported by the OS.
         *
         * \param[out] modifiers        The keyboard modifiers as interpreted by Qt.
         *
         * \param[out] nativeModifiers  The OS's native key modifiers.
         *
         * \param[out] nativeScanCodes  The OS's native scan codes.
         *
         * \param[out] nativeVirtualKey The native virtual key code as reported by the OS.
         *
         * \param[out] text             A textual representation of the key as reported by the OS.
         */
        void unknownKeyPressed(
            int                   keyCode,
            Qt::KeyboardModifiers modifiers,
            unsigned long         nativeModifiers,
            unsigned long         nativeScanCodes,
            unsigned long         nativeVirtualKey,
            const QString&        text
        );

        /**
         * Signal that is emitted when the "what's this" key is pressed.
         */
        void whatsThisKeyPressed();

        /**
         * Signal that is emitted when the zoom-in key is pressed.
         */
        void zoomInKeyPressed();

        /**
         * Signal that is emitted when the zoom-out key is pressed.
         */
        void zoomOutKeyPressed();

        /**
         * Signal that is emitted when the full-screen key is pressed.
         */
        void fullScreenKeyPressed();

        /**
         * Signal that is emitted when the cancel key is pressed.
         */
        void cancelKeyPressed();

        /**
         * Signal that is emitted when a text/character key is pressed.
         *
         * \param[out] keyCode          The key code reported by the OS.
         *
         * \param[out] modifiers        The keyboard modifiers as interpreted by Qt.
         *
         * \param[out] nativeModifiers  The OS's native key modifiers.
         *
         * \param[out] nativeScanCodes  The OS's native scan codes.
         *
         * \param[out] nativeVirtualKey The native virtual key code as reported by the OS.
         *
         * \param[out] text             A textual representation of the key as reported by the OS.
         */
        void textKeyPressed(
            int                   keyCode,
            Qt::KeyboardModifiers modifiers,
            unsigned long         nativeModifiers,
            unsigned long         nativeScanCodes,
            unsigned long         nativeVirtualKey,
            const QString&        text
        );

        /**
         * Signal that is emitted when a text/character key is pressed while the cursor points to a placeholder
         * element.
         *
         * \param[out] keyCode          The key code reported by the OS.
         *
         * \param[out] modifiers        The keyboard modifiers as interpreted by Qt.
         *
         * \param[out] nativeModifiers  The OS's native key modifiers.
         *
         * \param[out] nativeScanCodes  The OS's native scan codes.
         *
         * \param[out] nativeVirtualKey The native virtual key code as reported by the OS.
         *
         * \param[out] text             A textual representation of the key as reported by the OS.
         */
        void textKeyPressedOnPlaceholder(
            int                   keyCode,
            Qt::KeyboardModifiers modifiers,
            unsigned long         nativeModifiers,
            unsigned long         nativeScanCodes,
            unsigned long         nativeVirtualKey,
            const QString&        text
        );

        /**
         * Signal that is emitted when the right mouse button is clicked.
         *
         * \param[out] screenPosition The global cursor position of the cursor.
         */
        void rightMouseButtonClicked(const QPoint& screenPosition);

        /**
         * Signal that is emitted when the document filename is updated.  This signal is emitted by the
         * \ref Document::saveDocument(const QString&) and \ref Document::loadDocument methods.  This signal allows the
         * GUI to dynamically adjust such things as the application titlebar or name on the document tab.
         *
         * \param[out] newFilename The new filename associated with this document.
         */
        void filenameChanged(const QString& newFilename);

        /**
         * Signal that is emitted when the document's shortform name is updated.  This signal is emitted by the
         * \ref Document::saveDocument(const QString&) and \ref Document::loadDocument methods.  This signal allows the
         * GUI to dynamically adjust such things as the application titlebar or name on the document tab.
         *
         * \param[out] newShortformName The new filename associated with this document.
         */
        void shortformNameChanged(const QString& newShortformName);

        /**
         * Signal that is emitted when the document's title is updated.  The signal is generated using the
         * \ref Document::filenameChanged signal.
         *
         * \param[out] newTitle The new title to display for this view.
         */
        void titleChanged(const QString& newTitle);

        /**
         * Signal that is emitted when the document's modification status changed.
         *
         * \param[out] nowModified Holds true if the document has been modified.  Holds false if the document is
         *                         unmodified.
         */
        void documentModified(bool nowModified);

        /**
         * Signal that is emitted whenever the document managed by this view widget has been changed, including updates
         * to references.  This signal will not be called when the root element is loaded, saved, or assigned to this
         * view widget.
         */
        void documentChanged();

        /**
         * Signal that is emitted whenever undo status changes.
         *
         * \param[in] nowCanUndo   If true, we can now perform an undo operation.  If false, there are no operations to
         *                         be undone.
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

        /**
         * Signal that is emitted when the cursor type has changed.  You can use this signal to trigger the view to
         * update the appearance of the cursor.
         *
         * \param[out] newCursorType The new cursor type.
         */
        void cursorTypeChanged(Cursor::Type newCursorType);

        /**
         * Signal that is emitted when the requirements at the cursor position has changed.  You can use this signal to
         * enable and disable user options.
         *
         * \param[out] newRequirements The new requirements reported by the cursor.
         */
        void requirementsChanged(const Ld::Capabilities& newRequirements);

        /**
         * Signal that is emitted when the cursor position has been updated.
         */
        void cursorUpdated();

        /**
         * Signal that is emitted when the element stack contents changes.
         */
        void elementStackChanged();

        /**
         * Signal that is emitted when the formats under the cursor have changed.
         *
         * \param[out] aggregationsByCapabilityWithAncestors    A database of aggregations organized by capability.
         *                                                      This version includes ancestors.
         *
         * \param[out] aggregationsByCapabilityWithoutAncestors A database of aggregations organized by capability.
         *                                                      This version excludes any ancestors.
         */
        void formatsAtCursorChanged(
            const Ld::AggregationsByCapability& aggregationsByCapabilityWithAncestors,
            const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
        );

        /**
         * Signal that is emitted when the pages under the cursor changes.  This signal is both emitted when the cursor
         * is updated and when one or more formats have been updated.
         *
         * \param[out] pageList The list of pages that are now under the cursor.
         */
        void pagesUnderCursorChanged(const QList<PageList::Index>& pageList);

        /**
         * Signal that is emitted when the editor window's contents are incoherent.
         */
        void contentsAreIncoherent();

        /**
         * Signal that is emitted when the editor window's contents are coherent.
         */
        void contentsAreCoherent();

        /**
         * Signal that is emitted after a pending repositioning operation has been aborted.
         */
        void repositioningAborted();

    public slots:
        /**
         * Slot that is triggered each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Slot that is triggered each time the user interface no longer wants this proxy to be controlled by the main
         * window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

        /**
         * Slot that is triggered after binding to let this view take focus and perform other actions when it takes
         * the focus.
         *
         * This method makes certain that the editor window receives the focus.
         */
        virtual void viewActive();

        /**
         * Slot that is triggered before binding to let this view relinquish focus. and perform other actions before
         * being unbound.  The default implementation does nothing.
         */
        virtual void viewInactive();

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

        /**
         * Slot you can use to search forward for a string from the current location.
         *
         * \param[in] searchText        The search text to be located.
         *
         * \param[in] caseSensitive     If true, the search will require an exact case match.  If false, the search
         *                              will ignore case.
         *
         * \param[in] wholeWordsOnly    If true, the search will require that the search string represent an entire
         *                              word or group of words.  Strings making up a part of a word will be
         *                              ignored.
         *
         * \param[in] regularExpression If true, the search string will be treated as a regular expression.  If
         *                              false, the search string will be treated as normal text.
         *
         * \param[in] okToLoop          If true, the search will restart at the beginning.  If false, the search
         *                              will end at the end of the element tree.
         *
         * \return Returns the result from the search.
         */
        SearchResult searchForward(
            const QString& searchText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression,
            bool           okToLoop = true
        );

        /**
         * Slot you can use to search backward for a string from the current location.
         *
         * \param[in] searchText        The search text to be located.
         *
         * \param[in] caseSensitive     If true, the search will require an exact case match.  If false, the search
         *                              will ignore case.
         *
         * \param[in] wholeWordsOnly    If true, the search will require that the search string represent an entire
         *                              word or group of words.  Strings making up a part of a word will be
         *                              ignored.
         *
         * \param[in] regularExpression If true, the search string will be treated as a regular expression.  If
         *                              false, the search string will be treated as normal text.
         *
         * \param[in] okToLoop          If true, the search will restart at the end.  If false, the search
         *                              will end at the beginning of the element tree.
         *
         * \return Returns the result from the search.
         */
        SearchResult searchBackward(
            const QString& searchText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression,
            bool           okToLoop = true
        );

        /**
         * Changes the current zoom factor for the editor.
         *
         * \param[in] newZoomFactor The new zoom factor.  A value of 1 indicates 100%.
         */
        void setZoom(float newZoomFactor);

        /**
         * Slot that is triggered when the user wants the margins to be shown or hidden.
         *
         * \param[in] nowEnabled If true, margins will be shown.  If false, margins will be hidden.
         */
        void setMarginsEnabled(bool nowEnabled = true);

        /**
         * Slot that is triggered when the user wants the margins to be hidden or shown.
         *
         * \param[in] nowDisabled If true, margins will be hidden.  If false, margins will be shown.
         */
        inline void setMarginsDisabled(bool nowDisabled = true) {
            setMarginsEnabled(!nowDisabled);
        }

        /**
         * Slot that is triggered when the user wants the guides on the page to be shown or hidden.
         *
         * \param[in] nowEnabled If true, guildes will be shown.  If false, guides will be hidden.
         */
        void setGuidesEnabled(bool nowEnabled);

        /**
         * Slot that is triggered when the user wants the guides to be hidden or shown.
         *
         * \param[in] nowDisabled If true, guides will be hidden.  If false, guides will be shown.
         */
        inline void setGuidesDisabled(bool nowDisabled = true) {
            setGuidesEnabled(!nowDisabled);
        }

        /**
         * Slot that can be triggered to restore the focus.  You can use this slot after performing an update to point
         * keyboard focus back to this view.
         */
        void restoreFocus();

        /**
         * Slot that can be triggered to force an area of the current view to be visible.  Note that this method will
         * not update the cursor position.
         *
         * \param[in] visibleRectangle The rectangle to make visible, in scene units.
         */
        void forceVisibleArea(const QRectF& visibleRectangle);

        /**
         * Slot that can be triggered to immediately abort any rendering/repositioning operation.  Note that the
         * editor contents will be incoherent after triggering this slot.
         */
        void abortRepositioning();

    private slots:
        /**
         * Slot that is triggered when the document's filename is changed.  Reports udpates to the view's title.
         *
         * \param[in] newFilename The new filename for this document.
         */
        void reportTitleChanged(const QString& newFilename);

        /**
         * Slot that is triggered when the element stack changes.  The slot builds a database of elements under the
         * cursor based on the formats that the elements contain.
         */
        void processElementStackChange();

        /**
         * Slot that is triggered when the cursor position has been changed.
         */
        void processCursorUpdate();

        /**
         * Slot that reports an update to the page formats.
         */
        void pageFormatsChanged();

        /**
         * Slot that reports an update to the element formats.
         */
        void elementFormatsChanged();

    private:
        /**
         * Called by the constructors to configure this widget.
         *
         * \param[in] newDocument The document that should be managed by this view.
         */
        void configureWidget(Document* newDocument = Q_NULLPTR);

        /**
         * Performs static binding, then binds all the proxies to this class by calling the
         * ProgrammaticWidgetProxy::bind method in each proxy class.
         */
        void bind() final;

        /**
         * Method used to generate the element database.  Returns a pre-prepared database for use.
         */
        void generateAggregationsByCapability() const;

        /**
         * Database of element instances by format type.  This version includes ancestors.
         */
        mutable Ld::AggregationsByCapability currentAggregationsByCapabilityWithAncestors;

        /**
         * Database of element instances by format type.  This version excludes ancestors.
         */
        mutable Ld::AggregationsByCapability currentAggregationsByCapabilityWithoutAncestors;

        /**
         * List of pages that are currently under the cursor.
         */
        mutable QList<PageList::Index> currentPagesUnderCursor;
};

#endif
