/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LaTeXSelectionDataTranslator class.
***********************************************************************************************************************/

#include <QString>

#include <ld_element.h>
#include <ld_element_structures.h>
#include <ld_root_element.h>
#include <ld_latex_code_generation_engine.h>
#include <ld_latex_translator.h>

#include "selection_data.h"
#include "latex_selection_data_translator.h"

LaTeXSelectionDataTranslator::~LaTeXSelectionDataTranslator() {}


QString LaTeXSelectionDataTranslator::elementName() const {
    return SelectionData::elementName;
}


bool LaTeXSelectionDataTranslator::body(Ld::ElementPointer element, Ld::LaTeXCodeGenerationEngine& engine) {
    return translateAllChildren(element, engine);
}
