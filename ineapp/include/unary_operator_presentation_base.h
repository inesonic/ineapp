/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref UnaryOperatorPresentationBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef UNARY_OPERATOR_PRESENTATION_BASE_H
#define UNARY_OPERATOR_PRESENTATION_BASE_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QFont>

#include <ld_element.h>
#include <ld_format_structures.h>

#include "app_common.h"
#include "presentation_area_tracker.h"
#include "operator_presentation_base.h"

class QGraphicsItem;

namespace EQt {
    class GraphicsMathGroup;
}

class PlacementNegotiator;
class Presentation;
class PresentationArea;

/**
 * You can use this class as a base class for unary operator presentations.
 */
class APP_PUBLIC_API UnaryOperatorPresentationBase:public OperatorPresentationBase {
    Q_OBJECT

    public:
        UnaryOperatorPresentationBase();

        ~UnaryOperatorPresentationBase() override;

    protected:
        /**
         * Virtual method you can overload to indicate whether the operator should precede or follow the child
         * element.
         *
         * \return Returns true if the operator should precede.  Returns false if the operator should follow.  The
         *         default implementation always returns true.
         */
        virtual bool shouldPrecede() const;

        /**
         * Method you can overload to provide the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string.  The default implementation returns an inverted question mark.
         */
        virtual QString operatorString(Ld::FormatPointer operatorFormat) const;

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
         * The index to the operator symbol.
         */
        static constexpr unsigned operatorIndex = 0;

        /**
         * The required size for the area.
         */
        QSizeF requiredSize;

        /**
         * The required ascent for the area.
         */
        float requiredAscent;
};

#endif
