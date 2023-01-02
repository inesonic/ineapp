/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFramePresentation::RowLocation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_PRESENTATION_ROW_LOCATION_H
#define TABLE_FRAME_PRESENTATION_ROW_LOCATION_H

#include <util_bit_array.h>

#include "table_frame_presentation.h"

/**
 * Trivial class used to track the location of a table row across presentation areas.
 *
 * Note that cell borders are not included in comparison operations.
 */
class TableFramePresentation::RowLocation {
    public:
        RowLocation();

        /**
         * Constructor
         *
         * \param[in] presentationArea The presentation area containing the row.
         *
         * \param[in] y                The Y offset in the presentation area representing the row's top location.
         *
         * \param[in] cellBorders      A bit array that can be used to specify the location of cell borders.
         */
        RowLocation(unsigned long presentationArea, float y, const Util::BitArray& cellBorders = Util::BitArray());

        /**
         * Copy constructor
         *
         * \param[in] other The instance to be copied
         */
        RowLocation(const RowLocation& other);

        /**
         * Method you can use to obtain the presentation area ID.
         *
         * \return Returns the presentation area ID.
         */
        unsigned long presentationArea() const;

        /**
         * Method you can use to set the presentation area.
         *
         * \param[in] newPresentationArea The new presentation area.
         */
        void setPresentationArea(unsigned long newPresentationArea);

        /**
         * Method you can use to obtain the row's Y offset within the presentation area.
         *
         * \return Returns the row's Y offset within the presentation area.
         */
        float y() const;

        /**
         * Method you can use to set the row's Y offset within the presentation area.
         *
         * \param[in] newY The new Y offset for the row.
         */
        void setY(const float newY);

        /**
         * Method you can use to obtain a bit field indicating which columns should include cell borders.
         *
         * \return Returns a bit field indicating which columns require cell borders.
         */
        const Util::BitArray& cellBorders() const;

        /**
         * Method you can use to update the cell borders.
         *
         * \param[in] newCellBorders A bit field indicating which columns should have borders.
         */
        void setCellBorders(const Util::BitArray& newCellBorders);

        /**
         * Method you can use to mark cell borders as cleared over a range of columns.
         *
         * \param[in] columnIndex The zero based column index of the left-most column.
         *
         * \param[in] columnSpan  The column span.
         */
        void setCellBordersCleared(unsigned columnIndex, unsigned columnSpan = 1);

        /**
         * Method you can use to mark cell borders as set over a range of columns.
         *
         * \param[in] columnIndex The zero based column index of the left-most column.
         *
         * \param[in] columnSpan  The column span.
         */
        void setCellBordersSet(unsigned columnIndex, unsigned columnSpan = 1);

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be copied.
         *
         * \return Returns a reference to this instance.
         */
        RowLocation& operator=(const RowLocation& other);

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the instances are equal.  Returns false if the instances are not equal.
         */
        bool operator==(const RowLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if the instances are not equal.  Returns false if the instances are equal.
         */
        bool operator!=(const RowLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this instance precedes the other instance.  Returns false if this instance is equal
         *         to or follows the other instance.
         */
        bool operator<(const RowLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this instance follows the other instance.  Returns false if this instance is equal
         *         to or precedes the other instance.
         */
        bool operator>(const RowLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this instance precedes or equals the other instance.  Returns false if this instance
         *         follows the other instance.
         */
        bool operator<=(const RowLocation& other) const;

        /**
         * Comparison operator.
         *
         * \param[in] other The instance to be compared against.
         *
         * \return Returns true if this instance follows or equals the other instance.  Returns false if this instance
         *         precedes the other instance.
         */
        bool operator>=(const RowLocation& other) const;

    private:
        /**
         * The current presentation area ID.
         */
        unsigned long currentPresentationArea;

        /**
         * The current row Y offset.
         */
        float currentY;

        /**
         * The cell border line locations.
         */
        Util::BitArray currentCellBorders;
};


#endif
