/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref NumericValueFormatDialog class.
***********************************************************************************************************************/

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QMargins>
#include <QWidget>
#include <QLocale>
#include <QDialog>
#include <QSet>
#include <QSharedPointer>
#include <QLocale>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QLabel>
#include <QTabWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QFontMetrics>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>

#include <util_string.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <ios>
#include <iomanip>

#include <eqt_special_value_spin_box.h>
#include <eqt_tool_button.h>
#include <eqt_line_edit.h>
#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <model_intrinsic_types.h>
#include <model_complex.h>

#include <ld_image_format.h>
#include <ld_character_format.h>
#include <ld_variable_name.h>
#include <ld_numeric_data_type_format.h>
#include <ld_integer_data_type_format.h>
#include <ld_real_data_type_format.h>
#include <ld_complex_data_type_format.h>
#include <ld_list_data_type_format.h>
#include <ld_tuple_data_type_format.h>
#include <ld_special_characters.h>

#include "application.h"
#include "presentation.h"
#include "variable_name_editor_widget.h"
#include "numeric_value_format_dialog.h"

const unsigned      NumericValueFormatDialog::invalidBase                = 0;
const unsigned      NumericValueFormatDialog::ignoreMantissaDigits       = Ld::IntegerDataTypeFormat::ignoreWidth;
const unsigned      NumericValueFormatDialog::mantissaDigitsNotSet       = static_cast<unsigned>(-2);
const unsigned      NumericValueFormatDialog::inferPrecision             = Ld::RealDataTypeFormat::inferPrecision;
const unsigned      NumericValueFormatDialog::precisionNotSet            = static_cast<unsigned>(-2);
const double        NumericValueFormatDialog::multiplicationFactorNotSet = double(0);
const unsigned long NumericValueFormatDialog::showAllContents            = Ld::ListDataTypeFormat::showAllMembers;
const unsigned long NumericValueFormatDialog::invalidMemberCount         = Ld::ListDataTypeFormat::showAllMembers - 1;

NumericValueFormatDialog::NumericValueFormatDialog(
        QWidget* parent
    ):ProgrammaticDialog(
        "NumericValueFormatDialog",
        parent
    ) {
    configureWidget();
    runBuilders();
}


NumericValueFormatDialog::~NumericValueFormatDialog() {}


void NumericValueFormatDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void NumericValueFormatDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void NumericValueFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void NumericValueFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool NumericValueFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool NumericValueFormatDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


void NumericValueFormatDialog::setApplyButtonVisible(bool nowVisible) {
    #if (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setVisible(nowVisible);

    #elif (defined(Q_OS_DARWIN))

        (void) nowVisible;

    #else

        #error Unknown platform

    #endif
}


void NumericValueFormatDialog::setApplyButtonHidden(bool nowHidden) {
    setApplyButtonVisible(!nowHidden);
}


bool NumericValueFormatDialog::applyButtonVisible() const {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    return applyPushButton->isVisible();
}


bool NumericValueFormatDialog::applyButtonHidden() const {
    return !applyButtonVisible();
}


bool NumericValueFormatDialog::variableNameFieldsEnabled() const {
    QGroupBox* variableNameGroupBox = widget<QGroupBox>("variable_name_group_box");
    return variableNameGroupBox->isVisible();
}


bool NumericValueFormatDialog::variableNameFieldsDisbled() const {
    return !variableNameFieldsEnabled();
}


QSet<NumericValueFormatDialog::ValueType> NumericValueFormatDialog::supportedDataTypes() const {
    return currentSupportedDataTypes;
}


Ld::VariableName NumericValueFormatDialog::variableName() const {
    VariableNameEditorWidget* nameEditorWidget = widget<VariableNameEditorWidget>("variable_name_editor_widget");
    return nameEditorWidget->variableName();
}


NumericValueFormatDialog::BooleanStyle NumericValueFormatDialog::booleanStyle() const {
    BooleanStyle result;

    QRadioButton* lowerCaseTrueFalseRadioButton = widget<QRadioButton>("lower_case_true_false_radio_button");
    QRadioButton* titleCaseTrueFalseRadioButton = widget<QRadioButton>("title_case_true_false_radio_button");
    QRadioButton* upperCaseTrueFalseRadioButton = widget<QRadioButton>("upper_case_true_false_radio_button");
    QRadioButton* lowerCaseYesNoRadioButton     = widget<QRadioButton>("lower_case_yes_no_radio_button");
    QRadioButton* titleCaseYesNoRadioButton     = widget<QRadioButton>("title_case_yes_no_radio_button");
    QRadioButton* upperCaseYesNoRadioButton     = widget<QRadioButton>("upper_case_yes_no_radio_button");
    QRadioButton* numeric10RadioButton          = widget<QRadioButton>("numeric_1_0_radio_button");

    bool lowerCaseTrueFalseChecked = lowerCaseTrueFalseRadioButton->isChecked();
    bool titleCaseTrueFalseChecked = titleCaseTrueFalseRadioButton->isChecked();
    bool upperCaseTrueFalseChecked = upperCaseTrueFalseRadioButton->isChecked();
    bool lowerCaseYesNoChecked     = lowerCaseYesNoRadioButton->isChecked();
    bool titleCaseYesNoChecked     = titleCaseYesNoRadioButton->isChecked();
    bool upperCaseYesNoChecked     = upperCaseYesNoRadioButton->isChecked();
    bool numeric10Checked          = numeric10RadioButton->isChecked();

    if        ( lowerCaseTrueFalseChecked && !titleCaseTrueFalseChecked && !upperCaseTrueFalseChecked &&
               !lowerCaseYesNoChecked     && !titleCaseYesNoChecked     && !upperCaseYesNoChecked     &&
               !numeric10Checked                                                                         ) {
        result = BooleanStyle::LOWER_CASE_TRUE_FALSE;
    } else if (!lowerCaseTrueFalseChecked &&  titleCaseTrueFalseChecked && !upperCaseTrueFalseChecked &&
               !lowerCaseYesNoChecked     && !titleCaseYesNoChecked     && !upperCaseYesNoChecked     &&
               !numeric10Checked                                                                         ) {
        result = BooleanStyle::TITLE_CASE_TRUE_FALSE;
    } else if (!lowerCaseTrueFalseChecked && !titleCaseTrueFalseChecked &&  upperCaseTrueFalseChecked &&
               !lowerCaseYesNoChecked     && !titleCaseYesNoChecked     && !upperCaseYesNoChecked     &&
               !numeric10Checked                                                                         ) {
        result = BooleanStyle::UPPER_CASE_TRUE_FALSE;
    } else if (!lowerCaseTrueFalseChecked && !titleCaseTrueFalseChecked && !upperCaseTrueFalseChecked &&
                lowerCaseYesNoChecked     && !titleCaseYesNoChecked     && !upperCaseYesNoChecked     &&
               !numeric10Checked                                                                         ) {
        result = BooleanStyle::LOWER_CASE_YES_NO;
    } else if (!lowerCaseTrueFalseChecked && !titleCaseTrueFalseChecked && !upperCaseTrueFalseChecked &&
               !lowerCaseYesNoChecked     &&  titleCaseYesNoChecked     && !upperCaseYesNoChecked     &&
               !numeric10Checked                                                                         ) {
        result = BooleanStyle::TITLE_CASE_YES_NO;
    } else if (!lowerCaseTrueFalseChecked && !titleCaseTrueFalseChecked && !upperCaseTrueFalseChecked &&
               !lowerCaseYesNoChecked     && !titleCaseYesNoChecked     &&  upperCaseYesNoChecked     &&
               !numeric10Checked                                                                         ) {
        result = BooleanStyle::UPPER_CASE_YES_NO;
    } else if (!lowerCaseTrueFalseChecked && !titleCaseTrueFalseChecked && !upperCaseTrueFalseChecked &&
               !lowerCaseYesNoChecked     && !titleCaseYesNoChecked     && !upperCaseYesNoChecked     &&
                numeric10Checked                                                                         ) {
        result = BooleanStyle::NUMERIC_1_0;
    } else {
        result = BooleanStyle::INVALID;
    }

    return result;
}


unsigned NumericValueFormatDialog::mantissaDigits() const {
    unsigned                  result;
    EQt::SpecialValueSpinBox* widthSpinBox = widget<EQt::SpecialValueSpinBox>("integer_width_spin_box");
    int                       value        = widthSpinBox->currentValue();

    if (value == -1) {
        result = mantissaDigitsNotSet;
    } else if (value == 0) {
        result = ignoreMantissaDigits;
    } else {
        result = static_cast<unsigned>(value);
    }

    return result;
}


bool NumericValueFormatDialog::upperCase() const {
    QCheckBox* upperCaseCheckBox = widget<QCheckBox>("integer_upper_case_check_box");
    Qt::CheckState checkState = upperCaseCheckBox->checkState();

    return checkState == Qt::Checked;
}


bool NumericValueFormatDialog::lowerCase() const {
    QCheckBox* upperCaseCheckBox = widget<QCheckBox>("integer_upper_case_check_box");
    Qt::CheckState checkState = upperCaseCheckBox->checkState();

    return checkState == Qt::Unchecked;
}


bool NumericValueFormatDialog::caseIsSet() const {
    QCheckBox* upperCaseCheckBox = widget<QCheckBox>("integer_upper_case_check_box");
    Qt::CheckState checkState = upperCaseCheckBox->checkState();

    return checkState != Qt::PartiallyChecked;
}


NumericValueFormatDialog::IntegerNumberStyle NumericValueFormatDialog::integerNumberStyle() const {
    IntegerNumberStyle result;

    QRadioButton* noPrefixSuffixStyleRadioButton = widget<QRadioButton>("integer_no_prefix_suffix_style_radio_button");
    QRadioButton* cStyleRadioButton              = widget<QRadioButton>("integer_c_style_radio_button");
    QRadioButton* verilogStyleRadioButton        = widget<QRadioButton>("integer_verilog_style_radio_button");
    QRadioButton* subscriptStyleRadioButton      = widget<QRadioButton>("integer_subscript_style_radio_button");

    bool noPrefixSuffixStyle = noPrefixSuffixStyleRadioButton->isChecked();
    bool cStyle              = cStyleRadioButton->isChecked();
    bool verilogStyle        = verilogStyleRadioButton->isChecked();
    bool subscriptStyle      = subscriptStyleRadioButton->isChecked();

    if        ( noPrefixSuffixStyle && !cStyle && !verilogStyle && !subscriptStyle) {
        result = IntegerNumberStyle::NONE;
    } else if (!noPrefixSuffixStyle &&  cStyle && !verilogStyle && !subscriptStyle) {
        result = IntegerNumberStyle::C_STYLE;
    } else if (!noPrefixSuffixStyle && !cStyle &&  verilogStyle && !subscriptStyle) {
        result = IntegerNumberStyle::VERILOG_STYLE;
    } else if (!noPrefixSuffixStyle && !cStyle && !verilogStyle &&  subscriptStyle) {
        result = IntegerNumberStyle::SUBSCRIPT_STYLE;
    } else {
        result = IntegerNumberStyle::INVALID;
    }

    return result;
}


unsigned NumericValueFormatDialog::base() const {
    unsigned result;

    QRadioButton* integerBase2RadioButton  = widget<QRadioButton>("integer_base_2_radio_button");
    QRadioButton* integerBase10RadioButton = widget<QRadioButton>("integer_base_10_radio_button");
    QRadioButton* integerBase16RadioButton = widget<QRadioButton>("integer_base_16_radio_button");

    bool base2  = integerBase2RadioButton->isChecked();
    bool base10 = integerBase10RadioButton->isChecked();
    bool base16 = integerBase16RadioButton->isChecked();

    if        ( base2 && !base10 && !base16) {
        result = 2;
    } else if (!base2 &&  base10 && !base16) {
        result = 10;
    } else if (!base2 && !base10 &&  base16) {
        result = 16;
    } else {
        result = invalidBase;
    }

    return result;
}


NumericValueFormatDialog::RealNumberStyle NumericValueFormatDialog::realNumberStyle() const {
    RealNumberStyle result;

    QRadioButton* fixedPointStyleRadioButton         = widget<QRadioButton>("real_fixed_point_style_radio_button");
    QRadioButton* scientificStyleRadioButton         = widget<QRadioButton>("real_scientific_style_radio_button");
    QRadioButton* engineeringStyleRadioButton        = widget<QRadioButton>("real_engineering_style_radio_button");
    QRadioButton* computerScientificStyleRadioButton = widget<QRadioButton>(
        "real_computer_scientific_style_radio_button"
    );
    QRadioButton* cnciseStyleRadioButton             = widget<QRadioButton>("real_concise_style_radio_button");
    QRadioButton* conciseComputerStyleRadioButton    = widget<QRadioButton>("real_concise_computer_style_radio_button");

    bool fixedPoint         = fixedPointStyleRadioButton->isChecked();
    bool scientific         = scientificStyleRadioButton->isChecked();
    bool engineering        = engineeringStyleRadioButton->isChecked();
    bool computerScientific = computerScientificStyleRadioButton->isChecked();
    bool concise            = cnciseStyleRadioButton->isChecked();
    bool conciseComputer    = conciseComputerStyleRadioButton->isChecked();

    if        ( fixedPoint && !scientific && !engineering && !computerScientific && !concise && !conciseComputer) {
        result = RealNumberStyle::FLOATING_POINT;
    } else if (!fixedPoint &&  scientific && !engineering && !computerScientific && !concise && !conciseComputer) {
        result = RealNumberStyle::SCIENTIFIC;
    } else if (!fixedPoint && !scientific &&  engineering && !computerScientific && !concise && !conciseComputer) {
        result = RealNumberStyle::ENGINEERING;
    } else if (!fixedPoint && !scientific && !engineering &&  computerScientific && !concise && !conciseComputer) {
        result = RealNumberStyle::COMPUTER_SCIENTIFIC;
    } else if (!fixedPoint && !scientific && !engineering && !computerScientific &&  concise && !conciseComputer) {
        result = RealNumberStyle::CONCISE;
    } else if (!fixedPoint && !scientific && !engineering && !computerScientific && !concise &&  conciseComputer) {
        result = RealNumberStyle::COMPUTER_CONCISE;
    } else {
        result = RealNumberStyle::INVALID;
    }

    return result;
}


unsigned NumericValueFormatDialog::precision() const {
    unsigned                  result;
    EQt::SpecialValueSpinBox* precisionSpinBox = widget<EQt::SpecialValueSpinBox>("precision_spin_box");
    int                       value            = precisionSpinBox->currentValue();

    if (value == -1) {
        result = precisionNotSet;
    } else if (value == 0) {
        result = inferPrecision;
    } else {
        result = static_cast<unsigned>(value);
    }

    return result;
}


double NumericValueFormatDialog::multiplicationFactor() const {
    EQt::LineEdit* multiplierLineEdit = widget<EQt::LineEdit>("multiplier_line_edit");
    QString        valueString        = multiplierLineEdit->text();
    bool           ok;
    double         value              = valueString.toDouble(&ok);

    if (!ok) {
        value = multiplicationFactorNotSet;
    }

    return value;
}


NumericValueFormatDialog::ImaginaryUnit NumericValueFormatDialog::imaginaryUnit() const {
    ImaginaryUnit result;

    QRadioButton* imaginaryUnitIRadioButton = widget<QRadioButton>("complex_imaginary_unit_i_radio_button");
    QRadioButton* imaginaryUnitJRadioButton = widget<QRadioButton>("complex_imaginary_unit_j_radio_button");

    bool i = imaginaryUnitIRadioButton->isChecked();
    bool j = imaginaryUnitJRadioButton->isChecked();

    if        ( i && !j) {
        result = ImaginaryUnit::I;
    } else if (!i &&  j) {
        result = ImaginaryUnit::J;
    } else {
        result = ImaginaryUnit::INVALID;
    }

    return result;
}


NumericValueFormatDialog::TupleDisplayMode NumericValueFormatDialog::tupleDisplayMode() const {
    QComboBox*       comboBox    = widget<QComboBox>("tuple_display_mode_combo_box");
    TupleDisplayMode displayMode = static_cast<TupleDisplayMode>(comboBox->currentData().toUInt());

    return displayMode;
}


unsigned long NumericValueFormatDialog::leadingMemberCount() const {
    unsigned long result;

    QSpinBox* leadingMemberCountSpinBox = widget<QSpinBox>("leading_member_count_spin_box");
    if (leadingMemberCountSpinBox->text().isEmpty()) {
        result = invalidMemberCount;
    } else {
        int currentValue = leadingMemberCountSpinBox->value();
        if (currentValue < 0) {
            result = showAllContents;
        } else {
            result = static_cast<unsigned long>(currentValue);
        }
    }

    return result;
}


unsigned long NumericValueFormatDialog::trailingMemberCount() const {
    unsigned long result;

    if (leadingMemberCount() != showAllContents) {
        QSpinBox* trailingMemberCountSpinBox = widget<QSpinBox>("trailing_member_count_spin_box");
        if (trailingMemberCountSpinBox->text().isEmpty()) {
            result = invalidMemberCount;
        } else {
            int currentValue = trailingMemberCountSpinBox->value();
            if (currentValue < 0) {
                result = showAllContents;
            } else {
                result = static_cast<unsigned long>(currentValue);
            }
        }
    } else {
        result = showAllContents;
    }

    return result;
}


bool NumericValueFormatDialog::showHiddenMemberCount() const {
    bool result;

    QCheckBox* showHiddenMemberCountCheckbox = widget<QCheckBox>("show_hidden_member_count_checkbox");
    if (showHiddenMemberCountCheckbox->checkState() == Qt::PartiallyChecked) {
        result = false;
    } else {
        result = showHiddenMemberCountCheckbox->isChecked();
    }

    return result;
}


bool NumericValueFormatDialog::hideHiddenMemberCount() const {
    bool result;

    QCheckBox* showHiddenMemberCountCheckbox = widget<QCheckBox>("show_hidden_member_count_checkbox");
    if (showHiddenMemberCountCheckbox->checkState() == Qt::PartiallyChecked) {
        result = false;
    } else {
        result = !showHiddenMemberCountCheckbox->isChecked();
    }

    return result;
}


bool NumericValueFormatDialog::showHideHiddenMemberCountIsSet() const {
    QCheckBox* showHiddenMemberCountCheckbox = widget<QCheckBox>("show_hidden_member_count_checkbox");
    return     (showHiddenMemberCountCheckbox->checkState() != Qt::PartiallyChecked);
}


unsigned long NumericValueFormatDialog::leadingRowsCount() const {
    unsigned long result;

    QSpinBox* leadingRowsSpinBox = widget<QSpinBox>("leading_rows_spin_box");
    if (leadingRowsSpinBox->text().isEmpty()) {
        result = invalidMemberCount;
    } else {
        int currentValue = leadingRowsSpinBox->value();
        if (currentValue < 0) {
            result = showAllContents;
        } else {
            result = static_cast<unsigned long>(currentValue);
        }
    }

    return result;
}


unsigned long NumericValueFormatDialog::trailingRowsCount() const {
    unsigned long result;

    QSpinBox* trailingRowsSpinBox = widget<QSpinBox>("trailing_rows_spin_box");
    if (trailingRowsSpinBox->text().isEmpty()) {
        result = invalidMemberCount;
    } else {
        int currentValue = trailingRowsSpinBox->value();
        if (currentValue < 0) {
            result = showAllContents;
        } else {
            result = static_cast<unsigned long>(currentValue);
        }
    }

    return result;
}


unsigned long NumericValueFormatDialog::leadingColumnsCount() const {
    unsigned long result;

    QSpinBox* leadingColumnsSpinBox = widget<QSpinBox>("leading_columns_spin_box");
    if (leadingColumnsSpinBox->text().isEmpty()) {
        result = invalidMemberCount;
    } else {
        int currentValue = leadingColumnsSpinBox->value();
        if (currentValue < 0) {
            result = showAllContents;
        } else {
            result = static_cast<unsigned long>(currentValue);
        }
    }

    return result;
}


unsigned long NumericValueFormatDialog::trailingColumnsCount() const {
    unsigned long result;

    QSpinBox* trailingColumnsSpinBox = widget<QSpinBox>("trailing_columns_spin_box");
    if (trailingColumnsSpinBox->text().isEmpty()) {
        result = invalidMemberCount;
    } else {
        int currentValue = trailingColumnsSpinBox->value();
        if (currentValue < 0) {
            result = showAllContents;
        } else {
            result = static_cast<unsigned long>(currentValue);
        }
    }

    return result;
}


const QFont& NumericValueFormatDialog::sampleFont() const {
    QLabel* sampleValuesLabel = widget<QLabel>("sample_values_label");
    return sampleValuesLabel->font();
}


void NumericValueFormatDialog::populate() {
    VariableNameEditorWidget* nameEditorWidget = widget<VariableNameEditorWidget>("variable_name_editor_widget");
    nameEditorWidget->setVariableName(inputVariableName);

    EQt::SpecialValueSpinBox* widthSpinBox = widget<EQt::SpecialValueSpinBox>("integer_width_spin_box");
    if (inputMantissaDigits == mantissaDigitsNotSet) {
        widthSpinBox->setValue(-1);
    } else if (inputMantissaDigits == ignoreMantissaDigits) {
        widthSpinBox->setValue(0);
    } else {
        widthSpinBox->setValue(inputMantissaDigits);
    }

    EQt::SpecialValueSpinBox* precisionSpinBox = widget<EQt::SpecialValueSpinBox>("precision_spin_box");
    if (inputPrecision == precisionNotSet) {
        precisionSpinBox->setValue(-1);
    } else if (inputPrecision == inferPrecision) {
        precisionSpinBox->setValue(0);
    } else {
        precisionSpinBox->setValue(inputPrecision);

    }

    QCheckBox* integerUpperCaseCheckBox = widget<QCheckBox>("integer_upper_case_check_box");
    QCheckBox* realUpperCaseCheckBox = widget<QCheckBox>("real_upper_case_check_box");
    switch (inputUpperCase) {
        case TriState::UNSET: {
            integerUpperCaseCheckBox->setTristate(true);
            integerUpperCaseCheckBox->setCheckState(Qt::PartiallyChecked);

            realUpperCaseCheckBox->setTristate(true);
            realUpperCaseCheckBox->setCheckState(Qt::PartiallyChecked);

            break;
        }

        case TriState::TRUE: {
            integerUpperCaseCheckBox->setTristate(false);
            integerUpperCaseCheckBox->setChecked(true);

            realUpperCaseCheckBox->setTristate(false);
            realUpperCaseCheckBox->setChecked(true);

            break;
        }

        case TriState::FALSE: {
            integerUpperCaseCheckBox->setTristate(false);
            integerUpperCaseCheckBox->setChecked(false);

            realUpperCaseCheckBox->setTristate(false);
            realUpperCaseCheckBox->setChecked(false);

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    configureIntegerNumberStyleButtons(inputIntegerNumberStyles);
    configureIntegerRadixButtons(inputBases);
    configureRealNumberStyleButtons(inputRealNumberStyles);

    EQt::LineEdit* multiplierLineEdit = widget<EQt::LineEdit>("multiplier_line_edit");
    if (inputMultiplicationFactor == multiplicationFactorNotSet) {
        multiplierLineEdit->clear();
    } else {
        multiplierLineEdit->setText(QString::number(inputMultiplicationFactor));
    }

    configureImaginaryUnitButtons(inputImaginaryUnits);
    configureBooleanStyleButtons(inputBooleanStyles);

    updateSampleValues();


    QComboBox* tupleDisplayModeComboBox      = widget<QComboBox>("tuple_display_mode_combo_box");
    QSpinBox*  leadingMemberCountSpinBox     = widget<QSpinBox>("leading_member_count_spin_box");
    QSpinBox*  trailingMemberCountSpinBox    = widget<QSpinBox>("trailing_member_count_spin_box");
    QCheckBox* showHiddenMemberCountCheckbox = widget<QCheckBox>("show_hidden_member_count_checkbox");

    configureTupleDisplayModeComboBox(inputTupleDisplayMode == TupleDisplayMode::INVALID);
    unsigned numberValues = static_cast<unsigned>(tupleDisplayModeComboBox->count());
    unsigned i            = 0;
    while (i < numberValues                                                                                       &&
           static_cast<TupleDisplayMode>(tupleDisplayModeComboBox->itemData(i).toUInt()) != inputTupleDisplayMode    ) {
        ++i;
    }

    if (i < numberValues) {
        tupleDisplayModeComboBox->setCurrentIndex(i);
    } else {
        tupleDisplayModeComboBox->setCurrentIndex(0);
    }

    if (inputLeadingMemberCount == invalidMemberCount) {
        leadingMemberCountSpinBox->clear();
        trailingMemberCountSpinBox->setEnabled(true);
    } else if (inputLeadingMemberCount == showAllContents) {
        leadingMemberCountSpinBox->setValue(-1);
        trailingMemberCountSpinBox->setEnabled(false);
    } else {
        leadingMemberCountSpinBox->setValue(inputLeadingMemberCount);
        trailingMemberCountSpinBox->setEnabled(true);
    }

    if (inputTrailingMemberCount == invalidMemberCount) {
        trailingMemberCountSpinBox->clear();
    } else if (inputTrailingMemberCount == showAllContents) {
        trailingMemberCountSpinBox->setValue(-1);
    } else {
        trailingMemberCountSpinBox->setValue(inputTrailingMemberCount);
    }

    if (inputShowHiddenMemberCounts == TriState::UNSET) {
        showHiddenMemberCountCheckbox->setTristate(true);
        showHiddenMemberCountCheckbox->setCheckState(Qt::PartiallyChecked);
    } else {
        showHiddenMemberCountCheckbox->setTristate(false);
        showHiddenMemberCountCheckbox->setChecked(inputShowHiddenMemberCounts == TriState::TRUE);
    }


    QSpinBox* leadingRowsCountSpinBox  = widget<QSpinBox>("leading_rows_spin_box");
    QSpinBox* trailingRowsCountSpinBox = widget<QSpinBox>("trailing_rows_spin_box");

    if (inputLeadingRowsCount == invalidMemberCount) {
        leadingRowsCountSpinBox->clear();
        trailingRowsCountSpinBox->setEnabled(true);
    } else if (inputLeadingRowsCount == showAllContents) {
        leadingRowsCountSpinBox->setValue(-1);
        trailingRowsCountSpinBox->setEnabled(false);
    } else {
        leadingRowsCountSpinBox->setValue(inputLeadingRowsCount);
        trailingRowsCountSpinBox->setEnabled(true);
    }

    if (inputTrailingRowsCount == invalidMemberCount) {
        trailingRowsCountSpinBox->clear();
    } else if (inputTrailingRowsCount == showAllContents) {
        trailingRowsCountSpinBox->setValue(-1);
    } else {
        trailingRowsCountSpinBox->setValue(inputTrailingRowsCount);
    }

    QSpinBox* leadingColumnsCountSpinBox  = widget<QSpinBox>("leading_columns_spin_box");
    QSpinBox* trailingColumnsCountSpinBox = widget<QSpinBox>("trailing_columns_spin_box");

    if (inputLeadingColumnsCount == invalidMemberCount) {
        leadingColumnsCountSpinBox->clear();
        trailingColumnsCountSpinBox->setEnabled(true);
    } else if (inputLeadingColumnsCount == showAllContents) {
        leadingColumnsCountSpinBox->setValue(-1);
        trailingColumnsCountSpinBox->setEnabled(false);
    } else {
        leadingColumnsCountSpinBox->setValue(inputLeadingColumnsCount);
        trailingColumnsCountSpinBox->setEnabled(true);
    }

    if (inputTrailingColumnsCount == invalidMemberCount) {
        trailingColumnsCountSpinBox->clear();
    } else if (inputTrailingColumnsCount == showAllContents) {
        trailingColumnsCountSpinBox->setValue(-1);
    } else {
        trailingColumnsCountSpinBox->setValue(inputTrailingColumnsCount);
    }


    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    okPushButton->setEnabled(false);
    cancelPushButton->setEnabled(true);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif
}


void NumericValueFormatDialog::setVariableNameFieldsEnabled(bool nowEnabled) {
    QGroupBox* variableNameGroupBox = widget<QGroupBox>("variable_name_group_box");
    variableNameGroupBox->setVisible(nowEnabled);
}


void NumericValueFormatDialog::setVariableNameFieldsDisabled(bool nowDisabled) {
    return setVariableNameFieldsEnabled(!nowDisabled);
}


void NumericValueFormatDialog::setSupportedDataTypes(const QSet<ValueType>& supportedDataTypes) {
    currentSupportedDataTypes = supportedDataTypes;

    QTabWidget* tabWidget            = widget<QTabWidget>("main_tab_widget");
    QGroupBox*  sampleValuesGroupBox = widget<QGroupBox>("sample_values_group_box");

    bool hasBoolean       = supportedDataTypes.contains(ValueType::BOOLEAN);
    bool hasInteger       = supportedDataTypes.contains(ValueType::INTEGER);
    bool hasReal          = supportedDataTypes.contains(ValueType::REAL);
    bool hasComplex       = supportedDataTypes.contains(ValueType::COMPLEX);
    bool hasSet           = supportedDataTypes.contains(ValueType::SET);
    bool hasTuple         = supportedDataTypes.contains(ValueType::TUPLE);
    bool hasMatrixBoolean = supportedDataTypes.contains(ValueType::MATRIX_BOOLEAN);
    bool hasMatrixInteger = supportedDataTypes.contains(ValueType::MATRIX_INTEGER);
    bool hasMatrixReal    = supportedDataTypes.contains(ValueType::MATRIX_REAL);
    bool hasMatrixComplex = supportedDataTypes.contains(ValueType::MATRIX_COMPLEX);

    bool hasType          = hasBoolean || hasInteger || hasReal || hasComplex;
    bool hasRealOrComplex = hasReal || hasComplex;
    bool hasMatrix        = hasMatrixBoolean || hasMatrixInteger || hasMatrixReal || hasMatrixComplex;

    tabWidget->setTabEnabled(0, hasBoolean);
    tabWidget->setTabEnabled(1, hasInteger);
    tabWidget->setTabEnabled(2, hasRealOrComplex);
    tabWidget->setTabEnabled(3, hasSet || hasTuple);
    tabWidget->setTabEnabled(4, hasMatrix);

    sampleValuesGroupBox->setVisible(hasType);
}


void NumericValueFormatDialog::setVariableName(const Ld::VariableName& newVariableName) {
    inputVariableName = newVariableName;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setBooleanStyle(const QSet<BooleanStyle>& newBooleanPresentationStyles) {
    inputBooleanStyles = newBooleanPresentationStyles;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setMantissaDigits(unsigned newMantissaDigits) {
    inputMantissaDigits = newMantissaDigits;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setMantissaDigitsUnset() {
    setMantissaDigits(mantissaDigitsNotSet);
}


void NumericValueFormatDialog::setMantissaDigitsIgnored() {
    setMantissaDigits(ignoreMantissaDigits);
}


void NumericValueFormatDialog::setUpperCase(bool nowUpperCase) {
    inputUpperCase = nowUpperCase ? TriState::TRUE : TriState::FALSE;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setLowerCase(bool nowLowerCase) {
    setUpperCase(!nowLowerCase);
}


void NumericValueFormatDialog::setCaseUnset() {
    inputUpperCase = TriState::UNSET;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setIntegerNumberStyles(
        const QSet<NumericValueFormatDialog::IntegerNumberStyle>& newIntegerNumberStyles
    ) {
    inputIntegerNumberStyles = newIntegerNumberStyles;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setBases(const QSet<unsigned>& newBases) {
    inputBases = newBases;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setRealNumberStyles(
        const QSet<NumericValueFormatDialog::RealNumberStyle>& newRealStyles
    ) {
    inputRealNumberStyles = newRealStyles;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setPrecision(unsigned newPrecision) {
    inputPrecision = newPrecision;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setPrecisionUnset() {
    setPrecision(precisionNotSet);
}


void NumericValueFormatDialog::setPrecisionInferred() {
    setPrecision(inferPrecision);
}


void NumericValueFormatDialog::setMultiplicationFactor(double newMultiplicationFactor) {
    inputMultiplicationFactor = newMultiplicationFactor;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setMultiplicationFactorUnset() {
    setMultiplicationFactor(multiplicationFactorNotSet);
}


void NumericValueFormatDialog::setImaginaryUnits(
        const QSet<NumericValueFormatDialog::ImaginaryUnit>& newImaginaryUnits
    ) {
    inputImaginaryUnits = newImaginaryUnits;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setTupleDisplayMode(NumericValueFormatDialog::TupleDisplayMode newDisplayMode) {
    inputTupleDisplayMode = newDisplayMode;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setTupleDisplayModeUnset() {
    setTupleDisplayMode(TupleDisplayMode::INVALID);
}


void NumericValueFormatDialog::setLeadingMemberCount(unsigned long newLeadingMemberCount) {
    inputLeadingMemberCount = newLeadingMemberCount;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setLeadingMemberCountUnset() {
    setLeadingMemberCount(invalidMemberCount);
}


void NumericValueFormatDialog::setTrailingMemberCount(unsigned long newTrailingMemberCount) {
    inputTrailingMemberCount = newTrailingMemberCount;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setTrailingMemberCountUnset() {
    setTrailingMemberCount(invalidMemberCount);
}


void NumericValueFormatDialog::setShowHiddenMemberCounts(bool nowShowHiddenMemberCounts) {
    inputShowHiddenMemberCounts = nowShowHiddenMemberCounts ? TriState::TRUE : TriState::FALSE;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setHideHiddenMemberCounts(bool nowHideHiddenMemberCounts) {
    setShowHiddenMemberCounts(!nowHideHiddenMemberCounts);
}


void NumericValueFormatDialog::setShowHideHiddenMemberCountsUnset() {
    inputShowHiddenMemberCounts = TriState::UNSET;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setLeadingRowsCount(unsigned long newLeadingRowsCount) {
    inputLeadingRowsCount = newLeadingRowsCount;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setLeadingRowsCountUnset() {
    setLeadingRowsCount(invalidMemberCount);
}


void NumericValueFormatDialog::setTrailingRowsCount(unsigned long newTrailingRowsCount) {
    inputTrailingRowsCount = newTrailingRowsCount;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setTrailingRowsCountUnset() {
    setTrailingRowsCount(invalidMemberCount);
}


void NumericValueFormatDialog::setLeadingColumnsCount(unsigned long newLeadingColumnsCount) {
    inputLeadingColumnsCount =  newLeadingColumnsCount;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setLeadingColumnsCountUnset() {
    setLeadingColumnsCount(invalidMemberCount);
}


void NumericValueFormatDialog::setTrailingColumnsCount(unsigned long newTrailingColumnsCount) {
    inputTrailingColumnsCount = newTrailingColumnsCount;

    if (isVisible()) {
        populate();
    }
}


void NumericValueFormatDialog::setTrailingColumnsCountUnset() {
    setTrailingColumnsCount(invalidMemberCount);
}


void NumericValueFormatDialog::setSampleFont(const QFont& newFont) {
    QLabel* sampleValuesLabel = widget<QLabel>("sample_values_label");
    sampleValuesLabel->setFont(newFont);
}


void NumericValueFormatDialog::bind() {
    VariableNameEditorWidget* nameEditorWidget = widget<VariableNameEditorWidget>("variable_name_editor_widget");
    connect(
        nameEditorWidget,
        &VariableNameEditorWidget::changed,
        this,
        &NumericValueFormatDialog::variableNameEdited
    );

    EQt::SpecialValueSpinBox* widthSpinBox = widget<EQt::SpecialValueSpinBox>("integer_width_spin_box");
    connect(
        widthSpinBox,
        static_cast<void (EQt::SpecialValueSpinBox::*)(int)>(&EQt::SpecialValueSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::widthSpinBoxChanged
    );

    EQt::SpecialValueSpinBox* precisionSpinBox = widget<EQt::SpecialValueSpinBox>("precision_spin_box");
    connect(
        precisionSpinBox,
        static_cast<void (EQt::SpecialValueSpinBox::*)(int)>(&EQt::SpecialValueSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::precisionSpinBoxChanged
    );

    QCheckBox* integerUpperCaseCheckBox = widget<QCheckBox>("integer_upper_case_check_box");
    connect(
        integerUpperCaseCheckBox,
        &QCheckBox::clicked,
        this,
        &NumericValueFormatDialog::integerUpperCaseCheckBoxClicked
    );

    QCheckBox* realUpperCaseCheckBox = widget<QCheckBox>("real_upper_case_check_box");
    connect(
        realUpperCaseCheckBox,
        &QCheckBox::clicked,
        this,
        &NumericValueFormatDialog::realUpperCaseCheckBoxClicked
    );

    QRadioButton* noPrefixSuffixStyleRadioButton = widget<QRadioButton>("integer_no_prefix_suffix_style_radio_button");
    QRadioButton* cStyleRadioButton              = widget<QRadioButton>("integer_c_style_radio_button");
    QRadioButton* verilogStyleRadioButton        = widget<QRadioButton>("integer_verilog_style_radio_button");
    QRadioButton* subscriptStyleRadioButton      = widget<QRadioButton>("integer_subscript_style_radio_button");

    connect(
        noPrefixSuffixStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::noPrefixSuffixStyleRadioButtonClicked
    );
    connect(cStyleRadioButton, &QRadioButton::clicked, this, &NumericValueFormatDialog::cStyleRadioButtonClicked);
    connect(
        verilogStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::verilogStyleRadioButtonClicked
    );
    connect(
        subscriptStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::subscriptStyleRadioButtonClicked
    );

    QRadioButton* base2RadioButton = widget<QRadioButton>("integer_base_2_radio_button");
    QRadioButton* base10RadioButton = widget<QRadioButton>("integer_base_10_radio_button");
    QRadioButton* base16RadioButton = widget<QRadioButton>("integer_base_16_radio_button");

    connect(base2RadioButton, &QRadioButton::clicked, this, &NumericValueFormatDialog::base2RadioButtonClicked);
    connect(base10RadioButton, &QRadioButton::clicked, this, &NumericValueFormatDialog::base10RadioButtonClicked);
    connect(base16RadioButton, &QRadioButton::clicked, this, &NumericValueFormatDialog::base16RadioButtonClicked);

    QRadioButton* fixedPointStyleRadioButton         = widget<QRadioButton>("real_fixed_point_style_radio_button");
    QRadioButton* scientificStyleRadioButton         = widget<QRadioButton>("real_scientific_style_radio_button");
    QRadioButton* engineeringStyleRadioButton        = widget<QRadioButton>("real_engineering_style_radio_button");
    QRadioButton* computerScientificStyleRadioButton = widget<QRadioButton>(
        "real_computer_scientific_style_radio_button"
    );
    QRadioButton* conciseStyleRadioButton            = widget<QRadioButton>("real_concise_style_radio_button");
    QRadioButton* conciseComputerStyleRadioButton    = widget<QRadioButton>("real_concise_computer_style_radio_button");

    connect(
        fixedPointStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::fixedPointStyleRadioButtonClicked
    );
    connect(
        scientificStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::scientificStyleRadioButtonClicked
    );
    connect(
        engineeringStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::engineeringStyleRadioButtonClicked
    );
    connect(
        computerScientificStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::computerScientificStyleRadioButtonClicked
    );
    connect(
        conciseStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::conciseStyleRadioButtonClicked
    );
    connect(
        conciseComputerStyleRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::conciseComputerStyleRadioButtonClicked
    );

    QRadioButton* imaginaryUnitIRadioButton = widget<QRadioButton>("complex_imaginary_unit_i_radio_button");
    QRadioButton* imaginaryUnitJRadioButton = widget<QRadioButton>("complex_imaginary_unit_j_radio_button");

    connect(
        imaginaryUnitIRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::imaginaryUnitIRadioButtonClicked
    );
    connect(
        imaginaryUnitJRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::imaginaryUnitJRadioButtonClicked
    );

    EQt::LineEdit* multiplierLineEdit = widget<EQt::LineEdit>("multiplier_line_edit");
    connect(multiplierLineEdit, &EQt::LineEdit::textEdited, this, &NumericValueFormatDialog::multiplierEdited);

    QRadioButton* lowerCaseTrueFalseRadioButton = widget<QRadioButton>("lower_case_true_false_radio_button");
    connect(
        lowerCaseTrueFalseRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::lowerCaseTrueFalseRadioButtonClicked
    );

    QRadioButton* titleCaseTrueFalseRadioButton = widget<QRadioButton>("title_case_true_false_radio_button");
    connect(
        titleCaseTrueFalseRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::titleCaseTrueFalseRadioButtonClicked
    );

    QRadioButton* upperCaseTrueFalseRadioButton = widget<QRadioButton>("upper_case_true_false_radio_button");
    connect(
        upperCaseTrueFalseRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::upperCaseTrueFalseRadioButtonClicked
    );

    QRadioButton* lowerCaseYesNoRadioButton = widget<QRadioButton>("lower_case_yes_no_radio_button");
    connect(
        lowerCaseYesNoRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::lowerCaseYesNoRadioButtonClicked
    );

    QRadioButton* titleCaseYesNoRadioButton = widget<QRadioButton>("title_case_yes_no_radio_button");
    connect(
        titleCaseYesNoRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::titleCaseYesNoRadioButtonClicked
    );

    QRadioButton* upperCaseYesNoRadioButton = widget<QRadioButton>("upper_case_yes_no_radio_button");
    connect(
        upperCaseYesNoRadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::upperCaseYesNoRadioButtonClicked
    );

    QRadioButton* numeric10RadioButton = widget<QRadioButton>("numeric_1_0_radio_button");
    connect(
        numeric10RadioButton,
        &QRadioButton::clicked,
        this,
        &NumericValueFormatDialog::numeric10RadioButtonClicked
    );

    QComboBox* tupleDisplayModeComboBox = widget<QComboBox>("tuple_display_mode_combo_box");
    connect(
        tupleDisplayModeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &NumericValueFormatDialog::tupleDisplayModeChanged
    );

    QSpinBox* leadingMemberCountSpinBox = widget<QSpinBox>("leading_member_count_spin_box");
    connect(
        leadingMemberCountSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::leadingMemberCountChanged
    );

    QSpinBox* trailingMemberCountSpinBox = widget<QSpinBox>("trailing_member_count_spin_box");
    connect(
        trailingMemberCountSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::trailingMemberCountChanged
    );

    QCheckBox* showHiddenMemberCountCheckbox = widget<QCheckBox>("show_hidden_member_count_checkbox");
    connect(
        showHiddenMemberCountCheckbox,
        &QCheckBox::stateChanged,
        this,
        &NumericValueFormatDialog::showHiddenMemberCountCheckboxClicked
    );

    QSpinBox* leadingRowsSpinBox = widget<QSpinBox>("leading_rows_spin_box");
    connect(
        leadingRowsSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::leadingRowsCountChanged
    );

    QSpinBox* trailingRowsSpinBox = widget<QSpinBox>("trailing_rows_spin_box");
    connect(
        trailingRowsSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::trailingRowsCountChanged
    );

    QSpinBox* leadingColumnsSpinBox = widget<QSpinBox>("leading_columns_spin_box");
    connect(
        leadingColumnsSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::leadingColumnsCountChanged
    );

    QSpinBox* trailingColumnsSpinBox = widget<QSpinBox>("trailing_columns_spin_box");
    connect(
        trailingColumnsSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &NumericValueFormatDialog::trailingColumnsCountChanged
    );

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &NumericValueFormatDialog::accept);

    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
    connect(cancelButton, &QPushButton::clicked, this, &NumericValueFormatDialog::reject);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        connect(applyButton, &QPushButton::clicked, this, &NumericValueFormatDialog::applyClicked);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    ProgrammaticDialog::bind();
}


void NumericValueFormatDialog::variableNameEdited(const QString& /* newText1 */, const QString& /* newText2 */) {
    updateOkApplyButtons();
}


void NumericValueFormatDialog::widthSpinBoxChanged(int newValue) {
    if (newValue != -1) {
        EQt::SpecialValueSpinBox* widthSpinBox = widget<EQt::SpecialValueSpinBox>("integer_width_spin_box");
        widthSpinBox->setMinimum(0);
    }

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::precisionSpinBoxChanged(int newValue) {
    if (newValue != -1) {
        EQt::SpecialValueSpinBox* precisionSpinBox = widget<EQt::SpecialValueSpinBox>("precision_spin_box");
        precisionSpinBox->setMinimum(0);
    }

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::integerUpperCaseCheckBoxClicked(bool /* nowChecked */) {
    QCheckBox* integerUpperCaseCheckBox = widget<QCheckBox>("integer_upper_case_check_box");
    QCheckBox* realUpperCaseCheckBox = widget<QCheckBox>("real_upper_case_check_box");

    integerUpperCaseCheckBox->setTristate(false);

    realUpperCaseCheckBox->setTristate(false);
    realUpperCaseCheckBox->setChecked(integerUpperCaseCheckBox->isChecked());

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::realUpperCaseCheckBoxClicked(bool /* nowChecked */) {
    QCheckBox* realUpperCaseCheckBox = widget<QCheckBox>("real_upper_case_check_box");
    QCheckBox* integerUpperCaseCheckBox = widget<QCheckBox>("integer_upper_case_check_box");

    realUpperCaseCheckBox->setTristate(false);

    integerUpperCaseCheckBox->setTristate(false);
    integerUpperCaseCheckBox->setChecked(realUpperCaseCheckBox->isChecked());

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::noPrefixSuffixStyleRadioButtonClicked() {
    configureIntegerNumberStyleButtons(QSet<IntegerNumberStyle>() << IntegerNumberStyle::NONE);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::cStyleRadioButtonClicked() {
    configureIntegerNumberStyleButtons(QSet<IntegerNumberStyle>() << IntegerNumberStyle::C_STYLE);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::verilogStyleRadioButtonClicked() {
    configureIntegerNumberStyleButtons(QSet<IntegerNumberStyle>() << IntegerNumberStyle::VERILOG_STYLE);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::subscriptStyleRadioButtonClicked() {
    configureIntegerNumberStyleButtons(QSet<IntegerNumberStyle>() << IntegerNumberStyle::SUBSCRIPT_STYLE);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::base2RadioButtonClicked() {
    configureIntegerRadixButtons(QSet<unsigned>() << 2U);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::base10RadioButtonClicked() {
    configureIntegerRadixButtons(QSet<unsigned>() << 10U);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::base16RadioButtonClicked() {
    configureIntegerRadixButtons(QSet<unsigned>() << 16U);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::fixedPointStyleRadioButtonClicked() {
    configureRealNumberStyleButtons(QSet<RealNumberStyle>() << RealNumberStyle::FLOATING_POINT);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::scientificStyleRadioButtonClicked() {
    configureRealNumberStyleButtons(QSet<RealNumberStyle>() << RealNumberStyle::SCIENTIFIC);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::engineeringStyleRadioButtonClicked() {
    configureRealNumberStyleButtons(QSet<RealNumberStyle>() << RealNumberStyle::ENGINEERING);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::computerScientificStyleRadioButtonClicked() {
    configureRealNumberStyleButtons(QSet<RealNumberStyle>() << RealNumberStyle::COMPUTER_SCIENTIFIC);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::conciseStyleRadioButtonClicked() {
    configureRealNumberStyleButtons(QSet<RealNumberStyle>() << RealNumberStyle::CONCISE);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::conciseComputerStyleRadioButtonClicked() {
    configureRealNumberStyleButtons(QSet<RealNumberStyle>() << RealNumberStyle::COMPUTER_CONCISE);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::imaginaryUnitIRadioButtonClicked() {
    configureImaginaryUnitButtons(QSet<ImaginaryUnit>() << ImaginaryUnit::I);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::imaginaryUnitJRadioButtonClicked() {
    configureImaginaryUnitButtons(QSet<ImaginaryUnit>() << ImaginaryUnit::J);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::multiplierEdited(const QString& newMultiplierText) {
    EQt::LineEdit* multiplierLineEdit = widget<EQt::LineEdit>("multiplier_line_edit");
    bool       ok;
    double     value = newMultiplierText.toDouble(&ok);

    QPalette palette;
    if (ok && value != double(0)) {
        palette.setColor(QPalette::Text, Qt::black);
    } else {
        palette.setColor(QPalette::Text, Qt::red);
    }

    multiplierLineEdit->setPalette(palette);

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::lowerCaseTrueFalseRadioButtonClicked() {
    configureBooleanStyleButtons(QSet<BooleanStyle>() << BooleanStyle::LOWER_CASE_TRUE_FALSE);
    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::titleCaseTrueFalseRadioButtonClicked() {
    configureBooleanStyleButtons(QSet<BooleanStyle>() << BooleanStyle::TITLE_CASE_TRUE_FALSE);
    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::upperCaseTrueFalseRadioButtonClicked() {
    configureBooleanStyleButtons(QSet<BooleanStyle>() << BooleanStyle::UPPER_CASE_TRUE_FALSE);
    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::lowerCaseYesNoRadioButtonClicked() {
    configureBooleanStyleButtons(QSet<BooleanStyle>() << BooleanStyle::LOWER_CASE_YES_NO);
    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::titleCaseYesNoRadioButtonClicked() {
    configureBooleanStyleButtons(QSet<BooleanStyle>() << BooleanStyle::TITLE_CASE_YES_NO);
    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::upperCaseYesNoRadioButtonClicked() {
    configureBooleanStyleButtons(QSet<BooleanStyle>() << BooleanStyle::UPPER_CASE_YES_NO);
    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::numeric10RadioButtonClicked() {
    configureBooleanStyleButtons(QSet<BooleanStyle>() << BooleanStyle::NUMERIC_1_0);
    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::tupleDisplayModeChanged(int index) {
    QComboBox*       comboBox    = widget<QComboBox>("tuple_display_mode_combo_box");
    TupleDisplayMode displayMode = static_cast<TupleDisplayMode>(comboBox->itemData(index).toUInt());

    if (displayMode != TupleDisplayMode::INVALID) {
        TupleDisplayMode firstSelection = static_cast<TupleDisplayMode>(comboBox->itemData(0).toUInt());
        if (firstSelection == TupleDisplayMode::INVALID) {
            comboBox->removeItem(0);
        }
    }

    updateSampleValues();
    updateOkApplyButtons();
}


void NumericValueFormatDialog::leadingMemberCountChanged(int newValue) {
    QSpinBox* trailingMemberCountSpinBox = widget<QSpinBox>("trailing_member_count_spin_box");
    if (newValue < 0) {
        trailingMemberCountSpinBox->setEnabled(false);
    } else {
        trailingMemberCountSpinBox->setEnabled(true);
    }

    updateOkApplyButtons();
}


void NumericValueFormatDialog::trailingMemberCountChanged(int /* newValue */) {
    updateOkApplyButtons();
}


void NumericValueFormatDialog::showHiddenMemberCountCheckboxClicked(int newState) {
    if (newState != Qt::PartiallyChecked) {
        QCheckBox* showHiddenMemberCountCheckbox = widget<QCheckBox>("show_hidden_member_count_checkbox");
        showHiddenMemberCountCheckbox->setTristate(false);
    }

    updateOkApplyButtons();
}


void NumericValueFormatDialog::leadingRowsCountChanged(int newValue) {
    QSpinBox* trailingRowsSpinBox = widget<QSpinBox>("trailing_rows_spin_box");

    if (newValue < 0) {
        trailingRowsSpinBox->setEnabled(false);
    } else {
        trailingRowsSpinBox->setEnabled(true);
    }

    updateOkApplyButtons();
}


void NumericValueFormatDialog::trailingRowsCountChanged(int /* newValue */) {
    updateOkApplyButtons();
}


void NumericValueFormatDialog::leadingColumnsCountChanged(int newValue) {
    QSpinBox* trailingColumnsSpinBox = widget<QSpinBox>("trailing_columns_spin_box");

    if (newValue < 0) {
        trailingColumnsSpinBox->setEnabled(false);
    } else {
        trailingColumnsSpinBox->setEnabled(true);
    }

    updateOkApplyButtons();
}


void NumericValueFormatDialog::trailingColumnsCountChanged(int /* newValue */) {
    updateOkApplyButtons();
}


void NumericValueFormatDialog::applyClicked() {
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


void NumericValueFormatDialog::updateOkApplyButtons() {
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


void NumericValueFormatDialog::configureWidget() {
    setWindowTitle(tr("Insert Field"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentSupportedDataTypes.clear();
    currentSupportedDataTypes << ValueType::BOOLEAN << ValueType::INTEGER << ValueType::REAL << ValueType::COMPLEX;

    currentAutoApplyEnabled   = false;

    inputVariableName         = Ld::VariableName();
    inputMantissaDigits       = ignoreMantissaDigits;
    inputUpperCase            = TriState::FALSE;
    inputPrecision            = inferPrecision;
    inputMultiplicationFactor = 1.0;

    inputIntegerNumberStyles.clear();
    inputBases.clear();
    inputRealNumberStyles.clear();

    inputTupleDisplayMode       = TupleDisplayMode::NORMAL;
    inputLeadingMemberCount     = showAllContents;
    inputTrailingMemberCount    = showAllContents;
    inputShowHiddenMemberCounts = TriState::TRUE;

    inputLeadingRowsCount     = showAllContents;
    inputTrailingRowsCount    = 0;
    inputLeadingColumnsCount  = showAllContents;
    inputTrailingColumnsCount = 0;

    QFontMetrics applicationFontMetrics(Application::font());

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    QHBoxLayout* topHorizontalLayout = newHBoxLayout("top_horizontal_layout");
    mainLayout->addLayout(topHorizontalLayout);

    QGroupBox* variableNameGroupBox = new QGroupBox(tr("Variable Name"));
    registerWidget(variableNameGroupBox, "variable_name_group_box");
    topHorizontalLayout->addWidget(variableNameGroupBox);

    QVBoxLayout* variableNameLayout = newVBoxLayout("variable_name_vertical_layout");
    variableNameGroupBox->setLayout(variableNameLayout);

    VariableNameEditorWidget* variableNameEditorWidget = new VariableNameEditorWidget;
    registerWidget(variableNameEditorWidget, "variable_name_editor_widget");
    variableNameLayout->addWidget(variableNameEditorWidget);

    topHorizontalLayout->addStretch(1);

    QVBoxLayout* topRightVerticalLayout = newVBoxLayout("top_right_vertical_layout");
    topHorizontalLayout->addLayout(topRightVerticalLayout);


    QGroupBox* sampleValuesGroupBox = new QGroupBox(tr("Sample Values"));
    registerWidget(sampleValuesGroupBox, "sample_values_group_box");
    topRightVerticalLayout->addWidget(sampleValuesGroupBox);

    QVBoxLayout* sampleValuesVerticalLayout = newVBoxLayout("sample_values_vertical_layout");
    sampleValuesGroupBox->setLayout(sampleValuesVerticalLayout);

    QFont defaultMathIdentifierFont = Ld::CharacterFormat::applicationDefaultMathIdentifierFont()->toQFont();

    QLabel* sampleValuesLabel = new QLabel(tr("???\n???\n???"));
    sampleValuesLabel->setFont(defaultMathIdentifierFont);
    sampleValuesLabel->setAlignment(Qt::AlignCenter);
    registerWidget(sampleValuesLabel, "sample_values_label");
    sampleValuesVerticalLayout->addWidget(sampleValuesLabel);

    QTabWidget* mainTabWidget = new QTabWidget;
    registerWidget(mainTabWidget, "main_tab_widget");
    mainLayout->addWidget(mainTabWidget);


    QWidget* booleanStyleWidget = new QWidget(mainTabWidget);
    registerWidget(booleanStyleWidget, "boolean_style_widget");
    mainTabWidget->addTab(booleanStyleWidget, tr("Booleans"));

    QVBoxLayout* booleanVerticalLayout = newVBoxLayout("boolean_vertical_layout");
    booleanStyleWidget->setLayout(booleanVerticalLayout);


    QWidget* integerStyleWidget = new QWidget(mainTabWidget);
    registerWidget(integerStyleWidget, "integer_style_widget");
    mainTabWidget->addTab(integerStyleWidget, tr("Integers"));

    QVBoxLayout* integerVerticalLayout = newVBoxLayout("integer_vertical_layout");
    integerStyleWidget->setLayout(integerVerticalLayout);


    QWidget* realComplexStyleWidget = new QWidget(mainTabWidget);
    registerWidget(realComplexStyleWidget, "real_complex_style_widget");
    mainTabWidget->addTab(realComplexStyleWidget, tr("Real/Complex"));

    QVBoxLayout* realComplexVerticalLayout = newVBoxLayout("real_complex_vertical_layout");
    realComplexStyleWidget->setLayout(realComplexVerticalLayout);


    QWidget* setTupleStyleWidget = new QWidget(mainTabWidget);
    registerWidget(setTupleStyleWidget, "set_tuple_style_widget");
    mainTabWidget->addTab(setTupleStyleWidget, tr("Sets/Tuples"));

    QVBoxLayout* setTupleVerticalLayout = newVBoxLayout("set_tuple_vertical_layout");
    setTupleStyleWidget->setLayout(setTupleVerticalLayout);


    QWidget* matrixStyleWidget = new QWidget(mainTabWidget);
    registerWidget(matrixStyleWidget, "matrix_style_widget");
    mainTabWidget->addTab(matrixStyleWidget, tr("Matrix"));

    QVBoxLayout* matrixVerticalLayout = newVBoxLayout("matrix_vertical_layout");
    matrixStyleWidget->setLayout(matrixVerticalLayout);


    // Boolean tab.

    QRadioButton* lowerCaseTrueFalseRadioButton = new QRadioButton(tr("Lower case true/false"));
    lowerCaseTrueFalseRadioButton->setAutoExclusive(false);
    registerWidget(lowerCaseTrueFalseRadioButton, "lower_case_true_false_radio_button");
    booleanVerticalLayout->addWidget(lowerCaseTrueFalseRadioButton);

    QRadioButton* titleCaseTrueFalseRadioButton = new QRadioButton(tr("Title case True/False"));
    titleCaseTrueFalseRadioButton->setAutoExclusive(false);
    registerWidget(titleCaseTrueFalseRadioButton, "title_case_true_false_radio_button");
    booleanVerticalLayout->addWidget(titleCaseTrueFalseRadioButton);

    QRadioButton* upperCaseTrueFalseRadioButton = new QRadioButton(tr("Upper case TRUE/FALSE"));
    upperCaseTrueFalseRadioButton->setAutoExclusive(false);
    registerWidget(upperCaseTrueFalseRadioButton, "upper_case_true_false_radio_button");
    booleanVerticalLayout->addWidget(upperCaseTrueFalseRadioButton);

    QRadioButton* lowerCaseYesNoRadioButton = new QRadioButton(tr("Lower case yes/no"));
    lowerCaseYesNoRadioButton->setAutoExclusive(false);
    registerWidget(lowerCaseYesNoRadioButton, "lower_case_yes_no_radio_button");
    booleanVerticalLayout->addWidget(lowerCaseYesNoRadioButton);

    QRadioButton* titleCaseYesNoRadioButton = new QRadioButton(tr("Title case Yes/No"));
    titleCaseYesNoRadioButton->setAutoExclusive(false);
    registerWidget(titleCaseYesNoRadioButton, "title_case_yes_no_radio_button");
    booleanVerticalLayout->addWidget(titleCaseYesNoRadioButton);

    QRadioButton* upperCaseYesNoRadioButton = new QRadioButton(tr("Upper case Yes/No"));
    upperCaseYesNoRadioButton->setAutoExclusive(false);
    registerWidget(upperCaseYesNoRadioButton, "upper_case_yes_no_radio_button");
    booleanVerticalLayout->addWidget(upperCaseYesNoRadioButton);

    QRadioButton* numeric10RadioButton = new QRadioButton(tr("Numeric 1/0"));
    numeric10RadioButton->setAutoExclusive(false);
    registerWidget(numeric10RadioButton, "numeric_1_0_radio_button");
    booleanVerticalLayout->addWidget(numeric10RadioButton);


    // Integer tab

    QFormLayout* integerFormLayout = newFormLayout("integer_form_layout");
    integerVerticalLayout->addLayout(integerFormLayout);

    EQt::SpecialValueSpinBox* integerWidthSpinBox = new EQt::SpecialValueSpinBox;
    integerWidthSpinBox->setMinimum(-1);
    integerWidthSpinBox->setMaximum(
        std::max(std::numeric_limits<Model::Real>::digits10, std::numeric_limits<Model::Integer>::digits10)
    );
    integerWidthSpinBox->insert(-1, tr(" "));
    integerWidthSpinBox->insert(0, tr("Infer from value"));
    integerWidthSpinBox->setMinimumWidth(1.33 * applicationFontMetrics.horizontalAdvance(tr("Infer from value")));
    registerWidget(integerWidthSpinBox, "integer_width_spin_box");

    integerFormLayout->addRow(tr("Width: "), integerWidthSpinBox);

    QGroupBox* integerStyleGroupBox = new QGroupBox(tr("Style"));
    registerWidget(integerStyleGroupBox, "integer_style_group_box");
    integerVerticalLayout->addWidget(integerStyleGroupBox);

    QVBoxLayout* integerStyleLayout = newVBoxLayout("integer_style_layout");
    integerStyleGroupBox->setLayout(integerStyleLayout);

    QRadioButton* integerNoPrefixSuffixStyleRadioButton = new QRadioButton(tr("No prefix or suffix"));
    integerNoPrefixSuffixStyleRadioButton->setAutoExclusive(false);
    registerWidget(integerNoPrefixSuffixStyleRadioButton, "integer_no_prefix_suffix_style_radio_button");
    integerStyleLayout->addWidget(integerNoPrefixSuffixStyleRadioButton);

    QRadioButton* integerCStyleRadioButton = new QRadioButton(tr("ANSI-C/C++ style integer prefixes"));
    integerCStyleRadioButton->setAutoExclusive(false);
    registerWidget(integerCStyleRadioButton, "integer_c_style_radio_button");
    integerStyleLayout->addWidget(integerCStyleRadioButton);

    QRadioButton* integerVerilogStyleRadioButton = new QRadioButton(tr("Verilog style integer prefixes"));
    integerVerilogStyleRadioButton->setAutoExclusive(false);
    registerWidget(integerVerilogStyleRadioButton, "integer_verilog_style_radio_button");
    integerStyleLayout->addWidget(integerVerilogStyleRadioButton);

    QRadioButton* integerSubscriptStyleRadioButton = new QRadioButton(tr("Base subscripts"));
    integerSubscriptStyleRadioButton->setAutoExclusive(false);
    registerWidget(integerSubscriptStyleRadioButton, "integer_subscript_style_radio_button");
    integerStyleLayout->addWidget(integerSubscriptStyleRadioButton);

    QGroupBox* integerBaseGroupBox = new QGroupBox(tr("Base"));
    registerWidget(integerBaseGroupBox, "integer_base_group_box");
    integerVerticalLayout->addWidget(integerBaseGroupBox);

    QVBoxLayout* integerBaseLayout = newVBoxLayout("integer_base_layout");
    integerBaseGroupBox->setLayout(integerBaseLayout);

    QRadioButton* integerBase2RadioButton = new QRadioButton(tr("Base 2"));
    integerBase2RadioButton->setAutoExclusive(false);
    registerWidget(integerBase2RadioButton, "integer_base_2_radio_button");
    integerBaseLayout->addWidget(integerBase2RadioButton);

    QRadioButton* integerBase10RadioButton = new QRadioButton(tr("Base 10"));
    integerBase10RadioButton->setAutoExclusive(false);
    registerWidget(integerBase10RadioButton, "integer_base_10_radio_button");
    integerBaseLayout->addWidget(integerBase10RadioButton);

    QRadioButton* integerBase16RadioButton = new QRadioButton(tr("Base 16"));
    integerBase16RadioButton->setAutoExclusive(false);
    registerWidget(integerBase16RadioButton, "integer_base_16_radio_button");
    integerBaseLayout->addWidget(integerBase16RadioButton);

    QCheckBox* integerUpperCaseCheckBox = new QCheckBox(
        tr("Use upper case for hexidecimal digits A through F and real exponent \"E\"")
    );
    integerUpperCaseCheckBox->setTristate(true);
    registerWidget(integerUpperCaseCheckBox, "integer_upper_case_check_box");
    integerVerticalLayout->addWidget(integerUpperCaseCheckBox);

    integerVerticalLayout->addStretch(1);


    // Real/Complex tab

    QFormLayout* precisionFormLayout = newFormLayout("settings_form_layout");
    realComplexVerticalLayout->addLayout(precisionFormLayout);

    EQt::SpecialValueSpinBox* precisionSpinBox = new EQt::SpecialValueSpinBox;
    precisionSpinBox->setMinimum(-1);
    precisionSpinBox->setMaximum(
        std::max(std::numeric_limits<Model::Real>::digits10, std::numeric_limits<Model::Integer>::digits10)
    );
    precisionSpinBox->insert(-1, tr(" "));
    precisionSpinBox->insert(0, tr("Infer from value"));
    precisionSpinBox->setMinimumWidth(1.33 * applicationFontMetrics.horizontalAdvance(tr("Infer from value")));
    registerWidget(precisionSpinBox, "precision_spin_box");

    precisionFormLayout->addRow(tr("Precision: "), precisionSpinBox);

    QGroupBox* realNumberStyleGroupBox = new QGroupBox(tr("Style"));
    registerWidget(realNumberStyleGroupBox, "real_number_style_group_box");
    realComplexVerticalLayout->addWidget(realNumberStyleGroupBox);

    QVBoxLayout* realNumberStyleLayout = newVBoxLayout("real_number_style_layout");
    realNumberStyleGroupBox->setLayout(realNumberStyleLayout);

    QRadioButton* realFixedPointStyleRadioButton = new QRadioButton(tr("Fixed point"));
    realFixedPointStyleRadioButton->setAutoExclusive(false);
    registerWidget(realFixedPointStyleRadioButton, "real_fixed_point_style_radio_button");
    realNumberStyleLayout->addWidget(realFixedPointStyleRadioButton);

    QRadioButton* realScientificStyleRadioButton = new QRadioButton(tr("Scientific notation"));
    realScientificStyleRadioButton->setAutoExclusive(false);
    registerWidget(realScientificStyleRadioButton, "real_scientific_style_radio_button");
    realNumberStyleLayout->addWidget(realScientificStyleRadioButton);

    QRadioButton* realEngineeringStyleRadioButton = new QRadioButton(tr("Engineering notation"));
    realEngineeringStyleRadioButton->setAutoExclusive(false);
    registerWidget(realEngineeringStyleRadioButton, "real_engineering_style_radio_button");
    realNumberStyleLayout->addWidget(realEngineeringStyleRadioButton);

    QRadioButton* realComputerScientificStyleRadioButton = new QRadioButton(tr("Computer scientific notation"));
    realComputerScientificStyleRadioButton->setAutoExclusive(false);
    registerWidget(realComputerScientificStyleRadioButton, "real_computer_scientific_style_radio_button");
    realNumberStyleLayout->addWidget(realComputerScientificStyleRadioButton);

    QRadioButton* realConciseStyleRadioButton = new QRadioButton(
        tr("Most concise of fixed point and scientific notation")
    );
    realConciseStyleRadioButton->setAutoExclusive(false);
    registerWidget(realConciseStyleRadioButton, "real_concise_style_radio_button");
    realNumberStyleLayout->addWidget(realConciseStyleRadioButton);

    QRadioButton* realConciseComputerStyleRadioButton = new QRadioButton(
        tr("Most concise of fixed point and computer scientific notation")
    );
    realConciseComputerStyleRadioButton->setAutoExclusive(false);
    registerWidget(realConciseComputerStyleRadioButton, "real_concise_computer_style_radio_button");
    realNumberStyleLayout->addWidget(realConciseComputerStyleRadioButton);

    QHBoxLayout* realComplexHorizontalLayout = newHBoxLayout("real_complex_horizontal_layout");
    realComplexVerticalLayout->addLayout(realComplexHorizontalLayout);

    QGroupBox* complexImaginaryUnitGroupBox = new QGroupBox(tr("Imaginary Unit"));
    registerWidget(complexImaginaryUnitGroupBox, "complex_imaginary_unit_group_box");
    realComplexHorizontalLayout->addWidget(complexImaginaryUnitGroupBox);

    QVBoxLayout* complexImaginaryUnitLayout = newVBoxLayout("complex_imaginary_unit_layout");
    complexImaginaryUnitGroupBox->setLayout(complexImaginaryUnitLayout);

    QRadioButton* complexImaginaryUnitIRadioButton = new QRadioButton(tr("i"));
    complexImaginaryUnitIRadioButton->setAutoExclusive(false);
    registerWidget(complexImaginaryUnitIRadioButton, "complex_imaginary_unit_i_radio_button");
    complexImaginaryUnitLayout->addWidget(complexImaginaryUnitIRadioButton);

    QRadioButton* complexImaginaryUnitJRadioButton = new QRadioButton(tr("j"));
    complexImaginaryUnitJRadioButton->setAutoExclusive(false);
    registerWidget(complexImaginaryUnitJRadioButton, "complex_imaginary_unit_j_radio_button");
    complexImaginaryUnitLayout->addWidget(complexImaginaryUnitJRadioButton);

    QVBoxLayout* multiplierUpperCaseVerticalLayout = newVBoxLayout("multiplier_upper_case_vertical_layout");
    realComplexHorizontalLayout->addLayout(multiplierUpperCaseVerticalLayout);

    QFormLayout* multiplierFormLayout = newFormLayout("multiplier_form_layout");
    realComplexVerticalLayout->addLayout(multiplierFormLayout);

    EQt::LineEdit* multiplierLineEdit = new EQt::LineEdit;
    multiplierLineEdit->setValidator(new QDoubleValidator(multiplierLineEdit));
    registerWidget(multiplierLineEdit, "multiplier_line_edit");

    QLabel* multiplierLabel = new QLabel(tr("Multiplier: "));
    registerWidget(multiplierLabel, "multiplier_label");

    multiplierFormLayout->addRow(multiplierLabel, multiplierLineEdit);

    QCheckBox* realComplexUpperCaseCheckBox = new QCheckBox(
        tr("Use upper case for hexidecimal digits A through F and real exponent \"E\"")
    );
    realComplexUpperCaseCheckBox->setTristate(true);
    registerWidget(realComplexUpperCaseCheckBox, "real_upper_case_check_box");
    multiplierUpperCaseVerticalLayout->addWidget(realComplexUpperCaseCheckBox);

    realComplexVerticalLayout->addStretch(1);


    // Set/Tuple tab

    QFormLayout* setTupleFormLayout = newFormLayout("set_tuple_form_layout");
    setTupleVerticalLayout->addLayout(setTupleFormLayout);

    QComboBox* tupleDisplayModeComboBox = new QComboBox;
    registerWidget(tupleDisplayModeComboBox, "tuple_display_mode_combo_box");
    setTupleFormLayout->addRow("Tuples displayed as: ", tupleDisplayModeComboBox);
    configureTupleDisplayModeComboBox(true);

    QSpinBox* leadingMemberCountSpinBox = new QSpinBox;
    registerWidget(leadingMemberCountSpinBox, "leading_member_count_spin_box");
    setTupleFormLayout->addRow(tr("Number leading members: "), leadingMemberCountSpinBox);
    leadingMemberCountSpinBox->setSpecialValueText(tr("Show all members"));
    leadingMemberCountSpinBox->setMinimum(-1);
    leadingMemberCountSpinBox->setMaximum(std::numeric_limits<int>::max());

    QSpinBox* trailingMemberCountSpinBox = new QSpinBox;
    registerWidget(trailingMemberCountSpinBox, "trailing_member_count_spin_box");
    setTupleFormLayout->addRow(tr("Number trailing members: "), trailingMemberCountSpinBox);
    trailingMemberCountSpinBox->setMinimum(0);
    trailingMemberCountSpinBox->setMaximum(std::numeric_limits<int>::max());

    QCheckBox* showHiddenMemberCountCheckbox = new QCheckBox(tr("Show count of hidden members"));
    registerWidget(showHiddenMemberCountCheckbox, "show_hidden_member_count_checkbox");
    setTupleVerticalLayout->addWidget(showHiddenMemberCountCheckbox);

    setTupleVerticalLayout->addStretch(1);


    // Matrix tab.

    QGroupBox* rowGroupBox = new QGroupBox(tr("Rows"));
    registerWidget(rowGroupBox, "matrix_row_group_box");
    matrixVerticalLayout->addWidget(rowGroupBox);

    QFormLayout* rowFormLayout = newFormLayout("row_form_layout");
    rowGroupBox->setLayout(rowFormLayout);

    QLabel* leadingRowsLabel = new QLabel(tr("Number Leading Rows: "));
    registerWidget(leadingRowsLabel, "leading_rows_label");

    QSpinBox* leadingRowsSpinBox = new QSpinBox;
    registerWidget(leadingRowsSpinBox, "leading_rows_spin_box");
    leadingRowsSpinBox->setSpecialValueText(tr("Show all members"));
    leadingRowsSpinBox->setMinimum(-1);
    leadingRowsSpinBox->setMaximum(std::numeric_limits<int>::max());

    rowFormLayout->addRow(leadingRowsLabel, leadingRowsSpinBox);

    QLabel* trailingRowsLabel = new QLabel(tr("Number Trailing Rows: "));
    registerWidget(trailingRowsLabel, "trailing_rows_label");

    QSpinBox* trailingRowsSpinBox = new QSpinBox;
    registerWidget(trailingRowsSpinBox, "trailing_rows_spin_box");
    trailingRowsSpinBox->setMinimum(0);
    trailingRowsSpinBox->setMaximum(std::numeric_limits<int>::max());

    rowFormLayout->addRow(trailingRowsLabel, trailingRowsSpinBox);

    QGroupBox* columnGroupBox = new QGroupBox(tr("Columns"));
    registerWidget(columnGroupBox, "matrix_column_group_box");
    matrixVerticalLayout->addWidget(columnGroupBox);

    QFormLayout* columnFormLayout = newFormLayout("column_form_layout");
    columnGroupBox->setLayout(columnFormLayout);

    QLabel* leadingColumnsLabel = new QLabel(tr("Number Leading Columns: "));
    registerWidget(leadingColumnsLabel, "leading_columns_label");

    QSpinBox* leadingColumnsSpinBox = new QSpinBox;
    registerWidget(leadingColumnsSpinBox, "leading_columns_spin_box");
    leadingColumnsSpinBox->setSpecialValueText(tr("Show all members"));
    leadingColumnsSpinBox->setMinimum(-1);
    leadingColumnsSpinBox->setMaximum(std::numeric_limits<int>::max());

    columnFormLayout->addRow(leadingColumnsLabel, leadingColumnsSpinBox);

    QLabel* trailingColumnsLabel = new QLabel(tr("Number Trailing Columns: "));
    registerWidget(trailingColumnsLabel, "trailing_columns_label");

    QSpinBox* trailingColumnsSpinBox = new QSpinBox;
    registerWidget(trailingColumnsSpinBox, "trailing_columns_spin_box");
    trailingColumnsSpinBox->setMinimum(0);
    trailingColumnsSpinBox->setMaximum(std::numeric_limits<int>::max());

    columnFormLayout->addRow(trailingColumnsLabel, trailingColumnsSpinBox);

    matrixVerticalLayout->addStretch(1);


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


bool NumericValueFormatDialog::valuesChanged() const {
    bool valuesHaveChanged = (
           inputVariableName != variableName()
        && variableNameFieldsEnabled()
        && widget<VariableNameEditorWidget>("variable_name_editor_widget")->isValid()
    );

    if (!valuesHaveChanged) {
        TriState currentUpperCase = !caseIsSet() ? TriState::UNSET : upperCase() ? TriState::TRUE : TriState::FALSE;
        valuesHaveChanged = (currentUpperCase != inputUpperCase);
    }

    if (!valuesHaveChanged) {
        QSet<ValueType> supportedDataTypes     = NumericValueFormatDialog::supportedDataTypes();
        bool            booleanSettingsVisible = supportedDataTypes.contains(ValueType::BOOLEAN);
        bool            integerSettingsVisible = supportedDataTypes.contains(ValueType::INTEGER);
        bool            realSettingsVisible    = supportedDataTypes.contains(ValueType::REAL);
        bool            complexSettingsVisible = supportedDataTypes.contains(ValueType::COMPLEX);
        bool            setSettingsVisible     = supportedDataTypes.contains(ValueType::SET);
        bool            tupleSettingsVisible   = supportedDataTypes.contains(ValueType::TUPLE);
        bool            matrixBooleanVisible   = supportedDataTypes.contains(ValueType::MATRIX_BOOLEAN);
        bool            matrixIntegerVisible   = supportedDataTypes.contains(ValueType::MATRIX_INTEGER);
        bool            matrixRealVisible      = supportedDataTypes.contains(ValueType::MATRIX_REAL);
        bool            matrixComplexVisible   = supportedDataTypes.contains(ValueType::MATRIX_COMPLEX);

        bool matrixSettingsVisible  = (
               matrixBooleanVisible
            || matrixIntegerVisible
            || matrixRealVisible
            || matrixComplexVisible
        );

        if (booleanSettingsVisible) {
            valuesHaveChanged = (
                   (inputBooleanStyles.size() != 1 && booleanStyle() != BooleanStyle::INVALID)
                || (inputBooleanStyles.size() == 1 && *inputBooleanStyles.begin() != booleanStyle())
            );
        }

        if (!valuesHaveChanged && integerSettingsVisible) {
            valuesHaveChanged = (
                   inputMantissaDigits != mantissaDigits()
                || (inputIntegerNumberStyles.size() != 1 && integerNumberStyle() != IntegerNumberStyle::INVALID)
                || (inputIntegerNumberStyles.size() == 1 && *inputIntegerNumberStyles.begin() != integerNumberStyle())
                || (inputBases.size() != 1 && base() != invalidBase)
                || (inputBases.size() == 1 && *inputBases.begin() != base())
            );
        }

        if (!valuesHaveChanged && (realSettingsVisible || complexSettingsVisible)) {
            valuesHaveChanged = (
                   inputPrecision != precision()
                || (inputRealNumberStyles.size() != 1 && realNumberStyle() != RealNumberStyle::INVALID)
                || (inputRealNumberStyles.size() == 1 && *inputRealNumberStyles.begin() != realNumberStyle())
                || inputMultiplicationFactor != multiplicationFactor()
            );
        }

        if (!valuesHaveChanged && complexSettingsVisible) {
            valuesHaveChanged = (
                   (inputImaginaryUnits.size() != 1 && imaginaryUnit() != ImaginaryUnit::INVALID)
                || (inputImaginaryUnits.size() == 1 && *inputImaginaryUnits.begin() != imaginaryUnit())
            );
        }

        if (!valuesHaveChanged && (setSettingsVisible || tupleSettingsVisible)) {
            valuesHaveChanged = (
                   inputTupleDisplayMode != tupleDisplayMode()
                || inputLeadingMemberCount != leadingMemberCount()
                || inputTrailingMemberCount != trailingMemberCount()
            );

            if (!valuesHaveChanged && showHideHiddenMemberCountIsSet()) {
                if (inputShowHiddenMemberCounts == TriState::UNSET) {
                    valuesHaveChanged = true;
                } else {
                    bool inputSet = (inputShowHiddenMemberCounts == TriState::TRUE);
                    valuesHaveChanged = (inputSet != showHiddenMemberCount());
                }
            }
        }

        if (!valuesHaveChanged && matrixSettingsVisible) {
            valuesHaveChanged = (
                   inputLeadingRowsCount != leadingRowsCount()
                || inputTrailingRowsCount != trailingRowsCount()
                || inputLeadingColumnsCount != leadingColumnsCount()
                || inputTrailingColumnsCount != trailingColumnsCount()
            );
        }
    }

    return valuesHaveChanged;
}


void NumericValueFormatDialog::configureBooleanStyleButtons(const QSet<BooleanStyle>& enabledBooleanStyles) {
    QRadioButton* lowerCaseTrueFalseRadioButton = widget<QRadioButton>("lower_case_true_false_radio_button");
    QRadioButton* titleCaseTrueFalseRadioButton = widget<QRadioButton>("title_case_true_false_radio_button");
    QRadioButton* upperCaseTrueFalseRadioButton = widget<QRadioButton>("upper_case_true_false_radio_button");
    QRadioButton* lowerCaseYesNoRadioButton     = widget<QRadioButton>("lower_case_yes_no_radio_button");
    QRadioButton* titleCaseYesNoRadioButton     = widget<QRadioButton>("title_case_yes_no_radio_button");
    QRadioButton* upperCaseYesNoRadioButton     = widget<QRadioButton>("upper_case_yes_no_radio_button");
    QRadioButton* numeric10RadioButton          = widget<QRadioButton>("numeric_1_0_radio_button");

    lowerCaseTrueFalseRadioButton->setChecked(enabledBooleanStyles.contains(BooleanStyle::LOWER_CASE_TRUE_FALSE));
    titleCaseTrueFalseRadioButton->setChecked(enabledBooleanStyles.contains(BooleanStyle::TITLE_CASE_TRUE_FALSE));
    upperCaseTrueFalseRadioButton->setChecked(enabledBooleanStyles.contains(BooleanStyle::UPPER_CASE_TRUE_FALSE));
    lowerCaseYesNoRadioButton    ->setChecked(enabledBooleanStyles.contains(BooleanStyle::LOWER_CASE_YES_NO));
    titleCaseYesNoRadioButton    ->setChecked(enabledBooleanStyles.contains(BooleanStyle::TITLE_CASE_YES_NO));
    upperCaseYesNoRadioButton    ->setChecked(enabledBooleanStyles.contains(BooleanStyle::UPPER_CASE_YES_NO));
    numeric10RadioButton         ->setChecked(enabledBooleanStyles.contains(BooleanStyle::NUMERIC_1_0));
}


void NumericValueFormatDialog::configureIntegerNumberStyleButtons(
        const QSet<NumericValueFormatDialog::IntegerNumberStyle>& enabledIntegerNumberStyles
    ) {
    QRadioButton* noPrefixSuffixStyleRadioButton = widget<QRadioButton>("integer_no_prefix_suffix_style_radio_button");
    QRadioButton* cStyleRadioButton = widget<QRadioButton>("integer_c_style_radio_button");
    QRadioButton* verilogStyleRadioButton = widget<QRadioButton>("integer_verilog_style_radio_button");
    QRadioButton* subscriptStyleRadioButton = widget<QRadioButton>("integer_subscript_style_radio_button");

    noPrefixSuffixStyleRadioButton->setChecked(enabledIntegerNumberStyles.contains(IntegerNumberStyle::NONE));
    cStyleRadioButton->setChecked(enabledIntegerNumberStyles.contains(IntegerNumberStyle::C_STYLE));
    verilogStyleRadioButton->setChecked(enabledIntegerNumberStyles.contains(IntegerNumberStyle::VERILOG_STYLE));
    subscriptStyleRadioButton->setChecked(enabledIntegerNumberStyles.contains(IntegerNumberStyle::SUBSCRIPT_STYLE));
}


void NumericValueFormatDialog::configureIntegerRadixButtons(const QSet<unsigned>& enabledRadixes) {
    QRadioButton* base2RadioButton  = widget<QRadioButton>("integer_base_2_radio_button");
    QRadioButton* base10RadioButton = widget<QRadioButton>("integer_base_10_radio_button");
    QRadioButton* base16RadioButton = widget<QRadioButton>("integer_base_16_radio_button");

    base2RadioButton->setChecked(enabledRadixes.contains(2U));
    base10RadioButton->setChecked(enabledRadixes.contains(10U));
    base16RadioButton->setChecked(enabledRadixes.contains(16U));
}


void NumericValueFormatDialog::configureRealNumberStyleButtons(
        const QSet<NumericValueFormatDialog::RealNumberStyle>& enabledRealStyles
    ) {
    QRadioButton* fixedPointStyleRadioButton         = widget<QRadioButton>("real_fixed_point_style_radio_button");
    QRadioButton* scientificStyleRadioButton         = widget<QRadioButton>("real_scientific_style_radio_button");
    QRadioButton* engineeringStyleRadioButton        = widget<QRadioButton>("real_engineering_style_radio_button");
    QRadioButton* computerScientificStyleRadioButton = widget<QRadioButton>(
        "real_computer_scientific_style_radio_button"
    );
    QRadioButton* conciseStyleRadioButton            = widget<QRadioButton>("real_concise_style_radio_button");
    QRadioButton* conciseComputerStyleRadioButton    = widget<QRadioButton>("real_concise_computer_style_radio_button");

    fixedPointStyleRadioButton->setChecked(enabledRealStyles.contains(RealNumberStyle::FLOATING_POINT));
    scientificStyleRadioButton->setChecked(enabledRealStyles.contains(RealNumberStyle::SCIENTIFIC));
    engineeringStyleRadioButton->setChecked(enabledRealStyles.contains(RealNumberStyle::ENGINEERING));
    computerScientificStyleRadioButton->setChecked(enabledRealStyles.contains(RealNumberStyle::COMPUTER_SCIENTIFIC));
    conciseStyleRadioButton->setChecked(enabledRealStyles.contains(RealNumberStyle::CONCISE));
    conciseComputerStyleRadioButton->setChecked(enabledRealStyles.contains(RealNumberStyle::COMPUTER_CONCISE));
}


void NumericValueFormatDialog::configureImaginaryUnitButtons(
        const QSet<NumericValueFormatDialog::ImaginaryUnit>& enabledImaginaryUnits
    ) {
    QRadioButton* imaginaryUnitIRadioButton = widget<QRadioButton>("complex_imaginary_unit_i_radio_button");
    QRadioButton* imaginaryUnitJRadioButton = widget<QRadioButton>("complex_imaginary_unit_j_radio_button");

    imaginaryUnitIRadioButton->setChecked(enabledImaginaryUnits.contains(ImaginaryUnit::I));
    imaginaryUnitJRadioButton->setChecked(enabledImaginaryUnits.contains(ImaginaryUnit::J));
}


void NumericValueFormatDialog::configureTupleDisplayModeComboBox(bool showInvalidOption) {
    QComboBox* tupleDisplayModeComboBox = widget<QComboBox>("tuple_display_mode_combo_box");

    tupleDisplayModeComboBox->setEditable(false);
    tupleDisplayModeComboBox->clear();

    if (showInvalidOption) {
        tupleDisplayModeComboBox->addItem(
            tr(""),
            QVariant::fromValue(static_cast<unsigned>(TupleDisplayMode::INVALID))
        );
    }

    tupleDisplayModeComboBox->addItem(
        tr("List of members"),
        QVariant::fromValue(static_cast<unsigned>(TupleDisplayMode::NORMAL))
    );
    tupleDisplayModeComboBox->addItem(
        tr("String (unquoted)"),
        QVariant::fromValue(static_cast<unsigned>(TupleDisplayMode::STRING_BARE))
    );
    tupleDisplayModeComboBox->addItem(
        tr("String in angle brackets"),
        QVariant::fromValue(static_cast<unsigned>(TupleDisplayMode::STRING_ANGLE_BRACKETS))
    );
    tupleDisplayModeComboBox->addItem(
        tr("Single quoted string"),
        QVariant::fromValue(static_cast<unsigned>(TupleDisplayMode::STRING_SINGLE_QUOTES))
    );
    tupleDisplayModeComboBox->addItem(
        tr("Double quoted string"),
        QVariant::fromValue(static_cast<unsigned>(TupleDisplayMode::STRING_DOUBLE_QUOTES))
    );
}


void NumericValueFormatDialog::updateSampleValues() {
    QSet<ValueType> supportedDataTypes = NumericValueFormatDialog::supportedDataTypes();
    QString         text;

    bool upperCase   = NumericValueFormatDialog::upperCase();
    bool caseSet     = NumericValueFormatDialog::caseIsSet();
    bool needNewline = false;

    if (supportedDataTypes.contains(ValueType::BOOLEAN)) {
        BooleanStyle currentBooleanStyle = booleanStyle();
        if (currentBooleanStyle != BooleanStyle::INVALID) {
            text = tr("%1 / %2")
                   .arg(Util::booleanToUnicodeString(true, currentBooleanStyle))
                   .arg(Util::booleanToUnicodeString(false, currentBooleanStyle));
        }

        needNewline = true;
    }

    if (supportedDataTypes.contains(ValueType::INTEGER)) {
        if (needNewline) {
            text += tr("\n");
        }

        unsigned           mantissaDigits     = NumericValueFormatDialog::mantissaDigits();
        IntegerNumberStyle integerNumberStyle = NumericValueFormatDialog::integerNumberStyle();
        unsigned           base               = NumericValueFormatDialog::base();

        if (mantissaDigits != mantissaDigitsNotSet            &&
            integerNumberStyle != IntegerNumberStyle::INVALID &&
            base != invalidBase                               &&
            caseSet                                              ) {
            text += Util::longLongIntegerToUnicodeString(
                sampleIntegerValue,
                integerNumberStyle,
                mantissaDigits,
                base,
                upperCase
            );
        }

        needNewline = true;
    }

    if (supportedDataTypes.contains(ValueType::REAL)) {
        if (needNewline) {
            text += tr("\n");
        }

        RealNumberStyle realNumberStyle      = NumericValueFormatDialog::realNumberStyle();
        unsigned        precision            = NumericValueFormatDialog::precision();
        double          multiplicationFactor = NumericValueFormatDialog::multiplicationFactor();

        if (realNumberStyle != RealNumberStyle::INVALID        &&
            precision != precisionNotSet                       &&
            multiplicationFactor != multiplicationFactorNotSet &&
            caseSet                                               ) {
            double  sampleValue = multiplicationFactor * sampleRealValue;
            text += Util::longDoubleToUnicodeString(sampleValue, realNumberStyle, precision, upperCase);
        }

        needNewline = true;
    }

    if (supportedDataTypes.contains(ValueType::COMPLEX)) {
        if (needNewline) {
            text += tr("\n");
        }

        RealNumberStyle realNumberStyle      = NumericValueFormatDialog::realNumberStyle();
        unsigned        precision            = NumericValueFormatDialog::precision();
        double          multiplicationFactor = NumericValueFormatDialog::multiplicationFactor();
        ImaginaryUnit   imaginaryUnit        = NumericValueFormatDialog::imaginaryUnit();

        if (realNumberStyle != RealNumberStyle::INVALID        &&
            precision != precisionNotSet                       &&
            multiplicationFactor != multiplicationFactorNotSet &&
            imaginaryUnit != ImaginaryUnit::INVALID            &&
            caseSet                                               ) {
            double  realValue           = multiplicationFactor * sampleRealValue;
            double  imaginaryValue      = multiplicationFactor * sampleImaginaryValue;
            QString imaginaryUnitString = imaginaryUnit == ImaginaryUnit::I ? tr("i") : tr("j");

            text += tr("%1+%2%3")
                    .arg(Util::longDoubleToUnicodeString(realValue, realNumberStyle, precision, upperCase))
                    .arg(Util::longDoubleToUnicodeString(imaginaryValue, realNumberStyle, precision, upperCase))
                    .arg(imaginaryUnitString);
        }
    }

    QLabel* sampleValuesLabel = widget<QLabel>("sample_values_label");
    sampleValuesLabel->setText(text);
}

