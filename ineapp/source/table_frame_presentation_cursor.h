/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFramePresentation::Cursor class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_PRESENTATION_CURSOR_H
#define TABLE_FRAME_PRESENTATION_CURSOR_H

#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QSharedPointer>

#include <cstdint>

#include <util_bit_array.h>

#include <ld_table_frame_format.h>

#include "placement_negotiator.h"
#include "table_frame_presentation.h"
#include "table_frame_presentation_area.h"
#include "table_frame_presentation_row_location.h"
#include "table_frame_presentation_rectangle.h"
#include "scene_units.h"

class PlacementNegotiator;
class Presentation;

/**
 * You can use this class to track information used to position and present elements within a table.  This class is
 * intended to contain volatile data used to build the visual presentation of a table that can be destroyed once a
 * table is fully presented.
 *
 * This class provides tracking of rows as well as functions to maintain a cursor within a individual cell within a
 * row.
 *
 * This class uses the concept of row and cell sessions.  You must process cells in row, column in order.
 */
class TableFramePresentation::Cursor:public SceneUnits {
    public:
        /**
         * Constructor
         *
         * \param[in] parentPlacementNegotiator The parent placement negotiator that we can use to obtain space for the
         *                                      table.
         *
         * \param[in] thisChildIdentifier       The child identifier that we should report ourselves as when talking
         *                                      to the parent placement negotiator.
         *
         * \param[in] format                    The format information we should use for dimensional data.
         *
         * \param[in] tracker                   The tracker used to manage the table contents.
         *
         * \param[in] minimumTopSpacing         The minimum allowed spacing before the top of the table.
         */
        Cursor(
            PlacementNegotiator* parentPlacementNegotiator,
            unsigned long        thisChildIdentifier,
            Format*              format,
            Tracker*             tracker,
            float                minimumTopSpacing
        );

        ~Cursor();

        /**
         * Method you can use to mark the start of a cell.  You should call this method before filling a cell to
         * prepare the underlying cursor to place the cell contents.  This method does not apply additional space for
         * the top gutter.
         *
         * \param[in] columnIndex The zero based column index of the left-most column of the cell.
         *
         * \param[in] columnSpan  The column-span for the cell.
         *
         * \param[in] rowIndex    The zero based row index of the top-most row in the cell.
         *
         * \param[in] rowSpan     The row-span for the cell.
         */
        void startCell(unsigned columnIndex, unsigned columnSpan, unsigned rowIndex, unsigned rowSpan);

        /**
         * Method you can use to request the next available area within the active cell.
         *
         * \param[out] spaceQualifier The qualifier defining the constraints on the provided space.
         *
         * \return Returns a rectangle indicating the available region.
         */
        QRectF requestArea(PlacementNegotiator::SpaceQualifier* spaceQualifier = Q_NULLPTR);

        /**
         * Method you can use to obtain the last reported available area.
         *
         * \return Returns the last reported available area.
         */
        const QRectF& lastReportedArea() const;

        /**
         * Method you can use to allocate a portion of a previously requested area within the current cell.  You can
         * use this method both when space is allocated and to mark space as insufficient.
         *
         * \param[in] presentation            The child presentation to place at the given area.  This value is
         *                                    provided to the tracker.
         *
         * \param[in] childPresentationAreaId The ID to use to access the child presentation.  This value is provided
         *                                    to the tracker.
         *
         * \param[in] allocatedArea           The newly allocated area.
         */
        void allocateArea(
            Presentation* presentation,
            unsigned long childPresentationAreaId,
            const QRectF& allocatedArea
        );

        /**
         * Method you can use to flag the area reported by \ref TableFramePresentation::Cursor::requestArea as
         * insufficient.
         */
        void areaInsufficient();

        /**
         * Method you can use to indicate that the cell has been completed and should be closed.
         *
         * \param[in] minimumAdditionalGutter A value you can set to specify the minimum gutter to apply.  This value
         *                                    will generally be the bottom spacing of the last child.  The maximum of
         *                                    this value and the paragraph gutter will be applied to the cell.
         */
        void endCell(float minimumAdditionalGutter = 0.0F);

        /**
         * Method you can use to allocate space used in the last presentation area.  You should call this after you
         * finish processing every cell to inform the parent of any remaining space that is used.  The allocated
         * space will include space required for the bottom gutter.
         *
         * You can optionally add additional space, if needed.
         *
         * \param[in] additionalSpace Additional vertical space to include at the bottom of the table.
         */
        void endTable(float additionalSpace = 0.0F);

        /**
         * Method you can use to obtain the row top location and cell boundary information.
         *
         * \param[in] rowIndex The zero based row index.
         */
        const RowLocation& rowTopLine(unsigned rowIndex) const;

        /**
         * Method you can use to obtain the row bottom location and cell boundary information.
         *
         * \param[in] rowIndex The zero based row index.
         */
        const RowLocation& rowBottomLine(unsigned rowIndex) const;

        /**
         * Method you can use to obtain the column rectangle within a given table presentation area.
         *
         * \param[in] tablePresentationAreaId The zero based table presentation area we want the column rectangle for.
         *
         * \param[in] columnIndex             The zero based column index of the left-most column.
         *
         * \param[in] columnSpan              The column span.
         */
        QRectF columnRectangle(unsigned long tablePresentationAreaId, unsigned columnIndex, unsigned columnSpan) const;

        /**
         * Method you can use to obtain a list of all the table rectangles that represent a cell.
         *
         * \param[in] columnIndex The zero based column index of the left-most column of the cell.
         *
         * \param[in] columnSpan  The column-span for the cell.
         *
         * \param[in] rowIndex    The zero based row index of the top-most row in the cell.
         *
         * \param[in] rowSpan     The row-span for the cell.
         */
        QList<Rectangle> cellRectangles(
            unsigned columnIndex,
            unsigned columnSpan,
            unsigned rowIndex,
            unsigned rowSpan
        ) const;

        /**
         * Method you can use to identify the starting and ending row indexes of the column left side lines.
         *
         * \param[in] columnIndex The zero based column index.
         *
         * \return Returns a bit array identifying which rows in this column should include a column line.
         */
        const Util::BitArray& columnLeftLine(unsigned columnIndex) const;

        /**
         * Method you can use to identify the starting and ending row indexes of the column right side lines.
         *
         * \param[in] columnIndex The zero based column index.
         *
         * \return Returns a bit array identifying which rows in this column should include a column line.
         */
        const Util::BitArray& columnRightLine(unsigned columnIndex) const;

        /**
         * Method you can use to determine information for a specific table area.
         *
         * \param[in] tablePresentationAreaId The zero based ID of the presentation area of interest.
         */
        const Area& tableArea(unsigned long tablePresentationAreaId) const;

    private:
        /**
         * The minimum usable table height, in points.
         */
        static constexpr float minimumUsableHeightPoints = 72.0F;

        /**
         * The minimum usable table column width, in points.
         */
        static constexpr float minimumUsableColumnWidthPoints = 12.0F;

        /**
         * The minimum acceptable remaining height in points.
         */
        static constexpr float minimumRemainingHeightPoints = 3.6F;

        /**
         * Method you can use to flag the area reported by \ref TableFramePresentation::Cursor::requestArea as
         * insufficient.
         *
         * \param[in] allocatedArea The newly allocated area.
         */
        void areaInsufficient(const QRectF& allocatedArea);

        /**
         * Method you can use to request a new active area from the parent negotiator.
         *
         * \param[in] honorTopMargin If true, the top margin specified by the format will be honored.  If false, the
         *                           top margin specified by the format will be ignored.
         *
         * \return Returns the new presentation area ID.
         */
        unsigned long allocateNewPresentationArea(bool honorTopMargin = false);

        /**
         * The parent negotiator we should query for additional space.
         */
        PlacementNegotiator* currentParentPlacementNegotiator;

        /**
         * The child identifier we should use when communicating with the parent placement negotiator.
         */
        unsigned long currentChildIdentifier;

        /**
         * The current format data.
         */
        Format* currentFormat;

        /**
         * The current tracker.
         */
        Tracker* currentTracker;

        /**
         * The minimum usable height before we allocate a new region.
         */
        float minimumRemainingHeightSceneUnits;

        /**
         * List of placement areas being used by the table.
         */
        QList<Area> currentAreas;

        /**
         * List of the top locations for each row.
         */
        QList<RowLocation> currentRowTops;

        /**
         * Bit arrays used to identify column lines.  The array will contain one more entry than the number of columns.
         */
        QList<Util::BitArray> currentColumnBorders;

        /**
         * The current column index.
         */
        unsigned currentColumnIndex;

        /**
         * The current column span.
         */
        unsigned currentColumnSpan;

        /**
         * The current row index.
         */
        unsigned currentRowIndex;

        /**
         * The current row span.
         */
        unsigned currentRowSpan;

        /**
         * The current area for the column.
         */
        unsigned long currentArea;

        /**
         * The current space qualifier value for the current cell.
         */
        PlacementNegotiator::SpaceQualifier currentSpaceQualifier;

        /**
         * The current usable left edge of the cell.  We use Qt's real type here to avoid issues with tests against
         * equality between float, double, and/or long double types that can occur between Qt and non Qt code.  This
         * approach is simpler and faster than testing against some epsilon in the resulting values.
         */
        qreal currentLeftEdge;

        /**
         * The current usable right edge of the cell.  We use Qt's real type here to avoid issues with tests against
         * equality between float, double, and/or long double types that can occur between Qt and non Qt code.  This
         * approach is simplier and faster than testing against some epsilon in the resulting values.
         */
        qreal currentRightEdge;

        /**
         * Rectangle representing the available space in the current area.
         */
        QRectF currentAvailableSpace;

        /**
         * Value representing the maximum bottom value we've found.
         */
        float maximumBottomValue;
};

#endif
