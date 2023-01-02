/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref VariablePresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VARIABLE_PRESENTATION_H
#define VARIABLE_PRESENTATION_H

#include <QObject>
#include <QList>
#include <QPair>

#include <ld_variable_element.h>
#include <ld_element_cursor.h>
#include <ld_visual.h>
#include <ld_variable_visual.h>
#include <ld_character_format.h>

#include "app_common.h"
#include "leaf_presentation.h"

class QGraphicsItem;
class QFontMetricsF;

namespace M {
    class Variant;
}

namespace EQt {
    class GraphicsMultiTextGroup;
}

namespace Ld {
    class DataType;
}

class PlacementTracker;

/**
 * This class provides a visual for the Ld::VariableElement class with full support for presenting a
 * Ld::VariableElement within the document view of a program.
 */
class APP_PUBLIC_API VariablePresentation:public LeafPresentation, public virtual Ld::VariableVisual {
    Q_OBJECT

    public:
        VariablePresentation();

        ~VariablePresentation() override;

        /**
         * Static method that creates an instance of a \ref VariablePresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

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
        ) const final;

    signals:
        /**
         * Signal that is emitted when the text for this element is changed.  You can use this signal to process
         * notification that the text element has changed.
         *
         * \param[in] newText     The new text held by the element.
         *
         * \param[in] regionIndex The zero based region index.
         */
        void textHasChanged(QString newText, unsigned regionIndex);

    protected slots:
        /**
         * Slot that is triggered when the text tracked by this element is changed.  You can overload this slot if you
         * wish to receive notification of a text change event.
         *
         * The slot primarily exists to allow call-backs from the Ld::TextVisual to occur across thread boundaries.
         *
         * \param[in] newText     The new text.
         *
         * \param[in] regionIndex The zero based region index.
         */
        virtual void processTextChange(const QString& newText, unsigned regionIndex);

        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread
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
         * Slot that is triggered to indicate that a calculated value associated with the element has been
         * updated.
         *
         * \param[in] valueIndex      The zero based value index that was just updated.
         *
         * \param[in] calculatedValue The calculated value.
         */
        void processCalculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue) override;

        /**
         * Slot that is triggered to indicate that the calculated values have been cleared.
         */
        void processCalculatedValuesCleared() override;

    protected:
        /**
         * Method you can overload to determine the number of displayed text regions.  Text that can not be selected
         * by the cursor should not be included.
         *
         * \return Returns the number of displayed text regions.  The default implementation returns 0.
         */
        unsigned numberDisplayedTextRegions() const override;

        /**
         * Method you can overload to determine the text and font and associated with a given region.  Text that can
         * not be selected by the cursor should not be included.  This version will assert if called.
         *
         * \param[in] regionIndex The zero based displayed text region index.
         *
         * \return Returns a pair holding the text and font at the requested region.
         */
        QPair<QString, QFont> textAtDisplayedRegion(unsigned regionIndex) const override;

        /**
         * Method you can overload to determine the left position of a displayed text region.  This version will assert
         * if called.
         *
         * \param[in] regionIndex The zero based displayed text region index.
         *
         * \return Returns the left position of the text region in absolute scene coordinates.
         */
        QPointF displayedTextPosition(unsigned regionIndex) const override;

        /**
         * Method you can overload to convert a displayed text offset and displayed region index into a cursor
         * position.  The default implementation will assert.
         *
         * \param[in] displayedTextOffset
         *
         * \param[in] displayedRegionIndex
         *
         * \return Returns the cursor location corresponding to the supplied displayed text location.
         */
        Ld::ElementCursor convertToElementCursor(
            unsigned long displayedTextOffset,
            unsigned      displayedRegionIndex
        ) const override;

        /**
         * Method that is used to update the tool tip for this item.  The default implementation will return a string
         * holding the reported diagnostic message.
         *
         * \param[in] diagnostic The diagnostic used to determine the correct tool tip.
         *
         * \return Returns the tool tip to display in the active area.
         */
        QString updateToolTip(Ld::DiagnosticPointer diagnostic = Ld::DiagnosticPointer()) override;

    private:
        /**
         * Method that is called when the text managed by the element is changed.  Also note the
         * Ld::Visual::elementDataChanged method.
         *
         * \param[in] newText     The new text.
         *
         * \param[in] regionIndex The zero based region index.
         */
        void textChanged(const QString& newText, unsigned regionIndex) final;

        /**
         * The required ascent
         */
        float requiredAscent;

        /**
         * The default tool-tip text.
         */
        QString currentDefaultToolTip;

        /**
         * The graphics item.
         */
        EQt::GraphicsMultiTextGroup* currentGraphicsItem;
};

#endif
