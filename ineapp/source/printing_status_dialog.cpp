/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PrintingStatusDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialogButtonBox>
#include <QString>

#include "export_status_dialog.h"
#include "printing_status_dialog.h"

PrintingStatusDialog::PrintingStatusDialog(QWidget* parent):ExportStatusDialog(parent) {
    currentNormalCompletionText = tr("Done");
    currentAutoCloseEnabled     = false;

    setProgressFormatText(tr("Page ? (%p%)"));
    setCurrentValue(0);
    setRange(0, 1);

    setVisibleButtons(QDialogButtonBox::Abort);
}


PrintingStatusDialog::~PrintingStatusDialog() {}


QString PrintingStatusDialog::normalCompletionText() const {
    return currentNormalCompletionText;
}


void PrintingStatusDialog::setNormalCompletionText(const QString& newCompletionText) {
    currentNormalCompletionText = newCompletionText;
}


bool PrintingStatusDialog::autoCloseWhenDoneEnabled() const {
    return currentAutoCloseEnabled;
}


bool PrintingStatusDialog::autoCloseWhenDoneDisabled() const {
    return !currentAutoCloseEnabled;
}


void PrintingStatusDialog::setAutoCloseWhenDoneEnabled(bool nowEnabled) {
    currentAutoCloseEnabled = nowEnabled;
}


void PrintingStatusDialog::setAutoCloseWhenDoneDisabled(bool nowDisabled) {
    currentAutoCloseEnabled = !nowDisabled;
}


void PrintingStatusDialog::started(
        const QString& /* printerNameOrFile */,
        bool           /* printingToFile */,
        unsigned       numberPagesToPrint
    ) {
    setRange(0, numberPagesToPrint == 0 ? 1 : numberPagesToPrint);
    setCurrentValue(0);
}


void PrintingStatusDialog::completedPage(unsigned pageOrderIndex, unsigned pageNumber) {
    setCurrentValue(pageOrderIndex + 1);
    setProgressFormatText(tr("Page %1 (%p%)").arg(pageNumber));
}


void PrintingStatusDialog::completed(bool success) {
    if (currentAutoCloseEnabled) {
        accept();
    } else {
        if (success) {
            setFooterText(currentNormalCompletionText);
        };

        setVisibleButtons(QDialogButtonBox::Ok);
    }
}


void PrintingStatusDialog::aborted() {
    setFooterText(tr("Aborted"));
    setVisibleButtons(QDialogButtonBox::Ok);
}


void PrintingStatusDialog::errorDetected(const QString& errorText) {
    setFooterText(tr("Error: %1").arg(errorText));
}


void PrintingStatusDialog::reject() {
    setFooterText(tr("Please wait, aborting..."));
    emit abort();
}
