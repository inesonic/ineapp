/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FindAndReplaceDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FIND_AND_REPLACE_DIALOG_H
#define FIND_AND_REPLACE_DIALOG_H

#include <QSet>
#include <QSharedPointer>

#include <eqt_programmatic_dialog.h>

#include <ld_justified_block_format.h>
#include <ld_unordered_list_paragraph_format.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;

/**
 * Dialog used to control find and replace operations.
 */
class APP_PUBLIC_API FindAndReplaceDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Enumeration of the requested operations.  This value is returned by the exec method or the dialog result
         * method.
         */
        enum class Operation {
            /**
             * Indicates the user cancelled the dialog.
             */
            REJECTED = QDialog::Rejected,

            /**
             * Indicates the user wants to perform a forward find operations.
             */
            FIND = QDialog::Accepted,

            /**
             * Indicates the user wants to perform a forward find and replace operation.
             */
            FIND_AND_REPLACE,
        };

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        FindAndReplaceDialog(QWidget* parent = Q_NULLPTR);

        ~FindAndReplaceDialog() override;

        /**
         * Loads settings to be used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void loadSettings(const QString& groupName) final;

        /**
         * Saves settings used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void saveSettings(const QString& groupName) final;

        /**
         * Method you can use to determine if the dialog is configured for find and replace mode.
         *
         * \return Returns true if the dialog is configured for find and replace operatins.  Returns false if the
         *         dialog is configured for find mode only.
         */
        bool findAndReplaceModeEnabled() const;

        /**
         * Method you can use to determine if the dialog is configured for find only operations.
         *
         * \return Returns true if the dialog is configured for find-only operations.  Returns false if the dialog is
         *         configured for find and replace operations.
         */
        bool findAndReplaceModeDisabled() const;

        /**
         * Method you can use to determine if the case sensitive check box is checked.
         *
         * \return Returns true if case sensitive mode is checked.  Returns false if case sensitive mode is unchecked.
         */
        bool caseSensitiveModeEnabled() const;

        /**
         * Method you can use to determine if the case sensitive check box is unchecked.
         *
         * \return Returns true if case sensitive mode is unchecked.  Returns false if case sensitive mode is checked.
         */
        bool caseSensitiveModeDisabled() const;

        /**
         * Method you can use to determine if the whole word check box is checked.
         *
         * \return Returns true if whole word mode is checked.  Returns false if whole word mode is unchecked.
         */
        bool wholeWordModeEnabled() const;

        /**
         * Method you can use to determine if the whole word check box is unchecked.
         *
         * \return Returns true if whole word mode is unchecked.  Returns false if whole word mode is checked.
         */
        bool wholeWordModeDisabled() const;

        /**
         * Method you can use to determine if the regular expression check box is checked.
         *
         * \return Returns true if regular expression mode is checked.  Returns false if regular expression mode is
         *         unchecked.
         */
        bool regularExpressionModeEnabled() const;

        /**
         * Method you can use to determine if the regular expression check box is unchecked.
         *
         * \return Returns true if regular expression mode is unchecked.  Returns false if regular expression mode is
         *         checked.
         */
        bool regularExpressionModeDisabled() const;

        /**
         * Method you can use to obtain the current search text.
         *
         * \return Returns the currently entered search text.
         */
        QString searchText() const;

        /**
         * Method you can use to obtain the current replace text.
         *
         * \return Returns the currently entered replacement text.
         */
        QString replaceText() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:
        /**
         * Signal that is emitted when the search text changes.
         *
         * \param[out] newText The new search text.
         */
        void searchTextEdited(const QString& newText);

        /**
         * Signal that is emitted when the case sensitive checkbox is changed.
         *
         * \param[out] nowChecked If true, the checkbox is now checked.  If false, the checkbox is now unchecked.
         */
        void caseSensitiveCheckboxClicked(bool nowChecked);

        /**
         * Signal that is emitted when the whole words only checkbox changes.
         *
         * \param[out] nowChecked If true, the checkbox is now checked.  If flase, the checkbox is now unchecked.
         */
        void wholeWordsOnlyCheckboxClicked(bool nowChecked);

        /**
         * Signal that is emitted when the regular expression checkbox changes.
         *
         * \param[out] nowChecked If true, the checkbox is now checked.  If flase, the checkbox is now unchecked.
         */
        void regularExpressionCheckboxClicked(bool nowChecked);

        /**
         * Signal that is emitted when the user clicks the find next button.
         *
         * \param[out] searchText        The text to be located.
         *
         * \param[out] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                               case insensitive.
         *
         * \param[out] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                               can identify text in words.
         *
         * \param[out] regularExpression If true, the search should be treated as a regular expression search.  if
         *                               false, the search should be a normal search.
         */
        void searchForward(const QString& searchText, bool caseSensitive, bool wholeWordsOnly, bool regularExpression);

        /**
         * Signal that is emitted when the user clicks the find previous button.
         *
         * \param[out] searchText        The text to be located.
         *
         * \param[out] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                               case insensitive.
         *
         * \param[out] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                               can identify text in words.
         *
         * \param[out] regularExpression If true, the search should be treated as a regular expression search.  if
         *                               false, the search should be a normal search.
         */
        void searchBackwards(
            const QString& searchText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Signal that is emitted when the user clicks the replace button.
         *
         * \param[out] searchText        The text to be located.
         *
         * \param[out] replacementText   The text to replace the search text.
         *
         * \param[out] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                               case insensitive.
         *
         * \param[out] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                               can identify text in words.
         *
         * \param[out] regularExpression If true, the search should be treated as a regular expression search.  if
         *                               false, the search should be a normal search.
         */
        void replace(
            const QString& searchText,
            const QString& replacementText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Signal that is emitted when the user clicks the replace and find next button.
         *
         * \param[out] searchText        The text to be located.
         *
         * \param[out] replacementText   The text to replace the search text.
         *
         * \param[out] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                               case insensitive.
         *
         * \param[out] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                               can identify text in words.
         *
         * \param[out] regularExpression If true, the search should be treated as a regular expression search.  if
         *                               false, the search should be a normal search.
         */
        void replaceAndSearchForward(
            const QString& searchText,
            const QString& replacementText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Signal that is emitted when the user clicks the replace and find previous button.
         *
         * \param[out] searchText        The text to be located.
         *
         * \param[out] replacementText   The text to replace the search text.
         *
         * \param[out] caseSensitive     If true, the search should be case sensitive.  If false, the search should
         *                               case insensitive.
         *
         * \param[out] wholeWordsOnly    If true, the search should look for whole words only.  If false, the search
         *                               can identify text in words.
         *
         * \param[out] regularExpression If true, the search should be treated as a regular expression search.  if
         *                               false, the search should be a normal search.
         */
        void replaceAndSearchBackward(
            const QString& searchText,
            const QString& replacementText,
            bool           caseSensitive,
            bool           wholeWordsOnly,
            bool           regularExpression
        );

        /**
         * Signal that is emitted when the user clicks either the close button or the dialog X in the title bar.
         */
        void closeRequested();

    public slots:
        /**
         * Slot you can use to indicate if this dialog should be setup for find or find and replace.
         *
         * \param[in] nowFindAndReplace If true, the dialog will be configured for find and replace operations.  If
         *                              false, the dialog will be configured for find operations only.
         */
        void setFindAndReplaceEnabled(bool nowFindAndReplace = true);

        /**
         * Slot you can use to indicate if this dialog should be setup for find mode only.
         *
         * \param[in] nowFindOnly If true, the dialog will be configured for find operations only.  If false, the the
         *                        dialog will be configured for find and replace operations.
         */
        void setFindAndReplaceDisabled(bool nowFindOnly = true);

        /**
         * Slot you can use to set case sensitive mode enabled or disabled.
         *
         * \param[in] nowCaseSensitive If true, the case sensitive check box will be checked.  If false, the case
         *                             sensitive checkbox will not be checked.
         */
        void setCaseSensitiveModeEnabled(bool nowCaseSensitive = true);

        /**
         * Slot you can use to set case sensitive mode disabled or enabled.
         *
         * \param[in] nowCaseInsensitive If true, the case sensitive check box will not be checked.  If false, the case
         *                               sensitive checkbox will be checked.
         */
        void setCaseSensitiveModeDisabled(bool nowCaseInsensitive = true);

        /**
         * Slot you can use to set whole word mode enabled or disabled.
         *
         * \param[in] nowWholeWordMode If true, the whole word check box will be checked.  If false, the case
         *                             sensitive checkbox will not be checked.
         */
        void setWholeWordModeEnabled(bool nowWholeWordMode = true);

        /**
         * Slot you can use to set whole word mode disabled or enabled.
         *
         * \param[in] nowNotWholeWordMode If true, the whole word check box will not be checked.  If false, the case
         *                               sensitive checkbox will be checked.
         */
        void setWholeWordModeDisabled(bool nowNotWholeWordMode = true);

        /**
         * Slot you can use to set regular expression search/replace mode enabled or disabled.
         *
         * \param[in] nowRegularExpressionMode If true, the regular expression search/replace mode check box will be
         *                                     checked.  If false, the regular expression search/replace mode check box
         *                                     will be unchecked.
         */
        void setRegularExpressionModeEnabled(bool nowRegularExpressionMode = true);

        /**
         * Slot you can use to set whole word mode disabled or enabled.
         *
         * \param[in] nowNotRegularExpressionMode If true, the regular expression search/replace mode check box will be
         *                                        unchecked.  If false, the regular expression search/replace mode
         *                                        check box will be checked.
         */
        void setRegularExpressionModeDisabled(bool nowNotRegularExpressionMode = true);

        /**
         * Slot you can use to update the current search text.
         *
         * \param[in] newSearchText The new search text.
         */
        void setSearchText(const QString& newSearchText);

        /**
         * Slot you can use to update the current replace text.
         *
         * \param[in] newReplacementText The new replacement text.
         */
        void setReplacementText(const QString& newReplacementText);

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

        /**
         * Method that is called when the user closes the dialog. This method's behavior is platform specific.  On
         * Windows and Linux, the method will call the base class function.  On MacOS, this method will either accept
         * or reject the close event and set the result code to QDialog::Accepted if the window can be closed.
         */
        void closeEvent(QCloseEvent* event) final;

    private slots:
        /**
         * Slot that is triggered when the search text changes.
         *
         * \param[in] newText The new text in the search line edit.
         */
        void searchTextChanged(const QString& newText);

        /**
         * Slot that is triggered when the regular expressions button is clicked.
         *
         * \param[in] nowChecked If true, the regular expression checkbox button is now checked.  If false, the regular
         *                       expression checkbox is now unchecked.
         */
        void regularExpressionButtonClicked(bool nowChecked);

        /**
         * Slot that is triggered when the search backwards button is clicked.
         */
        void searchBackwardsButtonClicked();

        /**
         * Slot that is triggered when the search forwards button is clicked.
         */
        void searchForwardsButtonClicked();

        /**
         * Slot that is triggered when the advanced button is clicked.
         *
         * \param[in] nowChecked If true, the advanced button is checked.  If false, the advanced button is unchecked.
         */
        void advancedButtonClicked(bool nowChecked);

        /**
         * Slot that is triggered when the replace and find backwards button clicked.
         */
        void replaceAndSearchBackwardsButtonClicked();

        /**
         * Slot that is triggered when the replace button is clicked.
         */
        void replaceButtonClicked();

        /**
         * Slot that is triggered when the replace and find forwards button clicked.
         */
        void replaceAndSearchForwardsButtonClicked();

        /**
         * Slot that is triggered when the close button is clicked.
         */
        void closeButtonClicked();

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();
};

#endif
