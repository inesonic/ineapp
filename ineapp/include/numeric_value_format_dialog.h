/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref NumericValueFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef NUMERIC_VALUE_FORMAT_DIALOG_H
#define NUMERIC_VALUE_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>
#include <QFont>

#include <eqt_programmatic_dialog.h>

#include <util_string.h>

#include <model_intrinsic_types.h>
#include <model_variant.h>

#include <ld_variable_name.h>
#include <ld_numeric_data_type_format.h>
#include <ld_boolean_data_type_format.h>
#include <ld_integer_data_type_format.h>
#include <ld_real_data_type_format.h>
#include <ld_complex_data_type_format.h>
#include <ld_list_data_type_format.h>
#include <ld_tuple_data_type_format.h>
#include <ld_special_characters.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;

namespace EQt {
    class ToolButton;
    class ToolButtonArray;
}

/**
 * Dialog used to adjust how numeric values will be displayed.  This dialog also optionally supports entering variable
 * names.
 */
class APP_PUBLIC_API NumericValueFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Type representing an enumeration of supported data types.
         */
        typedef Model::ValueType ValueType;

        /**
         * Type representing the boolean display style.
         */
        typedef Ld::BooleanDataTypeFormat::BooleanStyle BooleanStyle;

        /**
         * Type representing the integer display style.
         */
        typedef Ld::IntegerDataTypeFormat::IntegerNumberStyle IntegerNumberStyle;

        /**
         * Type representing the real/complex number display style.
         */
        typedef Util::RealNumberStyle RealNumberStyle;

        /**
         * Type representing the complex imaginary unit.
         */
        typedef Ld::ComplexDataTypeFormat::ImaginaryUnit ImaginaryUnit;

        /**
         * Type representing the possible ways to present tuples.
         */
        typedef Ld::TupleDataTypeFormat::DisplayMode TupleDisplayMode;

        /**
         * Value used to indicate an invalid integer base setting.
         */
        static const unsigned invalidBase;

        /**
         * Value used to indicate that the mantissa width should be ignored.
         */
        static const unsigned ignoreMantissaDigits;

        /**
         * Value used to indicate that the mantissa width value has not been set.
         */
        static const unsigned mantissaDigitsNotSet;

        /**
         * Value used to indicate that the number precision should be inferred.
         */
        static const unsigned inferPrecision;

        /**
         * Value used to indicate that the number precision has not been set.
         */
        static const unsigned precisionNotSet;

        /**
         * Value used to indicate that the multiplication factor has not been set.
         */
        static const double multiplicationFactorNotSet;

        /**
         * Value used to indicate that all set/tuple/list contents should be displayed.
         */
        static const unsigned long showAllContents;

        /**
         * Value that indicates an invalid set/tuple/list member count.
         */
        static const unsigned long invalidMemberCount;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        NumericValueFormatDialog(QWidget* parent = Q_NULLPTR);

        ~NumericValueFormatDialog() override;

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
         * Method you can use to enable or disable the auto-apply function.
         *
         * \param[in] nowAutoApply If true, auto-apply will be enabled.  If false, auto-apply will be disabled.
         */
        void setAutoApplyEnabled(bool nowAutoApply = true);

        /**
         * Method you can use to disable or enable the auto-apply function.
         *
         * \param[in] nowAutoApplyDisabled If true, auto-apply will be disabled.  If false, auto-apply will be enabled.
         */
        void setAutoApplyDisabled(bool nowAutoApplyDisabled = true);

        /**
         * Method you can use to determine if auto-apply is enabled.
         *
         * \return Returns true if auto-apply is enabled.  Returns false if auto-apply is disabled.
         */
        bool autoApplyEnabled() const;

        /**
         * Method you can use to determine if auto-apply is disabled.
         *
         * \return Returns true if auto-apply is disabled.  Returns false if auto-apply is enabled.
         */
        bool autoApplyDisabled() const;

        /**
         * Method you can use to specify if the apply button should be visible or hidden.
         *
         * \param[in] nowVisible If true, the apply button should be shown.  If false, the apply button should be
         *                       hidden.
         */
        void setApplyButtonVisible(bool nowVisible = true);

        /**
         * Method you can use to specify if the Apply button should be hidden or visible.
         *
         * \param[in] nowHidden If true, the apply button should be hidden.  If false, the apply button should be
         *                      visible.
         */
        void setApplyButtonHidden(bool nowHidden = true);

        /**
         * Method you can use to determine if the Apply button is visible.
         *
         * \return Returns true if the Apply button is visible.  Returns false if the Apply button is hidden.
         */
        bool applyButtonVisible() const;

        /**
         * Method you can use to determine if the Apply button is hidden.
         *
         * \return Returns true if the Apply button is hidden.  Returns false if the Apply button is visible.
         */
        bool applyButtonHidden() const;

        /**
         * Method you can use to determine if the variable name fields have been enabled or disabled.
         *
         * \return Returns true if the variable name fields are enabled.  Returns false if the variable name fields are
         *         disabled.
         */
        bool variableNameFieldsEnabled() const;

        /**
         * Method you can use to determine if the variable name fields have been disabled or enabled.
         *
         * \return Returns true if the variable name fields are disabled.  Returns false if the variable name fields
         *         are enabled.
         */
        bool variableNameFieldsDisbled() const;

        /**
         * Method you can use to determine the currently supported data-types.
         *
         * \return Returns a set of currently supported data-types.  Fields will be enabled or disabled depending on
         *         what types are reported.
         */
        QSet<ValueType> supportedDataTypes() const;

        /**
         * Method you can use to determine the input variable name.
         *
         * \return Returns the variable name entered by the user.
         */
        Ld::VariableName variableName() const;

        /**
         * Method youc anuse to determine the requested boolean style.
         *
         * \return Returns the requested boolean style.  A value of
         *         Ld::BooleanDataTypeFormat::BooleanStyle::INVALID is returned if no boolean style has been
         *         selected.
         */
        BooleanStyle booleanStyle() const;

        /**
         * Method you can use to determine the requested number of mantissa digits.
         *
         * \return Returns the requested number of mantissa digits.
         */
        unsigned mantissaDigits() const;

        /**
         * Method you can use to determine if alphabetic characters should be presented in upper case or lower case.
         *
         * \return Returns true if alphabetic characters should be presented in upper case.  Returns false if
         *         alphabetic characters should be presented in lower case.
         */
        bool upperCase() const;

        /**
         * Method you can use to determine if alphabetic characters should be presented in lower case or upper case.
         *
         * \return Returns true if alphabetic characters should be presented in lower case.  Returns false if
         *         alphabetic characters should be presented in upper case.
         */
        bool lowerCase() const;

        /**
         * Method you can use to determine if the user has selected a case setting.
         *
         * \return Returns true if the user has selected a case setting.  Returns false if no case setting has been
         *         set by the user.
         */
        bool caseIsSet() const;

        /**
         * Method you can use to determine the currently selected integer display style.
         *
         * \return Returns the integer display style.  A value of NumericValueFormatDialog::IntegerStyle::INVALID is
         *         returned if the integer number style is not set.
         */
        IntegerNumberStyle integerNumberStyle() const;

        /**
         * Method you can use to determine the currently selected integer base.
         *
         * \return Returns the currently selected integer base.  A value of \ref NumericValueFormatDialog::invalidBase
         *         is returned if there is no singlular integer base selected.
         */
        unsigned base() const;

        /**
         * Method you can use to determine the currently selected real/complex number notation.
         *
         * \return Returns the currently selected real number style.  A value of
         *         NumericValueFormatDialog::RealNumberStyle::INVALID will be returned if there is no singular real
         *         number style selected.
         */
        RealNumberStyle realNumberStyle() const;

        /**
         * Method you can use to determine the currently selected real number precision.
         *
         * \return Returns the real number precision setting.
         */
        unsigned precision() const;

        /**
         * Method you can use to determine the currently selected multiplication factor.
         *
         * \return Returns the current multiplication factor.  A value of
         *         \ref NumericValueFormatDialog::multiplicationFactorNotSet is returned if no multiplication factor
         *         has been assigned.
         */
        double multiplicationFactor() const;

        /**
         * Method you can use to determine the currently selected imaginary unit.
         *
         * \return Returns the currently selected imaginary unit. A value of
         *         NumericValueFormatDialog::ImaginaryUnit::INVALID is returned if no simgular imaginary unit is
         *         selected.
         */
        ImaginaryUnit imaginaryUnit() const;

        /**
         * Method you can use to determine the currently selected tuple display format.
         *
         * \return Returns the current selected tuple display format.  A value of
         *         NumericValueFormatDialog::TupleDisplayFormat::INVALID is returned if this value is unset.
         */
        TupleDisplayMode tupleDisplayMode() const;

        /**
         * Method you can use to determine the currently selected set/tuple/list leading member count.
         *
         * \return Returns the currently selected leading member count.  A value of
         *         \ref NumericValueFormatDialog::invalidMemberCount is returned if this value is unset.
         */
        unsigned long leadingMemberCount() const;

        /**
         * Method you can use to determine the currently selected trailing member count.
         *
         * \return Returns the currently selected trailing member count.  A value of
         *         \ref NumericValueFormatDialog::invalidMemberCount is returned if this value is unset.
         */
        unsigned long trailingMemberCount() const;

        /**
         * Method you can use to determine if we should show or hide the count of hidden set/tuple/list members.
         *
         * \return Returns true if we should show the hidden member count.  Returns false if we should not show the
         *         hidden member count.
         */
        bool showHiddenMemberCount() const;

        /**
         * Method you can use to determine if we should hide or show the count of hidden set/tuple/list members.
         *
         * \return Returns true if we should hide the hidden member count.  Returns false if we should show the
         *         hidden member count.
         */
        bool hideHiddenMemberCount() const;

        /**
         * Method you can use to determine if the user as specified whether to hide or show the hidden member
         * count.
         *
         * \return Returns true if the show/hide hidden member count state has been set.
         */
        bool showHideHiddenMemberCountIsSet() const;

        /**
         * Method you can use to determine the currently selected leading rows count.
         *
         * \return Returns the leading rows count.  A value of \ref NumericValueFormatDialog::invalidMemberCount is
         *         returned if this value is unset.
         */
        unsigned long leadingRowsCount() const;

        /**
         * Method you can use to determine the currently selected trailing rows count.
         *
         * \return Returns the trailing rows count.  A value of \ref NumericValueFormatDialog::invalidMemberCount is
         *         returned if this value is unset.
         */
        unsigned long trailingRowsCount() const;

        /**
         * Method you can use to determine the currently selected leading columns count.
         *
         * \return Returns the leading columns count.  A value of \ref NumericValueFormatDialog::invalidMemberCount is
         *         returned if this value is unset.
         */
        unsigned long leadingColumnsCount() const;

        /**
         * Method you can use to determine the currently selected trailing columns count.
         *
         * \return Returns the trailing columns count.  A value of \ref NumericValueFormatDialog::invalidMemberCount is
         *         returned if this value is unset.
         */
        unsigned long trailingColumnsCount() const;

        /**
         * Method you can use to determine the font used for the sample window.
         *
         * \return Returns the font used for the sample window.
         */
        const QFont& sampleFont() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:
        /**
         * Signal that is emitted when the Apply button is clicked on Windows or when any update occurs on MacOS.
         *
         * \param[out] thisDialog A pointer to this dialog.  You can use this parameter to obtain the current values.
         */
        void apply(NumericValueFormatDialog* thisDialog);

    public slots:
        /**
         * Slot you can use to enable the variable name fields.
         *
         * \param[in] nowEnabled If true, the variable name fields will be enabled.  If false, the variable name fields
         *                       will be disabled.
         */
        void setVariableNameFieldsEnabled(bool nowEnabled = true);

        /**
         * Slot you can use to disable the variable name fields.
         *
         * \param[in] nowDisabled If true, the variable name fields will be disabled.  If false, the variable name
         *                        fields will be enabled.
         */
        void setVariableNameFieldsDisabled(bool nowDisabled = true);

        /**
         * Slot you can use to set the supported data types.
         *
         * \param[in] supportedDataTypes The set of supported data types.
         */
        void setSupportedDataTypes(const QSet<ValueType>& supportedDataTypes);

        /**
         * Slot you can use to set the variable name.
         *
         * \param[in] newVariableName The new variable name to show in the dialog.
         */
        void setVariableName(const Ld::VariableName& newVariableName);

        /**
         * Slot you can use to set the currently selected boolean presentation style.
         *
         * \param[in] newBooleanPresentationStyles A set of currently selected boolean presentation styles.
         */
        void setBooleanStyle(const QSet<BooleanStyle>& newBooleanPresentationStyles);

        /**
         * Slot you can use to set the number of mantissa digits.
         *
         * \param[in] newMantissaDigits The new number of mantissa digits.
         */
        void setMantissaDigits(unsigned newMantissaDigits);

        /**
         * Slot you can use to mark the number of mantissa digits as unset.
         */
        void setMantissaDigitsUnset();

        /**
         * Slot you can use to indicate that the number of mantissa digits should be ignored.
         */
        void setMantissaDigitsIgnored();

        /**
         * Slot you can use to indicate upper case or lower case alphabetic letters in numbers.
         *
         * \param[in] nowUpperCase If true, numbers will be presented using upper-case letters.  If false, numbers will
         *                         be presented using lower-case letters.
         */
        void setUpperCase(bool nowUpperCase = true);

        /**
         * Slot you can use to indicate lower case or upper case alphabetic letters in numbers.
         *
         * \param[in] nowLowerCase If true, numbers will be presented using lower-case letters.  If false, numbers will
         *                         be presented using upper-case letters.
         */
        void setLowerCase(bool nowLowerCase = true);

        /**
         * Slot you can use to indicate that the case has not been set.
         */
        void setCaseUnset();

        /**
         * Slot you can use to set the current integer number styles.
         *
         * \param[in] newIntegerNumberStyles A set containing the currently selected integer number styles.
         */
        void setIntegerNumberStyles(const QSet<IntegerNumberStyle>& newIntegerNumberStyles);

        /**
         * Slot you can use to set the currently selected integer base settings.
         *
         * \param[in] newBases A set containing the currently selected integer base values.
         */
        void setBases(const QSet<unsigned>& newBases);

        /**
         * Slot you can use to set the currently selected real number styles.
         *
         * \param[in] newRealNumberStyles A set containing the currently selected real number styles.
         */
        void setRealNumberStyles(const QSet<RealNumberStyle>& newRealNumberStyles);

        /**
         * Slot you can use to set the current precision setting
         *
         * \param[in] newPrecision The new precision setting.
         */
        void setPrecision(unsigned newPrecision);

        /**
         * Slot you can use to unset the precision.
         */
        void setPrecisionUnset();

        /**
         * Slot you can use to indicate that the precision should be inferred.
         */
        void setPrecisionInferred();

        /**
         * Slot you can use to set the current multiplication factor.
         *
         * \param[in] newMultiplicationFactor The new multiplication factor.
         */
        void setMultiplicationFactor(double newMultiplicationFactor);

        /**
         * Slot you can use to indicate that the multiplication factor should be unset.
         */
        void setMultiplicationFactorUnset();

        /**
         * Slot you can use to select the currently displayed imaginary units.
         *
         * \param[in] newImaginaryUnits A set indicating the imaginary units to be set.
         */
        void setImaginaryUnits(const QSet<ImaginaryUnit>& newImaginaryUnits);

        /**
         * Slot you can use to set the tuple display mode.
         *
         * \param[in] newDisplayMode The new tuple display mode.  A value of
         *                           NumericValueFormatDialog::TupleDisplayMode::INVALID will cause this field to be
         *                           shown as unset.
         */
        void setTupleDisplayMode(TupleDisplayMode newDisplayMode);

        /**
         * Slot you can use to set the tuple display mode as unset.
         */
        void setTupleDisplayModeUnset();

        /**
         * Slot you can use to set the leading member count.
         *
         * \param[in] newLeadingMemberCount The new leading member count.  A value of
         *                                  \ref NumericValueFormatDialog::invalidMemberCount will cause this field to
         *                                  be shown as "unset."  A value of
         *                                  \ref NumericValueFormatDialog::showAllContents will cause a "show all"
         *                                  special value to be displayed.
         */
        void setLeadingMemberCount(unsigned long newLeadingMemberCount);

        /**
         * Slot you can use to clear the leading member count.
         */
        void setLeadingMemberCountUnset();

        /**
         * Slot you can use to set the trailing member count.
         *
         * \param[in] newTrailingMemberCount The new trailing member count.  A value of
         *                                   \ref NumericValueFormatDialog::invalidMemberCount will cause this field to
         *                                   be shown as "unset."  A value of
         *                                   \ref NumericValueFormatDialog::showAllContents will cause a "show all"
         *                                   special value to be displayed.
         */
        void setTrailingMemberCount(unsigned long newTrailingMemberCount);

        /**
         * Slot you can use to clear the trailing member count.
         */
        void setTrailingMemberCountUnset();

        /**
         * Slot you can use to set or clear the show hidden member count checkbox.
         *
         * \param[in] nowShowHiddenMemberCounts If true, the "show hidden members" checkbox will be checked.  If false,
         *                                      the "show hidden members" checkbox will be unchecked.
         */
        void setShowHiddenMemberCounts(bool nowShowHiddenMemberCounts = true);

        /**
         * Slot you can use to clear or set the show hidden member count checkbox.
         *
         * \param[in] nowHideHiddenMemberCounts If true, the "show hidden members" checkbox will be unchecked.  If
         *                                      false, the "show hidden members" checkbox will be checked.
         */
        void setHideHiddenMemberCounts(bool nowHideHiddenMemberCounts = true);

        /**
         * Slot you can use to indicate that the show/hide hidden member count checkbox is indeterminate.
         */
        void setShowHideHiddenMemberCountsUnset();

        /**
         * Slot you can use to set the leading rows count.
         *
         * \param[in] newLeadingRowsCount The new leading rows count.  A value of
         *                                \ref NumericValueFormatDialog::invalidMemberCount will cause this field to
         *                                be shown as "unset."  A value of
         *                                \ref NumericValueFormatDialog::showAllContents will cause a "show all"
         *                                special value to be displayed.
         */
        void setLeadingRowsCount(unsigned long newLeadingRowsCount);

        /**
         * Slot you can use to clear the leading rows count.
         */
        void setLeadingRowsCountUnset();

        /**
         * Slot you can use to set the trailing rows count.
         *
         * \param[in] newTrailingRowsCount The new trailing rows count.  A value of
         *                                \ref NumericValueFormatDialog::invalidMemberCount will cause this field to
         *                                be shown as "unset."  A value of
         *                                \ref NumericValueFormatDialog::showAllContents will cause a "show all"
         *                                special value to be displayed.
         */
        void setTrailingRowsCount(unsigned long newTrailingRowsCount);

        /**
         * Slot you can use to clear the trailing rows count.
         */
        void setTrailingRowsCountUnset();

        /**
         * Slot you can use to set the leading columns count.
         *
         * \param[in] newLeadingColumnsCount The new leading columns count.  A value of
         *                                   \ref NumericValueFormatDialog::invalidMemberCount will cause this field
         *                                   to be shown as "unset."  A value of
         *                                   \ref NumericValueFormatDialog::showAllContents will cause a "show all"
         *                                   special value to be displayed.
         */
        void setLeadingColumnsCount(unsigned long newLeadingColumnsCount);

        /**
         * Slot you can use to clear the leading columns count.
         */
        void setLeadingColumnsCountUnset();

        /**
         * Slot you can use to set the trailing columns count.
         *
         * \param[in] newTrailingColumnsCount The new trailing columns count.  A value of
         *                                    \ref NumericValueFormatDialog::invalidMemberCount will cause this field
         *                                    to be shown as "unset."  A value of
         *                                    \ref NumericValueFormatDialog::showAllContents will cause a "show all"
         *                                    special value to be displayed.
         */
        void setTrailingColumnsCount(unsigned long newTrailingColumnsCount);

        /**
         * Slot you can use to clear the trailing columns count.
         */
        void setTrailingColumnsCountUnset();

        /**
         * Slot you can use to specify the font used for the sample text.
         *
         * \param[in] newFont The new sample font.
         */
        void setSampleFont(const QFont& newFont);

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered when the variable name is changed by the user.
         *
         * \param[in] newText1 The newly entered text1.
         *
         * \param[in] newText2 The newly entered text2.
         */
        void variableNameEdited(const QString& newText1, const QString& newText2);

        /**
         * Slot that is triggered when the width spin box is changed.
         *
         * \param[in] newValue The new width spin-box value.
         */
        void widthSpinBoxChanged(int newValue);

        /**
         * Slot that is triggered when the precision spin box is changed.
         *
         * \param[in] newValue The new width spin-box value.
         */
        void precisionSpinBoxChanged(int newValue);

        /**
         * Slot that is triggered when the integer upper-case check box is clicked.
         *
         * \param[in] nowChecked The new button state.
         */
        void integerUpperCaseCheckBoxClicked(bool nowChecked);

        /**
         * Slot that is triggered when the real upper-case check box is clicked.
         *
         * \param[in] nowChecked The new button state.
         */
        void realUpperCaseCheckBoxClicked(bool nowChecked);

        /**
         * Slot that is triggered when the no prefix/suffix radio button is clicked.
         */
        void noPrefixSuffixStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the C style radio button is clicked.
         */
        void cStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the Verilog style radio button is clicked.
         */
        void verilogStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the subscript style radio button is clicked.
         */
        void subscriptStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the base 2 radix button is clicked.
         */
        void base2RadioButtonClicked();

        /**
         * Slot that is triggered when the base 10 radix button is clicked.
         */
        void base10RadioButtonClicked();

        /**
         * Slot that is triggered when the base 16 radix button is clicked.
         */
        void base16RadioButtonClicked();

        /**
         * Slot that is triggered when the fixed-point style radio button is clicked.
         */
        void fixedPointStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the scientific style radio button is clicked.
         */
        void scientificStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the engineering style radio button is clicked.
         */
        void engineeringStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the computer scientific style radio button is clicked.
         */
        void computerScientificStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the concise style radio button is clicked.
         */
        void conciseStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the concise computer style radio button is clicked.
         */
        void conciseComputerStyleRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the "i" imaginary unit button.
         */
        void imaginaryUnitIRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the "j" imaginary unit button.
         */
        void imaginaryUnitJRadioButtonClicked();

        /**
         * Slot that is triggered when the user edits the multiplier line editor.
         *
         * \param[in] newMultiplierText The new multiplier text.
         */
        void multiplierEdited(const QString& newMultiplierText);

        /**
         * Slot that is triggered when the user clicks the lower case true/false radio button.
         */
        void lowerCaseTrueFalseRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the title case true/false radio button.
         */
        void titleCaseTrueFalseRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the upper case true/false radio button.
         */
        void upperCaseTrueFalseRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the lower case yes/no radio button.
         */
        void lowerCaseYesNoRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the title case yes/no radio button.
         */
        void titleCaseYesNoRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the upper case yes/no radio button.
         */
        void upperCaseYesNoRadioButtonClicked();

        /**
         * Slot that is triggered when the user clicks the numeric 1/0 radio button.
         */
        void numeric10RadioButtonClicked();

        /**
         * Slot that is triggered when the tuple display mode is changed.
         *
         * \param[in] index The zero based index of the newly selected entry.
         */
        void tupleDisplayModeChanged(int index);

        /**
         * Slot that is triggered when the leading member count spin box is changed.
         *
         * \param[in] newValue The newly selected value.
         */
        void leadingMemberCountChanged(int newValue);

        /**
         * Slot that is triggered when the trailing member count spin box is changed.
         *
         * \param[in] newValue The newly selected value.
         */
        void trailingMemberCountChanged(int newValue);

        /**
         * Slot that is triggered when the show/hide hidden member count checkbox is changed.
         *
         * \param[in] newState The newly selected checkbox state.
         */
        void showHiddenMemberCountCheckboxClicked(int newState);

        /**
         * Slot that is triggered when the number of leading rows is changed.
         *
         * \param[in] newValue The newly selected value.
         */
        void leadingRowsCountChanged(int newValue);

        /**
         * Slot that is triggered when the number of trailing rows is changed.
         *
         * \param[in] newValue The newly selected value.
         */
        void trailingRowsCountChanged(int newValue);

        /**
         * Slot that is triggered when the number of leading columns is changed.
         *
         * \param[in] newValue The newly selected value.
         */
        void leadingColumnsCountChanged(int newValue);

        /**
         * Slot that is triggered when the number of trailing columns is changed.
         *
         * \param[in] newValue The newly selected value.
         */
        void trailingColumnsCountChanged(int newValue);

        /**
         * Slot that is triggered when the apply button is clicked.
         */
        void applyClicked();

        /**
         * Slot that is triggered when a value changes.  The slot checks for updates and adjusts the dialog's OK and
         * Apply buttons.
         */
        void updateOkApplyButtons();

    private:
        /**
         * The default sample integer value.
         */
        static constexpr long long sampleIntegerValue = 123456789LL;

        /**
         * The default sample real value.
         */
        static constexpr Model::Real sampleRealValue = Model::Real(1.23456789E4L);

        /**
         * The default sample imaginary value.
         */
        static constexpr Model::Real sampleImaginaryValue = Model::Real(0.000987654321E3);

        /**
         * Enumeration indicating tri-state values.
         */
        enum class TriState {
            /**
             * Indicates a true value.
             */
            TRUE,

            /**
             * Indicates a false value.
             */
            FALSE,

            /**
             * Indicates an unset value.
             */
            UNSET
        };

        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Method that is called to check if values are different from the input values.
         *
         * \return Returns true if one or more values are different.  Returns false if all values match the inputs.
         */
        bool valuesChanged() const;

        /**
         * Method that sets the boolean style buttons from a set of possible values.
         *
         * \param[in] enabledBooleanStyles The set of enabled boolean styles.
         */
        void configureBooleanStyleButtons(const QSet<BooleanStyle>& enabledBooleanStyles);

        /**
         * Method that sets the integer number style from a set of possible values.
         *
         * \param[in] enabledIntegerNumberStyles The set of integer number styles to enable.
         */
        void configureIntegerNumberStyleButtons(const QSet<IntegerNumberStyle>& enabledIntegerNumberStyles);

        /**
         * Method that sets the integer radix from a set of possible values.
         *
         * \param[in] enabledRadixes The set of integer radixes to enable.
         */
        void configureIntegerRadixButtons(const QSet<unsigned>& enabledRadixes);

        /**
         * Method that sets the real number style from a set of possible values.
         *
         * \param[in] enabledRealNumberStyles The set real number styles to enable.
         */
        void configureRealNumberStyleButtons(const QSet<RealNumberStyle>& enabledRealNumberStyles);

        /**
         * Method that sets the imaginary unit from a set of possible values.
         *
         * \param[in] enabledImaginaryUnits The set imaginary units to enable.
         */
        void configureImaginaryUnitButtons(const QSet<ImaginaryUnit>& enabledImaginaryUnits);

        /**
         * Method that configures the tuple display mode combo box.
         *
         * \param[in] showInvalidOption If true, an option will be included for the invalid value setting.  If false,
         *                              an option for the invalid value will be omitted.
         */
        void configureTupleDisplayModeComboBox(bool showInvalidOption);

        /**
         * Method that updates the sample values label contents.
         */
        void updateSampleValues();

        /**
         * The set of data types we are supporting in this dialog.
         */
        QSet<ValueType> currentSupportedDataTypes;

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

        /**
         * The input variable name.
         */
        Ld::VariableName inputVariableName;

        /**
         * The input number of mantissa digits.
         */
        unsigned inputMantissaDigits;

        /**
         * The input set of selected boolean styles.
         */
        QSet<BooleanStyle> inputBooleanStyles;

        /**
         * The input upper case setting.
         */
        TriState inputUpperCase;

        /**
         * The collection of input integer number style values.
         */
        QSet<IntegerNumberStyle> inputIntegerNumberStyles;

        /**
         * The collection of input base settings.
         */
        QSet<unsigned> inputBases;

        /**
         * The collection of real styles.
         */
        QSet<RealNumberStyle> inputRealNumberStyles;

        /**
         * The input precision value.
         */
        unsigned inputPrecision;

        /**
         * The input multiplier.
         */
        double inputMultiplicationFactor;

        /**
         * The input imaginary units.
         */
        QSet<ImaginaryUnit> inputImaginaryUnits;

        /**
         * The input leading member count.
         */
        unsigned long inputLeadingMemberCount;

        /**
         * The input tuple display mode.
         */
        TupleDisplayMode inputTupleDisplayMode;

        /**
         * The input trailing member count.
         */
        unsigned long inputTrailingMemberCount;

        /**
         * The show/hide hidden member counts state.
         */
        TriState inputShowHiddenMemberCounts;

        /**
         * The input leading rows count.
         */
        unsigned long inputLeadingRowsCount;

        /**
         * The input trailing rows count.
         */
        unsigned long inputTrailingRowsCount;

        /**
         * The input leading columns count.
         */
        unsigned long inputLeadingColumnsCount;

        /**
         * The input trailing columns count.
         */
        unsigned long inputTrailingColumnsCount;
};

#endif
