/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ComplexInspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMPLEX_INSPECTOR_WIDGET_H
#define COMPLEX_INSPECTOR_WIDGET_H

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
 * Inspector widget for complex variables.
 */
class APP_PUBLIC_API ComplexInspectorWidget:public InspectorWidget {
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
        ComplexInspectorWidget(QWidget* parent = Q_NULLPTR);

        ~ComplexInspectorWidget() override;

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
         * Slot that is triggered when the real portion line editor contents have been edited by the user.
         *
         * \param[in] text The new text.
         */
        void realTextEdited(const QString& text);

        /**
         * Slot that is triggered when the complex portion line editor contents have been edited by the user.
         *
         * \param[in] text The new text.
         */
        void imaginaryTextEdited(const QString& text);

    private:
        /**
         * Method that triggers the appropriate signals when a value is changed.
         */
        void generateSignals();

        /**
         * The input value.
         */
        Ld::CalculatedValue currentCalculatedValue;

        /**
         * The line editor used to display the current real value.
         */
        QLineEdit* realLineEditor;

        /**
         * The line editor used to display the current imaginary value.
         */
        QLineEdit* imaginaryLineEditor;
};

#endif
