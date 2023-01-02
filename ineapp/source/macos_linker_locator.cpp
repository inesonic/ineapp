/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MacOSLinkerLocator class.
***********************************************************************************************************************/

#include <QObject>
#include <QMessageBox>
#include <QString>
#include <QProcess>

#include "application.h"
#include "macos_linker_locator.h"

const char MacOSLinkerLocator::xcodeSelectPath[] = "/usr/bin/xcode-select";

MacOSLinkerLocator::MacOSLinkerLocator(QObject* parent):QObject(parent) {}


MacOSLinkerLocator::~MacOSLinkerLocator() {}


bool MacOSLinkerLocator::locateLinker() {
    bool success;

    if (xcodeToolsAreInstalled()) {
        currentErrorStatus.clear();
        success = true;
    } else {
        QMessageBox::StandardButton userResult = QMessageBox::question(
            Q_NULLPTR,
            tr("Xcode Tools Missing"),
            tr(
                "You will need to install the Xcode command line tools to use %1.\n"
                "\n"
                "Click OK to launch the Xcode Command Line Tools Installer or "
                "Cancel to Exit %1."
            ).arg(Application::applicationName()),
            QMessageBox::StandardButton::Ok | QMessageBox::StandardButton::Cancel,
            QMessageBox::StandardButton::Ok
        );

        if (userResult == QMessageBox::StandardButton::Ok) {
            installXCodeTools();
            if (xcodeToolsAreInstalled()) {
                currentErrorStatus.clear();
                success = true;
            } else {
                currentErrorStatus = tr("Xcode Command Line Tools Installation failed.");
                success            = false;
            }
        } else {
            currentErrorStatus = tr("Xcode Command Line Tools Installation cancelled on user request.");
            success            = false;
        }
    }

    return success;
}


QString MacOSLinkerLocator::linkerPath() const {
    return QString("/usr/bin");
}


QString MacOSLinkerLocator::linkerExecutable() const {
    return QString("clang");
}


bool MacOSLinkerLocator::xcodeToolsAreInstalled() {
    QProcess xcodeProcess;

    xcodeProcess.setProgram(xcodeSelectPath);
    xcodeProcess.setArguments(QStringList() << "--print-path");
    xcodeProcess.start();
    bool success = xcodeProcess.waitForFinished(1000);

    return success;
}


void MacOSLinkerLocator::installXCodeTools() {
    QProcess xcodeProcess;

    xcodeProcess.setProgram(xcodeSelectPath);
    xcodeProcess.setArguments(QStringList() << "--install");
    xcodeProcess.start();
    xcodeProcess.waitForFinished(-1);
}
