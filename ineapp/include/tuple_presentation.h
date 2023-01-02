/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TuplePresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TUPLE_PRESENTATION_H
#define TUPLE_PRESENTATION_H

#include <QObject>
#include <QList>

#include <eqt_graphics_math_group.h>

#include <ld_visual.h>
#include <ld_tuple_visual.h>

#include "app_common.h"
#include "presentation_area_tracker.h"
#include "presentation_with_positional_children.h"
#include "list_presentation_base.h"

class QGraphicsItem;
class QFontMetricsF;

/**
 * Class that supports tuples containing zero or more values.
 */
class APP_PUBLIC_API TuplePresentation:public virtual ListPresentationBase, public virtual Ld::TupleVisual {
    Q_OBJECT

    public:
        TuplePresentation();

        ~TuplePresentation() override;

        /**
         * Static method that creates an instance of a \ref AdditionOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Method you can overload to determine the parenthesis style to use for this list.
         *
         * \return Returns the parenthesis style you should display for this list style.
         */
        EQt::GraphicsMathGroup::ParenthesisStyle parenthesisStyle() const override;
};

#endif
