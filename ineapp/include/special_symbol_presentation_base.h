/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SpecialSymbolPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SPECIAL_SYMBOL_PRESENTATION_BASE_H
#define SPECIAL_SYMBOL_PRESENTATION_BASE_H

#include <QObject>
#include <QList>
#include <QFont>
#include <QColor>

#include <ld_diagnostic_structures.h>
#include <ld_element_cursor.h>
#include <ld_visual.h>
#include <ld_text_visual.h>
#include <ld_character_format.h>

#include "app_common.h"
#include "leaf_presentation.h"

class QGraphicsItem;
class QFontMetricsF;

namespace EQt {
    class GraphicsMultiTextGroup;
}

class PlacementTracker;

/**
 * You can use this class as a common base class for static mathematical symbols containing no children.
 */
class APP_PUBLIC_API SpecialSymbolPresentationBase:public LeafPresentation, public virtual Ld::Visual {
    Q_OBJECT

    public:
        SpecialSymbolPresentationBase();

        ~SpecialSymbolPresentationBase() override;

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
        ) const override;

    protected slots:
        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread boundaries.
         *
         * This version will cast the graphics item back to a EQt::GraphicsTextItem and set the border pen color to
         * yellow or red based on the diagnostic type.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void processDiagnostic(Ld::DiagnosticPointer diagnostic) override;

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.  The
         * default implementation will remove any red borders drawn around the graphics items tied to this presentation.
         *
         * This version will cast the graphics item back to a EQt::GraphicsTextItem and set no border pen.
         */
        void clearDiagnosticDisplay() override;

    protected:
        /**
         * Method you can overload to obtain a string representation of the desired mathematical symbol.
         *
         * \return Returns the mathematical symbol as a string.  The default implementation returns an inverted question
         *         mark.
         */
        virtual QString specialCharacters() const;

        /**
         * Method you can overload to obtain the font used to render the special characters.  This method is called by
         * \ref SpecialSymbolPresentationBase::buildGraphicsItem
         *
         * \return Returns the font used to render the mathematical symbols.  The default implementation obtains the
         *         font from the element format.  If the font could not be determined, the method returns the
         *         application global math font.
         */
        virtual QFont specialCharactersFont() const;

        /**
         * Method you can overload to obtain the color to apply to the special characters.
         *
         * \return Returns the text color.  An invalid color will cause the text to be rendered as black.  The default
         *         implementation return the color reported by the underlying element format.  If the font color
         *         could not be determined, the method returns an invalid color.
         */
        virtual QColor specialCharactersColor() const;

        /**
         * Method you can overload to obtain the background color to apply to the special characters.
         *
         * \return Returns the text color.  An invalid color will cause the text to be rendered as black.  The default
         *         implementation return the background color reported by the underlying element format.  If the font
         *         background color could not be determined, the method returns an invalid color.
         */
        virtual QColor specialCharactersBackgroundColor() const;

        /**
         * Method you can overload to build the graphics item.  The default implementation will return a
         * EQt::TextGraphicsItem using the supplied text, font and colors supplied by the methods:
         *
         *     * \ref SpecialSymbolPresentationBase::specialCharacters
         *     * \ref SpecialSymbolPresentationBase::specialCharactersFont
         *     * \ref SpecialSymbolPresentationBase::specialCharactersColor
         *     * \ref SpecialSymbolPresentationBase::specialCharactersBackgroundColor
         *
         * \param[in]  relativeSize The relative size to apply to this graphics item.
         *
         * \param[out] areaSize The area size to use for space calculations.
         *
         * \param[out] ascent   The ascent to report for the graphics item.
         *
         * \return Returns the graphics item tied to the symbol.
         */
        virtual EQt::GraphicsMultiTextGroup* buildGraphicsItem(float relativeSize, QSizeF& areaSize, float& ascent);

    private:
        /**
         * The graphics item used to present this object.
         */
        EQt::GraphicsMultiTextGroup* symbolGraphicsItem;

        /**
         * Value holding the last reported graphics item size.
         */
        QSizeF symbolGraphicsItemSize;

        /**
         * Value holding the last reported graphics item ascent.
         */
        float symbolAscent;

        /**
         * Value holding the last reported relative scale.
         */
        float lastRelativeScale;
};

#endif
