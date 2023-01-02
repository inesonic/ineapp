/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TextPresentationHelper class.
***********************************************************************************************************************/

#include <QString>
#include <QFontMetricsF>

#include <algorithm>
#include <cmath>

#include "text_presentation_helper.h"

TextPresentationHelper::TextPresentationHelper() {}


TextPresentationHelper::~TextPresentationHelper() {}


unsigned long TextPresentationHelper::locateNextStart(const QString& text, unsigned long startingIndex) {
    unsigned long index  = startingIndex;
    unsigned long length = static_cast<unsigned long>(text.length());

    while(index < length && text.at(index).isSpace()) {
        ++index;
    }

    return index;
}


double TextPresentationHelper::textWidth(
        const QString&       text,
        const QFontMetricsF& fontMetrics,
        unsigned long        startingIndex,
        unsigned long        endingIndex
    ) {
    QString substring = text.mid(startingIndex, endingIndex - startingIndex);
    return fontMetrics.horizontalAdvance(substring);
}


unsigned long TextPresentationHelper::maximumEndingIndex(
        const QString&       text,
        const QFontMetricsF& fontMetrics,
        double               maximumWidth,
        unsigned long        startingIndex
    ) {
    unsigned long endingIndex = 0;

    if (!text.isEmpty()) {
        unsigned long upper      = static_cast<unsigned>(text.length());
        double        upperWidth = textWidth(text, fontMetrics, startingIndex, upper);

        if (upperWidth < maximumWidth) {
            endingIndex = upper;
        } else {
            unsigned long lower      = startingIndex;
            double        lowerWidth = 0;

            double        endingWidth = lowerWidth;

            while (upper - lower > 1) {
                endingIndex        = (upper + lower) / 2;
                double endingWidth = textWidth(text, fontMetrics, startingIndex, endingIndex);

                if (endingWidth < maximumWidth) {
                    lower      = endingIndex;
                    lowerWidth = endingWidth;
                } else {
                    upper      = endingIndex;
                    upperWidth = endingWidth;
                }
            }

            if (endingWidth > maximumWidth) {
                endingIndex = 0;
            }
        }
    }

    return endingIndex;
}


unsigned long TextPresentationHelper::bestSplitBefore(const QString& text, unsigned long initialEndingIndex) {
    unsigned long index = initialEndingIndex;
    if (index > 0) {
        while (index > 0 && !text.at(index - 1).isSpace()) {
            --index;
        }

        while (index > 0 && text.at(index - 1).isSpace()) {
            --index;
        }
    }

    return index;
}
