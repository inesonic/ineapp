/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides key defines for the application.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef APPLICATION_SETTINGS_H
#define APPLICATION_SETTINGS_H

#include <QtGlobal>

#if (defined(USE_STAGING_SITE))
    #define __ORGANIZATION_DOMAIN ("staging.inesonic.com")
    #define __TIME_DELTA_WEBHOOK_URL ("https://autonoma.staging.inesonic.com/v2/ts")
    #define __USAGE_STATISTICS_WEBHOOK_URL ("https://autonoma.staging.inesonic.com/v2/usage_data")
    #define __PRIVACY_POLICY_URL ("https://staging.inesonic.com/privacy-policy/")
    #define __FEEDBACK_URL ("https://staging.inesonic.com/application-support-request/?key=%1")
#else
    #define __ORGANIZATION_DOMAIN ("inesonic.com")
    #define __TIME_DELTA_WEBHOOK_URL ("https://autonoma.inesonic.com/v2/ts")
    #define __USAGE_STATISTICS_WEBHOOK_URL ("https://autonoma.inesonic.com/v2/usage_data")
    #define __PRIVACY_POLICY_URL ("https://www.inesonic.com/privacy-policy/")
    #define __FEEDBACK_URL ("https://www.inesonic.com/application-support-request/?key=%1")
#endif

/**
 * Defines the application name used for settings and OS related services.
 */
#define APPLICATION_NAME ("Aion")

/**
 * Defines the company name the creates and sells this application.
 */
#define ORGANIZATION_NAME ("Inesonic, LLC")

/**
 * Defines the domain registered for this organization.
 */
#define ORGANIZATION_DOMAIN __ORGANIZATION_DOMAIN

/**
 * Defines the default file extension to use for files.
 */
#define FILE_EXTENSION ("aion")

/**
 * Inesonic EULA version number.
 */
#define INESONIC_EULA_VERSION_NUMBER (0)

/**
 * Third party licenses page number.
 */
#define THIRD_PARTY_LICENSES_VERSION_NUMBER (0)

/**
 * Secret used to adjust webhook timestamp values.
 */
#define TIME_DELTA_SECRET                                 \
    {                                                     \
        0x13, 0xDF, 0x36, 0x03,   0x22, 0xAD, 0x3A, 0x99, \
        0xBC, 0x8F, 0x38, 0x14,   0xDA, 0x35, 0x25, 0x16, \
        0xB8, 0x23, 0x5B, 0x3D,   0xB7, 0xA3, 0xDE, 0xF3, \
        0x5F, 0x46, 0x9D, 0x84,   0x21, 0x8A, 0x07, 0xB0, \
        0x50, 0xD4, 0xE9, 0xD2,   0xDE, 0x82, 0xC5, 0x58, \
        0x6C, 0xF9, 0x57, 0xB0,   0x4F, 0x09, 0x69, 0x4B, \
        0x7D, 0xE1, 0x0A, 0x8D,   0x9D, 0x88, 0x0B, 0xD0, \
        0x04, 0x7D, 0xA6, 0x99,   0x99, 0x17, 0x8C, 0x8D  \
    }

/**
 * The time delta webhook URL.
 */
#define TIME_DELTA_WEBHOOK_URL __TIME_DELTA_WEBHOOK_URL

/**
 * Usage statistics report interval, in seconds.
 */
#define USAGE_STATISTICS_REPORT_PERIOD (7*24*60*60)

/**
 * HMAC secret used for usage data reporting.
 */
#define USAGE_STATISTICS_HMAC_SECRET                      \
    {                                                     \
        0x1C, 0x36, 0xCD, 0xE7,   0x7D, 0x6D, 0x61, 0xB6, \
        0x88, 0xB8, 0x67, 0xED,   0x8E, 0x01, 0x6F, 0x8E, \
        0x0A, 0x77, 0x3C, 0x2F,   0x3E, 0x7B, 0x24, 0xB6, \
        0x04, 0xC6, 0x5A, 0xDE,   0xF4, 0xDA, 0x10, 0x7C, \
        0xFB, 0x57, 0xD5, 0xA5,   0x03, 0x52, 0xE8, 0x31, \
        0xDF, 0xAB, 0xAE, 0x3C,   0x04, 0xAF, 0x3D, 0xBE, \
        0x39, 0x40, 0x91, 0x42                            \
    }

/**
 * XTEA encryption key used to encrypt the customer ID when sending feedback.
 */
#define SEND_FEEDBACK_ENCRYPTION_KEYS                     \
    {                                                     \
        0x7F, 0xE8, 0x4C, 0xFD,   0x4C, 0x3B, 0xD8, 0x12, \
        0xD1, 0xBC, 0xCA, 0x2F,   0x3A, 0x94, 0xF0, 0x2C  \
    }

/**
 * HMAC hash used to authenticate the send feedback data.
 *
 * Base-64 encoded: "T0cYehbc5vaKp1Cuvo6sPpbC7DtX7nYpCNMZp9mma5MuX2ux6Qfb9q6812rQirQ1XxRdIQ=="
 */
#define SEND_FEEDBACK_HMAC_SECRET                         \
    {                                                     \
        0x4F, 0x47, 0x18, 0x7A,   0x16, 0xDC, 0xE6, 0xF6, \
        0x8A, 0xA7, 0x50, 0xAE,   0xBE, 0x8E, 0xAC, 0x3E, \
        0x96, 0xC2, 0xEC, 0x3B,   0x57, 0xEE, 0x76, 0x29, \
        0x08, 0xD3, 0x19, 0xA7,   0xD9, 0xA6, 0x6B, 0x93, \
        0x2E, 0x5F, 0x6B, 0xB1,   0xE9, 0x07, 0xDB, 0xF6, \
        0xAE, 0xBC, 0xD7, 0x6A,   0xD0, 0x8A, 0xB4, 0x35, \
        0x5F, 0x14, 0x5D, 0x21                            \
    }

/**
 * Usage statistics webhook URL.
 */
#define USAGE_STATISTICS_WEBHOOK_URL __USAGE_STATISTICS_WEBHOOK_URL

/**
 * Link to location explaining usage data.
 */
#define PRIVACY_POLICY_URL __PRIVACY_POLICY_URL

/**
 * Link to location providing details on usage statistics.
 */
#define USAGE_STATISTICS_URL ("")

/**
 * One line catch phrase to be shown in the About dialog.
 */
#define CATCH_PHRASE tr("Turning Creativity into Algorithm Intelligence")

/**
 * Company address as shown in the About dialog.
 */
#define ORGANIZATION_ADDRESS tr("9951 West Prairie Road\nBoise, Idaho  83714")

/**
 * \def RESOURCE_RELATIVE_PATH
 *
 * Relative path from the application executable directory to the application resources.  Note that this path is
 * platform dependent.
 */
#if (defined(Q_OS_WIN))

    #define RESOURCE_RELATIVE_PATH ("../resources/")

#elif (defined(Q_OS_LINUX))

    #define RESOURCE_RELATIVE_PATH ("../resources/")

#elif (defined(Q_OS_DARWIN))

    #define RESOURCE_RELATIVE_PATH ("../Resources/")

#else

    #define RESOURCE_RELATIVE_PATH
    #error Unknown platform

#endif

/**
 * The on-line help page main page.  Note that %1 should be replaced with the application version.
 */
#define HELP_MAIN_PAGE_URL ("https://documentation.inesonic.com/")

/**
 * The on-line help getting started page.  Note that %1 should be replaced with the application version.
 */
#define HELP_GETTING_STARTED_URL ("https://documentation.inesonic.com/tutorial/tutorial.html")

/**
 * The on-line help page based on keyword.  Note that %1 should be replaced with the application version and %2 should
 * be replaced with the help keyword.
 */
#define HELP_KEYWORD_URL ("https://documentation.inesonic.com/search.html/?q=%2&check_keywords=yes&area=default")

/**
 * The on-line help page used to reference function documentation.  This URL is a prefix used to point to the function
 * reference documentation.  Note that %1 should be replaced with the function identifier.
 */
#define HELP_FUNCTION_URL_PREFIX ("https://documentation.inesonic.com/reference_manual/function_%1.html")

/**
 * The on-line help page based on search string.  Note that %1 should be replaced with the application version and %2
 * should be replaced with the search string.
 */
#define HELP_SEARCH_URL ("https://documentation.inesonic.com/search.html?q=%1&check_keywords=yes&area=default")

/**
 * Feedback website.  Note that the customer ID key should be inserted as the argument.
 */
#define FEEDBACK_URL __FEEDBACK_URL

/**
 * The minimum allowed font size, in points.
 */
#define MINIMUM_FONT_SIZE_IN_POINTS (6)

/**
 * The step size for fonts, in points.
 */
#define FONT_STEP_SIZE_IN_POINTS (2)

#endif
