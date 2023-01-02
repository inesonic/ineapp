/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HadamardProductOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HADAMARD_PRODUCT_OPERATOR_PRESENTATION_H
#define HADAMARD_PRODUCT_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_hadamard_product_operator_visual.h>

#include "app_common.h"
#include "multiplication_operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::HadamardProductOperatorElement class.
 */
class APP_PUBLIC_API HadamardProductOperatorPresentation:public         MultiplicationOperatorPresentationBase,
                                                         public virtual Ld::HadamardProductOperatorVisual {
    Q_OBJECT

    public:
        HadamardProductOperatorPresentation();

        ~HadamardProductOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref HadamardProductOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Method that returns the default symbol for this operation.
         *
         * \return Returns the default symbol for this operation.
         */
        Ld::MultiplicationOperatorFormat::MultiplicationSymbol defaultSymbol() const override;
};

#endif
