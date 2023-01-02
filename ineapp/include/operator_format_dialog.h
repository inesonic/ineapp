/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref OperatorFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef OPERATOR_FORMAT_DIALOG_H
#define OPERATOR_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>

#include <eqt_programmatic_dialog.h>

#include <ld_operator_format.h>
#include <ld_multiplication_operator_format.h>
#include <ld_division_operator_format.h>
#include <ld_logical_unary_not_operator_format.h>

#include "app_common.h"

class QWidget;
class QToolButton;

namespace Ld {
    class PageFormat;
};

/**
 * Dialog used to adjust one or more operator instances.
 */
class APP_PUBLIC_API OperatorFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent a parenthesis style.
         */
        typedef Ld::OperatorFormat::ParenthesisStyle ParenthesisStyle;

        /**
         * Type used to represent a multiplication symbol style.
         */
        typedef Ld::MultiplicationOperatorFormat::MultiplicationSymbol MultiplicationSymbol;

        /**
         * Type used to represent a division symbol style.
         */
        typedef Ld::DivisionOperatorFormat::DivisionSymbol DivisionSymbol;

        /**
         * Type used to represent a logical not symbol style.
         */
        typedef Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol LogicalNotSymbol;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        OperatorFormatDialog(QWidget* parent = Q_NULLPTR);

        ~OperatorFormatDialog() override;

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
         * Method you can use to obtain the current parenthesis style.
         *
         * \return Returns the current parenthesis style.  The value ParenthesisStyle::INVALID is returned if a single
         *         parenthesis style has not been selected.
         */
        ParenthesisStyle parenthesisStyle() const;

        /**
         * Method you can use to determine if parenthesis should be forced.
         *
         * \return Returns true if parenthesis should be forced.  Returns false if parenthesis should not be forced.
         */
        bool forceParenthesis() const;

        /**
         * method you can use to determine if the force parenthesis value has been selected.
         *
         * \return Returns true if the force parenthesis value has been selected.
         */
        bool forceParenthesisSet() const;

        /**
         * Method you can use to obtain the current parameter parenthesis style.
         *
         * \return Returns the current parameter parenthesis style.  The value ParenthesisStyle::INVALID is returned
         *         if a single parenthesis style has not been selected.
         */
        ParenthesisStyle parameterParenthesisStyle() const;

        /**
         * Method you can use to determine if parameter parenthesis should be forced.
         *
         * \return Returns true if parenthesis should be forced.  Returns false if parenthesis should not be forced.
         */
        bool forceParameterParenthesis() const;

        /**
         * method you can use to determine if the force parameter parenthesis value has been selected.
         *
         * \return Returns true if the force parenthesis value has been selected.
         */
        bool forceParameterParenthesisSet() const;

        /**
         * Method you can use to determine if the first parameter should be subscripted.
         *
         * \return Returns true if the first function parameter should be subscripted.  Returns false if the first
         *         function parameter should *not* be subscripted.
         */
        bool subscriptedFirstParameter() const;

        /**
         * method you can use to determine if subscripted first parameter checkbox is set rather than tri-stated.
         *
         * \return Returns true if the subscripted first parameter checkbox is checked or unchecked.
         */
        bool subscriptedFirstParameterSet() const;

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
         * Method you can use to determine if the parameter parenthesis buttons are visible or hidden.
         *
         * \return Returns true if the parameter parenthesis buttons are visible.  Returns false if the parameter
         *         parenthesis buttons are hidden.
         */
        bool parameterParenthesisButtonsVisible();

        /**
         * Method you can use to determine if the parameter parenthesis buttons are hidden or visible.
         *
         * \return Returns true if the parameter parenthesis buttons are hidden.  Returns false if the parameter
         *         parenthesis buttons are visible.
         */
        bool parameterParenthesisButtonsHidden();

        /**
         * Method you can use to determine if the force parameter parenthesis checkbox is visible or hidden.
         *
         * \return Returns true if the force parameter parenthesis checkbox is visible.  Returns false if the force
         *         parameter parenthesis checkbox is hidden.
         */
        bool forceParameterParenthesisCheckboxVisible();

        /**
         * Method you can use to determine if the force parameter parenthesis checkbox is hidden or visible.
         *
         * \return Returns true if the force parameter parenthesis checkbox is hidden.  Returns false if the force
         *         parameter parenthesis checkbox is visible.
         */
        bool forceParameterParenthesisCheckboxHidden();

        /**
         * Method you can use to determine if the subscripted first parameter checkbox is visible or hidden.
         *
         * \return Returns true if the subscripted first parameter checkbox is visible.  Returns false if the
         *         subscripted first parameter checkbox is hidden.
         */
        bool subscriptedFirstParameterCheckboxVisible();

        /**
         * Method you can use to determine if the subscripted first parameter checkbox is hidden or visible.
         *
         * \return Returns true if the subscripted first parameter checkbox is hidden.  Returns false if the
         *         subscripted first parameter checkbox is visible.
         */
        bool subscriptedFirstParameterCheckboxHidden();

        /**
         * Method you can use to obtain the currently selected multiplication symbol style.
         *
         * \return Returns the current multiplication symbol.  A value of MultiplicationSymbol::INVALID is returned
         *         if a single multiplication symbol has not been selected.
         */
        MultiplicationSymbol multiplicationSymbol() const;

        /**
         * Method you can use to determine if multipication symbols are visible.
         *
         * \return Returns true if the multiplication symbols are visible.  Returns false if the multiplication symbols
         *         are hidden.
         */
        bool multiplicationSymbolsVisible() const;

        /**
         * Method you can use to determine if multipication symbols are hidden.
         *
         * \return Returns true if the multiplication symbols are hidden.  Returns false if the multiplication symbols
         *         are visible.
         */
        bool multiplicationSymbolsHidden() const;

        /**
         * Method you can use to obtain the currently selected division symbol style.
         *
         * \return Returns the current division symbol.  A value of DivisionSymbol::INVALID is returned if a single
         *         division symbol has not been selected.
         */
        DivisionSymbol divisionSymbol() const;

        /**
         * Method you can use to determine if multipication symbols are visible.
         *
         * \return Returns true if the division symbols are visible.  Returns false if the division symbols are hidden.
         */
        bool divisionSymbolsVisible() const;

        /**
         * Method you can use to determine if multipication symbols are hidden.
         *
         * \return Returns true if the division symbols are hidden.  Returns false if the division symbols are visible.
         */
        bool divisionSymbolsHidden() const;

        /**
         * Method you can use to obtain the currently selected division symbol style.
         *
         * \return Returns the current division symbol.  A value of LogicalNotSymbol::INVALID is returned if a single
         *         division symbol has not been selected.
         */
        LogicalNotSymbol logicalNotSymbol() const;

        /**
         * Method you can use to determine if multipication symbols are visible.
         *
         * \return Returns true if the division symbols are visible.  Returns false if the division symbols are hidden.
         */
        bool logicalNotSymbolsVisible() const;

        /**
         * Method you can use to determine if multipication symbols are hidden.
         *
         * \return Returns true if the division symbols are hidden.  Returns false if the division symbols are visible.
         */
        bool logicalNotSymbolsHidden() const;

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
        void apply(OperatorFormatDialog* thisDialog);

    public slots:
        /**
         * Slot you can use to set the selected parenthesis styles.
         *
         * \param[in] newParenthesisStyles The parenthesis styles to be selected.
         */
        void setParenthesisStyles(const QSet<ParenthesisStyle> newParenthesisStyles);

        /**
         * Slot you can use to select a single parenthesis style.
         *
         * \param[in] newParenthesisStyle The parenthesis style to be selected.
         */
        void setParenthesisStyle(ParenthesisStyle newParenthesisStyle);

        /**
         * Slot you can use to set the force parenthesis checkbox to be checked or unchecked.
         *
         * \param[in] nowChecked If true, the checkbox will be checked.  If false, the checkbox will be unchecked.
         */
        void setForceParenthesisChecked(bool nowChecked = true);

        /**
         * Slot you can use to set the force parenthesis checkbox to be unchecked or checked.
         *
         * \param[in] nowUnchecked If true, the checkbox will be unchecked.  If false, the checkbox will be checked.
         */
        void setForceParenthesisUnchecked(bool nowUnchecked = true);

        /**
         * Slot you can use to set the force parenthesis tristated.
         */
        void setForceParenthesisTristated();

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
         * Method you can use to set the currently selected parameter parenthesis style.
         *
         * \param[in] newParenthesisStyle The newly selected parameter parenthesis style.
         */
        void setParameterParenthesisStyle(ParenthesisStyle newParenthesisStyle);

        /**
         * Method you can use to set the currently selected parameter parenthesis styles.
         *
         * \param[in] newParenthesisStyles The newly selected parameter parenthesis styles.
         */
        void setParameterParenthesisStyle(const QSet<ParenthesisStyle> newParenthesisStyles);

        /**
         * Method you can use to specify if parameter parenthesis are forced.
         *
         * \param[in] nowChecked If true, the force parameter parenthesis checkbox will be checked.  If false, the
         *                       force parameter parenthesis checkbox will be unchecked.
         */
        void setForceParameterParenthesisChecked(bool nowChecked = true);

        /**
         * Method you can use to specify if parameter parenthesis are forced.
         *
         * \param[in] nowUnchecked If true, the force parameter parenthesis checkbox will be unchecked.  If false, the
         *                         force parameter parenthesis checkbox will be checked.
         */
        void setForceParameterParenthesisUnchecked(bool nowUnchecked = true);

        /**
         * Method you can use to specify if the force parameter parenthesis checkbox should be tri-stated.
         */
        void setForceParameterParenthesisTristated();

        /**
         * Method you can use to specify if the first parameter should be subscripted.
         *
         * \param[in] nowChecked If true, the subscripted first parameter checkbox will be checked.  If false, the
         *                       subscripted first parameter checkbox will be unchecked.
         */
        void setSubscriptedFirstParameterChecked(bool nowChecked = true);

        /**
         * Method you can use to specify if the first parameter should be subscripted.
         *
         * \param[in] nowUnchecked If true, the subscripted first parameter checkbox will be unchecked.  If false, the
         *                         subscripted first parameter checkbox will be checked.
         */
        void setSubscriptedFirstParameterUnchecked(bool nowUnchecked = true);

        /**
         * Method you can use to specify if subscripted first parameter checkbox should be tri-stated.
         */
        void setSubscriptedFirstParameterTristated();

        /**
         * Slot you can use to show or hide the parameter parenthesis style buttons.
         *
         * \param[in] nowVisible If true, the buttons will be visible.  If false, the buttons will be hidden.
         */
        void setParameterParenthesisButtonsVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide or show the parameter parenthesis style buttons.
         *
         * \param[in] nowHidden If true, the buttons will be hidden.  If false, the buttons will be visible.
         */
        void setParameterParenthesisButtonsHidden(bool nowHidden = true);

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
         * Slot you can use to show or hide the subscripted first parameter checkbox.
         *
         * \param[in] nowVisible If true, the checkbox will be visible.  If false, the checkbox will be hidden.
         */
        void setSubscriptedFirstParameterCheckboxVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide or show the subscripted first parameter checkbox.
         *
         * \param[in] nowHidden If true, the checkbox will be hidden.  If false, the checkbox will be visible.
         */
        void setSubscriptedFirstParameterCheckboxHidden(bool nowHidden = true);

        /**
         * Slot you can use to set the selected multiplication symbols.
         *
         * \param[in] newMultiplicationSymbols The multiplication symbols to be selected.
         */
        void setMultiplicationSymbols(const QSet<MultiplicationSymbol> newMultiplicationSymbols);

        /**
         * Slot you can use to select a single multiplication symbol.
         *
         * \param[in] newMultiplicationSymbol The multiplication symbol to be selected.
         */
        void setMultiplicationSymbol(MultiplicationSymbol newMultiplicationSymbol);

        /**
         * Slot you can use to show/hide multiplication symbols.
         *
         * \param[in] nowVisible If true, multiplication symbols will be visible.  If false, multiplication symbols
         *                       will be hidden.
         */
        void setMultiplicationSymbolsVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide/show multiplication symbols.
         *
         * \param[in] nowHidden If true, multiplication symbols will be hidden.  If false, multiplication symbols
         *                      will be visible.
         */
        void setMultiplicationSymbolsHidden(bool nowHidden = true);

        /**
         * Slot you can use to set the selected division symbols.
         *
         * \param[in] newDivisionSymbols The division symbols to be selected.
         */
        void setDivisionSymbols(const QSet<DivisionSymbol> newDivisionSymbols);

        /**
         * Slot you can use to select a single division symbol.
         *
         * \param[in] newDivisionSymbol The division symbol to be selected.
         */
        void setDivisionSymbol(DivisionSymbol newDivisionSymbol);

        /**
         * Slot you can use to show/hide division symbols.
         *
         * \param[in] nowVisible If true, division symbols will be visible.  If false, division symbols will be hidden.
         */
        void setDivisionSymbolsVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide/show division symbols.
         *
         * \param[in] nowHidden If true, division symbols will be hidden.  If false, division symbols will be visible.
         */
        void setDivisionSymbolsHidden(bool nowHidden = true);

        /**
         * Slot you can use to set the selected logical not symbols.
         *
         * \param[in] newLogicalNotSymbols The logical not symbols to be selected.
         */
        void setLogicalNotSymbols(const QSet<LogicalNotSymbol> newLogicalNotSymbols);

        /**
         * Slot you can use to select a single logical not symbol.
         *
         * \param[in] newLogicalNotSymbol The logical not symbol to be selected.
         */
        void setLogicalNotSymbol(LogicalNotSymbol newLogicalNotSymbol);

        /**
         * Slot you can use to show/hide logical not symbols.
         *
         * \param[in] nowVisible If true, logical not symbols will be visible.  If false, logical not symbols will be
         *                       hidden.
         */
        void setLogicalNotSymbolsVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide/show logical not symbols.
         *
         * \param[in] nowHidden If true, logical not symbols will be hidden.  If false, logical not symbols will be
         *                      visible.
         */
        void setLogicalNotSymbolsHidden(bool nowHidden = true);

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
         * Slot that is triggered when the default parenthesis tool button is clicked.
         */
        void defaultParenthesisButtonClicked();

        /**
         * Slot that is triggered when the parenthesis tool button is clicked.
         */
        void parenthesisToolButtonClicked();

        /**
         * Slot that is triggered when the brackets tool button is clicked.
         */
        void bracketsToolButtonClicked();

        /**
         * Slot that is triggered when the braces tool button is clicked.
         */
        void bracesToolButtonClicked();

        /**
         * Slot that is triggered when the force parenthesis check box is clicked.
         */
        void forceParenthesisCheckBoxClicked();

        /**
         * Slot that is triggered when the parameter parenthesis tool button is clicked.
         */
        void parameterParenthesisToolButtonClicked();

        /**
         * Slot that is triggered when the parameter brackets tool button is clicked.
         */
        void parameterBracketsToolButtonClicked();

        /**
         * Slot that is triggered when the parameter braces tool button is clicked.
         */
        void parameterBracesToolButtonClicked();

        /**
         * Slot that is triggered when the force parameter parenthesis check box is clicked.
         */
        void forceParameterParenthesisCheckBoxClicked();

        /**
         * Slot that is triggered when the subscript first parameter check box is clicked.
         */
        void subscriptFirstParameterCheckBoxClicked();

        /**
         * Slot that is triggered when the default multiplication style button is clicked.
         */
        void defaultMultiplicationStyleToolButtonClicked();

        /**
         * Slot that is triggered when the space multiplication style button is clicked.
         */
        void spaceMultiplicationStyleToolButtonClicked();

        /**
         * Slot that is triggered when the dot multiplication style button is clicked.
         */
        void dotMultiplicationStyleToolButtonClicked();

        /**
         * Slot that is triggered when the cross multiplication style button is clicked.
         */
        void crossMultiplicationStyleToolButtonClicked();

        /**
         * Slot that is triggered when the circle multiplication style button is clicked.
         */
        void circleMultiplicationStyleToolButtonClicked();

        /**
         * Slot that is triggered when the default division style button is clicked.
         */
        void defaultDivisionStyleToolButtonClicked();

        /**
         * Slot that is triggered when the obelus division style button is clicked.
         */
        void obelusDivisionStyleToolButtonClicked();

        /**
         * Slot that is triggered when the fraction division style button is clicked.
         */
        void fractionDivisionStyleToolButtonClicked();

        /**
         * Slot that is triggered when the default logical not style button is clicked.
         */
        void defaultLogicalNotStyleToolButtonClicked();

        /**
         * Slot that is triggered when the negation logical not style button is clicked.
         */
        void negationLogicalNotStyleToolButtonClicked();

        /**
         * Slot that is triggered when the overbar logical not style button is clicked.
         */
        void overbarLogicalNotStyleToolButtonClicked();

        /**
         * Slot that is triggered when the tilde logical not style button is clicked.
         */
        void tildeLogicalNotStyleToolButtonClicked();

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
         * Enumeration of force parenthesis checkbox states.
         */
        enum class Tristate {
            /**
             * Checkbox is checked.
             */
            CHECKED,

            /**
             * Checkbox is unchecked.
             */
            UNCHECKED,

            /**
             * Checkbox is tri-stated.
             */
            TRISTATED
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
         * Method that sets all buttons in a list to the same (square) size.
         *
         * \param[in] buttons The list of buttons to be sized.
         */
        void setButtonSizes(const QList<QToolButton*>& buttons);

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

        /**
         * The input parenthesis styles.
         */
        QSet<ParenthesisStyle> inputParenthesisStyles;

        /**
         * Flag indicating if the default parenthesis style should be visible or hidden.
         */
        bool inputDefaultParenthesisStyleVisible;

        /**
         * The input forced parenthesis styles.
         */
        Tristate inputForceParenthesisState;

        /**
         * Flag that indicates if we should show or hide the force parenthesis checkbox.
         */
        bool inputForceParenthesisVisible;

        /**
         * The input parameter parenthesis styles.
         */
        QSet<ParenthesisStyle> inputParameterParenthesisStyles;

        /**
         * Flag that indicates if we should show the parameter parenthesis styles.
         */
        bool inputParameterParenthesisStylesVisible;

        /**
         * The input forced parameter parenthesis styles.
         */
        Tristate inputForceParameterParenthesisState;

        /**
         * Flag that indicates if we should show or hide the force parameter parenthesis checkbox.
         */
        bool inputForceParameterParenthesisVisible;

        /**
         * The use subscripted first parameter checkbox state.
         */
        Tristate inputUseSubscriptedFirstParameterState;

        /**
         * Flag that indicates the use subscripted first parameter checkbox should be visible.
         */
        bool inputUseSubscriptedFirstParameterVisible;

        /**
         * Flag that indicates if we should show or hide multiplication symbols.
         */
        bool inputMultiplicationSymbolsVisible;

        /**
         * The input multiplication symbols.
         */
        QSet<MultiplicationSymbol> inputMultiplicationSymbols;

        /**
         * Flag that indicates if we should show or hide division symbols.
         */
        bool inputDivisionSymbolsVisible;

        /**
         * The input division symbols.
         */
        QSet<DivisionSymbol> inputDivisionSymbols;

        /**
         * Flag that indicates if we should show or hide the logical not symbols.
         */
        bool inputLogicalNotSymbolsVisible;

        /**
         * The input logical not symbols.
         */
        QSet<LogicalNotSymbol> inputLogicalNotSymbols;
};

#endif
