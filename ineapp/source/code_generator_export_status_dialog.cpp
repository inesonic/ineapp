/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CodeGeneratorExportStatusDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialogButtonBox>
#include <QString>
#include <QTimer>

#include <cstdint>

#include <ld_root_element.h>
#include <ld_diagnostic.h>
#include <ld_diagnostic_structures.h>
#include <ld_translation_phase.h>
#include <ld_code_generator.h>
#include <ld_code_generator_visual.h>

#include "export_status_dialog.h"
#include "code_generator_export_status_dialog.h"

CodeGeneratorExportStatusDialog::CodeGeneratorExportStatusDialog(QWidget* parent):ExportStatusDialog(parent) {
    progressTimer = new QTimer(this);

    connect(progressTimer, &QTimer::timeout, this, &CodeGeneratorExportStatusDialog::updateProgressBar);

    connect(
        this,
        &CodeGeneratorExportStatusDialog::reportTranslationStarted,
        this,
        &CodeGeneratorExportStatusDialog::processTranslationStarted,
        Qt::QueuedConnection
    );

    connect(
        this,
        &CodeGeneratorExportStatusDialog::reportTranslationCompleted,
        this,
        &CodeGeneratorExportStatusDialog::processTranslationCompleted,
        Qt::QueuedConnection
    );

    connect(
        this,
        &CodeGeneratorExportStatusDialog::reportTranslationAborted,
        this,
        &CodeGeneratorExportStatusDialog::processTranslationAborted,
        Qt::QueuedConnection
    );

    setProgressFormatText(tr("%p%"));
    setVisibleButtons(QDialogButtonBox::Abort);

    currentNormalCompletionText = tr("Done");
}


CodeGeneratorExportStatusDialog::~CodeGeneratorExportStatusDialog() {}


QString CodeGeneratorExportStatusDialog::normalCompletionText() const {
    return currentNormalCompletionText;
}


void CodeGeneratorExportStatusDialog::setNormalCompletionText(const QString& newCompletionText) {
    currentNormalCompletionText = newCompletionText;
}


void CodeGeneratorExportStatusDialog::updateProgressBar() {
    // Note: Intel and ARM architectures do 32-bit writes/reads atomically so the snippet of code below is actually not
    //       necessary.  With that said, the overhead is small and this protects us from possible surprises in future.

    volatile std::uint32_t completedSteps;
    volatile std::uint32_t rereadCompletedSteps;

    do {
        completedSteps       = lastReportedCompletedTranslationStep;
        rereadCompletedSteps = lastReportedCompletedTranslationStep;
    } while (completedSteps != rereadCompletedSteps);

    // Also: Because Qt insists on using signed integers everywhere, even when completely inappropriate (e.g. size of
    //       strings and lists), we cap the value to prevent roll-overs which would look weird to the user.  The user's
    //       program would have to be massive for this to happen so, in reality, there is little concern here.

    if (completedSteps > 0x7FFFFFFF) {
        completedSteps = 0x7FFFFFFF;
    }

    setCurrentValue(static_cast<int>(completedSteps));
}


void CodeGeneratorExportStatusDialog::processTranslationStarted(unsigned long numberPerElementTranslationSteps) {
    if (numberPerElementTranslationSteps > 0x7FFFFFFF) {
        numberPerElementTranslationSteps = 0x7FFFFFFF;
    }

    setRange(0, numberPerElementTranslationSteps);
    setCurrentValue(0);
    progressTimer->start();
}


void CodeGeneratorExportStatusDialog::processTranslationCompleted(bool success) {
    progressTimer->stop();
    setCurrentValue(lastReportedCompletedTranslationStep);

    if (success) {
        setFooterText(currentNormalCompletionText);
    } else {
        Ld::CodeGenerator*        codeGenerator = CodeGeneratorExportStatusDialog::codeGenerator();
        Ld::DiagnosticPointerList diagnostics   = codeGenerator->reportedDiagnostics();

        if (diagnostics.isEmpty()) {
            setFooterText(tr("Failure: No reported diagnostics found"));
        } else {
            QString errorReport;
            for (Ld::DiagnosticPointerList::const_iterator diagnosticIterator    = diagnostics.constBegin(),
                                                           diagnosticEndIterator = diagnostics.constEnd()
                 ; diagnosticIterator != diagnosticEndIterator
                 ; ++diagnosticIterator
                ) {
                Ld::DiagnosticPointer diagnostic = *diagnosticIterator;
                if (!errorReport.isEmpty()) {
                    errorReport += tr("\n\n");
                }

                errorReport += diagnostic->diagnosticMessage();
            }

            setFooterText(errorReport);
        }
    }

    setVisibleButtons(QDialogButtonBox::Ok);
}


void CodeGeneratorExportStatusDialog::processTranslationAborted() {
    progressTimer->stop();
    setCurrentValue(lastReportedCompletedTranslationStep);

    setFooterText(tr("Aborted"));
    setVisibleButtons(QDialogButtonBox::Ok);
}


void CodeGeneratorExportStatusDialog::reject() {
    setFooterText(tr("Please wait, aborting..."));
    codeGenerator()->abort();
}


void CodeGeneratorExportStatusDialog::translationStarted(
        QSharedPointer<Ld::RootElement>             /* rootElement */,
        const Ld::CodeGeneratorOutputTypeContainer& /* outputType */,
        unsigned long                               numberPerElementTranslationSteps
    ) {
    emit reportTranslationStarted(numberPerElementTranslationSteps);
}


void CodeGeneratorExportStatusDialog::translationStepCompleted(unsigned long numberCompletedSteps) {
    lastReportedCompletedTranslationStep = numberCompletedSteps;
}


void CodeGeneratorExportStatusDialog::translationCompleted(
        bool                                        success,
        QSharedPointer<Ld::RootElement>             /* rootElement */,
        const Ld::CodeGeneratorOutputTypeContainer& /* outputType */
    ) {
    emit reportTranslationCompleted(success);
}


void CodeGeneratorExportStatusDialog::translationAborted(
        QSharedPointer<Ld::RootElement>             /* rootElement */,
        const Ld::CodeGeneratorOutputTypeContainer& /* outputType */
    ) {
    emit reportTranslationAborted();
}
