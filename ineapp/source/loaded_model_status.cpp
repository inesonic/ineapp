/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LoadedModelStatus class.
***********************************************************************************************************************/

#include <QObject>
#include <QSharedPointer>

#include <model_api.h>
#include <model_status.h>

#include "metatypes.h"
#include "loaded_model_status.h"

LoadedModelStatus::LoadedModelStatus(QObject* parent):QObject(parent) {}


LoadedModelStatus::~LoadedModelStatus() {}


void LoadedModelStatus::started(Model::Api* modelApi) {
    emit modelStarted(modelApi);
}


void LoadedModelStatus::finished(Model::Api* modelApi) {
    emit modelFinished(modelApi);
}


void LoadedModelStatus::aborted(
        Model::Api*            modelApi,
        Model::AbortReason     abortReason,
        Model::OperationHandle operationHandle
    ) {
    emit modelAborted(modelApi, abortReason, operationHandle);
}


void LoadedModelStatus::pausedOnUserRequest(Model::Api* modelApi, Model::OperationHandle operationHandle) {
    emit modelPausedOnUserRequest(modelApi, operationHandle);
}


void LoadedModelStatus::pausedAtOperation(Model::Api* modelApi, Model::OperationHandle operationHandle) {
    emit modelPausedAtOperation(modelApi, operationHandle);
}


void LoadedModelStatus::pausedOnVariableUpdate(
        Model::Api*             modelApi,
        Model::OperationHandle  operationHandle,
        Model::IdentifierHandle instructionHandle
    ) {
    emit modelPausedOnVariableUpdate(modelApi, operationHandle, instructionHandle);
}


void LoadedModelStatus::resumed(Model::Api* modelApi) {
    emit modelResumed(modelApi);
}


void LoadedModelStatus::sendToDevice(Model::Device device, const Model::Variant& value) {
    emit sendOutputToDevice(device, value);
}
