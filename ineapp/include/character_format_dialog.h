/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CharacterFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CHARACTER_FORMAT_DIALOG_H
#define CHARACTER_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>
#include <QColor>

#include <cstdint>

#include <eqt_programmatic_dialog.h>

#include <ld_format.h>
#include <ld_character_format.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;
class QCheckBox;
class QFont;

namespace Ld {
    class BlockFormat;
    class PageFormat;
};

/**
 * Dialog used to adjust one or more character format instances.
 */
class APP_PUBLIC_API CharacterFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Enumeration of supported demo text modes.
         */
        enum class DemoTextMode {
            /**
             * Indicates normal lorem ipsum will be displayed.
             */
            NORMAL,

            /**
             * Indicates bullets and number styles will be displayed.
             */
            LIST
        };

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        CharacterFormatDialog(QWidget* parent = Q_NULLPTR);

        ~CharacterFormatDialog() override;

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
         * Method you can use to determine the font position selected by the user.
         *
         * \return Returns the font position that was selected by the user.  This method returns an invalid position
         *         if multiple positions are selected or if no position is selected.
         */
        Ld::CharacterFormat::Position position() const;

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
         * Method you can call to determine if the baseline position controls are visible.
         *
         * \return Returns true if the baseline position controls are visible.  Returns false if the baseline position
         *         controls are hidden.
         */
        bool positionControlsEnabled() const;

        /**
         * Method you can call to determine if the baseline position controls are hidden.
         *
         * \return Returns true if the baseline position controls are hidden.  Returns false if the baseline position
         *         controls are visible.
         */
        bool positionControlsDisabled() const;

        /**
         * Method you can use to determine if the font background color controls are visible or hidden.
         *
         * \return Returns true if the font background color controls are visible.  Returns false if the font
         *         background color controls are hidden.
         */
        bool fontBackgroundColorControlsEnabled() const;

        /**
         * Method you can use to determine if the font background color controls are hidden or visible.
         *
         * \return Returns true if the font background color controls are visible.  Returns false if the font
         *         background color controls are hidden.
         */
        bool fontBackgroundColorControlsDisabled() const;

        /**
         * Method you can use to to determine the demo text mode.
         *
         * \return Returns the currently selected demo text mode.
         */
        DemoTextMode demoTextMode() const;

        /**
         * Method you can use to determine if the apply button is visible or hidden.
         *
         * \return Returns true if the Apply button is visible.  Returns false if the Apply button is hidden.
         */
        bool applyButtonVisible() const;

        /**
         * Method you can use to determine if the apply button is hidden or visible.
         *
         * \return Returns true if the Apply button is hidden.  Returns false if the Apply button is viible.
         */
        bool applyButtonHidden() const;

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
        void apply(CharacterFormatDialog* thisDialog);

    public slots:
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
         * Slot you can trigger to specify the font baseline position.
         *
         * \param[in] newPosition The new font baseline position to indicate.
         */
        void setPosition(Ld::CharacterFormat::Position newPosition);

        /**
         * Slot you can trigger to specify multiple font baseline positions.
         *
         * \param[in] newPositions A set of all the position to indicate.
         */
        void setPositions(const QSet<Ld::CharacterFormat::Position>& newPositions);

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

        /**
         * Slot you can trigger to enable or disable baseline position controls.
         *
         * \param[in] nowEnabled If true, the baseline position controls will be made visible and usable.  If false,
         *                       the baseline position controls will be hidden and made unusable.
         */
        void setPositionControlsEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable or enable baseline position controls.
         *
         * \param[in] nowDisabled If true, the baseline position controls will be hidden and made unusable.  if false,
         *                        the baseline position controls will be made visible.
         */
        void setPositionControlsDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to enable or disable the font background color controls
         *
         * \param[in] nowEnabled If true, the font background color controls will be visible and usable.  If false, the
         *                       font background color controls will be hidden.
         */
        void setFontBackgroundColorControlsEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable or enable the font background color controls
         *
         * \param[in] nowDisabled If true, the font background color controls will be hidden.  If faise, the font
         *                        background controls will be visible.
         */
        void setFontBackgroundColorControlsDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to change the demo text mode.
         *
         * \param[in] newDemoTextMode The new demo text mode.  Mode \ref CharacterFormatDialog::DemoTextMode::NORMAL
         *                            will be enabled by default.
         */
        void setDemoTextMode(DemoTextMode newDemoTextMode);

        /**
         * Method you can use to show or hide the apply button.  Note that this method performs no function on MacOS.
         *
         * \param[in] nowVisible If true, the Apply button will be made visible.  If false, the Apply button will be
         *                       hidden.
         */
        void setApplyButtonVisible(bool nowVisible = true);

        /**
         * Method you can use to hide or show the apply button.  Note that this method performs no function on MacOS.
         *
         * \param[in] nowHidden If true, the Apply button will be hidden.  If false, the Apply button will be made
         *                      visible.
         */
        void setApplyButtonHidden(bool nowHidden = true);

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * Eqt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered when the user selects a new font family.
         *
         * \param[in] newFont The newly selected font.
         */
        void newFontFamilySelected(const QFont& newFont);

        /**
         * Slot that is triggered when the user adjusts the font size.
         *
         * \param[in] newFontSize The newly selected font size.
         */
        void newFontSizeSelected(int newFontSize);

        /**
         * Slot that is triggered when the user selects thin font weight.
         */
        void thinFontWeightSelected();

        /**
         * Slot that is triggered when the user selects the extra-light font weight.
         */
        void extraLightFontWeightSelected();

        /**
         * Slot that is triggered when the user selects the light font weight.
         */
        void lightFontWeightSelected();

        /**
         * Slot that is selected when the user selects normal font weight.
         */
        void normalFontWeightSelected();

        /**
         * Slot that is selected when the user selects medium font weight.
         */
        void mediumFontWeightSelected();

        /**
         * Slot that is triggered whe the user selects demi-bold font weight.
         */
        void demiBoldFontWeightSelected();

        /**
         * Slot that is triggerd when the user selects the bold font weight.
         */
        void boldFontWeightSelected();

        /**
         * Slot that is triggered when the user selects the extra-bold font weight.
         */
        void extraBoldFontWeightSelected();

        /**
         * Slot that is triggered when the user selects the extra-bold font weight.
         */
        void blackFontWeightSelected();

        /**
         * Slot that is triggered when the user clicks the italics check box.
         */
        void italicsClicked();

        /**
         * Slot that is triggered when the user clicks the underline check box.
         */
        void underlineClicked();

        /**
         * Slot that is triggered when the user clicks the overline check box.
         */
        void overlineClicked();

        /**
         * Slot that is triggered when the user clicks the strikethrough check box.
         */
        void strikethroughClicked();

        /**
         * Slot that is triggered when superscript is selected.
         */
        void superscriptSelected();

        /**
         * Slot that is triggered when normal baseline is selected.
         */
        void normalBaselineSelected();

        /**
         * Slot that is triggered when subscript is selected.
         */
        void subscriptSelected();

        /**
         * Slot that is triggered when the user clicks on the change font color button.
         */
        void changeFontColorButtonClicked();

        /**
         * Slot that is triggered when the user clicks on the remove font color button.
         */
        void removeFontColorButtonClicked();

        /**
         * Slot that is triggered when the user clicks on the change font background color button.
         */
        void changeFontBackgroundColorButtonClicked();

        /**
         * Slot that is triggered when the user clicks on the remove font background color button.
         */
        void removeFontBackgroundColorButtonClicked();

        /**
         * Slot that is triggered when the letter spacing value changes.
         *
         * \param[in] newLetterSpacing The new letter spacing value.
         */
        void letterSpacingChanged(double newLetterSpacing);

        /**
         * Slot that is triggered when the sample text scaling changes.
         *
         * \param[in] newScalingFactor The new scaling factor.
         */
        void sampleTextScalingChanged(float newScalingFactor);

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
         * Spacing between columns.
         */
        static constexpr unsigned embellishmentsColumnsSpacing = 80;

        /**
         * The font size used for filler text.
         */
        static constexpr unsigned fillerTextPointSize = 12;

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
         * Method that is called to check if any of the basic settings have changed.  Basic settings include:
         *
         *     * The font family
         *     * The font size
         *     * The font weight.
         *
         * \return Returns true if one or more values are different.  Returns false if all values match the inputs.
         */
        bool basicSettingsChanged() const;

        /**
         * Method that is called to check if any of the embellishments have changed.  Embellishments include:
         *
         *     * Italics settings
         *     * Underline settings
         *     * Overline settings
         *     * Strikethrough settings.
         *
         * \return Returns true if one or more values are different.  Returns false if all values match the inputs.
         */
        bool embellishmentsChanged() const;

        /**
         * Method that is called to check if the baseline settings have changed.
         *
         * \return Returns true if the baseline settings have changed.  Returns false if the baseline settings have
         *         not changed or if baseline settings are currently disabled.
         */
        bool baselineChanged() const;

        /**
         * Method that is called to check if the letter spacing value has changed.
         *
         * \return Returns true if the letter spacing value has changed.  Returns false if the letter spacing value has
         *         not changed.
         */
        bool letterSpacingChanged() const;

        /**
         * Method that is called to check if the color settings values have changed.
         *
         * \return Returns true if one or more of the color settings have changed.  Returns false if the color settings
         *         have not changed.
         */
        bool colorSettingsChanged() const;

        /**
         * Method that is called to update the weight radio button state.
         *
         * \param[in] newFontWeight The new font weight value.
         */
        void updateWeightRadioButtons(Ld::FontFormat::Weight newFontWeight);

        /**
         * Method that is called to update the position radio button state.
         *
         * \param[in] newPosition The new character position.
         */
        void updatePositionRadioButtons(Ld::CharacterFormat::Position newPosition);

        /**
         * Method that is called to update the sample text.
         */
        void updateSampleText();

        /**
         * Method that is called to update which weights are enabled or disabled.
         */
        void updateWeights();

        /**
         * Method that is called to update the remove color button states.
         */
        void updateColorButtonStates();

        /**
         * Method that is called to update a checkbox based on a QuadState value.
         *
         * \param[in] checkBox The check box to be updated.
         *
         * \param[in] quadState The quad state value to use to update the checkbox.
         */
        void updateCheckbox(QCheckBox* checkBox, QuadState quadState);

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

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
         * Current input font positions.
         */
        QSet<Ld::CharacterFormat::Position> inputPositions;

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
         * The current demo text mode.
         */
        DemoTextMode currentDemoTextMode;
};

#endif
