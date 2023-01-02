/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ListFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LIST_FORMAT_DIALOG_H
#define LIST_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>
#include <QColor>

#include <eqt_programmatic_dialog.h>

#include <ld_font_format.h>
#include <ld_justified_block_format.h>
#include <ld_ordered_list_paragraph_format.h>
#include <ld_unordered_list_paragraph_format.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;

class CharacterFormatDialog;

namespace Ld {
    class PageFormat;
};

/**
 * Dialog used to adjust one or more block format instances.
 */
class APP_PUBLIC_API ListFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Enumeration used for tri-stated checkbox values.
         */
        enum class CheckState {
            /**
             * Indicated a checked state.
             */
            CHECKED = Qt::Checked,

            /**
             * Indicates an unchecked state.
             */
            UNCHECKED = Qt::Unchecked,

            /**
             * Indicates a partially checked or tri-state value.
             */
            PARTIALLY_CHECKED = Qt::PartiallyChecked
        };

        /**
         * Enumeration of supported number styles.
         */
        typedef Ld::OrderedListParagraphFormat::Style NumberStyle;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        ListFormatDialog(QWidget* parent = Q_NULLPTR);

        ~ListFormatDialog() override;

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
         * Method you can call to determine the state of the show bullet or number check box.
         *
         * \return Returns the current state of the show bullet or number check box.
         */
        CheckState showBulletOrNumberCheckState() const;

        /**
         * Method you can use to determine if the show bullet or number check box is enabled.
         *
         * \return Returns true if the checkbox is enabled.  Returns false if the check box is disabled.
         */
        bool showBulletOrNumberCheckBoxEnabled() const;

        /**
         * Method you can use to determine if the show bullet or number check box is disabled.
         *
         * \return Returns true if the checkbox is disabled.  Returns false if the check box is enabled.
         */
        bool showBulletOrNumberCheckBoxDisabled() const;

        /**
         * Method you can use to determine the selected bullet style.
         *
         * \return Selected bullet style.  An empty string is returned if no bullet style is selected.
         */
        QString selectedBulletStyle() const;

        /**
         * Method you can use to determine the selected number style.
         *
         * \return Returns the selected number style.  The value ListFormatDialog::NumberStyle::INVALID is returned
         *         if no number style is selected.
         */
        NumberStyle selectedNumberStyle() const;

        /**
         * Method you can use to determine the selected starting number value.
         *
         * \return Returns the selected starting number value.
         */
        unsigned startingNumber() const;

        /**
         * Method you can use to determine if the starting number has been set.
         *
         * \return Returns true if the starting number value is set.  Returns false if the starting number value is
         *         not set.
         */
        bool isStartingNumberSet() const;

        /**
         * Method you can use to determine if the starting number has not been set.
         *
         * \return Returns true if the starting number value is not set.  Returns false if the starting number value is
         *         set.
         */
        bool isStartingNumberCleared() const;

        /**
         * Method you can use to determine the currently selected prefix string.
         *
         * \return Returns the current prefix string.
         */
        QString prefixString() const;

        /**
         * Method you can use to determine if the user changed the prefix string.
         *
         * \return Returns true if the user touched the prefix string.  Returns false if the user has not touched the
         *         prefix string.
         */
        bool isPrefixStringSet();

        /**
         * Method you can use to determine if the user has not changed the prefix string.
         *
         * \return Returns true if the user has not touched the prefix string.  Returns false if the user has touched
         *         the prefix string.
         */
        bool isPrefixStringCleared();

        /**
         * Method you can use to determine the currently selected suffix string.
         *
         * \return Returns the current suffix string.
         */
        QString suffixString() const;

        /**
         * Method you can use to determine if the user changed the suffix string.
         *
         * \return Returns true if the user touched the suffix string.  Returns false if the user has not touched the
         *         suffix string.
         */
        bool isSuffixStringSet();

        /**
         * Method you can use to determine if the user has not changed the suffix string.
         *
         * \return Returns true if the user has not touched the suffix string.  Returns false if the user has touched
         *         the suffix string.
         */
        bool isSuffixStringCleared();

        /**
         * Method you can use to determine the font family selected by the user.
         *
         * \return Returns the name of the selected font family.  An empty string is returned if no font family is
         *         selected or if multiple font families are selected.
         */
        QString fontFamily() const;

        /**
         * Method you can use to determine the font size selected by the user.
         *
         * \return Returns the name of the selected font size.  Returns 0 if no font size is selected.
         */
        unsigned fontSize() const;

        /**
         * Method you can use to determine the font weight selected by the user.
         *
         * \return Returns the font weight currently selected by the user.  An invalid font weight is returned if
         *         either no font weights are selected or multiple font weights are selected.
         */
        Ld::FontFormat::Weight fontWeight() const;

        /**
         * Method you can use to determine if italics are enabled.
         *
         * \return Returns true if italics are enabled.  Returns false if italics are disabled.
         */
        bool italicsEnabled() const;

        /**
         * Method you can use to determine if italics are disabled.
         *
         * \return Returns true if italics are disabled.  Returns false if italics are enabled.
         */
        bool italicsDisabled() const;

        /**
         * Method you can use to determine if italics are currently both enabled and disabled.
         *
         * \return Returns true if italics are both enabled and disabled.
         */
        bool italicsBothEnabledAndDisabled() const;

        /**
         * Method you can use to determine if underline are enabled.
         *
         * \return Returns true if underline are enabled.  Returns false if underline are disabled.
         */
        bool underlineEnabled() const;

        /**
         * Method you can use to determine if underline are disabled.
         *
         * \return Returns true if underline are disabled.  Returns false if underline are enabled.
         */
        bool underlineDisabled() const;

        /**
         * Method you can use to determine if underline is currently both enabled and disabled.
         *
         * \return Returns true if underline is both enabled and disabled.
         */
        bool underlineBothEnabledAndDisabled() const;

        /**
         * Method you can use to determine if overline are enabled.
         *
         * \return Returns true if overline are enabled.  Returns false if overline are disabled.
         */
        bool overlineEnabled() const;

        /**
         * Method you can use to determine if overline are disabled.
         *
         * \return Returns true if overline are disabled.  Returns false if overline are enabled.
         */
        bool overlineDisabled() const;

        /**
         * Method you can use to determine if overline is currently both enabled and disabled.
         *
         * \return Returns true if overline is both enabled and disabled.
         */
        bool overlineBothEnabledAndDisabled() const;

        /**
         * Method you can use to determine if strikethrough are enabled.
         *
         * \return Returns true if strikethrough are enabled.  Returns false if strikethrough are disabled.
         */
        bool strikethroughEnabled() const;

        /**
         * Method you can use to determine if strikethrough are disabled.
         *
         * \return Returns true if strikethrough are disabled.  Returns false if strikethrough are enabled.
         */
        bool strikethroughDisabled() const;

        /**
         * Method you can use to determine if strikethrough is currently both enabled and disabled.
         *
         * \return Returns true if strikethrough is both enabled and disabled.
         */
        bool strikethroughBothEnabledAndDisabled() const;

        /**
         * Method you can use to determine the currently selected font color.
         *
         * \return Returns the selected font color.    An invalid color value is returned if there is no font color
         *         selected or multiple font colors are currently selected.
         */
        QColor fontColor() const;

        /**
         * Method you can use to determine if the user has selected a new font color.
         *
         * \return Returns true if the user has selected a font color.  Returns false if the user has not selected a
         *          font color.
         */
        bool fontColorSelected() const;

        /**
         * Method you can use to determine the currently selected font background color.
         *
         * \return Returns the selected font background color.    An invalid color value is returned if there is no
         *         font background color selected or multiple font background colors are selected.
         */
        QColor fontBackgroundColor() const;

        /**
         * Method you can use to determine if the user has selected a new font background color.
         *
         * \return Returns true if the user has selected a new font background color.  Returns false if the user has
         *         not selected a new font background color.
         */
        bool fontBackgroundColorSelected() const;

        /**
         * Method you can use to determine the current letter spacing value.
         *
         * \return Returns the current letter spacing value.  A value of 0 is returned if there is no valid letter
         *         spacing.
         */
        float letterSpacing() const;

        /**
         * Method you can use to determine if the current letter spacing value is valid.
         *
         * \return Returns true if the current letter spacing value is valid.  Returns false if the current letter
         *         spacing value is invalid.
         */
        bool letterSpacingValid() const;

        /**
         * Method you can use to determine if the current letter spacing value is invalid.
         *
         * \return Returns true if the current letter spacing value is invalid.  Returns false if the current letter
         *         spacing value is valid.
         */
        bool letterSpacingInvalid() const;

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
        void apply(ListFormatDialog* thisDialog);

    public slots:
        /**
         * Slot you can use to set the state of the show bullet or number check box.
         *
         * \param[in] newCheckState The new state of the show bullet or number check box.
         */
        void setShowBulletOrNumberCheckState(CheckState newCheckState);

        /**
         * Slot you can use to enable or disable the show bullet or number check box.
         *
         * \param[in] nowEnabled If true, the checkbox will be enabled.  If false, the checkbox will be disabled.
         */
        void setShowBulletOrNumberCheckBoxEnabled(bool nowEnabled = true);

        /**
         * Slot you can use to disable or enable the show bullet or number check box.
         *
         * \param[in] nowDisabled If true, the checkbox will be disabled.  If false, the checkbox will be enabled.
         */
        void setShowBulletOrNumberCheckBoxDisabled(bool nowDisabled = true);

        /**
         * Slot you can use to enable or disable the list font dialog control.
         *
         * \param[in] nowEnabled If true, the list font dialog will be enabled.  If false, the list font dialog will
         *                       be disabled.
         */
        void setListFontControlsEnabled(bool nowEnabled = true);

        /**
         * Slot you can use to disable or enable the show bullet or number check box.
         *
         * \param[in] nowDisabled If true, the list font dialog will be enabled.  If false, the list font dialog will
         *                        be disabled.
         */
        void setListFontControlsDisabled(bool nowDisabled = true);

        /**
         * Slot you can use to set the selected bullet styles.
         *
         * \param[in] selectedStyles A set containing the selected bullet styles.
         */
        void setSelectedBulletStyle(const QSet<QString>& selectedStyles);

        /**
         * Slot you can use to set the selected number style.
         *
         * \param[in] selectedStyles A set containing the selected number styles.
         */
        void setSelectedNumberStyles(const QSet<NumberStyle>& selectedStyles);

        /**
         * Slot you can use to set the selected starting number value.
         *
         * \param[in] newStartingNumber The new starting number value.
         */
        void setStartingNumber(unsigned newStartingNumber);

        /**
         * Slot you can use to clear the starting number.
         */
        void clearStartingNumber();

        /**
         * Slot you can use to set the currently selected prefix string.
         *
         * \param[in] newPrefixString The new prefix string value.
         */
        void setPrefixString(const QString& newPrefixString);

        /**
         * Slot you can use to flag the prefix string as unset.
         */
        void clearPrefixString();

        /**
         * Slot you can use to set the currently selected suffix string.
         *
         * \param[in] newSuffixString The new suffix string value.
         */
        void setSuffixString(const QString& newSuffixString);

        /**
         * Slot you can use to flag the suffix string as unset.
         */
        void clearSuffixString();

        /**
         * Slot you can trigger to set the font family.
         *
         * \param[in] newFontFamily The new font family to show in the dialog window.
         */
        void setFontFamily(const QString& newFontFamily);

        /**
         * Slot you can trigger to indicate that multiple font families are set.
         *
         * \param[in] newFontFamilies A set containing the names of every set font family.
         */
        void setFontFamilies(const QSet<QString>& newFontFamilies);

        /**
         * Slot you can trigger to set the font size.
         *
         * \param[in] newFontSize The new font size to show in the dialog window.
         */
        void setFontSize(unsigned newFontSize);

        /**
         * Slot you can trigger to indicate that multiple font sizes are set.
         *
         * \param[in] newFontSizes THe new font sizes to show in the dialog window.
         */
        void setFontSizes(const QSet<unsigned>& newFontSizes);

        /**
         * Slot you can trigger to set the font weight.
         *
         * \param[in] newFontWeight The new font weight value.
         */
        void setFontWeight(Ld::FontFormat::Weight newFontWeight);

        /**
         * Method you can call to indicate multiple font weights are set.
         *
         * \param[in] newFontWeights A set indicating every selected font weight.
         */
        void setFontWeights(const QSet<Ld::FontFormat::Weight>& newFontWeights);

        /**
         * Method you can call to indicate which font weights should be flagged as supported.
         *
         * \param[in] newSupportedFontWeights A set indicating which font weights should be supported.
         */
        void setSupportedFontWeights(const QSet<Ld::FontFormat::Weight>& newSupportedFontWeights);

        /**
         * Slot you can trigger to specify if italics should be set or cleared.
         *
         * \param[in] nowEnabled If true, italics will be indicated as set.  If false, italics will be indicated as
         *                       cleared.
         */
        void setItalicsEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to specify if italics should be cleared or set.
         *
         * \param[in] nowDisabled If true, italics will be indicated as not set.  If false, italics will be indicated
         *                        as set.
         */
        void setItalicsDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to specify if italics should be indicated as both set or cleared.
         */
        void setItalicsBothEnabledAndDisabled();

        /**
         * Slot you can trigger to specify if underline should be set or cleared.
         *
         * \param[in] nowEnabled If true, underline will be indicated as set.  If false, underline will be indicated as
         *                       cleared.
         */
        void setUnderlineEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to specify if underline should be cleared or set.
         *
         * \param[in] nowDisabled If true, underline will be indicated as not set.  If false, underline will be
         *                        indicated as set.
         */
        void setUnderlineDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to specify if underline should be indicated as both set or cleared.
         */
        void setUnderlineBothEnabledAndDisabled();

        /**
         * Slot you can trigger to specify if overline should be set or cleared.
         *
         * \param[in] nowEnabled If true, overline will be indicated as set.  If false, overline will be indicated as
         *                       cleared.
         */
        void setOverlineEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to specify if overline should be cleared or set.
         *
         * \param[in] nowDisabled If true, overline will be indicated as not set.  If false, overline will be
         *                        indicated as set.
         */
        void setOverlineDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to specify if overline should be indicated as both set or cleared.
         */
        void setOverlineBothEnabledAndDisabled();

        /**
         * Slot you can trigger to specify if strikethrough should be set or cleared.
         *
         * \param[in] nowEnabled If true, strikethrough will be indicated as set.  If false, strikethrough will be
         *                       indicated as cleared.
         */
        void setStrikethroughEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to specify if strikethrough should be cleared or set.
         *
         * \param[in] nowDisabled If true, strikethrough will be indicated as not set.  If false, strikethrough will
         *                        be indicated as set.
         */
        void setStrikethroughDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to specify if strikethrough should be indicated as both set or cleared.
         */
        void setStrikethroughBothEnabledAndDisabled();

        /**
         * Slot you can trigger to set the font color.
         *
         * \param[in] newFontColor The new font color selected by the user.
         */
        void setFontColor(const QColor& newFontColor);

        /**
         * Slot you can trigger to set multiple font colors.
         *
         * \param[in] newFontColors A set containing every selected font color.
         */
        void setFontColors(const QSet<QColor>& newFontColors);

        /**
         * Slot you can trigger to set the font background color.
         *
         * \param[in] newFontBackgroundColor The new font background color selected by the user.
         */
        void setFontBackgroundColor(const QColor& newFontBackgroundColor);

        /**
         * Slot you can trigger to set multiple font background colors.
         *
         * \param[in] newFontBackgroundColors A set containing every selected font color.
         */
        void setFontBackgroundColors(const QSet<QColor>& newFontBackgroundColors);

        /**
         * Slot you can trigger to set a single letter spacing value.
         *
         * \param[in] newLetterSpacing The new letter spacing value.
         */
        void setLetterSpacing(float newLetterSpacing);

        /**
         * Slot you can trigger to set multiple letter spacing values.
         *
         * \param[in] newLetterSpacing A set holding the new letter spacing values.
         */
        void setLetterSpacing(const QSet<float>& newLetterSpacing);

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
         * Slot that is triggered when the list font button is clicked.
         */
        void listFontButtonClicked();

        /**
         * Slot that is triggered when a new bullet style is selected.
         *
         * \param[in] unicodeValue The unicode value of the newly selected bullet.
         */
        void bulletStyleSelected(Ld::UnorderedListParagraphFormat::StandardBullet::UnicodeValue unicodeValue);

        /**
         * Slot that is triggered when the number list style is selected.
         */
        void numberListStyleSelected();

        /**
         * Slot that is triggered when the upper case letter list style is selected.
         */
        void upperCaseLetterListStyleSelected();

        /**
         * Slot that is triggered when the lower case letter list style is selected.
         */
        void lowerCaseLetterListStyleSelected();

        /**
         * Slot that is triggered when the upper case Roman numeral style is selected.
         */
        void upperCaseRomanNumeralStyleSelected();

        /**
         * Slot that is triggered when the lower case Roman numeral style is selected.
         */
        void lowerCaseRomanNumeralStyleSelected();

        /**
         * Slot that is triggered when the spin box value changes.
         *
         * \param[in] newValue The new value reported by the spin box.
         */
        void startingNumberChanged(int newValue);

        /**
         * Slot that is triggered when the user touches the prefix string.
         *
         * \param[in] newText The new prefix text.
         */
        void prefixTextChanged(const QString& newText);

        /**
         * Slot that is triggered when the user touches the suffix string.
         *
         * \param[in] newSuffix The new suffix text.
         */
        void suffixTextChanged(const QString& newSuffix);

        /**
         * Slot that is triggered when the remove list formatting button is clicked.
         */
        void removeListFormattingButtonClicked();

        /**
         * Slot that is triggered when the show/hide bullet/number check box is clicked.
         *
         * \param[in] newState The new check box state.
         */
        void showHideButtonNumberCheckBoxClicked(int newState);

        /**
         * Slot that is triggered when the apply button is clicked.
         */
        void applyClicked();

        /**
         * Slot that is triggered when a value changes.  The slot checks for updates and adjusts the dialog
         * accordingly.
         */
        void updateDialog();

    private:
        /**
         * Enumeration used internally to track the state of flags that can take on four states.
         */
        enum class QuadState:std::uint8_t {
            /**
             * Indicates no values set or cleared.
             */
            NO_VALUE,

            /**
             * Indicates all values are enabled.
             */
            ENABLED,

            /**
             * Indicates all values are disabled.
             */
            DISABLED,

            /**
             * Indicates we have both enabled and disabled values.
             */
            ENABLED_AND_DISABLED
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
         * Method that enables or disables the number style controls.
         *
         * \param[in] nowEnabled If true, the number style controls will be enabled.  if false, the number style
         *                       controls (but not the style buttons) will be disabled.
         */
        void setNumberStyleControlsEnabled(bool nowEnabled);

        /**
         * Method that unchecks all bullet style buttons.
         */
        void uncheckBulletStyles();

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

        /**
         * The font format dialog.  The value is maintained persistently so we can easily marshall settings to from
         * the underlying dialog.
         */
        CharacterFormatDialog* fontFormatDialog;

        /**
         * The input show bullet/number check state.
         */
        CheckState inputShowBulletOrNumberCheckState;

        /**
         * The input set of selected bullet styles.
         */
        QSet<QString> inputBulletStyles;

        /**
         * The input set of selected number styles.
         */
        QSet<NumberStyle> inputNumberStyles;

        /**
         * The input starting number value.  A value of static_cast<unsigned>(-1) indicates an invalid value.
         */
        unsigned inputStartingNumber;

        /**
         * The input prefix string.
         */
        QString inputPrefixString;

        /**
         * Flag indicating if the input prefix string should be considered set.
         */
        bool inputPrefixStringSet;

        /**
         * The input suffix string.
         */
        QString inputSuffixString;

        /**
         * Flag indicating if the input suffix string should be considered set.
         */
        bool inputSuffixStringSet;

        /**
         * Current input font families.
         */
        QSet<QString> inputFontFamilies;

        /**
         * Current input font sizes.
         */
        QSet<unsigned> inputFontSizes;

        /**
         * Current input font weights.
         */
        QSet<Ld::FontFormat::Weight> inputFontWeights;

        /**
         * Current supported font weights.
         */
        QSet<Ld::FontFormat::Weight> inputSupportedFontWeights;

        /**
         * Flag indicating if we have italics.
         */
        QuadState inputItalics;

        /**
         * Flag indicating if we have underlines.
         */
        QuadState inputUnderline;

        /**
         * Flag indicates if we have overline.
         */
        QuadState inputOverline;

        /**
         * Flag indicating if we have strikethrough.
         */
        QuadState inputStrikethrough;

        /**
         * Current input font color.
         */
        QSet<QColor> inputFontColors;

        /**
         * Current input font background color
         */
        QSet<QColor> inputFontBackgroundColors;

        /**
         * Current set of input letter spacing values.
         */
        QSet<float> inputLetterSpacings;

        /**
         * Flag that indicates that the values loaded into the font dialog are correct.
         */
        bool useFontDialogValues;

        /**
         * Flag indicating if the input prefix string was changed.
         */
        bool prefixStringWasChanged;

        /**
         * Flag indicating if the input suffix string was changed.
         */
        bool suffixStringWasChanged;
};

#endif
