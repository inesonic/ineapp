/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ListPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LIST_PRESENTATION_BASE_H
#define LIST_PRESENTATION_BASE_H

#include <QObject>
#include <QList>

#include <eqt_graphics_math_group.h>

#include <ld_literal_element.h>
#include <ld_element_cursor.h>
#include <ld_visual.h>
#include <ld_literal_visual.h>
#include <ld_character_format.h>

#include "app_common.h"
#include "presentation_area_tracker.h"
#include "presentation_with_positional_children.h"

class QGraphicsItem;
class QFontMetricsF;

class PlacementTracker;

/**
 * Base class for presentations of arrays, sets, vectors, and tuples.  The class also includes protected methods
 * allowing you to directly access the underlying graphics item.  This last feature allows you to use this class as a
 * basis for other specialized presentation classes that operate on positional children.
 */
class APP_PUBLIC_API ListPresentationBase:public PresentationWithPositionalChildren {
    Q_OBJECT

    public:
        ListPresentationBase();

        ~ListPresentationBase() override;

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
         *         the presentationAreaId is invalid.
         */
        QGraphicsItem* graphicsItem(unsigned long presentationAreaId) const override;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.
         */
        void resetPlacement() final;

        /**
         * Method that is called when this placement negotiator needs to remove itself from the scene.  You should
         * overload this method in derived classes with code that removes and optionally destroys placement areas from
         * the parent.
         */
        void removeFromScene() final;

        /**
         * Method that is called to translate an element position into a bounding rectangle in scene coordinates.
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
         *         presentation.  An empty list indicates that the element has not been placed and thus the cursor
         *         position could not be determined.
         */
        QList<QRectF> cursorRangeToScene(
            const Ld::ElementCursor& endingElementCursor,
            const Ld::ElementCursor& startingElementCursor = Ld::ElementCursor()
        ) const final;

    protected:
        /**
         * Method you can overload to determine the parenthesis style to use for this list.
         *
         * \return Returns the parenthesis style you should display for this list style.
         */
        virtual EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle() const;

        /**
         * Method you can use to change the character used to separate elements.
         *
         * \return Returns the characters used to separate elements.  The default implementation returns a commma and
         *         space.
         */
        virtual QString separatorCharacters() const;

        /**
         * Method you can use to set the underlying graphics item.  Any existing graphics item will be deleted.
         *
         * \param[in] newGraphicsItem The new graphics item.
         */
        void setGraphicsItem(EQt::GraphicsMathGroup* newGraphicsItem);

        /**
         * Method you can use to obtain the underlying graphics item.
         *
         * \return Returns a pointer to the underlying graphics item.
         */
        EQt::GraphicsMathGroup* graphicsItem() const;

        /**
         * Method you can call to populate the presentation area trackers.
         *
         * \param[in] placementTracker  A pointer to the placement tracker that can be used to track and update status
         *                              during a time-consuming placement operation.
         *
         * \param[in] minimumTopSpacing Spacing to apply above the first line of the placement area.  The value is
         *                              primarily intended to be used to manage paragraph spacing.  A negative value
         *                              indicates that there should be no imposed top spacing.
         *
         * \param[in] relativeScale     A hint value used to indicate if this child should try to scale smaller than
         *                              usual. This value is intended for cases where an equation element needs to be
         *                              rendered in a smaller than usual font.  A value of 1.0 indicates normal scale.
         *                              The value is intended as a hint, children are not required to honor this
         *                              parameter.
         *
         * \return Returns a reference to the list of presentation area trackers.
         */
        QList<PresentationAreaTracker>& populatePresentationAreaTrackers(
            PlacementTracker* placementTracker,
            float             minimumTopSpacing,
            float             relativeScale
        );

        /**
         * Method you can call to obtain the current list of presentation area trackers.
         *
         * \return Returns a reference to the list of presentation area trackers.
         */
        QList<PresentationAreaTracker>& presentationAreaTrackers();

        /**
         * Method you can use to determine the maximum ascent and descent across all the presentation areas.
         *
         * \param[in,out] maximumAscent  A value that will be filled with the maximum ascent. You should either
         *                               initialize the value to 0 or populate with the currently known maximum ascent.
         *
         * \param[in,out] maximumDescent A value that will be filled with the maximum descent. You should either
         *                               initialize the value to 0 or populate with the currently known maximum
         *                               descent.
         */
        void calculateMaximumPresentationAreaAscentAndDescent(float& maximumAscent, float& maximumDescent) const;

        /**
         * Method you can call to allocate space for this presentation and set the required ascent.
         *
         * \param[in] parent          Pointer to the parent placement negotiator.
         *
         * \param[in] childIdentifier The child identifier used to identify this presentation.
         *
         * \param[in] newSize         The new size to assign to this presentation.
         *
         * \param[in] newAscent       The new ascent to assign to this presentation.
         */
        void allocateSpaceForThisPresentation(
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            const QSizeF&        newSize,
            float                newAscent
        );

    protected slots:
        /**
         * Slot that is triggered when an aspect of the associated Ld::Element has been changed.  This slot exists to
         * cover such things as text updates that are not covered by the addition or removal of children, nor format
         * updates.
         */
        void processDataChange() override;

        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread boundaries.
         * boundaries.
         *
         * This version will cast the graphics item back to a EQt::GraphicsMultiTextGroup and set the border pen color
         * to yellow or red based on the diagnostic type.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void processDiagnostic(Ld::DiagnosticPointer diagnostic) override;

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.  The
         * default implementation will remove any red borders drawn around the graphics items tied to this presentation.
         *
         * This version will cast the graphics item back to a EQt::GraphicsMultiTextGroup and set no border pen.
         */
        void clearDiagnosticDisplay() override;

        /**
         * Slot that is triggered when a child is removed by the associated
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by both the
         * Ld::ElementWithPositionalChildren::removeChild(Ld::ElementPointer) method and the
         * Ld::ElementWithPositionalChildren::removeChild(unsigned long) method.
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child element being removed.
         */
        void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation) override;

        /**
         * Slot that is triggered when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void processChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation) override;

        /**
         * Slot that is triggered when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void processChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation) override;

    private:
        /**
         * The child presentation areas.
         */
        QList<PresentationAreaTracker> childPresentationAreas;

        /**
         * The required ascent
         */
        float requiredAscent;

        /**
         * The graphics item.
         */
        EQt::GraphicsMathGroup* currentGraphicsItem;
};

#endif
