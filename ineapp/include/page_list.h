/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PageList class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PAGE_LIST_H
#define PAGE_LIST_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QPainter>

#include "app_common.h"
#include "scene_units.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsScene;
    class GraphicsItemGroup;
}

namespace Ld {
    class PageFormat;
};

/**
 * You can use this class to maintain a searchable list of page instances.  Longer term, this class can support
 * multiple schemes for stacking pages.
 */
class APP_PUBLIC_API PageList:public QObject, private SceneUnits {
    Q_OBJECT

    private:
        /**
         * Internal page object.
         */
        class Page;

    public:
        /**
         * Shadow width, in points.
         */
        static constexpr unsigned shadowWidthPoints = 6;

        /**
         * Color used for the margins.
         */
        static const QColor marginColor;

        /**
         * Color used for the active region.
         */
        static const QColor pageContentsBackgroundColor;

        /**
         * Color used for the portion of the shadow closest to the document.
         */
        static const QColor shadowColor1;

        /**
         * Color used for the portion of the shadow farthest from the document.
         */
        static const QColor shadowColor2;

        /**
         * Color used for guide lines.
         */
        static const QColor guideLineColor;

        /**
         * Pen used for the corner guides.
         */
        static const QPen guideCornerPen;

        /**
         * Pen used for the bulk of the guides, excepting the corners.
         */
        static const QPen guidePen;

        /**
         * Optimal length for the guides, in points.
         */
        static constexpr unsigned guideLengthPoints = 18;

        /**
         * Brush used for the background of the page contents.  Brush is also used for the margins, when disabled.
         */
        static const QBrush pageContentsBackgroundBrush;

        /**
         * Brush used for the margins.
         */
        static const QBrush marginBrush;

        /**
         * Distance that must be added to each page's extents.  The value is not used internally.
         */
        static constexpr unsigned pageMarginPoints = 3;

        /**
         * Type used to express a page index.
         */
        typedef unsigned Index;

        /**
         * Value used to indicate an invalid page index.
         */
        static constexpr Index invalidPageIndex = static_cast<Index>(-1);

        /**
         * A single entry into the page list holding data on a single page.
         */
        class Entry {
            friend class PageList;

            private:
                /**
                 * Constructor
                 *
                 * \param[in] pageInstance A pointer to the page to be tracked.
                 */
                Entry(PageList::Page* pageInstance);

            public:
                Entry();

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be copied.
                 */
                Entry(const Entry& other);

                ~Entry();

                /**
                 * Method you can call to determine if this page entry is valid.
                 *
                 * \return Returns true if the page entry is valid.  Returns false if the page entry is invalid.
                 */
                bool isValid() const;

                /**
                 * Method you can call to determine if this page entry is invalid.
                 *
                 * \return Returns true if the page entry is invalid.  Returns false if the page entry is valid.
                 */
                bool isInvalid() const;

                /**
                 * Method that returns the page format currently used by this page.
                 *
                 * \return Returns the page format being used by this page.
                 */
                const QSharedPointer<Ld::PageFormat> format() const;

                /**
                 * Determines the page index of this page.
                 *
                 * \return Returns the zero based page index of this page.
                 */
                Index index() const;

                /**
                 * Method you can use to obtain the current top-left corner of the page's extent within the scene.
                 *
                 * \return Returns the position of the top left corner of the page within the scene.
                 */
                QPointF position() const;

                /**
                 * Returns the page extents within the scene, in scene units.  This value represents the entire rendered
                 * page, including borders and shadows.
                 *
                 * \returns Returns a rectangle containing everything rendered for this page.  Coordinates are relative
                 *          to the page boundary.
                 */
                const QRectF extentsSceneUnits() const;

                /**
                 * Returns the page boundary within the scene, in scene units. This value represents the contents of the
                 * page that represents the paper.
                 *
                 * \return Returns the size of the paper as rendered in the scene, in scene units.  Coordinates are
                 *         relative to the page boundary.
                 */
                const QRectF boundarySceneUnits() const;

                /**
                 * Returns the active area of the page, in scene units.  This value represents the portion of the page
                 * within the page margins.
                 *
                 * \return Returns the size of active area of the page contained within the page margins, in scene
                 *         units. Coordinates are relative to the page boundary.
                 */
                const QRectF activeAreaSceneUnits() const;

                /**
                 * Returns a EQt::GraphicsItemGroup you can use for the active area of the page.
                 *
                 * \return Returns a EQt::GraphicsItemGroup you can use for this page.
                 */
                EQt::GraphicsItemGroup* activeAreaGroup() const;

                /**
                 * Returns the size of the active area of the page, in scene units.
                 *
                 * \return Returns the size of the active area, in scene units.  You can use this to control the placement of
                 *         child items within the active area group.
                 */
                QSizeF activeAreaSizeSceneUnits() const;

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this object.
                 */
                Entry& operator=(const Entry& other);

            private:
                /**
                 * The page instance that is being tracked.
                 */
                Page* currentPage;
        };

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PageList(QObject* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] newMaximumNumberColumns The new maximum number of displayed columns.  A value of zero will
         *                                    indicate a single vertical column of pages.
         *
         * \param[in] parent                  Pointer to the parent object.
         */
        PageList(unsigned newMaximumNumberColumns, QObject* parent);

        /**
         * Method you can call to clear the page list.
         */
        void clear();

        /**
         * Method you can call to change the number of displayed page columns.
         *
         * \param[in] numberColumns The new number of columns.
         */
        void setMaximumNumberColumns(unsigned numberColumns);

        /**
         * Method you can use to determine the maximum number of displayed page columns.
         *
         * \return Returns the maximum number of displayed page columns.
         */
        unsigned maximumNumberColumns() const;

        /**
         * Method you can use to quickly detemrine if the page list is empty.
         *
         * \return Returns true if the page list is empty.  Returns false if the page list is not empty.
         */
        bool isEmpty() const;

        /**
         * Method you can use to quickly detemrine if the page list is empty.
         *
         * \return Returns true if the page list is empty.  Returns false if the page list is not empty.
         */
        bool isNotEmpty() const;

        /**
         * Method you can use to determine the current number of pages.
         *
         * \return Returns the current number of pages.
         */
        Index numberPages() const;

        /**
         * Method you can use to obtain a page entry for a given page.
         *
         * \param[in] index The zero based index you can use to reference a page entry instance.
         *
         * \return Returns the page entry of interest.
         */
        const Entry& at(Index index) const;

        /**
         * Convenience subscripting operator you can use to access the page list.
         *
         * \param[in] index The zero based index you can use to reference a page entry instance.
         *
         * \return Returns the page entry of interest.
         */
        Entry operator[](Index index) const;

        /**
         * Method you can call to update the page format of a page.  Calling the method may cause other pages to resize
         * and reposition themselves.
         *
         * \param[in] pageIndex  The zero based page index of the page to be updated.
         *
         * \param[in] pageFormat A shared pointer to the page format to apply.
         *
         * \return Returns true on success, returns false on error.
         */
        bool setFormat(Index pageIndex, QSharedPointer<Ld::PageFormat> pageFormat);

        /**
         * Method you can call to append a new page.  This method will also set the position of the page based on the
         * location of other pages.  You can simply add the returned page to the scene.
         *
         * \param[in] format The format to apply to the new page.
         *
         * \param[in] parent The parent to assign the newly created page to.
         *
         * \return Returns a page entry you can use to obtain details about the page.
         */
        Entry append(QSharedPointer<Ld::PageFormat> format, QGraphicsItem* parent = Q_NULLPTR);

        /**
         * Method you can call to append a new page.  This method will also set the position of the page based on the
         * location of other pages.  You can simply add the returned page to the scene.
         *
         * \param[in] format The format to apply to the new page.
         *
         * \param[in] scene  The scene to add the page to.
         *
         * \return Returns a page entry you can use to obtain details about the page.
         */
        Entry append(QSharedPointer<Ld::PageFormat> format, EQt::GraphicsScene* scene);

        /**
         * Method you can call to truncate the page list at a specific page index.
         *
         * \param[in] pageIndex The page index of the first page to be removed from the list.  This method will delete
         *                      the pages.
         */
        void truncate(Index pageIndex);

        /**
         * Method you can call to determine the maximum width of the largest page in each column, in points.  The name
         * is a misnomer in that it represents the page boundary rather than extents.
         *
         * \return Returns ta list holding the maximum horizontal extents for each active column.
         */
        QList<double> maximumHorizontalExtentPoints() const;

        /**
         * Method you can call to determine the left edge of each column, in points.  The first column (column 0) will
         * be at 0.
         *
         * \return Returns the position of the left edge of each column, in points.
         */
        QList<double> columnLeftEdgePoints() const;

        /**
         * Method that can be called to draw per-view page decorations, margins, and guides on a paint device.
         *
         * \param[in] sceneArea      The area that requires decorations be drawn.
         *
         * \param[in] painter        The painter to use to draw the decorations.
         *
         * \param[in] includeMargins If true, the decorations will include page margins in a different shade.
         *
         * \param[in] includeGuides  If true, the decorations will include margin guide lines at the corners of the
         *                           active area.
         */
        void drawDecorations(const QRectF& sceneArea, QPainter* painter, bool includeMargins, bool includeGuides);

        /**
         * Method you can use to identify a page at a specified location.
         *
         * \param[in] position The position to locate the page at, in scene units.
         *
         * \return Returns the page entry at the specified location.  An invalid entry is returned if there is no page
         *         at the indicated location.
         */
        Entry pageAt(const QPointF& position) const;

        /**
         * Convenience method you can use to identify a page at a specified location.
         *
         * \param[in] x the X coordinate, in scene units.
         *
         * \param[in] y The Y coordinate, in scene units.
         *
         * \return Returns the page entry at the specified location.  An invalid entry is returned if there is no page
         *         at the indicated location.
         */
        Entry pageAt(double x, double y) const;

        /**
         * Method that returns the current bounding rectangle required to just hold every page instance.
         *
         * \return Returns the page bounding rectangle.
         */
        QRectF pageBoundingRectangle() const;

    signals:
        /**
         * Signal that is emitted when the maximum page extents has changed.  You can use this to adjust the lenth of
         * the visible ruler(s).
         *
         * \param[out] newMaximumHorizontalExtentsPoints The new width of the page columns, in points.  The name is a
         *                                               misnomer in that it represents the page boundary rather than
         *                                               extents.
         *
         * \param[out] newPageLeftEdgePoints             The new left edges of each page within the scene, translated to
         *                                               points.
         */
        void maximumExtentsChanged(
            const QList<double>& newMaximumHorizontalExtentsPoints,
            const QList<double>& newPageLeftEdgePoints
        );

        /**
         * Signal that is emitted when the format of a page has changed.  This signal is *not* emitted when a new page
         * is appended or when the layout of the pages is modified.
         *
         * \param[out] pageEntry The entry holding the updated page information.
         */
        void pageFormatChanged(Entry pageEntry);

        /**
         * Signal that is emitted when the bounding rectangle required to display every page changes.
         *
         * \param[out] pageBoundingRectangle The bounding rectangle that represents every page.
         */
        void pageBoundingRectangleChanged(const QRectF& pageBoundingRectangle);

    private:
        /**
         * Value used internally to indicate an invalid column number.
         */
        static constexpr unsigned invalidColumnNumber = static_cast<unsigned>(-1);

        /**
         * Value used internally to indicate an invalid row number.
         */
        static constexpr Index invalidRowNumber = static_cast<Index>(-1);

        /**
         * Value used during page row search to determine when we should transition from Newton-Raphson method to
         * binary search method.
         */
        static constexpr unsigned maximumRowSearchNewtonRaphsonIterations = 5;

        /**
         * Method that determines the maximum horizontal page width across a range of pages.
         *
         * \param[in] startingIndex The zero based starting page index (inclusive).  The value of the index is not
         *                          checked and must be valid.
         *
         * \param[in] endingIndex   The zero based ending page index (inclusive).  The value of the index is not checked
         *                          and must be valid.
         *
         * \param[in] increment     A value used to determine how to increment between pages.
         *
         * \return Returns the maximum page width across the requested number of pages, in scene units.
         */
        double maximumPageWidthSceneUnits(Index startingIndex, Index endingIndex, Index increment = 1);

        /**
         * Method that determines the maximum vertical extent across a range of pages.
         *
         * \param[in] startingIndex The zero based starting page index (inclusive).  The value of the index is not
         *                          checked and must be valid.
         *
         * \param[in] endingIndex   The zero based ending page index (inclusive).  The value of the index is not checked
         *                          and must be valid.
         *
         * \param[in] increment     A value used to determine how to increment between pages.
         *
         * \return Returns the maximum extents across the requested number of pages, in scene units.
         */
        double maximumVerticalExtentsSceneUnits(Index startingIndex, Index endingIndex, Index increment = 1);

        /**
         * Method that is used internally to reposition pages starting at a specified page index.
         *
         * \param[in] startingIndex The zero based page index of the page to reposition.
         */
        void reposition(Index startingIndex = 0);

        /**
         * Method that locates the page column based on a specified X coordinate, in scene units.
         *
         * \param[in] x              The X coordinate to locate the page column for.
         *
         * \param[in] startingColumn A value indicating the column to start scanning from.  The value is used to speed
         *                           up the search.
         *
         * \return Returns the zero based page column number.  A value of 0 is returned if the X value is to the left of
         *         the first column.  The last column is returned if the value is to the right of the last column.
         *         The method treats the shadow and spacing at each column as being part of the column to the left.  A
         *         value of PageList::invalidColumnNumber is returned if there are no columns or if the starting column
         *         is invalid based on the current number of displayed columns.
         */
        unsigned pageColumnAt(double x, unsigned startingColumn = 0) const;

        /**
         * Method that locates the page row based on a specified Y coordinate, in scene units.
         *
         * \param[in] y           The Y coordinate to locate the page row for.
         *
         * \param[in] startingRow A value indicating the row we should start scanning from.  The value is used to
         *                        speed up the search.
         *
         * \return Returns the zero based page row number.  A value of PageList::invalidRowNumber is returned if the
         *         Y value lands before the first row or if there are no pages displayed.
         */
        Index pageRowAt(double y, Index startingRow = 0) const;

        /**
         * Method that is called to update the page bounding rectangle.
         */
        void updatePageBoundingRectangle();

        /**
         * The maximum number of columsn of pages to display.  The number of actual colums of pages may be less than
         * this value.
         */
        unsigned currentMaximumNumberColumns;

        /**
         * A default entry used to allow return by reference.
         */
        Entry defaultEntry;

        /**
         * The underlying list of pages.
         */
        QList<Entry> pages;

        /**
         * The maximum horizontal page width across all pages in each column, in points.  The name is a misnomer in that
         * it represents the page boundary rather than extents.
         */
        QList<double> currentMaximumHorizontalExtentsPoints;

        /**
         * The left edge of each page column, in points.
         */
        QList<double> currentLeftPageEdgePoints;

        /**
         * A rectangle indicating the outside boundary of every page.
         */
        QRectF currentPageBoundingRectangle;
};

/**
 * The metatype for the PageList::Entry class.
 */
Q_DECLARE_METATYPE(PageList::Entry);

#endif
