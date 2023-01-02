/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CppCodeGeneratorVisual class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CPP_CODE_GENERATOR_VISUAL_H
#define CPP_CODE_GENERATOR_VISUAL_H

#include <QObject>
#include <QSharedPointer>
#include <QSet>
#include <QMutex>

#include <cstdint>

#include <ld_element_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_code_generator_output_type.h>
#include <ld_code_generator_output_type_container.h>
#include <ld_translation_phase.h>
#include <ld_code_generator_visual.h>

#include "app_common.h"

namespace Ld {
    class RootElement;
}

/**
 * This class provides a wrapper on the Ld::CodeGeneratorVisual that pulls the visual into the Qt signal/slot
 * framework.
 *
 * The method also includes an API that can be used by a client to more easily manage coherency.
 */
class APP_PUBLIC_API CppCodeGeneratorVisual:public QObject, public virtual Ld::CodeGeneratorVisual {
    Q_OBJECT

    public:
        /**
         * Pure virtual client base class.  You can derive from this class to obtain an API that simplifies management
         * of the generator by a given client.
         *
         * Note that this class will automatically register itself with the visual if a C++ code generator with a
         * visual of type \ref CppCodeGeneratorVisual.  This class will also unregister itself from the code generator
         * visual at end of scope.
         */
        class Client {
            friend class CppCodeGeneratorVisual;

            public:
                /**
                 * Constructor
                 *
                 * \param[in] alwaysReceiveStatusUpdates If true, then this client will *always* receive status
                 *                                       updates even if it doesn't currently own the generator.  if
                 *                                       false, this client will receive status updates only if it
                 *                                       has ownership of the underlying generator.
                 */
                Client(bool alwaysReceiveStatusUpdates = false);

                virtual ~Client();

                /**
                 * Method you can use to determine if this client will always receive status updates.
                 *
                 * \return Returns true if this client will always receive status updates.  Returns false if this
                 *         client only receives updates if it actively owns the generator.
                 */
                bool alwaysReceivesStatusUpdates() const;

            protected:
                /**
                 * Convenience type used to specify the output export mode.
                 */
                typedef Ld::CodeGeneratorOutputType::ExportMode ExportMode;

                /**
                 * Method you can call to try to claim ownership of the generator.
                 *
                 * \return Returns true if this call successfully claimed the generator.  Returns false if this call
                 *         did not claim the generator.
                 */
                bool tryToOwn();

                /**
                 * Method you can call to wait until you can take ownership of the generator.
                 */
                void waitForOwnership();

                /**
                 * Method you can call to release ownership of the generator.
                 *
                 * \return Returns true on success, returns false on error.
                 */
                bool releaseOwnership();

                /**
                 * Method you can use to start translation.  Note that the translation process will run in a separate
                 * thread.
                 *
                 * \param[in] rootElement The root element you should use to start translation.
                 *
                 * \param[in] outputFile     The name of the translated output file.  If the output type is not
                 *                           exportable, then the translation will be deleted by this class either when
                 *                           a new translation is requested or when the class reaches end of scope.
                 *
                 * \param[in] outputType     A value used to indicate the type of output to be generated.  The meaning
                 *                           of this value is dependent on the type of generator.
                 *
                 * \param[in] exportMode     A value indicating the export mode to be used.
                 *
                 * \param[in] runtimeLibrary A value indicating the runtime library to be used to generate the output.
                 *                           The value is only meaningful on Windows when generating DLLs.  An empty
                 *                           string will indicate that the default runtime library should be used.
                 *
                 * \return Returns true if the translation can be started.  Returns false if either a translation is in
                 *         progress or a translation is being executed.
                 */
                bool translate(
                    QSharedPointer<Ld::RootElement>             rootElement,
                    const QString&                              outputFile,
                    const Ld::CodeGeneratorOutputTypeContainer& outputType,
                    ExportMode                                  exportMode = ExportMode::DEFAULT,
                    const QString&                              runtimeLibrary = QString()
                );

                /**
                 * Method you can use to start translation of the default output type.
                 *
                 * \param[in] rootElement    The root element you should use to start translation.
                 *
                 * \param[in] outputFile     The name of the translated output file.
                 *
                 * \param[in] exportMode     A value indicating the export mode to be used.
                 *
                 * \param[in] runtimeLibrary A value indicating the runtime library to be used to generate the output.
                 *                           The value is only meaningful on Windows when generating DLLs.  An empty
                 *                           string will indicate that the default runtime library should be used.
                 *
                 * \return Returns true on success.  Returns false on failure.
                 */
                bool translate(
                    QSharedPointer<Ld::RootElement> rootElement,
                    const QString&                  outputFile,
                    ExportMode                      exportMode = ExportMode::DEFAULT,
                    const QString&                  runtimeLibrary = QString()
                );

                /**
                 * Method you can use to obtain the intermediate representation.
                 *
                 * \return Returns the last generated intermediate representation.
                 */
                QString intermediateRepresentation() const;

                /**
                 * Method you can call to determine if the code generator is running in the background.
                 *
                 * \return Returns true if the code generator is active.  Returns false if the code generator is not active.
                 */
                bool active() const;

                /**
                 * Method you can call to abort the code generator.  The method will block until the code generator has
                 * aborted.
                 *
                 * \return Returns true on success, returns false if this client does not currently own the generator.
                 */
                bool abort();

                /**
                 * Method that is called when this client receives control of the code generator.
                 */
                virtual void clientOwnsGenerator();

                /**
                 * Method that is called when another client receives control of the code generator.
                 *
                 * \param[in] rootElement The root element the generator is operating on.
                 */
                virtual void generatorBusy();

                /**
                 * Method that is called when the generator becomes idle and can be claimed.
                 */
                virtual void generatorIdle();

                /**
                 * Method that is called when translation has started.
                 *
                 * \param[in] rootElement The root element the generator is operating on.
                 */
                virtual void translationStarted(QSharedPointer<Ld::RootElement> rootElement);

                /**
                 * Method that is called when when the code generator reaches a new translation phase.
                 *
                 * \param[in] rootElement    The root element the generator is operating on.
                 *
                 * \param[in] newPhaseNumber The new phase number.
                 *
                 * \param[in] numberPhases   The number of translation phases.
                 *
                 * \param[in] phaseName      A textual description of the translation phase.
                 */
                virtual void translationPhaseStarted(
                    QSharedPointer<Ld::RootElement> rootElement,
                    unsigned                        newPhaseNumber,
                    unsigned                        numberPhases,
                    const QString&                  phaseName
                );

                /**
                 * Method that is called when the code generator has completed the last translation phase.
                 *
                 * \param[in] rootElement The root element the generator is operating on.
                 *
                 * \param[in] successful  A flag holding true if the translation was successful.
                 */
                virtual void translationCompleted(QSharedPointer<Ld::RootElement> rootElement, bool successful);

                /**
                 * Method that is called when the code generator has aborted a translation process.
                 *
                 * \param[in] rootElement The root element the generator is operating on.
                 */
                virtual void translationAborted(QSharedPointer<Ld::RootElement> rootElement);

                /**
                 * Method that is called each time the code generator uncovers an error.
                 *
                 * \param[in] rootElement The root element the generator is operating on.
                 *
                 * \param[out] diagnostic The diagnostic tied to the error, warning, etc.
                 */
                virtual void translationErrorDetected(
                    QSharedPointer<Ld::RootElement> rootElement,
                    Ld::DiagnosticPointer           diagnostic
                );

            private:
                /**
                 * Visual this client operates with.
                 */
                CppCodeGeneratorVisual* currentVisual;

                /**
                 * Flag that indicates if this client will always receive updates.
                 */
                bool currentAlwaysReceiveUpdates;
        };

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        CppCodeGeneratorVisual(QObject* parent = Q_NULLPTR);

        ~CppCodeGeneratorVisual() override;

        /**
         * Method you can use to register a new client.
         *
         * \param[in] newClient A pointer to the newly added client.
         */
        void addClient(Client* newClient);

        /**
         * Method you can use to remove an client.
         *
         * \param[in] client The client to be removed.
         */
        void removeClient(Client* client);

    signals:
        /**
         * Signal that is emitted when the code generator starts a translation.
         *
         * \param[out] rootElement                      The root element that is used as the basis for the
         *                                              translation.  A null pointer indicates we're processing a
         *                                              selection.
         *
         * \param[out] outputType                       A value used to indicate the type of output being generated.
         *                                              The meaning of this value is dependent on the type of generator.
         *
         * \param[out] numberPerElementTranslationSteps The total number of per-element translation steps that the
         *                                              code generator will need to perform.
         */
        void translationHasStarted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType,
            unsigned long                               numberPerElementTranslationSteps
        );

        /**
         * Signal that is emitted when the code generator begins a new translation phase.
         *
         * \param[out] rootElement The root element you should use to start translation.  A null pointer indicates
         *                         we're processing a selection.
         *
         * \param[out] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                         value is dependent on the type of generator.
         *
         * \param[out] phase       The translation phase being started.
         */
        void translationPhaseHasStarted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType,
            const Ld::TranslationPhase&                 phase
        );

        /**
         * Signal that is emitted when the code generator completes a translation process.
         *
         * \param[out] success     Holds true if the translation was successful.  Holds false if the translation
         *                         process failed.
         *
         * \param[out] rootElement The root element that is used as the basis for the translation.  A null pointer
         *                         indicates we're processing a selection.
         *
         * \param[out] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                         value is dependent on the type of generator.
         */
        void translationHasCompleted(
            bool                                        success,
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType
        );

        /**
         * Signal that is emitted when the code generator is aborted during a translation.
         *
         * \param[out] rootElement The root element that is used as the basis for the translation.  A null pointer
         *                         indicates we're processing a selection.
         *
         * \param[out] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                         value is dependent on the type of generator.
         */
        void translationWasAborted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType
        );

        /**
         * Signal that is emitted each time the code generator detects an error condition.  This method may be
         * triggered multiple times during a translation and may be triggered from a different thread from that used
         * to invoke the code generator.
         *
         * \param[out] diagnostic The diagnostic tied to the error, warning, etc.
         */
        void translationErrorWasDetected(Ld::DiagnosticPointer diagnostic);

    private slots:
        /**
         * Slot that is triggered when the code generator starts a translation.
         *
         * \param[in] rootElement                      The root element that is used as the basis for the
         *                                             translation.  A null pointer indicates we're processing a
         *                                             selection.
         *
         * \param[in] outputType                       A value used to indicate the type of output being generated.
         *                                             The meaning of this value is dependent on the type of generator.
         *
         * \param[in] numberPerElementTranslationSteps The total number of per-element translation steps that the
         *                                             code generator will need to perform.
         */
        void processTranslationStarted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType,
            unsigned long                               numberPerElementTranslationSteps
        );

        /**
         * Slot that is triggered when the code generator begins a new translation phase.
         *
         * \param[in] rootElement The root element you should use to start translation.  A null pointer indicates
         *                        we're processing a selection.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         *
         * \param[in] phase       The translation phase being started.
         */
        void processTranslationPhaseStarted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType,
            const Ld::TranslationPhase&                 phase
        );

        /**
         * Slot that is triggered when the code generator completes a translation process.
         *
         * \param[in] success     Holds true if the translation was successful.  Holds false if the translation
         *                        process failed.
         *
         * \param[in] rootElement The root element that is used as the basis for the translation.  A null pointer
         *                        indicates we're processing a selection.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         */
        void processTranslationCompleted(
            bool                                        success,
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType
        );

        /**
         * Slot that is triggered when the code generator is aborted during a translation.
         *
         * \param[in] rootElement The root element that is used as the basis for the translation.  A null pointer
         *                        indicates we're processing a selection.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         */
        void processTranslationAborted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType
        );

        /**
         * Slot that is triggered each time the code generator detects an error condition.  This method may be
         * triggered multiple times during a translation and may be triggered from a different thread from that used
         * to invoke the code generator.
         *
         * \param[in] diagnostic The diagnostic tied to the error, warning, etc.
         */
        void processTranslationErrorDetected(Ld::DiagnosticPointer diagnostic);

    private:
        /**
         * Method you can call to try to claim ownership of the generator.
         *
         * \param[in] client The client making the request.
         *
         * \return Returns true if this call successfully claimed the generator.  Returns false if this call
         *         did not claim the generator.
         */
        bool tryToOwn(Client* client);

        /**
         * Method you can call to wait until you can take ownership of the generator.
         *
         * \param[in] client The client making the request.
         */
        void waitForOwnership(Client* client);

        /**
         * Method you can call to release ownership of the generator.
         *
         * \param[in] client The client making the request.
         *
         * \return Returns true on success, returns false on error.
         */
        bool releaseOwnership(Client* client);

        /**
         * Method you can use to obtain the intermediate representation.
         *
         * \return Returns the last generated intermediate representation.
         */
        QString intermediateRepresentation() const;

        /**
         * Method that is called when the code generator starts a translation.
         *
         * \param[in] rootElement                      The root element that is used as the basis for the
         *                                             translation.  A null pointer indicates we're processing a
         *                                             selection.
         *
         * \param[in] outputType                       A value used to indicate the type of output being generated.
         *                                             The meaning of this value is dependent on the type of generator.
         *
         * \param[in] numberPerElementTranslationSteps The total number of per-element translation steps that the
         *                                             code generator will need to perform.
         */
        void translationStarted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType,
            unsigned long                               numberPerElementTranslationSteps
        ) final;

        /**
         * Method that is called when the code generator begins a new translation phase.
         *
         * \param[in] rootElement The root element you should use to start translation.  A null pointer indicates
         *                        we're processing a selection.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         *
         * \param[in] phase       The translation phase being started.
         */
        void translationPhaseStarted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType,
            const Ld::TranslationPhase&                 phase
        ) final;

        /**
         * Method that is called when the code generator completes a translation process.
         *
         * \param[in] success     Holds true if the translation was successful.  Holds false if the translation
         *                        process failed.
         *
         * \param[in] rootElement The root element that is used as the basis for the translation.  A null pointer
         *                        indicates we're processing a selection.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         */
        void translationCompleted(
            bool                                        success,
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType
        ) final;

        /**
         * Method that is called when the code generator is aborted during a translation.
         *
         * \param[in] rootElement The root element that is used as the basis for the translation.  A null pointer
         *                        indicates we're processing a selection.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         */
        void translationAborted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType
        ) final;

        /**
         * Method that is called each time the code generator detects an error condition.  This method may be
         * triggered multiple times during a translation and may be triggered from a different thread from that used
         * to invoke the code generator.
         *
         * \param[in] diagnostic The diagnostic tied to the error, warning, etc.
         */
        void translationErrorDetected(Ld::DiagnosticPointer diagnostic) final;

        /**
         * Method that is called by the \ref CppCodeGeneratorVisual::Client class to start a translation.  Note that
         * the translation process will run in a separate thread.
         *
         * \param[in] client         The client making the request.
         *
         * \param[in] rootElement    The root element you should use to start translation.
         *
         * \param[in] outputFile     The name of the translated output file.  If the output type is not exportable,
         *                           then the translation will be deleted by this class either when a new translation
         *                           is requested or when the class reaches end of scope.
         *
         * \param[in] outputType     A value used to indicate the type of output to be generated.  The meaning of this
         *                           value is dependent on the type of generator.
         *
         * \param[in] exportMode     A value indicating the export mode to be used.
         *
         * \param[in] runtimeLibrary A value indicating the runtime library to be used to generate the output.
         *                           The value is only meaningful on Windows when generating DLLs.  An empty
         *                           string will indicate that the default runtime library should be used.
         *
         * \return Returns true if the translation can be started.  Returns false if either a translation is in
         *         progress or a translation is being executed.
         */
        bool translate(
            Client*                                     client,
            QSharedPointer<Ld::RootElement>             rootElement,
            const QString&                              outputFile,
            const Ld::CodeGeneratorOutputTypeContainer& outputType,
            Client::ExportMode                          exportMode,
            const QString&                              runtimeLibrary
        );

        /**
         * Method that is called by the \ref CppCodeGeneratorVisual::Client class to start a translation using the
         * default output type.  Note that the translation process will run in a separate thread.
         *
         * \param[in] client         The client making the request.
         *
         * \param[in] rootElement    The root element you should use to start translation.
         *
         * \param[in] outputFile     The name of the translated output file.
         *
         * \param[in] exportMode     A value indicating the export mode to be used.
         *
         * \param[in] runtimeLibrary A value indicating the runtime library to be used to generate the output.
         *                           The value is only meaningful on Windows when generating DLLs.  An empty
         *                           string will indicate that the default runtime library should be used.
         *
         * \return Returns true on success.  Returns false on failure.
         */
        bool translate(
            Client*                         client,
            QSharedPointer<Ld::RootElement> rootElement,
            const QString&                  outputFile,
            Client::ExportMode              exportMode,
            const QString&                  runtimeLibrary
        );

        /**
         * Method that is called by the \ref CppCodeGeneratorVisual::Client class to determine if the code generator
         * is running in the background.
         *
         * \return Returns true if the code generator is active.  Returns false if the code generator is not active.
         */
        bool active() const;

        /**
         * Method that is called by the \ref CppCodeGeneratorVisual::Client class to abort the code generator.  The
         * method will block until the code generator has aborted.  This will also abort an executing translation.
         *
         * \param[in] client The client making the request.
         *
         * \return Returns true on success, returns false if this client does not currently own the generator.
         */
        bool abort(Client* client);

        /**
         * Method that updates the owner status for all the clients.
         */
        void updateOwnerStatus();

        /**
         * Mutex used to gate access to shared structures.
         */
        QMutex accessMutex;

        /**
         * Set containing every known client.
         */
        QSet<Client*> currentClients;

        /**
         * The client that currently owns the generator.
         */
        Client* currentClient;

        /**
         * The root element we are translating and possibly executing.
         */
        QSharedPointer<Ld::RootElement> currentRootElement;

        /**
         * Holds the translation start time.
         */
        std::uint64_t translateStartTime;

        /**
         * Holds the last phase start time.
         */
        std::uint64_t translatePhaseStartTime;
};

#endif
