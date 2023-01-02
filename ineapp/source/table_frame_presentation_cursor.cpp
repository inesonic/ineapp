/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFramePresentation::Cursor class.
***********************************************************************************************************************/

#include <QSizeF>
#include <QSharedPointer>
#include <QRectF>

#include <cmath>
#include <algorithm>

#include <util_bit_array.h>

#include <ld_table_frame_element.h>
#include <ld_table_frame_format.h>

#include "placement_negotiator.h"
#include "table_frame_presentation.h"
#include "table_frame_presentation_area.h"
#include "table_frame_presentation_format.h"
#include "table_frame_presentation_row_location.h"
#include "table_frame_presentation_rectangle.h"
#include "table_frame_presentation_tracker.h"
#include "scene_units.h"
#include "table_frame_presentation_cursor.h"

TableFramePresentation::Cursor::Cursor(
        PlacementNegotiator*             parentPlacementNegotiator,
        unsigned long                    thisChildIdentifier,
        TableFramePresentation::Format*  format,
        TableFramePresentation::Tracker* tracker,
        float                            minimumTopSpacing
    ) {
    currentParentPlacementNegotiator = parentPlacementNegotiator;
    currentChildIdentifier           = thisChildIdentifier;
    currentFormat                    = format;
    currentTracker                   = tracker;

    minimumRemainingHeightSceneUnits = toScene(QSizeF(0.0F, minimumRemainingHeightPoints)).height();

    unsigned long presentationAreaId = allocateNewPresentationArea(true);
    float         topSpacing         = std::max(minimumTopSpacing, currentFormat->topSpacing());

    tracker->setPresentationAreaTop(presentationAreaId, topSpacing);

    Util::BitArray topCellBorders;
    topCellBorders.resize(format->numberColumns());
    topCellBorders.setBits(0, format->numberColumns() - 1);
    currentRowTops.append(RowLocation(presentationAreaId, topSpacing, topCellBorders));

    unsigned numberRows    = format->numberRows();
    unsigned numberColumns = format->numberColumns();

    Util::BitArray firstColumnBorder;
    firstColumnBorder.resize(numberRows);
    firstColumnBorder.setBits(0, numberRows - 1);
    currentColumnBorders.append(firstColumnBorder);

    Util::BitArray columnBorder;
    columnBorder.resize(numberRows);
    for (unsigned columnBorderIndex=0 ; columnBorderIndex<numberColumns ; ++columnBorderIndex) {
        currentColumnBorders.append(columnBorder);
    }

    currentColumnIndex = Ld::TableFrameElement::invalidColumn;
    currentColumnSpan  = Ld::TableFrameElement::invalidColumn;
    currentRowIndex    = Ld::TableFrameElement::invalidRow;
    currentRowSpan     = Ld::TableFrameElement::invalidRow;
    currentArea        = static_cast<unsigned>(-1);
}


TableFramePresentation::Cursor::~Cursor() {}


void TableFramePresentation::Cursor::startCell(
        unsigned columnIndex,
        unsigned columnSpan,
        unsigned rowIndex,
        unsigned rowSpan
    ) {
    currentColumnIndex = columnIndex;
    currentColumnSpan  = columnSpan;
    currentRowIndex    = rowIndex;
    currentRowSpan     = rowSpan;

    Q_ASSERT(rowIndex < static_cast<unsigned>(currentRowTops.size()));
    const RowLocation& rowLocation = currentRowTops.at(rowIndex);
    currentArea = rowLocation.presentationArea();

    const TableFramePresentation::Area& area = currentAreas.at(currentArea);

    currentLeftEdge = area.columnLeft(columnIndex);
    currentRightEdge = area.columnRight(columnIndex + columnSpan - 1);

    currentAvailableSpace.setLeft(currentLeftEdge);
    currentAvailableSpace.setTop(rowLocation.y());
    currentAvailableSpace.setRight(currentRightEdge);
    currentAvailableSpace.setBottom(area.areaSize().height());

    currentSpaceQualifier =   area.isMaximumPossibleHeight()
                            ? PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT
                            : PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH;

    maximumBottomValue = currentAvailableSpace.top();
}


QRectF TableFramePresentation::Cursor::requestArea(PlacementNegotiator::SpaceQualifier* spaceQualifier) {
    if (currentAvailableSpace.height() <= minimumRemainingHeightSceneUnits) {
        if (currentArea + 1 >= static_cast<unsigned>(currentAreas.size())) {
            allocateNewPresentationArea();
        }

        ++currentArea;

        const TableFramePresentation::Area& area = currentAreas.at(currentArea);

        currentLeftEdge = area.columnLeft(currentColumnIndex);
        currentRightEdge = area.columnRight(currentColumnIndex + currentColumnSpan - 1);

        currentAvailableSpace.setLeft(currentLeftEdge);
        currentAvailableSpace.setTop(0.0F);
        currentAvailableSpace.setRight(currentRightEdge);
        currentAvailableSpace.setBottom(area.areaSize().height());

        currentSpaceQualifier =   area.isMaximumPossibleHeight()
                                ? PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT
                                : PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH;

        maximumBottomValue = currentAvailableSpace.top();
    }

    if (spaceQualifier != Q_NULLPTR) {
        if (currentAvailableSpace.left() == currentLeftEdge) {
            *spaceQualifier = currentSpaceQualifier;
        } else {
            *spaceQualifier = PlacementNegotiator::SpaceQualifier::CURRENT_AVAILABLE;
        }
    }

    return currentAvailableSpace;
}


const QRectF& TableFramePresentation::Cursor::lastReportedArea() const {
    return currentAvailableSpace;
}


void TableFramePresentation::Cursor::allocateArea(
        Presentation* presentation,
        unsigned long childPresentationAreaId,
        const QRectF& allocatedArea
    ) {
    currentTracker->recordChildObject(currentArea, allocatedArea.topLeft(), presentation, childPresentationAreaId);
    areaInsufficient(allocatedArea);
}


void TableFramePresentation::Cursor::areaInsufficient() {
    areaInsufficient(currentAvailableSpace);
}



void TableFramePresentation::Cursor::endCell(float minimumAdditionalGutter) {
    unsigned bottomRowIndex = currentRowIndex + currentRowSpan;

    float bottomGutter = std::max(currentFormat->bottomGutter(), minimumAdditionalGutter);
    if (bottomRowIndex < static_cast<unsigned>(currentRowTops.size())) {
        RowLocation  cellBottomLocation(currentArea, maximumBottomValue + bottomGutter);
        RowLocation& currentCellBottomLocation = currentRowTops[bottomRowIndex];
        if (cellBottomLocation > currentCellBottomLocation) {
            cellBottomLocation.setCellBorders(currentCellBottomLocation.cellBorders());
            currentCellBottomLocation = cellBottomLocation;
        }

        currentCellBottomLocation.setCellBordersSet(currentColumnIndex, currentColumnSpan);
    } else {
        while (bottomRowIndex >= static_cast<unsigned>(currentRowTops.size())) {
            Util::BitArray cellBorders(currentFormat->numberColumns(), false);
            RowLocation cellBottomLocation(currentArea, maximumBottomValue + bottomGutter, cellBorders);
            currentRowTops.append(cellBottomLocation);
        }

        RowLocation& currentCellBottomLocation = currentRowTops[bottomRowIndex];
        currentCellBottomLocation.setCellBordersSet(currentColumnIndex, currentColumnSpan);
    }

    unsigned endingColumnBorder = currentColumnIndex + currentColumnSpan;
    currentColumnBorders[endingColumnBorder].setBits(currentRowIndex, currentRowIndex + currentRowSpan - 1);

    currentColumnIndex = Ld::TableFrameElement::invalidColumn;
    currentColumnSpan  = Ld::TableFrameElement::invalidColumn;
    currentRowIndex    = Ld::TableFrameElement::invalidRow;
    currentRowSpan     = Ld::TableFrameElement::invalidRow;
    currentArea        = static_cast<unsigned>(-1);
}


void TableFramePresentation::Cursor::endTable(float additionalSpace) {
    if (!currentAreas.isEmpty()) {
        const Area&   lastArea               = currentAreas.last();
        unsigned long lastPresentationAreaId = static_cast<unsigned long>(currentAreas.size() - 1);

        QSizeF        areaSize               = lastArea.areaSize();
        areaSize.setHeight(maximumBottomValue + currentFormat->bottomGutter() + additionalSpace);

        currentParentPlacementNegotiator->allocateArea(currentChildIdentifier, lastPresentationAreaId, areaSize);
    }
}


const TableFramePresentation::RowLocation& TableFramePresentation::Cursor::rowTopLine(unsigned rowIndex) const {
    return currentRowTops.at(rowIndex);
}


const TableFramePresentation::RowLocation& TableFramePresentation::Cursor::rowBottomLine(unsigned rowIndex) const {
    return currentRowTops.at(rowIndex + 1);
}


QRectF TableFramePresentation::Cursor::columnRectangle(
        unsigned long tablePresentationAreaId,
        unsigned      columnIndex,
        unsigned      columnSpan
    ) const {
    const Area& area        = currentAreas.at(tablePresentationAreaId);
    float       columnLeft  = area.columnLeftEdge(columnIndex);
    float       columnRight = area.columnRightEdge(columnIndex + columnSpan - 1);

    return QRectF(columnLeft, 0, columnRight - columnLeft, area.areaSize().height());
}


QList<TableFramePresentation::Rectangle> TableFramePresentation::Cursor::cellRectangles(
        unsigned columnIndex,
        unsigned columnSpan,
        unsigned rowIndex,
        unsigned rowSpan
    ) const {
    QList<Rectangle> result;

    const RowLocation& rowTopLocation    = currentRowTops.at(rowIndex);
    const RowLocation& rowBottomLocation = currentRowTops.at(rowIndex + rowSpan);

    unsigned long firstPresentationArea = rowTopLocation.presentationArea();
    unsigned long lastPresentationArea  = rowBottomLocation.presentationArea();
    for (unsigned areaId=firstPresentationArea ; areaId<=lastPresentationArea ; ++areaId) {
        const Area& area            = currentAreas.at(areaId);
        float       rectangleTop    = areaId == firstPresentationArea ? rowTopLocation.y() : 0;
        float       rectangleBottom =   areaId == lastPresentationArea
                                      ? rowBottomLocation.y()
                                      : area.areaSize().height();

        float       columnLeft      = area.columnLeftEdge(columnIndex);
        float       columnRight     = area.columnRightEdge(columnIndex + columnSpan - 1);

        Rectangle rectangle(
            areaId,
            columnLeft,
            rectangleTop,
            columnRight - columnLeft,
            rectangleBottom - rectangleTop
        );

        result.append(rectangle);
    }

    return result;
}


const Util::BitArray& TableFramePresentation::Cursor::columnLeftLine(unsigned int columnIndex) const {
    return currentColumnBorders.at(columnIndex);
}


const Util::BitArray& TableFramePresentation::Cursor::columnRightLine(unsigned int columnIndex) const {
    return currentColumnBorders.at(columnIndex + 1);
}


const TableFramePresentation::Area& TableFramePresentation::Cursor::tableArea(
        unsigned long tablePresentationAreaId
    ) const {
    return currentAreas.at(tablePresentationAreaId);
}


void TableFramePresentation::Cursor::areaInsufficient(const QRectF& allocatedArea) {
    if (allocatedArea.right() < currentRightEdge) {
        currentAvailableSpace.setLeft(allocatedArea.right());
        if (allocatedArea.bottom() > maximumBottomValue) {
            maximumBottomValue = allocatedArea.bottom();
        }
    } else {
        if (allocatedArea.bottom() > maximumBottomValue) {
            currentAvailableSpace.setTop(allocatedArea.bottom());
        } else {
            currentAvailableSpace.setTop(maximumBottomValue);
        }

        currentAvailableSpace.setLeft(currentLeftEdge);
        maximumBottomValue = currentAvailableSpace.top();
    }
}


unsigned long TableFramePresentation::Cursor::allocateNewPresentationArea(bool honorTopMargin) {
    if (!currentAreas.isEmpty()) {
        const Area&   lastArea               = currentAreas.last();
        unsigned long lastPresentationAreaId = static_cast<unsigned long>(currentAreas.size() - 1);
        currentParentPlacementNegotiator->allocateArea(
            currentChildIdentifier,
            lastPresentationAreaId,
            lastArea.areaSize()
        );
    }

    QSizeF areaSize;
    bool   insufficientSpace;

    QSizeF minimumUsables = toScene(QSizeF(minimumUsableColumnWidthPoints, minimumUsableHeightPoints));
    float  minimumUsableColumnWidth = minimumUsables.width();
    float  minimumUsableHeight      = minimumUsables.height();

    Area tableArea;

    do {
        PlacementNegotiator::SpaceQualifier spaceQualifier;

        areaSize = currentParentPlacementNegotiator->requestArea(currentChildIdentifier, &spaceQualifier);
        if (spaceQualifier == PlacementNegotiator::SpaceQualifier::CURRENT_AVAILABLE) {
            insufficientSpace = true;
        } else {
            tableArea = Area(
                areaSize,
                currentFormat,
                honorTopMargin,
                spaceQualifier == PlacementNegotiator::SpaceQualifier::MAXIMUM_WIDTH_AND_HEIGHT,
                minimumUsableHeight,
                minimumUsableColumnWidth
            );
            insufficientSpace = tableArea.isInvalid();
        }

        if (insufficientSpace) {
            currentParentPlacementNegotiator->areaInsufficient(currentChildIdentifier, areaSize);
        }
    } while (insufficientSpace);

    unsigned long presentationAreaId = static_cast<unsigned long>(currentAreas.size());
    currentAreas.append(tableArea);

    unsigned long trackerPresentationAreaId = currentTracker->createNewTablePresentationArea(tableArea.columnEdges());
    Q_ASSERT(trackerPresentationAreaId == presentationAreaId);

    return presentationAreaId;
}


