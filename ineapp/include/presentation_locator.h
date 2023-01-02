/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PresentationLocator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_LOCATOR_H
#define PRESENTATION_LOCATOR_H

#include <QPointF>

#include "app_common.h"

class Presentation;

/**
 * You can use this class to track the location of element presentations (visuals).
 */
class APP_PUBLIC_API PresentationLocator {
    public:
        /**
         * Static method you can use to indicate if reading order is left-to-right or right-to-left.
         *
         * \param[in] nowLeftToRightOrder If true, elements should be ordered left-to-right.  If false, elements should
         *                                be ordered right-to-left.
         */
        static void setLeftToRightOrdering(bool nowLeftToRightOrder = true);

        /**
         * Static method you can use to indicate if reading order is right-to-left or left-to-right.
         *
         * \param[in] nowRightToLeftOrder If true, elements should be ordered right-to-left.  If false, elements should
         *                                be ordered left-to-right.
         */
        static void setRightToLeftOrdering(bool nowRightToLeftOrder = true);

        /**
         * Static method you can use to determine if reading order is currently set to left-to-right ordering or
         * right-to-left ordering.
         *
         * \return Returns true if ordering is left-to-right.  Returns false if ordering is right-to-left.
         */
        static bool leftToRightOrdering();

        /**
         * Static method you can use to indicate if reading order is right-to-left or left-to-right.
         *
         * \param[in] nowRightToLeftOrder If true, elements should be ordered right-to-left.  If false, elements should
         *                                be ordered left-to-right.
         */
        static bool rightToLeftOrdering();

        /**
         * Constructor
         *
         * \param[in] newX The X location of the presentation.
         *
         * \param[in] newY The Y location of the presentation.
         */
        PresentationLocator(double newX = 0, double newY = 0);

        /**
         * Constructor
         *
         * \param[in] newPosition The new position for the presentation.
         */
        PresentationLocator(const QPointF& newPosition);

        /**
         * Copy constructor.
         *
         * \param[in] other The instance to be copied.
         */
        PresentationLocator(const PresentationLocator& other);

        ~PresentationLocator();

        /**
         * Method you can use to set the position.
         *
         * \param[in] newX The new X position.
         *
         * \param[in] newY The new Y position.
         */
        void setPosition(double newX, double newY);

        /**
         * Convenience method you can use to set the new position.
         *
         * \param[in] newPosition The new position.
         */
        void setPosition(const QPointF& newPosition);

        /**
         * Method you can use to obtain the X axis value.
         *
         * \return Returns the current X axis value.
         */
        double x() const;

        /**
         * Method you can use to obtain the Y axis value.
         *
         * \return Returns the current Y axis value.
         */
        double y() const;

        /**
         * Method you can use to obtain the current position.
         *
         * \return Returns the current position tuple.
         */
        QPointF position() const;

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this object instance.
         */
        PresentationLocator& operator=(const PresentationLocator& other);

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the locations are the same, returns false if the locations are different.
         */
        bool operator==(const PresentationLocator& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the locations are different, returns false if the locations are the same.
         */
        bool operator!=(const PresentationLocator& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this location should fall before the other location.
         */
        bool operator<(const PresentationLocator& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this location should fall before the other location.
         */
        bool operator>(const PresentationLocator& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this location should fall before or land on the other location.
         */
        bool operator<=(const PresentationLocator& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this location should fall before or land on the other location.
         */
        bool operator>=(const PresentationLocator& other) const;

    private:
        /**
         * Value used to indicate reading order.
         */
        static bool currentUseLeftToRightOrdering;

        /**
         * Current X position.
         */
        double currentX;

        /**
         * Current Y position.
         */
        double currentY;
};

#endif
