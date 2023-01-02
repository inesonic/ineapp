/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RunExportViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef RUN_EXPORT_VIEW_PROXY_H
#define RUN_EXPORT_VIEW_PROXY_H

#include <QObject>

#include <model_status.h>

#include "command_container.h"
#include "build_execute_state_machine.h"
#include "cpp_code_generator_visual.h"
#include "view_proxy.h"

class QKeyEvent;
class QPrinter;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

class Command;
class FindAndReplaceDialog;
class RootPresentation;

/**
 * View proxy used to manage the run/export functions.
 */
class RunExportViewProxy:public ViewProxy, protected CppCodeGeneratorVisual::Client {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        RunExportViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~RunExportViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

        /**
         * Method that is called to perform binding of static actions to slots used to perform those actions.  This
         * method is called once each time a new view is created.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered when the element stack contents changes.
         */
        void elementStackChanged();

        /**
         * Slot that is triggered when the user requests the File | Export | Image... dialog.
         */
        void fileExportImage();

        /**
         * Slot that is triggered when the user requests the File | Export | HTML... dialog.
         */
        void fileExportHtml();

        /**
         * Slot that is triggered when the user requests the File | Export | LaTeX... dialog.
         */
        void fileExportLaTeX();

        /**
         * Slot that is triggered when the user requests the File | Export | Library... dialog (variously named).
         */
        void fileExportLibrary();

        /**
         * Slot that is triggered when the user requests the File | Export | Object... dialog (variously named).
         */
        void fileExportObject();

        /**
         * Slot that is triggered when the user requests the Debug | Run option.
         */
        void debugRun();

        /**
         * Slot that is triggered when the user requests the Debug | Stop option.
         */
        void debugStop();

        /**
         * Slot that is triggered when the user requests the Debug | Pause option.
         */
        void debugPause();

        /**
         * Slot that is triggered when the user requests the Debug | Single Step option.
         */
        void debugSingleStep();

        /**
         * Slot that is triggered when the user requests the Debug | Run To Cursor option.
         */
        void debugRunToCursor();

        /**
         * Slot that is triggered when the user requests the Debug | Inspect variable... option.
         */
        void debugInspectVariable();

        /**
         * Slot that is triggered when the user requests the Debug | Set/Clear Instruction Breakpoint option.
         */
        void debugToggleInstructionBreakpoint();

        /**
         * Slot that is triggered when the user requests that all breakpoints be cleared.
         */
        void debugClearAllBreakpoints();

        /**
         * Slot that is triggered when the state machine is idle or has a build available.  This signal can be used to
         * enable run/pause controls.
         *
         * \param[out] nowReady Holds true if the state machine is ready for new user run/pause commands.  Holds false
         *                      if the state machine is in a transitional state or is actively running a model.
         */
        void debugEngineReady(bool nowReady);

        /**
         * Slot that is triggered when a model begins execution.
         *
         * \param[in] runningRootElement The root element that we are running.
         *
         * \param[in] debugMode          Flag indicating if the program is being run in debug or non-debug mode.
         */
        void modelRunning(QSharedPointer<Ld::RootElement> runningRootElement, bool debugMode);

        /**
         * Slot that is triggered when the running executable terminates.
         *
         * \param[in] stoppedRootElement The root element associated with the library being executed.
         */
        void modelStopped(QSharedPointer<Ld::RootElement> stoppedRootElement);

        /**
         * Signal that is emitted when the running executable is aborted.
         *
         * \param[out] abortedRootElement The root element associated with the library being executed.
         */
        void modelAborted(QSharedPointer<Ld::RootElement> abortedRootElement);

        /**
         * Slot that is triggered when the running executable pauses due to a user request.
         *
         * \param[in] pausedRootElement The root element associated with the library being executed.
         *
         * \param[in] pausedAt          The element that we are paused at.
         */
        void modelPausedOnUserRequest(QSharedPointer<Ld::RootElement> pausedRootElement, Ld::ElementPointer pausedAt);

        /**
         * Slot that is triggered when the running executable pauses at an instruction breakpoint.
         *
         * \param[in] pausedRootElement The root element associated with the library being executed.
         *
         * \param[in] pausedAt          The element that we are paused at.
         */
        void modelPausedAtInstructionBreakpoint(
            QSharedPointer<Ld::RootElement> pausedRootElement,
            Ld::ElementPointer              pausedAt
        );

        /**
         * Slot that is triggered when the running executable pauses on a variable update.
         *
         * \param[in] pausedRootElement The root element associated with the library being executed.
         *
         * \param[in] pausedAt          The element that we are paused at.
         *
         * \param[in] identifierData    The identifier that triggered the pause condition.
         */
        void modelPausedOnVariableUpdate(
            QSharedPointer<Ld::RootElement> pausedRootElement,
            Ld::ElementPointer              pausedAt,
            const Ld::IdentifierContainer&  identifierData
        );

        /**
         * Slot that is triggered when the running executable resumes execution.
         *
         * \param[in] resumedRootElement The root element associated with the library being executed.
         */
        void modelResumed(QSharedPointer<Ld::RootElement> resumedRootElement);

        /**
         * Slot that is triggered whenever the document managed by this view widget has been changed, including updates
         * to references.  This signal will not be called when the root element is loaded, saved, or assigned to this
         * view widget.
         */
        void documentChanged();

        /**
         * Slot that is triggered when the model reports immediate output.  You can use this to stream output from
         * the model to a given device and channel.  This method is called from the same thread(s) that the model
         * is running in and will block the thread.
         *
         * \param[in] device  The device to receive the data.
         *
         * \param[in] value   The reported value.
         */
        void sendOutputToDevice(Model::Device device, const Model::Variant& value);

        /**
         * Slot that is triggered whenever we attempt to execute a model in a read-only directory.
         *
         * \param[in] directoryPath The path to the read-only directory in question.
         */
        void directoryIsReadOnly(const QString& directoryPath);

    private:
        /**
         * Method that is called when this client receives control of the code generator.
         */
        void clientOwnsGenerator() final;

        /**
         * Method that is called when another client receives control of the code generator.
         *
         * \param[in] rootElement The root element the generator is operating on.
         */
        void generatorBusy() final;

        /**
         * Method that is called when the generator becomes idle and can be claimed.
         */
        void generatorIdle() final;

        /**
         * Method that is called when translation has started.
         *
         * \param[in] rootElement The root element the generator is operating on.
         */
        void translationStarted(QSharedPointer<Ld::RootElement> rootElement) final;

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
        void translationPhaseStarted(
            QSharedPointer<Ld::RootElement> rootElement,
            unsigned                        newPhaseNumber,
            unsigned                        numberPhases,
            const QString&                  phaseName
        ) final;

        /**
         * Method that is called when the code generator has completed the last translation phase.
         *
         * \param[in] rootElement The root element the generator is operating on.
         *
         * \param[in] successful  A flag holding true if the translation was successful.
         */
        void translationCompleted(QSharedPointer<Ld::RootElement> rootElement, bool successful) final;

        /**
         * Method that is called when the code generator has aborted a translation process.
         *
         * \param[in] rootElement The root element the generator is operating on.
         */
        void translationAborted(QSharedPointer<Ld::RootElement> rootElement) final;

        /**
         * Method that is called each time the code generator uncovers an error.
         *
         * \param[in] rootElement The root element the generator is operating on.
         *
         * \param[out] diagnostic The diagnostic tied to the error, warning, etc.
         */
        void translationErrorDetected(
            QSharedPointer<Ld::RootElement> rootElement,
            Ld::DiagnosticPointer           diagnostic
        ) final;

        /**
         * Method that can be called to connect signals.
         */
        void connectSignals();

        /**
         * Method that can be called to disconnect signals.
         */
        void disconnectSignals();

        /**
         * Method that is called when the running executable pauses due to a user request.
         *
         * \param[in] pausedRootElement The root element associated with the library being executed.
         *
         * \param[in] pausedAt          The element that we are paused at.
         *
         * \param[in] pauseReason       A string providing the reason we paused.
         */
        void modelPaused(
            QSharedPointer<Ld::RootElement> pausedRootElement,
            Ld::ElementPointer              pausedAt,
            const QString&                  pauseReason
        );

        /**
         * Method that checks if exporting to a location is allowed.
         *
         * \param[in] exportDirectory The directory we want to export to.
         *
         * \param[in] exportedContent The type of content we're exporting.
         *
         * \return Returns true if we should export the content.  Return false if we should not export the content.
         */
        bool doExportMarkup(const QString& exportDirectory, const QString& exportedContent);

        /**
         * Method that checks if it's OK to overwrite a specified file.
         *
         * \param[in] overwriteFilename The filename of the file we are about to overwrite.
         *
         * \return Returns true if we should overwrite the specified file.  Returns false if we should *not* overwrite
         *         the specified file.
         */
        bool doOverwriteFile(const QString& overwriteFilename);
};

#endif
