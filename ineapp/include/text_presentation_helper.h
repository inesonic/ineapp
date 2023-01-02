/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TextPresentationHelper class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TEXT_PRESENTATION_HELPER_H
#define TEXT_PRESENTATION_HELPER_H

#include <QString>
#include <QFontMetricsF>

#include "app_common.h"

/**
 * This class provides a small number of helper methods you can use to facilitate presenting text.
 */
class APP_PUBLIC_API TextPresentationHelper {
    public:
        TextPresentationHelper();

        ~TextPresentationHelper();

        /**
         * Method you can call to locate the first non-whitespace character.
         *
         * \param[in] text          The text to scan.
         *
         * \param[in] startingIndex The zero based index of the first character position to check.
         *
         * \return Returns the location of the first non-whitespace character.  The line length is returned if no
         *         non-whitespace characters are found.
         */
        static unsigned long locateNextStart(const QString& text, unsigned long startingIndex = 0);

        /**
         * Method that determines the width of a subset of a string, in scene units.
         *
         * \param[in] text          The text to examine.
         *
         * \param[in] fontMetrics   The font metrics class to use to calculate widths.
         *
         * \param[in] startingIndex The starting index (inclusive).
         *
         * \param[in] endingIndex   The ending index (exclusive).
         *
         * \return Returns the width of the substring, in scene units.
         */
        static double textWidth(
            const QString&       text,
            const QFontMetricsF& fontMetrics,
            unsigned long        startingIndex,
            unsigned long        endingIndex
        );

        /**
         * Method that locates the largest substring that is less than a given width, in scene units.
         *
         * \param[in] text          The text to examine.
         *
         * \param[in] fontMetrics   The font metrics class to use to calculate widths.
         *
         * \param[in] maximumWidth  The maximum width we can support, in scene units.
         *
         * \param[in] startingIndex The starting index (inclusive).
         *
         * \return Returns the zero based ending index (exclusive).  A value of 0 is returned if no ending index could
         *         be found that meets the criteria.
         */
        static unsigned long maximumEndingIndex(
            const QString&       text,
            const QFontMetricsF& fontMetrics,
            double               maximumWidth,
            unsigned long        startingIndex = 0
        );

        /**
         * Method that walks backwards from a location to identify the closest split before the location.
         *
         * \param[in] text               The text to examine.
         *
         * \param[in] initialEndingIndex The ending index to start searching from (exclusive).
         *
         * \return Returns the suggested best ending index (inclusive) for the split in question.
         */
        static unsigned long bestSplitBefore(const QString& text, unsigned long initialEndingIndex);
};

#endif
