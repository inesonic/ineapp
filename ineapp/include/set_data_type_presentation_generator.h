/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SetDataTypePresentationGenerator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SET_DATA_TYPE_PRESENTATION_GENERATOR_H
#define SET_DATA_TYPE_PRESENTATION_GENERATOR_H

#include <QCoreApplication>
#include <QSharedPointer>
#include <QSizeF>
#include <QFont>
#include <QColor>

#include <model_set.h>
#include <model_variant.h>

#include <ld_set_data_type_decoder.h>

#include "app_common.h"
#include "presentation.h"
#include "data_type_presentation_generator.h"

namespace EQt {
    class GraphicsMultiTextGroup;
}

namespace Ld {
    class DataTypeFormat;
}

class QGraphicsItem;

class PlacementTracker;

/**
 * Presentation generator used to render set values.
 */
class APP_PUBLIC_API SetDataTypePresentationGenerator:public virtual Ld::SetDataTypeDecoder,
                                                      public virtual DataTypePresentationGenerator {
    Q_DECLARE_TR_FUNCTIONS(SetDataTypePresentationGenerator)

    public:
        /**
         * Static instance you can use for the application configuration.
         */
        static const SetDataTypePresentationGenerator instance;

        SetDataTypePresentationGenerator();

        ~SetDataTypePresentationGenerator() override;

        /**
         * Method that calculates the height and ascent of the expected representation of an instance of this data
         * type.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update status
         *                              during a time-consuming placement operation.
         *
         * \param[in]  variant          The value to be rendered.
         *
         * \param[in]  format           The format used to render the data type.
         *
         * \param[in]  relativeScale    An optional relative scale factor used to adjust the size.
         *
         * \param[out] ascent           An optional location that will be populated with the ascent.
         *
         * \return Returns the expected height, in scene units.
         */
        float calculateHeight(
            PlacementTracker*     placementTracker,
            const Model::Variant& variant,
            Ld::FormatPointer     format,
            float                 relativeScale,
            float*                ascent = Q_NULLPTR
        ) const override;

        /**
         * Method that is called by the DataTypePresenter to request that the generator render a presentation for a
         * supplied value.
         *
         * \param[in]  placementTracker         A pointer to the placement tracker that can be used to track and
         *                                      update status during a time-consuming placement operation.
         *
         * \param[in]  currentPresentationAreas The current list of presentation areas.  The method may choose to
         *                                      modify the supplied list and return it or delete the list contents and
         *                                      return a new list.
         *
         * \param[in]  presenter                Pointer to the presenter requesting the rendering operation.
         *
         * \param[in]  variant                  The value to be rendered.
         *
         * \param[in]  format                   The format used to render the data type.
         *
         * \param[in]  lineSpacing              Additional line spacing that should be optionally included by this
         *                                      child.  Children can adjust the height they report for their
         *                                      presentation areas.  A value of 1.0 indicates single spacing.  A va;ue
         *                                      of 1.5 indicates 1 1/2 line spacing.  A value of 2.0 indicates double
         *                                      spacing.
         *
         * \param[in]  relativeScale            A hint value used to indicate if this child should try to scale smaller
         *                                      than usual. This value is intended for cases where an equation element
         *                                      needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                      indicates normal scale.  The value is intended as a hint, children are
         *                                      not required to honor this parameter.
         *
         * \param[in]  forceAppend              If true, this call should always append presentation areas and should
         *                                      not make assumptions about the current state of the presentation areas.
         *
         * \return Returns a list of presentation areas.
         */
        DataTypePresenter::PresentationAreas generateGraphicsItems(
            PlacementTracker*                     placementTracker,
            DataTypePresenter::PresentationAreas& currentPresentationAreas,
            DataTypePresenter*                    presenter,
            const Model::Variant&                 variant,
            Ld::FormatPointer                     format,
            float                                 lineSpacing = Presentation::defaultLineSpacing,
            float                                 relativeScale = 1.0,
            bool                                  forceAppend = false
        ) const override;

        /**
         * Method you can overload to create a graphic representation of a value of this data-type.  This version will
         * assert if called.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update
         *                              status during a time-consuming placement operation.
         *
         * \param[in]  value            A variant type containing the value to be translated.
         *
         * \param[in]  format           Format information used to control how the data-type is translated.  Note that
         *                              the translation engine must be able to perform a reasonable translation even if
         *                              a format has not been supplied.
         *
         * \param[in]  presenter        Pointer to the presenter requesting the rendering operation.
         *
         * \param[out] size             A reference to a location to hold the size of the graphic representation in
         *                              scene units.
         *
         * \param[out] ascent           The ascent to apply to the graphics representation in scene units.
         *
         * \return Returns a graphics item containing the graphic representation of the data-type.
         */
        EQt::GraphicsMultiTextGroup* toGraphicsItem(
            PlacementTracker*     placementTracker,
            const Model::Variant& value,
            Ld::FormatPointer     format,
            DataTypePresenter*    presenter,
            QSizeF&               size,
            float&                ascent
        ) const override;

        /**
         * Method you can use to obtain the recommended reflow hint for this data type.
         *
         * \param[in] afterAbort If true, the reflow hint should assume that we've aborted the placement and will
         *                       need to redo placement.  If false, the reflow hint should assume that the item has
         *                       already been successfully placed.
         *
         * \return Returns the reflow hint to apply to this data type.  The default version always returns
         *         Presentation::ReflowHint::REFLOW_NOT_SUPPORTED.
         */
        Presentation::ReflowHint reflowHint(bool afterAbort) const override;

    private:
        /**
         * Method that is called to render a set using the wrapped format.
         *
         * \param[in]     placementTracker         A pointer to the placement tracker that can be used to track and
         *                                         update status during a time-consuming placement operation.
         *
         * \param[in,out] currentPresentationAreas The current list of presentation areas.
         *
         * \param[in]     presenter                Pointer to the presenter requesting the rendering operation.
         *
         * \param[in]     set                      The set to be rendered.
         *
         * \param[in]     format                   The format to use for the child elements.
         *
         * \param[in]     font                     The font to be used to render text.  The font should be pre-scaled
         *                                         based on the relative scale and application level font scaling.
         *
         * \param[in]     fontColor                The font color to be used.
         *
         * \param[in]     fontBackgroundColor      The font background color to be used.
         *
         * \param[in]     maximumChildHeight       The maximum height across all the of the child elements.
         *
         * \param[in]     leadingMemberCount       The number of children at the front of the set to be rendered.
         *
         * \param[in]     trailingMemberCount      The number of children at the end of the set to be rendered.
         *
         * \param[in]     hiddenMemberCount        The number of unrendered children.
         *
         * \param[in]     showHiddenMemberCount    If true, the rendered set should show the number of children that
         *                                         were not rendered.
         *
         * \param[in]     lineSpacing              Additional line spacing that should be optionally included by this
         *                                         child.  Children can adjust the height they report for their
         *                                         presentation areas.  A value of 1.0 indicates single spacing.  A
         *                                         value of 1.5 indicates 1 1/2 line spacing.  A value of 2.0
         *                                         indicates double spacing.
         *
         * \param[in]     relativeScale            A hint value used to indicate if this child should try to scale
         *                                         smaller than usual. This value is intended for cases where an
         *                                         equation element needs to be rendered in a smaller than usual
         *                                         font.  A value of 1.0 indicates normal scale.  The value is
         *                                         intended as a hint, children are not required to honor this
         *                                         parameter.
         *
         * \return Returns a list of presentation areas.
         */
        DataTypePresenter::PresentationAreas renderWrapped(
            PlacementTracker*                     placementTracker,
            DataTypePresenter::PresentationAreas& currentPresentationAreas,
            DataTypePresenter*                    presenter,
            const Model::Set&                     set,
            Ld::FormatPointer                     format,
            const QFont&                          font,
            const QColor&                         fontColor,
            const QColor&                         fontBackgroundColor,
            float                                 maximumChildHeight,
            unsigned long                         leadingMemberCount,
            unsigned long                         trailingMemberCount,
            unsigned long                         hiddenMemberCount,
            bool                                  showHiddenMemberCount,
            float                                 lineSpacing = Presentation::defaultLineSpacing,
            float                                 relativeScale = 1.0
        ) const;

        /**
         * Method that is called to calculate the maximum ascent and descent across a range of set entries.
         *
         * \param[in]     placementTracker A pointer to the placement tracker that can be used to track and update
         *                                 status during a time-consuming placement operation.
         *
         * \param[in,out] setIterator      Iterator to the first member of the set to be rendered.
         *
         * \param[in]     memberCount      The number of set members to be rendered.
         *
         * \param[in]     format           The format to use for the child elements.
         *
         * \param[in]     relativeScale    A hint value used to indicate if this child should try to scale smaller than
         *                                 usual. This value is intended for cases where an equation element needs to
         *                                 be rendered in a smaller than usual font.  A value of 1.0 indicates normal
         *                                 scale.  The value is intended as a hint, children are not required to honor
         *                                 this parameter.
         *
         * \param[in,out] maximumAscent  The working maximum ascent value.
         *
         * \param[in,out] maximumDescent The working maximum descent value.
         */
        static void calculateMaximumChildHeight(
            PlacementTracker*          placementTracker,
            Model::Set::ConstIterator& setIterator,
            unsigned long              memberCount,
            Ld::FormatPointer          format,
            float                      relativeScale,
            float&                     maximumAscent,
            float&                     maximumDescent
        );

        /**
         * Method that is called to render a range of values in a set.
         *
         * \param[in]     placementTracker         A pointer to the placement tracker that can be used to track and
         *                                         update status during a time-consuming placement operation.
         *
         * \param[in,out] setIterator              Iterator to the first member of the set to be rendered.
         *
         * \param[in]     memberCount              The number of set members to be rendered.
         *
         * \param[in,out] isFirstEntry             Flag that is used to identify if a comma is needed before the
         *                                         first entry.  The value will be set to false after an entry is
         *                                         rendered.
         *
         * \param[in,out] currentPresentationAreas The current list of presentation areas.
         *
         * \param[in]     presenter                Pointer to the presenter requesting the rendering operation.
         *
         * \param[in]     format                   The format to use for the child elements.
         *
         * \param[in]     font                     The font to be used to render text.
         *
         * \param[in]     fontColor                The font color to be used.
         *
         * \param[in]     fontBackgroundColor      The font background color to be used.
         *
         * \param[in]     lineSpacing              Additional line spacing that should be optionally included by this
         *                                         child.  Children can adjust the height they report for their
         *                                         presentation areas.  A value of 1.0 indicates single spacing.  A
         *                                         value of 1.5 indicates 1 1/2 line spacing.  A value of 2.0
         *                                         indicates double spacing.
         *
         * \param[in]     relativeScale            A hint value used to indicate if this child should try to scale
         *                                         smaller than usual. This value is intended for cases where an
         *                                         equation element needs to be rendered in a smaller than usual
         *                                         font.  A value of 1.0 indicates normal scale.  The value is
         *                                         intended as a hint, children are not required to honor this
         *                                         parameter.
         */
        static void renderSetMembers(
            PlacementTracker*                     placementTracker,
            Model::Set::ConstIterator&            setIterator,
            unsigned long                         memberCount,
            bool&                                 isFirstEntry,
            DataTypePresenter::PresentationAreas& currentPresentationAreas,
            DataTypePresenter*                    presenter,
            Ld::FormatPointer                     format,
            const QFont&                          font,
            const QColor&                         fontColor,
            const QColor&                         fontBackgroundColor,
            float                                 lineSpacing = Presentation::defaultLineSpacing,
            float                                 relativeScale = 1.0
        );
};

#endif
