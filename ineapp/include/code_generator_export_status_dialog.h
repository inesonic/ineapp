/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CodeGeneratorExportStatusDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CODE_GENERATOR_EXPORT_STATUS_DIALOG_H
#define CODE_GENERATOR_EXPORT_STATUS_DIALOG_H

#include <QString>
#include <QSharedPointer>
#include <QDialogButtonBox>

#include <cstdint>

#include <ld_diagnostic_structures.h>
#include <ld_code_generator_output_type_container.h>
#include <ld_translation_phase.h>
#include <ld_code_generator_visual.h>

#include "app_common.h"
#include "export_status_dialog.h"

class QWidget;
class QTimer;

namespace Ld {
    class RootElement;
}
/**
 * Class that extends the \ref ExportStatusDialog to provide status updates from code generators.  You can plug an
 * instance of this class into a code generator as the visual used by the code generator.
 */
class APP_PUBLIC_API CodeGeneratorExportStatusDialog:public ExportStatusDialog, public Ld::CodeGeneratorVisual {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        CodeGeneratorExportStatusDialog(QWidget* parent = Q_NULLPTR);

        ~CodeGeneratorExportStatusDialog() override;

        /**
         * Method you can use to obtain the current normal completion text.
         *
         * \return Returns the normal completion text.
         */
        QString normalCompletionText() const;

        /**
         * Method you can use to set the text to display in the footer area on normal completion.
         *
         * \param[in] newCompletionText The new completion text.  The default completion text is "Done".
         */
        void setNormalCompletionText(const QString& newCompletionText);

    signals:
        /**
         * Signal that is emitted when the code generator starts a translation.
         *
         * \param[in] numberPerElementTranslationSteps The total number of per-element translation steps that the
         *                                             code generator will need to perform.
         */
        void reportTranslationStarted(unsigned long numberPerElementTranslationSteps);

        /**
         * Signal that is emitted when the code generator completes a translation process.
         *
         * \param[in] success Holds true if the translation was successful.  Holds false if the translation process
         *                    failed.
         */
        void reportTranslationCompleted(bool success);

        /**
         * Signal that is emitted when the code generator is aborted during a translation.
         */
        void reportTranslationAborted();

    private slots:
        /**
         * Slot that is triggered when the pacing timer expires.
         */
        void updateProgressBar();

        /**
         * Slot that is triggered when the code generator starts a translation.
         *
         * \param[in] numberPerElementTranslationSteps The total number of per-element translation steps that the
         *                                             code generator will need to perform.
         */
        void processTranslationStarted(unsigned long numberPerElementTranslationSteps);

        /**
         * Slot that is triggered when the code generator completes a translation process.
         *
         * \param[in] success Holds true if the translation was successful.  Holds false if the translation process
         *                    failed.
         */
        void processTranslationCompleted(bool success);

        /**
         * Slot that is triggered when the code generator is aborted during a translation.
         *
         * \param[in] rootElement The root element that is used as the basis for the translation.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         */
        void processTranslationAborted();

        /**
         * Slot that is triggered when a reject button is clicked.  If you overload this method, be sure to call the
         * base class implementation so that the dialog status can be properly updated and propagated.
         */
        void reject() final;

    private:
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
         * Method that is called each time the code generator completes a translation step.  This method reports
         * the step's completion to the visual tied to the code generator.
         *
         * \param[in] numberCompletedSteps The current number of completed steps.
         */
        void translationStepCompleted(unsigned long numberCompletedSteps) final;

        /**
         * Method that is called when the code generator completes a translation process.
         *
         * \param[in] success     Holds true if the translation was successful.  Holds false if the translation
         *                        process failed.
         *
         * \param[in] rootElement The root element that is used as the basis for the translation.
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
         * \param[in] rootElement The root element that is used as the basis for the translation.
         *
         * \param[in] outputType  A value used to indicate the type of output being generated.  The meaning of this
         *                        value is dependent on the type of generator.
         */
        void translationAborted(
            QSharedPointer<Ld::RootElement>             rootElement,
            const Ld::CodeGeneratorOutputTypeContainer& outputType
        ) final;

    private:
        /**
         * Delay between status update messages.
         */
        static const unsigned long statusUpdatePeriodMilliseconds = 300;

        /**
         * The current normal completion text.
         */
        QString currentNormalCompletionText;

        /**
         * Timer used to gate translation step completed signals.
         */
        QTimer* progressTimer;

        /**
         * Value used to track completion across threads.
         */
        volatile std::uint32_t lastReportedCompletedTranslationStep;
};

#endif
