/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref IntegerInspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef INTEGER_INSPECTOR_WIDGET_H
#define INTEGER_INSPECTOR_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QRadioButton>
#include <QRegularExpression>

#include <ld_calculated_value.h>

#include "app_common.h"
#include "inspector_widget.h"

namespace Ld {
    class CalculatedValue;
};

/**
 * Inspector widget for integer variables.
 */
class APP_PUBLIC_API IntegerInspectorWidget:public InspectorWidget {
    Q_OBJECT

    public:
        /**
         * Factory function
         *
         * \param[in] parent The parent widget.
         */
        static InspectorWidget* creator(QWidget* parent);

        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent widget.
         */
        IntegerInspectorWidget(QWidget* parent = Q_NULLPTR);

        ~IntegerInspectorWidget() override;

        /**
         * Method you can call to obtain the currently displayed calculated value.
         *
         * \return Returns the currently displayed calculated value.
         */
        Ld::CalculatedValue calculatedValue() const override;

        /**
         * method you can call to determine if the currently displayed calculated value is modified.
         *
         * \return Returns true if the currently displayed calculated value is different from the original calculated
         *         value.  Returns false if the calculated value matches the original calculated value.
         */
        bool calculatedValueChanged() const override;

    public slots:
        /**
         * Pure virtual slot you should overload to receive the calculated value to be displayed by this widget.
         *
         * \param[in] calculatedValue The calculated value to display in this widget.
         */
        void setCalculatedValue(const Ld::CalculatedValue& calculatedValue) override;

        /**
         * Pure virtual slot you should overload to receive notification that the calculated value should be reset.
         */
        void resetCalculatedValue() override;

    private slots:
        /**
         * Slot that is triggered when the line editor contents have been edited by the user.
         *
         * \param[in] text The new text.
         */
        void textEdited(const QString& text);

        /**
         * Slot that is triggered when a new radix is selected.
         */
        void newRadixSelected();

    private:
        /**
         * The regular expression used to test if an entered value is correct.
         */
        static const QRegularExpression regularExpression;

        /**
         * Method that determines the radix and value of the currently entered string.
         *
         * \param[in]  text  The text to be converted.
         *
         * \param[out] radix The radix.  A radix of 0 will be returned on error.
         *
         * \return Returns the value of the entered string.
         */
        static long long valueAndRadixOf(const QString& text, unsigned& radix);

        /**
         * Method that returns the currently selected radix.
         *
         * \return Returns the currently selected radix.
         */
        unsigned selectedRadix() const;

        /**
         * Method that displays a value.
         *
         * \param[in] value The value to be displayed.
         */
        void displayValue(long long value);

        /**
         * The input value.
         */
        Ld::CalculatedValue currentCalculatedValue;

        /**
         * The line editor used to display the current integer value.
         */
        QLineEdit* integerLineEditor;

        /**
         * Radio button used to request base 2 values.
         */
        QRadioButton* base2RadioButton;

        /**
         * Radio button used to request base 10 values.
         */
        QRadioButton* base10RadioButton;

        /**
         * Radio button used to request base 16 values.
         */
        QRadioButton* base16RadioButton;
};

#endif
