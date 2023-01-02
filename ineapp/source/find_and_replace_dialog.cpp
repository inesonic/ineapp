/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FindAndReplaceDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QCloseEvent>
#include <QSharedPointer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QLabel>
#include <QPixmap>
#include <QSize>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QRegularExpression>

#include "application.h"
#include "find_and_replace_dialog.h"

FindAndReplaceDialog::FindAndReplaceDialog(QWidget* parent):ProgrammaticDialog("FindAndReplaceDialog", parent) {
    configureWidget();
    runBuilders();
}


FindAndReplaceDialog::~FindAndReplaceDialog() {}


void FindAndReplaceDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    QString searchText              = settings->value("search_text", QString()).toString();
    bool    caseSensitive           = settings->value("case_sensitive", false).toBool();
    bool    wholeWordsOnly          = settings->value("whole_words_only", false).toBool();
    bool    regularExpressionSearch = settings->value("regular_expression_search", false).toBool();
    bool    advancedMode            = settings->value("replace_mode", false).toBool();
    QString replacementText         = settings->value("replacement_text", QString()).toString();

    setSearchText(searchText);
    setCaseSensitiveModeEnabled(caseSensitive);
    setWholeWordModeEnabled(wholeWordsOnly);
    setRegularExpressionModeEnabled(regularExpressionSearch);
    setFindAndReplaceEnabled(advancedMode);
    setReplacementText(replacementText);

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void FindAndReplaceDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->setValue("search_text", searchText());
    settings->setValue("case_sensitive", caseSensitiveModeEnabled());
    settings->setValue("whole_words_only", wholeWordModeEnabled());
    settings->setValue("regular_expression_search", regularExpressionModeEnabled());
    settings->setValue("replace_mode", findAndReplaceModeEnabled());
    settings->setValue("replacement_text", replaceText());

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


bool FindAndReplaceDialog::findAndReplaceModeEnabled() const {
    QToolButton* advancedToolButton = widget<QToolButton>("advanced_tool_button");
    return advancedToolButton->isChecked();
}


bool FindAndReplaceDialog::findAndReplaceModeDisabled() const {
    return !findAndReplaceModeEnabled();
}


bool FindAndReplaceDialog::caseSensitiveModeEnabled() const {
    QCheckBox* caseSensitiveCheckBox = widget<QCheckBox>("case_sensitive_check_box");
    return caseSensitiveCheckBox->isChecked();
}


bool FindAndReplaceDialog::caseSensitiveModeDisabled() const {
    return !caseSensitiveModeEnabled();
}


bool FindAndReplaceDialog::wholeWordModeEnabled() const {
    QCheckBox* wholeWordsOnlyCheckBox = widget<QCheckBox>("whole_words_only_check_box");
    return wholeWordsOnlyCheckBox->isChecked();
}


bool FindAndReplaceDialog::wholeWordModeDisabled() const {
    return !wholeWordModeEnabled();
}


bool FindAndReplaceDialog::regularExpressionModeEnabled() const {
    QCheckBox* regularExpressionsCheckBox = widget<QCheckBox>("regular_expressions_check_box");
    return regularExpressionsCheckBox->isChecked();
}


bool FindAndReplaceDialog::regularExpressionModeDisabled() const {
    return !regularExpressionModeEnabled();
}


QString FindAndReplaceDialog::searchText() const {
    QLineEdit* searchLineEdit = widget<QLineEdit>("search_line_edit");
    return searchLineEdit->text();
}


QString FindAndReplaceDialog::replaceText() const {
    QLineEdit* replaceLineEdit = widget<QLineEdit>("replace_line_edit");
    return replaceLineEdit->text();
}


void FindAndReplaceDialog::populate() {
    EQt::ProgrammaticDialog::populate();
}


void FindAndReplaceDialog::setFindAndReplaceEnabled(bool nowFindAndReplace) {
    QToolButton* advancedToolButton = widget<QToolButton>("advanced_tool_button");
    advancedToolButton->setChecked(nowFindAndReplace);

    advancedButtonClicked(nowFindAndReplace);
}


void FindAndReplaceDialog::setFindAndReplaceDisabled(bool nowFindOnly) {
    setFindAndReplaceEnabled(!nowFindOnly);
}


void FindAndReplaceDialog::setCaseSensitiveModeEnabled(bool nowCaseSensitive) {
    QCheckBox* caseSensitiveCheckBox = widget<QCheckBox>("case_sensitive_check_box");
    caseSensitiveCheckBox->setChecked(nowCaseSensitive);
}


void FindAndReplaceDialog::setCaseSensitiveModeDisabled(bool nowCaseInsensitive) {
    setCaseSensitiveModeEnabled(!nowCaseInsensitive);
}


void FindAndReplaceDialog::setWholeWordModeEnabled(bool nowWholeWordMode) {
    QCheckBox* wholeWordsOnlyCheckBox = widget<QCheckBox>("whole_words_only_check_box");
    wholeWordsOnlyCheckBox->setChecked(nowWholeWordMode);
}


void FindAndReplaceDialog::setWholeWordModeDisabled(bool nowNotWholeWordMode) {
    setWholeWordModeEnabled(!nowNotWholeWordMode);
}


void FindAndReplaceDialog::setRegularExpressionModeEnabled(bool nowRegularExpressionMode) {
    QCheckBox* regularExpressionsCheckBox = widget<QCheckBox>("regular_expressions_check_box");
    regularExpressionsCheckBox->setChecked(nowRegularExpressionMode);

    regularExpressionButtonClicked(nowRegularExpressionMode);
}


void FindAndReplaceDialog::setRegularExpressionModeDisabled(bool nowNotRegularExpressionMode) {
    setRegularExpressionModeEnabled(!nowNotRegularExpressionMode);
}


void FindAndReplaceDialog::setSearchText(const QString& searchText) {
    QLineEdit* searchLineEdit = widget<QLineEdit>("search_line_edit");
    searchLineEdit->setText(searchText);

    searchTextChanged(searchText);
}


void FindAndReplaceDialog::setReplacementText(const QString& replacementText) {
    QLineEdit* replaceLineEdit = widget<QLineEdit>("replace_line_edit");
    replaceLineEdit->setText(replacementText);
}


void FindAndReplaceDialog::bind() {
    QLineEdit*   searchLineEdit                   = widget<QLineEdit>("search_line_edit");
    QCheckBox*   caseSensitiveCheckBox            = widget<QCheckBox>("case_sensitive_check_box");
    QCheckBox*   wholeWordsOnlyCheckBox           = widget<QCheckBox>("whole_words_only_check_box");
    QCheckBox*   regularExpressionsCheckBox       = widget<QCheckBox>("regular_expressions_check_box");
    QPushButton* searchBackwardsPushButton        = widget<QPushButton>("search_backwards_push_button");
    QPushButton* searchForwardsPushButton         = widget<QPushButton>("search_forwards_push_button");
    QToolButton* advancedToolButton               = widget<QToolButton>("advanced_tool_button");
    QPushButton* replaceAndFindPreviousPushButton = widget<QPushButton>("replace_and_find_previous_push_button");
    QPushButton* replacePushButton                = widget<QPushButton>("replace_push_button");
    QPushButton* replaceAndFindNextPushButton     = widget<QPushButton>("replace_and_find_next_push_button");
    QPushButton* closePushButton                  = widget<QPushButton>("close_push_button");

    connect(searchLineEdit, &QLineEdit::textChanged, this, &FindAndReplaceDialog::searchTextChanged);
    connect(
        regularExpressionsCheckBox,
        &QCheckBox::clicked,
        this,
        &FindAndReplaceDialog::regularExpressionButtonClicked
    );
    connect(
        searchBackwardsPushButton,
        &QPushButton::clicked,
        this,
        &FindAndReplaceDialog::searchBackwardsButtonClicked
    );
    connect(
        searchForwardsPushButton,
        &QPushButton::clicked,
        this,
        &FindAndReplaceDialog::searchForwardsButtonClicked
    );
    connect(
        advancedToolButton,
        &QToolButton::clicked,
        this,
        &FindAndReplaceDialog::advancedButtonClicked
    );
    connect(
        replaceAndFindPreviousPushButton,
        &QPushButton::clicked,
        this,
        &FindAndReplaceDialog::replaceAndSearchBackwardsButtonClicked
    );
    connect(
        replacePushButton,
        &QPushButton::clicked,
        this,
        &FindAndReplaceDialog::replaceButtonClicked
    );
    connect(
        replaceAndFindNextPushButton,
        &QPushButton::clicked,
        this,
        &FindAndReplaceDialog::replaceAndSearchForwardsButtonClicked
    );
    connect(
        closePushButton,
        &QPushButton::clicked,
        this,
        &FindAndReplaceDialog::closeButtonClicked
    );

    connect(searchLineEdit, &QLineEdit::textEdited, this, &FindAndReplaceDialog::searchTextEdited);
    connect(caseSensitiveCheckBox, &QCheckBox::clicked, this, &FindAndReplaceDialog::caseSensitiveCheckboxClicked);
    connect(wholeWordsOnlyCheckBox, &QCheckBox::clicked, this, &FindAndReplaceDialog::wholeWordsOnlyCheckboxClicked);
    connect(
        regularExpressionsCheckBox,
        &QCheckBox::clicked,
        this,
        &FindAndReplaceDialog::regularExpressionButtonClicked
    );

    ProgrammaticDialog::bind();
}


void FindAndReplaceDialog::closeEvent(QCloseEvent* event) {
    event->ignore();
    emit closeRequested();
}


void FindAndReplaceDialog::searchTextChanged(const QString& newText) {
    QLineEdit*   searchLineEdit                   = widget<QLineEdit>("search_line_edit");
    QLineEdit*   replaceLineEdit                  = widget<QLineEdit>("replace_line_edit");
    QGroupBox*   replaceGroupBox                  = widget<QGroupBox>("replace_group_box");
    QCheckBox*   caseSensitiveCheckBox            = widget<QCheckBox>("case_sensitive_check_box");
    QCheckBox*   wholeWordsOnlyCheckBox           = widget<QCheckBox>("whole_words_only_check_box");
    QCheckBox*   regularExpressionsCheckBox       = widget<QCheckBox>("regular_expressions_check_box");
    QPushButton* searchBackwardsPushButton        = widget<QPushButton>("search_backwards_push_button");
    QPushButton* searchForwardsPushButton         = widget<QPushButton>("search_forwards_push_button");
    QPushButton* replaceAndFindPreviousPushButton = widget<QPushButton>("replace_and_find_previous_push_button");
    QPushButton* replacePushButton                = widget<QPushButton>("replace_push_button");
    QPushButton* replaceAndFindNextPushButton     = widget<QPushButton>("replace_and_find_next_push_button");

    if (newText.isEmpty()) {
        replaceLineEdit->setEnabled(false);
        replaceGroupBox->setEnabled(false);
        caseSensitiveCheckBox->setEnabled(false);
        wholeWordsOnlyCheckBox->setEnabled(false);
        regularExpressionsCheckBox->setEnabled(false);
        searchBackwardsPushButton->setEnabled(false);
        searchForwardsPushButton->setEnabled(false);
        replaceAndFindPreviousPushButton->setEnabled(false);
        replacePushButton->setEnabled(false);
        replaceAndFindNextPushButton->setEnabled(false);

        QPalette palette;
        palette.setColor(QPalette::Text, Qt::black);
        searchLineEdit->setPalette(palette);
        searchLineEdit->setToolTip(tr("Text to be found"));
    } else {
        bool usingRegularExpressions = regularExpressionsCheckBox->isChecked();

        caseSensitiveCheckBox->setDisabled(usingRegularExpressions);
        wholeWordsOnlyCheckBox->setDisabled(usingRegularExpressions);

        bool     controlsEnabled;
        QString  toolTip = tr("Text to be found");
        QPalette palette;

        if (usingRegularExpressions) {
            QRegularExpression regularExpression(newText);

            if (regularExpression.isValid()) {
                controlsEnabled = true;
                palette.setColor(QPalette::Text, Qt::black);
            } else {
                controlsEnabled = false;
                palette.setColor(QPalette::Text, Qt::red);
                toolTip = regularExpression.errorString();
            }
        } else {
            controlsEnabled = true;
            palette.setColor(QPalette::Text, Qt::black);
        }

        searchLineEdit->setPalette(palette);
        searchLineEdit->setToolTip(toolTip);

        replaceLineEdit->setEnabled(true);
        replaceGroupBox->setEnabled(true);
        regularExpressionsCheckBox->setEnabled(true);
        searchBackwardsPushButton->setEnabled(controlsEnabled);
        searchForwardsPushButton->setEnabled(controlsEnabled);
        replaceAndFindPreviousPushButton->setEnabled(controlsEnabled);
        replacePushButton->setEnabled(controlsEnabled);
        replaceAndFindNextPushButton->setEnabled(controlsEnabled);
    }
}


void FindAndReplaceDialog::regularExpressionButtonClicked(bool) {
    QLineEdit* searchLineEdit = widget<QLineEdit>("search_line_edit");
    searchTextChanged(searchLineEdit->text());
}


void FindAndReplaceDialog::searchBackwardsButtonClicked() {
    QLineEdit* searchLineEdit             = widget<QLineEdit>("search_line_edit");
    QCheckBox* caseSensitiveCheckBox      = widget<QCheckBox>("case_sensitive_check_box");
    QCheckBox* wholeWordsOnlyCheckBox     = widget<QCheckBox>("whole_words_only_check_box");
    QCheckBox* regularExpressionsCheckBox = widget<QCheckBox>("regular_expressions_check_box");

    QString searchText         = searchLineEdit->text();
    bool    caseSensitive      = caseSensitiveCheckBox->isChecked();
    bool    wholeWordsOnly     = wholeWordsOnlyCheckBox->isChecked();
    bool    regularExpressions = regularExpressionsCheckBox->isChecked();

    emit searchBackwards(searchText, caseSensitive, wholeWordsOnly, regularExpressions);
}


void FindAndReplaceDialog::searchForwardsButtonClicked() {
    QLineEdit* searchLineEdit             = widget<QLineEdit>("search_line_edit");
    QCheckBox* caseSensitiveCheckBox      = widget<QCheckBox>("case_sensitive_check_box");
    QCheckBox* wholeWordsOnlyCheckBox     = widget<QCheckBox>("whole_words_only_check_box");
    QCheckBox* regularExpressionsCheckBox = widget<QCheckBox>("regular_expressions_check_box");

    QString searchText         = searchLineEdit->text();
    bool    caseSensitive      = caseSensitiveCheckBox->isChecked();
    bool    wholeWordsOnly     = wholeWordsOnlyCheckBox->isChecked();
    bool    regularExpressions = regularExpressionsCheckBox->isChecked();

    emit searchForward(searchText, caseSensitive, wholeWordsOnly, regularExpressions);
}


void FindAndReplaceDialog::advancedButtonClicked(bool nowChecked) {
    QToolButton* advancedToolButton               = widget<QToolButton>("advanced_tool_button");
    QGroupBox*   replaceGroupBox                  = widget<QGroupBox>("replace_group_box");
    QPushButton* replaceAndFindPreviousPushButton = widget<QPushButton>("replace_and_find_previous_push_button");
    QPushButton* replacePushButton                = widget<QPushButton>("replace_push_button");
    QPushButton* replaceAndFindNextPushButton     = widget<QPushButton>("replace_and_find_next_push_button");

    replaceGroupBox->setVisible(nowChecked);
    replaceAndFindPreviousPushButton->setVisible(nowChecked);
    replacePushButton->setVisible(nowChecked);
    replaceAndFindNextPushButton->setVisible(nowChecked);

    if (nowChecked) {
        advancedToolButton->setIcon(Application::icon("advanced_controls_visible"));
    } else {
        advancedToolButton->setIcon(Application::icon("advanced_controls_hidden"));
    }

    adjustSize();
}


void FindAndReplaceDialog::replaceAndSearchBackwardsButtonClicked() {
    QLineEdit* searchLineEdit             = widget<QLineEdit>("search_line_edit");
    QLineEdit* replaceLineEdit            = widget<QLineEdit>("replace_line_edit");
    QCheckBox* caseSensitiveCheckBox      = widget<QCheckBox>("case_sensitive_check_box");
    QCheckBox* wholeWordsOnlyCheckBox     = widget<QCheckBox>("whole_words_only_check_box");
    QCheckBox* regularExpressionsCheckBox = widget<QCheckBox>("regular_expressions_check_box");

    QString searchText         = searchLineEdit->text();
    QString replacementText    = replaceLineEdit->text();
    bool    caseSensitive      = caseSensitiveCheckBox->isChecked();
    bool    wholeWordsOnly     = wholeWordsOnlyCheckBox->isChecked();
    bool    regularExpressions = regularExpressionsCheckBox->isChecked();

    emit replaceAndSearchBackward(searchText, replacementText, caseSensitive, wholeWordsOnly, regularExpressions);
}


void FindAndReplaceDialog::replaceButtonClicked() {
    QLineEdit* searchLineEdit             = widget<QLineEdit>("search_line_edit");
    QLineEdit* replaceLineEdit            = widget<QLineEdit>("replace_line_edit");
    QCheckBox* caseSensitiveCheckBox      = widget<QCheckBox>("case_sensitive_check_box");
    QCheckBox* wholeWordsOnlyCheckBox     = widget<QCheckBox>("whole_words_only_check_box");
    QCheckBox* regularExpressionsCheckBox = widget<QCheckBox>("regular_expressions_check_box");

    QString searchText         = searchLineEdit->text();
    QString replacementText    = replaceLineEdit->text();
    bool    caseSensitive      = caseSensitiveCheckBox->isChecked();
    bool    wholeWordsOnly     = wholeWordsOnlyCheckBox->isChecked();
    bool    regularExpressions = regularExpressionsCheckBox->isChecked();

    emit replace(searchText, replacementText, caseSensitive, wholeWordsOnly, regularExpressions);
}


void FindAndReplaceDialog::replaceAndSearchForwardsButtonClicked() {
    QLineEdit* searchLineEdit             = widget<QLineEdit>("search_line_edit");
    QLineEdit* replaceLineEdit            = widget<QLineEdit>("replace_line_edit");
    QCheckBox* caseSensitiveCheckBox      = widget<QCheckBox>("case_sensitive_check_box");
    QCheckBox* wholeWordsOnlyCheckBox     = widget<QCheckBox>("whole_words_only_check_box");
    QCheckBox* regularExpressionsCheckBox = widget<QCheckBox>("regular_expressions_check_box");

    QString searchText         = searchLineEdit->text();
    QString replacementText    = replaceLineEdit->text();
    bool    caseSensitive      = caseSensitiveCheckBox->isChecked();
    bool    wholeWordsOnly     = wholeWordsOnlyCheckBox->isChecked();
    bool    regularExpressions = regularExpressionsCheckBox->isChecked();

    emit replaceAndSearchForward(searchText, replacementText, caseSensitive, wholeWordsOnly, regularExpressions);
}


void FindAndReplaceDialog::closeButtonClicked() {
    emit closeRequested();
}


void FindAndReplaceDialog::configureWidget() {
    setWindowTitle(tr("Search & Replace"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QGroupBox* searchGroupBox = new QGroupBox(tr("Search"));
    registerWidget(searchGroupBox, "search_group_box");
    mainLayout->addWidget(searchGroupBox, 0);

    QVBoxLayout* searchVerticalLayout = newVBoxLayout("search_vertical_layout");
    searchGroupBox->setLayout(searchVerticalLayout);

    QLineEdit* searchLineEdit = new QLineEdit;
    searchLineEdit->setToolTip(tr("Text to be found"));
    searchLineEdit->setWhatsThis(
        tr(
            "You can enter text to be found in this field.  You can also enter regular expressions in this field by "
            "clicking on the \"Regular expressions\" check box below."
        )
    );
    registerWidget(searchLineEdit, "search_line_edit");
    searchVerticalLayout->addWidget(searchLineEdit);

    QCheckBox* caseSensitiveCheckBox = new QCheckBox(tr("Case sensitive"));
    caseSensitiveCheckBox->setToolTip(tr("Click to make searches case sensitive."));
    caseSensitiveCheckBox->setWhatsThis(
        tr("You can click this check box to make searches case sensitive.  If unchecked, searches will ignore case.")
    );
    registerWidget(caseSensitiveCheckBox, "case_sensitive_check_box");
    searchVerticalLayout->addWidget(caseSensitiveCheckBox);

    QCheckBox* wholeWordsOnlyCheckBox = new QCheckBox(tr("Whole words only"));
    wholeWordsOnlyCheckBox->setToolTip(tr("Click to forces searches to locate whole words only."));
    wholeWordsOnlyCheckBox->setWhatsThis(
        tr(
            "You can click this checkbox to indicate that searches should look for whole words only.  The search "
            "will ignore cases where the search text is part of a word.  Unclick this checkbox to locate the search "
            "string anywhere it might be found."
        )
    );
    registerWidget(wholeWordsOnlyCheckBox, "whole_words_only_check_box");
    searchVerticalLayout->addWidget(wholeWordsOnlyCheckBox);

    QCheckBox* regularExpressionsCheckBox = new QCheckBox(tr("Regular expression search"));
    registerWidget(regularExpressionsCheckBox, "regular_expressions_check_box");
    searchVerticalLayout->addWidget(regularExpressionsCheckBox);

    QHBoxLayout* searchButtonLayout = newHBoxLayout("search_button_layout");
    mainLayout->addLayout(searchButtonLayout);

    searchButtonLayout->addStretch(1);

    QPushButton* searchBackwardsPushButton = new QPushButton(
        Application::icon("edit_find_previous"),
        tr("Search Backwards")
    );
    searchBackwardsPushButton->setToolTip(tr("Click here to search backwards."));
    searchBackwardsPushButton->setWhatsThis(
        tr("You can click on this button to begin search backwards from the current cursor position.")
    );
    registerWidget(searchBackwardsPushButton, "search_backwards_push_button");
    searchButtonLayout->addWidget(searchBackwardsPushButton);

    searchButtonLayout->addStretch(1);

    QPushButton* searchForwardsPushButton = new QPushButton(
        Application::icon("edit_find_next"),
        tr("Search Forwards")
    );
    searchForwardsPushButton->setToolTip(tr("Click here to search forwards."));
    searchForwardsPushButton->setWhatsThis(
        tr("You can click on this button to begin search forwards from the current cursor position.")
    );
    registerWidget(searchForwardsPushButton, "search_forwards_push_button");
    searchButtonLayout->addWidget(searchForwardsPushButton);

    searchButtonLayout->addStretch(1);

    QToolButton* advancedToolButton = new QToolButton;
    advancedToolButton->setIcon(Application::icon("advanced_controls_hidden"));
    advancedToolButton->setIconSize(QSize(16, 16));
    advancedToolButton->setToolTip(tr("Click here to access or hide replace features."));
    advancedToolButton->setWhatsThis(tr("You can click this button to enable or disable replace functions."));
    advancedToolButton->setCheckable(true);
    registerWidget(advancedToolButton, "advanced_tool_button");
    searchButtonLayout->addWidget(advancedToolButton);

    QGroupBox* replaceGroupBox = new QGroupBox(tr("Replace"));
    registerWidget(replaceGroupBox, "replace_group_box");
    mainLayout->addWidget(replaceGroupBox);

    QHBoxLayout* replaceLayout = newHBoxLayout("replace_layout");
    replaceGroupBox->setLayout(replaceLayout);

    QLineEdit* replaceLineEdit = new QLineEdit;
    replaceLineEdit->setToolTip(tr("Replacement text"));
    replaceLineEdit->setWhatsThis(
        tr("You can enter replacement text in this field.")
    );
    registerWidget(replaceLineEdit, "replace_line_edit");
    replaceLayout->addWidget(replaceLineEdit);

    QHBoxLayout* replaceButtonLayout = newHBoxLayout("replace_button_layout");
    mainLayout->addLayout(replaceButtonLayout);

    replaceButtonLayout->addStretch(1);

    QPushButton* replaceAndFindPreviousPushButton = new QPushButton(tr("Replace & Find Previous"));
    replaceAndFindPreviousPushButton->setToolTip(
        tr("Click to replace the current selection and find the previous text.")
    );
    replaceAndFindPreviousPushButton->setWhatsThis(
        tr("You can click this button to replace the current selection and locate the previous matching text.")
    );
    registerWidget(replaceAndFindPreviousPushButton, "replace_and_find_previous_push_button");
    replaceButtonLayout->addWidget(replaceAndFindPreviousPushButton);

    replaceButtonLayout->addStretch(1);

    QPushButton* replacePushButton = new QPushButton(tr("Replace"));
    replacePushButton->setToolTip(tr("Click to replace the current selection."));
    replacePushButton->setWhatsThis(
        tr("You can click this button to replace the current selection.  No new search will be performed.")
    );
    registerWidget(replacePushButton, "replace_push_button");
    replaceButtonLayout->addWidget(replacePushButton);

    replaceButtonLayout->addStretch(1);

    QPushButton* replaceAndFindNextPushButton = new QPushButton(tr("Replace & Find Next"));
    replaceAndFindNextPushButton->setToolTip(tr("Click to replace the current selection and find the next text."));
    replaceAndFindNextPushButton->setWhatsThis(
        tr("You can click this button to replace the current selection and locate the next matching text.")
    );
    registerWidget(replaceAndFindNextPushButton, "replace_and_find_next_push_button");
    replaceButtonLayout->addWidget(replaceAndFindNextPushButton);

    replaceButtonLayout->addStretch(1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Close), "close_push_button");

    mainLayout->addWidget(buttonBox);

    caseSensitiveCheckBox->setEnabled(false);
    wholeWordsOnlyCheckBox->setEnabled(false);
    regularExpressionsCheckBox->setEnabled(false);
    searchBackwardsPushButton->setEnabled(false);
    searchForwardsPushButton->setEnabled(false);
    replaceGroupBox->setEnabled(false);
    replaceAndFindPreviousPushButton->setEnabled(false);
    replacePushButton->setEnabled(false);
    replaceAndFindNextPushButton->setEnabled(false);

    advancedToolButton->setChecked(false);
    replaceGroupBox->setVisible(false);
    replaceAndFindPreviousPushButton->setVisible(false);
    replacePushButton->setVisible(false);
    replaceAndFindNextPushButton->setVisible(false);

    searchLineEdit->setFocus();
}
