/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlaceholderPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEHOLDER_PRESENTATION_H
#define PLACEHOLDER_PRESENTATION_H

#include <QObject>
#include <QList>

#include <ld_diagnostic_structures.h>
#include <ld_text_element.h>
#include <ld_element_cursor.h>
#include <ld_visual.h>
#include <ld_placeholder_visual.h>
#include <ld_character_format.h>

#include "app_common.h"
#include "special_symbol_presentation_base.h"

class QGraphicsItem;

/**
 * This class provides a visual for the Ld::PlaceholderElement class.
 */
class APP_PUBLIC_API PlaceholderPresentation:public         SpecialSymbolPresentationBase,
                                             public virtual Ld::PlaceholderVisual {
    Q_OBJECT

    public:
        PlaceholderPresentation();

        ~PlaceholderPresentation() override;

        /**
         * Static method that creates an instance of a \ref PlaceholderPresentation class.
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
         * Method you can overload to obtain the color to apply to the special characters.
         *
         * \return Returns the text color.  An invalid color will cause the text to be rendered as black.  The default
         *         implementation return the color reported by the underlying element format.  If the font color
         *         could not be determined, the method returns an invalid color.
         */
        QColor specialCharactersColor() const override;

        /**
         * Method you can overload to obtain the background color to apply to the special characters.
         *
         * \return Returns the text color.  An invalid color will cause the text to be rendered as black.  The default
         *         implementation return the background color reported by the underlying element format.  If the font
         *         background color could not be determined, the method returns an invalid color.
         */
        QColor specialCharactersBackgroundColor() const override;
};

#endif
