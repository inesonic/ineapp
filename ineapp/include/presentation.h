/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Presentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QRectF>
#include <QFont>
#include <QPair>

#include <eqt_graphics_math_group.h>

#include <model_variant.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_diagnostic.h>
#include <ld_element_cursor.h>
#include <ld_visual.h>

#include "app_common.h"
#include "scene_units.h"
#include "placement_negotiator.h"

namespace Ld {
    class Format;
    class DataType;
    class CalculatedValue;
};

/**
 * This is a pure virtual base class for classes used to present and manipulate language elements.  The class extends
 * the Ld::Visual class to bring visuals into the Qt QObject and signal/slot framework and to include additional
 * methods used to support positioning and rendering of components.
 */
class APP_PUBLIC_API Presentation:public         QObject,
                                  public virtual Ld::Visual,
                                  public         PlacementNegotiator,
                                  protected      SceneUnits {
    Q_OBJECT

    public:
        /**
         * Value indicating how we should process signals from callbacks to slots used to process those signals.  The
         * value exists to provide consistent handling of ordering of signals.
         */
        static constexpr Qt::ConnectionType connectionType = Qt::DirectConnection;

        /**
         * Value indicating the Z height depth to assign to the page background.  Items with a lower Z height depth
         * will be rendered behind the page and tfhis ignored.
         */
        static constexpr float pageZHeight = -1000.0F;

        /**
         * Value indicating the minimum depth of presentation embellishments.
         */
        static constexpr float minimumEmbellishmentsZHeight = -999.0F;

        /**
         * Value indicating the maximum depth of presentation embellishments.
         */
        static constexpr float maximumEmbellishmentsZHeight = -1.0F;

        /**
         * Value indicating the minimum depth of page contents.
         */
        static constexpr float minimumContentsZHeight = 0.0F;

        /**
         * Value indicating the Z-height to use for large operator overlays.
         */
        static constexpr float minimumLargeOperatorOverlayZHeight = +10.0F;

        /**
         * Value that indicates the baseline for subscripted or inferior text.
         */
        static constexpr float subscriptBaseline = -0.14F;

        /**
         * Value that indicates the character size reduction for subscripted or inferior text.
         */
        static constexpr float subscriptSizeAdjustment = 0.65F;

        /**
         * Value that indicates the weighting adjustment for subscripted or inferior text.  A positive value
         * indicates a weight increase by this amount towards 1.  A negative value indicates a weight decrease by this
         * amount away from 1.
         */
        static constexpr float subscriptWeightAdjustment = +0.10F;

        /**
         * Value that indicates the baseline for superscripted or superior text.
         */
        static constexpr float superscriptBaseline = +0.25F;

        /**
         * Value that indicates the character size reduction for superscripted or superior text.
         */
        static constexpr float superscriptSizeAdjustment = 0.65F;

        /**
         * Value that indicates the weighting adjustment for superscripted or superior text.  A positive value
         * indicates a weight increase by this amount towards 1.  A negative value indicates a weight decrease by this
         * amount away from 1.
         */
        static constexpr float superscriptWeightAdjustment = +0.10F;

        /**
         * Scale factor aplied to parenthesis.  Value must be greater than or equal to 1.0.
         */
        static constexpr float parenthesisScaleFactor = 1.2F;

        /**
         * Value used as the margin to allow for parenthesis height.
         */
        static constexpr float parenthesisHeightMargin = 0.5F;

        /**
         * Value that represent the color to apply to diagnostic borders when providing information.
         */
        static const QColor informationBorderColor;

        /**
         * Value that represents the color to apply to diagnostic borders when a warning is detected.
         */
        static const QColor warningBorderColor;

        /**
         * Value that represents the color to apply to diagnostic borders when an error is detected.
         */
        static const QColor errorBorderColor;

        /**
         * Value that represents the color to apply to borders when a breakpoint is set.
         */
        static const QColor breakpointBorderColor;

        /**
         * Value that represents the color to apply to borders when the program counter is paused on the element.
         */
        static const QColor pausedAtBorderColor;

        /**
         * Value that represents the pen width to use for warning and error borders.
         */
        static const float diagnosticErrorBorderWidth;

        /**
         * Value that represents the maximum space that can every be allocated.
         */
        static const QSizeF maximumPossibleSpace;

        /**
         * Value that indicates the default exported image format.
         */
        static const QString defaultExportImageFormat;

        /**
         * Enumeration of supported reflow modes.  The value is used to determine how to handle the parent presentation
         * should handle rendering based on the available space requirements.
         */
        enum class ReflowHint {
            /**
             * Indicates this presentation does not support reflow.  Size if fixed.
             */
            REFLOW_NOT_SUPPORTED = 0,

            /**
             * Indicates this presentation supports reflow.  Reflow should be performed based on the space relative
             * to the end of the maximum available area.
             */
            SUPPORTS_REFLOW = 1,

            /**
             * Indicates this presentation should always be reflowed as the size can increase or decrease based on the
             * available area.
             */
            ALWAYS_REFLOW = 2
        };

        Presentation();

        ~Presentation() override;

        /**
         * Method that indicates if there there are pending updates at or under this presentation.  You can use this
         * method to optimize rendering by identifying this presentation or a child has requested an update to its
         * placement.
         *
         * Traditionally, this method should return true if a child or this presentation requests repositioning or if
         * the parent calls \ref Presentation::flagPendingRepositioning.
         *
         * \return Returns true if there are pending updates.  Returns false if there are no pending updates.  The
         *         default implementation returns true to indicate that this presentation always needs to be updated.
         */
        virtual bool pendingRepositioning() const;

        /**
         * Method you can call to indicate this presentation needs to be repositioned.  The default implementation
         * does nothing.
         */
        virtual void flagPendingRepositioning();

        /**
         * Method that indicates if this presentation support reflow or splitting.  The method can be used by parent
         * presentations during repositioning to short circuit the repositioning process.  This method will generally
         * be overloaded in paragraphs and similar frames that can dynamically reorganize or resize content.
         *
         * \return Returns the current reflow hint for this presentation.  The default implementation always returns
         *         \ref Presentation::ReflowHint::REFLOW_NOT_SUPPORTED.
         */
        virtual ReflowHint reflowHint() const;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.
         *
         * This version will call the \ref PlacementNegotiator::resetPlacement method for every child.
         */
        void resetPlacement() override;

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
         * \param[out] closestPoint       An optional value that can be populated with the closest point on the closest
         *                                presentation area.
         *
         * \return Returns the distance in scene units.
         */
        virtual double distanceToClosestPresentationArea(
            const QPointF&       location,
            const Presentation** bestPresentation = Q_NULLPTR,
            unsigned long*       presentationAreaId = Q_NULLPTR,
            QPointF*             closestPoint = Q_NULLPTR
        ) const;

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
        virtual QList<QRectF> cursorRangeToScene(
            const Ld::ElementCursor& endingElementCursor,
            const Ld::ElementCursor& startingElementCursor = Ld::ElementCursor()
        ) const;

        /**
         * Method that determines if a location is contained within this presentation or in a child under this
         * presentation.
         *
         * \param[in] location    The location, in scene units.
         *
         * \param[in] findClosest If true, the closest presentation will be returned even if the location is not
         *                        contained within the presentation.  Note that presentation can chose to honor or
         *                        ignore this parameter.
         *
         * \return Returns the cursor at a given location if that location is contained under this presentation.  An
         *         invalid cursor is returned if the location is not tied to this presentation or a child of this
         *         presentation.  The default implementation simply queries every child.
         */
        virtual Ld::ElementCursor presentationAtLocation(const QPointF& location, bool findClosest) const;

        /**
         * Method that is called by the cursor logic to determine if this element should be highlighted during
         * selection.
         *
         * \return Returns true if the entire element can be highlighted.  Returns false if the element should not be
         *         highlighted by the cursor.  The default implementation returns true.
         */
        virtual bool cursorCanHighlight() const;

        /**
         * Method that calculates the image bounding rectangle.  The default version determines the bounding rectangle
         * from the graphics items.
         *
         * \return Returns a bounding rectangle for the image, in scene units.
         */
        virtual QRectF imageBoundingRectangle() const;

    signals:
        /**
         * Signal that is emitted when this object is newly coupled to an element.  You can use this signal to trigger
         * updates to the presentation based on the element contents and type.
         *
         * \param[out] element The element that this visual is now tied to.
         */
        void nowTiedToElement(Ld::ElementPointer element);

        /**
         * Signal that is emitted when this object is disconnected from an element.  You can use this signal to perform
         * a controlled clean-up of the presentation.
         */
        void nowUntiedFromElement();

        /**
         * Signal that is emitted when a Ld::Element reports that its format has been changed.  You can use this signal
         * to perform updates to the visual representation of the element.
         *
         * The signal will automatically be bound to the \ref processFormatChange slot.
         *
         * \param[out] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                       Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[out] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                       oldFormat pointer if the Ld::Format instance was not changed.
         */
        void formatWasChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat);

        /**
         * Signal that is emitted when an Ld::Element reports that a descendant's format has been changed.  You can use
         * this signal to perform updates to the visual representation of the element.
         *
         * \param[out] element   The element who's format has been changed.
         *
         * \param[out] newFormat The new format associated with this element.
         */
        void descendantFormatWasChanged(Ld::ElementPointer element, Ld::FormatPointer newFormat);

        /**
         * Signal that is emitted when an Ld::Element reports that a descendant's format has been updated.  You can use
         * this signal to perform updates to the visual representation of the element.
         *
         * \param[out] element   The element who's format has been changed.
         *
         * \param[out] newFormat The updated format associated with this element.
         */
        void descendantFormatWasUpdated(Ld::ElementPointer element, Ld::FormatPointer newFormat);

        /**
         * Signal that is emitted when the Ld::Element reports that underlying data has been updated.  You can use this
         * signal to perform updates to the visual representation of the element on data changes.
         *
         * The signal will automatically be bound to the \ref processDataChange slot.
         *
         * The default version of this slot simply returns.
         */
        void elementDataWasChanged();

        /**
         * Signal that is emitted when a diagnostic is reported for the associated Ld::Element during a code generation
         * process.
         *
         * The signal will automatically be bound to the \ref processDiagnostic slot.
         *
         * \param[out] diagnostic A code indicating the type of error.
         */
        void diagnosticWasReported(Ld::DiagnosticPointer diagnostic);

        /**
         * Signal that is emitted when a diagnostic tied to the assoicated Ld::Element has been cleared.
         *
         * The signal will automatically be bound to the \ref Presentation::clearDiagnosticDisplay slot.
         */
        void diagnosticWasCleared();

        /**
         * Signal that is triggered when the element has been newly graphed to an element tree.
         */
        void nowGraftedToTree();

        /**
         * Signal that is triggered when the element has been ungraphed from an element tree.
         */
        void nowUngraftedFromTree();

        /**
         * Signal that is triggered when an instruction breakpoint has been set or cleared on this element.
         *
         * \param[out] breakpointNowSet If true, a breakpoint on this element has been set.  If false, a breakpoint
         *                             on this element has been cleared.
         */
        void instructionBreakpointWasUpdated(bool breakpointNowSet);

        /**
         * Signal that is triggered to indicate that a calculated value associated with the element has been updated.
         *
         * \param[out] valueIndex      The index of the value that was updated.
         *
         * \param[out] calculatedValue The calculated value.
         */
        void calculatedValueWasUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue);

        /**
         * Signal that is triggered to indicate that the calculated values associated with the element should be
         * cleared.
         */
        void calculatedValueWasCleared();

    protected slots:
        /**
         * Slot that is triggered when the default tool-tip text for the presentation should be updated.  Note that
         * presentations can choose to ignore this text or display different text.  The default implementation does
         * nothing.
         *
         * \param[in] newToolTipText The new text to present for the tool-tip.  An empty string will clear the default
         *                           tool-tip.
         */
        virtual void setToolTip(const QString& newToolTipText);

        /**
         * Slot that is triggered to clear the default tool-tip text for the presentation.  The default implementation
         * calls \ref Presentation::setToolTip with an empty string.
         */
        virtual void clearToolTip();

        /**
         * Slot that is triggered when this object is newly coupled to an element.  You can use this slot to trigger
         * updates to the presentation based on the element contents and type.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] element The element that this visual is now tied to.
         */
        virtual void tiedToElement(Ld::ElementPointer element);

        /**
         * Slot that is triggered when this object is decoupled from an element.  The default implementation will cause
         * this object to be destroyed.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries and to
         * provides a controlled clean-up of this presentation class.
         *
         * The default implementation calls QObject::deleteLater().
         */
        virtual void untiedFromElement();

        /**
         * Slot that is triggered when a Ld::Element reports that its format has been changed.  You can use this slot
         * to perform updates to the visual representation of the element.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default version of this slot will trigger the presentation to be repositioned and redrawn.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                      oldFormat pointer if the Ld::Format instance was not changed.
         */
        virtual void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat);

        /**
         * Slot that is triggered when a Ld::Element reports that a descendant's format has been changed.  The default
         * implementation performs no action.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] element   The element who's format has been changed.
         *
         * \param[in] newFormat The new format associated with this element.
         */
        virtual void processDescendantFormatChange(Ld::ElementPointer element, Ld::FormatPointer newFormat);

        /**
         * Slot that is triggered when a Ld::Element reports that a descendant's format has been updated.  The defualt
         * implementation performs no action.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] element   The element who's format has been changed.
         *
         * \param[in] newFormat The updated format associated with this element.
         */
        virtual void processDescendantFormatUpdate(Ld::ElementPointer element, Ld::FormatPointer newFormat);

        /**
         * Slot that is triggered when an aspect of the associated Ld::Element has been changed.  This slot exists to
         * cover such things as text updates that are not covered by the addition or removal of children, nor format
         * updates.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread
         * boundaries.
         *
         * The default version of this slot simply returns.
         */
        virtual void processDataChange();

        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread boundaries.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        virtual void processDiagnostic(Ld::DiagnosticPointer diagnostic);

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.  The
         * default implementation will remove any red borders drawn around the graphics items tied to this presentation.
         */
        virtual void clearDiagnosticDisplay();

        /**
         * Slot that is triggered when the element tied to this presentation is newly graphed to a tree.  Be sure to
         * call the base class method if you overload this slot.
         *
         * The default implementation of this slot simply returns.
         */
        virtual void processGraftedToTree();

        /**
         * Slot that is triggered when the element has been ungraphed from an element tree.
         *
         * The default implementation calls \ref PlacementNegotiator::removeFromScene.  If you overload this slot be
         * sure to call the base class inplementation.
         */
        virtual void ungraftedFromTree();

        /**
         * Slot that is triggered when and instruction breakpoint has been set or cleared on this element.
         *
         * \param[in] breakpointNowSet If true, a breakpoint on this element has been set.  If false, a breakpoint
         *                             on this element has been cleared.
         */
        virtual void processInstructionBreakpointUpdated(bool breakpointNowSet);

        /**
         * Slot that is triggered to indicate that a calculated value associated with the element has been
         * updated.
         *
         * The default implementation performs no function.
         *
         * \param[in] valueIndex      The zero based value index that was just updated.
         *
         * \param[in] calculatedValue The calculated value.
         */
        virtual void processCalculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue);

        /**
         * Slot that is triggered to indicate that the calculated values have been cleared.  The default implementation
         * performs no function.
         */
        virtual void processCalculatedValuesCleared();

    protected:
        /**
         * Class used to store information about a set of parenthesis.  You can use this as a QSizeF instance holding
         * the recommended width and height for the parenthesis.
         */
        class ParenthesisData:public QSizeF {
            public:
                ParenthesisData();

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to be copied.
                 */
                ParenthesisData(const ParenthesisData& other);

                ~ParenthesisData();

                /**
                 * Method you can use to specify the relative position to apply for the parenthesis.  This value
                 * represents the Y position used to display the parenthesis relative to the contents if encapsulates.
                 *
                 * If the parenthesis is rendered as text, this value represents the baseline.  If the parenthesis is
                 * drawn, this value represents the top edge.
                 *
                 * \param[in] newRelativePosition The new relative position.  In scene units
                 */
                void setRelativePosition(float newRelativePosition);

                /**
                 * Method you can use to obtain the relative position calculated for this parenthesis.  This value
                 * represents the Y position used to display the parenthesis relative to the contents if encapsulates.
                 *
                 * If the parenthesis is rendered as text, this value represents the baseline.  If the parenthesis is
                 * drawn, this value represents the top edge.
                 *
                 * \return Returns the relative top position calculated for the parenthesis.
                 */
                float relativePosition() const;

                /**
                 * Method you can use to specify if the parenthesis should be rendered using fonts or drawn.
                 *
                 * \param[in] nowFontRendering If true, the parenthesis will be rendered using fonts.  If false, the
                 *                             parenthesis will be drawn.
                 */
                void setFontRendering(bool nowFontRendering = true);

                /**
                 * Method you can use to specify if the parenthesis should be drawn or rendered using fonts.
                 *
                 * \param[in] nowDrawn If true, the parenthesis will be drawn.  If false, the parenthesis will be
                 *                     rendered using fonts.
                 */
                void setDrawn(bool nowDrawn = true);

                /**
                 * Method you can use to determine if the parenthesis should be rendered using fonts.
                 *
                 * \return Returns true if the parenthesis should be rendered using fonts.  Returns false if the
                 *         parenthesis should be drawn.
                 */
                bool rendered() const;

                /**
                 * Method you can use to determine if the parenthesis should be drawn.
                 *
                 * \return Returns true if the parenthesis should be drawn.  Returns false if the parenthesis should be
                 *         rendered using fonts.
                 */
                bool drawn() const;

                /**
                 * Method you can use to set the parenthesis style to be used.
                 *
                 * \param[in] newParenthesisStyle The new parenthesis style.
                 */
                void setParenthesisStyle(EQt::GraphicsMathGroup::ParenthesisStyle newParenthesisStyle);

                /**
                 * Method you can use to obtain the parenthesis style to be used.
                 *
                 * \return Returns the parenthesis style to be used.
                 */
                EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle() const;

                /**
                 * Method you can use to set the scaled font used to render parenthesis.
                 *
                 * \param[in] newScaledFont The new scaled font.
                 */
                void setScaledFont(const QFont& newScaledFont);

                /**
                 * Method you can use to obtain the scaled font to be used to render parenthesis.
                 *
                 * \return Returns a reference to the font to be used to render the parenthesis.
                 */
                const QFont& scaledFont() const;

                /**
                 * Assignment operator
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                ParenthesisData& operator=(const ParenthesisData& other);

            private:
                /**
                 * The current relative position.
                 */
                float currentRelativePosition;

                /**
                 * Flag indicating if font rendering should be used.
                 */
                bool currentRendered;

                /**
                 * The requested parenthesis style.
                 */
                EQt::GraphicsMathGroup::ParenthesisStyle currentParenthesisStyle;

                /**
                 * The scaled font to be used.
                 */
                QFont currentScaledFont;
        };

        /**
         * Method that is used to update the border pen.  The default implementation will return a standard pen color
         * based on the type of reported error.
         *
         * \param[in] diagnostic The diagnostic used to determine the border.  An empty pointer will cause the
         *                       default border to be provided.
         *
         * \return Returns the border pen to apply to the border.
         */
        virtual QPen updateBorder(Ld::DiagnosticPointer diagnostic = Ld::DiagnosticPointer());

        /**
         * Method that is used to update the tool tip for this item.  The default implementation will return a string
         * holding the reported diagnostic message.
         *
         * \param[in] diagnostic The diagnostic used to determine the correct tool tip.
         *
         * \return Returns the tool tip to display in the active area.
         */
        virtual QString updateToolTip(Ld::DiagnosticPointer diagnostic = Ld::DiagnosticPointer());

        /**
         * Method you can use to determine the size to apply to compensate for the left parenthesis.  You can use this
         * to determine spacings prior to actually inserting the parenthesis symbol.
         *
         * \param[in]  font                    The font to associate with the parenthesis.
         *
         * \param[in]  contentsHeight          The height of the contents the parenthesis will contain, in scene units.
         *
         * \param[in]  parenthesisStyle        The type of parenthesis we expect to render.
         *
         * \param[in]  useFontBasedParenthesis If true, this call will render parenthesis using fonts when the height
         *                                     is close to that recommended by the font.  If false, this call will
         *                                     assume parenthesis are always drawn.
         *
         * \return Returns a \ref Presentation::ParenthesisData instance providing information about the parenthesis.
         */
        ParenthesisData leftParenthesisSize(
            const QFont&                             font,
            float                                    contentsHeight,
            EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
            bool                                     useFontBasedParenthesis = true
        ) const;

        /**
         * Method you can use to determine the size to apply to compensate for the right parenthesis.  You can use this
         * to determine spacings prior to actually inserting the parenthesis symbol.
         *
         * \param[in]  font                    The font to associate with the parenthesis.
         *
         * \param[in]  contentsHeight          The height of the contents the parenthesis will contain, in scene units.
         *
         * \param[in]  parenthesisStyle        The type of parenthesis we expect to render.
         *
         * \param[in]  useFontBasedParenthesis If true, this call will render parenthesis using fonts when the height
         *                                     is close to that recommended by the font.  If false, this call will
         *                                     assume parenthesis are always drawn.
         *
         * \return Returns a \ref Presentation::ParenthesisData instance providing information about the parenthesis.
         */
        ParenthesisData rightParenthesisSize(
            const QFont&                             font,
            float                                    contentsHeight,
            EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
            bool                                     useFontBasedParenthesis = true
        ) const;

        /**
         * Method you can use to insert a left parenthesis into a graohics math item.  Note that each math item can
         * only include a single left parenthesis.
         *
         * \param[in]     parenthesisData     The \ref Presentation::ParenthesisData instance supplied by the
         *                                    \ref Presentation::leftParenthesisSize method.
         *
         * \param[in]     parenthesisLeftEdge The desired left edge of the parenthesis.
         *
         * \param[in]     childTopEdge        The top edge of the child.  This method uses this to locate the top edge
         *                                    of the parenthesis.
         *
         * \param[in,out] mathGraphicsItem    The graphics item to received a left parenthesis.
         *
         * \param[in,out] textEntryIndex      An index to the text entry that may (or may not) be used to add the
         *                                    parenthesis.  The value will be incremented if used.  If an entry already
         *                                    exists at the index, it will be reused.
         */
        void insertLeftParenthesis(
            const ParenthesisData&  parenthesisData,
            float                   parenthesisLeftEdge,
            float                   childTopEdge,
            EQt::GraphicsMathGroup* mathGraphicsItem,
            unsigned&               textEntryIndex
        );

        /**
         * Method you can use to insert a right parenthesis into a graohics math item.  Note that each math item can
         * only include a single right parenthesis.
         *
         * \param[in]     parenthesisData     The \ref Presentation::ParenthesisData instance supplied by the
         *                                    \ref Presentation::leftParenthesisSize method.
         *
         * \param[in]     parenthesisLeftEdge The desired left edge of the parenthesis.
         *
         * \param[in]     childTopEdge        The top edge of the child.  This method uses this to locate the top edge
         *                                    of the parenthesis.
         *
         * \param[in,out] mathGraphicsItem    The graphics item to received a left parenthesis.
         *
         * \param[in,out] textEntryIndex      An index to the text entry that may (or may not) be used to add the
         *                                    parenthesis.  The value will be incremented if used.  If an entry already
         *                                    exists at the index, it will be reused.
         */
        void insertRightParenthesis(
            const ParenthesisData&  parenthesisData,
            float                   parenthesisLeftEdge,
            float                   childTopEdge,
            EQt::GraphicsMathGroup* mathGraphicsItem,
            unsigned&               textEntryIndex
        );

        /**
         * Method that provides the character to use for the left parenthesis.
         *
         * \param[in] parenthesisStyle
         *
         * \return Returns the charater tied to the style.
         */
        static QString leftParenthesisCharacter(EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle);

        /**
         * Method that provides the character to use for the right parenthesis.
         *
         * \param[in] parenthesisStyle
         *
         * \return Returns the charater tied to the style.
         */
        static QString rightParenthesisCharacter(EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle);

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
         * \param[out] closestPoint       An optional value that can be populated with the closest point on the closest
         *                                presentation area.
         *
         * \return Returns the distance in scene units.
         */
        double distanceToClosestChild(
            const QPointF&       location,
            const Presentation** bestPresentation = Q_NULLPTR,
            unsigned long*       presentationAreaId = Q_NULLPTR,
            QPointF*             closestPoint = Q_NULLPTR
        ) const;

        /**
         * Method that can be called from slots to determine if this presentation has an associated element and if
         * the method has been grafted to the root.
         *
         * \return Returns true if all the conditions are met.  Returns false if any condition is not met.
         */
        bool graftedToRoot() const;

        /**
         * Method you can use to scale a font to a specific height in scene units.
         *
         * \param[in] font The font to be adjusted.
         *
         * \param[in] height The desired height for the font, in scene units.
         *
         * \return Returns the scaled version of the font.
         */
        static QFont scaleFontToHeight(const QFont& font, float height);

        /**
         * Method that calculates the closest character index to a location.
         *
         * \param[in] text                 The text relative to the left edge.
         *
         * \param[in] font                 The font used to locate the text.
         *
         * \param[in] distanceFromLeftEdge The distance from the left edge
         *
         * \return Returns the closest character index.
         */
        static unsigned textIndexAtLocation(const QString& text, const QFont& font, float distanceFromLeftEdge);

        /**
         * Method that identifies the child presentation that is closest to a given point.
         *
         * \param[in]     location        The reference location, in absolute scene coordinates.
         *
         * \param[in,out] squaredDistance The measured squared distance to the closest child presentation.
         *
         * \param[in,out] closestPoint    An optional pointer to a QPointF instance that will hold the location of the
         *                                closest known point in the child presentation.
         *
         * \return Returns a zero based presentation index of the closest child.  A value of
         *         Ld::Element::invalidChildIndex will be returned if no child could be identified.
         */
        virtual unsigned long closestChildPresentation(
            const QPointF& location,
            float*         squaredDistance = Q_NULLPTR,
            QPointF*       closestPoint = Q_NULLPTR
        ) const;

        /**
         * Method that identifies the closest text region and offset into the text to a given point.
         *
         * \param[in]     location        The reference location, in absolute scene coordinates.
         *
         * \param[in,out] squaredDistance The measured squared distance to the closest child presentation.
         *
         * \return Returns a pair indicating the zero based offset into the region and the zero based displayed region
         *         index.  A pair with the values Ld::Element::invalidTextIndex and Ld::Element::invalidRegionIndex
         &         will be returned if no text could be identified.
         */
        virtual QPair<unsigned long, unsigned> closestText(
            const QPointF& location,
            float*         squaredDistance = Q_NULLPTR
        ) const;

        /**
         * Method you can overload to determine the number of displayed text regions.  Text that can not be selected
         * by the cursor should not be included.
         *
         * \return Returns the number of displayed text regions.  The default implementation returns 0.
         */
        virtual unsigned numberDisplayedTextRegions() const;

        /**
         * Method you can overload to determine the text and font and associated with a given region.  Text that can
         * not be selected by the cursor should not be included.  This version will assert if called.
         *
         * \param[in] regionIndex The zero based displayed text region index.
         *
         * \return Returns a pair holding the text and font at the requested region.
         */
        virtual QPair<QString, QFont> textAtDisplayedRegion(unsigned regionIndex) const;

        /**
         * Method you can overload to indicate if the left psotion of a displayed text region represents the baseline
         * position or the top-left corner.
         *
         * \return Returns true if the displayed text region position represents the baseline position.  Returns false
         *         if the text region position represents the top-left corner.  The default position returns true.
         */
        virtual bool textPositionIsBaseline() const;

        /**
         * Method you can overload to determine the left position of a displayed text region.  This version will assert
         * if called.
         *
         * \param[in] regionIndex The zero based displayed text region index.
         *
         * \return Returns the left position of the text region in absolute scene coordinates.
         */
        virtual QPointF displayedTextPosition(unsigned regionIndex) const;

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
        virtual Ld::ElementCursor convertToElementCursor(
            unsigned long displayedTextOffset,
            unsigned      displayedRegionIndex
        ) const;

        /**
         * Method you can use to obtain the presentation tied to a child element.
         *
         * \param[in] presentationIndex The zero based presentation index.
         *
         * \return Returns a pointer to the presentation.  A null pointer is returned if the element is null or if
         *         there is no presentation tied to the element.
         */
        const Presentation* childPresentation(unsigned long presentationIndex) const;

        /**
         * Method you can use to obtain the presentation tied to a child element.
         *
         * \param[in] presentationIndex The zero based presentation index.
         *
         * \return Returns a pointer to the presentation.  A null pointer is returned if the element is null or if
         *         there is no presentation tied to the element.
         */
        Presentation* childPresentation(unsigned long presentationIndex);

        /**
         * Method you can overload to handle a request for an image of the associated element.
         *
         * \param[in] dpi The image resolution in DPI.
         *
         * \return Returns a byte array containing a binary representation of the requested image.  An empty byte
         *         array is returned if the requested image could not be generated.  The default implementation
         *         returns an empty byte array.
         */
        QByteArray generateImage(float dpi) const override;

    private:
        /**
         * Method that is called when this visual is tied to an element.
         *
         * \param[in] element The element that this visual is now tied to.
         */
        void coupledToElement(Ld::ElementPointer element) final;

        /**
         * Method that is called when this visual is disconnected from an element.
         */
        void decoupledFromElement() final;

        /**
         * Method that is called when the format tied to the associated Ld::Element changes or is updated.  The method
         * can be triggered by either the Ld::Element::setFormat method or by the Ld::Format::reportUpdate method.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same
         *                      as the oldFormat pointer if the Ld::Format instance was not changed.
         */
        void formatChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) final;

        /**
         * Method that is triggered whenever a descendant's format has been changed.
         *
         * \param[in] changedElement The element who's format has been changed.
         *
         * \param[in] newFormat      The new format associated with this element.
         */
        void descendantFormatChanged(Ld::ElementPointer changedElement, Ld::FormatPointer newFormat) final;

        /**
         * Method that is triggered whenever a descendant's format has been updated.
         *
         * \param[in] changedElement The element who's format has been changed.
         *
         * \param[in] newFormat      The updated format associated with this element.
         */
        void descendantFormatUpdated(Ld::ElementPointer changedElement, Ld::FormatPointer newFormat) final;

        /**
         * Method that is called when an aspect of the associated Ld::Element has been changed.  This method exists to
         * cover such things as text updates that are not covered by the addition or removal of children, nor format
         * updates.
         */
        void elementDataChanged() final;

        /**
         * Method that is called to indicate that a diagnostic was just reported for the associated element.  Note
         * that this method may be called from multiple threads.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void diagnosticReported(Ld::DiagnosticPointer diagnostic) final;

        /**
         * Method that is called to indicate that any pending diagnostic condition has been cleared.  Note that this
         * method may be called even if there is no reported diagnostic and may be called from multiple threads.
         */
        void diagnosticCleared() final;

        /**
         * Method that is called to indicate that the element has just ben grafted to a new element tree.
         */
        void graftedToTree() final;

        /**
         * Method that is called to indicate that the element is about to be ungrafted from the element tree.  Note
         * that the element being ungrafted from the tree may be different than the element being deleted.
         */
        void aboutToUngraftFromTree() final;

        /**
         * Method that is called to indicate that an instruction breakpoint has been set or cleared on this
         * element.
         *
         * \param[in] breakpointNowSet If true, a breakpoint on this element has been set.  If false, a breakpoint
         *                             on this element has been cleared.
         */
        void instructionBreakpointUpdated(bool breakpointNowSet) final;

        /**
         * Method that is called to indicae that a calculated value associated with the element has been
         * updated.  The default implementation does nothing.
         *
         * \param[in] valueIndex      The zero based value index that was just updated.
         *
         * \param[in] calculatedValue The calculated value.
         */
        void calculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue) final;

        /**
         * Method you can overload to receive notification when the calculated value should be cleared.
         */
        void calculatedValueCleared() final;

        /**
         * Flag that is set if this presentation is grafted to the root.
         */
        bool currentlyGraftedToRoot;
};

#endif
