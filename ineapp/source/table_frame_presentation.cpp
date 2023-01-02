/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*`
* This file implements the \ref TableFramePresentation class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QLineF>
#include <QList>
#include <QBitArray>
#include <QtAlgorithms>
#include <QGraphicsItem>

#include <eqt_graphics_item_group.h>

#include <limits>
#include <algorithm>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_visual.h>
#include <ld_table_column_width.h>
#include <ld_table_line_settings.h>
#include <ld_table_frame_format.h>
#include <ld_table_frame_visual.h>
#include <ld_table_frame_element.h>

#include "table_frame_presentation_format.h"
#include "table_frame_presentation_tracker.h"
#include "table_frame_presentation_cursor.h"
#include "table_frame_presentation_row_location.h"
#include "table_frame_presentation_rectangle.h"
#include "placement_tracker.h"
#include "presentation.h"
#include "presentation_with_positional_children.h"
#include "presentation_with_grouped_children.h"
#include "scene_units.h"
#include "table_frame_presentation.h"

TableFramePresentation::TableFramePresentation() {
    connect(
        this,
        SIGNAL(rowsWereInserted(unsigned, unsigned, bool)),
        SLOT(processRowsInserted(unsigned, unsigned, bool)),
        connectionType
    );

    connect(
        this,
        SIGNAL(columnsWereInserted(unsigned, unsigned, bool)),
        SLOT(processColumnsInserted(unsigned, unsigned, bool)),
        connectionType
    );

    connect(
        this,
        SIGNAL(removingRow(unsigned, bool)),
        SLOT(processAboutToRemoveRow(unsigned, bool)),
        connectionType
    );
    connect(
        this,
        SIGNAL(rowWasRemoved(unsigned, bool)),
        SLOT(processRowRemoved(unsigned, bool)),
        connectionType
    );

    connect(
        this,
        SIGNAL(removingColumn(unsigned, bool)),
        SLOT(processAboutToRemoveColumn(unsigned, bool)),
        connectionType
    );

    connect(
        this,
        SIGNAL(columnWasRemoved(unsigned, bool)),
        SLOT(processColumnRemoved(unsigned, bool)),
        connectionType
    );

    connect(
        this,
        SIGNAL(mergingCells(unsigned, unsigned, unsigned, unsigned, bool)),
        SLOT(processAboutToMergeCells(unsigned, unsigned, unsigned, unsigned, bool)),
        connectionType
    );

    connect(
        this,
        SIGNAL(cellsWereMerged(unsigned, unsigned, bool)),
        SLOT(processCellsMerged(unsigned, unsigned, bool)),
        connectionType
    );

    connect(
        this,
        SIGNAL(unmergingCells(unsigned, unsigned)),
        SLOT(processAboutToUnmergeCells(unsigned, unsigned)),
        connectionType
    );
    connect(
        this,
        SIGNAL(cellsWereUnmerged(unsigned, unsigned)),
        SLOT(processCellsUnmerged(unsigned, unsigned)),
        connectionType
    );

    currentTracker           = new Tracker();
    currentCursor            = Q_NULLPTR;
    currentChildPresentation = Q_NULLPTR;
}


TableFramePresentation::~TableFramePresentation() {
    delete currentTracker;
}


Ld::Visual* TableFramePresentation::creator(const QString&) {
    return new TableFramePresentation;
}


Ld::Element::ChildPlacement TableFramePresentation::childPlacement() const {
    return Ld::Element::ChildPlacement::GROUPED;
}


void TableFramePresentation::requestRepositioning(Presentation*) {
    requestRepositioning();
}


void TableFramePresentation::recalculatePlacement(
        PlacementTracker*    placementTracker,
        PlacementNegotiator* parent,
        unsigned long        childIdentifier,
        Presentation*        /* nextSibling */,
        bool                 /* honorLeadingWhitespace */,
        float                minimumTopSpacing,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    currentTracker->reset();

    QSharedPointer<Ld::TableFrameElement> element = TableFramePresentation::element();
    Format format(element->format(), element->numberColumns(), element->numberRows());
    Cursor cursor(parent, childIdentifier, &format, currentTracker, minimumTopSpacing);
    currentCursor = &cursor;

    placementTracker->addNewJobs(element->numberChildren());

    unsigned long childPresentationIdentifier = 0;
    Ld::TableFrameElement::CellDataByPosition cellDataByPosition = element->cellDataByPosition(true);
    for (  Ld::TableFrameElement::CellDataByPosition::const_iterator
               cellDataIterator = cellDataByPosition.constBegin(),
               cellDataEndIterator = cellDataByPosition.constEnd()
         ; cellDataIterator != cellDataEndIterator
         ; ++cellDataIterator
        ) {
        const Ld::TableFrameElement::CellPosition& position = cellDataIterator.key();
        const Ld::TableFrameElement::CellData&     data     = cellDataIterator.value();

        unsigned rowIndex    = position.rowIndex();
        unsigned columnIndex = position.columnIndex();
        unsigned rowSpan     = data.rowSpan();
        unsigned columnSpan  = data.columnSpan();

        const Ld::ElementPointerList& children = data.children();

        currentChildPresentation = Q_NULLPTR;

        Presentation* nextChildPresentation = Q_NULLPTR;
        float         minimumTopSpacing     = format.topGutter();

        cursor.startCell(columnIndex, columnSpan, rowIndex, rowSpan);

        for (  Ld::ElementPointerList::const_iterator childIterator    = children.constBegin(),
                                                      childEndIterator = children.constEnd()
             ; childIterator != childEndIterator
             ; ++childIterator
            ) {
            currentChildPresentation = nextChildPresentation;

            Ld::ElementPointer child = *childIterator;
            Q_ASSERT(!child.isNull());

            nextChildPresentation = dynamic_cast<Presentation*>(child->visual());
            if (currentChildPresentation != Q_NULLPTR) {
                currentChildPresentation->recalculatePlacement(
                    placementTracker,
                    this,
                    childPresentationIdentifier,
                    nextChildPresentation,
                    false,
                    true,
                    minimumTopSpacing
                );

                minimumTopSpacing = 0.0F;
                ++childPresentationIdentifier;
            }

            placementTracker->completedJob();
        }

        float childBottomSpacing;
        if (nextChildPresentation != Q_NULLPTR) {
            currentChildPresentation = nextChildPresentation;
            currentChildPresentation->recalculatePlacement(
                placementTracker,
                this,
                childIdentifier,
                Q_NULLPTR,
                false,
                true,
                minimumTopSpacing
            );

            childBottomSpacing = nextChildPresentation->bottomSpacingSceneUnits();

            placementTracker->completedJob();
        } else {
            childBottomSpacing = 0.0F;
        }

        cursor.endCell(childBottomSpacing);
    }

    cursor.endTable();
    currentTracker->trimUnusedTablePresentationAreas();

    drawCellBackgrounds(cellDataByPosition, cursor, format, *currentTracker);
    drawRowLines(cursor, format, *currentTracker);
    drawColumnLines(cursor, format, *currentTracker);
    updateTrackerRowLocations(cursor, format, *currentTracker);

    currentTracker->purgeUnusedEmbellishments();

    currentTracker->placeChildObjects();
}


void TableFramePresentation::redoPlacement(
        PlacementNegotiator* /* parent */,
        unsigned long        /* childIdentifier */,
        unsigned long        /* firstPresentationAreaId */,
        float                /* minimumTopSpacing */,
        float                /* lineSpacing */,
        float                /* relativeScale */
    ) {
    // We always request full lines so this method should never be called.
    Q_ASSERT(false);
}


float TableFramePresentation::bottomSpacingSceneUnits() {
    float result;

    QSharedPointer<Ld::TableFrameElement> element = TableFramePresentation::element();
    if (!element.isNull()) {
        QSharedPointer<Ld::TableFrameFormat> format = element->format().dynamicCast<Ld::TableFrameFormat>();
        if (!format.isNull()) {
            result = toScene(QSizeF(0.0F, format->bottomSpacing())).height();
        } else {
            result = 0.0F;
        }
    } else {
        result = 0.0F;
    }

    return result;
}


QSizeF TableFramePresentation::requestArea(
        unsigned long                           /* childIdentifier */,
        TableFramePresentation::SpaceQualifier* spaceQualifier
    ) {
    return currentCursor->requestArea(spaceQualifier).size();
}


void TableFramePresentation::allocateArea(
        unsigned long /* childIdentifier */,
        unsigned long presentationAreaId,
        const QSizeF& size,
        float         /* ascent */,
        bool          /* canStretch */
    ) {
    QRectF lastReportedArea = currentCursor->lastReportedArea();
    QRectF allocatedSpace(lastReportedArea.topLeft(), size);

    currentCursor->allocateArea(currentChildPresentation, presentationAreaId, allocatedSpace);
}


void TableFramePresentation::areaInsufficient(unsigned long /* childIdentifier */, const QSizeF& /* size */) {
    currentCursor->areaInsufficient();
}


void TableFramePresentation::applyStretch(unsigned long, float) {
    // Tables report that they can't be stretched so this method should never get called.
    Q_ASSERT(false);
}


QGraphicsItem* TableFramePresentation::graphicsItem(unsigned long presentationAreaId) const {
    return currentTracker->tablePresentationArea(presentationAreaId);
}


void TableFramePresentation::resetPlacement() {}


void TableFramePresentation::removeFromScene() {
    currentTracker->clear();
}


bool TableFramePresentation::cursorCanHighlight() const {
    return false;
}


void TableFramePresentation::processRowsInserted(unsigned, unsigned, bool) {
    requestRepositioning();
}


void TableFramePresentation::processColumnsInserted(unsigned, unsigned, bool) {
    requestRepositioning();
}


void TableFramePresentation::processAboutToRemoveRow(unsigned, bool) {}


void TableFramePresentation::processRowRemoved(unsigned, bool) {
    requestRepositioning();
}


void TableFramePresentation::processAboutToRemoveColumn(unsigned, bool) {}


void TableFramePresentation::processColumnRemoved(unsigned, bool) {
    requestRepositioning();
}


void TableFramePresentation::processAboutToMergeCells(unsigned, unsigned, unsigned, unsigned, bool) {}


void TableFramePresentation::processCellsMerged(unsigned, unsigned, bool) {
    requestRepositioning();
}


void TableFramePresentation::processAboutToUnmergeCells(unsigned, unsigned) {}


void TableFramePresentation::processCellsUnmerged(unsigned, unsigned) {
    requestRepositioning();
}


void TableFramePresentation::processRemovingChildPresentation(unsigned long, Presentation*) {
    requestRepositioning();
}


void TableFramePresentation::processChildPresentationInsertedBefore(unsigned long, Presentation*) {
    requestRepositioning();
}


void TableFramePresentation::processChildPresentationInsertedAfter(unsigned long, Presentation*) {
    requestRepositioning();
}


void TableFramePresentation::processFormatChange(Ld::FormatPointer, Ld::FormatPointer newFormat) {
    updateFormatData(newFormat);
    requestRepositioning();
}


void TableFramePresentation::processDiagnostic(Ld::DiagnosticPointer) {}


void TableFramePresentation::clearDiagnosticDisplay() {}


void TableFramePresentation::tiedToElement(Ld::ElementPointer element) {
    updateFormatData(element->format());
}


void TableFramePresentation::rowsInserted(unsigned rowNumber, unsigned numberRows, bool formatUpdated) {
    emit rowsWereInserted(rowNumber, numberRows, formatUpdated);
}


void TableFramePresentation::columnsInserted(unsigned columnNumber, unsigned numberColumns, bool formatUpdated) {
    emit columnsWereInserted(columnNumber, numberColumns, formatUpdated);
}


void TableFramePresentation::aboutToRemoveRow(unsigned rowNumber, bool formatWillBeUpdated) {
    emit removingRow(rowNumber, formatWillBeUpdated);
}


void TableFramePresentation::rowRemoved(unsigned int rowNumber, bool formatUpdated) {
    emit rowWasRemoved(rowNumber, formatUpdated);
}


void TableFramePresentation::aboutToRemoveColumn(unsigned columnNumber, bool formatWillBeUpdated) {
    emit removingColumn(columnNumber, formatWillBeUpdated);
}


void TableFramePresentation::columnRemoved(unsigned int columnNumber, bool formatUpdated) {
    emit columnWasRemoved(columnNumber, formatUpdated);
}


void TableFramePresentation::aboutToMergeCells(
        unsigned rowIndex,
        unsigned columnIndex,
        unsigned numberCellsToMergeRight,
        unsigned numberCellsToMergeDown,
        bool     formatWillBeUpdated
    ) {
    emit mergingCells(rowIndex, columnIndex, numberCellsToMergeRight, numberCellsToMergeDown, formatWillBeUpdated);
}


void TableFramePresentation::cellsMerged(unsigned rowIndex, unsigned columnIndex, bool formatUpdated) {
    emit cellsWereMerged(rowIndex, columnIndex, formatUpdated);
}


void TableFramePresentation::aboutToUnmergeCells(unsigned rowIndex, unsigned columnIndex) {
    emit unmergingCells(rowIndex, columnIndex);
}


void TableFramePresentation::cellsUnmerged(unsigned rowIndex, unsigned columnIndex) {
    emit cellsWereUnmerged(rowIndex, columnIndex);
}


void TableFramePresentation::requestRepositioning() {
    Ld::ElementPointer parentElement = element()->parent();
    if (!parentElement.isNull()) {
        PlacementNegotiator* parentNegotiator = dynamic_cast<PlacementNegotiator*>(parentElement->visual());
        if (parentNegotiator != Q_NULLPTR) {
            parentNegotiator->requestRepositioning(this);
        }
    }
}


void TableFramePresentation::updateFormatData(Ld::FormatPointer newFormat) {
    QSharedPointer<Ld::TableFrameFormat> format = newFormat.dynamicCast<Ld::TableFrameFormat>();
    if (!format.isNull()) {

    } else {

    }
}


void TableFramePresentation::drawCellBackgrounds(
        const Ld::TableFrameElement::CellDataByPosition& cellDataByPosition,
        const TableFramePresentation::Cursor&            cursor,
        const TableFramePresentation::Format&            format,
        TableFramePresentation::Tracker&                 tracker
    ) {
    for (  Ld::TableFrameElement::CellDataByPosition::const_iterator
               cellDataIterator = cellDataByPosition.constBegin(),
               cellDataEndIterator = cellDataByPosition.constEnd()
         ; cellDataIterator != cellDataEndIterator
         ; ++cellDataIterator
        ) {
        const Ld::TableFrameElement::CellPosition& position = cellDataIterator.key();
        const Ld::TableFrameElement::CellData&     data     = cellDataIterator.value();

        unsigned rowIndex    = position.rowIndex();
        unsigned columnIndex = position.columnIndex();
        unsigned rowSpan     = data.rowSpan();
        unsigned columnSpan  = data.columnSpan();

        QList<Rectangle> rects     = cursor.cellRectangles(columnIndex, columnSpan, rowIndex, rowSpan);
        QColor           cellColor = format.blendedColor(rowIndex, columnIndex);

        if (cellColor.isValid()) {
            for (QList<Rectangle>::const_iterator it=rects.constBegin(),end=rects.constEnd() ; it!=end ; ++it) {
                tracker.addBackground(it->presentationAreaId(), *it, cellColor);
            }
        }
    }
}


void TableFramePresentation::drawRowLines(
        const TableFramePresentation::Cursor& cursor,
        const TableFramePresentation::Format& format,
        TableFramePresentation::Tracker&      tracker
    ) {
    unsigned numberRows    = format.numberRows();
    unsigned numberColumns = format.numberColumns();

    for (unsigned rowIndex=0 ; rowIndex<=numberRows ; ++rowIndex) {
        const Ld::TableLineSettings& lineSettings = format.rowLineSetting(rowIndex);
        if (lineSettings.lineStyle() == Ld::TableLineSettings::Style::SINGLE ||
            lineSettings.lineStyle() == Ld::TableLineSettings::Style::DOUBLE    ) {
            const RowLocation& rowLocation =   rowIndex < numberRows
                                             ? cursor.rowTopLine(rowIndex)
                                             : cursor.rowBottomLine((rowIndex - 1));
            const Area&        area        = cursor.tableArea(rowLocation.presentationArea());

            const Util::BitArray& cellBorders = rowLocation.cellBorders();
            Util::BitArray::Index startingIndex = cellBorders.firstSetBit();
            while (startingIndex != Util::BitArray::invalidIndex) {
                Util::BitArray::Index endingIndex = cellBorders.firstClearedBit(startingIndex + 1);
                Util::BitArray::Index endingColumn;
                if (endingIndex == Util::BitArray::invalidIndex) {
                    endingColumn = numberColumns - 1;
                } else {
                    endingColumn = endingIndex - 1;
                }

                float lineWidth = lineSettings.width();
                if (lineSettings.lineStyle() == Ld::TableLineSettings::Style::SINGLE) {
                    QLineF line(
                        area.columnLeftEdge(static_cast<unsigned>(startingIndex)),
                        rowLocation.y(),
                        area.columnRightEdge(static_cast<unsigned>(endingColumn)),
                        rowLocation.y()
                    );

                    tracker.addLine(rowLocation.presentationArea(), line, lineWidth, QColor(Qt::black));
                } else {
                    QLineF line1(
                        area.columnLeftEdge(static_cast<unsigned>(startingIndex)),
                        rowLocation.y() - lineWidth,
                        area.columnRightEdge(static_cast<unsigned>(endingColumn)),
                        rowLocation.y() - lineWidth
                    );

                    QLineF line2(
                        area.columnLeftEdge(static_cast<unsigned>(startingIndex)),
                        rowLocation.y() + lineWidth,
                        area.columnRightEdge(static_cast<unsigned>(endingColumn)),
                        rowLocation.y() + lineWidth
                    );

                    tracker.addLine(rowLocation.presentationArea(), line1, lineWidth, QColor(Qt::black));
                    tracker.addLine(rowLocation.presentationArea(), line2, lineWidth, QColor(Qt::black));
                }

                if (endingIndex == Util::BitArray::invalidIndex) {
                    startingIndex = Util::BitArray::invalidIndex;
                } else {
                    startingIndex = cellBorders.firstSetBit(endingIndex);
                }
            }
        }
    }
}


void TableFramePresentation::drawColumnLines(
        const TableFramePresentation::Cursor& cursor,
        const TableFramePresentation::Format& format,
        TableFramePresentation::Tracker&      tracker
    ) {
    unsigned numberRows    = format.numberRows();
    unsigned numberColumns = format.numberColumns();

    for (unsigned columnLineIndex=0 ; columnLineIndex<=numberColumns ; ++columnLineIndex) {
        const Ld::TableLineSettings& lineSettings = format.columnLineSetting(columnLineIndex);

        if (lineSettings.lineStyle() == Ld::TableLineSettings::Style::SINGLE ||
            lineSettings.lineStyle() == Ld::TableLineSettings::Style::DOUBLE    ) {
            float                        lineWidth    = lineSettings.width();
            const Util::BitArray&        columnLines  =   columnLineIndex == numberColumns
                                                        ? cursor.columnRightLine(columnLineIndex - 1)
                                                        : cursor.columnLeftLine(columnLineIndex);

            Util::BitArray::Index startingBitIndex = columnLines.firstSetBit();
            while (startingBitIndex != Util::BitArray::invalidIndex) {
                unsigned              startingRowIndex = static_cast<unsigned>(startingBitIndex);

                Util::BitArray::Index endingBitIndex = columnLines.firstClearedBit(startingRowIndex);
                unsigned              endingRowIndex;
                if (endingBitIndex == Util::BitArray::invalidIndex) {
                    endingRowIndex = numberRows - 1;
                } else {
                    endingRowIndex = static_cast<unsigned>(endingBitIndex - 1);
                }

                const RowLocation& startingRowLocation = cursor.rowTopLine(startingRowIndex);
                const RowLocation& endingRowLocation   = cursor.rowBottomLine(endingRowIndex);

                unsigned long startingPresentationId = startingRowLocation.presentationArea();
                unsigned long endingPresentationId   = endingRowLocation.presentationArea();

                for (unsigned long areaId=startingPresentationId ; areaId<=endingPresentationId ; ++areaId) {
                    const Area& area      = cursor.tableArea(areaId);
                    float       startingY =   areaId == startingPresentationId
                                            ? startingRowLocation.y()
                                            : 0;
                    float       endingY   =   areaId == endingPresentationId
                                            ? endingRowLocation.y()
                                            : area.areaSize().height();
                    float       x         =   columnLineIndex == numberColumns
                                            ? area.columnRightEdge(columnLineIndex - 1)
                                            : area.columnLeftEdge(columnLineIndex);

                    if (lineSettings.lineStyle() == Ld::TableLineSettings::Style::SINGLE) {
                        QLineF line(x, startingY, x, endingY);
                        tracker.addLine(areaId, line, lineWidth, QColor(Qt::black));
                    } else {
                        QLineF line1(x - lineWidth, startingY, x - lineWidth, endingY);
                        QLineF line2(x + lineWidth, startingY, x + lineWidth, endingY);

                        tracker.addLine(areaId, line1, lineWidth, QColor(Qt::black));
                        tracker.addLine(areaId, line2, lineWidth, QColor(Qt::black));
                    }
                }

                if (endingBitIndex == Util::BitArray::invalidIndex) {
                    startingBitIndex = Util::BitArray::invalidIndex;
                } else {
                    startingBitIndex = columnLines.firstSetBit(endingBitIndex);
                }
            }
        }
    }
}


void TableFramePresentation::updateTrackerRowLocations(const Cursor& cursor, const Format& format, Tracker& tracker) {
    unsigned numberRows = format.numberRows();

    for (unsigned rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
        const RowLocation& rowBottomLocation     = cursor.rowBottomLine(rowIndex);
        float              rowBottomY            = rowBottomLocation.y();
        unsigned long      presentationAreaIndex = rowBottomLocation.presentationArea();

        tracker.setRowBottom(presentationAreaIndex, rowIndex, rowBottomY);
    }
}
