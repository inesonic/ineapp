/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Registrar class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef REGISTRAR_H
#define REGISTRAR_H

#include <QString>
#include <QSharedPointer>

#include "ld_format.h"
#include "ld_document_setting.h"
#include "ld_element.h"
#include "ld_visual.h"
#include "ld_data_type.h"
#include "ld_cpp_library_information.h"

#include "app_common.h"
#include "inspector_widget.h"
#include "plug_in_registrar.h"

namespace EQt {
    class Builder;
}

namespace Ld {
    class DataTypeTranslator;
    class DataTypeDecoder;
    class CodeGenerator;
    class FunctionTranslationEngine;
    class FunctionData;
    class FunctionDatabase;
    class VariableName;
}

class Fixer;

/**
 * Application's implementation of the \ref PlugInRegistrar and Ld::PlugInRegistrar classes.
 */
class APP_PUBLIC_API Registrar:public PlugInRegistrar {
    public:
        Registrar();

        virtual ~Registrar();

        /**
         * Method you can call to indicate that we're about to configure a new plug-in.
         *
         * \param[in] plugInName The name of the plug-in we're about to load.
         */
        void startConfiguration(const QString& plugInName) override;

        /**
         * Method you can call to indicate that we've finished configuring a plug-in.
         *
         * \param[in] plugInName The name of the plug-in we've finished configuring.
         */
        void endConfiguration(const QString& plugInName) override;

        /**
         * Method you can call to determine if the configuration was successful.
         *
         * \return Returns true if the configuration was successful.  Returns false if the configuration failed.
         */
        bool successful() const override;

        /**
         * Method you can call to register a new format instance.
         *
         * \param[in] formatName      The name of the format class.
         *
         * \param[in] creatorFunction The format creator function.
         */
        void registerFormat(const QString& formatName, Ld::Format::CreatorFunction creatorFunction) override;

        /**
         * Method you can call to register a new document setting type.
         *
         * \param[in] documentSettingsName The name used to reference this document setting.
         *
         * \param[in] creatorFunction      Pointer to the function that should be called to create a default instance
         *                                 of the correct object type.
         */
        void registerDocumentSetting(
            const QString&                       documentSettingsName,
            Ld::DocumentSetting::CreatorFunction creatorFunction
        ) override;

        /**
         * Method you can call to register a new element type.
         *
         * \param[in] elementName     The name used to reference this element type.
         *
         * \param[in] creatorFunction Pointer to the function that should be called to create a default instance of
         *                            the correct object type.
         */
        void registerElement(const QString& elementName, Ld::Element::CreatorFunction creatorFunction) override;

        /**
         * Method you can use to register a new visual.
         *
         * \param[in] elementName       The name used to reference this visual type.
         *
         * \param[in] creatorFunction   Pointer to the function that should be called to create the visual.
         *
         * \param[in] overwriteExisting If true, any existing creatir method will be overwritten with a new entry.
         *                              If false, this method will fail (return false) and the existing entry will
         *                              not be overwritten.
         */
        void registerVisual(
            const QString&              elementName,
            Ld::Visual::CreatorFunction creatorFunction,
            bool                        overwriteExisting = false
        ) override;

        /**
         * Method you can call to register a new data type.
         *
         * \param[in] valueType             The Ld::DataType::ValueType this data type represents.
         *
         * \param[in] description           A description of this data type.
         *
         * \param[in] typeSymbolElementName The name of the symbol used to define this data-type.  An empty string
         *                                  can be provided if there is no element representing this data type.
         *
         * \param[in] properties            Properties to apply to the data type.
         */
        void registerDataType(
            Ld::DataType::ValueType valueType,
            const QString&          description,
            const QString&          typeSymbolElementName = QString(),
            Ld::DataType::Property  properties = Ld::DataType::defaultProperties
        ) override;

        /**
         * Method you can use to register a new data type translator to this data type.
         *
         * \param[in] generatorName The name of the generator using this translator.
         *
         * \param[in] valueType     The value type this translator works with.
         *
         * \param[in] newTranslator The new data-type translator.  The translator is expected to be defined as a
         *                          static instance variable and will *not* be deleted.
         */
        void registerDataTypeTranslator(
            const QString&                generatorName,
            Ld::DataType::ValueType       valueType,
            const Ld::DataTypeTranslator* newTranslator
        ) override;

        /**
         * Method you can use to register a data type decoder to a data type.  Note that there can be only one
         * decoder tied to each data-type.
         *
         * \param[in] valueType  The value type this translator works with.
         *
         * \param[in] newDecoder The new data-type translator.  Data type decoders should be defined consistently
         *                       as static instance variables or on the heap.  If defined on the heap, be sure to
         *                       call the Ld::DataType::destroyDecoders method when the program terminates to destroy
         *                       the decoders.
         */
        void registerDataTypeDecoder(
            Ld::DataType::ValueType    valueType,
            const Ld::DataTypeDecoder* newDecoder
        ) override;

        /**
         * Method you can use to register a new code generator.
         *
         * \param[in] newCodeGenerator The code generator to be added to the system.
         */
        void registerCodeGenerator(QSharedPointer<Ld::CodeGenerator> newCodeGenerator) override;

        /**
         * Convenience method you can use to register a new code generator.
         *
         * \param[in] newCodeGenerator The code generator to be added to the system.  The system will take
         *                             ownership of the code generator.
         */
        void registerCodeGenerator(Ld::CodeGenerator* newCodeGenerator) override;

        /**
         * Method you can use to obtain an existing code generator, by name.
         *
         * \param[in] codeGeneratorName The name of the desired code generator.
         *
         * \return Returns a shared pointer to the desired code generator.  A null pointer is returned if the
         *         requested code generator does not already exist.
         */
        QSharedPointer<Ld::CodeGenerator> codeGenerator(const QString& codeGeneratorName) override;

        /**
         * Method you can use to register a default function translator for a translation engine.
         *
         * \param[in] generatorName     The name of the generator.
         *
         * \param[in] defaultTranslator A pointer to the default translator.  This translator is expected to be
         *                              statically defined and will not be deleted.
         */
        void registerDefaultTranslator(
            const QString&                       generatorName,
            const Ld::FunctionTranslationEngine* defaultTranslator
        ) override;

        /**
         * Method you can use to register a new C++ declaration payload with the system.
         *
         * \param[in] library    The library tied to this payload.
         *
         * \param[in] payload    The payload.  The payload is expected to remain resident during the entire
         *                       lifetime of the application.
         *
         * \param[in] length     The length of the payload, in bytes.
         *
         * \param[in] compressed If true, the C++ declaration payload has been compressed.  If false, the C++
         *                       declaration payload has not been compressed.
         */
        void registerCppDeclarationPayload(
            const QString&       library,
            const unsigned char* payload,
            unsigned long        length,
            bool                 compressed = false
        ) override;

        /**
         * Method you can use to register a dependency between libraries.
         *
         * \param[in] libraryName  The name of the library we have dependencies to.  The name should exclude the
         *                         path, any prepended "lib" and the extension.
         *
         * \param[in] dependencies A list of dependent libraries.
         */
        void registerCppLibraryDependencies(
            const QString&                          libraryName,
            const QList<Ld::CppLibraryInformation>& dependencies
        ) override;

        /**
         * Method you can use to register a new fixer instance.
         *
         * \param[in] elementName The element type name that this fixer should be used with.
         *
         * \param[in] provider    The provider that should be used to manipulate the element.
         */
        void registerFixer(const QString& elementName, const Fixer& provider);

        /**
         * Static method you can use to register a new default child provider with the system.
         *
         * \param[in] elementName The element type name that this provider should be used with.
         *
         * \param[in] provider    The provider that should be used to manipulate the element.
         */
        void registerFixer(const QString& elementName, const Fixer* provider);

        /**
         * Method you can call to register a new persistent function.
         *
         * \param[in] functionData The new function data instance.
         */
        void registerFunction(const Ld::FunctionData& functionData) override;

        /**
         * Method you can call to obtain a persistent function by internal name.  You can use this method to add
         * new variants to a persistent function.
         *
         * \param[in] internalName The internal name for the function.
         *
         * \return Returns the persistent function data instance associated with the given internal name.
         */
        Ld::FunctionData& function(const QString& internalName) override;

        /**
         * Method you can call to obtain a persistent function by internal name.  You can use this method to add
         * new variants to a persistent function.
         *
         * \param[in] variableName The variable name for the function.
         *
         * \return Returns the persistent function data instance associated with the given internal name.
         */
        Ld::FunctionData& function(const Ld::VariableName& variableName) override;

        /**
         * Pure virtual method you can call to register a builder with the application.  The builder will be used by
         * the application customize the user interface for this plug-in.
         *
         * Note that the application will take ownership of the builder and will destroy it automatically when the
         * application terminates.
         *
         * \param[in] newBuilder The builder instance to be used.
         */
        void registerBuilder(EQt::Builder* newBuilder) override;

        /**
         * Method you can use to register new inspector widget factory function.
         *
         * \param[in] valueType       The value type to associate with this factory function.
         *
         * \param[in] creatorFunction The factory function.
         */
        void registerInspectorWidget(
            Ld::DataType::ValueType          valueType,
            InspectorWidget::CreatorFunction creatorFunction
        ) override;

    private:
        /**
         * Flag indicating success or failure while processing the current plug-in.
         */
        bool currentlySuccessful;

        /**
         * String holding the filename of the currently loaded plug-in.
         */
        QString currentPlugInFilename;
};

#endif
