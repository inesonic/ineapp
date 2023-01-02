/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RealInspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef REAL_INSPECTOR_WIDGET_H
#define REAL_INSPECTOR_WIDGET_H

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
 * Inspector widget for real variables.
 */
class APP_PUBLIC_API RealInspectorWidget:public InspectorWidget {
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
        RealInspectorWidget(QWidget* parent = Q_NULLPTR);

        ~RealInspectorWidget() override;

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

    private:
        /**
         * The input value.
         */
        Ld::CalculatedValue currentCalculatedValue;

        /**
         * The line editor used to display the current real value.
         */
        QLineEdit* realLineEditor;
};

#endif
