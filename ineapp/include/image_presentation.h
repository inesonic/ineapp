/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImagePresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMAGE_PRESENTATION_H
#define IMAGE_PRESENTATION_H

#include <QObject>
#include <QByteArray>
#include <QSizeF>
#include <QTransform>

#include <ld_format_structures.h>
#include <ld_text_element.h>
#include <ld_element_cursor.h>
#include <ld_visual.h>
#include <ld_image_visual.h>
#include <ld_image_format.h>

#include "app_common.h"
#include "scene_units.h"
#include "leaf_presentation.h"

namespace EQt {
    class GraphicsItem;
}

class PlacementTracker;

/**
 * This class provides a presentation for the Ld::ImageElement class.
 */
class APP_PUBLIC_API ImagePresentation:public LeafPresentation, public virtual Ld::ImageVisual {
    Q_OBJECT

    public:
        ImagePresentation();

        ~ImagePresentation() override;

        /**
         * Static method that creates an instance of a \ref ImagePresentation class.
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
        ReflowHint reflowHint() const override;

        /**
         * Method that returns the unscaled image resolution in dots per point.
         *
         * \return Returns the unscaled size of the image in dots per point.  A null scaling factor will be returned
         *         if the scaling could not be determined.
         */
        QSizeF prescaledDotsPerPoint() const;

        /**
         * Method that returns the unscaled image size in points.  Note that, if needed, this method will load the
         * image.
         *
         * \return Returns the unscaled size of the image in points.  An image DPI of 96 dots/inch is assumed if the
         *         image DPI could not be determined.  A null size (0,0) will be returned if an error occurred.
         */
        QSizeF prescaledImageSizePoints();

        /**
         * Method that returns the current image size in points.  Note that, if needed, this method will load the
         * image.
         *
         * \return Returns the unscaled size of the image in points.  An image DPI of 96 dots/inch is assumed if the
         *         image DPI could not be determined.  A null size (0,0) will be returned if an error occurred.
         */
        QSizeF displayedImageSizePoints();

    signals:
        /**
         * Signal that is emitted when the payload tied to this visual changes.  This signal exists primarily to allow
         * messages from the element to be processed cleanly across thread boundaries.
         */
        void payloadWasChanged();

        /**
         * Signal that is emitted when the payload tied to this visual failed to load.  This signal exists primarily to
         * allow messages from the element to be processed cleanly across thread boundaries.
         */
        void payloadFailedToLoad();

    protected slots:
        /**
         * Slot that is triggered when a Ld::Element reports that its format has been changed.  You can use this slot
         * to perform updates to the visual representation of the element.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                      oldFormat pointer if the Ld::Format instance was not changed.
         */
        void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) final;

        /**
         * Slot that is triggered when an aspect of the associated Ld::Element has been changed.  This slot exists to
         * cover such things as text updates that are not covered by the addition or removal of children, nor format
         * updates.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread boundaries.
         *
         * The default version of this slot simply returns.
         */
        void processDataChange() final;

        /**
         * Slot that is triggered when the payload tied to this visual changes.
         */
        virtual void processPayloadChange();

        /**
         * Slot that is triggered when a load failure occurs with a payload.
         */
        virtual void reportPayloadCouldNotBeLoaded();

    private:
        /**
         * Static value used to indicate the size of an error pixmap, in points.
         */
        static const QSizeF errorImageSizePoints;

        /**
         * Static value used to represent the default image dots per point value.
         */
        static const QSizeF defaultDotsPerPoint;

        /**
         * Method that is called when the payload tied to this visual changes.  The default implementation simply
         * returns.
         */
        void payloadChanged() final;

        /**
         * Method that is called when a load failure occurs with a payload.
         */
        void payloadCouldNotBeLoaded() final;

        /**
         * Method that updates the image based on data contained in the element.  This method updates the
         * ImagePresentation::currentGraphicsItem and ImagePresentation::currentTransform instance variables.
         *
         * \param[in] imageData             The image data to be rendered.  An empty byte array will cause a default
         *                                  image to be included.
         *
         * \param[in] horizontalAxisScaling The horizontal axis scaling.
         *
         * \param[in] verticalAxisScaling   The vertical axis scaling.
         *
         * \param[in] rotation              The rotation value for the image.
         *
         * \return Returns true if the provided image data was valid.  Returns false if the provided image data was
         *         invalid.
         */
        bool createGraphicsItem(
            const QByteArray&            imageData,
            const Ld::ImageFormat::Axis& horizontalAxisScaling,
            const Ld::ImageFormat::Axis& verticalAxisScaling,
            Ld::ImageFormat::Rotation    rotation
        );

        /**
         * Method that calculates the scale factor for a dimension.
         *
         * \param[in] axisScaling           The axis scaling data.
         *
         * \param[in] sizePoints            The default image size of this axis, in points, based on the screen
         *                                  resolution and image pixel size only.
         *
         * \param[in] prescaledDotsPerPoint The dots per point the image is reporting it wants, prior to scaling.
         *
         * \param[in] otherScaleFactor      The scale factor applied to the other axis.
         *
         * \return Returns a scale factor for the image to be applied to a transformation matrix.
         */
        float calculateScaleFactor(
            const Ld::ImageFormat::Axis& axisScaling,
            float                        sizePoints,
            float                        prescaledDotsPerPoint,
            float                        otherScaleFactor = 0.0F
        );

        /**
         * Method that loads and properly adjusts the image based on the current format.
         *
         * \return Returns true on success, returns false on error.
         */
        bool loadAndAdjustImage();

        /**
         * Method that can be called to clear the list of simple graphics items.
         */
        void clearGraphicsItem();

        /**
         * The graphics item being used to render the image.
         */
        EQt::GraphicsItem* currentGraphicsItem;

        /**
         * Transformation to be applied to the image.
         */
        QTransform currentTransform;

        /**
         * The current transformed image size.
         */
        QSizeF currentTransformedSize;

        /**
         * The current unscaled image resolution in dots per point.
         */
        QSizeF currentPrescaledResolution;

        /**
         * The current unscaled image size in pixels.
         */
        QSize currentPrescaledSizePixels;

        /**
         * A flag that is used to determine if we have a valid image.
         */
        bool currentImageValid;
};

#endif
