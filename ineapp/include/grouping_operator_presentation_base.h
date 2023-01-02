/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref GroupingOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef GROUPING_OPERATOR_PRESENTATION_BASE_H
#define GROUPING_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QFont>
#include <QPen>

#include <eqt_graphics_math_group.h>

#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_operator_format.h>

#include "app_common.h"
#include "scene_units.h"
#include "presentation_area_tracker.h"
#include "presentation_with_fixed_children.h"

class QGraphicsItem;

class PlacementNegotiator;
class PlacementTracker;
class Presentation;
class PresentationArea;

/**
 * You can use this class as a base class for grouping operator presentations with a single child element.
 */
class APP_PUBLIC_API GroupingOperatorPresentationBase:public PresentationWithFixedChildren {
    Q_OBJECT

    public:
        /**
         * Type used to represent a parenthesis style.
         */
        typedef EQt::GraphicsMathGroup::ParenthesisStyle ParenthesisStyle;

        GroupingOperatorPresentationBase();

        ~GroupingOperatorPresentationBase() override;

        /**
         * Method that is called by a child to the parent to request that the parent reposition the contents of the
         * child.
         *
         * \param[in] childPresentation The presentation that needs to be repositioned.
         */
        void requestRepositioning(Presentation* childPresentation) override;

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
        void recalculatePlacement(
            PlacementTracker*    placementTracker,
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            Presentation*        nextSibling,
            bool                 honorLeadingWhitespace,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) override;

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
        void redoPlacement(
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            unsigned long        firstPresentationAreaId = 0,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) override;

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
        QSizeF requestArea(unsigned long childIdentifier, SpaceQualifier* spaceQualifier = Q_NULLPTR) override;

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
        void allocateArea(
            unsigned long childIdentifier,
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0,
            bool          canStretch = false
        ) override;

        /**
         * Method called by a child to indicate that the provided space by PlacementNegotiator::requestArea is
         * insufficient.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] size               The last reported available space.  This value must be equal to the space
         *                               reported by PlacementNegotiator:requestArea.
         */
        void areaInsufficient(unsigned long childIdentifier, const QSizeF& size) override;

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
        void applyStretch(unsigned long presentationAreaId, float stretchFactor) override;

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
        QGraphicsItem* graphicsItem(unsigned long presentationAreaId) const override;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.  Note that this method must call the
         * \ref PlacementNegotiator::resetPlacement method of every child.
         */
        void resetPlacement() override;

        /**
         * Method that is called when this placement negotiator needs to remove itself from the scene.  You should
         * overload this method in derived classes with code that removes and optionally destroys placement areas from
         * the parent.
         */
        void removeFromScene() override;

        /**
         * Method that is called to translate an element position into a bounding rectangle in scene coordinates.  The
         * default implementation will assert if the element supports one or more text regions.  The default
         * implemetnation also assumes that presentation area IDs count upwards from 0.
         *
         * \param[in] endingElementCursor   The element cursor to use to determine the ending location.  The end of the
         *                                  presentation will be used if the element is not the element behind the
         *                                  presentation.
         *
         * \param[in] startingElementCursor The element cursor to use to determine the starting location.  The start of
         *                                  the presentation will be used if the element is not the element behind the
         *                                  presentation.
         *
         * \return Returns a list of bounding rectangles in scene coordinates that represent the extents of the
         *         presentation.  The default implementation will return a rectangle, translated to scene coordinates
         *         for each presentation area.  An empty list indicates that the element has not been placed and thus
         *         the cursor position could not be determined.
         */
        QList<QRectF> cursorRangeToScene(
            const Ld::ElementCursor& endingElementCursor,
            const Ld::ElementCursor& startingElementCursor = Ld::ElementCursor()
        ) const override;

    protected slots:
        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread
         * boundaries.
         *
         * This version will cast the graphics item back to a EQt::GraphicsMathGroup and set the border pen color to
         * yellow or red based on the diagnostic type.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void processDiagnostic(Ld::DiagnosticPointer diagnostic) override;

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.  The
         * default implementation will remove any red borders drawn around the graphics items tied to this presentation.
         *
         * This version will cast the graphics item back to a EQt::GraphicsMathGroup and set no border pen.
         */
        void clearDiagnosticDisplay() override;

        /**
         * Slot that is triggered when a child element is removed.  You can overload this slot if you wish to receive
         * notification that a child presentation should be removed.  This slot is ultimately triggered by the methods:
         *
         *     * ElementWithFixedChildren::removeChild(Element*)
         *     * ElementWithFixedChildren::removeChild(unsigned long)
         *
         * The default implementation will trigger updates to the visual representation of the document.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread
         * boundaries.
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child element being removed.
         */
        void processPresentationRemoved(unsigned long childIndex, Presentation* childPresentation) override;

        /**
         * Slot that is triggered when a child element is added.  You can overload this slot if you wish to receive
         * notification that a child presentation has been added.  This slot is ultimately triggered by the
         * ElementWithFixedChildren::setChild method.
         *
         * The default implementation will trigger updates to the visual representation of the document.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child element being placed.
         */
        void processChildPresentationSet(unsigned long childIndex, Presentation* childPresentation) override;

    protected:
        /**
         * Pure virtual method you should overload to provide the left parenthesis style.
         *
         * \param[in] format The format instance tied to this operator.
         *
         * \return Returns the style to apply to the left parenthesis.
         */
        virtual ParenthesisStyle leftParenthesisStyle(Ld::FormatPointer format) const = 0;

        /**
         * Pure virtual method you should overload to provide the right parenthesis style.
         *
         * \param[in] format The format instance tied to this operator.
         *
         * \return Returns the style to apply to the right parenthesis.
         */
        virtual ParenthesisStyle rightParenthesisStyle(Ld::FormatPointer format) const = 0;

        /**
         * Method you can call to clear all the child presentation area trackers. You can use this to force all the
         * child trackers to be repopulated.
         */
        void clearChildPresentationAreas();

    private:
        /**
         * The group used to contain this widget.
         */
        EQt::GraphicsMathGroup* currentGraphicsItem;

        /**
         * The child presentation data.
         */
        PresentationAreaTracker currentChildPresentationArea;

        /**
         * The required size for the area.
         */
        QSizeF requiredSize;

        /**
         * The required ascent for the area.
         */
        float requiredAscent;
};

#endif
