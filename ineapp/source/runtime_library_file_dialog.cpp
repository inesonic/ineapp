/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RuntimeLibraryFileDialog class.
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
#include "runtime_library_file_dialog.h"

RuntimeLibraryFileDialog::RuntimeLibraryFileDialog(
        QWidget* parent
    ):EQt::FileDialog(
        tr("Select Runtime Import Library"),
        parent
    ) {
    setAcceptMode(QFileDialog::AcceptOpen);
    setFileMode(QFileDialog::ExistingFile);
    setWindowTitle(tr("Runtime Import Library"));
}


RuntimeLibraryFileDialog::~RuntimeLibraryFileDialog() {}


QString RuntimeLibraryFileDialog::settingsGroupName() const {
    return "RuntimeLibraryFileDialog";
}


QStringList RuntimeLibraryFileDialog::filterList() const {
    return QStringList() << tr("Import Library (*.lib)") << tr("All files (*)");
}
