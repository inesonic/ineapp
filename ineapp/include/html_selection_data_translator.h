/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HtmlSelectionDataTranslator class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HTML_SELECTION_DATA_TRANSLATOR_H
#define HTML_SELECTION_DATA_TRANSLATOR_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QString>
#include <QSharedPointer>

#include <ld_element_structures.h>
#include <ld_html_translator.h>

#include "app_common.h"

namespace Ld {
    class HtmlCodeGenerationEngine;
}

/**
 * HTML translator class for the root element.
 */
class APP_PUBLIC_API HtmlSelectionDataTranslator:public Ld::HtmlTranslator {
    Q_DECLARE_TR_FUNCTIONS(Ld::HtmlSelectionDataTranslator)

    public:
        ~HtmlSelectionDataTranslator() override;

        /**
         * Method that returns the name of the element this translator operates on.
         *
         * \return Returns the name of the element this translator operates on.
         */
        QString elementName() const final;

        /**
         * Method that is called to insert body content.
         *
         * \param[in,out] element          A pointer to the element to be translated.
         *
         * \param[in,out] generationEngine The generation engine driving the conversion.
         *
         * \return Returns true on success, returns false on error.
         */
        bool body(Ld::ElementPointer element, Ld::HtmlCodeGenerationEngine& generationEngine) final;
};

#endif
