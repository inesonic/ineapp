/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DesktopServices class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DESKTOP_SERVICES_H
#define DESKTOP_SERVICES_H

#include <QString>
#include <QDesktopServices>

#include <crypto_xtea_encryptor.h>

#include "app_common.h"

/**
 * Thin wrapper on the Qt QDesktopServices class that adds application specific functions.
 */
class APP_PUBLIC_API DesktopServices:public QDesktopServices {
    public:
        /**
         * Method that can be caled to open up the main help page.
         *
         * \return Returns True on success.  Returns false on error.
         */
        static bool openHelp();

        /**
         * Method that can be called to open up the getting started page.
         *
         * \return Returns True on success.  Returns false on error.
         */
        static bool openGettingStarted();

        /**
         * Method that can be called to open a new help page based on a keyword.
         *
         * \param[in] keyword The keyword that should be used to open the help page.
         *
         * \return Returns True on success.  Returns false on error.
         */
        static bool openHelpPage(const QString& keyword);

        /**
         * Method that can be called to search the on-line help.
         *
         * \param[in] searchString The string to use to search for information.
         *
         * \return Returns True on success.  Returns false on error.
         */
        static bool searchHelp(const QString& searchString);

        /**
         * Method that can be called to display information about a function.
         *
         * \param[in] functionIdentifier The function identifier to be searched.
         *
         * \return Returns True on success.  Returns false on error.
         */
        static bool openFunctionReference(const QString& functionIdentifier);

        /**
         * Method that can be called to open a web page to send customer feedback.
         *
         * \param[in] customerId The customer ID requesting support.
         *
         * \return Returns True on success.  Returns false on error.
         */
        static bool sendFeedback(unsigned long long customerId);
};

#endif
