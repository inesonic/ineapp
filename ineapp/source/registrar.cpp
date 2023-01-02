/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Registrar class.
***********************************************************************************************************************/

#include <QString>
#include <QSharedPointer>

#include <eqt_builder.h>

#include "ld_format.h"
#include "ld_document_setting.h"
#include "ld_element.h"
#include "ld_visual.h"
#include "ld_data_type.h"
#include "ld_data_type_translator.h"
#include "ld_data_type_decoder.h"
#include "ld_code_generator.h"
#include "ld_cpp_declaration_payload.h"
#include "ld_cpp_library_information.h"
#include "ld_cpp_library_dependencies.h"
#include "ld_function_translation_engine.h"
#include "ld_variable_name.h"
#include "ld_function_data.h"
#include "ld_function_database.h"

#include "application.h"
#include "fixer.h"
#include "plug_in_data.h"
#include "registrar.h"

Registrar::Registrar() {
    currentlySuccessful = true;
}


Registrar::~Registrar() {}



void Registrar::startConfiguration(const QString& plugInName) {
    currentPlugInFilename = plugInName;
    currentlySuccessful   = true;
}


void Registrar::endConfiguration(const QString& /* plugInName */) {
    currentPlugInFilename.clear();
}


bool Registrar::successful() const {
    return currentlySuccessful;
}


void Registrar::registerFormat(const QString& formatName, Ld::Format::CreatorFunction creatorFunction) {
    if (!Ld::Format::registerCreator(formatName, creatorFunction)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerDocumentSetting(
        const QString&                       documentSettingName,
        Ld::DocumentSetting::CreatorFunction creatorFunction
    ) {
    if (!Ld::DocumentSetting::registerCreator(documentSettingName, creatorFunction)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerElement(const QString& elementName, Ld::Element::CreatorFunction creatorFunction) {
    if (!Ld::Element::registerCreator(elementName, creatorFunction)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerVisual(
        const QString&              elementName,
        Ld::Visual::CreatorFunction creatorFunction,
        bool                        overwriteExisting
    ) {
    if (!Ld::Visual::registerCreator(elementName, creatorFunction, overwriteExisting) && !overwriteExisting) {
        currentlySuccessful = false;
    }

}

void Registrar::registerDataType(
        Ld::DataType::ValueType valueType,
        const QString&          description,
        const QString&          typeSymbolElementName,
        Ld::DataType::Property  properties
    ) {
    if (!Ld::DataType::registerType(valueType, description, typeSymbolElementName, properties)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerDataTypeTranslator(
        const QString&                generatorName,
        Ld::DataType::ValueType       valueType,
        const Ld::DataTypeTranslator* newTranslator
    ) {
    if (!Ld::DataType::registerTranslator(generatorName, valueType, newTranslator)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerDataTypeDecoder(Ld::DataType::ValueType valueType, const Ld::DataTypeDecoder* newDecoder) {
    if (!Ld::DataType::registerDecoder(valueType, newDecoder)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerCodeGenerator(QSharedPointer<Ld::CodeGenerator> newCodeGenerator) {
    if (!Ld::CodeGenerator::registerCodeGenerator(newCodeGenerator)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerCodeGenerator(Ld::CodeGenerator* newCodeGenerator) {
    if (!Ld::CodeGenerator::registerCodeGenerator(newCodeGenerator)) {
        currentlySuccessful = false;
    }
}


QSharedPointer<Ld::CodeGenerator> Registrar::codeGenerator(const QString& codeGeneratorName) {
    return Ld::CodeGenerator::codeGenerator(codeGeneratorName);
}


void Registrar::registerDefaultTranslator(
        const QString&                       generatorName,
        const Ld::FunctionTranslationEngine* defaultTranslator
    ) {
    if (!Ld::FunctionData::registerDefaultTranslator(generatorName, defaultTranslator)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerCppDeclarationPayload(
        const QString&       library,
        const unsigned char* payload,
        unsigned long        length,
        bool                 compressed
    ) {
    if (!Ld::CppDeclarationPayload::registerPayload(Ld::CppDeclarationPayload(library, payload, length, compressed))) {
        currentlySuccessful = false;
    }
}


void Registrar::registerCppLibraryDependencies(
        const QString&                          libraryName,
        const QList<Ld::CppLibraryInformation>& dependencies
    ) {
    if (!Ld::CppLibraryDependencies::registerDependencies(Ld::CppLibraryDependencies(libraryName, dependencies))) {
        currentlySuccessful = false;
    }
}


void Registrar::registerFixer(const QString& elementName, const Fixer& provider) {
    if (!Fixer::registerFixer(elementName, provider)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerFixer(const QString& elementName, const Fixer* provider) {
    if (!Fixer::registerFixer(elementName, provider)) {
        currentlySuccessful = false;
    }
}


void Registrar::registerFunction(const Ld::FunctionData& functionData) {
    if (!Ld::FunctionDatabase::registerFunction(functionData)) {
        currentlySuccessful = false;
    }
}


Ld::FunctionData& Registrar::function(const QString& internalName) {
    return Ld::FunctionDatabase::function(internalName);
}


Ld::FunctionData& Registrar::function(const Ld::VariableName& variableName) {
    return Ld::FunctionDatabase::function(variableName);
}


void Registrar::registerBuilder(EQt::Builder* newBuilder) {
    Application::instance()->addBuilder(newBuilder);
}


void Registrar::registerInspectorWidget(
        Ld::DataType::ValueType          valueType,
        InspectorWidget::CreatorFunction creatorFunction
    ) {
    if (!InspectorWidget::registerCreator(valueType, creatorFunction)) {
        currentlySuccessful = false;
    }
}
