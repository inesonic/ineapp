/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DataTypePresentationGenerator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DATA_TYPE_PRESENTATION_GENERATOR_H
#define DATA_TYPE_PRESENTATION_GENERATOR_H

#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QList>

#include <eqt_graphics_math_group.h>

#include <model_variant.h>

#include <ld_format_structures.h>
#include <ld_data_type_decoder.h>

#include "app_common.h"
#include "presentation.h"
#include "data_type_presenter.h"

namespace EQt {
    class GraphicsMultiTextGroup;
}

namespace Ld {
    class DataTypeFormat;
}

class QGraphicsItem;

class PlacementTracker;

/**
 * Base class for generators used to generate textual and graphics item presentations of variables. data type.
 */
class APP_PUBLIC_API DataTypePresentationGenerator:public virtual Ld::DataTypeDecoder {
    public:
        DataTypePresentationGenerator();

        ~DataTypePresentationGenerator() override;

        /**
         * Method that is called by the DataTypePresenter to request that the generator render a presentation for a
         * supplied value.
         *
         * The default implementation will call either \ref DataTypePresentationGenerator::toGraphicsItem or
         * \ref DataTypePresentationGenerator::updateGraphicsItem depending the the state of the supplied list.
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
        virtual DataTypePresenter::PresentationAreas generateGraphicsItems(
            PlacementTracker*                     placementTracker,
            DataTypePresenter::PresentationAreas& currentPresentationAreas,
            DataTypePresenter*                    presenter,
            const Model::Variant&                 variant,
            Ld::FormatPointer                     format,
            float                                 lineSpacing = Presentation::defaultLineSpacing,
            float                                 relativeScale = 1.0,
            bool                                  forceAppend = false
        ) const;

        /**
         * Pure virtual method you can call to calculate the height and ascent of the expected representation of an
         * instance of this data type.
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
        virtual float calculateHeight(
            PlacementTracker*     placementTracker,
            const Model::Variant& variant,
            Ld::FormatPointer     format,
            float                 relativeScale = 1.0,
            float*                ascent = Q_NULLPTR
        ) const = 0;

        /**
         * Method you can overload to create a graphic representation of a value of this data-type.
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
        virtual EQt::GraphicsMultiTextGroup* toGraphicsItem(
            PlacementTracker*     placementTracker,
            const Model::Variant& value,
            Ld::FormatPointer     format,
            DataTypePresenter*    presenter,
            QSizeF&               size,
            float&                ascent
        ) const = 0;

        /**
         * Method you can optionally overload to update an existing graphic representation of a value of this
         * data-type.  The default implementation will simply call \ref DataTypePresentationGenerator::toGraphicsItem.
         *
         * \param[in]  placementTracker A pointer to the placement tracker that can be used to track and update
         *                              status during a time-consuming placement operation.
         *
         * \param[in]  graphicsItem     The graphics item to be updated (or replaced).  Note that you do not need to
         *                              delete this graphics item if you intend to replace it.  The calling methods
         *                              will automatically detect that the graphics item is different and delete this
         *                              instance.
         *
         * \param[in]  value            A variant type containing the value to be translated.
         *
         * \param[in]  format           Format information used to control how the data-type is translated.  Note that
         *                              the translation engine must be able to perform a reasonable translation even
         *                              if a format has not been supplied.
         *
         * \param[in]  presenter        Pointer to the presenter requesting the rendering operation.
         *
         * \param[out] size             A reference to a location to hold the size of the graphic representation in
         *                              scene units.
         *
         * \param[out] ascent           The ascent to apply to the graphics representation in scene units.
         *
         * \return Returns a graphics item containing the graphic representation of the data-type.  Note that this may
         *         be a different graphics item than the one supplied.
         */
        virtual EQt::GraphicsMultiTextGroup* updateGraphicsItem(
            PlacementTracker*            placementTracker,
            EQt::GraphicsMultiTextGroup* graphicsItem,
            const Model::Variant&        value,
            Ld::FormatPointer            format,
            DataTypePresenter*           presenter,
            QSizeF&                      size,
            float&                       ascent
        ) const;

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
        virtual Presentation::ReflowHint reflowHint(bool afterAbort) const;

    protected:
        /**
         * Method you can use to request free space from the presenter.
         *
         * \param[in]  presenter      The presenter that will be displaying this data item.
         *
         * \param[in]  requiredSpace  The space required for the requested item.
         *
         * \param[in]  spaceQualifier An optional value providing additional information about the provided space.
         *
         * \return Returns the amount of space found.  You will need to call the presenter to allocate the space.
         */
        static QSizeF locateFreeSpace(
            DataTypePresenter*                   presenter,
            const QSizeF&                        requiredSpace,
            PlacementNegotiator::SpaceQualifier* spaceQualifier = Q_NULLPTR
        );

        /**
         * Method you can use to append an EQt::GraphicsTextItem containing provided text to a set of presentation
         * areas.
         *
         * \param[in,out] currentPresentationAreas The current list of presentation areas.
         *
         * \param[in]     presenter                The presenter that will be displaying this data item.
         *
         * \param[in]     text                     The text to be added.
         *
         * \param[in]     font                     The font to be applied to the text.  The font should be pre-scaled
         *                                         based on the relative scale and application level font scaling.
         *
         * \param[in]     fontColor                The desired font color.
         *
         * \param[in]     fontBackgroundColor      The desired font background color.
         *
         * \param[in]     lineSpacing              Additional line spacing that should be optionally included by this
         *                                         child.  Children can adjust the height they report for their
         *                                         presentation areas.  A value of 1.0 indicates single spacing.  A
         *                                         value of 1.5 indicates 1 1/2 line spacing.  A value of 2.0 indicates
         *                                         double spacing.
         */
        static void addText(
            DataTypePresenter::PresentationAreas& currentPresentationAreas,
            DataTypePresenter*                    presenter,
            const QString&                        text,
            const QFont&                          font,
            const QColor&                         fontColor,
            const QColor&                         fontBackgroundColor,
            float                                 lineSpacing
        );

        /**
         * Method you can use to add a left parenthesis to a collection of presentation areas.
         *
         * \param[in,out] currentPresentationAreas The current list of presentation areas.
         *
         * \param[in]     presenter                The presenter that will be displaying this data item.
         *
         * \param[in]     contentsHeight           The height of the contents the parenthesis will contain.
         *
         * \param[in]     parenthesisStyle         The style to apply to the parenthesis.
         *
         * \param[in]     leftParenthesisString    A string holding the character representation of the left
         *                                         parenthesis.  The value is used as an alternate to the
         *                                         parenthesisStyle parameter when the parenthesis are rendered using
         *                                         text.
         *
         * \param[in]     font                     The font to be applied to the text.  You do not need to pre-scale
         *                                         the font although doing so may reduce overhead slightly.
         *
         * \param[in]     fontColor                The desired font color.
         *
         * \param[in]     fontBackgroundColor      The desired font background color.
         */
        static void addLeftParenthesis(
            DataTypePresenter::PresentationAreas&    currentPresentationAreas,
            DataTypePresenter*                       presenter,
            float                                    contentsHeight,
            EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
            const QString&                           leftParenthesisString,
            const QFont&                             font,
            const QColor&                            fontColor,
            const QColor&                            fontBackgroundColor
        );

        /**
         * Method you can use to add a right parenthesis to a collection of presentation areas.
         *
         * \param[in,out] currentPresentationAreas The current list of presentation areas.
         *
         * \param[in]     presenter                The presenter that will be displaying this data item.
         *
         * \param[in]     contentsHeight           The height of the contents the parenthesis will contain.
         *
         * \param[in]     parenthesisStyle         The style to apply to the parenthesis.
         *
         * \param[in]     rightParenthesisString   A string holding the character representation of the right
         *                                         parenthesis.  The value is used as an alternate to the
         *                                         parenthesisStyle parameter when the parenthesis are rendered using
         *                                         text.
         *
         * \param[in]     font                     The font to be applied to the text.  You do not need to pre-scale
         *                                         the font although doing so may reduce overhead slightly.
         *
         * \param[in]     fontColor                The desired font color.
         *
         * \param[in]     fontBackgroundColor      The desired font background color.
         */
        static void addRightParenthesis(
            DataTypePresenter::PresentationAreas&    currentPresentationAreas,
            DataTypePresenter*                       presenter,
            float                                    contentsHeight,
            EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle,
            const QString&                           rightParenthesisString,
            const QFont&                             font,
            const QColor&                            fontColor,
            const QColor&                            fontBackgroundColor
        );

        /**
         * Method that inserts a text field into a graphics item.
         *
         * \param[in]     graphicsItem The graphics item to receive the text.
         *
         * \param[in]     text         The text to be inserted.
         *
         * \param[in]     font         The font to apply to the text.The font should be pre-scaled base on the relative
         *                             relative scale and application level font scaling.
         *
         * \param[in]     position     The position where the text should be placed.
         *
         * \param[in,out] index        The current index.  The value will be incremented each time a new text entry
         *                             is inserted.
         */
        static void addText(
            EQt::GraphicsMultiTextGroup* graphicsItem,
            const QString&               text,
            const QFont&                 font,
            const QPointF&               position,
            unsigned&                    index
        );
};


#endif
