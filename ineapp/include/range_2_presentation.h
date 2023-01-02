/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Range2Presentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef RANGE_2_PRESENTATION_H
#define RANGE_2_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_range_2_visual.h>

#include "app_common.h"
#include "binary_operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::Range2Element class.
 */
class APP_PUBLIC_API Range2Presentation:public BinaryOperatorPresentationBase, public virtual Ld::Range2Visual {
    Q_OBJECT

    public:
        Range2Presentation();

        ~Range2Presentation() override;

        /**
         * Static method that creates an instance of a \ref Range2Presentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Method you can overload to provide the operator string.
         *
         * \param[in] operatorFormat The format instance tied to this operator.
         *
         * \return Returns the operator as a string
         */
        QString operatorString(Ld::FormatPointer operatorFormat) const override;
};

#endif
