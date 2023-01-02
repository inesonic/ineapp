/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LaTeXSelectionDataTranslator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LATEX_SELECTION_DATA_TRANSLATOR_H
#define LATEX_SELECTION_DATA_TRANSLATOR_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QString>

#include <ld_element_structures.h>
#include <ld_latex_translator.h>

#include "app_common.h"

namespace Ld {
    class LaTeXCodeGenerationEngine;
}

/**
 * LaTeX translator for the root element.
 */
class APP_PUBLIC_API LaTeXSelectionDataTranslator:public Ld::LaTeXTranslator {
    Q_DECLARE_TR_FUNCTIONS(Ld::LaTeXSelectionDataTranslator)

    public:
        ~LaTeXSelectionDataTranslator() override;

        /**
         * Method that returns the name of the element this translator operates on.
         *
         * \return Returns the name of the element this translator operates on.
         */
        QString elementName() const final;

        /**
         * Method that is called to insert the LaTeX body content.
         *
         * \param[in,out] element          A pointer to the element to be translated.
         *
         * \param[in,out] generationEngine The generation engine driving the conversion.
         *
         * \return Returns true on success, returns false on error.
         */
        bool body(Ld::ElementPointer element, Ld::LaTeXCodeGenerationEngine& generationEngine) final;
};


#endif
