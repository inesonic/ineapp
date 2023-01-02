/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFramePresentation::Rectangle class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_PRESENTATION_RECTANGLE_H
#define TABLE_FRAME_PRESENTATION_RECTANGLE_H

#include <QRectF>
#include <QPointF>
#include <QSizeF>

#include <util_bit_array.h>

#include "table_frame_presentation.h"

/**
 * Trivial class that extends the Qt QRectF class to include a table presentation area index.
 */
class TableFramePresentation::Rectangle:public QRectF {
    public:
        Rectangle();

        /**
         * Constructor
         *
         * \param[in] presentationAreaId The presentation area ID.
         *
         * \param[in] topLeft            The top-left corner of the rectangle in the presentation area.
         *
         * \param[in] size               The size of the rectangle in the presentation area.
         */
        Rectangle(unsigned long presentationAreaId, const QPointF& topLeft, const QSizeF& size);

        /**
         * Constructor
         *
         * \param[in] presentationAreaId The presentation area ID.
         *
         * \param[in] topLeft            The top-left corner of the rectangle in the presentation area.
         *
         * \param[in] bottomRight        The bottom-right corner of the rectangle in the presentation area.
         */
        Rectangle(unsigned long presentationAreaId, const QPointF& topLeft, const QPointF& bottomRight);

        /**
         * Constructor
         *
         * \param[in] presentationAreaId The presentation area ID.
         *
         * \param[in] x                  The rectangular region's left edge.
         *
         * \param[in] y                  The rectangular region's top.
         *
         * \param[in] width              The width of the rectangular region in the presentation area.
         *
         * \param[in] height             The height of the rectanglular region in the presentation area.
         */
        Rectangle(unsigned long presentationAreaId, float x, float y, float width, float height);

        /**
         * Copy constructor
         *
         * \param[in] other The instance to be copied.
         */
        Rectangle(const Rectangle& other);

        ~Rectangle();

        /**
         * Method you can use to obtain the presentation area ID.
         *
         * \return Returns the presentation area ID.
         */
        unsigned long presentationAreaId() const;

        /**
         * Method you can use to set the presentation area ID.
         *
         * \param[in] newPresentationAreaId The new presentation area ID.
         */
        void setPresentationAreaId(unsigned long newPresentationAreaId);

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be assigned to this instance.
         *
         * \return Returns a reference to this instance.
         */
        Rectangle& operator=(const Rectangle& other);

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against this instance.
         *
         * \return Returns true if the instances are equal.  Returns false if the instances are not equal.
         */
        bool operator==(const Rectangle& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against this instance.
         *
         * \return Returns true if the instances are not equal.  Returns false if the instances are equal.
         */
        bool operator!=(const Rectangle& other) const;

    private:
        /**
         * The current presentation area ID.
         */
        unsigned long currentPresentationAreaId;
};


#endif
