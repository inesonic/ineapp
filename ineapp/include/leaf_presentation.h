/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LeafPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LEAF_PRESENTATION_H
#define LEAF_PRESENTATION_H

#include <QObject>
#include <QSizeF>

#include "app_common.h"
#include "presentation_with_no_children.h"

class QGraphicsItem;

class Presentation;

/**
 * This is a pure virtual base class used to present language elements.  The class combines the functions of the
 * \ref PresentationWithNoChildren class with the API provided by the \ref PlacementNegotiator class to provide a single
 * base class you can use for language element presentations that do not hold other children.  Implementations of a
 * subset of the pure methods defined by \ref PlacementNegotiator are also provided.
 *
 * To use this class, you must extend the methods:
 *
 *     * \ref PlacementNegotiator::startPlacement()
 *     * \ref PlacementNegotiator::applyStretch()
 *     * \ref PlacementNegotiator::graphicsItem()
 *
 * To provide placement support for your specific language element.
 */
class APP_PUBLIC_API LeafPresentation:public PresentationWithNoChildren {
    Q_OBJECT

    public:
        LeafPresentation();

        ~LeafPresentation() override;

    private:
        /**
         * Method that is called by a child to the parent to request that the parent reposition the contents of the
         * child.
         *
         * This version will assert when called.
         *
         * \param[in] childPresentation The presentation that needs to be repositioned.
         */
        void requestRepositioning(Presentation* childPresentation) final;

        /**
         * Method called by a child to request the next available area on the current line.  Note that this method does
         * not actually modify the parent.
         *
         * \param[in]  childIdentifier An identifier this child must provide to the parent to help it quickly identify
         *                             the child.
         *
         * \param[out] spaceQualifier  A qualifier indicating the nature of the provided space.
         *
         * \return Returns the width and height of the next available area on the current line.  Values represent the
         *         maximum available space in each dimension.
         */
        QSizeF requestArea(unsigned long childIdentifier, SpaceQualifier* spaceQualifier = Q_NULLPTR) final;

        /**
         * Method called by a child to a parent to allocate the next available area.
         *
         * This version will assert when called.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.
         *
         * \param[in] size               The required space.  This value must be less than or equal to the space
         *                               required by PlacementNegotiator:requestArea.
         *
         * \param[in] ascent             Ascent used for text to perform vertical alignment.  Non text elements should
         *                               set this value to 0.
         *
         * \param[in] canStretch         If true, the parent can ask the child to horizontally stretch this presentation
         *                               area.  This parameter is used to justify text.
         */
        void allocateArea(
            unsigned long childIdentifier,
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0,
            bool          canStretch = false
        ) final;

        /**
         * Method called by a child to indicate that the provided space by both PlacementNegotiator::requestArea
         * and PlacementNegotiator::requestFullWideArea are insufficient.
         *
         * This version will assert when called.
         *
         * \param[in] childIdentifier The identifier used by the parent to track this child.
         *
         * \param[in] size            The required space.  This value must be less than or equal to the space required
         *                            by PlacementNegotiator:requestArea.
         */
        void areaInsufficient(unsigned long childIdentifier, const QSizeF& size) final;
};

#endif
