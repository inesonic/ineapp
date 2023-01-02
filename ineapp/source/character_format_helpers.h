/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines several functions used to help manage character and font formats.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CHARACTER_FORMAT_HELPERS_H
#define CHARACTER_FORMAT_HELPERS_H

#include <QSet>
#include <QSharedPointer>

#include <ld_font_format.h>
#include <ld_character_format.h>

/**
 * Function that returns a set containing every available font format weight.
 *
 * \return Returns a set of all supported font formats.
 */
QSet<Ld::FontFormat::Weight> supportedFontWeights();

/**
 * Function that converts a set set of QFont::Weight values to a set of Ld::FontFormat::Weight values.
 *
 * \param[in] qFontSet The set of QFont values to be converted.
 *
 * \return Returns a set of Ld::FontFormat::Weight values.
 */
QSet<Ld::FontFormat::Weight> toFontFormatWeights(const QSet<QFont::Weight>& qFontSet);

/**
 * Function that returns a set containing every font weight supported by a given font format on this platform.
 *
 * \param[in] fontFormat The font format we want to query the weight information for.
 *
 * \return Returns a set of font weights supproted by the font format.
 */
QSet<Ld::FontFormat::Weight> supportedFontWeights(QSharedPointer<Ld::FontFormat> fontFormat);

/**
 * Function that determines the font weights supported by all the formats (intersection) in an aggregation.
 *
 * \param[in] aggregation The aggregation we want to check formats in.
 *
 * \return Returns a set of font weights supported by every format in the aggregation.
 */
QSet<Ld::FontFormat::Weight> supportedFontWeights(const Ld::FontFormat::Aggregation& aggregation);

/**
 * Function that determines the correct font weight for bold or normal selections.
 *
 * \param[in] nowBold If true, we want to get as close to a traditional bold font as possible.  If false, we want to
 *                    get to as close to a traditional normal font as possible.
 *
 * \param[in] supportedFontWeights A set of all the supported font weights.
 *
 * \return Returns the recommended font weight to apply.
 */
Ld::FontFormat::Weight bestWeight(bool nowBold, const QSet<Ld::FontFormat::Weight>& supportedFontWeights);

#endif
