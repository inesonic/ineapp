/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BooleanDataTypePresentationGenerator class.
***********************************************************************************************************************/

#include <QString>

#include <model_variant.h>

#include <ld_format_structures.h>

#include "simple_text_data_type_presentation_generator_base.h"
#include "boolean_data_type_presentation_generator.h"

const BooleanDataTypePresentationGenerator BooleanDataTypePresentationGenerator::instance;

BooleanDataTypePresentationGenerator::BooleanDataTypePresentationGenerator() {}


BooleanDataTypePresentationGenerator::~BooleanDataTypePresentationGenerator() {}


QString BooleanDataTypePresentationGenerator::toDisplayedString(
        const Model::Variant& value,
        Ld::FormatPointer     format
    ) const {
    return toString(value, format);
}
