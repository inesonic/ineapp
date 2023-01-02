/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PiSpecialValuePresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PI_SPECIAL_VALUE_PRESENTATION_H
#define PI_SPECIAL_VALUE_PRESENTATION_H

#include <QObject>
#include <QList>

#include <ld_diagnostic_structures.h>
#include <ld_element_cursor.h>
#include <ld_pi_special_value_visual.h>

#include "app_common.h"
#include "special_symbol_presentation_base.h"

class QGraphicsItem;

/**
 * This class provides a presentation for the Ld::PiSpecialValueElement class.
 */
class APP_PUBLIC_API PiSpecialValuePresentation:public         SpecialSymbolPresentationBase,
                                                public virtual Ld::PiSpecialValueVisual {
    Q_OBJECT

    public:
        PiSpecialValuePresentation();

        ~PiSpecialValuePresentation() override;

        /**
         * Static method that creates an instance of a \ref PiSpecialValuePresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

    protected:
        /**
         * Method you can overload to obtain a string representation of the desired mathematical symbol.
         *
         * \return Returns the mathematical symbol as a string.  The default implementation returns an inverted question
         *         mark.
         */
        QString specialCharacters() const override;
};

#endif
