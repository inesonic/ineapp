/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ObjectExportDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QString>

#include <eqt_file_dialog.h>

#include "application.h"
#include "object_export_dialog.h"

ObjectExportDialog::ObjectExportDialog(QWidget* parent):EQt::FileDialog(parent) {
    setOptions(QFileDialog::DontUseNativeDialog);
    setWindowTitle(tr("Export Binary Object File"));

    setAcceptMode(QFileDialog::AcceptSave);
    setFileMode(QFileDialog::AnyFile);
    setLabelText(DialogLabel::Accept, tr("Export"));
}


ObjectExportDialog::~ObjectExportDialog() {}


QString ObjectExportDialog::settingsGroupName() const {
    return "ObjectExportDialog";
}


QStringList ObjectExportDialog::filterList() const {
    QStringList filters;

    #if (defined(Q_OS_WIN))

        filters << tr("Object Files (*.obj)") << tr("Any files (*.*)");

    #elif (defined(Q_OS_LINUX) || defined(Q_OS_DARWIN))

        filters << tr("Object Files (*.o)") << tr("Any files (*)");

    #else

        #error Unknown platform

    #endif

    return filters;
}
