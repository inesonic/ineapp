/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DocumentFileDialog class.
***********************************************************************************************************************/

#include <QFileDialog>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QByteArray>
#include <QDir>

#include <eqt_file_dialog.h>

#include "application.h"
#include "application_settings.h"
#include "document_file_dialog.h"

DocumentFileDialog::DocumentFileDialog(
        bool     savingFile,
        QWidget* parent
    ):EQt::FileDialog(
        savingFile ? tr("Save file as") : tr("Select document to open"),
        parent
    ) {
    if (savingFile) {
        setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
        setFileMode(QFileDialog::AnyFile);
        setWindowTitle(tr("Save File"));
    } else {
        setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
        setFileMode(QFileDialog::ExistingFile);
        setWindowTitle(tr("Load File"));
    }
}


DocumentFileDialog::~DocumentFileDialog() {}


void DocumentFileDialog::configureForFileOpen(bool nowFileOpen) {
    if (nowFileOpen) {
        setAcceptMode(QFileDialog::AcceptOpen);
        setFileMode(QFileDialog::ExistingFile);
        setWindowTitle(tr("Load File"));
    } else {
        setAcceptMode(QFileDialog::AcceptSave);
        setFileMode(QFileDialog::AnyFile);
        setWindowTitle(tr("Save File"));
    }
}


void DocumentFileDialog::configureForFileSave(bool nowFileSave) {
    configureForFileOpen(!nowFileSave);
}


QString DocumentFileDialog::settingsGroupName() const {
    return "documentFileDialog";
}


QStringList DocumentFileDialog::filterList() const {
    QStringList filters;
    filters << tr("%1 documents (*.%2)").arg(Application::applicationName()).arg(FILE_EXTENSION)
            << tr("All files (*)");

    return filters;
}
