/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
*************************************f*******************************************************************************//**
* \file
*
* This header defines the \ref ValueFieldPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VALUE_FIELD_PRESENTATION_H
#define VALUE_FIELD_PRESENTATION_H

#include <QObject>
#include <QSizeF>
#include <QString>

#include <model_variant.h>

#include <ld_format_structures.h>
#include <ld_element_cursor.h>
#include <ld_diagnostic_structures.h>
#include <ld_data_type.h>
#include <ld_diagnostic.h>
#include <ld_visual.h>
#include <ld_value_field_visual.h>
#include <ld_value_field_format.h>

#include "app_common.h"
#include "scene_units.h"
#include "data_type_presentation_generator.h"
#include "data_type_presenter.h"
#include "leaf_presentation.h"

class QSvgRenderer;
class QPen;

namespace EQt {
    class GraphicsMultiTextGroup;
}

class PlacementTracker;

/**
 * This class provides a presentation for the Ld::ValueFieldElement class.
 */
class APP_PUBLIC_API ValueFieldPresentation:public         LeafPresentation,
                                            public virtual Ld::ValueFieldVisual,
                                            public         DataTypePresenter {
    Q_OBJECT

    public:
        ValueFieldPresentation();

        ~ValueFieldPresentation() override;

        /**
         * Static method that creates an instance of a \ref ValueFieldPresentation class.
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

        /**
         * Method that indicates if this presentation support reflow or splitting.  The method can be used by parent
         * presentations during repositioning to short circuit the repositioning process.  This method will generally
         * be overloaded in paragraphs and similar frames that can dynamically reorganize or resize content.
         *
         * \return Returns the current reflow hint for this presentation.  The return value will vary based on the
         *         text content.
         */
        ReflowHint reflowHint() const final;

        /**
         * Method called by the DataTypePresentationGenerator to request space.  This method should marshall the
         * request to the parent of the presentation operating with the generator.
         *
         * \param[out] spaceQualifier The qualifier defining the constraints on the provided space.
         *
         * \return Returns the width and height of the next available area on the current line.  Values represent the
         *         maximum available space in each dimension.
         */
        QSizeF requestDataTypeArea(SpaceQualifier* spaceQualifier = Q_NULLPTR) override;

        /**
         * Method called by the DataTypePresentationGenerator to allocate a subset of the requested space.  The method
         * should marshall the requests to the parent of the presentation operating with the generator.
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
        void allocateDataTypeArea(
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0
        ) override;

        /**
         * Method called by the DataTypePresentationGenerator to report that the provided space is insufficient.  the
         * method should marshall the request to the parent of the presentation operating with the generator.
         *
         * \param[in] size The total provided space.  This value should equal the space reported by
         *                 DataTypePresenter::requestDataTypeArea.
         */
        void dataTypeAreaInsufficient(const QSizeF& size) override;

        /**
         * Method that can be called to determine the maximum space that can ever be made available for a data type
         * presentation.
         *
         * \return Returns the maximum space that can be made available for a data type presentation.
         */
        QSizeF largestAvailableAreaSceneUnits() const override;

    protected slots:
        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread boundaries.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void processDiagnostic(Ld::DiagnosticPointer diagnostic) override;

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.  The
         * default implementation will remove any red borders drawn around the graphics items tied to this presentation.
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

    private:
        /**
         * Method you can call to destroy all presentation areas.
         */
        void destroyPresentationAreas();

        /**
         * Method that updates diagnostic information.
         *
         * \param[in] borderPen The border pen to be used.
         *
         * \param[in] toolTip   The tool tip to apply.
         */
        void updateDiagnosticInformation(const QPen& borderPen, const QString& toolTip);

        /**
         * Method that is called to build the default graphics item.
         *
         * \param[in]  currentPresentationAreas The current graphics items.
         *
         * \param[in]  parent                   Pointer to the parent placement negotiator.
         *
         * \param[in]  childIdentifier          The child identifier the parent uses to identify this presentation.
         *
         * \param[in]  format                   The format to apply to the default graphics item.
         *
         * \param[in] relativeScale             A hint value used to indicate if this child should try to scale smaller
         *                                      than usual. This value is intended for cases where an equation element
         *                                      needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                      indicates normal scale.  The value is intended as a hint, children are
         *                                      not required to honor this parameter.
         *
         * \return Returns an updated list containing the newly created presentation areas.
         */
        PresentationAreas buildDefaultGraphicsItem(
            PresentationAreas&   currentPresentationAreas,
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            Ld::FormatPointer    format,
            float                relativeScale
        );

        /**
         * The child identifier being used to identify this presentation to the parent.
         */
        unsigned long currentChildIdentifier;

        /**
         * The curernt list of tracked presentation areas.
         */
        PresentationAreas currentPresentationAreas;

        /**
         * Value holding the last reported data-type.
         */
        Ld::DataType lastReportedDataType;

        /**
         * Flag indicating that full repositioning is needed for this field.
         */
        ReflowHint currentReflowHint;
};

#endif
