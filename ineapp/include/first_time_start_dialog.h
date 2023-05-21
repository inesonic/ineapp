/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FirstTimeStartDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FIRST_TIME_START_DIALOG_H
#define FIRST_TIME_START_DIALOG_H

#include <eqt_programmatic_dialog.h>

#include "app_common.h"

class QWidget;

/**
 * First time start dialog.
 */
class APP_PUBLIC_API FirstTimeStartDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] usageStatisticsUrl The URL where usage statistics information is documented.
         *
         * \param[in] parent             Pointer to the parent object.
         */
        FirstTimeStartDialog(QString const& usageStatisticsUrl, QWidget* parent = Q_NULLPTR);

        ~FirstTimeStartDialog() override;

        /**
         * Method you can use to determine if collection of usage statistics should be enabled or disabled.
         *
         * \return Returns true if usage statistics should be enabled.  Returns false if usage statistics should be
         *         disabled.
         */
        bool usageStatisticsEnabled() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

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
         * Slot that is triggered when the user enables or disables usage statistics.
         *
         * \param[in] checked Flag indicating if the button is checked. The value is ignored.
         */
        void usageStatisticsValueChanged(bool checked);

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * The URL documenting usage statistics, in detail.
         */
        QString currentUsageStatisticsUrl;
};

#endif
