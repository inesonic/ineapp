/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CharacterFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QSet>
#include <QFont>
#include <QFontMetrics>
#include <QFontComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QSizePolicy>
#include <QToolButton>
#include <QPushButton>
#include <QDialogButtonBox>

#include <cstdint>
#include <algorithm>
#include <cmath>

#include <util_bit_functions.h>

#include <eqt_font_data.h>
#include <eqt_color_tool_button.h>
#include <eqt_font_demo_widget.h>
#include <eqt_color_dialog.h>
#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_format.h>
#include <ld_character_format.h>
#include <ld_unordered_list_paragraph_format.h>

#include "application.h"
#include "view_widget.h"
#include "character_format_helpers.h"
#include "text_presentation.h" // For baseline adjustments.
#include "character_format_dialog.h"

CharacterFormatDialog::CharacterFormatDialog(QWidget* parent):ProgrammaticDialog("CharacterFormatDialog", parent) {
    configureWidget();
    runBuilders();
}


CharacterFormatDialog::~CharacterFormatDialog() {}


void CharacterFormatDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void CharacterFormatDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void CharacterFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void CharacterFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool CharacterFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool CharacterFormatDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


QString CharacterFormatDialog::fontFamily() const {
    QString result;

    QFontComboBox* fontFamilyComboBox = widget<QFontComboBox>("font_family_combo_box");
    if (!fontFamilyComboBox->currentText().isEmpty()) {
        result = fontFamilyComboBox->currentFont().family();
    }

    return result;
}


unsigned CharacterFormatDialog::fontSize() const {
    unsigned result;

    QSpinBox* fontSizeSpinBox = widget<QSpinBox>("font_size_spin_box");
    if (!fontSizeSpinBox->text().isEmpty()) {
        result = static_cast<unsigned>(fontSizeSpinBox->value());
    } else {
        result = 0;
    }

    return result;
}


Ld::FontFormat::Weight CharacterFormatDialog::fontWeight() const {
    Ld::FontFormat::Weight result = Ld::FontFormat::Weight::INVALID;

    QRadioButton* thinFontWeightRadioButton       = widget<QRadioButton>("thin_font_weight_radio_button");
    QRadioButton* extraLightFontWeightRadioButton = widget<QRadioButton>("extra_light_font_weight_radio_button");
    QRadioButton* lightFontWeightRadioButton      = widget<QRadioButton>("light_font_weight_radio_button");
    QRadioButton* normalFontWeightRadioButton     = widget<QRadioButton>("normal_font_weight_radio_button");
    QRadioButton* mediumFontWeightRadioButton     = widget<QRadioButton>("medium_font_weight_radio_button");
    QRadioButton* demiBoldFontWeightRadioButton   = widget<QRadioButton>("demi_bold_font_weight_radio_button");
    QRadioButton* boldFontWeightRadioButton       = widget<QRadioButton>("bold_font_weight_radio_button");
    QRadioButton* extraBoldFontWeightRadioButton  = widget<QRadioButton>("extra_bold_font_weight_radio_button");
    QRadioButton* blackFontWeightRadioButton      = widget<QRadioButton>("black_font_weight_radio_button");

    bool thinFontWeightChecked       = thinFontWeightRadioButton->isChecked();
    bool extraLightFontWeightChecked = extraLightFontWeightRadioButton->isChecked();
    bool lightFontWeightChecked      = lightFontWeightRadioButton->isChecked();
    bool normalFontWeightChecked     = normalFontWeightRadioButton->isChecked();
    bool mediumFontWeightChecked     = mediumFontWeightRadioButton->isChecked();
    bool demiBoldFontWeightChecked   = demiBoldFontWeightRadioButton->isChecked();
    bool boldFontWeightChecked       = boldFontWeightRadioButton->isChecked();
    bool extraBoldFontWeightChecked  = extraBoldFontWeightRadioButton->isChecked();
    bool blackFontWeightChecked      = blackFontWeightRadioButton->isChecked();

    unsigned numberSetFlags = 0;

    if (thinFontWeightChecked) {
        result = Ld::FontFormat::Weight::THIN;
        ++numberSetFlags;
    }

    if (extraLightFontWeightChecked) {
        result = Ld::FontFormat::Weight::EXTRA_LIGHT;
        ++numberSetFlags;
    }

    if (lightFontWeightChecked) {
        result = Ld::FontFormat::Weight::LIGHT;
        ++numberSetFlags;
    }

    if (normalFontWeightChecked) {
        result = Ld::FontFormat::Weight::NORMAL;
        ++numberSetFlags;
    }

    if (mediumFontWeightChecked) {
        result = Ld::FontFormat::Weight::MEDIUM;
        ++numberSetFlags;
    }

    if (demiBoldFontWeightChecked) {
        result = Ld::FontFormat::Weight::DEMI_BOLD;
        ++numberSetFlags;
    }

    if (boldFontWeightChecked) {
        result = Ld::FontFormat::Weight::BOLD;
        ++numberSetFlags;
    }

    if (extraBoldFontWeightChecked) {
        result = Ld::FontFormat::Weight::EXTRA_BOLD;
        ++numberSetFlags;
    }

    if (blackFontWeightChecked) {
        result = Ld::FontFormat::Weight::BLACK;
        ++numberSetFlags;
    }

    if (numberSetFlags != 1) {
        result = Ld::FontFormat::Weight::INVALID;
    }

    return result;
}


bool CharacterFormatDialog::italicsEnabled() const {
    QCheckBox* italicsCheckBox = widget<QCheckBox>("italics_check_box");
    return italicsCheckBox->checkState() == Qt::Checked;
}


bool CharacterFormatDialog::italicsDisabled() const {
    QCheckBox* italicsCheckBox = widget<QCheckBox>("italics_check_box");
    return italicsCheckBox->checkState() == Qt::Unchecked;
}


bool CharacterFormatDialog::italicsBothEnabledAndDisabled() const {
    QCheckBox* italicsCheckBox = widget<QCheckBox>("italics_check_box");
    return italicsCheckBox->checkState() == Qt::PartiallyChecked;
}


bool CharacterFormatDialog::underlineEnabled() const {
    QCheckBox* underlineCheckBox = widget<QCheckBox>("underline_check_box");
    return underlineCheckBox->checkState() == Qt::Checked;
}


bool CharacterFormatDialog::underlineDisabled() const {
    QCheckBox* underlineCheckBox = widget<QCheckBox>("underline_check_box");
    return underlineCheckBox->checkState() == Qt::Unchecked;
}


bool CharacterFormatDialog::underlineBothEnabledAndDisabled() const {
    QCheckBox* underlineCheckBox = widget<QCheckBox>("underline_check_box");
    return underlineCheckBox->checkState() == Qt::PartiallyChecked;
}


bool CharacterFormatDialog::overlineEnabled() const {
    QCheckBox* overlineCheckBox = widget<QCheckBox>("overline_check_box");
    return overlineCheckBox->checkState() == Qt::Checked;
}


bool CharacterFormatDialog::overlineDisabled() const {
    QCheckBox* overlineCheckBox = widget<QCheckBox>("overline_check_box");
    return overlineCheckBox->checkState() == Qt::Unchecked;
}


bool CharacterFormatDialog::overlineBothEnabledAndDisabled() const {
    QCheckBox* overlineCheckBox = widget<QCheckBox>("overline_check_box");
    return overlineCheckBox->checkState() == Qt::PartiallyChecked;
}


bool CharacterFormatDialog::strikethroughEnabled() const {
    QCheckBox* strikethroughCheckBox = widget<QCheckBox>("strikethrough_check_box");
    return strikethroughCheckBox->checkState() == Qt::Checked;
}


bool CharacterFormatDialog::strikethroughDisabled() const {
    QCheckBox* strikethroughCheckBox = widget<QCheckBox>("strikethrough_check_box");
    return strikethroughCheckBox->checkState() == Qt::Unchecked;
}


bool CharacterFormatDialog::strikethroughBothEnabledAndDisabled() const {
    QCheckBox* strikethroughCheckBox = widget<QCheckBox>("strikethrough_check_box");
    return strikethroughCheckBox->checkState() == Qt::PartiallyChecked;
}


Ld::CharacterFormat::Position CharacterFormatDialog::position() const {
    Ld::CharacterFormat::Position result;

    QRadioButton* superscriptRadioButton    = widget<QRadioButton>("superscript_radio_button");
    QRadioButton* normalBaselineRadioButton = widget<QRadioButton>("normal_baseline_radio_button");
    QRadioButton* subscriptRadioButton      = widget<QRadioButton>("subscript_radio_button");

    bool superscriptChecked    = superscriptRadioButton->isChecked();
    bool normalBaselineChecked = normalBaselineRadioButton->isChecked();
    bool subscriptChecked      = subscriptRadioButton->isChecked();

    if (superscriptChecked) {
        if (!normalBaselineChecked && !subscriptChecked) {
            result = Ld::CharacterFormat::Position::SUPERSCRIPT;
        } else {
            result = Ld::CharacterFormat::Position::INVALID;
        }
    } else if (normalBaselineChecked) {
        if (!subscriptChecked) {
            result = Ld::CharacterFormat::Position::NORMAL;
        } else {
            result = Ld::CharacterFormat::Position::INVALID;
        }
    } else if (subscriptChecked) {
        result = Ld::CharacterFormat::Position::SUBSCRIPT;
    } else {
        result = Ld::CharacterFormat::Position::INVALID;
    }

    return result;
}


QColor CharacterFormatDialog::fontColor() const {
    QColor result;

    EQt::ColorToolButton* fontColorToolButton = widget<EQt::ColorToolButton>("change_font_color_tool_button");
    QSet<QColor>          colors              = fontColorToolButton->colors();

    if (colors.count() == 1) {
        result = *colors.begin();
    }

    return result;
}


bool CharacterFormatDialog::fontColorSelected() const {
    EQt::ColorToolButton* fontColorToolButton = widget<EQt::ColorToolButton>("change_font_color_tool_button");
    QSet<QColor>          colors              = fontColorToolButton->colors();

    return (colors != inputFontColors);
}


QColor CharacterFormatDialog::fontBackgroundColor() const {
    QColor result;

    EQt::ColorToolButton* fontBackgroundColorToolButton = widget<EQt::ColorToolButton>(
        "change_font_background_color_tool_button"
    );
    QSet<QColor> colors = fontBackgroundColorToolButton->colors();

    if (colors.count() == 1) {
        result = *colors.begin();
    }

    return result;
}


bool CharacterFormatDialog::fontBackgroundColorSelected() const {
    EQt::ColorToolButton* fontBackgroundColorToolButton = widget<EQt::ColorToolButton>(
        "change_font_background_color_tool_button"
    );
    QSet<QColor> colors = fontBackgroundColorToolButton->colors();

    return (colors != inputFontBackgroundColors);
}


float CharacterFormatDialog::letterSpacing() const {
    QDoubleSpinBox* letterSpacingSpinBox = widget<QDoubleSpinBox>("letter_spacing_spin_box");
    return letterSpacingSpinBox->value() / 100.0F;
}


bool CharacterFormatDialog::letterSpacingValid() const {
    QDoubleSpinBox* letterSpacingSpinBox = widget<QDoubleSpinBox>("letter_spacing_spin_box");
    return letterSpacingSpinBox->text() != letterSpacingSpinBox->suffix();
}


bool CharacterFormatDialog::letterSpacingInvalid() const {
    QDoubleSpinBox* letterSpacingSpinBox = widget<QDoubleSpinBox>("letter_spacing_spin_box");
    return letterSpacingSpinBox->text() == letterSpacingSpinBox->suffix();
}


bool CharacterFormatDialog::positionControlsEnabled() const {
    QGroupBox* positionGroupBox = widget<QGroupBox>("position_group_box");
    return positionGroupBox->isVisible();
}


bool CharacterFormatDialog::positionControlsDisabled() const {
    return !positionControlsEnabled();
}


bool CharacterFormatDialog::fontBackgroundColorControlsEnabled() const {
    QGroupBox* fontBackgroundColorGroupBox = widget<QGroupBox>("font_background_color_group_box");
    return fontBackgroundColorGroupBox->isVisible();
}


bool CharacterFormatDialog::fontBackgroundColorControlsDisabled() const {
    return !fontBackgroundColorControlsEnabled();
}


CharacterFormatDialog::DemoTextMode CharacterFormatDialog::demoTextMode() const {
    return currentDemoTextMode;
}


bool CharacterFormatDialog::applyButtonVisible() const {
    bool result;

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        result = applyPushButton->isVisible();

    #elif (defined(Q_OS_DARWIN))

        result = false;

    #else

        #error Unknown platform

    #endif

    return result;
}


bool CharacterFormatDialog::applyButtonHidden() const {
    return !applyButtonVisible();
}


void CharacterFormatDialog::populate() {
    QFontComboBox* fontFamilyComboBox = widget<QFontComboBox>("font_family_combo_box");
    if (inputFontFamilies.count() == 1) {
        fontFamilyComboBox->setCurrentFont(QFont(*inputFontFamilies.begin()));
    } else {
        fontFamilyComboBox->clearEditText();
    }

    QSpinBox* fontSizeSpinBox = widget<QSpinBox>("font_size_spin_box");
    if (inputFontSizes.count() == 1) {
        fontSizeSpinBox->setValue(*inputFontSizes.begin());
    } else {
        fontSizeSpinBox->clear();
    }

    QRadioButton* thinFontWeightRadioButton = widget<QRadioButton>("thin_font_weight_radio_button");
    thinFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::THIN));

    QRadioButton* extraLightFontWeightRadioButton = widget<QRadioButton>("extra_light_font_weight_radio_button");
    extraLightFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::EXTRA_LIGHT));

    QRadioButton* lightFontWeightRadioButton = widget<QRadioButton>("light_font_weight_radio_button");
    lightFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::LIGHT));

    QRadioButton* normalFontWeightRadioButton = widget<QRadioButton>("normal_font_weight_radio_button");
    normalFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::NORMAL));

    QRadioButton* mediumFontWeightRadioButton = widget<QRadioButton>("medium_font_weight_radio_button");
    mediumFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::MEDIUM));

    QRadioButton* demiBoldFontWeightRadioButton = widget<QRadioButton>("demi_bold_font_weight_radio_button");
    demiBoldFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::DEMI_BOLD));

    QRadioButton* boldFontWeightRadioButton = widget<QRadioButton>("bold_font_weight_radio_button");
    boldFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::BOLD));

    QRadioButton* extraBoldFontWeightRadioButton = widget<QRadioButton>("extra_bold_font_weight_radio_button");
    extraBoldFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::EXTRA_BOLD));

    QRadioButton* blackFontWeightRadioButton = widget<QRadioButton>("black_font_weight_radio_button");
    blackFontWeightRadioButton->setChecked(inputFontWeights.contains(Ld::FontFormat::Weight::BLACK));

    QCheckBox* italicsCheckBox = widget<QCheckBox>("italics_check_box");
    updateCheckbox(italicsCheckBox, inputItalics);

    QCheckBox* underlineCheckBox = widget<QCheckBox>("underline_check_box");
    updateCheckbox(underlineCheckBox, inputUnderline);

    QCheckBox* overlineCheckBox = widget<QCheckBox>("overline_check_box");
    updateCheckbox(overlineCheckBox, inputOverline);

    QCheckBox* strikethroughCheckBox = widget<QCheckBox>("strikethrough_check_box");
    updateCheckbox(strikethroughCheckBox, inputStrikethrough);

    QRadioButton* superscriptRadioButton = widget<QRadioButton>("superscript_radio_button");
    superscriptRadioButton->setChecked(inputPositions.contains(Ld::CharacterFormat::Position::SUPERSCRIPT));

    QRadioButton* normalBaselineRadioButton = widget<QRadioButton>("normal_baseline_radio_button");
    normalBaselineRadioButton->setChecked(inputPositions.contains(Ld::CharacterFormat::Position::NORMAL));

    QRadioButton* subscriptRadioButton = widget<QRadioButton>("subscript_radio_button");
    subscriptRadioButton->setChecked(inputPositions.contains(Ld::CharacterFormat::Position::SUBSCRIPT));

    EQt::ColorToolButton* fontColorToolButton = widget<EQt::ColorToolButton>("change_font_color_tool_button");
    fontColorToolButton->setColors(inputFontColors);

    EQt::ColorToolButton* fontBackgroundColorToolButton = widget<EQt::ColorToolButton>(
        "change_font_background_color_tool_button"
    );
    fontBackgroundColorToolButton->setColors(inputFontBackgroundColors);

    QDoubleSpinBox* letterSpacingSpinBox = widget<QDoubleSpinBox>("letter_spacing_spin_box");
    if (inputLetterSpacings.count() == 1) {
        float letterSpacing = *inputLetterSpacings.begin();
        letterSpacingSpinBox->setValue(100.0F * letterSpacing);
    } else {
        letterSpacingSpinBox->clear();
    }

    updateColorButtonStates();
    updateWeights();
    updateSampleText();

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(false);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif
}


void CharacterFormatDialog::setFontFamily(const QString& newFontFamily) {
    inputFontFamilies.clear();
    inputFontFamilies << newFontFamily;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontFamilies(const QSet<QString>& newFontFamilies) {
    inputFontFamilies = newFontFamilies;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontSize(unsigned newFontSize) {
    inputFontSizes.clear();
    inputFontSizes << newFontSize;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontSizes(const QSet<unsigned>& newFontSizes) {
    inputFontSizes = newFontSizes;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontWeight(Ld::FontFormat::Weight newFontWeight) {
    inputFontWeights.clear();
    inputFontWeights << newFontWeight;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontWeights(const QSet<Ld::FontFormat::Weight>& newFontWeights) {
    inputFontWeights = newFontWeights;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setSupportedFontWeights(const QSet<Ld::FontFormat::Weight>& newSupportedFontWeights) {
    inputSupportedFontWeights = newSupportedFontWeights;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setItalicsEnabled(bool nowEnabled) {
    inputItalics = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setItalicsDisabled(bool nowDisabled) {
    setItalicsEnabled(!nowDisabled);
}


void CharacterFormatDialog::setItalicsBothEnabledAndDisabled() {
    inputItalics = QuadState::ENABLED_AND_DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setUnderlineEnabled(bool nowEnabled) {
    inputUnderline = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setUnderlineDisabled(bool nowDisabled) {
    setUnderlineEnabled(!nowDisabled);
}


void CharacterFormatDialog::setUnderlineBothEnabledAndDisabled() {
    inputUnderline = QuadState::ENABLED_AND_DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setOverlineEnabled(bool nowEnabled) {
    inputOverline = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setOverlineDisabled(bool nowDisabled) {
    setOverlineEnabled(!nowDisabled);
}


void CharacterFormatDialog::setOverlineBothEnabledAndDisabled() {
    inputUnderline = QuadState::ENABLED_AND_DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setStrikethroughEnabled(bool nowEnabled) {
    inputStrikethrough = nowEnabled ? QuadState::ENABLED : QuadState::DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setStrikethroughDisabled(bool nowDisabled) {
    setStrikethroughEnabled(!nowDisabled);
}


void CharacterFormatDialog::setStrikethroughBothEnabledAndDisabled() {
    inputStrikethrough = QuadState::ENABLED_AND_DISABLED;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setPosition(Ld::CharacterFormat::Position newPosition) {
    inputPositions.clear();
    inputPositions << newPosition;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setPositions(const QSet<Ld::CharacterFormat::Position>& newPositions) {
    inputPositions = newPositions;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontColor(const QColor& newFontColor) {
    inputFontColors.clear();
    inputFontColors << newFontColor;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontColors(const QSet<QColor>& newFontColors) {
    inputFontColors = newFontColors;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontBackgroundColor(const QColor& newFontBackgroundColor) {
    inputFontBackgroundColors.clear();
    inputFontBackgroundColors << newFontBackgroundColor;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setFontBackgroundColors(const QSet<QColor>& newFontBackgroundColors) {
    inputFontBackgroundColors = newFontBackgroundColors;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setLetterSpacing(float newLetterSpacing) {
    inputLetterSpacings.clear();
    inputLetterSpacings << newLetterSpacing;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setLetterSpacing(const QSet<float>& newLetterSpacing) {
    inputLetterSpacings = newLetterSpacing;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setPositionControlsEnabled(bool nowEnabled) {
    QGroupBox* positionGroupBox = widget<QGroupBox>("position_group_box");
    positionGroupBox->setVisible(nowEnabled);
}


void CharacterFormatDialog::setPositionControlsDisabled(bool nowDisabled) {
    setPositionControlsEnabled(!nowDisabled);
}


void CharacterFormatDialog::setFontBackgroundColorControlsEnabled(bool nowEnabled) {
    QGroupBox* fontBackgroundColorGroupBox = widget<QGroupBox>("font_background_color_group_box");
    fontBackgroundColorGroupBox->setVisible(nowEnabled);
}


void CharacterFormatDialog::setFontBackgroundColorControlsDisabled(bool nowDisabled) {
    setFontBackgroundColorControlsEnabled(!nowDisabled);
}


void CharacterFormatDialog::setDemoTextMode(DemoTextMode newDemoTextMode) {
    currentDemoTextMode = newDemoTextMode;

    if (isVisible()) {
        populate();
    }
}


void CharacterFormatDialog::setApplyButtonVisible(bool nowVisible) {
    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setVisible(nowVisible);

    #elif (defined(Q_OS_DARWIN))

        (void) nowVisible;

    #else

        #error Unknown platform

    #endif
}


void CharacterFormatDialog::setApplyButtonHidden(bool nowHidden) {
    setApplyButtonVisible(!nowHidden);
}


void CharacterFormatDialog::bind() {
    EQt::FontDemoWidget* sampleTextDemoWidget = widget<EQt::FontDemoWidget>("sample_text_demo_widget");
    connect(
        sampleTextDemoWidget,
        &EQt::FontDemoWidget::scaleFactorChanged,
        this,
        &CharacterFormatDialog::sampleTextScalingChanged
    );

    QFontComboBox* fontFamilyComboBox = widget<QFontComboBox>("font_family_combo_box");
    connect(
        fontFamilyComboBox,
        &QFontComboBox::currentFontChanged,
        this,
        &CharacterFormatDialog::newFontFamilySelected
    );

    QSpinBox* fontSizeSpinBox = widget<QSpinBox>("font_size_spin_box");
    connect(
        fontSizeSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &CharacterFormatDialog::newFontSizeSelected
    );

    QRadioButton* thinFontWeightRadioButton = widget<QRadioButton>("thin_font_weight_radio_button");
    connect(
        thinFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::thinFontWeightSelected
    );

    QRadioButton* extraLightFontWeightRadioButton = widget<QRadioButton>("extra_light_font_weight_radio_button");
    connect(
        extraLightFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::extraLightFontWeightSelected
    );

    QRadioButton* lightFontWeightRadioButton = widget<QRadioButton>("light_font_weight_radio_button");
    connect(
        lightFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::lightFontWeightSelected
    );

    QRadioButton* normalFontWeightRadioButton = widget<QRadioButton>("normal_font_weight_radio_button");
    connect(
        normalFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::normalFontWeightSelected
    );

    QRadioButton* mediumFontWeightRadioButton = widget<QRadioButton>("medium_font_weight_radio_button");
    connect(
        mediumFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::mediumFontWeightSelected
    );


    QRadioButton* demiBoldFontWeightRadioButton = widget<QRadioButton>("demi_bold_font_weight_radio_button");
    connect(
        demiBoldFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::demiBoldFontWeightSelected
    );

    QRadioButton* boldFontWeightRadioButton = widget<QRadioButton>("bold_font_weight_radio_button");
    connect(
        boldFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::boldFontWeightSelected
    );

    QRadioButton* extraBoldFontWeightRadioButton = widget<QRadioButton>("extra_bold_font_weight_radio_button");
    connect(
        extraBoldFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::extraBoldFontWeightSelected
    );

    QRadioButton* blackFontWeightRadioButton = widget<QRadioButton>("black_font_weight_radio_button");
    connect(
        blackFontWeightRadioButton,
        &QRadioButton::clicked,
        this,
        &CharacterFormatDialog::blackFontWeightSelected
    );

    QCheckBox* italicsCheckBox = widget<QCheckBox>("italics_check_box");
    connect(italicsCheckBox, &QCheckBox::clicked, this, &CharacterFormatDialog::italicsClicked);

    QCheckBox* underlineCheckBox = widget<QCheckBox>("underline_check_box");
    connect(underlineCheckBox, &QCheckBox::clicked, this, &CharacterFormatDialog::underlineClicked);

    QCheckBox* overlineCheckBox = widget<QCheckBox>("overline_check_box");
    connect(overlineCheckBox, &QCheckBox::clicked, this, &CharacterFormatDialog::overlineClicked);

    QCheckBox* strikethroughCheckBox = widget<QCheckBox>("strikethrough_check_box");
    connect(strikethroughCheckBox, &QCheckBox::clicked, this, &CharacterFormatDialog::strikethroughClicked);

    QRadioButton* superscriptRadioButton = widget<QRadioButton>("superscript_radio_button");
    connect(superscriptRadioButton, &QRadioButton::clicked, this, &CharacterFormatDialog::superscriptSelected);

    QRadioButton* normalBaselineRadioButton = widget<QRadioButton>("normal_baseline_radio_button");
    connect(normalBaselineRadioButton, &QRadioButton::clicked, this, &CharacterFormatDialog::normalBaselineSelected);

    QRadioButton* subscriptRadioButton = widget<QRadioButton>("subscript_radio_button");
    connect(subscriptRadioButton, &QRadioButton::clicked, this, &CharacterFormatDialog::subscriptSelected);

    EQt::ColorToolButton* fontColorToolButton = widget<EQt::ColorToolButton>("change_font_color_tool_button");
    connect(
        fontColorToolButton,
        &EQt::ColorToolButton::clicked,
        this,
        &CharacterFormatDialog::changeFontColorButtonClicked
    );

    QPushButton* changeFontColorPushButton = widget<QPushButton>("change_font_color_push_button");
    connect(
        changeFontColorPushButton,
        &QPushButton::clicked,
        this,
        &CharacterFormatDialog::changeFontColorButtonClicked
    );

    QPushButton* removeFontColorPushButton = widget<QPushButton>("remove_font_color_push_button");
    connect(
        removeFontColorPushButton,
        &QPushButton::clicked,
        this,
        &CharacterFormatDialog::removeFontColorButtonClicked
    );

    EQt::ColorToolButton* fontBackgroundColorToolButton = widget<EQt::ColorToolButton>(
        "change_font_background_color_tool_button"
    );
    connect(
        fontBackgroundColorToolButton,
        &EQt::ColorToolButton::clicked,
        this,
        &CharacterFormatDialog::changeFontBackgroundColorButtonClicked
    );

    QPushButton* changefontBackgroundColorPushButton = widget<QPushButton>(
        "change_font_background_color_push_button"
    );
    connect(
        changefontBackgroundColorPushButton,
        &QPushButton::clicked,
        this,
        &CharacterFormatDialog::changeFontBackgroundColorButtonClicked
    );

    QPushButton* removefontBackgroundColorPushButton = widget<QPushButton>(
        "remove_font_background_color_push_button"
    );
    connect(
        removefontBackgroundColorPushButton,
        &QPushButton::clicked,
        this,
        &CharacterFormatDialog::removeFontBackgroundColorButtonClicked
    );

    QDoubleSpinBox* letterSpacingSpinBox = widget<QDoubleSpinBox>("letter_spacing_spin_box");
    connect(
        letterSpacingSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        static_cast<void (CharacterFormatDialog::*)(double)>(&CharacterFormatDialog::letterSpacingChanged)
    );

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &CharacterFormatDialog::accept);

    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
    connect(cancelButton, &QPushButton::clicked, this, &CharacterFormatDialog::reject);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        connect(applyButton, &QPushButton::clicked, this, &CharacterFormatDialog::applyClicked);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    ProgrammaticDialog::bind();
}


void CharacterFormatDialog::newFontFamilySelected(const QFont&) {
    updateDialog();
}


void CharacterFormatDialog::newFontSizeSelected(int) {
    updateDialog();
}


void CharacterFormatDialog::thinFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::THIN);
}


void CharacterFormatDialog::extraLightFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::EXTRA_LIGHT);
}


void CharacterFormatDialog::lightFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::LIGHT);
}


void CharacterFormatDialog::normalFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::NORMAL);
}


void CharacterFormatDialog::mediumFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::MEDIUM);
}


void CharacterFormatDialog::demiBoldFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::DEMI_BOLD);
}


void CharacterFormatDialog::boldFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::BOLD);
}


void CharacterFormatDialog::extraBoldFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::EXTRA_BOLD);
}


void CharacterFormatDialog::blackFontWeightSelected() {
    updateWeightRadioButtons(Ld::FontFormat::Weight::BLACK);
}


void CharacterFormatDialog::italicsClicked() {
    QCheckBox* italicsCheckBox = widget<QCheckBox>("italics_check_box");
    if (italicsCheckBox->isTristate()) {
        italicsCheckBox->setTristate(false);
        italicsCheckBox->setChecked(false);
    }

    updateDialog();
}


void CharacterFormatDialog::underlineClicked() {
    QCheckBox* underlineCheckBox = widget<QCheckBox>("underline_check_box");
    if (underlineCheckBox->isTristate()) {
        underlineCheckBox->setTristate(false);
        underlineCheckBox->setChecked(false);
    }

    updateDialog();
}


void CharacterFormatDialog::overlineClicked() {
    QCheckBox* overlineCheckBox = widget<QCheckBox>("overline_check_box");
    if (overlineCheckBox->isTristate()) {
        overlineCheckBox->setTristate(false);
        overlineCheckBox->setChecked(false);
    }

    updateDialog();
}


void CharacterFormatDialog::strikethroughClicked() {
    QCheckBox* strikethroughCheckBox = widget<QCheckBox>("strikethrough_check_box");
    if (strikethroughCheckBox->isTristate()) {
        strikethroughCheckBox->setTristate(false);
        strikethroughCheckBox->setChecked(false);
    }

    updateDialog();
}


void CharacterFormatDialog::superscriptSelected() {
    updatePositionRadioButtons(Ld::CharacterFormat::Position::SUPERSCRIPT);
}


void CharacterFormatDialog::normalBaselineSelected() {
    updatePositionRadioButtons(Ld::CharacterFormat::Position::NORMAL);
}


void CharacterFormatDialog::subscriptSelected() {
    updatePositionRadioButtons(Ld::CharacterFormat::Position::SUBSCRIPT);
}


void CharacterFormatDialog::changeFontColorButtonClicked() {
    EQt::ColorToolButton* fontColorToolButton = widget<EQt::ColorToolButton>("change_font_color_tool_button");
    QSet<QColor>          colors = fontColorToolButton->colors();

    EQt::ColorDialog colorDialog(this);

    if (colors.count() == 1) {
        colorDialog.setCurrentColor(*colors.begin());
    }

    colorDialog.setWindowTitle(tr("Font Color"));
    colorDialog.loadSettings();

    int result = colorDialog.exec();
    if (result == EQt::ColorDialog::Accepted) {
        colorDialog.saveSettings();
        fontColorToolButton->setColor(colorDialog.selectedColor());

        updateDialog();
    }
}


void CharacterFormatDialog::removeFontColorButtonClicked() {
    EQt::ColorToolButton* fontColorToolButton = widget<EQt::ColorToolButton>("change_font_color_tool_button");
    fontColorToolButton->setColor(QColor());

    updateDialog();
}


void CharacterFormatDialog::changeFontBackgroundColorButtonClicked() {
    EQt::ColorToolButton* fontBackgroundColorToolButton = widget<EQt::ColorToolButton>(
        "change_font_background_color_tool_button"
    );
    QSet<QColor> colors = fontBackgroundColorToolButton->colors();

    EQt::ColorDialog colorDialog(this);

    if (colors.count() == 1) {
        colorDialog.setCurrentColor(*colors.begin());
    }

    colorDialog.setWindowTitle(tr("Highlight Color"));
    colorDialog.loadSettings();

    int result = colorDialog.exec();
    if (result == EQt::ColorDialog::Accepted) {
        colorDialog.saveSettings();
        fontBackgroundColorToolButton->setColor(colorDialog.selectedColor());

        updateDialog();
    }
}


void CharacterFormatDialog::removeFontBackgroundColorButtonClicked() {
    EQt::ColorToolButton* fontBackgroundColorToolButton = widget<EQt::ColorToolButton>(
        "change_font_background_color_tool_button"
    );
    fontBackgroundColorToolButton->setColor(QColor());

    updateDialog();
}


void CharacterFormatDialog::letterSpacingChanged(double) {
    updateDialog();
}


void CharacterFormatDialog::sampleTextScalingChanged(float newScalingFactor) {
    QGroupBox* sampleTextGroupBox   = widget<QGroupBox>("sample_text_group_box");

    float denominator = 0;
    float numerator;
    float error;

    do {
        denominator += 1;
        numerator = newScalingFactor * denominator;
        error     = std::fabs(numerator - std::round(numerator));
    } while (error > 0.0000001);

    sampleTextGroupBox->setTitle(tr("Sample Text (%1:%2 scale)").arg(numerator).arg(denominator));
}


void CharacterFormatDialog::applyClicked() {
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


void CharacterFormatDialog::updateDialog() {
    updateSampleText();
    updateWeights();
    updateColorButtonStates();

    bool valuesHaveChanged = valuesChanged();

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(valuesHaveChanged);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(valuesHaveChanged);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    if (currentAutoApplyEnabled) {
        applyClicked();
    }
}


void CharacterFormatDialog::configureWidget() {
    setWindowTitle(tr("Character Format"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentAutoApplyEnabled   = false;

    inputFontFamilies.clear();
    inputFontSizes.clear();
    inputFontWeights.clear();
    inputSupportedFontWeights = supportedFontWeights();
    inputItalics = QuadState::NO_VALUE;
    inputUnderline = QuadState::NO_VALUE;
    inputOverline = QuadState::NO_VALUE;
    inputStrikethrough = QuadState::NO_VALUE;
    inputPositions.clear();
    inputFontColors.clear();
    inputFontBackgroundColors.clear();

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* sampleHorizontalLayout = newHBoxLayout("sample_horizontal_layout");
    mainLayout->addLayout(sampleHorizontalLayout);

    sampleHorizontalLayout->addStretch(1);

    QGroupBox* sampleTextGroupBox = new QGroupBox(tr("Sample (1:1 scale)"));
    registerWidget(sampleTextGroupBox, "sample_text_group_box");
    sampleHorizontalLayout->addWidget(sampleTextGroupBox);

    sampleHorizontalLayout->addStretch(1);

    QVBoxLayout* sampleTextLayout = newVBoxLayout("sample_text_layout");
    sampleTextGroupBox->setLayout(sampleTextLayout);

    EQt::FontDemoWidget* sampleTextDemoWidget = new EQt::FontDemoWidget;
    sampleTextDemoWidget->setScalingLimits(QSize(600, 600));
    registerWidget(sampleTextDemoWidget, "sample_text_demo_widget");
    sampleTextLayout->addWidget(sampleTextDemoWidget, Qt::AlignCenter);

    QHBoxLayout* basicSettingsHorizontalLayout = newHBoxLayout("top_horizontal_layout");
    mainLayout->addLayout(basicSettingsHorizontalLayout);

    QFontComboBox* fontFamilyComboBox = new QFontComboBox;
    registerWidget(fontFamilyComboBox, "font_family_combo_box");
    basicSettingsHorizontalLayout->addWidget(fontFamilyComboBox);

    QSpinBox* fontSizeSpinBox = new QSpinBox;
    fontSizeSpinBox->setRange(Ld::FontFormat::minimumFontSize, Ld::FontFormat::maximumFontSize);
    fontSizeSpinBox->setSuffix(tr(" pt."));
    registerWidget(fontSizeSpinBox, "font_size_spin_box");
    basicSettingsHorizontalLayout->addWidget(fontSizeSpinBox);

    QHBoxLayout* embellishmentsHorizontalLayout = newHBoxLayout("embellishments_horizontal_layout");
    mainLayout->addLayout(embellishmentsHorizontalLayout);

    QGroupBox* fontWeightGroupBox = new QGroupBox(tr("Weight"));
    registerWidget(fontWeightGroupBox, "font_weight_group_box");
    embellishmentsHorizontalLayout->addWidget(fontWeightGroupBox);

    QVBoxLayout* fontWeightLayout = newVBoxLayout("font_weight_layout");
    fontWeightGroupBox->setLayout(fontWeightLayout);

    QRadioButton* thinFontWeightRadioButton = new QRadioButton(tr("Thin"));
    thinFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(thinFontWeightRadioButton, "thin_font_weight_radio_button");
    fontWeightLayout->addWidget(thinFontWeightRadioButton);

    QRadioButton* extraLightFontWeightRadioButton = new QRadioButton(tr("Extra Light"));
    extraLightFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(extraLightFontWeightRadioButton, "extra_light_font_weight_radio_button");
    fontWeightLayout->addWidget(extraLightFontWeightRadioButton);

    QRadioButton* lightFontWeightRadioButton = new QRadioButton(tr("Light"));
    lightFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(lightFontWeightRadioButton, "light_font_weight_radio_button");
    fontWeightLayout->addWidget(lightFontWeightRadioButton);

    QRadioButton* normalFontWeightRadioButton = new QRadioButton(tr("Normal"));
    normalFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(normalFontWeightRadioButton, "normal_font_weight_radio_button");
    fontWeightLayout->addWidget(normalFontWeightRadioButton);

    QRadioButton* mediumFontWeightRadioButton = new QRadioButton(tr("Medium"));
    mediumFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(mediumFontWeightRadioButton, "medium_font_weight_radio_button");
    fontWeightLayout->addWidget(mediumFontWeightRadioButton);

    QRadioButton* demiBoldFontWeightRadioButton = new QRadioButton(tr("Demi Bold"));
    demiBoldFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(demiBoldFontWeightRadioButton, "demi_bold_font_weight_radio_button");
    fontWeightLayout->addWidget(demiBoldFontWeightRadioButton);

    QRadioButton* boldFontWeightRadioButton = new QRadioButton(tr("Bold"));
    boldFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(boldFontWeightRadioButton, "bold_font_weight_radio_button");
    fontWeightLayout->addWidget(boldFontWeightRadioButton);

    QRadioButton* extraBoldFontWeightRadioButton = new QRadioButton(tr("Extra Bold"));
    extraBoldFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(extraBoldFontWeightRadioButton, "extra_bold_font_weight_radio_button");
    fontWeightLayout->addWidget(extraBoldFontWeightRadioButton);

    QRadioButton* blackFontWeightRadioButton = new QRadioButton(tr("Black"));
    blackFontWeightRadioButton->setAutoExclusive(false);
    registerWidget(blackFontWeightRadioButton, "black_font_weight_radio_button");
    fontWeightLayout->addWidget(blackFontWeightRadioButton);

    embellishmentsHorizontalLayout->addSpacing(embellishmentsColumnsSpacing);

    QVBoxLayout* embellishmentsVerticalLayout = newVBoxLayout("embellishments_vertical_layout");
    embellishmentsHorizontalLayout->addLayout(embellishmentsVerticalLayout);

    QCheckBox* italicsCheckBox = new QCheckBox(tr("Italics"));
    italicsCheckBox->setTristate(true);
    registerWidget(italicsCheckBox, "italics_check_box");
    embellishmentsVerticalLayout->addWidget(italicsCheckBox);

    QCheckBox* underlineCheckBox = new QCheckBox(tr("Underline"));
    underlineCheckBox->setTristate(true);
    registerWidget(underlineCheckBox, "underline_check_box");
    embellishmentsVerticalLayout->addWidget(underlineCheckBox);

    QCheckBox* overlineCheckBox = new QCheckBox(tr("Overline"));
    overlineCheckBox->setTristate(true);
    registerWidget(overlineCheckBox, "overline_check_box");
    embellishmentsVerticalLayout->addWidget(overlineCheckBox);

    QCheckBox* strikethroughCheckBox = new QCheckBox(tr("Strikethrough"));
    strikethroughCheckBox->setTristate(true);
    registerWidget(strikethroughCheckBox, "strikethrough_check_box");
    embellishmentsVerticalLayout->addWidget(strikethroughCheckBox);

    embellishmentsVerticalLayout->addStretch(1);

    QGroupBox* positionGroupBox = new QGroupBox(tr("Position"));
    registerWidget(positionGroupBox, "position_group_box");
    embellishmentsVerticalLayout->addWidget(positionGroupBox);

    QVBoxLayout* positionLayout = newVBoxLayout("position_layout");
    positionGroupBox->setLayout(positionLayout);

    QRadioButton* superscriptRadioButton = new QRadioButton(tr("Superscript"));
    superscriptRadioButton->setAutoExclusive(false);
    registerWidget(superscriptRadioButton, "superscript_radio_button");
    positionLayout->addWidget(superscriptRadioButton);

    QRadioButton* normalBaselineRadioButton = new QRadioButton(tr("Normal"));
    normalBaselineRadioButton->setAutoExclusive(false);
    registerWidget(normalBaselineRadioButton, "normal_baseline_radio_button");
    positionLayout->addWidget(normalBaselineRadioButton);

    QRadioButton* subscriptRadioButton = new QRadioButton(tr("Subscript"));
    subscriptRadioButton->setAutoExclusive(false);
    registerWidget(subscriptRadioButton, "subscript_radio_button");
    positionLayout->addWidget(subscriptRadioButton);

    embellishmentsHorizontalLayout->addSpacing(embellishmentsColumnsSpacing);

    QVBoxLayout* colorLayout = newVBoxLayout("color_layout");
    embellishmentsHorizontalLayout->addLayout(colorLayout);

    QGroupBox* fontColorGroupBox = new QGroupBox(tr("Font Color"));
    registerWidget(fontColorGroupBox, "font_color_group_box");
    colorLayout->addWidget(fontColorGroupBox);

    QHBoxLayout* fontColorLayout = newHBoxLayout("font_color_layout");
    fontColorGroupBox->setLayout(fontColorLayout);

    EQt::ColorToolButton* fontColorToolButton = new EQt::ColorToolButton(QColor(Qt::yellow), 1);
    registerWidget(fontColorToolButton, "change_font_color_tool_button");
    fontColorLayout->addWidget(fontColorToolButton);

    QPushButton* changeFontColorPushButton = new QPushButton(tr("Change"));
    registerWidget(changeFontColorPushButton, "change_font_color_push_button");
    fontColorLayout->addWidget(changeFontColorPushButton);

    QPushButton* removeFontColorPushButton = new QPushButton(tr("Remove"));
    registerWidget(removeFontColorPushButton, "remove_font_color_push_button");
    fontColorLayout->addWidget(removeFontColorPushButton);

    QGroupBox* fontBackgroundColorGroupBox = new QGroupBox(tr("Font Background Color / Highlight"));
    registerWidget(fontBackgroundColorGroupBox, "font_background_color_group_box");
    colorLayout->addWidget(fontBackgroundColorGroupBox);

    QHBoxLayout* fontBackgroundColorLayout = newHBoxLayout("font_background_color_layout");
    fontBackgroundColorGroupBox->setLayout(fontBackgroundColorLayout);

    EQt::ColorToolButton* fontBackgroundColorToolButton = new EQt::ColorToolButton(QColor(Qt::yellow), 1);
    registerWidget(fontBackgroundColorToolButton, "change_font_background_color_tool_button");
    fontBackgroundColorLayout->addWidget(fontBackgroundColorToolButton);

    QPushButton* changefontBackgroundColorPushButton = new QPushButton(tr("Change"));
    registerWidget(changefontBackgroundColorPushButton, "change_font_background_color_push_button");
    fontBackgroundColorLayout->addWidget(changefontBackgroundColorPushButton);

    QPushButton* removefontBackgroundColorPushButton = new QPushButton(tr("Remove"));
    registerWidget(removefontBackgroundColorPushButton, "remove_font_background_color_push_button");
    fontBackgroundColorLayout->addWidget(removefontBackgroundColorPushButton);

    colorLayout->addStretch(1);

    QGroupBox* letterSpacingGroupBox = new QGroupBox(tr("Letter Spacing"));
    registerWidget(letterSpacingGroupBox, "letter_spacing_group_box");
    colorLayout->addWidget(letterSpacingGroupBox);

    QHBoxLayout* letterSpacingLayout = newHBoxLayout("letter_spacing_layout");
    letterSpacingGroupBox->setLayout(letterSpacingLayout);

    QDoubleSpinBox* letterSpacingSpinBox = new QDoubleSpinBox;
    letterSpacingSpinBox->setRange(
        Ld::FontFormat::minimumLetterSpacing * 100.0F,
        Ld::FontFormat::maximumLetterSpacing * 100.0F
    );
    letterSpacingSpinBox->setDecimals(1);
    letterSpacingSpinBox->setSingleStep(0.1F);
    letterSpacingSpinBox->setSuffix(tr("%"));
    registerWidget(letterSpacingSpinBox, "letter_spacing_spin_box");
    letterSpacingLayout->addWidget(letterSpacingSpinBox);

    currentDemoTextMode = DemoTextMode::NORMAL;

    #if (defined(Q_OS_DARWIN))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    #elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
        );

    #else

        #error Unknown platform

    #endif

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
        registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    mainLayout->addWidget(buttonBox);
}


bool CharacterFormatDialog::valuesChanged() const {
    return (
           basicSettingsChanged()
        || embellishmentsChanged()
        || baselineChanged()
        || letterSpacingChanged()
        || colorSettingsChanged()
    );
}


bool CharacterFormatDialog::basicSettingsChanged() const {
    bool changed = true;

    QString family = fontFamily();
    if (family.isEmpty() || (inputFontFamilies.count() == 1 && *inputFontFamilies.begin() == family)) {
        unsigned size = fontSize();
        if (size < Ld::FontFormat::minimumFontSize                           ||
            (inputFontSizes.count() == 1 && *inputFontSizes.begin() == size)    ) {
            Ld::FontFormat::Weight weight = fontWeight();
            if (weight == Ld::FontFormat::Weight::INVALID ||
                (inputFontWeights.count() == 1 && *inputFontWeights.begin() == weight)    ) {
                changed = false;
            }
        }
    }

    return changed;
}


bool CharacterFormatDialog::embellishmentsChanged() const {
    bool changed = true;

    if (italicsBothEnabledAndDisabled() || ((inputItalics == QuadState::ENABLED) == italicsEnabled())) {
        if (underlineBothEnabledAndDisabled() || ((inputUnderline == QuadState::ENABLED) == underlineEnabled())) {
            if (overlineBothEnabledAndDisabled() || ((inputOverline == QuadState::ENABLED) == overlineEnabled())) {
                if (strikethroughBothEnabledAndDisabled() ||
                    ((inputStrikethrough == QuadState::ENABLED) == strikethroughEnabled())) {
                    changed = false;
                }
            }
        }
    }

    return changed;
}


bool CharacterFormatDialog::baselineChanged() const {
    bool changed;

    if (positionControlsDisabled()) {
        changed = false;
    } else {
        Ld::CharacterFormat::Position position = CharacterFormatDialog::position();
        if (position == Ld::CharacterFormat::Position::INVALID ||
            (inputPositions.count() == 1 && *inputPositions.begin() == position)) {
            changed = false;
        } else {
            changed = true;
        }
    }

    return changed;
}


bool CharacterFormatDialog::letterSpacingChanged() const {
    bool changed;

    if (letterSpacingInvalid()                                                                ||
        (inputLetterSpacings.count() == 1 && *inputLetterSpacings.begin() == letterSpacing())    ) {
        changed = false;
    } else {
        changed = true;
    }

    return changed;
}


bool CharacterFormatDialog::colorSettingsChanged() const {
    return fontColorSelected() || fontBackgroundColorSelected();
}


void CharacterFormatDialog::updateWeightRadioButtons(Ld::FontFormat::Weight newFontWeight) {
    QRadioButton* thinFontWeightRadioButton       = widget<QRadioButton>("thin_font_weight_radio_button");
    QRadioButton* extraLightFontWeightRadioButton = widget<QRadioButton>("extra_light_font_weight_radio_button");
    QRadioButton* lightFontWeightRadioButton      = widget<QRadioButton>("light_font_weight_radio_button");
    QRadioButton* normalFontWeightRadioButton     = widget<QRadioButton>("normal_font_weight_radio_button");
    QRadioButton* mediumFontWeightRadioButton     = widget<QRadioButton>("medium_font_weight_radio_button");
    QRadioButton* demiBoldFontWeightRadioButton   = widget<QRadioButton>("demi_bold_font_weight_radio_button");
    QRadioButton* boldFontWeightRadioButton       = widget<QRadioButton>("bold_font_weight_radio_button");
    QRadioButton* extraBoldFontWeightRadioButton  = widget<QRadioButton>("extra_bold_font_weight_radio_button");
    QRadioButton* blackFontWeightRadioButton      = widget<QRadioButton>("black_font_weight_radio_button");

    thinFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::THIN);
    extraLightFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::EXTRA_LIGHT);
    lightFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::LIGHT);
    normalFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::NORMAL);
    mediumFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::MEDIUM);
    demiBoldFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::DEMI_BOLD);
    boldFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::BOLD);
    extraBoldFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::EXTRA_BOLD);
    blackFontWeightRadioButton->setChecked(newFontWeight == Ld::FontFormat::Weight::BLACK);

    updateDialog();
}


void CharacterFormatDialog::updatePositionRadioButtons(Ld::CharacterFormat::Position newPosition) {
    QRadioButton* superscriptRadioButton    = widget<QRadioButton>("superscript_radio_button");
    QRadioButton* normalBaselineRadioButton = widget<QRadioButton>("normal_baseline_radio_button");
    QRadioButton* subscriptRadioButton      = widget<QRadioButton>("subscript_radio_button");

    superscriptRadioButton->setChecked(newPosition == Ld::CharacterFormat::Position::SUPERSCRIPT);
    normalBaselineRadioButton->setChecked(newPosition == Ld::CharacterFormat::Position::NORMAL);
    subscriptRadioButton->setChecked(newPosition == Ld::CharacterFormat::Position::SUBSCRIPT);

    updateDialog();
}


void CharacterFormatDialog::updateSampleText() {
    QGroupBox*           sampleTextGroupBox   = widget<QGroupBox>("sample_text_group_box");
    EQt::FontDemoWidget* sampleTextDemoWidget = widget<EQt::FontDemoWidget>("sample_text_demo_widget");

    QString                family          = fontFamily();
    unsigned               size            = fontSize();
    Ld::FontFormat::Weight weight          = fontWeight();

    if (!family.isEmpty()                                          &&
        size > 0                                                   &&
        weight != Ld::FontFormat::Weight::INVALID                  &&
        !italicsBothEnabledAndDisabled()                           &&
        !underlineBothEnabledAndDisabled()                         &&
        !overlineBothEnabledAndDisabled()                          &&
        !strikethroughBothEnabledAndDisabled()                     &&
        (positionControlsDisabled()                           ||
         position() != Ld::CharacterFormat::Position::INVALID    )    ) {
        sampleTextGroupBox->setVisible(true);

        QColor color           = fontColor();
        QColor backgroundColor = fontBackgroundColor();

        Ld::CharacterFormat::Position position =   positionControlsDisabled()
                                                 ? Ld::CharacterFormat::Position::NORMAL :
                                                   CharacterFormatDialog::position();

        bool  italics       = italicsEnabled();
        bool  underline     = underlineEnabled();
        bool  overline      = overlineEnabled();
        bool  strikethrough = strikethroughEnabled();
        float letterSpacing = CharacterFormatDialog::letterSpacing();

        QFont sampleTextFont(family, size, static_cast<QFont::Weight>(weight), italics);
        sampleTextFont.setUnderline(underline);
        sampleTextFont.setOverline(overline);
        sampleTextFont.setStrikeOut(strikethrough);
        sampleTextFont.setLetterSpacing(QFont::PercentageSpacing, 100.0F * (1.0F + letterSpacing));

        QFont fillerFont(tr("Helvetica"), fillerTextPointSize, QFont::Weight::Normal, true);

        sampleTextDemoWidget->setSampleTextFont(sampleTextFont);
        sampleTextDemoWidget->setFillerFont(fillerFont);

        sampleTextDemoWidget->setSampleTextFontColor(color);
        sampleTextDemoWidget->setSampleTextFontBackgroundColor(backgroundColor);

        float relativeSize      = 1.0F;
        float relativePosition  = 0.0F;
        float relativeWeight    = 0.0F;
        bool  enableMarkerLines = true; // false;

        QString sampleHeadText;
        QString sampleTailText;

        if (currentDemoTextMode == DemoTextMode::NORMAL) {
            if (position == Ld::CharacterFormat::Position::NORMAL) {
                sampleHeadText = tr("Cwm fjord bank glyphs vext quiz.  0123456789");
                sampleTailText.clear();
            } else if (position == Ld::CharacterFormat::Position::SUBSCRIPT) {
                relativeSize      = TextPresentation::subscriptSizeAdjustment;
                relativePosition  = TextPresentation::subscriptBaseline;
                relativeWeight    = TextPresentation::subscriptWeightAdjustment;

                enableMarkerLines = true;

                sampleHeadText = tr("H");
                sampleTailText = tr("0 1234567890  Cwm fjord bank glyphs vext quiz.");
            } else if (position == Ld::CharacterFormat::Position::SUPERSCRIPT) {
                relativeSize      = TextPresentation::superscriptSizeAdjustment;
                relativePosition  = TextPresentation::superscriptBaseline;
                relativeWeight    = TextPresentation::superscriptWeightAdjustment;

                enableMarkerLines = true;

                sampleHeadText = tr("A");
                sampleTailText = tr("-1 234567890  Cwm fjord bank glyphs vext quiz.");
            } else {
                Q_ASSERT(false);
            }
        } else {
            Q_ASSERT(currentDemoTextMode == DemoTextMode::LIST);

            QFontMetrics fontMetrics(sampleTextFont);
            QList<Ld::UnorderedListParagraphFormat::StandardBullet> bullets =
                Ld::UnorderedListParagraphFormat::standardBullets();

            bool isFirst = true;
            for (  QList<Ld::UnorderedListParagraphFormat::StandardBullet>::const_iterator it  = bullets.constBegin(),
                                                                                           end = bullets.constEnd()
                 ; it != end
                 ; ++it
                ) {
                Ld::UnorderedListParagraphFormat::StandardBullet::UnicodeValue unicode = it->unicodeValue();
                if (fontMetrics.inFont(QChar(unicode))) {
                    if (isFirst) {
                        sampleHeadText += QChar(unicode);
                        isFirst = false;
                    } else {
                        sampleHeadText += tr(" %1").arg(QChar(unicode));
                    }
                } else {
                    if (isFirst) {
                        sampleHeadText += tr(" ");
                        isFirst = false;
                    } else {
                        sampleHeadText += tr("  ");
                    }
                }
            }

            sampleHeadText = tr("%1    1  A  a  IV  iv").arg(sampleHeadText);
        }

        sampleTextDemoWidget->setRelativePosition(relativePosition);
        sampleTextDemoWidget->setRelativeSize(relativeSize);
        sampleTextDemoWidget->setRelativeWeight(relativeWeight);
        sampleTextDemoWidget->setSampleHeadText(sampleHeadText);
        sampleTextDemoWidget->setSampleTailText(sampleTailText);
        sampleTextDemoWidget->setMarkersEnabled(enableMarkerLines);
    } else {
        sampleTextGroupBox->setVisible(false);
    }
}


void CharacterFormatDialog::updateWeights() {
    QString  fontFamily = CharacterFormatDialog::fontFamily();
    unsigned fontSize   = CharacterFormatDialog::fontSize();

    QSet<Ld::FontFormat::Weight> supportedWeights = inputSupportedFontWeights;
    if (!fontFamily.isEmpty() && fontSize >= Ld::FontFormat::minimumFontSize) {
        bool italics = italicsEnabled();
        QFont font(fontFamily, fontSize);
        font.setItalic(italics);

        supportedWeights = toFontFormatWeights(EQt::FontData::uniqueWeights(font));
    }

    Q_ASSERT(!supportedWeights.isEmpty());

    QRadioButton* thinFontWeightRadioButton       = widget<QRadioButton>("thin_font_weight_radio_button");
    QRadioButton* extraLightFontWeightRadioButton = widget<QRadioButton>("extra_light_font_weight_radio_button");
    QRadioButton* lightFontWeightRadioButton      = widget<QRadioButton>("light_font_weight_radio_button");
    QRadioButton* normalFontWeightRadioButton     = widget<QRadioButton>("normal_font_weight_radio_button");
    QRadioButton* mediumFontWeightRadioButton     = widget<QRadioButton>("medium_font_weight_radio_button");
    QRadioButton* demiBoldFontWeightRadioButton   = widget<QRadioButton>("demi_bold_font_weight_radio_button");
    QRadioButton* boldFontWeightRadioButton       = widget<QRadioButton>("bold_font_weight_radio_button");
    QRadioButton* extraBoldFontWeightRadioButton  = widget<QRadioButton>("extra_bold_font_weight_radio_button");
    QRadioButton* blackFontWeightRadioButton      = widget<QRadioButton>("black_font_weight_radio_button");

    QMap<Ld::FontFormat::Weight, QRadioButton*> buttonMap;
    buttonMap.insert(Ld::FontFormat::Weight::THIN, thinFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::EXTRA_LIGHT, extraLightFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::LIGHT, lightFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::NORMAL, normalFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::MEDIUM, mediumFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::DEMI_BOLD, demiBoldFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::BOLD, boldFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::EXTRA_BOLD, extraBoldFontWeightRadioButton);
    buttonMap.insert(Ld::FontFormat::Weight::BLACK, blackFontWeightRadioButton);

    QMap<Ld::FontFormat::Weight, QRadioButton*>::const_iterator buttonBeginIterator = buttonMap.constBegin();
    QMap<Ld::FontFormat::Weight, QRadioButton*>::const_iterator buttonEndIterator   = buttonMap.constEnd();
    QMap<Ld::FontFormat::Weight, QRadioButton*>::const_iterator buttonIterator      = buttonBeginIterator;
    while (buttonIterator != buttonEndIterator) {
        Ld::FontFormat::Weight weight      = buttonIterator.key();
        QRadioButton*          radioButton = buttonIterator.value();

        if (supportedWeights.contains(weight)) {
            radioButton->setEnabled(true);
        } else {
            if (radioButton->isChecked()) {
                QMap<Ld::FontFormat::Weight, QRadioButton*>::const_iterator backwardsIterator = buttonIterator;
                QMap<Ld::FontFormat::Weight, QRadioButton*>::const_iterator forwardsIterator  = buttonIterator;

                bool foundBackwards = false;
                bool foundForwards  = false;
                do {
                    if (backwardsIterator != buttonBeginIterator) {
                        --backwardsIterator;

                        foundBackwards = supportedWeights.contains(backwardsIterator.key());
                    }

                    if (forwardsIterator != buttonEndIterator) {
                        ++forwardsIterator;
                        if (forwardsIterator != buttonEndIterator) {
                            foundForwards = supportedWeights.contains(forwardsIterator.key());
                        }
                    }
                } while (!foundBackwards && !foundForwards);

                if (foundBackwards) {
                    backwardsIterator.value()->setChecked(true);
                } else if (foundForwards) {
                    forwardsIterator.value()->setChecked(true);
                }

                radioButton->setChecked(false);
            }

            radioButton->setEnabled(false);
        }

        ++buttonIterator;
    }
}


void CharacterFormatDialog::updateColorButtonStates() {
    EQt::ColorToolButton* fontColorToolButton       = widget<EQt::ColorToolButton>("change_font_color_tool_button");
    QPushButton*          removeFontColorPushButton = widget<QPushButton>("remove_font_color_push_button");
    QSet<QColor>          fontColors                = fontColorToolButton->colors();

    if (fontColors.count() == 0) {
        removeFontColorPushButton->setEnabled(false);
    } else if (fontColors.count() == 1 && !fontColors.begin()->isValid()) {
        removeFontColorPushButton->setEnabled(false);

        #if (defined(Q_OS_DARWIN))
            // On MacOS, both Qt-5.10 and Qt-5.11.1 appear to have an issue where enabling/disabling a button does
            // not cause the controls to be reliably repainted until or unless the entire window needs to be repainted.
            //
            // To work around this, we explicitly repaint the button. on MacOS.
            //
            // Note similar code below to address this issue in multiple locations.

            removeFontColorPushButton->repaint();

        #endif
    } else {
        removeFontColorPushButton->setEnabled(true);

        #if (defined(Q_OS_DARWIN))

            removeFontColorPushButton->repaint();

        #endif
    }

    EQt::ColorToolButton* fontBackgroundColorToolButton       = widget<EQt::ColorToolButton>(
        "change_font_background_color_tool_button"
    );
    QPushButton*          removeFontBackgroundColorPushButton = widget<QPushButton>(
        "remove_font_background_color_push_button"
    );
    QSet<QColor>          fontBackgroundColors                = fontBackgroundColorToolButton->colors();

    if (fontBackgroundColors.count() == 0) {
        removeFontBackgroundColorPushButton->setEnabled(false);
    } else if (fontBackgroundColors.count() == 1 && !fontBackgroundColors.begin()->isValid()) {
        removeFontBackgroundColorPushButton->setEnabled(false);

        #if (defined(Q_OS_DARWIN))

            removeFontBackgroundColorPushButton->repaint();

        #endif
    } else {
        removeFontBackgroundColorPushButton->setEnabled(true);

        #if (defined(Q_OS_DARWIN))

            removeFontBackgroundColorPushButton->repaint();

        #endif
    }
}


void CharacterFormatDialog::updateCheckbox(QCheckBox* checkBox, CharacterFormatDialog::QuadState quadState) {
    switch (quadState) {
        case QuadState::ENABLED: {
            checkBox->setTristate(false);
            checkBox->setCheckState(Qt::Checked);
            break;
        }

        case QuadState::DISABLED: {
            checkBox->setTristate(false);
            checkBox->setCheckState(Qt::Unchecked);
            break;
        }

        case QuadState::NO_VALUE:
        case QuadState::ENABLED_AND_DISABLED: {
            checkBox->setTristate(true);
            checkBox->setCheckState(Qt::PartiallyChecked);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}
