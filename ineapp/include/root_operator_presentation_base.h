/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RootOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ROOT_OPERATOR_PRESENTATION_BASE_H
#define ROOT_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>
#include <QFont>
#include <QPointF>
#include <QSizeF>
#include <QRectF>

#include <ld_format_structures.h>
#include <ld_square_root_operator_visual.h>

#include "app_common.h"
#include "operator_presentation_base.h"

namespace EQt {
    class GraphcsMathGroup;
    class GraphicsMathGroupWithPainterPath;
}

namespace Ld {
    class Visual;
}

/**
 * This class is a common base class for root operators.  The class provides support for drawing root symbols and
 * placing child elements around those root symbols.
 */
class APP_PUBLIC_API RootOperatorPresentationBase:public OperatorPresentationBase {
    Q_OBJECT

    public:
        RootOperatorPresentationBase();

        ~RootOperatorPresentationBase() override;

    protected:
        /**
         * Class that holds information used to place children under the root symbol.
         */
        class ChildPlacementData {
            public:
                /**
                 * Constructor.
                 *
                 * \param[in] child0Rectangle Bounding rectangle for child 0.
                 *
                 * \param[in] child1Rectangle Bounding rectangle for child 1.
                 */
                ChildPlacementData(const QRectF& child0Rectangle = QRectF(), const QRectF& child1Rectangle = QRectF());

                /**
                 * Copy constructor.
                 *
                 * \param[in] other The instance to be copied to this instance.
                 */
                ChildPlacementData(const ChildPlacementData& other);

                ~ChildPlacementData();

                /**
                 * Method you can use to set the bounding rectangle for child 0.
                 *
                 * \param[in] boundingRectangle The new bounding rectangle for child 0.
                 */
                void setChild0BoundingRectangle(const QRectF& boundingRectangle);

                /**
                 * Method you can use to set the top left position of child 0.
                 *
                 * \param[in] position The new top left position of child 0.
                 */
                void setChild0TopLeft(const QPointF& position);

                /**
                 * Method you can use to set the top left position of child 0.
                 *
                 * \param[in] x The X coordinate for child 0.
                 *
                 * \param[in] y The Y coordinate for child 0.
                 */
                void setChild0TopLeft(float x, float y);

                /**
                 * Method you can use to set the top right position of child 0.
                 *
                 * \param[in] position The new top right position of child 0.
                 */
                void setChild0TopRight(const QPointF& position);

                /**
                 * Method you can use to set the top right position of child 0.
                 *
                 * \param[in] x The X coordinate for child 0.
                 *
                 * \param[in] y The Y coordinate for child 0.
                 */
                void setChild0TopRight(float x, float y);

                /**
                 * Method you can use to set the bottom left position of child 0.
                 *
                 * \param[in] position The new bottom right position of child 0.
                 */
                void setChild0BottomLeft(const QPointF& position);

                /**
                 * Method you can use to set the bottom left position of child 0.
                 *
                 * \param[in] x The X coordinate for child 0.
                 *
                 * \param[in] y The Y coordinate for child 0.
                 */
                void setChild0BottomLeft(float x, float y);

                /**
                 * Method you can use to set the bottom right position of child 0.
                 *
                 * \param[in] position The new bottom right position of child 0.
                 */
                void setChild0BottomRight(const QPointF& position);

                /**
                 * Method you can use to set the bottom right position of child 0.
                 *
                 * \param[in] x The X coordinate for child 0.
                 *
                 * \param[in] y The Y coordinate for child 0.
                 */
                void setChild0BottomRight(float x, float y);

                /**
                 * Method you can use to set the size of child 0.
                 *
                 * \param[in] size The new size of child 0.
                 */
                void setChild0Size(const QSizeF& size);

                /**
                 * Method you can use to set the size of child 0.
                 *
                 * \param[in] width The new width of child 0.
                 *
                 * \param[in] height THe new height of child 0.
                 */
                void setChild0Size(float width, float height);

                /**
                 * Method you can use to set the width of child 0.
                 *
                 * \param[in] width The new width of child 0.
                 */
                void setChild0Width(float width);

                /**
                 * Method you can use to set the height of child 0.
                 *
                 * \param[in] height THe new height of child 0.
                 */
                void setChild0Height(float height);

                /**
                 * Method you can use to obtain child 0's bounding rectangle.
                 *
                 * \return Returns the bounding rectangle for child 0.
                 */
                const QRectF& child0BoundingRectangle() const;

                /**
                 * Method you can use to obtain the top left position of child 0.
                 *
                 * \return Returns the top left position of child 0.
                 */
                QPointF child0TopLeft() const;

                /**
                 * Method you can use to obtain the top right position of child 0.
                 *
                 * \return Returns the top right position of child 0.
                 */
                QPointF child0TopRight() const;

                /**
                 * Method you can use to obtain the bottom left position of child 0.
                 *
                 * \return Returns the bottom left position of child 0.
                 */
                QPointF child0BottomLeft() const;

                /**
                 * Method you can use to obtain the bottom right position of child 0.
                 *
                 * \return Returns the bottom right position of child 0.
                 */
                QPointF child0BottomRight() const;

                /**
                 * Method you can use to obtain the left coordinate of child 0.
                 *
                 * \return Returns the left coordinate of child 0.
                 */
                float child0Left() const;

                /**
                 * Method you can use to obtain the top coordinate of child 0.
                 *
                 * \return Returns the top coordinate of child 0.
                 */
                float child0Top() const;

                /**
                 * Method you can use to obtain the right coordinate of child 0.
                 *
                 * \return Returns the right coordinate of child 0.
                 */
                float child0Right() const;

                /**
                 * Method you can use to obtain the bottom coordinate of child 0.
                 *
                 * \return Returns the bottom coordinate of child 0.
                 */
                float child0Bottom() const;

                /**
                 * Method you can use to obtain the size of child 0.
                 *
                 * \return Returns the size of child 0.
                 */
                QSizeF child0Size() const;

                /**
                 * Method you can use to obtain the width of child 0.
                 *
                 * \return Returns the width of child 0.
                 */
                float child0Width();

                /**
                 * Method you can use to obtain the height of child 0.
                 *
                 * \return Returns the height of child 0.
                 */
                float child0Height();

                /**
                 * Method you can use to set the bounding rectangle for child 1.
                 *
                 * \param[in] boundingRectangle The new bounding rectangle for child 1.
                 */
                void setChild1BoundingRectangle(const QRectF& boundingRectangle);

                /**
                 * Method you can use to set the top left position of child 1.
                 *
                 * \param[in] position The new top left position of child 1.
                 */
                void setChild1TopLeft(const QPointF& position);

                /**
                 * Method you can use to set the top left position of child 1.
                 *
                 * \param[in] x The X coordinate for child 1.
                 *
                 * \param[in] y The Y coordinate for child 1.
                 */
                void setChild1TopLeft(float x, float y);

                /**
                 * Method you can use to set the top right position of child 1.
                 *
                 * \param[in] position The new top right position of child 1.
                 */
                void setChild1TopRight(const QPointF& position);

                /**
                 * Method you can use to set the top right position of child 1.
                 *
                 * \param[in] x The X coordinate for child 1.
                 *
                 * \param[in] y The Y coordinate for child 1.
                 */
                void setChild1TopRight(float x, float y);

                /**
                 * Method you can use to set the bottom left position of child 1.
                 *
                 * \param[in] position The new bottom right position of child 1.
                 */
                void setChild1BottomLeft(const QPointF& position);

                /**
                 * Method you can use to set the bottom left position of child 1.
                 *
                 * \param[in] x The X coordinate for child 1.
                 *
                 * \param[in] y The Y coordinate for child 1.
                 */
                void setChild1BottomLeft(float x, float y);

                /**
                 * Method you can use to set the bottom right position of child 1.
                 *
                 * \param[in] position The new bottom right position of child 1.
                 */
                void setChild1BottomRight(const QPointF& position);

                /**
                 * Method you can use to set the bottom right position of child 1.
                 *
                 * \param[in] x The X coordinate for child 1.
                 *
                 * \param[in] y The Y coordinate for child 1.
                 */
                void setChild1BottomRight(float x, float y);

                /**
                 * Method you can use to set the size of child 1.
                 *
                 * \param[in] size The new size of child 1.
                 */
                void setChild1Size(const QSizeF& size);

                /**
                 * Method you can use to set the size of child 1.
                 *
                 * \param[in] width The new width of child 1.
                 *
                 * \param[in] height THe new height of child 1.
                 */
                void setChild1Size(float width, float height);

                /**
                 * Method you can use to set the width of child 1.
                 *
                 * \param[in] width The new width of child 1.
                 */
                void setChild1Width(float width);

                /**
                 * Method you can use to set the height of child 1.
                 *
                 * \param[in] height THe new height of child 1.
                 */
                void setChild1Height(float height);

                /**
                 * Method you can use to obtain child 1's bounding rectangle.
                 *
                 * \return Returns the bounding rectangle for child 1.
                 */
                const QRectF& child1BoundingRectangle() const;

                /**
                 * Method you can use to obtain the top left position of child 1.
                 *
                 * \return Returns the top left position of child 1.
                 */
                QPointF child1TopLeft() const;

                /**
                 * Method you can use to obtain the top right position of child 1.
                 *
                 * \return Returns the top right position of child 1.
                 */
                QPointF child1TopRight() const;

                /**
                 * Method you can use to obtain the bottom left position of child 1.
                 *
                 * \return Returns the bottom left position of child 1.
                 */
                QPointF child1BottomLeft() const;

                /**
                 * Method you can use to obtain the bottom right position of child 1.
                 *
                 * \return Returns the bottom right position of child 1.
                 */
                QPointF child1BottomRight() const;

                /**
                 * Method you can use to obtain the left coordinate of child 1.
                 *
                 * \return Returns the left coordinate of child 1.
                 */
                float child1Left() const;

                /**
                 * Method you can use to obtain the top coordinate of child 1.
                 *
                 * \return Returns the top coordinate of child 1.
                 */
                float child1Top() const;

                /**
                 * Method you can use to obtain the right coordinate of child 1.
                 *
                 * \return Returns the right coordinate of child 1.
                 */
                float child1Right() const;

                /**
                 * Method you can use to obtain the bottom coordinate of child 1.
                 *
                 * \return Returns the bottom coordinate of child 1.
                 */
                float child1Bottom() const;

                /**
                 * Method you can use to obtain the size of child 1.
                 *
                 * \return Returns the size of child 1.
                 */
                QSizeF child1Size() const;

                /**
                 * Method you can use to obtain the width of child 1.
                 *
                 * \return Returns the width of child 1.
                 */
                float child1Width();

                /**
                 * Method you can use to obtain the height of child 1.
                 *
                 * \return Returns the height of child 1.
                 */
                float child1Height();

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to be copied.
                 */
                ChildPlacementData& operator=(const ChildPlacementData& other);

            private:
                /**
                 * The child 0 bounding rectangle.
                 */
                QRectF currentChild0BoundingRectangle;

                /**
                 * The child 1 bounding rectangle.
                 */
                QRectF currentChild1BoundingRectangle;
        };

        /**
         * Method that inserts a root operator symbol within a graphics item.  The method will return recommended
         * locations for both child 0 and child 1 based on the provided data.
         *
         * \param[in] graphicsItem  The graphics item to receive the root symbol.  Providing a null pointer will cause
         *                          no updates to be performed.
         *
         * \param[in] format        The format instance used to calculate cap height, colors, etc.
         *
         * \param[in] relativeScale The relative scale applied to this operator.
         *
         * \param[in] basePosition  The top left corner of the root symbol or child 1.  You can use this to adjust for
         *                          for parenthenesis and other similar constructs.
         *
         * \param[in] child0Size    The size of the child placed under the root symbol.
         *
         * \param[in] child1Size    The size of the child placed as the "exponent" of the root symbol.
         *
         * \returns the recommended location for each child element.
         */
        ChildPlacementData drawRootSymbol(
            EQt::GraphicsMathGroupWithPainterPath* graphicsItem,
            Ld::FormatPointer                      format,
            float                                  relativeScale,
            const QPointF&                         basePosition,
            const QSizeF&                          child0Size,
            const QSizeF&                          child1Size = QSizeF(0.0F, 0.0F)
        );
};

#endif
