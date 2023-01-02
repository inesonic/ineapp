/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LogicalNotOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LOGICAL_NOT_OPERATOR_PRESENTATION_BASE_H
#define LOGICAL_NOT_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_logical_unary_not_operator_format.h>
#include <ld_visual_with_fixed_children.h>

#include "app_common.h"
#include "unary_operator_presentation_base.h"

namespace EQt {
    class GraphicsMathGroup;
    class GraphicsMathGroupWithLine;
}

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::LogicalUnaryNotOperatorElement class.
 */
class APP_PUBLIC_API LogicalNotOperatorPresentationBase:public         UnaryOperatorPresentationBase,
                                                        public virtual Ld::VisualWithFixedChildren {
    Q_OBJECT

    public:
        LogicalNotOperatorPresentationBase();

        ~LogicalNotOperatorPresentationBase() override;

    protected slots:
        /**
         * Slot that is triggered when a Ld::Element reports that its format has been changed.  You can use this slot
         * to perform updates to the visual representation of the element.
         *
         * This version will clear the child presentation area trackers and then call the base class version to trigger
         * the presentation to be repositioned and redrawn.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                      oldFormat pointer if the Ld::Format instance was not changed.
         */
        void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) override;

    protected:
        /**
         * Method you can overload to specify the default format for this logical not operation.
         *
         * \return Returns the default logical not style to use for this operation.
         */
        virtual Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol defaultLogicalNotSymbol() const = 0;

        /**
         * Method that is used to build the graphics item for this operator.  The default implementation builds the
         * graphics item from calls to other virtual methods.  This method will also call update the border and tool
         * tip based on any currently reported diagnostics.
         *
         * \param[in]  format                     Pointer to the format instance tied to the element.
         *
         * \param[in]  parenthesisStyle           Indicates the type of parenthesis that should be placed around the
         *                                        operator.  A value of
         *                                        OperatorPresentationBase::ParenthesisStyle::INVALID indicates that
         *                                        parenthesis should not be included.
         *
         * \param[in]  relativeScale              The relative scale to apply to the various symbols.
         *
         * \param[in]  childPresentationAreaData  The presentation area data for the children.
         *
         * \param[out] requiredSize               The required size of the graphics item, in scene units.
         *
         * \param[out] requiredAscent             The required ascent of the graphics item, in scene units.
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
         * Method that is used to build the graphics item for this operator.  The default implementation builds the
         * graphics item from calls to other virtual methods.  This method will also call update the border and tool
         * tip based on any currently reported diagnostics.
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

        /**
         * Method that provides the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string.  The default implementation returns an inverted question mark.
         */
         QString operatorString(Ld::FormatPointer operatorFormat) const override;

     private:
         /**
          * Method that is used to update the overbar style not operator.
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
         EQt::GraphicsMathGroupWithLine* updateOverbarGraphicsItem(
             Ld::FormatPointer                     format,
             EQt::GraphicsMathGroupWithLine*       currentItem,
             ParenthesisStyle                      parenthesisStyle,
             float                                 relativeScale,
             const QList<PresentationAreaTracker>& childPresentationAreaData,
             QSizeF&                               requiredSize,
             float&                                requiredAscent
         );

         /**
          * Method used to determine the required operator style.
          *
          * \param[in] format The format to be tested.
          *
          * \return Returns the required operator style.
          */
         Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol logicalNotStyle(Ld::FormatPointer format) const;
};

#endif
