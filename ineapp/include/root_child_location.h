/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RootChildLocation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ROOT_CHILD_LOCATION_H
#define ROOT_CHILD_LOCATION_H

#include "app_common.h"

class Presentation;

/**
 * You can use this trivial class to track the location of a child of the root presentation.
 */
class APP_PUBLIC_API RootChildLocation {
    public:
        /**
         * Constructor
         *
         * \param[in] newTopPageIndex      The zero based page index of the page containing the first presentation area
         *                                 for the child.  This value will also be assigned to the bottom page index.
         *
         * \param[in] newTopY              The Y location representing the top of the child.  This value will also be
         *                                 assigned to the bottom Y location.
         *
         * \param[in] newMinimumTopSpacing The minimum top spacing to apply when placing this child.
         */
        RootChildLocation(unsigned newTopPageIndex, float newTopY, float newMinimumTopSpacing);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        RootChildLocation(const RootChildLocation& other);

        ~RootChildLocation();

        /**
         * Method you can use to set the top page index tied to this child.
         *
         * \param[in] newTopPageIndex The new zero based top page index of the page to tie to this child.
         */
        void setTopPageIndex(unsigned newTopPageIndex);

        /**
         * Method you can use to obtain the current top page index for this child.
         *
         * \return Returns the top page index that was reported for this child.
         */
        unsigned topPageIndex() const;

        /**
         * Method you can use to set the top Y location where this child starts.
         *
         * \param[in] newTopY The new top Y location representing the top of this child.
         */
        void setTopY(float newTopY);

        /**
         * Method you can use to obtain the Y location where this child starts.
         *
         * \return Returns the page index that was reported for this child.
         */
        float topY() const;

        /**
         * Method you can use to set the bottom page index tied to this child.
         *
         * \param[in] newBottomPageIndex The new zero based bottom page index of the page to tie to this child.
         */
        void setBottomPageIndex(unsigned newBottomPageIndex);

        /**
         * Method you can use to obtain the current bottom page index for this child.
         *
         * \return Returns the bottom page index that was reported for this child.
         */
        unsigned bottomPageIndex() const;

        /**
         * Method you can use to set the bottom Y location where this child starts.
         *
         * \param[in] newBottomY The new bottom Y location representing the bottom of this child.
         */
        void setBottomY(float newBottomY);

        /**
         * Method you can use to obtain the Y location where this child starts.
         *
         * \return Returns the page index that was reported for this child.
         */
        float bottomY() const;

        /**
         * Method you can use to set the minimum top spacing that was applied to this child.
         *
         * \param[in] newMinimumTopSpacing The new minimum top spacing to apply to this child.
         */
        void setMinimumTopSpacing(float newMinimumTopSpacing);

        /**
         * Method you can use to obtain the minimum top spacing that should be applied to this child.
         *
         * \return Returns the minimum top spacing to apply to this child.
         */
        float minimumTopSpacing() const;

        /**
         * Method you can use to set the top location data.
         *
         * \param[in] newTopPageIndex      The zero based top page index of the page containing the first presentation
         *                                 area for the child.
         *
         * \param[in] newTopY              The top Y location representing the top of the child.
         *
         * \param[in] newMinimumTopSpacing The minimum top spacing to apply when placing this child.
         */
        void setTopLocation(unsigned newTopPageIndex, float newTopY, float newMinimumTopSpacing);

        /**
         * Method you can use to set the bottom location data.
         *
         * \param[in] newBottomPageIndex The zero based bottom page index of the page containing the first presentation
         *                               area for the child.
         *
         * \param[in] newBottomY         The bottom Y location representing the top of the child.
         */
        void setBottomLocation(unsigned newBottomPageIndex, float newBottomY);

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this object instance.
         */
        RootChildLocation& operator=(const RootChildLocation& other);

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if the two instances are equal.  Returns false if the two instances are not equal.
         */
        bool operator==(const RootChildLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if the two instances are not equal.  Returns false if the two instances are equal.
         */
        bool operator!=(const RootChildLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should precede the other instance.  Returns false if this instance is
         *         equal to or should follow the other instance.
         */
        bool operator<(const RootChildLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should follow the other instance.  Returns false if this instance is
         *         equal to or should precede the other instance.
         */
        bool operator>(const RootChildLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should precede the other instance or is equal to the other instance.
         *         Returns false if this instance should follow the other instance.
         */
        bool operator<=(const RootChildLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to compare against.
         *
         * \return Returns true if this instance should follow the other instance or is equal to the other instance.
         *         Returns false if this instance should precede the other instance.
         */
        bool operator>=(const RootChildLocation& other) const;

    private:
        /**
         * The top page index for this child.
         */
        unsigned currentTopPageIndex;

        /**
         * The child's top Y coordinate.
         */
        float currentTopY;

        /**
         * The bottom page index for this child.
         */
        unsigned currentBottomPageIndex;

        /**
         * The child's bottom Y coordinate.
         */
        float currentBottomY;

        /**
         * The child's minimum top spacing.
         */
        float currentMinimumTopSpacing;
};

#endif
