/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BraceConditionalConfigurationDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BRACE_CONDITIONAL_CONFIGURATION_DIALOG_H
#define BRACE_CONDITIONAL_CONFIGURATION_DIALOG_H

#include <QWidget>
#include <QFont>
#include <QString>

#include <eqt_programmatic_dialog.h>

#include "app_common.h"

class QWidget;
class QLabel;
class QSpinBox;
class QRadioButton;
class QLineEdit;

namespace Ld {
    class BlockFormat;
    class PageFormat;
};

/**
 * Dialog used to obtain the initial number of brace conditional terms as well as modify the configuration of a brace
 * conditional operator.
 */
class APP_PUBLIC_API BraceConditionalConfigurationDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        BraceConditionalConfigurationDialog(QWidget* parent = Q_NULLPTR);

        ~BraceConditionalConfigurationDialog() override;

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
         * Method you can use to display or hide the apply button.
         *
         * \param[in] nowVisible If true, the apply button will be made visible.  If false, the apply button will be
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
         * Method you can use to determine if the apply button is visible.
         *
         * \return Returns true if the apply button is visible.  Returns false if the apply button is not visible.
         */
        bool applyButtonVisible() const;

        /**
         * Method you can use to determine if the apply button is hidden.
         *
         * \return Returns true if the apply button is hidden.  Returns false if the apply button is visible.
         */
        bool applyButtonHidden() const;

        /**
         * Method you can use to specify if the OK button should always be enabled.
         *
         * \param[in] alwaysEnable If true, the OK button will always be enabled.  If false, the OK button will only
         *                         be enabled if the values are changed from the initial values.
         */
        void setOkButtonAlwaysEnabled(bool alwaysEnable = true);

        /**
         * Method you can use to specify if the OK button should only be enabled on change.  If false, the OK button
         * will always be enabled.
         *
         * \param[in] conditionallyEnableOnly If true, the OK button will be enabled only if values change from the
         *                                    initial values.  If false, the OK button will always be enabled.
         */
        void setOkButtonEnabledOnChange(bool conditionallyEnableOnly = true);

        /**
         * Method you can use to determine if the OK button is always enabled.
         *
         * \return Returns true if the OK button is always enabled.  Returns false if the OK button is only enabled
         *         when the dialog deviates from the current default values.
         */
        bool okButtonAlwaysEnabled() const;

        /**
         * Method you can use to determine if the OK button is only enabled when the dialog deviates from the initial
         * values.
         *
         * \return Returns true if the OK button is conditionally enabled.  Returns false if the OK button is always
         *         enabled.
         */
        bool okButtonEnabledOnChange() const;

        /**
         * Method you can use to enable or disable the number of explicit conditions spin box.
         *
         * \param[in] nowEnabled If true, the number of explicit terms spin box will be enabled.   If false, the
         *                       number of explicit terms spin box will be disabled.
         */
        void setNumberExplicitConditionsControlEnabled(bool nowEnabled = true);

        /**
         * Method you can use to disable or enable the number of explicit conditions spin box.
         *
         * \param[in] nowDisabled If true, the number of explicit terms spin box will be disabled.   If false, the
         *                        number of explicit terms spin box will be enabled.
         */
        void setNumberExplicitConditionsControlDisabled(bool nowDisabled = true);

        /**
         * Method you can use to determine if the number of explicit conditions spin box is enabled.
         *
         * \return Returns true if the number of explicit terms spin box is enabled.  Returns false if the number of
         *         explicit terms spin box is disabled.
         */
        bool isNumberExplicitConditionsControlEnabled() const;

        /**
         * Method you can use to determine if the number of explicit conditions spin box is disabled.
         *
         * \return Returns true if the number of explicit terms spin box is disabled.  Returns false if the number of
         *         explicit terms spin box is enabled.
         */
        bool isNumberExplicitConditionsControlDisabled() const;

        /**
         * Method you can use to enable or disable the explicit/implicit else condition radio button.  Disabling the
         * radio buttons will cause the else condition text field to be enabled in all cases.
         *
         * \param[in] nowEnabled If true, the radio buttons will be enabled.  If false, the radio buttons will be
         *                       disabled.
         */
        void setElseConditionControlsEnabled(bool nowEnabled = true);

        /**
         * Method you can use to disable or enable the explicit/implicit else condition radio button.  Disabling the
         * radio buttons will cause the else condition text field to be enabled in all cases.
         *
         * \param[in] nowDisabled If true, the radio buttons will be disabled.  If false, the radio buttons will be
         *                        enabled.
         */
        void setElseConditionControlsDisabled(bool nowDisabled = true);

        /**
         * Method you can use to determine if the explicit/implicit else condition radio buttons are enabled.
         *
         * \return Returns true if the controls are enabled.  Returns false if the controls are disabled.
         */
        bool isElseConditionControlsEnabled() const;

        /**
         * Method you can use to determine if the explicit/implicit else condition radio buttons are disabled.
         *
         * \return Returns true if the controls are disabled.  Returns false if the controls are enabled.
         */
        bool isElseConditionControlsDisabled() const;

        /**
         * Method you can use to determine the currently selected number of explicit brace conditions.
         *
         * \return Returns the number of currently selected brace conditions.  A value of 0 indicates the number of
         *         explicit conditions is currently unset.
         */
        unsigned long numberExplicitConditions() const;

        /**
         * Method you can use to determine if the brace should have an explicit else condition.
         *
         * \return Returns true if the brace should have an explicit else condition.  Returns false if the else
         *         condition is indicated via a text field.
         */
        bool hasExplicitElseCondition() const;

        /**
         * Method you can use to determine if the brace should have an implicit else condition.
         *
         * \return Returns true if the brace should have an implicit else condition.  Returns false if the else
         *         condition should be explicitly shown.
         */
        bool hasImplicitElseCondition() const;

        /**
         * Method you can use to determine if the brace else condition style has been selected.
         *
         * \return Returns true if a style has been selected.  Returns false if a style has not been selected.
         */
        bool isElseConditionSet() const;

        /**
         * Method you can use to determine the desired preamble text for the brace operator.
         *
         * \return Returns the preamble text for the brace operator.
         */
        QString conditionPreambleText() const;

        /**
         * Method you can use to determine the implicit else condition text.
         *
         * \return Returns the implicit else condition text.
         */
        QString implicitElseConditionText() const;

        /**
         * Method you can use to obtain the current math text font.
         *
         * \return Returns the current math text font.
         */
        QFont mathTextFont() const;

        /**
         * Method you can use to obtain the current math identifier font.
         *
         * \return Returns the current math identifier font.
         */
        QFont mathIdentifierFont() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() override;

    signals:
        /**
         * Signal that is emitted when the Apply button is clicked on Windows or when any update occurs on MacOS.
         *
         * \param[out] thisDialog A pointer to this dialog.  You can use this parameter to obtain the current values.
         */
        void apply(BraceConditionalConfigurationDialog* thisDialog);

    public slots:
        /**
         * Slot you can use to set the number of explicit conditions.
         *
         * \param[in] newNumberExplicitConditions The new number of explicit conditions. A value of 0 makes the number
         *                                        of else condtions appear as unset.
         */
        void setNumberExplicitConditions(unsigned long newNumberExplicitConditions);

        /**
         * Slot you can use to clear the number of explicit conditions.
         */
        void clearNumberExplicitConditions();

        /**
         * Slot you can use to indicate that the brace should have an explicit else condition.
         *
         * \param[in] nowHaveExplicitElseCondition If true, the brace should have an explicit else condition.  If
         *                                         false, the brace should have an implicit else condition.
         */
        void setHaveExplicitElseCondition(bool nowHaveExplicitElseCondition);

        /**
         * Slot you can use to indicate that the brace should have an implicit else condition.
         *
         * \param[in] nowHaveImplicitElseCondition If true, the brace should have an implicit else condition.  If
         *                                         false, the brace should have an explicit else condition.
         */
        void setHaveImplicitElseCondition(bool nowHaveImplicitElseCondition);

        /**
         * Slot you can use to indicate that the implicit else condition should be undefined.
         */
        void setElseConditionUndefined();

        /**
         * Slot you can use to set the condition preamble text.
         *
         * \param[in] newPreambleText The new condition preamble text.
         */
        void setConditionPreambleText(const QString& newPreambleText);

        /**
         * Slot you can use to set the implicit else condition text.
         *
         * \param[in] newElseConditionText The new else condition text.
         */
        void setImplicitElseConditionText(const QString& newElseConditionText);

        /**
         * Slot you can use to set the current math text font.
         *
         * \param[in] newFont The new math text font.
         */
        void setMathTextFont(const QFont& newFont);

        /**
         * Slot you can use to set the current math identifier font.
         *
         * \param[in] newFont the new math identifier font.
         */
        void setMathIdentifierFont(const QFont& newFont);

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() override;

    private slots:
        /**
         * Slot that is triggered when the number of explicit conditions is changed.
         *
         * \param[in] newValue The new value.
         */
        void numberExplicitConditionsChanged(int newValue);

        /**
         * Slot that is triggered when the explicit else condition radio button is clicked.
         */
        void explicitElseConditionRadioButtonClicked();

        /**
         * Slot that is triggered when the implicit else condition radio button is clicked.
         */
        void implicitElseConditionRadioButtonClicked();

        /**
         * Slot that is triggered whenever the condition preamble line edit is edited by the user.
         *
         * \param[in] newText The new text in the line editor.
         */
        void conditionPreambleLineEditModified(const QString& newText);

        /**
         * Slot that is triggered whenever the implicit else condition text is changed.
         *
         * \param[in] newText The new text in the line editor.
         */
        void implicitElseCondtionTextModified(const QString& newText);

        /**
         * Slot that is triggered when the apply button is clicked.
         */
        void applyClicked();

    private:
        /**
         * Enumeration used to indicate tri-state values.
         */
        enum TriState {
            /**
             * Indicates a true value.
             */
            TRUE,

            /**
             * Indicates a false value.
             */
            FALSE,

            /**
             * Indicates an undefined value.
             */
            UNSET
        };

        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Called to update the sample display.
         */
        void updateSample();

        /**
         * Called to update the dialog state.
         */
        void updateDialogControls();

        /**
         * Method that is called to check if values are different from the input values.
         *
         * \return Returns true if one or more values are different.  Returns false if all values match the inputs.
         */
        bool valuesChanged() const;

        /**
         * Flag that indicates if the OK button should be enabled all the time.
         */
        bool currentOkAlwaysEnabled;

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

        /**
         * Font used for math operators.
         */
        QFont currentMathTextFont;

        /**
         * Font used for math identifiers.
         */
        QFont currentMathIdentifierFont;

        /**
         * The input number of explicit conditionals.
         */
        unsigned long inputNumberExplicitConditionals;

        /**
         * Flag indicating if the else condition is explicitly shown.
         */
        TriState inputExplicitElseClause;

        /**
         * Value indicating the input condition prefix text.
         */
        QString inputConditionPrefixText;

        /**
         * Value indicating the input implicit else clause text.
         */
        QString inputImplicitElseClauseText;
};

#endif
