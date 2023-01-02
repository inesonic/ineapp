/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Ld::HtmlSelectionDataTranslator class.
***********************************************************************************************************************/

#include <QString>
#include <QSharedPointer>

#include <ld_code_generation_engine.h>
#include <ld_html_code_generation_engine.h>
#include <ld_html_translator.h>

#include "selection_data.h"
#include "html_selection_data_translator.h"

HtmlSelectionDataTranslator::~HtmlSelectionDataTranslator() {}


QString HtmlSelectionDataTranslator::elementName() const {
    return SelectionData::elementName;
}


bool HtmlSelectionDataTranslator::body(Ld::ElementPointer element, Ld::HtmlCodeGenerationEngine& engine) {
    return translateAllChildren(element, engine);
}
