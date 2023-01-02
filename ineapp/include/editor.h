/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Editor class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef EDITOR_H
#define EDITOR_H

#include <QRectF>
#include <QList>
#include <QGraphicsView>
#include <QPoint>
#include <QTimer>
#include <QSharedPointer>

#include <eqt_signal_aggregator.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_capabilities.h>

#include "app_common.h"
#include "cursor.h"
#include "page_list.h"
#include "scene_units.h"
#include "command.h"
#include "command_container.h"

class QResizeEvent;
class QPainter;
class QDragEventEvent;
class QDragLeaveEvent;
class QDragMoveEvent;
class QDropEvent;
class QFocusEvent;
class QKeyEvent;
class QMouseEvent;

namespace Ld {
    class Format;
    class PageFormat;
    class DocumentSetting;
}

class ViewWidget;
class Document;
class RootPresentation;

/**
 * Class that provides the view in a model/view relationship for an application worksheet.  This class provides both
 * the rendering for as well as update notifications to the associated \ref Document class.  The Editor class will also
 * manage the cursor, cursor movement, and decorations.  The Editor class will not generate any commands on text entry
 * or delete operations.
 *
 * You can intercept most key strokes received by the editor using the supplied signals.  Special signals are also
 * provided for most standard key bindings.  You should note that these signals will only be triggered if the editor
 * actually receives the key press event so some signals may not trigger.
 *
 * Note that you should check for a non-empty error string after instantiating a new editor instance as that indicates
 * that needed temporary files could not be created.
 */
class APP_PUBLIC_API Editor:public QGraphicsView, private SceneUnits {
    Q_OBJECT

    public:
        /**
         * Type used to indicate the result from a search.
         */
        typedef Cursor::SearchResult SearchResult;

        /**
         * Indicates if the guides should be shown.
         */
        static constexpr bool defaultGuidesAreVisible = true;

        /**
         * Indicates if the margins should be shown.
         */
        static constexpr bool defaultMarginsAreVisible =  true;

        /**
         * Indicates the horizontal margin, in pixels to guarantee between the cursor and the edge of the screen.
         */
        static constexpr unsigned cursorMarginX = 2;

        /**
         * Indicates the vertical margin, in pixels to guarantee between the cursor and the edge of the screen.
         */
        static constexpr unsigned cursorMarginY = 2;

        /**
         * Constructor.  This version constructs a new-empty document for the editor.
         *
         * \param[in] parent The parent object for this window.
         */
        explicit Editor(ViewWidget* parent);

        /**
         * Constructor.
         *
         * \param[in] newDocument The document (model) being managed by this editor.
         *
         * \param[in] parent      The parent object for this window.
         */
        Editor(Document* newDocument, ViewWidget* parent);

        ~Editor() override;

        /**
         * Sets the document being managed by this editor instance.  Note that this method will emit signals indicating
         * changes that must be triggered to align the GUI to the new document state.
         *
         * \param[in] newDocument The document being managed by this editor.
         */
        void setDocument(Document* newDocument);

        /**
         * Obtains the document being managed by this editor instance.
         *
         * \return Returns the editor document.
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
         * Obtains the current document's identifier.
         *
         * \return Returns the current document identifier.
         */
        QString identifier() const;

        /**
         * Method that can be used to determine if the document is empty.  An empty document is defined as having only
         * a single page containing no other items.
         *
         * \return Returns true if the document is empty.  Returns false if the document has content.
         */
        bool isEmpty() const;

        /**
         * Method that can be used to determine if the document is not empty.
         *
         * \return Returns true if the document has content.  Returns false if the document is empty.
         */
        inline bool isNotEmpty() const {
            return !isEmpty();
        }

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
         * Method that reports the maximum horizontal size of any page in the document, in points.
         *
         * \return Returns the maximum width of any page in each column of pages in the editor window, in points.
         */
        QList<double> maximumHorizontalExtentPoints() const;

        /**
         * Method that reports the left edge of each page in the editor window, in points.
         *
         * \return Returns the left edge of each column of pages in the editor window, in points.
         */
        QList<double> pageColumnLeftEdgePoints() const;

        /**
         * Method that reports the current left edge of the underlying document scene, in scene units.
         *
         * \return Returns the scene's left edge, in scene units.
         */
        double sceneLeftEdge() const;

        /**
         * Method that reports the current right edge of the underlying document scene, in scene units.
         *
         * \return Returns the scene's right edge, in scene units.
         */
        double sceneRightEdge() const;

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
         * Method you can use to obtain the page index number of pages under the cursor.
         *
         * \return Returns a list of page index values for pages currently under the cursor.
         */
        QList<PageList::Index> pagesUnderCursor() const;

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
         * Method you can use to inform the editor of the format to use at the next text insertion.
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
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToEndOfBlockKeyPressed(bool selection);

        /**
         * Signal that is emitted when the end of document key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToEndOfDocumentKeyPressed(bool selection);

        /**
         * Signal that is emitted when the end of line key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToEndOfLineKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move forward by character key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextCharacterKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to next line key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextLineKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to next page key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextPageKeyPressed(bool selection);

        /**
         * Signal that is emitted when the mvoe to next word key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToNextWordKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous character key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousCharacterKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous line key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousLineKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous page key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousPageKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to previous word key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToPreviousWordKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to start of block key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToStartOfBlockKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to start of document key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
         */
        void moveToStartOfDocumentKeyPressed(bool selection);

        /**
         * Signal that is emitted when the move to start of line key is pressed.
         *
         * \param[in] selection If true, the user is holding down the shift key/making a selection.
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
         * Signal that is emitted when the document's modification status changed.
         *
         * \param[out] nowModified Holds true if the document has been modified.  Holds false if the document is
         *                         unmodified.
         */
        void documentModified(bool nowModified);

        /**
         * Signal that is emitted whenever the document managed by this editor has been changed, including updates to
         * the list of imports.  This signal will not be called when the root element is loaded, saved, or assigned to
         * this editor widget.
         */
        void documentChanged();

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

        /**
         * Signal that is emitted when the maximum width across all pages in the document changes.  The signal is
         * emitted either when a new document is provided to this editor or when the document contents changes.
         *
         * \param[out] newMaximumExtentsPoints     A list containing the maximum horizontal extents for each column of
         *                                         pages in the editor window.
         *
         * \param[out] newPageColumnLeftEdgePoints A list containing the position of each ruler left edge, in points.
         *
         * \param[out] newLeftEdge                 The new document scene's left edge, in scene units.
         *
         * \param[out] newRightEdge                The new document scene's right edge, in scene units.
         */
        void maximumHorizontalExtentChanged(
            QList<double> newMaximumExtentsPoints,
            QList<double> newPageColumnLeftEdgePoints,
            double        newLeftEdge,
            double        newRightEdge
        );

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
         * Signal that is emitted when the cursor position changes.  Reports the new cursor horizontal position in
         * scene coordinates.
         *
         * \param[out] newPosition The new cursor position in scene coordinates.
         */
        void cursorSceneCoordinatePositionChanged(double newPosition);

        /**
         * Signal that is emitted to indicate if the ruler bar's cursor should be visible or invisible.
         *
         * \param[out] nowVisible If true, the cursor bar should be made visible.  If false, the cursor bar should be
         *                        made invisible.
         */
        void cursorBarVisible(bool nowVisible);

        /**
         * Signal that is emitted when the cursor position has been updated.
         */
        void cursorUpdated();

        /**
         * Signal that is emitted when the element stack contents have changed.
         */
        void elementStackChanged();

        /**
         * Signal that is emitted when one or more page formats have changed.
         */
        void pageFormatsChanged();

        /**
         * Signal that is emitted when an Ld::Element reports that a format's been changed or updated.
         */
        void elementFormatsChanged();

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
         * Slot that inserts a new command into the command queue.
         *
         * The slot will automatically set the cursor and editor for the command.
         *
         * \param[in] newCommand The command to be inserted into the command queue.  The class will take ownership of
         *                       the command.
         */
        void insertCommand(Command* newCommand);

        /**
         * Slot that inserts a new command into the command queue.
         *
         * The slot will automatically set the cursor and editor for the command.
         *
         * \param[in] newCommand The command to be inserted into the command queue.
         */
        void insertCommand(QSharedPointer<Command> newCommand);

        /**
         * Slot that inserts a new command into the command queue.
         *
         * The slot will automatically set the cursor and editor for the command.
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
         * Slot that can be triggered to immediately abort any rendering/repositioning operation.  Note that the
         * editor contents will be incoherent after triggering this slot.
         */
        void abortRepositioning();

    protected:
        /**
         * Method that is triggered when the viewport is redrawn to draw any foreground content after the background
         * and scene are drawn.
         *
         * \param[in] painter The painter to use to draw any foreground content.
         *
         * \param[in] rect    The bounding rectangle for any drawn content.
         */
        void drawForeground(QPainter* painter, const QRectF& rect) final;

        /**
         * Method that is called when the user drags content into this editor.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void dragEnterEvent(QDragEnterEvent* event) final;

        /**
         * Method that is called when the user drags content out of the editor.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void dragLeaveEvent(QDragLeaveEvent* event) final;

        /**
         * Method that is called when the user drags content inside the editor.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void dragMoveEvent(QDragMoveEvent* event) final;

        /**
         * Method that is called when content is dropped into this editor.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void dropEvent(QDropEvent* event) final;

        /**
         * Method that is called when this editor receives the focus.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void focusInEvent(QFocusEvent* event) final;

        /**
         * Method that is called when the focus moves between children of this widget.
         *
         * \param[in] next If true, focus is moving to the next child.  If false, focus is moving to the previous
         *                 child.
         *
         * \return Returns true if there is a new child to receive focus.  Returns false if there is no child to receive
         *         the focus.  This version always returns false.
         */
        bool focusNextPrevChild(bool next) final;

        /**
         * Method that is called when this editor loses the focus.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void focusOutEvent(QFocusEvent* event) final;

        // virtual void inputMethodEvent(QInputMethodEvent* event);

        /**
         * Method that is called when a key press event occurs.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void keyPressEvent(QKeyEvent* event) final;

        // virtual void keyReleaseEvent(QKeyEvent* event);

        /**
         * Method that is called when the user double clicks in the window.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void mouseDoubleClickEvent(QMouseEvent* event) final;

        /**
         * Method that is called when the mouse is moved within the window.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void mouseMoveEvent(QMouseEvent* event) final;

        /**
         * Method that is called when the mouse button is pressed.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void mousePressEvent(QMouseEvent* event) final;

        /**
         * Method that is called when the mouse button is released.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void mouseReleaseEvent(QMouseEvent* event) final;

        /**
         * Method that is triggered when the window is resized.  We overload this method to overcome an issue in Qt 5.7
         * that prevents the underlying view from being consistently redrawn.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void resizeEvent(QResizeEvent* event) final;

    private slots:
        /**
         * Slot that is triggered when presentation updates in the document complete.
         */
        void presentationUpdatesCompleted();

        /**
         * Slot that is triggered when presentation updates were restarted.
         */
        void presentationUpdatesRestarted();

        /**
         * Slot that is triggered when the maximum width across all pages in the document changes.  The signal is
         * emitted either when a new document is provided to this editor or when the document contents changes.
         *
         * \param[out] newMaximumExtentsPoints     A list containing the maximum horizontal extents for each column of
         *                                         pages in the editor window.
         *
         * \param[out] newPageColumnLeftEdgePoints A list containing the position of each ruler left edge, in points.
         *
         * \param[out] newLeftEdge                 The new scene left edge, in scene units.
         *
         * \param[out] newRightEdge                The new scene right edge, in scene units.
         */
        void documentMaximumHorizontalExtentChanged(
            QList<double> newMaximumExtentsPoints,
            QList<double> newPageColumnLeftEdgePoints,
            double        newLeftEdge,
            double        newRightEdge
        );

        /**
         * Slot that is triggered when the scene rectangle that defines the document area changes.  The information is
         * used to adjust scroll bars and the ruler.
         *
         * \param[in] rectangle The new document scene rectangle, in scene units.
         */
        void documentSceneRectangleChanged(const QRectF& rectangle);

        /**
         * Slot that is triggered when one or more areas of the graphics scene has changed.  The slot is used to
         * determine if the cursor position needs to be adjusted.
         *
         * \param[in] regionList A list of rectangles representing the changed regions.
         */
        void sceneAreasChanged(const QList<QRectF>& regionList);

        /**
         * Slot that is triggered when a page format has been changed.
         *
         * \param[in] pageNumber The page number of the first page touched by the format change.
         *
         * \param[in] oldFormat  The old page format that is being replaced.
         *
         * \param[in] newFormat  The new format assigned to the page.
         */
        void pageFormatChanged(
            unsigned long                  pageNumber,
            QSharedPointer<Ld::PageFormat> oldFormat,
            QSharedPointer<Ld::PageFormat> newFormat
        );

        /**
         * Slot that is triggered when an element format is changed or updated.
         *
         * \param[in] element The element who's format has been changed.
         *
         * \param[in] format  The new format associated with this element.
         */
        void descendantFormatWasChanged(Ld::ElementPointer element, Ld::FormatPointer format);

        /**
         * Slot that is triggered when the cursor position has been updated.
         */
        void processCursorUpdate();

        /**
         * Slot that is triggered when the cursor timer expires.
         */
        void blinkCursor();

    private:
        /**
         * Performs configuration common to all constructors.
         *
         * \param[in] newDocument The document that should be tied to this editor upon creation.
         */
        void configureWidget(Document* newDocument);

        /**
         * Method that performs a page up/down operation.
         *
         * \param[in] event The event that triggered this operation.
         */
        void pageMove(QKeyEvent* event);

        /**
         * Method that updates the allowed actions (in the form of a ParentRequirements instance) each time the cursor
         * is moved.
         *
         * \param[in] alwaysGenerateSignals If true, this method will ignore the current state of the cursor or the
         *                                  last reported information and will regenerate all state information.
         */
        void updateAllowedActions(bool alwaysGenerateSignals = false);

        /**
         * Method that is called to update the cursor position in the editor.
         */
        void updateCursorPosition();

        /**
         * Signal aggregator used to reduce processing overhead for the page format updates.
         */
        EQt::SignalAggregator* pageFormatsSignalAggregator;

        /**
         * Signal aggregator used to reduce processing overhead for the element format updates.
         */
        EQt::SignalAggregator* elementFormatsSignalAggregator;

        /**
         * Flag used to indicate whether the page margins are enabled.
         */
        bool marginsAreEnabled;

        /**
         * Flag used to indicate whether the page guides are enabled.
         */
        bool guidesAreEnabled;

        /**
         * The cursor object.
         */
        CursorPointer currentCursor;

        /**
         * Flag that is set if a cursor position update is pending.
         */
        bool cursorUpdatePending;

        /**
         * Flag that is set if a repositioning operation abort has been requested.
         */
        bool abortRepositioningRequested;

        /**
         * Shared pointer to an Ld::Format instance to be applied at the next text insertion.  The pointer will be
         * reset or cleared if a command is issued other than text insertion, if the cursor position is changed, or
         * if the cursor position does not match the supplied cursor position.
         */
        Ld::FormatPointer currentNextTextInsertionFormat;

        /**
         * Cursor position required to use the \ref Editor::nextTextInsertionFormat instance.
         */
        Ld::ElementCursor currentNextTextInsertionElementCursor;

        /**
         * The previous cursor bounding rectangle.
         */
        QRectF previousCursorBoundingRectangle;

        /**
         * The timer used to blink the cursor.
         */
        QTimer* cursorTimer;

        /**
         * If true, the cursor should be displayed the next time the cursor timer expires.
         */
        bool currentShowCursor;

        /**
         * The parent requirements at the current cursor location.
         */
        Ld::Capabilities currentParentRequirements;

        /**
         * Indicates the current cursor position type.
         */
        Cursor::Type currentCursorType;
};

#endif
