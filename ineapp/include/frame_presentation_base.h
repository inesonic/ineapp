/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FramePresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FRAME_PRESENTATION_BASE_H
#define FRAME_PRESENTATION_BASE_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QList>

#include <ld_element.h>
#include <ld_diagnostic_structures.h>

#include "app_common.h"
#include "presentation_with_positional_children.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsItemGroup;
}

class PlacementNegotiator;
class PlacementTracker;
class Presentation;
class PresentationArea;

/**
 * This class presents a base class for presentations that align children vertically.  The class can be used in
 * cases where line/word wrapping is not needed.  Also see the class \ref ParagraphPresentationBase.
 */
class APP_PUBLIC_API FramePresentationBase:public PresentationWithPositionalChildren {
    Q_OBJECT

    public:
        FramePresentationBase();

        ~FramePresentationBase() override;

        /**
         * Static method that creates an instance of a \ref FramePresentationBase class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::POSITIONAL.
         */
        Ld::Element::ChildPlacement childPlacement() const override;

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
         * \param[in] childIdentifier The identifier used by the parent to track this child.
         *
         * \param[in] size            The last reported available space.  This value must be equal to the space
         *                            reported by PlacementNegotiator:requestArea.
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

    protected:
        /**
         * Method that is called to determine the left margin to apply to all frame elements.
         *
         * \return Returns the left margin to apply to all lines, except the first line, in scene units.
         */
        virtual float leftMarginSceneUnits() const = 0;

        /**
         * Method that is called to determine the right margin to apply to all frame elements.
         *
         * \return Returns the right margin to apply to all lines, except the first line, in scene units.
         */
        virtual float rightMarginSceneUnits() const = 0;

        /**
         * Method that is called to determine additional spacing to be placed at the top of the first frame element.
         *
         * \return Returns spacing, in scene units, to be applied to the top of the first active area.
         */
        virtual float topSpacingSceneUnits() const = 0;

    private slots:
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
         * \param[in] diagnostic A code indicating the type of error.
         */
        void processDiagnostic(Ld::DiagnosticPointer diagnostic) override;

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.
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
         * Method that returns the next active area rectangle.
         *
         * \param[in] isFirst If true, this is the first active area rectangle.  If false, we've already requested an
         *                    active area rectangle.
         */
        void requestNewActiveAreaRectangle(bool isFirst = false);

        /**
         * Method that is called to call the parent to allocate the last processed active area.
         */
        void allocateLastActiveArea();

        /**
         * Method that is called to remove any remaining, unused graphics items.
         *
         * \param[in] numberActiveAreas A count of the number of active areas to retain.
         */
        void trimExtraGraphicsItems(unsigned numberActiveAreas);

        /**
         * The current parent placement negotiator.
         */
        PlacementNegotiator* currentParentNegotiator;

        /**
         * The child identifier we should report as when talking to the parent.
         */
        unsigned long currentChildIdentifier;

        /**
         * The current active area rectangle.
         */
        QRectF currentActiveArea;

        /**
         * The current space qualifier.  We adjust this as we issue space to child presentation areas.
         */
        SpaceQualifier currentSpaceQualifier;

        /**
         * Value used to represent the curernt index into the graphics area group array.
         */
        unsigned currentActiveAreaId;

        /**
         * The current cursor Y position.
         */
        double cursorY;

        /**
         * List of active areas.
         */
        QList<EQt::GraphicsItemGroup*> currentGraphicsItemGroups;
};

#endif
