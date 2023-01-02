/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PageList::Page class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PAGE_LIST_PAGE_H
#define PAGE_LIST_PAGE_H

#include <QRectF>
#include <QGraphicsItem>

#include "scene_units.h"
#include "page_list.h"

class QGraphicsRectItem;
class QGraphicsPolygonItem;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace EQt {
    class GraphicsItemGroup;
}

namespace Ld {
    class PageFormat;
};

/**
 * Class that manages the rendering of the page contents.
 *
 * The \ref Page instance handles rendering of the page background and border shadows.  The Page instance also
 * determines the location of guidelines and margin shadows but does not take care of the rendering.  Rendering for the
 * guidlines and margin shadows is handled by the \ref Editor using information provided by the \ref Page class.
 */
class PageList::Page:public QGraphicsItem, private SceneUnits {
    public:
        /**
         * Constructor
         *
         * \param[in] newPageFormat The page format that should be used for this page.
         *
         * \param[in] parent        Pointer to the parent item.
         */
        Page(QSharedPointer<Ld::PageFormat> newPageFormat, QGraphicsItem* parent = Q_NULLPTR);

        ~Page() override;

        /**
         * Method that returns the page format currently used by this page.
         *
         * \return Returns the page format being used by this page.
         */
        QSharedPointer<Ld::PageFormat> format() const;

        /**
         * Method that updates the page format current used by this page.
         *
         * \param[in] newPageFormat The new page format.
         */
        void setFormat(QSharedPointer<Ld::PageFormat> newPageFormat);

        /**
         * Determines the page index of this page.
         *
         * \return Returns the zero based page index of this page.
         */
        PageList::Index index() const;

        /**
         * Sets the page index of this page.
         *
         * \param[in] newPageIndex The new page index for this page.
         */
        void setIndex(PageList::Index newPageIndex);

        /**
         * Returns the page extents within the scene, in scene units.  This value represents the entire rendered page,
         * including borders and shadows.
         *
         * \returns Returns a rectangle containing everything rendered for this page.
         */
        const QRectF& extentsSceneUnits() const;

        /**
         * Returns the page boundary within the scene, in scene units. This value represents the contents of the page
         * that represents the paper.
         *
         * \return Returns a rectangle indicating the size and position of the page boundary, in scene units.
         */
        const QRectF& boundarySceneUnits() const;

        /**
         * Returns the active area of the page, in scene units.  This value represents the portion of the page within
         * the page margins.
         *
         * \return Returns a rectangle indicating the size and position of active area of the page contained within the
         *         page margins, in scene units.
         */
        const QRectF& activeAreaSceneUnits() const;

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
         * Method that is called by the EQt::GraphicsScene class to determine the visual bounding rectangle for this
         * object.
         *
         * \return Returns the bounding rectangle for this object.
         */
        QRectF boundingRect() const final;

        /**
         * Method that is called to paint this item on a viewing area.
         *
         * \param[in] painter The painter used to perform the drawing.
         *
         * \param[in] option  Style options used when rendering this item.
         *
         * \param[in] widget  The widget to draw on.
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = Q_NULLPTR) final;

    private:
        /**
         * Method that is called to calculate the various dimensions associated with the page, in scene units.
         */
        void calculatePageDimensions();

        /**
         * The format used for this page.
         */
        QSharedPointer<Ld::PageFormat> currentFormat;

        /**
         * The zero based page index associated with this page.
         */
        PageList::Index currentIndex;

        /**
         * The page size, in pixels.
         */
        QRectF currentExtentsSceneUnits;

        /**
         * The page size, in pixels.
         */
        QRectF currentBoundarySceneUnits;

        /**
         * The page size, in pixels.
         */
        QRectF currentActiveAreaSceneUnits;

        /**
         * A rectangle used to draw the page background.
         */
        QGraphicsRectItem* pageBackgroundRect;

        /**
         * The active area on the page.
         */
        EQt::GraphicsItemGroup* currentActiveArea;

        /**
         * Polygon used to draw the right side shadow.
         */
        QGraphicsPolygonItem* rightPageShadowPolygon;

        /**
         * Polygon used to draw the bottom page shadow.
         */
        QGraphicsPolygonItem* bottomPageShadowPolygon;
};

#endif
