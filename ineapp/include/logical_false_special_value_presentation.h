/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LogicalFalseSpecialValuePresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LOGICAL_FALSE_SPECIAL_VALUE_PRESENTATION_H
#define LOGICAL_FALSE_SPECIAL_VALUE_PRESENTATION_H

#include <QObject>
#include <QList>

#include <ld_diagnostic_structures.h>
#include <ld_element_cursor.h>
#include <ld_logical_false_special_value_visual.h>

#include "app_common.h"
#include "special_symbol_presentation_base.h"

class QGraphicsItem;

/**
 * This class provides a presentation for the Ld::LogicalFalseSpecialValueElement class.
 */
class APP_PUBLIC_API LogicalFalseSpecialValuePresentation:public         SpecialSymbolPresentationBase,
                                                          public virtual Ld::LogicalFalseSpecialValueVisual {
    Q_OBJECT

    public:
        LogicalFalseSpecialValuePresentation();

        ~LogicalFalseSpecialValuePresentation() override;

        /**
         * Static method that creates an instance of a \ref LogicalFalseSpecialValuePresentation class.
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

        /**
         * Method you can overload to obtain the font used to render the special characters.  This method is called by
         * \ref SpecialSymbolPresentationBase::buildGraphicsItem
         *
         * \return Returns the font used to render the mathematical symbols.
         */
        QFont specialCharactersFont() const override;
};

#endif
