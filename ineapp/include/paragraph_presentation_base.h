/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ParagraphPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PARAGRAPH_PRESENTATION_BASE_H
#define PARAGRAPH_PRESENTATION_BASE_H

#include <QObject>
#include <QSizeF>
#include <QRectF>
#include <QList>

#include <ld_paragraph_format.h>

#include "app_common.h"
#include "placement_line_data.h"
#include "presentation_with_positional_children.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsItemGroup;
    class GraphicsTextItem;
}

class PlacementNegotiator;
class PlacementTracker;
class Presentation;
class PresentationArea;

/**
 * This class provides a base class for frames that can hold other elements.  This frame supports line wrapping and is
 * thus more computationally complex than \ref FramePresentationBase.
 */
class APP_PUBLIC_API ParagraphPresentationBase:public PresentationWithPositionalChildren {
    Q_OBJECT

    public:
        /**
         * Justification to apply to each line.
         */
        typedef Ld::ParagraphFormat::Justification Justification;

        ParagraphPresentationBase();

        ~ParagraphPresentationBase() override;

        /**
         * Method that is called by a child to the parent to request that the parent reposition the contents of the
         * child.
         *
         * \param[in] childPresentation The presentation that needs to be repositioned.
         */
        void requestRepositioning(Presentation* childPresentation) final;

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
        ) final;

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
        ) final;

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
         * This version exists to address issues with virtual inheritance and simply invokes the correct base class
         * method.
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
         * Method called by a child to indicate that the provided space by PlacementNegotiator::requestArea is
         * insufficient.
         *
         * This version exists to address issues with virtual inheritance and simply invokes the correct base class
         * method.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] size               The last reported available space.  This value must be equal to the space
         *                               reported by PlacementNegotiator:requestArea.
         */
        void areaInsufficient(unsigned long childIdentifier, const QSizeF& size) final;

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
        void applyStretch(unsigned long presentationAreaId, float stretchFactor) final;

        /**
         * Method called by a parent to obtain a specified child graphics item, by presentation area ID.
         *
         * \param[in] presentationAreaId A zero based presentation area ID indicating which graphics region is of
         *                               interest.
         *
         * \return Returns a pointer to a QGraphicsItem used to represent this visual.  A null pointer is returned if
         *         the presentationAreaId is invalid.
         */
        QGraphicsItem* graphicsItem(unsigned long presentationAreaId) const final;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.
         */
        void resetPlacement() override;

        /**
         * Method that is called when this placement negotiator needs to remove itself from the scene.  You should
         * overload this method in derived classes with code that removes and optionally destroys placement areas from
         * the parent.
         */
        void removeFromScene() override;

        /**
         * Method you can use to determine the distance between a given location and the closest graphics item managed
         * by this class.  This method assumes that graphics items follow in numerical order.
         *
         * \param[in]  location           The location to query, in scene units.
         *
         * \param[out] bestPresentation   An optional value that can be populated with the best presentation to
         *                                select for the location.
         *
         * \param[out] presentationAreaId An optional value that can be populated with the presentation area index of
         *                                the closest presentation area.
         *
         * \param[out] closestPoint       An optional value that can be populated with the closest point on the
         *                                closest presentation area.
         *
         * \return Returns the distance in scene units.
         */
        double distanceToClosestPresentationArea(
            const QPointF&       location,
            const Presentation** bestPresentation = Q_NULLPTR,
            unsigned long*       presentationAreaId = Q_NULLPTR,
            QPointF*             closestPoint = Q_NULLPTR
        ) const override;

        /**
         * Method that is called by the cursor logic to determine if this element should be highlighted during
         * selection.
         *
         * \return Returns true if the entire element can be highlighted.  Returns false if the element should not be
         *         highlighted by the cursor.
         */
        bool cursorCanHighlight() const override;

        /**
         * Method that indicates if there there are pending updates at or under this presentation.  You can use this
         * method to optimize rendering by identifying this presentation or a child has requested an update to its
         * placement.
         *
         * \return Returns true if there are pending updates.  Returns false if there are no pending updates.
         */
        bool pendingRepositioning() const override;

        /**
         * Method you can call to indicate this presentation needs to be repositioned.
         */
        void flagPendingRepositioning() override;

        /**
         * Method that indicates if this presentation support reflow or splitting.  The method can be used by parent
         * presentations during repositioning to short circuit the repositioning process.  This method will generally
         * be overloaded in paragraphs and similar frames that can dynamically reorganize or resize content.
         *
         * \return Returns the current reflow hint for this presentation.  The return value will depend on values
         *         returned by each child presentation.
         */
        ReflowHint reflowHint() const override;

    protected:
        /**
         * Method that is called to determine the justification to apply to lines.
         *
         * \return Returns the line justification to apply.  The default implementation will left justify lines.
         */
        virtual Justification lineJustification();

        /**
         * Method that is called to determine the left margin to apply to all lines except the first line.  Bullets
         * and list numbers will be aligned to this margin.
         *
         * \return Returns the left margin to apply to all lines, except the first line, in scene units.  The default
         *         implementation returns 0.
         */
        virtual float leftMarginSceneUnits();

        /**
         * Method that is called to determine the left margin to apply to all lines except the first line.  The value
         * is added to the left margin and list indentation to determine the indentation for the first line.
         *
         * \return Returns the left margin to apply to all lines, except the first line, in scene units.  The default
         *         implementation returns 0.
         */
        virtual float firstLineLeftMarginSceneUnits();

        /**
         * Method that is called to determine the right margin to apply to all lines except the first line.
         *
         * \return Returns the right margin to apply to all lines, except the first line, in scene units.  The default
         *         implementation returns 0.
         */
        virtual float rightMarginSceneUnits();

        /**
         * Method that is called to determine additional spacing to be placed at the top of the first active area.
         *
         * \return Returns spacing, in scene units, to be applied to the top of the first active area.  The default
         *         implementation returns 0.
         */
        virtual float topSpacingSceneUnits();

        /**
         * Method that is called to determine the list indentation.  The value is added to the left margin to determine
         * the left edge of text in the paragraph.  The first line left margin is also added to the left margin and
         * this value to determine the left edge of the text on the first line.
         *
         * \return Returns the list indentation in scene units.
         */
        virtual float listIndentationSceneUnits();

        /**
         * Method that is called to determine the bullet or number text to place to the left of the list indentation.
         *
         * This method is non-constant to allow the method to adjust local parameters, if needed.
         *
         * \param[out] font                The font to be used to render the list string.  The default implementation
         *                                 will populate the font, if non-null, with the application default font.
         *
         * \param[out] fontColor           The color to apply to the rendered text.  The default implementation will
         *                                 populate the color, if non-null, with an invalid color.
         *
         * \param[out] fontBackgroundColor The color to apply to the background of the rendered text.  The default
         *                                 implementation will populate the color, if non-null, with an invalid color.
         *
         * \return Returns the text to present.  The default implementation will return an empty string.
         */
        virtual QString listString(
            QFont*  font = Q_NULLPTR,
            QColor* fontColor = Q_NULLPTR,
            QColor* fontBackgroundColor = Q_NULLPTR
        );

        /**
         * Method you can use to obtain the current line spacing.
         *
         * \return Returns the current line spacing.  A value of 1.0 indicates single spacing.  A value of 1.5 indicates
         *         1 1/2 line spacing.  A value of 2.0 indicates double spacing.  The default implementation returns
         *         1.0.
         */
        virtual float lineSpacing();

    private slots:
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
        void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation) final;

        /**
         * Slot that is triggered when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void processChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation) final;

        /**
         * Slot that is triggered when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void processChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation) final;

    private:
        /**
         * The maximum allowed stretch factor.
         */
        static constexpr double maximumStretchFactor = 0.33;

        /**
         * Method used internally to clear any line data.
         */
        void clearLineData();

        /**
         * Method used internally to adjust the cursor position.
         *
         * \param[in] size The size used to perform the adjustment.
         *
         * \param[in] usableAdjustment If true, the size represents space actually used.  If false, the space was
         *                             unused.
         */
        void adjustCursor(const QSizeF& size, bool usableAdjustment);

        /**
         * Method that is called to position the presentation areas on the current line.  This method also handles
         * the justification and stretch process.
         */
        void positionPresentationAreas();

        /**
         * Method that provides a rectangle describing the active area.
         *
         * \param[in] areaIdentifier An identifier used to indicate the desired area.  The value can be used as a page
         *                           index, column index, section of a paragraph, etc.
         *
         * \return Returns the rectangle describing the dimensions (and position) of the active area.
         */
        QRectF activeAreaRectangle(unsigned long areaIdentifier);

        /**
         * Method you can overload to receive notification when the frame placement engine is done processing children
         * in a given active area.  This method can be used to report size changes up the tree, if needed.
         *
         * The default implementation simply returns with no operation.
         *
         * \param[in] areaIdentifier  An identifier that indicates the area we are done processing.
         *
         * \param[in] activeRectangle A rectangle representing the active area that was actually used.  This may be
         *                            slightly less than the area provided.
         */
        void activeAreaClosed(unsigned long areaIdentifier, const QRectF& activeRectangle);

        /**
         * Method that is called to truncate the active area data at and after a specified number of active areas.
         *
         * \param[in] newListSize The new size of the active area lists.
         */
        void truncateActiveAreasStartingAt(unsigned long newListSize);

        /**
         * Flag that indicates if repositioning is pending for this presentation.
         */
        bool currentPendingRepositioning;

        /**
         * Flag that indicates if we currently support reflow.
         */
        ReflowHint currentReflowHint;

        /**
         * The current area identifier.
         */
        unsigned long currentAreaIdentifier;

        /**
         * The current line index.
         */
        unsigned long currentLineIndex;

        /**
         * The current area rectangle.
         */
        QRectF currentAreaRectangle;

        /**
         * The current space qualifier.  We adjust this as we issue space to child presentation areas.
         */
        SpaceQualifier currentSpaceQualifier;

        /**
         * The horizontal position on the current line.
         */
        double cursorX;

        /**
         * The vertical position on the current line.
         */
        double cursorY;

        /**
         * Value holding the current minimum top spacing.
         */
        float currentMinimumTopSpacing;

        /**
         * Value holding the expected next minimum top spacing.
         */
        float nextMinimumTopSpacing;

        /**
         * A list holding information on each presentation area on the current line.
         */
        QList<PlacementLineData> currentLinePresentations;

        /**
         * The current maximum height of any presentation area on the current line.
         */
        float currentMaximumHeight;

        /**
         * The current maximum ascent of any presentation area on the current line.
         */
        float currentMaximumAscent;

        /**
         * The current left side list indentation.
         */
        float currentListIndentation;

        /**
         * The current amount of stretchable width.
         */
        double currentTotalStretchableWidth;

        /**
         * Parent negotiator
         */
        PlacementNegotiator* currentParentNegotiator;

        /**
         * An identifier used by the parent to identify this child.
         */
        unsigned long currentChildIdentifier;

        /**
         * List of graphics item groups containing children.
         */
        QList<EQt::GraphicsItemGroup*> activeAreas;

        /**
         * Graphics text item used to display list text.
         */
        EQt::GraphicsTextItem* currentListTextItem;
};

#endif
