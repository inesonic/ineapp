/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlacementNegotiator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEMENT_NEGOTIATOR_H
#define PLACEMENT_NEGOTIATOR_H

#include <QObject>
#include <QSizeF>
#include <QPointF>
#include <QRectF>

#include <ld_diagnostic.h>
#include <ld_visual.h>

#include "app_common.h"

class QGraphicsItem;

namespace Ld {
    class Element;
};

class Presentation;
class PlacementTracker;

/**
 * This is a pure virtual base class for classes used to present and manipulate the position of language elements
 * in the graphics scene.
 *
 * Note that all dimensions are in scene units.
 */
class APP_PUBLIC_API PlacementNegotiator {
    public:
        /**
         * Value used to indicate an invalid child identifier.
         */
        static constexpr unsigned long invalidChildIdentifier = static_cast<unsigned long>(-1);

        /**
         * Value used to indicate the first presentation area ID.
         */
        static constexpr unsigned long firstPresentationArea = 0;

        /**
         * A value that can be used to indicate the default line spacing.
         */
        static constexpr float defaultLineSpacing = 1.0;

        /**
         * A value that can be used to indicate no top spacing to be imposed when recalculating placement.
         */
        static const double imposeNoTopSpacing;

        /**
         * Enumeration of placement locations.
         */
        enum class SpaceQualifier {
            /**
             * The provided space is the maximum that we can expect to have for both width and height.  We should
             * use the provided space even if it means overflowing.
             *
             * This qualifier generally indicates the start of a page or the start of a table cell.
             */
            MAXIMUM_WIDTH_AND_HEIGHT = 0,

            /**
             * The provided space is the maximum width we can expect to have.  Rejecting the provided space will likely
             * cause a new area to be provided with additional height but not width.  We should use the provided space
             * if we're width constrained but not height constrained.
             *
             * This qualifier generally indicates the start of a line within a page or table cell.
             */
            MAXIMUM_WIDTH = 1,

            /**
             * The provided space is within a block that can not be re-formatted if space is rejected.  You should
             * use the space provided even if doing so will cause us to overflow.
             *
             * This qualifier generally indicates that we're rendering within an equation.
             */
            USE_PROVIDED = 2,

            /**
             * The provided space is within a flowing section of some type.  Requesting additional space will likely
             * provide a new area with additional width and/or height.
             *
             * This qualifier generally indicates that we're rendering within a paragraph and not at the start of a
             * line.
             */
            CURRENT_AVAILABLE = 3
        };

        PlacementNegotiator();

        virtual ~PlacementNegotiator();

        /**
         * Method that is called by a child to the parent to request that the parent reposition the contents of the
         * child.
         *
         * \param[in] childPresentation The presentation that needs to be repositioned.
         */
        virtual void requestRepositioning(Presentation* childPresentation) = 0;

        /**
         * Method that is called by a parent to a child to tell the child to start the placement operation,
         * recalculating sizes and positions.  This method will repeatedly call the parent's instance of
         * \ref PlacementNegotiator::requestArea followed by the parent's instance of one of
         * \ref PlacementNegotiator::allocateArea or \ref PlacementNegotiator::areaInsufficient until the data
         * managed by the child is processed and placed.
         *
         * \param[in] placementTracker       A pointer to the placement tracker that can be used to track and update
         *                                   status during a time-consuming placement operation.
         *
         * \param[in] parent                 Pointer to the parent presentation negotiator.  Provided so that the child
         *                                   does not have to do the bit of additional work to identify its parent.
         *
         * \param[in] childIdentifier        An identifier the parent uses to quickly identify the child.
         *
         * \param[in] nextSibling            A pointer to the next sibling under the parent.  This child can use this to
         *                                   determine if the next sibling's space requirements should be taken into
         *                                   account when allocating room for this child.
         *
         * \param[in] honorLeadingWhitespace If true, the child should honor, and not remove, whitespace at the left
         *                                   side of the string.
         *
         * \param[in] minimumTopSpacing      Spacing to apply above the first line of the placement area.  The value
         *                                   is primarily intended to be used to manage paragraph spacing.  A negative
         *                                   value indicates that there should be no imposed top spacing.
         *
         * \param[in] lineSpacing            Additional line spacing that should be optionally included by this child.
         *                                   Children can adjust the height they report for their presentation areas.
         *                                   A value of 1.0 indicates single spacing.  A value of 1.5 indicates 1 1/2
         *                                   line spacing.  A value of 2.0 indicates double spacing.
         *
         * \param[in] relativeScale          A hint value used to indicate if this child should try to scale smaller
         *                                   than usual. This value is intended for cases where an equation element
         *                                   needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                   indicates normal scale.  The value is intended as a hint, children are not
         *                                   required to honor this parameter.
         */
        virtual void recalculatePlacement(
            PlacementTracker*    placementTracker,
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            Presentation*        nextSibling,
            bool                 honorLeadingWhitespace,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) = 0;

        /**
         * Method that is called by a parent to a child to tell the child to start the placement operation using
         * existing data.  This method will call the parent \ref PlacementNegotiator::allocateArea instance on each
         * already analyzed placement area.
         *
         * \param[in] parent                  Pointer to the parent presentation negotiator.  Provided so that the child
         *                                    does not have to do the bit of additional work to identify its parent.
         *
         * \param[in] childIdentifier         An identifier the parent uses to quickly identify the child.
         *
         * \param[in] firstPresentationAreaId The zero based ID of the first presentation area to place.
         *
         * \param[in] minimumTopSpacing       Spacing to apply above the first line of the placement area.  The value
         *                                    is primarily intended to be used to manage paragraph spacing.  A negative
         *                                    value indicates that there should be no imposed top spacing.
         *
         * \param[in] lineSpacing             Additional line spacing that should be optionally included by this child.
         *                                    Children can adjust the height they report for their presentation areas.
         *                                    A value of 1.0 indicates single spacing.  A value of 1.5 indicates 1 1/2
         *                                    line spacing.  A value of 2.0 indicates double spacing.
         *
         * \param[in] relativeScale           A hint value used to indicate if this child should try to scale smaller
         *                                    than usual. This value is intended for cases where an equation element
         *                                    needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                    indicates normal scale.  The value is intended as a hint, children are not
         *                                    required to honor this parameter.
         */
        virtual void redoPlacement(
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            unsigned long        firstPresentationAreaId = 0,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) = 0;

        /**
         * Method called by the parent to determine the spacing that should be applied between this placement negotiator
         * and the next placement negotiator.  You should also return 0 for elements that are not full width.
         *
         * \return Returns the spacing to apply between this placement negotiator and the next placement negotiator.
         *         The spacing should be the maximum of this value and any top spacing requirement for the next
         *         placement negotiator.  The default implementation returns 0.
         */
        virtual float bottomSpacingSceneUnits();

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
        virtual QSizeF requestArea(unsigned long childIdentifier, SpaceQualifier* spaceQualifier = Q_NULLPTR) = 0;

        /**
         * Method called by a child to a parent to allocate the next available area.
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
        virtual void allocateArea(
            unsigned long childIdentifier,
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0,
            bool          canStretch = false
        ) = 0;

        /**
         * Method called by a child to indicate that the provided space by PlacementNegotiator::requestArea is
         * insufficient.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] size               The last reported available space.  This value must be equal to the space
         *                               reported by PlacementNegotiator:requestArea.
         */
        virtual void areaInsufficient(unsigned long childIdentifier, const QSizeF& size) = 0;

        /**
         * Method called by a parent to apply a stretch factor to a child.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.
         *
         * \param[in] stretchFactor      The stretch factor to be applied to this child's presentation area.  A value of
         *                               0 indicates normal unstretched width.  A value of 0.1 indicates that the area
         *                               should be stretched to 110% of it's normal width.  A value of 1.0 indicates
         *                               that the area should be twice as wide as normal.
         *
         * \return Returns the new stretched width.
         */
        virtual void applyStretch(unsigned long presentationAreaId, float stretchFactor) = 0;

        /**
         * Method called by a parent to obtain a specified child graphics item, by presentation area ID.
         *
         * \param[in] presentationAreaId A zero based presentation area ID indicating which graphics region is of
         *                               interest.
         *
         * \return Returns a pointer to a QGraphicsItem used to represent this visual.  A null pointer is returned if
         *         the presentationAreaId is invalid.  Note that the \ref PlacementNegotiator or \ref Presentation
         *         instance always maintains full ownership of the graphics items and is responsible for managing their
         *         life cycle.
         */
        virtual QGraphicsItem* graphicsItem(unsigned long presentationAreaId) const = 0;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.  Note that this method must call the
         * \ref PlacementNegotiator::resetPlacement method of every child.
         */
        virtual void resetPlacement() = 0;

        /**
         * Method that is called when this placement negotiator needs to remove itself from the scene.  You should
         * overload this method in derived classes with code that removes and optionally destroys placement areas from
         * the parent.
         */
        virtual void removeFromScene() = 0;
};

#endif
