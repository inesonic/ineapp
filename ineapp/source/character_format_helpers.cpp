/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements several functions used to help manage character and fotn formats.
***********************************************************************************************************************/

#include <QSet>
#include <QSharedPointer>

#include <eqt_font_data.h>

#include <ld_format_structures.h>
#include <ld_format.h>
#include <ld_font_format.h>
#include <ld_character_format.h>

#include "character_format_helpers.h"

QSet<Ld::FontFormat::Weight> supportedFontWeights() {
    QSet<Ld::FontFormat::Weight> result;
    result << Ld::FontFormat::Weight::THIN
           << Ld::FontFormat::Weight::EXTRA_LIGHT
           << Ld::FontFormat::Weight::LIGHT
           << Ld::FontFormat::Weight::NORMAL
           << Ld::FontFormat::Weight::MEDIUM
           << Ld::FontFormat::Weight::DEMI_BOLD
           << Ld::FontFormat::Weight::BOLD
           << Ld::FontFormat::Weight::EXTRA_BOLD
           << Ld::FontFormat::Weight::BLACK;

    return result;
}


QSet<Ld::FontFormat::Weight> toFontFormatWeights(const QSet<QFont::Weight>& qFontSet) {
    QSet<Ld::FontFormat::Weight> result;

    for (  QSet<QFont::Weight>::const_iterator it  = qFontSet.constBegin(),
                                               end = qFontSet.constEnd()
         ; it != end
         ; ++it
        ) {
        result << Ld::toFontFormatWeight(*it);
    }

    return result;

}

QSet<Ld::FontFormat::Weight> supportedFontWeights(QSharedPointer<Ld::FontFormat> fontFormat) {
    QSet<QFont::Weight> supportedWeights = EQt::FontData::uniqueWeights(fontFormat->toQFont());
    return toFontFormatWeights(supportedWeights);
}


QSet<Ld::FontFormat::Weight> supportedFontWeights(const Ld::FontFormat::Aggregation& aggregation) {
    QSet<Ld::FontFormat::Weight> result  = supportedFontWeights();
    Ld::FormatPointerSet         formats = aggregation.formats();

    for (  Ld::FormatPointerSet::const_iterator it = formats.constBegin(), end = formats.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::FontFormat> fontFormat = it->dynamicCast<Ld::FontFormat>();
        result.intersect(supportedFontWeights(fontFormat));
    }

    return result;
}


Ld::FontFormat::Weight bestWeight(bool nowBold, const QSet<Ld::FontFormat::Weight>& supportedFontWeights) {
    Ld::FontFormat::Weight result;

    if (nowBold) {
        if (supportedFontWeights.contains(Ld::FontFormat::Weight::BOLD)) {
            result = Ld::FontFormat::Weight::BOLD;
        } else if (supportedFontWeights.contains(Ld::FontFormat::Weight::BLACK)) {
            result = Ld::FontFormat::Weight::BLACK;
        } else if (supportedFontWeights.contains(Ld::FontFormat::Weight::EXTRA_BOLD)) {
            result = Ld::FontFormat::Weight::EXTRA_BOLD;
        } else if (supportedFontWeights.contains(Ld::FontFormat::Weight::DEMI_BOLD)) {
            result = Ld::FontFormat::Weight::DEMI_BOLD;
        } else if (supportedFontWeights.contains(Ld::FontFormat::Weight::MEDIUM)) {
            result = Ld::FontFormat::Weight::MEDIUM;
        } else {
            result = Ld::FontFormat::Weight::INVALID;
        }
    } else {
        if (supportedFontWeights.contains(Ld::FontFormat::Weight::NORMAL)) {
            result = Ld::FontFormat::Weight::NORMAL;
        } else if (supportedFontWeights.contains(Ld::FontFormat::Weight::LIGHT)) {
            result = Ld::FontFormat::Weight::LIGHT;
        } else if (supportedFontWeights.contains(Ld::FontFormat::Weight::EXTRA_LIGHT)) {
            result = Ld::FontFormat::Weight::EXTRA_LIGHT;
        } else if (supportedFontWeights.contains(Ld::FontFormat::Weight::THIN)) {
            result = Ld::FontFormat::Weight::THIN;
        } else {
            result = Ld::FontFormat::Weight::INVALID;
        }
    }

    return result;
}
