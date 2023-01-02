/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref AboutDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <eqt_programmatic_dialog.h>

#include "app_common.h"

class QWidget;
class QVBoxLayout;

/**
 * Dialog used to display information about this application.
 */
class APP_PUBLIC_API AboutDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        AboutDialog(QWidget* parent = Q_NULLPTR);

        ~AboutDialog() override;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

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
         * Slot that is triggered when the user requests information on plug-ins.
         */
        void listPlugIns();

    private:
        /**
         * The desired logo width, in inches.  Height will be scaled accordingly.
         */
        static constexpr float logoWidth = 3.5;

        /**
         * The desired product name point size.
         */
        static constexpr unsigned productNamePointSize = 16;

        /**
         * The desired point size for the catch phrase.
         */
        static constexpr unsigned catchPhrasePointSize = 12;

        /**
         * The desired point size for the version information.
         */
        static constexpr unsigned versionPointSize = 10;

        /**
         * The desired company name point size.
         */
        static constexpr unsigned companyNamePointSize = 16;

        /**
         * The desired company address point size.
         */
        static constexpr unsigned companyAddressPointSize = 10;

        /**
         * The desired company URL point size.
         */
        static constexpr unsigned companyUrlPointSize = 10;

        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Method that builds the application logo and related details.
         *
         * \param[in] layout The layout to be filled in.
         */
        void buildLogo(QVBoxLayout* layout);

        /**
         * Method that builds the company details.
         *
         * \param[in] layout The layout to be filled in.
         */
        void buildCompanyData(QVBoxLayout* layout);

        /**
         * Called to build up details related to the application.
         *
         * \param[in] layout The layout to be filled in.
         */
        void buildLicensingDetails(QVBoxLayout* layout);

        /**
         * Method that determines if there are any plug-ins.
         *
         * \return Returns true if one or more plug-ins are loaded.  Returns false if there are no available plug-ins.
         */
        bool plugInsLoaded();
};

#endif
