/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFramePresentation::Tracker class.
***********************************************************************************************************************/

#include <QSizeF>
#include <QPointF>
#include <QRectF>
#include <QLineF>
#include <QColor>
#include <QList>
#include <QMap>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>

#include <eqt_graphics_item_group.h>

#include <ld_table_frame_element.h>

#include "presentation.h"
#include "table_frame_presentation.h"
#include "table_frame_presentation_tracker.h"

TableFramePresentation::Tracker::Tracker() {
    nextTableArea = 0;
    nextRectangle = 0;
    nextLine      = 0;
}


TableFramePresentation::Tracker::~Tracker() {
    clear();
}


void TableFramePresentation::Tracker::clear() {
    reset();

    for (  QList<QGraphicsRectItem*>::const_iterator rectangleIterator    = currentRectangles.constBegin(),
                                                     rectangleEndIterator = currentRectangles.constEnd()
         ; rectangleIterator != rectangleEndIterator
         ; ++rectangleIterator
        ) {
        delete *rectangleIterator;
    }

    currentRectangles.clear();

    for (  QList<QGraphicsLineItem*>::const_iterator lineIterator    = currentLines.constBegin(),
                                                     lineEndIterator = currentLines.constEnd()
         ; lineIterator != lineEndIterator
         ; ++lineIterator
        ) {
        delete *lineIterator;
    }

    currentLines.clear();

    for (  QList<EQt::GraphicsItemGroup*>::const_iterator groupIterator    = currentTableAreas.constBegin(),
                                                          groupEndIterator = currentTableAreas.constEnd()
         ; groupIterator != groupEndIterator
         ; ++groupIterator
        ) {
        delete *groupIterator;
    }

    currentTableAreas.clear();
}


void TableFramePresentation::Tracker::reset() {
    nextTableArea = 0;
    nextRectangle = 0;
    nextLine      = 0;

    currentChildPresentations.clear();
}


unsigned long TableFramePresentation::Tracker::createNewTablePresentationArea(const QList<float>& columnEdges) {
    unsigned long result = nextTableArea;
    if (nextTableArea >= static_cast<unsigned long>(currentTableAreas.size())) {
        Q_ASSERT(nextTableArea == static_cast<unsigned long>(currentTableAreas.size()));
        currentTableAreas.append(new EQt::GraphicsItemGroup);
        currentColumnEdges.append(columnEdges);
        currentAreaTops.append(0.0F);
        currentRowBottomEdges.append(QMap<float, unsigned>());
    } else {
        currentColumnEdges[result] = columnEdges;
        currentAreaTops[result] = 0.0F;
        currentRowBottomEdges[result].clear();
    }

    ++nextTableArea;
    return result;
}


unsigned long TableFramePresentation::Tracker::numberTablePresentationAreas() const {
    return nextTableArea;
}


EQt::GraphicsItemGroup* TableFramePresentation::Tracker::tablePresentationArea(
        unsigned long presentationAreaIndex
    ) const {
    return presentationAreaIndex < nextTableArea ? currentTableAreas.at(presentationAreaIndex) : Q_NULLPTR;
}


void TableFramePresentation::Tracker::setPresentationAreaTop(unsigned long presentationAreaIndex, float areaTopY) {
    Q_ASSERT(presentationAreaIndex < static_cast<unsigned long>(currentAreaTops.size()));
    currentAreaTops[presentationAreaIndex] = areaTopY;
}


void TableFramePresentation::Tracker::setRowBottom(
        unsigned long presentationAreaIndex,
        unsigned      rowIndex,
        float         rowBottomY
        ) {
    Q_ASSERT(presentationAreaIndex < static_cast<unsigned long>(currentRowBottomEdges.size()));

    QMap<float, unsigned>& rowBottomEdges = currentRowBottomEdges[presentationAreaIndex];
    rowBottomEdges.insert(rowBottomY, rowIndex);
}


Ld::TableFrameElement::CellPosition TableFramePresentation::Tracker::cellAtLocation(const QPointF& location) const {
    Ld::TableFrameElement::CellPosition result;

    if (nextTableArea > 0) {
        QRectF   sceneBoundingRectangle;
        bool     foundPoint;
        unsigned tableAreaIndex = 0;
        do {
            QGraphicsItem* graphicsItem              = currentTableAreas.at(tableAreaIndex);
            QRectF         itemBoundingRectangle     = graphicsItem->boundingRect();
            QRectF         childrenBoundingRectangle = graphicsItem->childrenBoundingRect();
            QRectF         areaBoundingRectangle     = itemBoundingRectangle | childrenBoundingRectangle;

            sceneBoundingRectangle = graphicsItem->mapToScene(areaBoundingRectangle).boundingRect();
            foundPoint = sceneBoundingRectangle.contains(location);
            ++tableAreaIndex;
        } while (!foundPoint && tableAreaIndex < nextTableArea);

        if (foundPoint) {
            --tableAreaIndex;

            QPointF areaLocation = currentTableAreas.at(tableAreaIndex)->mapFromScene(location);
            float   x            = areaLocation.x();
            float   y            = areaLocation.y();

            const QList<float>& columnEdges = currentColumnEdges.at(tableAreaIndex);
            unsigned numberColumnEdges = static_cast<unsigned>(columnEdges.size());
            unsigned columnEdgeIndex   = 0;
            while (columnEdgeIndex < numberColumnEdges && columnEdges.at(columnEdgeIndex) < x) {
                ++columnEdgeIndex;
            }

            if (columnEdgeIndex > 0 && columnEdgeIndex < numberColumnEdges) {
                unsigned columnIndex = columnEdgeIndex - 1;

                float areaTop = currentAreaTops.at(tableAreaIndex);
                if (y > areaTop) {
                    const QMap<float, unsigned>& rowBottom = currentRowBottomEdges.at(tableAreaIndex);
                    QMap<float, unsigned>::const_iterator pos = rowBottom.lowerBound(y);
                    if (pos != rowBottom.constEnd()) {
                        unsigned rowIndex = pos.value();
                        result.setColumnIndex(columnIndex);
                        result.setRowIndex(rowIndex);
                    }
                }
            }
        }
    }

    return result;
}


void TableFramePresentation::Tracker::trimUnusedTablePresentationAreas() {
    unsigned long numberAllocated = static_cast<unsigned long>(currentTableAreas.size());
    if (nextTableArea < numberAllocated) {
        for (unsigned long index=nextTableArea ; index<numberAllocated ; ++index) {
            delete currentTableAreas.at(index);
        }

        currentTableAreas.erase(currentTableAreas.begin() + nextTableArea, currentTableAreas.end());
        currentColumnEdges.erase(currentColumnEdges.begin() + nextTableArea, currentColumnEdges.end());
        currentAreaTops.erase(currentAreaTops.begin() + nextTableArea, currentAreaTops.end());
        currentRowBottomEdges.erase(currentRowBottomEdges.begin() + nextTableArea, currentRowBottomEdges.end());
    }
}


void TableFramePresentation::Tracker::purgeUnusedEmbellishments() {
    unsigned long numberAllocatedRectangles = static_cast<unsigned long>(currentRectangles.size());
    if (nextRectangle < numberAllocatedRectangles) {
        for (unsigned long index=nextRectangle ; index<numberAllocatedRectangles ; ++index) {
            delete currentRectangles.at(index);
        }

        currentRectangles.erase(currentRectangles.begin() + nextRectangle, currentRectangles.end());
    }

    unsigned long numberAllocatedLines = static_cast<unsigned long>(currentLines.size());
    if (nextLine < numberAllocatedLines) {
        for (unsigned long index=nextLine ; index<numberAllocatedLines ; ++index) {
            delete currentLines.at(index);
        }

        currentLines.erase(currentLines.begin() + nextLine, currentLines.end());
    }
}


void TableFramePresentation::Tracker::recordChildObject(
        unsigned long  tablePresentationArea,
        const QPointF& childPosition,
        Presentation*  childPresentation,
        unsigned long  childPresentationAreaIdentifier
    ) {
    while (tablePresentationArea >= static_cast<unsigned long>(currentChildPresentations.size())) {
        currentChildPresentations.append(ChildPresentationAreasByChild());
    }

    ChildPresentationAreasByChild&     presentationAreasByChild = currentChildPresentations[tablePresentationArea];
    AreaPositionByChildPresentationId& areaPositionByChildId    = presentationAreasByChild[childPresentation];

    areaPositionByChildId.insert(childPresentationAreaIdentifier, childPosition);
}


void TableFramePresentation::Tracker::placeChildObjects() {
    unsigned long numberPresentationAreas = static_cast<unsigned long>(currentChildPresentations.size());
    for (unsigned long presentationIndex=0 ; presentationIndex<numberPresentationAreas ; ++presentationIndex) {
        EQt::GraphicsItemGroup*              tableGraphicsItem = currentTableAreas.at(presentationIndex);
        const ChildPresentationAreasByChild& areasByChild      = currentChildPresentations.at(presentationIndex);

        for (  ChildPresentationAreasByChild::const_iterator childAreaIterator    = areasByChild.constBegin(),
                                                             childAreaEndIterator = areasByChild.constEnd()
             ; childAreaIterator != childAreaEndIterator
             ; ++childAreaIterator
            ) {
            Presentation*                            childPresentation = childAreaIterator.key();
            const AreaPositionByChildPresentationId& positionsById     = childAreaIterator.value();

            for (  AreaPositionByChildPresentationId::const_iterator
                       positionByIdIterator    = positionsById.constBegin(),
                       positionByIdEndIterator = positionsById.constEnd()
                 ; positionByIdIterator != positionByIdEndIterator
                 ; ++positionByIdIterator
                ) {
                unsigned long  childPresentationId = positionByIdIterator.key();
                const QPointF& childPosition       = positionByIdIterator.value();

                QGraphicsItem* childGraphicsItem = childPresentation->graphicsItem(childPresentationId);
                if (childGraphicsItem != Q_NULLPTR) {
                    tableGraphicsItem->addToGroup(childGraphicsItem);
                    childGraphicsItem->setPos(childPosition);
                }
            }
        }
    }
}


void TableFramePresentation::Tracker::addBackground(
        unsigned long tablePresentationArea,
        const QRectF& areaRectangle,
        const QColor& color
    ) {
    QGraphicsRectItem* rectangleItem;

    if (nextRectangle >= static_cast<unsigned>(currentRectangles.size())) {
        Q_ASSERT(nextRectangle == static_cast<unsigned>(currentRectangles.size()));

        rectangleItem = new QGraphicsRectItem;
        rectangleItem->setPen(QPen(Qt::NoPen));
        rectangleItem->setZValue(Presentation::minimumEmbellishmentsZHeight);

        currentRectangles.append(rectangleItem);
        ++nextRectangle;
    } else {
        rectangleItem = currentRectangles.at(nextRectangle);
        ++nextRectangle;
    }

    QBrush brush(color);
    rectangleItem->setBrush(brush);
    rectangleItem->setPen(QPen(color));

    EQt::GraphicsItemGroup* tableGraphicsItem = currentTableAreas.at(tablePresentationArea);
    tableGraphicsItem->addToGroup(rectangleItem);

    rectangleItem->setRect(QRectF(QPointF(0, 0),areaRectangle.size()));
    rectangleItem->setPos(areaRectangle.topLeft());
}


void TableFramePresentation::Tracker::addLine(
        unsigned long tablePresentationArea,
        const QLineF& line,
        float         lineWidth,
        const QColor& color
    ) {
    QGraphicsLineItem* lineItem;

    if (nextLine >= static_cast<unsigned>(currentLines.size())) {
        Q_ASSERT(nextLine == static_cast<unsigned>(currentLines.size()));

        lineItem = new QGraphicsLineItem;
        lineItem->setZValue(Presentation::minimumEmbellishmentsZHeight + 1);

        currentLines.append(lineItem);
        ++nextLine;
    } else {
        lineItem = currentLines.at(nextLine);
        ++nextLine;
    }

    EQt::GraphicsItemGroup* tableGraphicsItem = currentTableAreas.at(tablePresentationArea);
    tableGraphicsItem->addToGroup(lineItem);

    QPen pen(QBrush(color), lineWidth);
    lineItem->setPen(pen);

    QLineF adjustedLine(QPointF(0, 0), line.p2() - line.p1());
    lineItem->setLine(adjustedLine);
    lineItem->setPos(line.p1());
}
