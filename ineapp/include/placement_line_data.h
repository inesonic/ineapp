/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlacementLineData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEMENT_LINE_DATA_H
#define PLACEMENT_LINE_DATA_H

#include "app_common.h"

/**
 * You can use this class to track information about the presentation areas on a line.
 */
class APP_PUBLIC_API PlacementLineData {
    public:
        PlacementLineData();

        /**
         * Constructor
         *
         * \param[in] newChildIdentifier    The child identifier we are tracking.
         *
         * \param[in] newPresentationAreaId The presentation area ID we are tracking.
         *
         * \param[in] newWidth              The width of this area, in scene units.
         *
         * \param[in] newHeight             The height of this presentation area, in scene units.
         *
         * \param[in] newAscent             The ascent for text in this presentation area, in scene units.
         *
         * \param[in] canStretch            If true, this presentation area can be stretched.
         */
        PlacementLineData(
            unsigned long newChildIdentifier,
            unsigned long newPresentationAreaId,
            double        newWidth,
            double        newHeight,
            double        newAscent,
            bool          canStretch
        );

        /**
         * Copy constructor
         *
         * \param[in] other The instance to be copied.
         */
        PlacementLineData(const PlacementLineData& other);

        ~PlacementLineData();

        /**
         * Method you can use to change the child identifier at this location.
         *
         * \param[in] newChildIdentifier The new child identify to report.
         */
        void setChildIdentifier(unsigned long newChildIdentifier);

        /**
         * Method you can use to obtain the child identifier at this location.
         *
         * \return Returns the child identifier at this location.
         */
        unsigned long childIdentifier() const;

        /**
         * Method you can use to change the presentation area ID at this location.
         *
         * \param[in] newPresentationAreaId The new presentation area ID to report.
         */
        void setPresentationAreaId(unsigned long newPresentationAreaId);

        /**
         * Method you can use to obtain the presentation area ID at this location.
         *
         * \return Returns the presentation area ID at this location.
         */
        unsigned long presentationAreaId() const;

        /**
         * Method you can use to set or change the width of this presentation area.
         *
         * \param[in] newWidth The new width of this presentation area, in scene units.
         */
        void setWidth(double newWidth);

        /**
         * Method you can use to obtain the width of this presentation area, in scene units.
         *
         * \return Returns the width of the presentation area in scene units.
         */
        double width() const;

        /**
         * Method you can use to set the height of this presentation area.
         *
         * \param[in] newHeight The new height to apply to this presentation area, in scene units.
         */
        void setHeight(double newHeight);

        /**
         * Method you can use to determine the current height of this presentation area.
         *
         * \return returns the height of this presentation area, in scene units.
         */
        double height() const;

        /**
         * Method you can use to set the font ascent of this presentation area.
         *
         * \param[in] newAscent The new font ascent to apply to this presentation area, in scene units.
         */
        void setAscent(double newAscent);

        /**
         * Method you can use to determine the current font ascent of this presentation area.
         *
         * \return returns the font ascent of this presentation area, in scene units.
         */
        double ascent() const;

        /**
         * Method you can use to change whether or not this area can be stretched.
         *
         * \param[in] nowCanStretch If true, this area is stretchable.  If false, this area is fixed width.
         */
        void setStretchable(bool nowCanStretch);

        /**
         * Method you can use to determine whether or not this area is stretchable.
         *
         * \return Returns true if this area is stretchable.  Returns false if the area can not be stretched.
         */
        bool canStretch() const;

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this object.
         */
        PlacementLineData& operator=(const PlacementLineData& other);

    private:
        /**
         * The current child identifier.
         */
        unsigned long currentChildIdentifier;

        /**
         * The current presentation area ID.
         */
        unsigned long currentPresentationAreaId;

        /**
         * The width of this presentation area, in scene units.
         */
        double currentWidth;

        /**
         * The height of this presentation area, in scene units.
         */
        double currentHeight;

        /**
         * The ascent to apply to this presentation area, in scene units.
         */
        double currentAscent;

        /**
         * The current stretch status.
         */
        bool currentlyCanStretch;
};

#endif
