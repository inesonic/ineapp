/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DivisionOperatorPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DIVISION_OPERATOR_PRESENTATION_H
#define DIVISION_OPERATOR_PRESENTATION_H

#include <QObject>
#include <QString>

#include <ld_format_structures.h>
#include <ld_division_operator_visual.h>

#include "app_common.h"
#include "division_operator_presentation_base.h"

class QGraphicsItem;

namespace Ld {
    class Visual;
}

/**
 * This class provides a presentation for the Ld::DivisionOperatorElement class.
 */
class APP_PUBLIC_API DivisionOperatorPresentation:public DivisionOperatorPresentationBase {
    Q_OBJECT

    public:
        DivisionOperatorPresentation();

        ~DivisionOperatorPresentation() override;

        /**
         * Static method that creates an instance of a \ref DivisionOperatorPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Method you can overload to specify the default format for this division operation.
         *
         * \return Returns the default division format to use for this division.
         */
        Ld::DivisionOperatorFormat::DivisionSymbol defaultDivisionSymbol() const override;
};

#endif
