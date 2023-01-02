/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HomeViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HOME_VIEW_PROXY_H
#define HOME_VIEW_PROXY_H

#include <QObject>
#include <QHash>

#include <ld_element_cursor.h>

#include "command_container.h"
#include "insert_element_command.h"
#include "view_widget.h"
#include "view_proxy.h"

class QKeyEvent;
class QPrinter;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

class Command;
class FindAndReplaceDialog;
class DocumentPreferencesDialog;

/**
 * View proxy used to manage the home functions.  These functions include:
 *
 *     * Basic file menu items
 *     * Basic edit functions
 *     * Basic editor display/zoom functions.
 */
class HomeViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        HomeViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~HomeViewProxy() override;

        /**
         * Loads settings to be used by the view.  The group for the view will already be set.
         */
        void loadSettings() final;

        /**
         * Saves settings used by the widget.  The group for the view will already be set.
         */
        void saveSettings() final;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

        /**
         * Method that is called to perform binding of static actions to slots used to perform those actions.  This
         * method is called once each time a new view is created.
         */
        void bind() final;

    signals:
        /**
         * Signal that is emitted whenever the filename changed.
         *
         * \param[in] newWindowTitle The new window title to apply to the application.
         */
        void setMainWindowTitle(const QString& newWindowTitle);

        /**
         * Signal that is emitted when the user zooms in.  This signal can be used to adjusted the EQt::ZoomComboBox.
         *
         * \param[in] currentZoom The current zoom level.
         */
        void zoomInRequested(unsigned currentZoom);

        /**
         * Signal that is emitted when the user zooms out.  This signal can be used to adjusted the EQt::ZoomComboBox.
         *
         * \param[in] currentZoom The current zoom level.
         */
        void zoomOutRequested(unsigned currentZoom);

    protected:
        /**
         * Method you can overload to receive notification that the document under this view is about to be saved.
         *
         * The default implementation performs no action.
         *
         * \param[in] document Pointer to the document instance that is about to be saved.
         */
        void aboutToSave(Document* document) override;

        /**
         * Method you can overload to receive notification that the document under this view was just loaded from a
         * file.
         *
         * The default implementation performs no action.
         *
         * \param[in] document Pointer to the newly loaded document instance.
         */
        void loaded(Document* document) override;

    private slots:
        /**
         * Slot that is triggered when the cursor type changes.
         *
         * \param[in] newCursorType The new cursor type.
         */
        void cursorTypeChanged(Cursor::Type newCursorType);

        /**
         * Signal that is emitted when the editor window's contents are incoherent.
         */
        void contentsAreIncoherent();

        /**
         * Signal that is emitted when the editor window's contents are coherent.
         */
        void contentsAreCoherent();

        /**
         * Slot that is triggered when the add tab key sequence is pressed.
         */
        void addTabKeyPressed();

        /**
         * Slot that is triggered when the backspace or back key is pressed.
         */
        void backspaceKeyPressed();

        /**
         * Slot that is triggered when the close key sequence is pressed.
         */
        void closeKeyPressed();

        /**
         * Slot that is triggered when the "Delete" key sequence is pressed.
         */
        void deleteKeyPressed();

        /**
         * Slot that is triggered when the delete to end of line key sequence is pressed.
         */
        void deleteToEndOfLineKeyPressed();

        /**
         * Slot that is triggered when the delete to end of word key pressed.
         */
        void deleteToEndOfWordKeyPressed();

        /**
         * Slot that is triggered when the delete to start of word key pressed.
         */
        void deleteToStartOfWordKeyPressed();

        /**
         * Slot that is triggered when the delete complete line key pressed.
         */
        void deleteLineKeyPressed();

        /**
         * Slot that is triggered when the find next key is pressed.
         */
        void findNextKeyPressed();

        /**
         * Slot that is triggered when the find previous key is pressed.
         */
        void findPreviousKeyPressed();

        /**
         * Slot that is triggered when the forward key is pressed.
         */
        void forwardKeyPressed();

        /**
         * Slot that is triggered when the help-contents key pressed.
         */
        void helpContentsKeyPressed();

        /**
         * Slot that is triggered when the insert line separator key is pressed.
         */
        void insertLineSeparatorKeyPressed();

        /**
         * Slot that is triggered when the insert paragraph separator key is pressed.
         */
        void insertParagraphSeparatorKeyPressed();

        /**
         * Slot that is triggered when the "next child" key is pressed.
         */
        void nextChildKeyPressed();

        /**
         * Slot that is triggered when the preferences key is pressed.
         */
        void preferencesKeyPressed();

        /**
         * Slot that is triggered when the "previous child" key is pressed.
         */
        void previousChildKeyPressed();

        /**
         * Slot that is triggered when the print key is pressed.
         */
        void printKeyPressed();

        /**
         * Slot that is triggered when the quit key is pressed.
         */
        void quitKeyPressed();

        /**
         * Slot that is triggered when the replace key is pressed.
         */
        void replaceKeyPressed();

        /**
         * Slot that is triggered when the deselect key is pressed.
         */
        void deselectKeyPressed();

        /**
         * Slot that is triggered when an unbound key is pressed.
         *
         * \param[in] keyCode          The key code reported by the OS.
         *
         * \param[in] modifiers        The keyboard modifiers as interpreted by Qt.
         *
         * \param[in] nativeModifiers  The OS's native key modifiers.
         *
         * \param[in] nativeScanCodes  The OS's native scan codes.
         *
         * \param[in] nativeVirtualKey The native virtual key code as reported by the OS.
         *
         * \param[in] text             A textual representation of the key as reported by the OS.
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
         * Slot that is triggered when the "what's this" key is pressed.
         */
        void whatsThisKeyPressed();

        /**
         * Slot that is triggered when the zoom-in key is pressed.
         */
        void zoomInKeyPressed();

        /**
         * Slot that is triggered when the zoom-out key is pressed.
         */
        void zoomOutKeyPressed();

        /**
         * Slot that is triggered when the full-screen key is pressed.
         */
        void fullScreenKeyPressed();

        /**
         * Slot that is triggered when the cancel key is pressed.
         */
        void cancelKeyPressed();

        /**
         * Slot that is triggered when a text/character key is pressed.
         *
         * \param[in] keyCode          The key code reported by the OS.
         *
         * \param[in] modifiers        The keyboard modifiers as interpreted by Qt.
         *
         * \param[in] nativeModifiers  The OS's native key modifiers.
         *
         * \param[in] nativeScanCodes  The OS's native scan codes.
         *
         * \param[in] nativeVirtualKey The native virtual key code as reported by the OS.
         *
         * \param[in] text             A textual representation of the key as reported by the OS.
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
         * \param[in] screenPosition The global cursor position of the cursor.
         */
        void rightMouseButtonClicked(const QPoint& screenPosition);

        /**
         * Slot that is triggered when an issued command fails.
         *
         * \param[in] command The command that failed.
         */
        void commandFailed(const CommandContainer& command);

        /**
         * Slot that is triggered when an undo operation fails.
         *
         * \param[in] command The command that failed.
         */
        void undoFailed(const CommandContainer& command);

        /**
         * Slot that is triggered when a redo operation fails.
         *
         * \param[in] command The command that failed.
         */
        void redoFailed(const CommandContainer& command);

        /**
         * Slot that is triggered when the undo/redo stack changes.
         *
         * \param[in] undoStackSize The new size of the undo stack.
         *
         * \param[in] redoStackSize The new size of the redo stack.
         */
        void undoRedoStackChanged(unsigned long undoStackSize, unsigned long redoStackSize);

        /**
         * Slot that is triggered when the users requests the File | Imports... dialog.
         */
        void fileImports();

        /**
         * Slot that is triggered when the user requests the File | Export | PDF... dialog.
         */
        void fileExportPdf();

        /**
         * Slot that is triggered for the File | Print menu item.
         */
        void filePrint();

        /**
         * Slot that is triggered when the user selects "Edit | Cut".
         */
        void editCut();

        /**
         * Slot that is triggered when the user selects "Edit | Copy".
         */
        void editCopy();

        /**
         * Slot that is triggered when the user selects "Edit | Paste"
         */
        void editPaste();

        /**
         * Slot that is triggered when the user requests "select all"
         */
        void editSelectAll();

        /**
         * Slot that is triggered when the user requests the Edit | Find... dialog.
         */
        void editFind();

        /**
         * Slot that is triggered when the user requests the Edit | Find and Replace... dialog.
         */
        void editFindAndReplace();

        /**
         * Slot that is triggered when the user clicks on the case sensitive action.
         *
         * \param[in] nowChecked If true, the case sensitive checkbox is now checked.  If false, the case sensitive
         *                       checkbox is now unchecked.
         */
        void caseSensitiveActionTriggered(bool nowChecked);

        /**
         * Slot that is triggered when the user clicks on the whole words only action.
         *
         * \param[in] nowChecked if true, the whole words checkbox is now checked.  If false, the whole words only
         *                       checkbox is now unchecked.
         */
        void wholeWordsOnlyActionTriggered(bool nowChecked);

        /**
         * Slot that is triggered when the regular expression search action is triggered.
         *
         * \param[in] nowChecked If true, the regular expression action is now checked.  If false, the regular
         *                       expression action is now not checked.
         */
        void regularExpressionActionTriggered(bool nowChecked);

        /**
         * Slot that is triggered when the user clicks the search forwards button.
         */
        void searchForwardClicked();

        /**
         * Slot that is triggered when the user clicks the search backwards button.
         */
        void searchBackwardsClicked();

        /**
         * Slot that is triggered when the user presses return on the search text.
         */
        void searchTextEntered();

        /**
         * Slot that is triggered when the horizontal dock widget's text is changed.
         *
         * \param[in] newText The new text.
         */
        void horizontalSearchTextEdited(const QString& newText);

        /**
         * Slot that is triggered when the vertical dock widget's text is changed.
         *
         * \param[in] newText The new text.
         */
        void verticalSearchTextEdited(const QString& newText);

        /**
         * Slot that is triggered when the user enters new search text into the dialog.
         *
         * \param[in] newText The new text.
         */
        void dialogSearchTextEdited(const QString& newText);

        /**
         * Slot that is triggered when the user clicks the find next button in the find and replace dialog.
         *
         * \param[in] searchText        The text to be located.
         *
         * \param[in] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                              case insensitive.
         *
         * \param[in] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                              can identify text in words.
         *
         * \param[in] regularExpression If true, the search should be treated as a regular expression search.  if
         *                              false, the search should be a normal search.
         */
        void searchForward(const QString& searchText, bool caseSensitive, bool wholeWordsOnly, bool regularExpression);

        /**
         * Slot that is triggered when the user clicks the find previous button in the find and replace dialog.
         *
         * \param[in] searchText        The text to be located.
         *
         * \param[in] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                              case insensitive.
         *
         * \param[in] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                              can identify text in words.
         *
         * \param[in] regularExpression If true, the search should be treated as a regular expression search.  if
         *                              false, the search should be a normal search.
         */
        void searchBackwards(
            const QString& searchText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Slot that is triggered when the user clicks the replace button in the find and replace dialog.
         *
         * \param[in] searchText        The text to be located.
         *
         * \param[in] replacementText   The text to replace the search text.
         *
         * \param[in] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                              case insensitive.
         *
         * \param[in] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                              can identify text in words.
         *
         * \param[in] regularExpression If true, the search should be treated as a regular expression search.  if
         *                              false, the search should be a normal search.
         */
        void replace(
            const QString& searchText,
            const QString& replacementText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Slot that is triggered when the user clicks the replace and find next button in the find and replace dialog.
         *
         * \param[in] searchText        The text to be located.
         *
         * \param[in] replacementText   The text to replace the search text.
         *
         * \param[in] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                              case insensitive.
         *
         * \param[in] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                              can identify text in words.
         *
         * \param[in] regularExpression If true, the search should be treated as a regular expression search.  if
         *                              false, the search should be a normal search.
         */
        void replaceAndSearchForward(
            const QString& searchText,
            const QString& replacementText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Slot that is triggered when the user clicks the replace and find previous button in the find and replace
         * dialog.
         *
         * \param[in] searchText        The text to be located.
         *
         * \param[in] replacementText   The text to replace the search text.
         *
         * \param[in] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                              case insensitive.
         *
         * \param[in] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                              can identify text in words.
         *
         * \param[in] regularExpression If true, the search should be treated as a regular expression search.  if
         *                              false, the search should be a normal search.
         */
        void replaceAndSearchBackward(
            const QString& searchText,
            const QString& replacementText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Slot that is triggered when the user clicks either the close button or the dialog X in the title bar in the
         * find and replace dialog.
         */
        void findAndReplaceDialogCloseRequested();

        /**
         * Slot that is triggered when the user selects a new zoom factor using the zoom combo box.
         */
        void zoom();

        /**
         * Slot that is triggered when the user attempts to zoom in.  This slot will update the combo box which will,
         * in turn, update the view.
         */
        void zoomIn();

        /**
         * Slot that is triggered when the user attempts to zoom out.  This slot will update the combo box which will,
         * in turn, update the view.
         */
        void zoomOut();

        /**
         * Slot that is triggered by the view when the shortform name for the document changs.  The slot will update
         * the tab in the main window associated with this widget.
         *
         * \param[in] newShortformName The new filename associated with this document.
         */
        void shortformNameChanged(const QString& newShortformName);

        /**
         * Slot that is triggered when the view's title changes.  The slot will update the window title if and only
         * if this slot is active.
         *
         * \param[in] newTitle The new filename for this view.
         */
        void titleChanged(const QString& newTitle);

        /**
         * Slot that is triggered whenever the view reports that the document's modification status changed.
         *
         * \param[in] nowModified Holds true if the document has been modified.  Holds false if the document is
         *                         unmodified.
         */
        void documentModified(bool nowModified);

        /**
         * Slot that is triggered whenever the user selects the document preferences menu option.
         */
        void documentPreferencesSelected();

        /**
         * Slot that is triggered when the document preferences dialog has pending updates.
         *
         * \param[in] dialog The dialog containing the required updates.
         */
        void applyDocumentPreferencesDialogUpdates(DocumentPreferencesDialog* dialog);

    private:
        /**
         * Method that can be called to connect signals.
         */
        void connectSignals();

        /**
         * Method that can be called to disconnect signals.
         */
        void disconnectSignals();

        /**
         * Method that issues a command to the view.
         *
         * \param[in] newCommand The new command to be issued.
         */
        void issueCommand(Command* newCommand);

        /**
         * Method that is called to insert a new string.
         *
         * \param[in] newText The new text string to be inserted.
         */
        void insertTextAtCursor(const QString& newText);

        /**
         * Method that is called to manage the command popup window.  This method is called when code detects that the
         * user pressed the command key.
         */
        void manageCommandPopupWindow();

        /**
         * Method that can be called when an invalid key is pressed.
         */
        void invalidKeyPressed();

        /**
         * Method that updates the search controls after new text is entered.
         *
         * \param[in] newText The newly entered text.
         */
        void updateSearchControls(const QString& newText);

        /**
         * Method that is called to open the find and find/replace dialogs.
         *
         * \param[in] showReplaceOptions If true, replace options will be presented.  If false, replace options will
         *                               be hidden.
         */
        void displayFindReplaceDialog(bool showReplaceOptions);

        /**
         * Method that temporarily lowers the find and replace dialog so message boxes can be made visible.
         */
        void lowerFindAndReplaceDialog();

        /**
         * Method that re-raises the find and replace dialog.
         */
        void raiseFindAndReplaceDialog();

        /**
         * Method that processes the result of a search.
         *
         * \param[in] searchResult The result from the search.
         */
        void processSearchResult(ViewWidget::SearchResult searchResult);

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * Method that checks if it's OK to overwrite a specified file.
         *
         * \param[in] overwriteFilename The filename of the file we are about to overwrite.
         *
         * \return Returns true if we should overwrite the specified file.  Returns false if we should *not* overwrite
         *         the specified file.
         */
        bool doOverwriteFile(const QString& overwriteFilename);

        /**
         * Method that is called when a comma is pressed.  Determines if we should insert a placeholder into an
         * ancestor element.
         *
         * \return Returns true if the comma triggered special action.  Returns false if the comma did not trigger any
         *         special actions.
         */
        bool processComma();

        /**
         * The element that is currently under the cursor.
         */
        Ld::ElementPointer currentActiveElement;

        /**
         * Hash of all elements that have been referred to fixers along with the number of times.
         */
        static QHash<Ld::ElementPointer, unsigned> allActiveParents;

        /**
         * Pointer to the current find/replace dialog.
         */
        FindAndReplaceDialog* currentFindAndReplaceDialog;
};

#endif
