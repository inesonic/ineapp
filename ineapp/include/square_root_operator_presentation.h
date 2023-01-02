/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SquareRootOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SQUARE_ROOT_OPERATOR_PRESENTATION_H
#define SQUARE_ROOT_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_square_root_operator_visual.h>

#include "app_common.h"
#include "root_operator_presentation_base.h"

namespace EQt {
    class GraphcsMathGroup;
    class GraphicsMathGroupWithPainterPath;
}

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::SquareRootOperatorElement class.
 */
class APP_PUBLIC_API SquareRootOperatorPresentation:public         RootOperatorPresentationBase,
                                                    public virtual Ld::SquareRootOperatorVisual {
    Q_OBJECT

    public:
        SquareRootOperatorPresentation();

        ~SquareRootOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref UnaryPlusOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Method that is used to build the graphics item for this operator.
         *
         * \param[in]  format                    Pointer to the format instance tied to the element.
         *
         * \param[in]  parenthesisStyle          Indicates the type of parenthesis that should be placed around the
         *                                       operator.  A value of
         *                                       OperatorPresentationBase::ParenthesisStyle::INVALID indicates that
         *                                       parenthesis should not be included.
         *
         * \param[in]  relativeScale             The relative scale to apply to the various symbols.
         *
         * \param[in]  childPresentationAreaData The presentation area data for the children.
         *
         * \param[out] requiredSize              The required size of the graphics item, in scene units.
         *
         * \param[out] requiredAscent            The required ascent of the graphics item, in scene units.
         *
         * \return Returns the newly constructed graphics item.
         */
        EQt::GraphicsMathGroup* buildGraphicsItem(
            Ld::FormatPointer                     format,
            ParenthesisStyle                      parenthesisStyle,
            float                                 relativeScale,
            const QList<PresentationAreaTracker>& childPresentationAreaData,
            QSizeF&                               requiredSize,
            float&                                requiredAscent
        ) override;

        /**
         * Method that is used to build the graphics item for this operator.
         *
         * \param[in]  format                     Pointer to the format instance tied to the element.
         *
         * \param[in]  currentItem                The current graphics multi-text item to be updated.
         *
         * \param[in]  parenthesisStyle           Indicates the type of parenthesis that should be placed around the
         *                                        operator.  A value of
         *                                        OperatorPresentationBase::ParenthesisStyle::INVALID indicates
         *                                        that parenthesis should not be included.
         *
         * \param[in]  relativeScale              The relative scale to apply to the various symbols.
         *
         * \param[in]  childPresentationAreaData  The presentation area data for the children.
         *
         * \param[out] requiredSize               The required size of the graphics item, in scene units.
         *
         * \param[out] requiredAscent             The required ascent of the graphics item, in scene units.
         *
         * \return Returns the a pointer to the current (or new) EQt::GraphicsMathGroup item.
         */
        EQt::GraphicsMathGroup* updateGraphicsItem(
            Ld::FormatPointer                     format,
            EQt::GraphicsMathGroup*               currentItem,
            ParenthesisStyle                      parenthesisStyle,
            float                                 relativeScale,
            const QList<PresentationAreaTracker>& childPresentationAreaData,
            QSizeF&                               requiredSize,
            float&                                requiredAscent
        ) override;

    private:
        /**
         * Method that is used to build the graphics item for this operator.
         *
         * \param[in]  format                     Pointer to the format instance tied to the element.
         *
         * \param[in]  currentItem                The current graphics multi-text item to be updated.
         *
         * \param[in]  parenthesisStyle           Indicates the type of parenthesis that should be placed around the
         *                                        operator.  A value of
         *                                        OperatorPresentationBase::ParenthesisStyle::INVALID indicates that
         *                                        that parenthesis should not be included.
         *
         * \param[in]  relativeScale              The relative scale to apply to the various symbols.
         *
         * \param[in]  childPresentationAreaData  The presentation area data for the children.
         *
         * \param[out] requiredSize               The required size of the graphics item, in scene units.
         *
         * \param[out] requiredAscent             The required ascent of the graphics item, in scene units.
         *
         * \return Returns the a pointer to the current (or new) EQt::GraphicsMathGroup item.
         */
        EQt::GraphicsMathGroup* updateGraphicsItem(
            Ld::FormatPointer                      format,
            EQt::GraphicsMathGroupWithPainterPath* currentItem,
            ParenthesisStyle                       parenthesisStyle,
            float                                  relativeScale,
            const QList<PresentationAreaTracker>&  childPresentationAreaData,
            QSizeF&                                requiredSize,
            float&                                 requiredAscent
        );
};

#endif
