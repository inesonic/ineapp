/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ApplicationPreferencesDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef APPLICATION_PREFERENCES_DIALOG_H
#define APPLICATION_PREFERENCES_DIALOG_H

#include <QSize>

#include <eqt_programmatic_dialog.h>

#include <ld_page_format.h>

#include "app_common.h"

class QWidget;

/**
 * Dialog used to display and manage application global preferences.
 */
class APP_PUBLIC_API ApplicationPreferencesDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        ApplicationPreferencesDialog(QWidget* parent = Q_NULLPTR);

        ~ApplicationPreferencesDialog() override;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    public slots:
        /**
         * Slot that can be used to indicate that the user accepted the dialog contents.
         */
        void accept() final;

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
         * Slot that is triggered when new information is entered into the document defaults tab.
         */
        void validateDocumentDefaultsTab();

        /**
         * Slot that is triggered when new information is entered into the document defaults tab.
         *
         * \param[in] index A dummy index.  Used to allow this signal to be intercepted from QComboBox objects.
         */
        void validateDocumentDefaultsTab(int index);

    private:
        /**
         * Minimum supported display DPI settings.
         */
        static const QSize minimumDisplayDpi;

        /**
         * Maximum supported display DPI settings.
         */
        static const QSize maximumDisplayDpi;

        /**
         * Method that builds the format defaults tab.
         */
        QWidget* buildDocumentDefaultsTab();

        /**
         * Method that builds the system settings tab.
         */
        QWidget* buildSystemSettingsTab();

        /**
         * Method that builds the privacy and security tab.
         */
        QWidget* buildPrivacyTab();

        /**
         * Method that builds the application defaults tab.
         */
        QWidget* buildApplicationDefaultsTab();

        /**
         * Method that builds the default dialog.
         */
        void configureDialog();

        /**
         * Method that generates a page layout based on the current user input.
         */
        Ld::PageFormat generatePageFormat();

        /**
         * Flag used to indicate that we've already called accept.  Qt 5.10 on MacOS appears to call "accept"
         * twice.
         */
        bool acceptCalled;
};

#endif
