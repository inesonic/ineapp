/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlacementLineStart class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEMENT_LINE_START_H
#define PLACEMENT_LINE_START_H

#include <util_hash_functions.h>

#include "app_common.h"
#include "placement_negotiator.h"
#include "presentation_area.h"

/**
 * You can use this trivial class to track information about each line in a frame.
 */
class APP_PUBLIC_API PlacementLineStart:public PresentationArea {
    public:
        /**
         * Constructor
         *
         * \param[in] newLineY              The Y position of the line.
         *
         * \param[in] newMinimumTopSpacing The new minimum top spacing, in scene units.
         *
         * \param[in] newChildIdentifier    The identifier of the child at this location.
         *
         * \param[in] newPresentationAreaId The presentation area ID to track with this presentation.
         */
        PlacementLineStart(
            double        newLineY = 0,
            double        newMinimumTopSpacing = PlacementNegotiator::imposeNoTopSpacing,
            unsigned long newChildIdentifier = 0,
            unsigned long newPresentationAreaId = 0);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        PlacementLineStart(const PlacementLineStart& other);

        ~PlacementLineStart();

        /**
         * Method you can use to set the Y position of the line, in scene units.
         *
         * \param[in] newLineY The new Y position of the line.
         */
        void setLineY(double newLineY);

        /**
         * Method you can use to obtain the Y position of the line, in scene units.
         *
         * \return Returns the Y position of the line, in scene units.
         */
        double lineY() const;

        /**
         * Method you can use to set the minimum top spacing to apply when repositioning this line.
         *
         * \param[in] newMinimumTopSpacing The new minimum top spacing, in scene units.
         */
        void setMinimumTopSpacing(double newMinimumTopSpacing);

        /**
         * Method you can use to obtain the minimum top spacing to apply when repositioning this line.
         *
         * \return Returns the reported minimum top spacing.
         */
        double minimumTopSpacing() const;

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this object instance.
         */
        PlacementLineStart& operator=(const PlacementLineStart& other);

    private:
        /**
         * The current line Y position.
         */
        double currentLineY;

        /**
         * The current minimum top spacing.
         */
        double currentMinimumTopSpacing;
};

/**
 * Hash function for the \ref PlacementLineStart class.
 *
 * \param[in] lineStart The line start we need to hash.
 *
 * \param[in] seed      An optional hash seed.
 */
APP_PUBLIC_API Util::HashResult qHash(const PlacementLineStart& lineStart, Util::HashSeed seed = 0);

#endif
