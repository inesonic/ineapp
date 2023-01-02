/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MultiplicationOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MULTIPLICATION_OPERATOR_PRESENTATION_H
#define MULTIPLICATION_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_multiplication_operator_format.h>

#include "app_common.h"
#include "multiplication_operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::MultiplicationOperatorElement class.
 */
class APP_PUBLIC_API MultiplicationOperatorPresentation:public         MultiplicationOperatorPresentationBase,
                                                        public virtual Ld::MultiplicationOperatorVisual {
    Q_OBJECT

    public:
        MultiplicationOperatorPresentation();

        ~MultiplicationOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref MultiplicationOperatorPresentation class.
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
