/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlugInManager class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>

#include <plug_in_data.h>
#include <ld_plug_in_information.h>
#include <ld_plug_in_manager.h>

#include "application.h"
#include "plug_in_manager.h"

PlugInManager::PlugInManager(QObject* parent):QObject(parent) {}


PlugInManager::~PlugInManager() {}


void PlugInManager::aboutToLoad(const QString& loadPath) {
    emit aboutToLoadPlugIn(loadPath);
    Application::processEvents();
}


void PlugInManager::plugInLoaded(const QString& loadPath, Ld::PlugInInformation plugInInformation) {
    emit loadedPlugIn(loadPath, plugInInformation);
    Application::processEvents();
}


void PlugInManager::plugInFailedToLoad(const QString& loadPath) {
    emit failedToLoadPlugIn(loadPath);
    Application::processEvents();
}
