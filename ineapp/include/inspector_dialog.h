/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref InspectorDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef INSPECTOR_DIALOG_H
#define INSPECTOR_DIALOG_H

#include <QSet>
#include <QSharedPointer>

#include <eqt_programmatic_dialog.h>

#include <ld_calculated_value.h>

#include "app_common.h"

class QWidget;
class QToolButton;

namespace Ld {
    class PageFormat;
};

/**
 * Dialog used to inspect and modify variable values.
 */
class APP_PUBLIC_API InspectorDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        InspectorDialog(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] calculatedValue The calculated value.
         *
         * \param[in] parent          Pointer to the parent object.
         */
        InspectorDialog(const Ld::CalculatedValue& calculatedValue, QWidget* parent = Q_NULLPTR);

        ~InspectorDialog() override;

        /**
         * Method you can call to determine if the OK button is always enabled.
         *
         * \return Returns true if the OK button is always enabled.  Returns false if the OK button is only
         *         conditionally enabled.
         */
        bool okAlwaysEnabled() const;

        /**
         * Method you can call to determine if the OK button is conditionally enabled.
         *
         * \return Returns true if the OK button is conditionally enabled.  Returns false if the OK button is always
         *         enabled.
         */
        bool okConditionallyEnabled() const;

        /**
         * Method you can use to obtain the calculated value used by the dialog.
         *
         * \return Returns the calculated value currently tied to the dialog.
         */
        Ld::CalculatedValue calculatedValue() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:

    public slots:
        /**
         * Slot you can trigger to set the calculated value to be operated on.
         *
         * \param[in] calculatedValue The calculated value to apply to the dialog.
         */
        void setCalculatedValue(const Ld::CalculatedValue& calculatedValue);

        /**
         * Slot you can trigger to force the OK button to always be enabled.
         *
         * \param[in] nowAlwaysEnabled If true, the OK button will always be enabled.  If false, the OK button will
         *                             be conditionally enabled.
         */
        void setOkAlwaysEnabled(bool nowAlwaysEnabled = true);

        /**
         * Slot you can trigger to force the OK button to be conditionally enabled.
         *
         * \param[in] nowConditionallyEnabled If true, the OK button will always be enabled.  If false, the OK button
         *                                    will be conditionally enabled.
         */
        void setOkConditionallyEnabled(bool nowConditionallyEnabled = true);

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
         * Slot that is triggered when the reset button is clicked.
         */
        void resetClicked();

        /**
         * Slot that is triggered when the calculated value is updated by the user.
         *
         * \param[in] newCalculatedValue The newly changed calculated value.
         */
        void valueChanged(const Ld::CalculatedValue& newCalculatedValue);

        /**
         * Slot that is triggered when the calculated value is reset to its original value.
         *
         * \param[in] originalCalculatedValue The restored calculated value.
         */
        void valueRestored(const Ld::CalculatedValue& originalCalculatedValue);

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Flag that indicates if the OK button should always be enabled.
         */
        bool currentOkAlwaysEnabled;

        /**
         * The calculated value this dialog is operating on.
         */
        Ld::CalculatedValue currentCalculatedValue;
};

#endif
