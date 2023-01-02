 /*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFramePresentation::Area class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_PRESENTATION_AREA_H
#define TABLE_FRAME_PRESENTATION_AREA_H

#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QSharedPointer>

#include <ld_table_frame_format.h>

#include "table_frame_presentation.h"
#include "scene_units.h"

/**
 * You can use this class to track information used to position and present elements within a table.  This class is
 * intended to contain volatile data used to build the visual presentation of a table that can be destroyed once a
 * table is fully presented.
 */
class TableFramePresentation::Area:public SceneUnits {
    public:
        Area();

        /**
         * Constructor
         *
         * \param[in] areaSize                 The area size, in scene units.
         *
         * \param[in] format                   The format associated with this table.  If null, columns will be equally
         *                                     spaced across the entire available table width.
         *
         * \param[in] isFirstArea              If true, this area will be treated as the first area and top margins
         *                                     will be honored.  If false, this area will be treated as a continuation
         *                                     of the table and top margins will be ignored.
         *
         * \param[in] maximumHeight            If true, this area will be viewed as being the maximum possible height
         *                                     for any area.
         *
         * \param[in] minimumUsableSpace       The minimum top spacing in scene units.  This value will always be
         *                                     honored as a minimum spacing before the table contents.
         *
         * \param[in] minimumUsableColumnWidth The minimum allowed usable column width in scene units.
         */
        Area(
            const QSizeF& areaSize,
            Format*       format,
            bool          isFirstArea,
            bool          maximumHeight,
            float         minimumUsableSpace = 0.0F,
            float         minimumUsableColumnWidth = 0.0F
        );

        /**
         * Copy constructor
         *
         * \param[in] other The instance to be copied.
         */
        Area(const Area& other);

        ~Area();

        /**
         * Method you can use to determine if the area data was valid.
         *
         * \return Returns true if the supplied area data was valid.  Returns false if the supplied area data was
         *         invalid.
         */
        bool isValid() const;

        /**
         * Method you can use to determine if the area data was invalid.
         *
         * \return Returns true if the supplied area data was invalid.  Returns false if the supplied area data was
         *         valid.
         */
        bool isInvalid() const;

        /**
         * Method you can use to obtain the area size in scene units.
         *
         * \return Returns the area size in scene units.
         */
        QSizeF areaSize() const;

        /**
         * Method that indicates if this area is the maximum possible height.
         *
         * \return Returns true if this area is the maximum possible height (full page height).
         */
        bool isMaximumPossibleHeight() const;

        /**
         * Method you can use to obtain the current number of columns, ignoring merges.
         *
         * \return Returns the current number of columns.
         */
        unsigned numberColumns() const;

        /**
         * Method you can use to obtain the left edge of a specified column, in scene units.
         *
         * \param[in] columnIndex The zero based column index.
         *
         * \return Returns the left edge of the column, in scene units.
         */
        float columnLeftEdge(unsigned columnIndex) const;

        /**
         * Method you can use to obtain the right edge of a specified column, in scene units.
         *
         * \param[in] columnIndex the zero based column index.
         *
         * \return Returns the right edge of the column, in scene units.
         */
        float columnRightEdge(unsigned columnIndex) const;

        /**
         * Method you can use to obtain the left margin of a specified column, in scene units.  This value represents
         * the column's left edge plus gutter area.
         *
         * \param[in] columnIndex The zero based column index.
         *
         * \return Returns the left margin of the specified column, in scene units.
         */
        float columnLeft(unsigned columnIndex) const;

        /**
         * Method you can use to obtain the right margin of a specified column, in scene units.  This value represents
         * the column's right edge less the gutter area.
         *
         * \param[in] columnIndex The zero based column index.
         *
         * \return Returns the right margin of the specified column, in scene units.
         */
        float columnRight(unsigned columnIndex) const;

        /**
         * Method you can use to obtain all the column edges.
         *
         * \return Returns a reference to a list containing the location of every column edge within this area.  The
         *         returned list will contain one more entry than the number of columns.
         */
        const QList<float>& columnEdges() const;

        /**
         * Assignment operator
         *
         * \param[in] other The instance to assign to this instance.
         *
         * \return Returns a reference to this instance.
         */
        Area& operator=(const Area& other);

    private:
        /**
         * The current area size in scene units.
         */
        QSizeF currentAreaSize;

        /**
         * List of column edge locations.  This list will have one more entry than the total number of columns.
         */
        QList<float> currentColumnEdges;

        /**
         * The current left cell gutter.
         */
        float currentLeftGutter;

        /**
         * The current right cell gutter.
         */
        float currentRightGutter;

        /**
         * Flag that indicates if this area is the maximum possible height.
         */
        bool currentAreaIsMaximumHeight;
};

#endif
