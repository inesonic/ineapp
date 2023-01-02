/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref InfinitySpecialValuePresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef INFINITY_SPECIAL_VALUE_PRESENTATION_H
#define INFINITY_SPECIAL_VALUE_PRESENTATION_H

#include <QObject>
#include <QList>

#include <ld_diagnostic_structures.h>
#include <ld_element_cursor.h>
#include <ld_infinity_special_value_visual.h>

#include "app_common.h"
#include "special_symbol_presentation_base.h"

class QGraphicsItem;

/**
 * This class provides a presentation for the Ld::InfinitySpecialValueElement class.
 */
class APP_PUBLIC_API InfinitySpecialValuePresentation:public         SpecialSymbolPresentationBase,
                                                      public virtual Ld::InfinitySpecialValueVisual {
    Q_OBJECT

    public:
        InfinitySpecialValuePresentation();

        ~InfinitySpecialValuePresentation() override;

        /**
         * Static method that creates an instance of a \ref InfinitySpecialValuePresentation class.
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
