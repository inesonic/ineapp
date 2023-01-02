/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFramePresentation::Tracker class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FRAME_PRESENTATION_TRACKER_H
#define TABLE_FRAME_PRESENTATION_TRACKER_H

#include <QSizeF>
#include <QPointF>
#include <QRectF>
#include <QLineF>
#include <QColor>
#include <QList>
#include <QMap>

#include <ld_table_line_settings.h>
#include <ld_table_frame_element.h>

#include "table_frame_presentation.h"
#include "scene_units.h"

class QGraphicsRectItem;
class QGraphicsLineItem;

namespace EQt {
    class GraphicsItemGroup;
}

class Presentation;

/**
 * You can use this class to manage and track graphics item groups as well as child presentation areas and row/column
 * boundaries stored within each graphics item group.
 */
class TableFramePresentation::Tracker {
    public:
        Tracker();

        ~Tracker();

        /**
         * Method you can use to clear all internally tracked data.  Calling this method is the same as creating a
         * new instance of this class.
         */
        void clear();

        /**
         * Method you can use to reset internally tracked data to an initial state prior to recalculating table
         * child positions.
         */
        void reset();

        /**
         * Method you can call to create a new area to contain children.
         *
         * \param[in] columnEdges A list containing all the column edges tied to this presentation area.
         *
         * \return The zero based index of the newly assigned presentation area.
         */
        unsigned long createNewTablePresentationArea(const QList<float>& columnEdges);

        /**
         * Method you can use to determine the current number of table presentation areas tracked by this instance.
         *
         * \return Returns the current number of presentation areas that are tracked by this instance.
         */
        unsigned long numberTablePresentationAreas() const;

        /**
         * Method you can use to obtain one of the table's presentation areas by zero based index.
         *
         * \param[in] presentationAreaIndex The zero based index of the desired table presentation area.
         *
         * \return Returns a pointer to the requested table presentation area.  Returns a null pointer if the
         *         presentation area index is invalid.
         */
        EQt::GraphicsItemGroup* tablePresentationArea(unsigned long presentationAreaIndex) const;

        /**
         * Method you can use to indicate the Y offset of the top of the table within a given presentatin area.
         *
         * \param[in] presentationAreaIndex The zero based index of the presentation area.
         *
         * \param[in] areaTopY              The Y offset of the area's top in scene coordinates relative to the top of
         *                                  the presentation area.
         */
        void setPresentationAreaTop(unsigned long presentationAreaIndex, float areaTopY);

        /**
         * Method you can use to mark the location of the bottom of a given row.
         *
         * \param[in] presentationAreaIndex The ID of the presentation area associated with this row line.
         *
         * \param[in] rowIndex              The zero based row index of the row whose bottom we are now setting.
         *
         * \param[in] rowBottomY            The row's bottom Y in scene coordinates relative to the presentation area.
         */
        void setRowBottom(unsigned long presentationAreaIndex, unsigned rowIndex, float rowBottomY);

        /**
         * Method you can use to determine the row/column location tied to a scene location.
         *
         * \param[in] location The location to query the cell position of, in scene coordinates.
         *
         * \return Returns the cell position.  An invalid cell position is returned if the location is not in the
         *         table.
         */
        Ld::TableFrameElement::CellPosition cellAtLocation(const QPointF& location) const;

        /**
         * Method you can use to trim unused table presentation areas.
         */
        void trimUnusedTablePresentationAreas();

        /**
         * Method you can use to deallocate all unused table embellishments.
         */
        void purgeUnusedEmbellishments();

        /**
         * Method you can use to add a child presentation area.  This method will track the child but will not fetch
         * or place the child object.
         *
         * \param[in] tablePresentationArea           The table presentation area to contain the child object.
         *
         * \param[in] childPosition                   The position of the child presentation object relative to the
         *                                            specified table presentation area.
         *
         * \param[in] childPresentation               The child presentation object to get the object from.
         *
         * \param[in] childPresentationAreaIdentifier The child presentation area identifier we should use to access
         *                                            the child object.
         */
        void recordChildObject(
            unsigned long  tablePresentationArea,
            const QPointF& childPosition,
            Presentation*  childPresentation,
            unsigned long  childPresentationAreaIdentifier
        );

        /**
         * Method you can use to place all the recorded child objects.
         */
        void placeChildObjects();

        /**
         * Method you can use to add a rectangular background area to a table presentation area.  You can use this
         * method to fill cell backgrounds.
         *
         * \param[in] tablePresentationArea The table presentation area to contain the embellishment.
         *
         * \param[in] areaRectangle         The location of the background area within the table presentation area.
         *
         * \param[in] color                 The color to assign to the background area.
         */
        void addBackground(unsigned long tablePresentationArea, const QRectF& areaRectangle, const QColor& color);

        /**
         * Method you can use to add a border line to the table presentation area.
         *
         * \param[in] tablePresentationArea The table presentation area to contain the embellishment.
         *
         * \param[in] line                  The line to be placed.
         *
         * \param[in] lineWidth             The line width.
         *
         * \param[in] color                 The line color.
         */
        void addLine(
            unsigned long tablePresentationArea,
            const QLineF& line,
            float         lineWidth,
            const QColor& color
        );

    private:
        /**
         * Type used to track information about child presentation position by child presentation ID.
         */
        typedef QMap<unsigned long, QPointF> AreaPositionByChildPresentationId;

        /**
         * Type used to track information about all the child presentation areas within a single table presentation
         * area.
         */
        typedef QMap<Presentation*, AreaPositionByChildPresentationId> ChildPresentationAreasByChild;

        /**
         * List used to track all the presentation areas.
         */
        QList<ChildPresentationAreasByChild> currentChildPresentations;

        /**
         * Table of presentation areas.
         */
        QList<EQt::GraphicsItemGroup*> currentTableAreas;

        /**
         * List of column edges by presentation area ID.
         */
        QList<QList<float>> currentColumnEdges;

        QList<float> currentAreaTops;
        /**
         * List of row edges by presentation area ID.  An invalid row edge is used to indicate the location of the
         * row
         */
        QList<QMap<float, unsigned>> currentRowBottomEdges;

        /**
         * Index used to track the next table active area to assign.
         */
        unsigned long nextTableArea;

        /**
         * List used to allocate background rectangles.
         */
        QList<QGraphicsRectItem*> currentRectangles;

        /**
         * Index used to track the next allocated rectangle.
         */
        unsigned long nextRectangle;

        /**
         * List used to allocate border lines.
         */
        QList<QGraphicsLineItem*> currentLines;

        /**
         * Index used to track the next allocated graphics line.
         */
        unsigned long nextLine;
};

#endif
