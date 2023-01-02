/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DataTypePresenter class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DATA_TYPE_PRESENTER_H
#define DATA_TYPE_PRESENTER_H

#include <QList>

#include <ld_text_element.h>
#include <ld_element_cursor.h>
#include <ld_visual.h>
#include <ld_text_visual.h>
#include <ld_character_format.h>

#include "app_common.h"
#include "placement_negotiator.h"
#include "leaf_presentation.h"

class QGraphicsItem;
class QFontMetricsF;

namespace EQt {
    class GraphicsTextItem;
}

/**
 * This pure-virtual base class defines the interface used for DataTypePresentationGenerator classes to render data
 * values.  The class provides an API similar to the \ref Presentation class but in a way that allows the
 * generator to operate without maintaining state information.
 */
class APP_PUBLIC_API DataTypePresenter {
    public:
        /**
         * Trivial class used to store information about a presentation area.  This class will *not* take ownership of
         * the underlying graphics object so you are expected to destroy it manually.
         */
        class PresentationArea {
            public:
                PresentationArea();

                /**
                 * Constructor
                 *
                 * \param[in] graphicsItem The graphics item tracked by this class.
                 *
                 * \param[in] size         The size of this area, in scene units.
                 *
                 * \param[in] ascent       The ascent to associate with this graphics item, in scene units.
                 */
                PresentationArea(QGraphicsItem* graphicsItem, const QSizeF& size, float ascent);

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be assigned to this instance.
                 */
                PresentationArea(const PresentationArea& other);

                ~PresentationArea();

                /**
                 * Method you can use to set the graphics item tracked by this presentation area.  Any existing
                 * graphics item will be destroyed.
                 *
                 * \param[in] newGraphicsItem The new graphics item to be tracked.
                 */
                void setGraphicsItem(QGraphicsItem* newGraphicsItem);

                /**
                 * Method you can use to obtain the graphics item tracked by this presentation area.
                 *
                 * \return Returns a pointer to the associated graphics item.
                 */
                QGraphicsItem* graphicsItem() const;

                /**
                 * Method you can use to set the area size, in scene units.
                 *
                 * \param[in] newSize The new area size, in scene units.
                 */
                void setSize(const QSizeF& newSize);

                /**
                 * Method you can use to obtain the size, in scene units.
                 *
                 * \return Returns the size of the graphics item, in scene units.
                 */
                const QSizeF& size() const;

                /**
                 * Method you can use to set the ascent of the graphics item, in scene units.
                 *
                 * \param[in] newAscent The new ascent, in scene units.
                 */
                void setAscent(float newAscent);

                /**
                 * Method you can use to obtain the current ascent of the graphics item, in scene units.
                 *
                 * \return Returns the current ascent of the item.  In scene units.
                 */
                float ascent() const;

                /**
                 * Move assignment operator.
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                PresentationArea& operator=(const PresentationArea& other);

            private:
                /**
                 * The current graphics item.
                 */
                QGraphicsItem* currentGraphicsItem;

                /**
                 * The current size, in scene units.
                 */
                QSizeF currentSize;

                /**
                 * The current ascent, in scene units.
                 */
                float currentAscent;
        };

        /**
         * Type used to represent a list of presentation areas.
         */
        typedef QList<PresentationArea> PresentationAreas;

        DataTypePresenter();

        virtual ~DataTypePresenter();

        /**
         * Method called by the DataTypePresentationGenerator to request space.  This method should marshall the
         * request to the parent of the presentation operating with the generator.
         *
         * \param[out] spaceQualifier The qualifier defining the constraints on the provided space.
         *
         * \return Returns the width and height of the next available area on the current line.  Values represent the
         *         maximum available space in each dimension.
         */
        virtual QSizeF requestDataTypeArea(PlacementNegotiator::SpaceQualifier* spaceQualifier = Q_NULLPTR) = 0;

        /**
         * Method called by the DataTypePresentationGenerator to allocate a subset of the requested space.  The
         * method should marshall the requests to the parent of the presentation operating with the generator.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.  This value
         *                               must correlate with the areas returned by
         *                               DataTypePresentationGenerator::generateGraphicsItems.
         *
         * \param[in] size               The required space.  This value must be less than or equal to the space
         *                               required by PlacementNegotiator:requestArea.
         *
         * \param[in] ascent             Ascent used for text to perform vertical alignment.  Non text elements should
         *                               set this value to 0.
         */
        virtual void allocateDataTypeArea(
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0
        ) = 0;

        /**
         * Method called by the DataTypePresentationGenerator to report that the provided space is insufficient.  The
         * method should marshall the request to the parent of the presentation operating with the generator.
         *
         * \param[in] size The total provided space.  This value should equal the space reported by
         *                 DataTypePresenter::requestArea.
         */
        virtual void dataTypeAreaInsufficient(const QSizeF& size) = 0;

        /**
         * Method that can be called to determine the maximum space that can ever be made available for a data type
         * presentation.
         *
         * \return Returns the maximum space that can be made available for a data type presentation.
         */
        virtual QSizeF largestAvailableAreaSceneUnits() const = 0;
};

#endif
