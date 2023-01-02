/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationArea class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_AREA_H
#define PRESENTATION_AREA_H

#include <util_hash_functions.h>

#include "app_common.h"

class Presentation;

/**
 * You can use this trivial class to track a presentation area defined by a child identifier and presentation area ID.
 *
 * The class includes operators you can use to sort presentation areas by child identifier and then by presentation
 * area IDs.
 */
class APP_PUBLIC_API PresentationArea {
    public:
        /**
         * Type used to store and represented encoded presentation area values.  The value is intended for sorting and
         * fast searching algorithms.
         */
        typedef unsigned long long EncodedValue;

        /**
         * Constructor
         *
         * \param[in] newChildIdentifier    The identifier of the child at this location.
         *
         * \param[in] newPresentationAreaId The presentation area ID to track with this presentation.
         */
        PresentationArea(unsigned long newChildIdentifier = 0, unsigned long newPresentationAreaId = 0);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        PresentationArea(const PresentationArea& other);

        ~PresentationArea();

        /**
         * Method you can use to set the child identifier at this location.
         *
         * \param[in] newChildIdentifier The new child identifier value.
         */
        void setChildIdentifier(unsigned long newChildIdentifier);

        /**
         * Method you can use to obtain the child identifier at this location.
         *
         * \return Returns the child identifier.
         */
        unsigned long childIdentifier() const;

        /**
         * Method you can use to set the presentation area ID tied to the presentation.
         *
         * \param[in] newPresentationAreaId The new presentation area ID tied to the presentation.
         */
        void setPresentationAreaId(unsigned long newPresentationAreaId);

        /**
         * Method you can use to obtain the presentation area ID at the specified location.
         *
         * \return Returns the presentation area ID at the specified location.
         */
        unsigned long presentationAreaId() const;

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this object instance.
         */
        PresentationArea& operator=(const PresentationArea& other);

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if the two instances are equal.  Returns false if the two instances are not equal.
         */
        bool operator==(const PresentationArea& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if the two instances are not equal.  Returns false if the two instances are equal.
         */
        bool operator!=(const PresentationArea& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should precede the other instance.  Returns false if this instance is
         *         equal to or should follow the other instance.
         */
        bool operator<(const PresentationArea& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should follow the other instance.  Returns false if this instance is
         *         equal to or should precede the other instance.
         */
        bool operator>(const PresentationArea& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should precede the other instance or is equal to the other instance.
         *         Returns false if this instance should follow the other instance.
         */
        bool operator<=(const PresentationArea& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should follow the other instance or is equal to the other instance.
         *         Returns false if this instance should precede the other instance.
         */
        bool operator>=(const PresentationArea& other) const;

        /**
         * Method you can use to return an encoded value for the presentation area.  The value is intended for fast
         * search and sorting algorithms and is guaranteed such that, for two arbitrary presentation areas,
         * \f$ p _ x \f$, that have encoded values \f$ e _ x \f$ and \f$ e _ y \f$ respectively:
         *
         * \f{equation}{
             \forall p _ x < p _ y : e _ x < e _ y
           \f}
         *
         * \return Returns the encoded value for this presentation area.
         */
        EncodedValue encodedValue() const;

    private:
        /**
         * The child identifier for this area.
         */
        unsigned long currentChildIdentifier;

        /**
         * The area ID we are tracking.
         */
        unsigned long currentPresentationAreaId;
};

/**
 * Hash function for the \ref PresentationArea class.
 *
 * \param[in] presentationArea The line start we need to hash.
 *
 * \param[in] seed             An optional hash seed.
 */
APP_PUBLIC_API Util::HashResult qHash(const PresentationArea& presentationArea, Util::HashSeed seed = 0);

#endif
