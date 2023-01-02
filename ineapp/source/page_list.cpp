/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageList class.
***********************************************************************************************************************/

#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QGraphicsItem>
#include <QPainter>

#include <algorithm>

#include <eqt_graphics_scene.h>
#include <eqt_graphics_item_group.h>

#include <ld_page_format.h>

#include "page_list_page.h"
#include "page_list.h"

/***********************************************************************************************************************
 * PageList::Entry
 */

PageList::Entry::Entry(PageList::Page* pageInstance) {
    currentPage = pageInstance;
}


PageList::Entry::Entry() {
    currentPage = Q_NULLPTR;
}


PageList::Entry::Entry(const PageList::Entry& other) {
    currentPage = other.currentPage;
}


PageList::Entry::~Entry() {}


bool PageList::Entry::isValid() const {
    return currentPage != Q_NULLPTR;
}


bool PageList::Entry::isInvalid() const {
    return currentPage == Q_NULLPTR;
}


const QSharedPointer<Ld::PageFormat> PageList::Entry::format() const {
    return currentPage != Q_NULLPTR ? currentPage->format() : QSharedPointer<Ld::PageFormat>();
}


PageList::Index PageList::Entry::index() const {
    return currentPage != Q_NULLPTR ? currentPage->index() : PageList::invalidPageIndex;
}


QPointF PageList::Entry::position() const {
    return currentPage != Q_NULLPTR ? currentPage->pos() : QPointF();
}


const QRectF PageList::Entry::extentsSceneUnits() const {
    return currentPage != Q_NULLPTR ? currentPage->extentsSceneUnits() : QRectF();
}


const QRectF PageList::Entry::boundarySceneUnits() const {
    return currentPage != Q_NULLPTR ? currentPage->boundarySceneUnits() : QRectF();
}


const QRectF PageList::Entry::activeAreaSceneUnits() const {
    return currentPage != Q_NULLPTR ? currentPage->activeAreaSceneUnits() : QRectF();
}


EQt::GraphicsItemGroup* PageList::Entry::activeAreaGroup() const {
    return currentPage != Q_NULLPTR ? currentPage->activeAreaGroup() : Q_NULLPTR;
}


QSizeF PageList::Entry::activeAreaSizeSceneUnits() const {
    return currentPage != Q_NULLPTR ? currentPage->activeAreaSizeSceneUnits() : QSizeF();
}


PageList::Entry& PageList::Entry::operator=(const PageList::Entry& other) {
    currentPage = other.currentPage;
    return *this;
}

/***********************************************************************************************************************
 * PageList
 */

const QColor PageList::marginColor                 = QColor(0xF0, 0xF0, 0xF0, 128);
const QColor PageList::pageContentsBackgroundColor = QColor(Qt::white);
const QColor PageList::shadowColor1                = QColor(0x60, 0x60, 0x60);
const QColor PageList::shadowColor2                = QColor(Qt::lightGray);
const QColor PageList::guideLineColor              = QColor(Qt::gray);
const QPen   PageList::guideCornerPen              = QPen(PageList::guideLineColor);
const QPen   PageList::guidePen                    = QPen(PageList::marginColor);
const QBrush PageList::pageContentsBackgroundBrush = QBrush(PageList::pageContentsBackgroundColor);
const QBrush PageList::marginBrush                 = QBrush(PageList::marginColor);

PageList::PageList(QObject* parent):QObject(parent) {
    currentMaximumNumberColumns = 1;

    currentLeftPageEdgePoints.clear();
    currentMaximumHorizontalExtentsPoints.clear();
}


PageList::PageList(unsigned newMaximumNumberColumns, QObject* parent):QObject(parent) {
    currentMaximumNumberColumns = std::max(1U, newMaximumNumberColumns);

    currentLeftPageEdgePoints.clear();
    currentMaximumHorizontalExtentsPoints.clear();
}


void PageList::clear() {
    truncate(0);
}


void PageList::setMaximumNumberColumns(unsigned numberColumns) {
    if (currentMaximumNumberColumns != numberColumns) {
        currentMaximumNumberColumns = std::max(1U, numberColumns);
        reposition(0);
        emit maximumExtentsChanged(currentMaximumHorizontalExtentsPoints, currentLeftPageEdgePoints);
    }
}


unsigned PageList::maximumNumberColumns() const {
    return currentMaximumNumberColumns;
}


bool PageList::isEmpty() const {
    return pages.isEmpty();
}


bool PageList::isNotEmpty() const {
    return !pages.isEmpty();
}


PageList::Index PageList::numberPages() const {
    return static_cast<Index>(pages.size());
}


const PageList::Entry& PageList::at(PageList::Index index) const {
    return index < static_cast<unsigned>(pages.size()) ? pages.at(index) : defaultEntry;
}


PageList::Entry PageList::operator[](PageList::Index index) const {
    return at(index);
}


bool PageList::setFormat(PageList::Index pageIndex, QSharedPointer<Ld::PageFormat> pageFormat) {
    bool success;

    if (pageIndex < static_cast<unsigned>(pages.size())) {
        Page*  page                = pages[pageIndex].currentPage;
        QSizeF currentPageBoundary = page->boundarySceneUnits().size();
        QSizeF newPageBoundary     = toScene(QSizeF(pageFormat->horizontalExtents(), pageFormat->verticalExtents()));

        page->setFormat(pageFormat);
        if (currentPageBoundary != newPageBoundary) {
            reposition(pageIndex);
        }

        emit pageFormatChanged(pages[pageIndex]);
        updatePageBoundingRectangle();

        emit maximumExtentsChanged(currentMaximumHorizontalExtentsPoints, currentLeftPageEdgePoints);

        success = true;
    } else {
        success = false;
    }

    return success;
}


PageList::Entry PageList::append(QSharedPointer<Ld::PageFormat> format, QGraphicsItem* parent) {
    Page* newPage = new Page(format, parent);
    Entry entry(newPage);

    Index pageIndex = static_cast<Index>(pages.size());

    newPage->setIndex(pageIndex);

    QSizeF spacing = toScene(QSizeF(PageList::pageMarginPoints, PageList::pageMarginPoints));

    if (static_cast<unsigned>(pages.size()) < currentMaximumNumberColumns) {
        // First row
        currentMaximumHorizontalExtentsPoints << format->horizontalExtents();

        double leftEdgeSceneUnits;
        if (pages.isEmpty()) {
            leftEdgeSceneUnits = 0;
        } else {
            Page* lastPage = pages.last().currentPage;
            leftEdgeSceneUnits = lastPage->pos().x() + lastPage->extentsSceneUnits().width() + spacing.width();
        }

        currentLeftPageEdgePoints << fromScene(QPointF(leftEdgeSceneUnits, 0)).x();

        newPage->setPos(leftEdgeSceneUnits, 0);
        pages.append(entry);

        emit maximumExtentsChanged(currentMaximumHorizontalExtentsPoints, currentLeftPageEdgePoints);
    } else {
        unsigned column = pageIndex % currentMaximumNumberColumns;
        Index    row    = pageIndex / currentMaximumNumberColumns;

        double pageWidthPoints = format->horizontalExtents();

        pages.append(entry);

        if (currentMaximumHorizontalExtentsPoints[column] < pageWidthPoints) {
            currentMaximumHorizontalExtentsPoints[column] = pageWidthPoints;
            reposition(column);

            emit maximumExtentsChanged(currentMaximumHorizontalExtentsPoints, currentLeftPageEdgePoints);
        } else {
            Index  previousStartIndex = (row - 1) * currentMaximumNumberColumns;
            Index  previousEndIndex   = previousStartIndex + currentMaximumNumberColumns - 1;
            double maximumHeight      = maximumVerticalExtentsSceneUnits(previousStartIndex, previousEndIndex);
            double previousTop        = pages[previousStartIndex].currentPage->pos().y();
            double currentTop         = previousTop + maximumHeight + spacing.height();
            double currentLeft        = toScene(QPointF(currentLeftPageEdgePoints[column], 0)).x();

            newPage->setPos(currentLeft, currentTop);
        }
    }

    return entry;
}


PageList::Entry PageList::append(QSharedPointer<Ld::PageFormat> format, EQt::GraphicsScene* scene) {
    Entry pageEntry = append(format);
    scene->addItem(pageEntry.currentPage);

    updatePageBoundingRectangle();

    return pageEntry;
}


void PageList::truncate(PageList::Index pageIndex) {
    QList<PageList::Entry>::iterator firstToDelete = pages.begin() + pageIndex;
    for (QList<PageList::Entry>::const_iterator it=firstToDelete,end=pages.constEnd() ; it!=end ; ++it) {
        delete it->currentPage;
    }

    pages.erase(firstToDelete, pages.end());

    unsigned numberColumns = std::min(static_cast<unsigned>(pages.size()), currentMaximumNumberColumns);

    if (numberColumns != static_cast<unsigned>(currentMaximumHorizontalExtentsPoints.size())) {
        reposition();
    } else {
        bool     extentsChanged = false;
        unsigned column         = 0;

        while (!extentsChanged && column < numberColumns) {
            double columnWidthSceneUnits = maximumPageWidthSceneUnits(
                column,
                pages.size() - 1,
                currentMaximumNumberColumns
            );

            double columnWidthPoints = fromScene(QSizeF(columnWidthSceneUnits, 0)).width();
            if (columnWidthPoints != currentMaximumHorizontalExtentsPoints[column]) {
                extentsChanged = true;
            } else {
                ++column;
            }
        }

        if (extentsChanged) {
            reposition(column);
        }
    }

    updatePageBoundingRectangle();

}


QList<double> PageList::maximumHorizontalExtentPoints() const {
    return currentMaximumHorizontalExtentsPoints;
}


QList<double> PageList::columnLeftEdgePoints() const {
    return currentLeftPageEdgePoints;
}


void PageList::drawDecorations(const QRectF& sceneArea, QPainter* painter, bool includeMargins, bool includeGuides) {
    if (includeMargins || includeGuides) {
        double left   = sceneArea.left();
        double right  = sceneArea.right();
        double top    = sceneArea.top();
        double bottom = sceneArea.bottom();

        if (right >= 0 && bottom >= 0 && !pages.isEmpty()) {
            if (left < 0) {
                left = 0;
            }

            if (top < 0) {
                top = 0;
            }

            unsigned leftColumn  = pageColumnAt(left);
            unsigned rightColumn = pageColumnAt(right, leftColumn);
            Index    topRow      = pageRowAt(top);
            Index    bottomRow   = pageRowAt(bottom, topRow);

            Index    numberPages = static_cast<Index>(pages.size());

            for (Index pageRow=topRow ; pageRow<=bottomRow ; ++pageRow) {
                for (unsigned pageColumn=leftColumn ; pageColumn<=rightColumn ; ++pageColumn) {
                    Index pageIndex = pageRow * currentMaximumNumberColumns + pageColumn;

                    if (pageIndex < numberPages) {
                        Page*   page       = pages[pageIndex].currentPage;
                        QPointF offset     = page->pos();
                        QRectF  boundary   = page->boundarySceneUnits().translated(offset);
                        QRectF  activeArea = page->activeAreaSceneUnits().translated(offset);

                        QRectF  topMargin    = QRectF(boundary.topLeft(), activeArea.topRight());
                        QRectF  rightMargin  = QRectF(boundary.topRight(), activeArea.bottomRight()).normalized();
                        QRectF  bottomMargin = QRectF(activeArea.bottomLeft(), boundary.bottomRight());
                        QRectF  leftMargin   = QRectF(boundary.bottomLeft(), activeArea.topLeft()).normalized();

                        if (includeMargins) {
                            painter->setPen(Qt::NoPen);
                            painter->setBrush(marginBrush);

                            painter->drawRect(topMargin.intersected(sceneArea));
                            painter->drawRect(rightMargin.intersected(sceneArea));
                            painter->drawRect(bottomMargin.intersected(sceneArea));
                            painter->drawRect(leftMargin.intersected(sceneArea));
                        }

                        if (includeGuides) {
                            QPointF maximumGuidePoints = fromScene(
                                QPointF(
                                    std::min(leftMargin.width(), rightMargin.width()),
                                    std::min(topMargin.height(), bottomMargin.height())
                                )
                            ) / 2.0;

                            float currentGuideLengthPoints = std::min(
                                std::min(
                                    static_cast<float>(maximumGuidePoints.x()),
                                    static_cast<float>(maximumGuidePoints.y())
                                ),
                                static_cast<float>(guideLengthPoints)
                            );

                            QPointF guideLengthScene = toScene(
                                QPointF(currentGuideLengthPoints, currentGuideLengthPoints)
                            );
                            QPointF vertical1(0, guideLengthScene.y());
                            QPointF horizontal1(guideLengthScene.x(), 0);

                            QPointF vertical2(0, 4);
                            QPointF horizontal2(4, 0);

                            painter->setPen(guideCornerPen);
                            painter->setBrush(Qt::NoBrush);

                            QPointF topLeft     = activeArea.topLeft();
                            QPointF topRight    = activeArea.topRight();
                            QPointF bottomRight = activeArea.bottomRight();
                            QPointF bottomLeft  = activeArea.bottomLeft();

                            painter->drawLine(topLeft - vertical1, topLeft - vertical2);
                            painter->drawLine(topLeft - horizontal1, topLeft - horizontal2);

                            painter->drawLine(topRight + horizontal1, topRight + horizontal2);
                            painter->drawLine(topRight - vertical1, topRight - vertical2);

                            painter->drawLine(bottomRight + vertical1, bottomRight + vertical2);
                            painter->drawLine(bottomRight + horizontal1, bottomRight + horizontal2);

                            painter->drawLine(bottomLeft - horizontal1, bottomLeft - horizontal2);
                            painter->drawLine(bottomLeft + vertical1, bottomLeft + vertical2);

                            if (!includeMargins) {
                                // We also place dim lines between the guide lines that will only show up if there are no
                                // margins on the page.

                                painter->setPen(guidePen);
                                painter->drawLine(topLeft, topRight);
                                painter->drawLine(topRight, bottomRight);
                                painter->drawLine(bottomRight, bottomLeft);
                                painter->drawLine(bottomLeft, topLeft);
                            }
                        }
                    }
                }
            }
        }
    }
}


PageList::Entry PageList::pageAt(const QPointF& position) const {
    PageList::Entry entry;

    if (!pages.isEmpty()) {
        unsigned column = pageColumnAt(position.x());
        if (column != invalidColumnNumber) {
            Index row = pageRowAt(position.y());

            if (row != invalidRowNumber) {
                Index pageIndex = row * currentMaximumNumberColumns + column;

                if (pageIndex < static_cast<unsigned>(pages.size())) {
                    entry = pages[pageIndex];
                    QPointF relativePosition = position - entry.position();
                    if (!entry.extentsSceneUnits().contains(relativePosition)) {
                        entry = PageList::Entry();
                    }
                }
            }
        }
    }

    return entry;
}


PageList::Entry PageList::pageAt(double x, double y) const {
    return pageAt(QPointF(x, y));
}


QRectF PageList::pageBoundingRectangle() const {
    return currentPageBoundingRectangle;
}


double PageList::maximumPageWidthSceneUnits(
        PageList::Index startingIndex,
        PageList::Index endingIndex,
        PageList::Index increment
    ) {
    double maximumPageWidth = 0;

    for (Index index=startingIndex ; index<=endingIndex ; index+=increment) {
        double pageWidth = pages[index].currentPage->boundarySceneUnits().width();
        if (pageWidth > maximumPageWidth) {
            maximumPageWidth = pageWidth;
        }
    }

    return maximumPageWidth;
}


double PageList::maximumVerticalExtentsSceneUnits(
        PageList::Index startingIndex,
        PageList::Index endingIndex,
        PageList::Index increment
    ) {
    double maximumVerticalExtent = 0;

    for (Index index=startingIndex ; index<=endingIndex ; index+=increment) {
        double pageHeight = pages[index].currentPage->extentsSceneUnits().height();
        if (pageHeight > maximumVerticalExtent) {
            maximumVerticalExtent = pageHeight;
        }
    }

    return maximumVerticalExtent;
}


void PageList::reposition(PageList::Index startingIndex) {
    QSizeF spacing = toScene(QSizeF(PageList::pageMarginPoints, PageList::pageMarginPoints));

    currentMaximumHorizontalExtentsPoints.clear();
    currentLeftPageEdgePoints.clear();

    unsigned numberActiveColumns = std::min(static_cast<unsigned>(pages.size()), currentMaximumNumberColumns);
    double   currentLeft         = 0;

    QList<double> leftEdgeSceneUnits;

    for (unsigned column=0 ; column<numberActiveColumns ; ++column) {
        double columnWidthSceneUnits = maximumPageWidthSceneUnits(
            column,
            pages.size() - 1,
            currentMaximumNumberColumns
        );

        double columnWidthPoints = fromScene(QSizeF(columnWidthSceneUnits, 0)).width();

        currentMaximumHorizontalExtentsPoints << columnWidthPoints;
        currentLeftPageEdgePoints << fromScene(QSizeF(currentLeft, 0)).width();
        leftEdgeSceneUnits << currentLeft;

        currentLeft += columnWidthPoints + spacing.width() + shadowWidthPoints;
    }

    Index    row    = startingIndex / currentMaximumNumberColumns;
    unsigned column = startingIndex % currentMaximumNumberColumns;

    double currentTop;
    double maximumRowHeight;
    if (column == 0) {
        if (row == 0) {
            currentTop = 0;
        } else {
            Index  previousStartIndex = (row - 1) * currentMaximumNumberColumns;
            Index  previousEndIndex   = previousStartIndex + currentMaximumNumberColumns - 1;
            double previousTop        = pages[previousStartIndex].currentPage->pos().y();
            double previousRowHeight  = maximumVerticalExtentsSceneUnits(previousStartIndex, previousEndIndex);

            currentTop = previousTop + previousRowHeight + spacing.height();
        }

        maximumRowHeight = 0;
    } else {
        currentTop       = pages[startingIndex - 1].currentPage->pos().y();
        maximumRowHeight = maximumVerticalExtentsSceneUnits(row * currentMaximumNumberColumns, pages.size() - 1);
    }

    Index index        = startingIndex;
    Index numberPages = pages.size();
    while (index < numberPages) {
        Page* page = pages[index].currentPage;

        page->setPos(leftEdgeSceneUnits[column], currentTop);
        double height = page->extentsSceneUnits().height();

        if (height > maximumRowHeight) {
            maximumRowHeight = height;
        }

        ++column;
        if (column >= currentMaximumNumberColumns) {
            column            = 0;
            currentTop       += maximumRowHeight + spacing.height();
            maximumRowHeight  = 0;
        }

        ++index;
    }
}


unsigned PageList::pageColumnAt(double x, unsigned startingColumn) const {
    unsigned column;
    unsigned numberColumns = static_cast<unsigned>(currentMaximumHorizontalExtentsPoints.size());

    if (startingColumn < numberColumns) {
        double xPoints = fromScene(QPointF(x, 0)).x();

        column = startingColumn;
        while (column < numberColumns && currentLeftPageEdgePoints[column] <= xPoints) {
            ++column;
        }

        if (column > 0) {
            --column;
        }
    } else {
        column = invalidColumnNumber;
    }

    return column;
}


PageList::Index PageList::pageRowAt(double y, PageList::Index startingRow) const {
    Index row;
    if (!pages.isEmpty() && y >= 0) {
        Index numberRows = 1 + (pages.size() - 1) / currentMaximumNumberColumns;

        if ((startingRow + 1) >= numberRows) {
            row = startingRow;
        } else if (numberRows > 0) {
            // Since row Y coordinates increase monotonically with row number, we can locate the row extremely quickly
            // using Newton-Raphson method.  We switch to a binary search method if, for some reason, Newton-Raphson
            // doesn't convolve on an answer quickly enough.

            unsigned numberIterations = 0;
            Index    lastRow          = numberRows - 1;
            Index    lowerRow         = startingRow;
            double   lowerY           = pages[lowerRow * currentMaximumNumberColumns].position().y();
            Index    upperRow         = lastRow;
            double   upperY           = pages[upperRow * currentMaximumNumberColumns].position().y();
            double   rowY;

            do {
                if (numberIterations >= maximumRowSearchNewtonRaphsonIterations) {
                    row = (upperRow + lowerRow) / 2;
                } else {
                    float inverseSlope = (0.5 + 1.0 * (upperRow - lowerRow)) / (upperY - lowerY);

                    row = static_cast<long>(inverseSlope * (y - lowerY) + lowerRow);
                    if (row >= numberRows) {
                        row = numberRows - 1;
                    }
                }

                ++numberIterations;

                rowY = pages[row * currentMaximumNumberColumns].position().y();

                if (rowY < y) {
                    lowerRow = row;
                    lowerY   = rowY;
                } else {
                    upperRow = row;
                    upperY   = rowY;
                }
            } while (upperRow - lowerRow > 1);

            // We can exit from the loop off by one row.  Adjust downward if needed (we could also have adjusted the
            // exit condition but this approach requires the test only be done once.
            if (rowY > y && row > 0) {
                --row;
            }
        } else {
            row = 0;
        }
    } else {
        row = invalidRowNumber;
    }

    return row;
}


void PageList::updatePageBoundingRectangle() {
    QRectF newBoundingRectangle;

    if (pages.isEmpty()) {
        newBoundingRectangle = QRectF(0, 0, 1, 1);
    } else {
        QRectF firstPageExtents = pages.first().extentsSceneUnits();
        QRectF lastPageExtents  = pages.last().extentsSceneUnits();

        firstPageExtents.moveTo(pages.first().currentPage->pos());
        lastPageExtents.moveTo(pages.last().currentPage->pos());

        QRectF rightMostPageExtents;
        if (currentMaximumNumberColumns < static_cast<unsigned>(pages.size())) {
            rightMostPageExtents = pages.at(currentMaximumNumberColumns - 1).extentsSceneUnits();
        } else {
            rightMostPageExtents = lastPageExtents;
        }

        newBoundingRectangle.setTopLeft(firstPageExtents.topLeft());
        newBoundingRectangle.setBottom(lastPageExtents.bottom());
        newBoundingRectangle.setRight(rightMostPageExtents.right());
    }

    if (newBoundingRectangle != currentPageBoundingRectangle) {
        currentPageBoundingRectangle = newBoundingRectangle;
        emit pageBoundingRectangleChanged(newBoundingRectangle);
    }
}
