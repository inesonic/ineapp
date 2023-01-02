/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ParenthesisStylePopupDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PARENTHESIS_STYLE_POPUP_DIALOG_H
#define PARENTHESIS_STYLE_POPUP_DIALOG_H

#include <QDialog>
#include <QPoint>
#include <QSet>
#include <QHash>
#include <QFont>

#include <ld_operator_format.h>

#include "app_common.h"

class QWidget;
class QToolButton;
class QCheckBox;
class QGroupBox;
class QFocusEvent;

/**
 * Class that provides a small popup dialog you can use to select a parenthesis style.
 */
class APP_PUBLIC_API ParenthesisStylePopupDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent a parenthesis style.
         */
        typedef Ld::OperatorFormat::ParenthesisStyle ParenthesisStyle;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        ParenthesisStylePopupDialog(QWidget* parent = Q_NULLPTR);

        ~ParenthesisStylePopupDialog() override;

        /**
         * Method you can use to determine if the force parenthesis checkbox is visible or hidden.
         *
         * \return Returns true if the force parenthesis checkbox is visible.  Returns false if the force parenthesis
         *         checkbox is hidden.
         */
        bool forceParenthesisCheckboxVisible() const;

        /**
         * Method you can use to determine if the force parenthesis checkbox is hidden or visible.
         *
         * \return Returns true if the force parenthesis checkbox is hidden.  Returns false if the force parenthesis
         *         checkbox is visible.
         */
        bool forceParenthesisCheckboxHidden() const;

        /**
         * Method you can use to determine if the default parenthesis style button is visible.
         *
         * \return Returns true if the default parenthesis style button is visible.  Returns false if the default
         *         parenthesis style button is hidden.
         */
        bool defaultParenthesisStyleButtonVisible() const;

        /**
         * Method you can use to determine if the default parenthesis style button is hidden.
         *
         * \return Returns true if the default parenthesis style button is hidden.  Returns false if the default
         *         parenthesis style button is visible.
         */
        bool defaultParenthesisStyleButtonHidden() const;

        /**
         * Method you can use to determine if the function parameter parenthesis controls are visible.
         *
         * \return Returns true if the function parameter parenthesis controls are visible.  Returns false if the
         *         function parameter parenthesis controls are hidden.
         */
        bool functionParameterParenthesisButtonsVisible() const;

        /**
         * Method you can use to determine if the function parameter parenthesis controls are hidden.
         *
         * \return Returns true if the function parameter parenthesis controls are hidden.  Returns false if the
         *         function parameter parenthesis controls are hidden.
         */
        bool functionParameterParenthesisButtonsHidden() const;

        /**
         * Method you can use to determine if force function parameter parenthesis checkbox is visible.
         *
         * \return Returns true if the force function parameter parenthesis checkbox is visible.  Returns false if the
         *         force function parameter parenthesis checkbox is hidden.
         */
        bool forceFunctionParameterParenthesisCheckboxVisible() const;

        /**
         * Method you can use to determine if the force function parameter parenthesis checkbox is hidden.
         *
         * \return Returns true if the force function parameter parenthesis checkbox is hidden.  Returns false if the
         *         force function parameter parenthesis checkbox is hidden.
         */
        bool forceFunctionParameterParenthesisCheckboxHidden() const;

    signals:
        /**
         * Signal that is emitted when the user selects a new parenthesis style.
         *
         * \param[out] newParenthesisStyle The newly selected parenthesis style.
         */
        void parenthesisStyleSelected(ParenthesisStyle newParenthesisStyle);

        /**
         * Signal that is emitted when the user checks or unchecks the force parenthesis checkbox.
         *
         * \param[out] nowChecked If true, the force parenthesis button is now checked.
         */
        void forceParenthesisButtonChecked(bool nowChecked);

        /**
         * Signal that is emitted when the user selects a new parameter parenthesis style.
         *
         * \param[out] newParenthesisStyle The newly selected parenthesis style.
         */
        void parameterParenthesisStyleSelected(ParenthesisStyle newParenthesisStyle);

        /**
         * Signal that is emitted when the user checks or unchecks the force parameter parenthesis checkbox.
         *
         * \param[out] nowChecked If true, the force parameter parenthesis button is now checked.
         */
        void forceParameterParenthesisButtonChecked(bool nowChecked);

        /**
         * Signal that is emitted when the dialog loses focus.
         */
        void focusLost();

    public slots:
        /**
         * Slot you can use to set the parenthesis styles.
         *
         * \param[in] parenthesisStyle The current parenthesis style.
         */
        void setParenthesisStyle(ParenthesisStyle parenthesisStyle);

        /**
         * Slot you can use to set the parenthesis styles.
         *
         * \param[in] parenthesisStyles The current parenthesis styles.
         */
        void setParenthesisStyle(const QSet<ParenthesisStyle>& parenthesisStyles);

        /**
         * Slot you can use to check the force parenthesis button.
         *
         * \param[in] forceParenthesisChecked If true, the force parenthesis checkbox is checked.
         */
        void setForceParenthesisButtonChecked(bool forceParenthesisChecked = true);

        /**
         * Slot you can use to uncheck the force parenthesis button.
         *
         * \param[in] forceParenthesisUnchecked If true, the force parenthesis checkbox is unchecked.  If false, the
         *                                      force parenthesis checkbox will be checked.
         */
        void setForceParenthesisButtonUnchecked(bool forceParenthesisUnchecked = true);

        /**
         * Slot you can use to tri-state the parenthesis button.
         */
        void setForceParenthesisTriStated();

        /**
         * Slot you can use to show or hide the force parenthesis checkbox.
         *
         * \param[in] nowVisible If true, the checkbox will be visible.  If false, the checkbox will be hidden.
         */
        void setForceParenthesisCheckboxVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide or show the force parenthesis checkbox.
         *
         * \param[in] nowHidden If true, the checkbox will be hidden.  If false, the checkbox will be visible.
         */
        void setForceParenthesisCheckboxHidden(bool nowHidden = true);

        /**
         * Slot you can use to show or hide the default parenthesis style button.
         *
         * \param[in] nowVisible If true, the button will be visible.  If false, the button will be hidden.
         */
        void setDefaultParenthesisStyleButtonVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide or show the default parenthesis style button.
         *
         * \param[in] nowHidden If true, the button will be hidden.  If false, the button will be visible.
         */
        void setDefaultParenthesisStyleButtonHidden(bool nowHidden = true);

        /**
         * Slot you can use to set the parameter parenthesis styles.
         *
         * \param[in] parenthesisStyle The current parameter parenthesis style.
         */
        void setParameterParenthesisStyle(ParenthesisStyle parenthesisStyle);

        /**
         * Slot you can use to set the parameter parenthesis styles.
         *
         * \param[in] parenthesisStyles The current parameter parenthesis styles.
         */
        void setParameterParenthesisStyle(const QSet<ParenthesisStyle>& parenthesisStyles);

        /**
         * Slot you can use to check the force parameter parenthesis button.
         *
         * \param[in] forceParenthesisChecked If true, the force parameter parenthesis checkbox is checked.
         */
        void setForceParameterParenthesisButtonChecked(bool forceParenthesisChecked = true);

        /**
         * Slot you can use to uncheck the force parameter parenthesis button.
         *
         * \param[in] forceParenthesisUnchecked If true, the force parameter parenthesis checkbox is unchecked.  If false, the
         *                                      force parameter parenthesis checkbox will be checked.
         */
        void setForceParameterParenthesisButtonUnchecked(bool forceParenthesisUnchecked = true);

        /**
         * Slot you can use to tri-state the parameter parenthesis button.
         */
        void setForceParameterParenthesisTriStated();

        /**
         * Slot you can use to show or hide the force parameter parenthesis checkbox.
         *
         * \param[in] nowVisible If true, the checkbox will be visible.  If false, the checkbox will be hidden.
         */
        void setForceParameterParenthesisCheckboxVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide or show the force parameter parenthesis checkbox.
         *
         * \param[in] nowHidden If true, the checkbox will be hidden.  If false, the checkbox will be visible.
         */
        void setForceParameterParenthesisCheckboxHidden(bool nowHidden = true);

        /**
         * Slot you can use to show or hide the parameter parenthesis controls.
         *
         * \param[in] nowVisible If true, the parameter parenthesis controls will be visible.  If false, the parameter
         *                       parenthesis controls will be hidden.
         */
        void setParameterParenthesisButtonsVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide or show the parameter parenthesis controls.
         *
         * \param[in] nowHidden If true, the parameter parenthesis controls will be hidden.  If false, the parameter
         *                      parenthesis controls will be visible.
         */
        void setParameterParenthesisButtonsHidden(bool nowHidden = true);

    protected:
        /**
         * Method that is called when the dialog loses focus.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void focusOutEvent(QFocusEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the default parenthesis style button is clicked.
         */
        void defaultParenthesisButtonClicked();

        /**
         * Slot that is triggered when the parenthesis style button is clicked.
         */
        void parenthesisButtonClicked();

        /**
         * Slot that is triggered when the bracket style button is clicked.
         */
        void bracketButtonClicked();

        /**
         * Slot that is triggered when the brace style button is clicked.
         */
        void braceButtonClicked();

        /**
         * Slot that is triggered when the user clicks the force parenthesis checkbox.
         *
         * \param[in] nowChecked If true, the force parenthesis button is now checked.
         */
        void forceParenthesisButtonClicked(bool nowChecked);

        /**
         * Slot that is triggered when the parameter parenthesis style button is clicked.
         */
        void parameterParenthesisButtonClicked();

        /**
         * Slot that is triggered when the parameter bracket style button is clicked.
         */
        void parameterBracketButtonClicked();

        /**
         * Slot that is triggered when the parameter brace style button is clicked.
         */
        void parameterBraceButtonClicked();

        /**
         * Slot that is triggered when the user clicks the force parameter parenthesis checkbox.
         *
         * \param[in] nowChecked If true, the force parameter parenthesis button is now checked.
         */
        void forceParameterParenthesisButtonClicked(bool nowChecked);

    private:
        /**
         * Button used to trigger the default parenthesis setting, whatever that may be for the element.
         */
        QToolButton* defaultParenthesisToolButton;

        /**
         * Button used to trigger traditional parenthesis.
         */
        QToolButton* parenthesisToolButton;

        /**
         * Button used to trigger brackets.
         */
        QToolButton* bracketsToolButton;

        /**
         * Button used to trigger braces.
         */
        QToolButton* bracesToolButton;

        /**
         * Check box used to indicate if the element should force parenthesis.
         */
        QCheckBox* forceParenthesisCheckBox;

        /**
         * Group box for the parameter parenthesis style controls.
         */
        QGroupBox* parameterParenthesisStyleGroupBox;

        /**
         * Button used to trigger traditional parameter parenthesis.
         */
        QToolButton* parameterParenthesisToolButton;

        /**
         * Button used to trigger parameter brackets.
         */
        QToolButton* parameterBracketsToolButton;

        /**
         * Button used to trigger parameter braces.
         */
        QToolButton* parameterBracesToolButton;

        /**
         * Check box used to indicate if the element should force parameter parenthesis.
         */
        QCheckBox* forceParameterParenthesisCheckBox;
};

#endif
