/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ListFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QLocale>
#include <QDialog>
#include <QSet>
#include <QSharedPointer>
#include <QLocale>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QLabel>
#include <QIcon>
#include <QSize>
#include <QToolButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QFont>
#include <QFontMetrics>
#include <QDialogButtonBox>

#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_block_format.h>
#include <ld_justified_block_format.h>
#include <ld_text_block_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_ordered_list_paragraph_format.h>
#include <ld_page_format.h>

#include "application.h"
#include "character_format_dialog.h"
#include "view_widget.h"
#include "unicode_marshaller.h"
#include "list_format_dialog.h"

ListFormatDialog::ListFormatDialog(QWidget* parent):ProgrammaticDialog("ListFormatDialog", parent) {
    configureWidget();
    runBuilders();
}


ListFormatDialog::~ListFormatDialog() {}


void ListFormatDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void ListFormatDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void ListFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void ListFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool ListFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool ListFormatDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


ListFormatDialog::CheckState ListFormatDialog::showBulletOrNumberCheckState() const {
    QCheckBox* showBulletOrNumberCheckbox = widget<QCheckBox>("show_bullet_or_number_check_box");
    return static_cast<CheckState>(showBulletOrNumberCheckbox->checkState());
}


bool ListFormatDialog::showBulletOrNumberCheckBoxEnabled() const {
    QCheckBox* showBulletOrNumberCheckbox = widget<QCheckBox>("show_bullet_or_number_check_box");
    return showBulletOrNumberCheckbox->isEnabled();
}


bool ListFormatDialog::showBulletOrNumberCheckBoxDisabled() const {
    return !showBulletOrNumberCheckBoxEnabled();
}


QString ListFormatDialog::selectedBulletStyle() const {
    QList<Ld::UnorderedListParagraphFormat::StandardBullet> standardBullets =
        Ld::UnorderedListParagraphFormat::standardBullets();

    QSet<QString> selectedBullets;
    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(standardBullets.size()) ; ++bulletIndex) {
        const Ld::UnorderedListParagraphFormat::StandardBullet& bulletData  = standardBullets.at(bulletIndex);

        QString      bullet     = bulletData.bullet();
        QString      widgetName = QString("bullet_tool_button_%1").arg(bulletData.unicodeValue(), 4, 16, QChar('0'));
        QToolButton* toolButton = widget<QToolButton>(widgetName);
        Q_ASSERT(toolButton != Q_NULLPTR);

        if (toolButton->isChecked()) {
            selectedBullets << bullet;
        }
    }

    QString result;
    if (selectedBullets.count() == 1) {
        result = *selectedBullets.begin();
    }

    return result;
}


ListFormatDialog::NumberStyle ListFormatDialog::selectedNumberStyle() const {
    NumberStyle result;

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    bool numberSelected     = numberStyleToolButton->isChecked();
    bool upperCaseSelected  = upperCaseStyleToolButton->isChecked();
    bool lowerCaseSelected  = lowerCaseStyleToolButton->isChecked();
    bool romanUpperSelected = romanUpperStyleToolButton->isChecked();
    bool romanLowerSelected = romanLowerStyleToolButton->isChecked();

    if (numberSelected) {
        if (!(upperCaseSelected || lowerCaseSelected || romanUpperSelected || romanLowerSelected)) {
            result = NumberStyle::NUMBER;
        } else {
            result = NumberStyle::INVALID;
        }
    } else /* if (!numberSelected) */ {
        if (upperCaseSelected) {
            if (!(lowerCaseSelected || romanUpperSelected || romanLowerSelected)) {
                result = NumberStyle::UPPER_CASE;
            } else {
                result = NumberStyle::INVALID;
            }
        } else /* if (!numberSelected && !upperCaseSelected && !upperCaseSelected) */ {
            if (lowerCaseSelected) {
                if (!(romanUpperSelected || romanLowerSelected)) {
                    result = NumberStyle::LOWER_CASE;
                } else {
                    result = NumberStyle::INVALID;
                }
            } else /* if (!numberSelected && !upperCaseSelected && !upperCaseSelected && !lowerCaseSelected) */ {
                if (romanUpperSelected) {
                    if (!romanLowerSelected) {
                        result = NumberStyle::ROMAN_UPPER_CASE;
                    } else {
                        result = NumberStyle::INVALID;
                    }
                } else /* if (!numberSelected     &&
                              !upperCaseSelected  &&
                              !upperCaseSelected  &&
                              !lowerCaseSelected  &&
                              !romanUpperSelected    ) */ {
                    if (romanLowerSelected) {
                        result = NumberStyle::ROMAN_LOWER_CASE;
                    } else {
                        result = NumberStyle::INVALID;
                    }
                }
            }
        }
    }

    return result;
}


unsigned ListFormatDialog::startingNumber() const {
    unsigned result;

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    bool numberSelected     = numberStyleToolButton->isChecked();
    bool upperCaseSelected  = upperCaseStyleToolButton->isChecked();
    bool lowerCaseSelected  = lowerCaseStyleToolButton->isChecked();
    bool romanUpperSelected = romanUpperStyleToolButton->isChecked();
    bool romanLowerSelected = romanLowerStyleToolButton->isChecked();

    if (numberSelected || upperCaseSelected || lowerCaseSelected || romanUpperSelected || romanLowerSelected) {
        QSpinBox* startsWithSpinBox = widget<QSpinBox>("starts_with_spin_box");
        if (startsWithSpinBox->text().isEmpty()) {
            result = static_cast<unsigned>(-1);
        } else {
            result = static_cast<unsigned>(startsWithSpinBox->value());
        }
    } else {
        result = static_cast<unsigned>(-1);
    }

    return result;
}


bool ListFormatDialog::isStartingNumberSet() const {
    bool result;

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    bool numberSelected     = numberStyleToolButton->isChecked();
    bool upperCaseSelected  = upperCaseStyleToolButton->isChecked();
    bool lowerCaseSelected  = lowerCaseStyleToolButton->isChecked();
    bool romanUpperSelected = romanUpperStyleToolButton->isChecked();
    bool romanLowerSelected = romanLowerStyleToolButton->isChecked();

    if (numberSelected || upperCaseSelected || lowerCaseSelected || romanUpperSelected || romanLowerSelected) {
        QSpinBox* startsWithSpinBox = widget<QSpinBox>("starts_with_spin_box");
        result = !startsWithSpinBox->text().isEmpty();
    } else {
        result = false;
    }

    return result;
}


bool ListFormatDialog::isStartingNumberCleared() const {
    return !isStartingNumberSet();
}


QString ListFormatDialog::prefixString() const {
    QString result;

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    bool numberSelected     = numberStyleToolButton->isChecked();
    bool upperCaseSelected  = upperCaseStyleToolButton->isChecked();
    bool lowerCaseSelected  = lowerCaseStyleToolButton->isChecked();
    bool romanUpperSelected = romanUpperStyleToolButton->isChecked();
    bool romanLowerSelected = romanLowerStyleToolButton->isChecked();

    if (numberSelected || upperCaseSelected || lowerCaseSelected || romanUpperSelected || romanLowerSelected) {
        QLineEdit* prefixLineEdit = widget<QLineEdit>("prefix_line_edit");
        result = prefixLineEdit->text();
    }

    return result;
}


bool ListFormatDialog::isPrefixStringSet() {
    bool result;

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    bool numberSelected     = numberStyleToolButton->isChecked();
    bool upperCaseSelected  = upperCaseStyleToolButton->isChecked();
    bool lowerCaseSelected  = lowerCaseStyleToolButton->isChecked();
    bool romanUpperSelected = romanUpperStyleToolButton->isChecked();
    bool romanLowerSelected = romanLowerStyleToolButton->isChecked();

    if (numberSelected || upperCaseSelected || lowerCaseSelected || romanUpperSelected || romanLowerSelected) {
        result = prefixStringWasChanged;
    } else {
        result = false;
    }

    return result;
}


bool ListFormatDialog::isPrefixStringCleared() {
    return !isPrefixStringSet();
}


QString ListFormatDialog::suffixString() const {
    QString result;

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    bool numberSelected     = numberStyleToolButton->isChecked();
    bool upperCaseSelected  = upperCaseStyleToolButton->isChecked();
    bool lowerCaseSelected  = lowerCaseStyleToolButton->isChecked();
    bool romanUpperSelected = romanUpperStyleToolButton->isChecked();
    bool romanLowerSelected = romanLowerStyleToolButton->isChecked();

    if (numberSelected || upperCaseSelected || lowerCaseSelected || romanUpperSelected || romanLowerSelected) {
        QLineEdit* suffixLineEdit = widget<QLineEdit>("suffix_line_edit");
        result = suffixLineEdit->text();
    }

    return result;
}


bool ListFormatDialog::isSuffixStringSet() {
    bool result;

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    bool numberSelected     = numberStyleToolButton->isChecked();
    bool upperCaseSelected  = upperCaseStyleToolButton->isChecked();
    bool lowerCaseSelected  = lowerCaseStyleToolButton->isChecked();
    bool romanUpperSelected = romanUpperStyleToolButton->isChecked();
    bool romanLowerSelected = romanLowerStyleToolButton->isChecked();

    if (numberSelected || upperCaseSelected || lowerCaseSelected || romanUpperSelected || romanLowerSelected) {
        result = suffixStringWasChanged;
    } else {
        result = false;
    }

    return result;
}


bool ListFormatDialog::isSuffixStringCleared() {
    return !isSuffixStringSet();
}


QString ListFormatDialog::fontFamily() const {
    QString result;

    if (useFontDialogValues) {
        result = fontFormatDialog->fontFamily();
    } else {
        result = inputFontFamilies.count() == 1 ? *inputFontFamilies.begin() : QString();
    }

    return result;
}


unsigned ListFormatDialog::fontSize() const {
    unsigned result;

    if (useFontDialogValues) {
        result = fontFormatDialog->fontSize();
    } else {
        result = inputFontSizes.count() == 1 ? *inputFontSizes.begin() : 0;
    }

    return result;
}


Ld::FontFormat::Weight ListFormatDialog::fontWeight() const {
    Ld::FontFormat::Weight result;

    if (useFontDialogValues) {
        result = fontFormatDialog->fontWeight();
    } else {
        result = inputFontWeights.count() == 1 ? *inputFontWeights.begin() : Ld::FontFormat::Weight::INVALID;
    }

    return result;
}


bool ListFormatDialog::italicsEnabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->italicsEnabled();
    } else {
        result = inputItalics == QuadState::ENABLED;
    }

    return result;
}


bool ListFormatDialog::italicsDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->italicsDisabled();
    } else {
        result = inputItalics == QuadState::DISABLED;
    }

    return result;
}


bool ListFormatDialog::italicsBothEnabledAndDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->italicsBothEnabledAndDisabled();
    } else {
        result = inputItalics == QuadState::ENABLED_AND_DISABLED;
    }

    return result;
}


bool ListFormatDialog::underlineEnabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->underlineEnabled();
    } else {
        result = inputUnderline == QuadState::ENABLED;
    }

    return result;
}


bool ListFormatDialog::underlineDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->underlineDisabled();
    } else {
        result = inputUnderline == QuadState::DISABLED;
    }

    return result;
}


bool ListFormatDialog::underlineBothEnabledAndDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->underlineBothEnabledAndDisabled();
    } else {
        result = inputUnderline == QuadState::ENABLED_AND_DISABLED;
    }

    return result;
}


bool ListFormatDialog::overlineEnabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->overlineEnabled();
    } else {
        result = inputOverline == QuadState::ENABLED;
    }

    return result;
}


bool ListFormatDialog::overlineDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->overlineDisabled();
    } else {
        result = inputOverline == QuadState::DISABLED;
    }

    return result;
}


bool ListFormatDialog::overlineBothEnabledAndDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->overlineBothEnabledAndDisabled();
    } else {
        result = inputOverline == QuadState::ENABLED_AND_DISABLED;
    }

    return result;
}


bool ListFormatDialog::strikethroughEnabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->strikethroughEnabled();
    } else {
        result = inputStrikethrough == QuadState::ENABLED;
    }

    return result;
}


bool ListFormatDialog::strikethroughDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->strikethroughDisabled();
    } else {
        result = inputStrikethrough == QuadState::DISABLED;
    }

    return result;
}


bool ListFormatDialog::strikethroughBothEnabledAndDisabled() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->strikethroughBothEnabledAndDisabled();
    } else {
        result = inputStrikethrough == QuadState::ENABLED_AND_DISABLED;
    }

    return result;
}


QColor ListFormatDialog::fontColor() const {
    QColor result;

    if (useFontDialogValues) {
        result = fontFormatDialog->fontColor();
    } else {
        result = inputFontColors.count() == 1 ? *inputFontColors.begin() : QColor();
    }

    return result;
}


bool ListFormatDialog::fontColorSelected() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->fontColorSelected();
    } else {
        result = false;
    }

    return result;
}


QColor ListFormatDialog::fontBackgroundColor() const {
    QColor result;

    if (useFontDialogValues) {
        result = fontFormatDialog->fontBackgroundColor();
    } else {
        result = inputFontBackgroundColors.count() == 1 ? *inputFontBackgroundColors.begin() : QColor();
    }

    return result;
}


bool ListFormatDialog::fontBackgroundColorSelected() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->fontBackgroundColorSelected();
    } else {
        result = false;
    }

    return result;
}


float ListFormatDialog::letterSpacing() const {
    float result;

    if (useFontDialogValues) {
        result = fontFormatDialog->letterSpacing();
    } else {
        result = inputLetterSpacings.count() == 1 ? *inputLetterSpacings.begin() : 0;
    }

    return result;
}


bool ListFormatDialog::letterSpacingValid() const {
    bool result;

    if (useFontDialogValues) {
        result = fontFormatDialog->letterSpacingValid();
    } else {
        result = false;
    }

    return result;
}


bool ListFormatDialog::letterSpacingInvalid() const {
    return fontFormatDialog->letterSpacingInvalid();
}


void ListFormatDialog::populate() {
    QToolButton* numberStyleToolButton          = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton       = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton       = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton      = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton      = widget<QToolButton>("roman_lower_style_tool_button");

    QSpinBox*    startingNumberSpinBox          = widget<QSpinBox>("starts_with_spin_box");
    QLineEdit*   prefixLineEdit                 = widget<QLineEdit>("prefix_line_edit");
    QLineEdit*   suffixLineEdit                 = widget<QLineEdit>("suffix_line_edit");
    QCheckBox*   showBulletOrNumberCheckBox     = widget<QCheckBox>("show_bullet_or_number_check_box");

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");

    QList<Ld::UnorderedListParagraphFormat::StandardBullet> standardBullets =
        Ld::UnorderedListParagraphFormat::standardBullets();

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(standardBullets.size()) ; ++bulletIndex) {
        const Ld::UnorderedListParagraphFormat::StandardBullet& bulletData  = standardBullets.at(bulletIndex);
        QString      bullet     = bulletData.bullet();

        QString      buttonName = QString("bullet_tool_button_%1").arg(bulletData.unicodeValue(), 4, 16, QChar('0'));
        QToolButton* button     = widget<QToolButton>(buttonName);

        if (inputBulletStyles.contains(bullet)) {
            button->setChecked(true);
        } else {
            button->setChecked(false);
        }
    }

    numberStyleToolButton->setChecked(inputNumberStyles.contains(NumberStyle::NUMBER));
    upperCaseStyleToolButton->setChecked(inputNumberStyles.contains(NumberStyle::UPPER_CASE));
    lowerCaseStyleToolButton->setChecked(inputNumberStyles.contains(NumberStyle::LOWER_CASE));
    romanUpperStyleToolButton->setChecked(inputNumberStyles.contains(NumberStyle::ROMAN_UPPER_CASE));
    romanLowerStyleToolButton->setChecked(inputNumberStyles.contains(NumberStyle::ROMAN_LOWER_CASE));

    setNumberStyleControlsEnabled(!inputNumberStyles.isEmpty());

    if (inputStartingNumber != static_cast<unsigned>(-1)) {
        startingNumberSpinBox->setValue(inputStartingNumber);
    } else {
        startingNumberSpinBox->clear();
    }

    if (inputPrefixStringSet) {
        prefixLineEdit->setText(inputPrefixString);
        prefixLineEdit->setPlaceholderText(QString());
    } else {
        prefixLineEdit->clear();
        prefixLineEdit->setPlaceholderText(tr("Unchanged"));
    }

    if (inputSuffixStringSet) {
        suffixLineEdit->setText(inputSuffixString);
        suffixLineEdit->setPlaceholderText(QString());
    } else {
        suffixLineEdit->clear();
        suffixLineEdit->setPlaceholderText(tr("Unchanged"));
    }

    showBulletOrNumberCheckBox->setCheckState(static_cast<Qt::CheckState>(inputShowBulletOrNumberCheckState));

    if (inputBulletStyles.isEmpty() && inputNumberStyles.isEmpty()) {
        removeListFormattingPushButton->setEnabled(false);
    } else {
        removeListFormattingPushButton->setEnabled(true);
    }

    useFontDialogValues = false;

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
        okPushButton->setEnabled(false);

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif
}


void ListFormatDialog::setShowBulletOrNumberCheckState(CheckState newCheckState) {
    inputShowBulletOrNumberCheckState = newCheckState;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::setShowBulletOrNumberCheckBoxEnabled(bool nowEnabled) {
    QCheckBox* showBulletOrNumberCheckBox = widget<QCheckBox>("show_bullet_or_number_check_box");
    showBulletOrNumberCheckBox->setEnabled(nowEnabled);

    if (!nowEnabled) {
        showBulletOrNumberCheckBox->setChecked(false);
    }
}


void ListFormatDialog::setShowBulletOrNumberCheckBoxDisabled(bool nowDisabled) {
    setShowBulletOrNumberCheckBoxEnabled(!nowDisabled);
}


void ListFormatDialog::setListFontControlsEnabled(bool nowEnabled) {
    QPushButton* listFontPushButton = widget<QPushButton>("list_font_push_button");
    listFontPushButton->setEnabled(nowEnabled);
}


void ListFormatDialog::setListFontControlsDisabled(bool nowDisabled) {
    setListFontControlsEnabled(!nowDisabled);
}


void ListFormatDialog::setSelectedBulletStyle(const QSet<QString>& selectedStyles) {
    inputBulletStyles = selectedStyles;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::setSelectedNumberStyles(const QSet<NumberStyle>& selectedStyles) {
    inputNumberStyles = selectedStyles;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::setStartingNumber(unsigned newStartingNumber) {
    inputStartingNumber = newStartingNumber;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::clearStartingNumber() {
    inputStartingNumber = static_cast<unsigned>(-1);

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::setPrefixString(const QString& newPrefixString) {
    inputPrefixString    = newPrefixString;
    inputPrefixStringSet = true;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::clearPrefixString() {
    inputPrefixString.clear();
    inputPrefixStringSet = false;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::setSuffixString(const QString& newSuffixString) {
    inputSuffixString    = newSuffixString;
    inputSuffixStringSet = true;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::clearSuffixString() {
    inputSuffixString.clear();
    inputSuffixStringSet = false;

    if (isVisible()) {
        populate();
    }
}


void ListFormatDialog::setFontFamily(const QString& newFontFamily) {
    inputFontFamilies.clear();
    inputFontFamilies << newFontFamily;
}


void ListFormatDialog::setFontFamilies(const QSet<QString>& newFontFamilies) {
    inputFontFamilies = newFontFamilies;
}


void ListFormatDialog::setFontSize(unsigned newFontSize) {
    inputFontSizes.clear();
    inputFontSizes << newFontSize;
}


void ListFormatDialog::setFontSizes(const QSet<unsigned>& newFontSizes) {
    inputFontSizes = newFontSizes;
}


void ListFormatDialog::setFontWeight(Ld::FontFormat::Weight newFontWeight) {
    inputFontWeights.clear();
    inputFontWeights << newFontWeight;
}


void ListFormatDialog::setFontWeights(const QSet<Ld::FontFormat::Weight>& newFontWeights) {
    inputFontWeights = newFontWeights;
}


void ListFormatDialog::setSupportedFontWeights(const QSet<Ld::FontFormat::Weight>& newSupportedFontWeights) {
    inputSupportedFontWeights = newSupportedFontWeights;
}


void ListFormatDialog::setItalicsEnabled(bool nowEnabled) {
    inputItalics = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;
}


void ListFormatDialog::setItalicsDisabled(bool nowDisabled) {
    setItalicsEnabled(!nowDisabled);
}


void ListFormatDialog::setItalicsBothEnabledAndDisabled() {
    inputItalics = QuadState::ENABLED_AND_DISABLED;
}


void ListFormatDialog::setUnderlineEnabled(bool nowEnabled) {
    inputUnderline = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;
}


void ListFormatDialog::setUnderlineDisabled(bool nowDisabled) {
    setUnderlineEnabled(!nowDisabled);
}


void ListFormatDialog::setUnderlineBothEnabledAndDisabled() {
    inputUnderline = QuadState::ENABLED_AND_DISABLED;
}


void ListFormatDialog::setOverlineEnabled(bool nowEnabled) {
    inputOverline = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;
}


void ListFormatDialog::setOverlineDisabled(bool nowDisabled) {
    setOverlineEnabled(!nowDisabled);
}


void ListFormatDialog::setOverlineBothEnabledAndDisabled() {
    inputUnderline = QuadState::ENABLED_AND_DISABLED;
}


void ListFormatDialog::setStrikethroughEnabled(bool nowEnabled) {
    inputStrikethrough = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;
}


void ListFormatDialog::setStrikethroughDisabled(bool nowDisabled) {
    setStrikethroughEnabled(!nowDisabled);
}


void ListFormatDialog::setStrikethroughBothEnabledAndDisabled() {
    inputStrikethrough = QuadState::ENABLED_AND_DISABLED;
}


void ListFormatDialog::setFontColor(const QColor& newFontColor) {
    inputFontColors.clear();
    inputFontColors << newFontColor;
}


void ListFormatDialog::setFontColors(const QSet<QColor>& newFontColors) {
    inputFontColors = newFontColors;
}


void ListFormatDialog::setFontBackgroundColor(const QColor& newFontBackgroundColor) {
    inputFontBackgroundColors.clear();
    inputFontBackgroundColors << newFontBackgroundColor;
}


void ListFormatDialog::setFontBackgroundColors(const QSet<QColor>& newFontBackgroundColors) {
    inputFontBackgroundColors = newFontBackgroundColors;
}


void ListFormatDialog::setLetterSpacing(float newLetterSpacing) {
    inputLetterSpacings.clear();
    inputLetterSpacings << newLetterSpacing;
}


void ListFormatDialog::setLetterSpacing(const QSet<float>& newLetterSpacing) {
    inputLetterSpacings = newLetterSpacing;
}


void ListFormatDialog::bind() {
    QToolButton* numberStyleToolButton          = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton       = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton       = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton      = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton      = widget<QToolButton>("roman_lower_style_tool_button");

    QSpinBox*    startingNumberSpinBox          = widget<QSpinBox>("starts_with_spin_box");
    QLineEdit*   prefixLineEdit                 = widget<QLineEdit>("prefix_line_edit");
    QLineEdit*   suffixLineEdit                 = widget<QLineEdit>("suffix_line_edit");
    QPushButton* listFontPushButton             = widget<QPushButton>("list_font_push_button");
    QCheckBox*   showBulletOrNumberCheckBox     = widget<QCheckBox>("show_bullet_or_number_check_box");

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");

    QList<Ld::UnorderedListParagraphFormat::StandardBullet> standardBullets =
        Ld::UnorderedListParagraphFormat::standardBullets();

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(standardBullets.size()) ; ++bulletIndex) {
        const Ld::UnorderedListParagraphFormat::StandardBullet& bulletData  = standardBullets.at(bulletIndex);

        QString      buttonName = QString("bullet_tool_button_%1").arg(bulletData.unicodeValue(), 4, 16, QChar('0'));
        QToolButton* button     = widget<QToolButton>(buttonName);

        UnicodeMarshaller* marshaller = new UnicodeMarshaller(bulletData.unicodeValue(), this);
        connect(button, &QToolButton::clicked, marshaller, &UnicodeMarshaller::trigger);
        connect(
            marshaller,
            static_cast<void (UnicodeMarshaller::*)(UnicodeMarshaller::UnicodeValue)>(&UnicodeMarshaller::triggered),
            this,
            &ListFormatDialog::bulletStyleSelected
        );
    }

    connect(numberStyleToolButton, &QToolButton::clicked, this, &ListFormatDialog::numberListStyleSelected);
    connect(
        upperCaseStyleToolButton,
        &QToolButton::clicked,
        this,
        &ListFormatDialog::upperCaseLetterListStyleSelected
    );
    connect(
        lowerCaseStyleToolButton,
        &QToolButton::clicked,
        this,
        &ListFormatDialog::lowerCaseLetterListStyleSelected
    );
    connect(
        romanUpperStyleToolButton,
        &QToolButton::clicked,
        this,
        &ListFormatDialog::upperCaseRomanNumeralStyleSelected
    );
    connect(
        romanLowerStyleToolButton,
        &QToolButton::clicked,
        this,
        &ListFormatDialog::lowerCaseRomanNumeralStyleSelected
    );

    connect(
        startingNumberSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &ListFormatDialog::startingNumberChanged
    );

    connect(prefixLineEdit, &QLineEdit::textEdited, this, &ListFormatDialog::prefixTextChanged);
    connect(suffixLineEdit, &QLineEdit::textEdited, this, &ListFormatDialog::suffixTextChanged);

    connect(
        removeListFormattingPushButton,
        &QPushButton::clicked,
        this,
        &ListFormatDialog::removeListFormattingButtonClicked
    );

    connect(listFontPushButton, &QPushButton::clicked, this, &ListFormatDialog::listFontButtonClicked);
    connect(
        showBulletOrNumberCheckBox,
        &QCheckBox::stateChanged,
        this,
        &ListFormatDialog::showHideButtonNumberCheckBoxClicked
    );

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* okButton = widget<QPushButton>("ok_push_button");
        connect(okButton, &QPushButton::clicked, this, &ListFormatDialog::accept);

        QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
        connect(cancelButton, &QPushButton::clicked, this, &ListFormatDialog::reject);

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        connect(applyButton, &QPushButton::clicked, this, &ListFormatDialog::applyClicked);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    ProgrammaticDialog::bind();
}


void ListFormatDialog::closeEvent(QCloseEvent* event) {
    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        EQt::ProgrammaticDialog::closeEvent(event);

    #elif (defined(Q_OS_DARWIN))

        if (valuesChanged()) {
            setResult(QDialog::Accepted);
        } else {
            setResult(QDialog::Rejected);
        }

        EQt::ProgrammaticDialog::closeEvent(event);

    #else

        #error Unknown platform

    #endif
}


void ListFormatDialog::listFontButtonClicked() {
    if (!useFontDialogValues) {
        fontFormatDialog->setFontFamilies(inputFontFamilies);
        fontFormatDialog->setFontSizes(inputFontSizes);
        fontFormatDialog->setFontWeights(inputFontWeights);

        if (inputItalics == QuadState::ENABLED) {
            fontFormatDialog->setItalicsEnabled();
        } else if (inputItalics == QuadState::DISABLED) {
            fontFormatDialog->setItalicsDisabled();
        } else {
            fontFormatDialog->setItalicsBothEnabledAndDisabled();
        }

        if (inputUnderline == QuadState::ENABLED) {
            fontFormatDialog->setUnderlineEnabled();
        } else if (inputUnderline == QuadState::DISABLED) {
            fontFormatDialog->setUnderlineDisabled();
        } else {
            fontFormatDialog->setUnderlineBothEnabledAndDisabled();
        }

        if (inputOverline == QuadState::ENABLED) {
            fontFormatDialog->setOverlineEnabled();
        } else if (inputOverline == QuadState::DISABLED) {
            fontFormatDialog->setOverlineDisabled();
        } else {
            fontFormatDialog->setOverlineBothEnabledAndDisabled();
        }

        if (inputStrikethrough == QuadState::ENABLED) {
            fontFormatDialog->setStrikethroughEnabled();
        } else if (inputStrikethrough == QuadState::DISABLED) {
            fontFormatDialog->setStrikethroughDisabled();
        } else {
            fontFormatDialog->setStrikethroughBothEnabledAndDisabled();
        }

        fontFormatDialog->setFontColors(inputFontColors);
        fontFormatDialog->setFontBackgroundColors(inputFontBackgroundColors);
        fontFormatDialog->setLetterSpacing(inputLetterSpacings);
    }

    int result = fontFormatDialog->exec();
    if (result == CharacterFormatDialog::Accepted) {
        QString newFontFamily = fontFormatDialog->fontFamily();
        if (!newFontFamily.isEmpty()) {
            fontFormatDialog->setFontFamily(fontFormatDialog->fontFamily());
        }

        unsigned newFontSize = fontFormatDialog->fontSize();
        if (newFontSize > 0) {
            fontFormatDialog->setFontSize(newFontSize);
        }

        Ld::FontFormat::Weight newFontWeight = fontFormatDialog->fontWeight();
        if (newFontWeight != Ld::FontFormat::Weight::INVALID) {
            fontFormatDialog->setFontWeight(newFontWeight);
        }

        if (!fontFormatDialog->italicsBothEnabledAndDisabled()) {
            fontFormatDialog->setItalicsEnabled(fontFormatDialog->italicsEnabled());
        }

        if (!fontFormatDialog->underlineBothEnabledAndDisabled()) {
            fontFormatDialog->setUnderlineEnabled(fontFormatDialog->underlineEnabled());
        }

        if (!fontFormatDialog->overlineBothEnabledAndDisabled()) {
            fontFormatDialog->setOverlineEnabled(fontFormatDialog->overlineEnabled());
        }

        if (!fontFormatDialog->strikethroughBothEnabledAndDisabled()) {
            fontFormatDialog->setStrikethroughEnabled(fontFormatDialog->strikethroughEnabled());
        }

        if (fontFormatDialog->fontColorSelected()) {
            fontFormatDialog->setFontColor(fontFormatDialog->fontColor());
        }

        if (fontFormatDialog->fontBackgroundColorSelected()) {
            fontFormatDialog->setFontBackgroundColor(fontFormatDialog->fontBackgroundColor());
        }

        if (fontFormatDialog->letterSpacingValid()) {
            fontFormatDialog->setLetterSpacing(fontFormatDialog->letterSpacing());
        }

        useFontDialogValues = true;

        updateDialog();
    }
}


void ListFormatDialog::bulletStyleSelected(
        Ld::UnorderedListParagraphFormat::StandardBullet::UnicodeValue unicodeValue
    ) {
    QList<Ld::UnorderedListParagraphFormat::StandardBullet> standardBullets =
        Ld::UnorderedListParagraphFormat::standardBullets();

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(standardBullets.size()) ; ++bulletIndex) {
        const Ld::UnorderedListParagraphFormat::StandardBullet& bulletData  = standardBullets.at(bulletIndex);
        QString buttonName = QString("bullet_tool_button_%1").arg(bulletData.unicodeValue(), 4, 16, QChar('0'));

        QToolButton* button = widget<QToolButton>(buttonName);
        if (bulletData.unicodeValue() == unicodeValue) {
            button->setChecked(true);
        } else {
            button->setChecked(false);
        }
    }

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    numberStyleToolButton->setChecked(false);
    upperCaseStyleToolButton->setChecked(false);
    lowerCaseStyleToolButton->setChecked(false);
    romanUpperStyleToolButton->setChecked(false);
    romanLowerStyleToolButton->setChecked(false);

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");
    removeListFormattingPushButton->setEnabled(true);

    setNumberStyleControlsEnabled(false);

    updateDialog();
}


void ListFormatDialog::numberListStyleSelected() {
    uncheckBulletStyles();

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    numberStyleToolButton->setChecked(true);
    upperCaseStyleToolButton->setChecked(false);
    lowerCaseStyleToolButton->setChecked(false);
    romanUpperStyleToolButton->setChecked(false);
    romanLowerStyleToolButton->setChecked(false);

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");
    removeListFormattingPushButton->setEnabled(true);

    setNumberStyleControlsEnabled(true);
    updateDialog();
}


void ListFormatDialog::upperCaseLetterListStyleSelected() {
    uncheckBulletStyles();

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    numberStyleToolButton->setChecked(false);
    upperCaseStyleToolButton->setChecked(true);
    lowerCaseStyleToolButton->setChecked(false);
    romanUpperStyleToolButton->setChecked(false);
    romanLowerStyleToolButton->setChecked(false);

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");
    removeListFormattingPushButton->setEnabled(true);

    setNumberStyleControlsEnabled(true);
    updateDialog();
}


void ListFormatDialog::lowerCaseLetterListStyleSelected() {
    uncheckBulletStyles();

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    numberStyleToolButton->setChecked(false);
    upperCaseStyleToolButton->setChecked(false);
    lowerCaseStyleToolButton->setChecked(true);
    romanUpperStyleToolButton->setChecked(false);
    romanLowerStyleToolButton->setChecked(false);

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");
    removeListFormattingPushButton->setEnabled(true);

    setNumberStyleControlsEnabled(true);
    updateDialog();
}


void ListFormatDialog::upperCaseRomanNumeralStyleSelected() {
    uncheckBulletStyles();

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    numberStyleToolButton->setChecked(false);
    upperCaseStyleToolButton->setChecked(false);
    lowerCaseStyleToolButton->setChecked(false);
    romanUpperStyleToolButton->setChecked(true);
    romanLowerStyleToolButton->setChecked(false);

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");
    removeListFormattingPushButton->setEnabled(true);

    setNumberStyleControlsEnabled(true);
    updateDialog();
}


void ListFormatDialog::lowerCaseRomanNumeralStyleSelected() {
    uncheckBulletStyles();

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    numberStyleToolButton->setChecked(false);
    upperCaseStyleToolButton->setChecked(false);
    lowerCaseStyleToolButton->setChecked(false);
    romanUpperStyleToolButton->setChecked(false);
    romanLowerStyleToolButton->setChecked(true);

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");
    removeListFormattingPushButton->setEnabled(true);

    setNumberStyleControlsEnabled(true);
    updateDialog();
}


void ListFormatDialog::startingNumberChanged(int) {
    updateDialog();
}


void ListFormatDialog::prefixTextChanged(const QString& newText) {
    if (newText != inputPrefixString) {
        QLineEdit* prefixLineEdit = widget<QLineEdit>("prefix_line_edit");
        prefixLineEdit->setPlaceholderText(QString());

        prefixStringWasChanged = true;
    }

    updateDialog();
}


void ListFormatDialog::suffixTextChanged(const QString& newText) {
    if (newText != inputSuffixString) {
        QLineEdit* suffixLineEdit = widget<QLineEdit>("suffix_line_edit");
        suffixLineEdit->setPlaceholderText(QString());

        suffixStringWasChanged = true;
    }

    updateDialog();
}


void ListFormatDialog::removeListFormattingButtonClicked() {
    uncheckBulletStyles();

    QToolButton* numberStyleToolButton      = widget<QToolButton>("number_style_tool_button");
    QToolButton* upperCaseStyleToolButton   = widget<QToolButton>("upper_case_style_tool_button");
    QToolButton* lowerCaseStyleToolButton   = widget<QToolButton>("lower_case_style_tool_button");
    QToolButton* romanUpperStyleToolButton  = widget<QToolButton>("roman_upper_style_tool_button");
    QToolButton* romanLowerStyleToolButton  = widget<QToolButton>("roman_lower_style_tool_button");

    numberStyleToolButton->setChecked(false);
    upperCaseStyleToolButton->setChecked(false);
    lowerCaseStyleToolButton->setChecked(false);
    romanUpperStyleToolButton->setChecked(false);
    romanLowerStyleToolButton->setChecked(false);

    QPushButton* removeListFormattingPushButton = widget<QPushButton>("remove_list_formatting_button");
    removeListFormattingPushButton->setEnabled(false);

    setNumberStyleControlsEnabled(false);
    updateDialog();
}


void ListFormatDialog::showHideButtonNumberCheckBoxClicked(int newState) {
    QCheckBox* showBulletOrNumberCheckBox = widget<QCheckBox>("show_bullet_or_number_check_box");
    if (newState == Qt::PartiallyChecked) {
        showBulletOrNumberCheckBox->setChecked(true);
    }

    showBulletOrNumberCheckBox->setTristate(false);
    updateDialog();
}


void ListFormatDialog::applyClicked() {
    if (showBulletOrNumberCheckBoxEnabled() && showBulletOrNumberCheckState() != inputShowBulletOrNumberCheckState) {
        inputShowBulletOrNumberCheckState = showBulletOrNumberCheckState();
    }

    QString bulletStyle = selectedBulletStyle();
    if (!bulletStyle.isEmpty() && (inputBulletStyles.count() != 1 || *inputBulletStyles.begin() != bulletStyle)) {
        inputBulletStyles.clear();
        inputBulletStyles << bulletStyle;
    }

    NumberStyle numberStyle = selectedNumberStyle();
    if (numberStyle != NumberStyle::INVALID                                           &&
        (inputNumberStyles.count() != 1 || *inputNumberStyles.begin() != numberStyle)    ) {
        inputNumberStyles.clear();
        inputNumberStyles << numberStyle;
    }

    if (isStartingNumberSet()) {
        inputStartingNumber = ListFormatDialog::startingNumber();
    }

    if (isPrefixStringSet()) {
        inputPrefixString    = ListFormatDialog::prefixString();
        inputPrefixStringSet = true;
    }

    if (isSuffixStringSet()) {
        inputSuffixString    = ListFormatDialog::suffixString();
        inputSuffixStringSet = true;
    }

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
        okPushButton->setEnabled(false);

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    emit apply(this);
}


void ListFormatDialog::updateDialog() {
    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        bool valuesHaveChanged = valuesChanged();

        QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
        okPushButton->setEnabled(valuesHaveChanged);

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(valuesHaveChanged);

    #elif (defined(Q_OS_DARWIN))

        if (currentAutoApplyEnabled) {
            applyClicked();
        }

    #else

        #error Unknown platform

    #endif
}


void ListFormatDialog::configureWidget() {
    setWindowTitle(tr("List Format"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    fontFormatDialog = new CharacterFormatDialog(this);
    fontFormatDialog->setDemoTextMode(CharacterFormatDialog::DemoTextMode::LIST);
    fontFormatDialog->setPositionControlsDisabled();
    fontFormatDialog->setApplyButtonHidden();

    useFontDialogValues = false;

    currentAutoApplyEnabled = false;

    inputShowBulletOrNumberCheckState = CheckState::PARTIALLY_CHECKED;
    inputStartingNumber               = static_cast<unsigned>(-1);
    inputPrefixStringSet              = false;
    inputSuffixStringSet              = false;
    inputItalics                      = QuadState::DISABLED;
    inputUnderline                    = QuadState::DISABLED;
    inputOverline                     = QuadState::DISABLED;
    inputStrikethrough                = QuadState::DISABLED;

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QGroupBox* bulletGroupBox = new QGroupBox(tr("Bullet Styles"));
    registerWidget(bulletGroupBox, "bullet_group_box");

    mainLayout->addWidget(bulletGroupBox);

    QList<Ld::UnorderedListParagraphFormat::StandardBullet> standardBullets =
        Ld::UnorderedListParagraphFormat::standardBullets();

    QGridLayout* bulletLayout = newGridLayout("bullet_grid_layout");
    bulletGroupBox->setLayout(bulletLayout);

    unsigned bulletsPerLine = static_cast<unsigned>((standardBullets.size() + 1) / 2);
    bulletsPerLine += (bulletsPerLine % 2);

    unsigned row    = 0;
    unsigned column = 0;

    QSet<QToolButton*> styleToolButtons;

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(standardBullets.size()) ; ++bulletIndex) {
        const Ld::UnorderedListParagraphFormat::StandardBullet& bulletData  = standardBullets.at(bulletIndex);
        QString                                                 bullet      = bulletData.bullet();
        QString                                                 description = bulletData.description();

        QToolButton* bulletToolButton = new QToolButton;
        bulletToolButton->setText(bullet);
        bulletToolButton->setToolTip(tr("Click to make an ordered list using %1s (%2)").arg(description).arg(bullet));
        bulletToolButton->setWhatsThis(
            tr(
               "You can use this button to turn this paragraph or selected paragraphs into an unordered or "
               "non-numbered list using %1s (%2) to mark each paragraph."
            ).arg(description).arg(bullet)
        );
        registerWidget(
            bulletToolButton,
            QString("bullet_tool_button_%1").arg(bulletData.unicodeValue(), 4, 16, QChar('0'))
        );

        bulletLayout->addWidget(bulletToolButton, row, column);
        styleToolButtons << bulletToolButton;

        ++column;
        if (column >= bulletsPerLine) {
            column = 0;
            ++row;
        }
    }

    QHBoxLayout* numberGroupBoxLayout = newHBoxLayout("number_group_box_layout");
    mainLayout->addLayout(numberGroupBoxLayout);

    QGroupBox* numberGroupBox = new QGroupBox(tr("Number Styles"));
    registerWidget(numberGroupBox, "number_group_box");
    numberGroupBoxLayout->addWidget(numberGroupBox);

    numberGroupBoxLayout->addStretch();

    QVBoxLayout* listFontLayout = newVBoxLayout("list_font_layout");
    numberGroupBoxLayout->addLayout(listFontLayout);

    QPushButton* listFontPushButton = new QPushButton(tr("List Font"));
    registerWidget(listFontPushButton, "list_font_push_button");
    listFontLayout->addWidget(listFontPushButton);

    listFontLayout->addStretch();

    QVBoxLayout* numberGroupBoxContentsLayout = newVBoxLayout("number_group_box_contents_layout");
    numberGroupBox->setLayout(numberGroupBoxContentsLayout);

    QHBoxLayout* numberStyleLayout = newHBoxLayout("number_style_layout");
    numberGroupBoxContentsLayout->addLayout(numberStyleLayout);

    QToolButton* numberStyleToolButton = new QToolButton;
    numberStyleToolButton->setText(tr("1."));
    numberStyleToolButton->setToolTip(tr("Click to make an ordered list.  Each paragraph will be numbered."));
    numberStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each "
           "paragraph will be consecutively numbered."
        )
    );
    registerWidget(numberStyleToolButton, "number_style_tool_button");
    numberStyleLayout->addWidget(numberStyleToolButton);

    styleToolButtons << numberStyleToolButton;

    QToolButton* upperCaseStyleToolButton = new QToolButton;
    upperCaseStyleToolButton->setText(tr("A."));
    upperCaseStyleToolButton->setToolTip(
      tr(
         "Click to make an ordered list.  Each paragraph will be marked using upper case letters, in alphabetical "
          "order."
        )
    );
    upperCaseStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using upper case letters in alphabetical order.  If needed, "
           "multiple letters will be used on each paragraph to uniquely mark large numbers consecutive paragraphs."
          )
    );
    registerWidget(upperCaseStyleToolButton, "upper_case_style_tool_button");
    numberStyleLayout->addWidget(upperCaseStyleToolButton);

    styleToolButtons << upperCaseStyleToolButton;

    QToolButton* lowerCaseStyleToolButton = new QToolButton;
    lowerCaseStyleToolButton->setText(tr("a."));
    lowerCaseStyleToolButton->setToolTip(
      tr(
         "Click to make an ordered list.  Each paragraph will be marked using lower case letters, in alphabetical "
          "order."
        )
    );
    lowerCaseStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using lower case letters in alphabetical order.  If needed, "
           "multiple letters will be used on each paragraph to uniquely mark large numbers consecutive paragraphs."
          )
    );
    registerWidget(lowerCaseStyleToolButton, "lower_case_style_tool_button");
    numberStyleLayout->addWidget(lowerCaseStyleToolButton);

    styleToolButtons << lowerCaseStyleToolButton;

    QToolButton* romanUpperStyleToolButton = new QToolButton;
    romanUpperStyleToolButton->setText(tr("IV."));
    romanUpperStyleToolButton->setToolTip(
      tr("Click to make an ordered list.  Each paragraph will be marked using roman numerals.")
    );
    romanUpperStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using roman numerals.  Note that values will be limited to 3999 or"
           "\"MMMCMXCIX\"."
          )
    );
    registerWidget(romanUpperStyleToolButton, "roman_upper_style_tool_button");
    numberStyleLayout->addWidget(romanUpperStyleToolButton);

    styleToolButtons << romanUpperStyleToolButton;

    QToolButton* romanLowerStyleToolButton = new QToolButton;
    romanLowerStyleToolButton->setText(tr("iv."));
    romanLowerStyleToolButton->setToolTip(
      tr("Click to make an ordered list.  Each paragraph will be marked using lower case roman numerals.")
    );
    romanLowerStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using lower case roman numerals.  Note that values will be limited "
           "to 3999 or \"mmmcmxcix\"."
          )
    );
    registerWidget(romanLowerStyleToolButton, "roman_lower_style_tool_button");
    numberStyleLayout->addWidget(romanLowerStyleToolButton);

    styleToolButtons << romanLowerStyleToolButton;


    numberStyleLayout->addSpacing(10);

    QFormLayout* numberSettingsLayout = newFormLayout("number_settings_layout");
    numberGroupBoxContentsLayout->addLayout(numberSettingsLayout);

    QSpinBox* startsWithSpinBox = new QSpinBox;
    startsWithSpinBox->setMinimum(0);
    startsWithSpinBox->setMaximum(3999);
    startsWithSpinBox->setToolTip(tr("Sets the starting paragraph number."));
    startsWithSpinBox->setWhatsThis(
        tr(
           "You can use this spin box to set the starting paragraph number.  A value of 1 will translate to \"1\", "
           "\"A\", \"a\", \'I\", or \"i\" depending on the ordered list style.  If desired, you can set the starting "
           "value to 0 which will cause the first paragraph to be numbered \"0\" or, for Roman numerals \"nulla\"."
          )
    );
    registerWidget(startsWithSpinBox, "starts_with_spin_box");
    numberSettingsLayout->addRow(tr("Starts At: "), startsWithSpinBox);
    registerWidget(numberSettingsLayout->labelForField(startsWithSpinBox), "starts_with_label");

    QLineEdit* prefixLineEdit = new QLineEdit;
    prefixLineEdit->setToolTip(tr("Sets prefix texr to be placed in front of each paragraph number."));
    prefixLineEdit->setWhatsThis(
        tr(
           "You can use this field to enter optional text to be placed in front of the number.  As an example, if you"
           "want paragraphs to be numbers \"(1)\", \"(2)\", etc. you would set the prefix text to \"(\" and suffix "
           "text to \")\"."
        )
    );
    registerWidget(prefixLineEdit, "prefix_line_edit");
    numberSettingsLayout->addRow(tr("Prefix: "), prefixLineEdit);
    registerWidget(numberSettingsLayout->labelForField(prefixLineEdit), "prefix_label");

    QLineEdit* suffixLineEdit = new QLineEdit;
    suffixLineEdit->setToolTip(tr("Sets suffix texr to be placed in front of each paragraph number."));
    suffixLineEdit->setWhatsThis(
        tr(
           "You can use this field to enter optional text to be placed in front of the number.  As an example, if you"
           "want paragraphs to be numbers \"1-\", \"2-\", etc. you would leave the prefix field blank and set the "
           "suffix text to \"-\"."
        )
    );
    registerWidget(suffixLineEdit, "suffix_line_edit");
    numberSettingsLayout->addRow(tr("Suffix: "), suffixLineEdit);
    registerWidget(numberSettingsLayout->labelForField(suffixLineEdit), "suffix_label");

    QPushButton* removeListFormattingButton = new QPushButton(tr("Remove List Formatting"));
    registerWidget(removeListFormattingButton, "remove_list_formatting_button");
    mainLayout->addWidget(removeListFormattingButton);

    QCheckBox* showBulletOrNumberCheckBox = new QCheckBox(tr("Show bullet or number on this paragraph"));
    showBulletOrNumberCheckBox->setTristate(true);
    showBulletOrNumberCheckBox->setToolTip(tr("Click to show or hide paragraph bullets or numbers."));
    showBulletOrNumberCheckBox->setWhatsThis(
        tr(
           "You can use this check box to indicate whether a bullet or number should be displayed or hidden for the "
           "selected paragraphs.  The paragraphs will still remain part of the list and will honor the indentation "
           "settings used for the rest of the list.  If the paragraph is part of an ordered or numbered list, then "
           "paragraph numbers will be reordered depending on whether a number is shown for this paragraph."
        )
    );
    registerWidget(showBulletOrNumberCheckBox, "show_bullet_or_number_check_box");
    mainLayout->addWidget(showBulletOrNumberCheckBox);

    QFontMetrics fontMetrics(Application::font());
    unsigned     maximumSide = static_cast<unsigned>(fontMetrics.height());
    for (  QSet<QToolButton*>::const_iterator buttonIterator    = styleToolButtons.constBegin(),
                                              buttonEndIterator = styleToolButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        QToolButton* button    = *buttonIterator;
        unsigned     textWidth = fontMetrics.horizontalAdvance(button->text());
        if (textWidth > maximumSide) {
            maximumSide = textWidth;
        }
    }

    QSize buttonSize(1.5 * maximumSide, 1.5 * maximumSide);
    for (  QSet<QToolButton*>::const_iterator buttonIterator    = styleToolButtons.constBegin(),
                                              buttonEndIterator = styleToolButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        QToolButton* button = *buttonIterator;
        button->setCheckable(true);
        button->setFixedSize(buttonSize);
    }

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
        );

        buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

        buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
        buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

        registerWidget(buttonBox, "dialog_button_box");
        registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
        registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");
        registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

        mainLayout->addWidget(buttonBox);

    #elif (defined(Q_OS_DARWIN))

    #else

        #error Unknown platform

    #endif
}


bool ListFormatDialog::valuesChanged() const {
    bool valueHasChanged = (
           (   showBulletOrNumberCheckBoxEnabled()
            && inputShowBulletOrNumberCheckState != showBulletOrNumberCheckState()
           )
        || (inputBulletStyles.count() != 1 && !selectedBulletStyle().isEmpty())
        || (inputBulletStyles.count() == 1 && *inputBulletStyles.begin() != selectedBulletStyle())
        || (inputNumberStyles.count() != 1 && selectedNumberStyle() != NumberStyle::INVALID)
        || (inputNumberStyles.count() == 1 && *inputNumberStyles.begin() != selectedNumberStyle())
        || (selectedNumberStyle() != NumberStyle::INVALID && inputStartingNumber != startingNumber())
        || prefixStringWasChanged
        || suffixStringWasChanged
        || (inputFontFamilies.count() != 1 && !fontFamily().isNull())
        || (inputFontFamilies.count() == 1 && *inputFontFamilies.begin() != fontFamily())
        || (inputFontSizes.count() != 1 && fontSize() != 0)
        || (inputFontSizes.count() == 1 && *inputFontSizes.begin() != fontSize())
        || (inputFontWeights.count() != 1 && fontWeight() != Ld::FontFormat::Weight::INVALID)
        || (inputFontWeights.count() == 1 && *inputFontWeights.begin() != fontWeight())
        || (inputItalics == QuadState::ENABLED_AND_DISABLED && !italicsBothEnabledAndDisabled())
        || (inputItalics == QuadState::ENABLED && !italicsEnabled())
        || (inputItalics == QuadState::DISABLED && !italicsDisabled())
        || (inputUnderline == QuadState::ENABLED_AND_DISABLED && !underlineBothEnabledAndDisabled())
        || (inputUnderline == QuadState::ENABLED && !underlineEnabled())
        || (inputUnderline == QuadState::DISABLED && !underlineDisabled())
        || (inputOverline == QuadState::ENABLED_AND_DISABLED && !overlineBothEnabledAndDisabled())
        || (inputOverline == QuadState::ENABLED && !overlineEnabled())
        || (inputOverline == QuadState::DISABLED && !overlineDisabled())
        || (inputStrikethrough == QuadState::ENABLED_AND_DISABLED && !strikethroughBothEnabledAndDisabled())
        || (inputStrikethrough == QuadState::ENABLED && !strikethroughEnabled())
        || (inputStrikethrough == QuadState::DISABLED && !strikethroughDisabled())
        || (inputFontColors.count() != 1 && fontColor().isValid())
        || (inputFontColors.count() == 1 && *inputFontColors.begin() != fontColor())
        || (inputFontBackgroundColors.count() != 1 && fontBackgroundColor().isValid())
        || (inputFontBackgroundColors.count() == 1 && *inputFontBackgroundColors.begin() != fontBackgroundColor())
        || (inputLetterSpacings.count() != 1 && letterSpacingValid())
        || (inputLetterSpacings.count() == 1 && *inputLetterSpacings.begin() != letterSpacing())
    );

    return valueHasChanged;
}


void ListFormatDialog::setNumberStyleControlsEnabled(bool nowEnabled) {
    QLabel*    startsWithLabel   = widget<QLabel>("starts_with_label");
    QSpinBox*  startsWithSpinBox = widget<QSpinBox>("starts_with_spin_box");
    QLabel*    prefixLabel       = widget<QLabel>("prefix_label");
    QLineEdit* prefixLineEdit    = widget<QLineEdit>("prefix_line_edit");
    QLabel*    suffixLabel       = widget<QLabel>("suffix_label");
    QLineEdit* suffixLineEdit    = widget<QLineEdit>("suffix_line_edit");

    startsWithLabel->setEnabled(nowEnabled);
    startsWithSpinBox->setEnabled(nowEnabled);
    prefixLabel->setEnabled(nowEnabled);
    prefixLineEdit->setEnabled(nowEnabled);
    suffixLabel->setEnabled(nowEnabled);
    suffixLineEdit->setEnabled(nowEnabled);
}


void ListFormatDialog::uncheckBulletStyles() {
    QList<Ld::UnorderedListParagraphFormat::StandardBullet> standardBullets =
        Ld::UnorderedListParagraphFormat::standardBullets();

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(standardBullets.size()) ; ++bulletIndex) {
        const Ld::UnorderedListParagraphFormat::StandardBullet& bulletData  = standardBullets.at(bulletIndex);
        QString buttonName = QString("bullet_tool_button_%1").arg(bulletData.unicodeValue(), 4, 16, QChar('0'));

        QToolButton* button = widget<QToolButton>(buttonName);
        button->setChecked(false);
    }
}
