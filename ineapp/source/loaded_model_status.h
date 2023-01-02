/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LoadedModelStatus class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LOADED_MODEL_STATUS_H
#define LOADED_MODEL_STATUS_H

#include <QObject>
#include <QTimer>

#include <model_status.h>
#include <model_api_types.h>

#include "app_common.h"
#include "metatypes.h"

namespace Model {
    class Api;
}

/**
 * This class provides a wrapper on the Model::Status class that pulls it into the Qt signal/slot framework.
 */
class APP_PUBLIC_API LoadedModelStatus:public QObject, public Model::Status {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        LoadedModelStatus(QObject* parent = Q_NULLPTR);

        ~LoadedModelStatus() override;

    signals:
        /**
         * Signal that is emitted when the model has started execution.
         *
         * \param[out] modelApi A pointer to the model API class.
         */
        void modelStarted(Model::Api* modelApi);

        /**
         * Signal that is emitted when the model has completed execution.
         *
         * \param[out] modelApi A pointer to the model API class.
         */
        void modelFinished(Model::Api* modelApi);

        /**
         * Signal that is emitted when the model has been aborted.
         *
         * \param[out] modelApi       A pointer to the model API class.
         *
         * \param[in] abortReason     The reason the model was aborted.
         *
         * \param[in] operationHandle The handle to the operation where the abort occurred.
         */
        void modelAborted(
            Model::Api*        modelApi,
            Model::AbortReason abortReason,
            unsigned long long operationHandle
        );

        /**
         * Signal that is emitted when a thread has paused due to a call to Model::Api::pause.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] operationHandle The operation handle where the model paused.
         */
        void modelPausedOnUserRequest(Model::Api* modelApi, unsigned long long operationHandle);

        /**
         * Signal that is emitted when a thread has paused due to an instruction breakpoint.  Note that this method
         * may be called from a thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] operationHandle The operation handle where the model paused.
         */
        void modelPausedAtOperation(Model::Api* modelApi, unsigned long long operationHandle);

        /**
         * Signal that is emitted when a thread has paused due to a data breakpoint.  Note that this method may be
         * called from a thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi         A pointer to the model API class.
         *
         * \param[in] operationHandle  The operation handle where the model paused.
         *
         * \param[in] identifierHandle The identifier handle that triggered the pause condition.
         */
        void modelPausedOnVariableUpdate(
            Model::Api*        modelApi,
            unsigned long long operationHandle,
            unsigned long long identifierHandle
        );

        /**
         * Signal that is emitted when the model resumes operation.
         *
         * \param[out] modelApi A pointer to the model API class.
         */
        void modelResumed(Model::Api* modelApi);

        /**
         * Signal that is emitted when the model reports immediate output.  You can use this to stream output from
         * the model to a given device and channel.  This method is called from the same thread(s) that the model
         * is running in and will block the thread.
         *
         * \param[in] device  The device to receive the data.
         *
         * \param[in] value   The reported value.
         */
        void sendOutputToDevice(Model::Device device, const Model::Variant& value);

    private:
        /**
         * Method that is called when the model has started execution.
         *
         * \param[in] modelApi A pointer to the model API class.
         */
        void started(Model::Api* modelApi) final;

        /**
         * Method that is called when the model has completed execution.  Note that this method may be called from
         * a thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi A pointer to the model API class.
         */
        void finished(Model::Api* modelApi) final;

        /**
         * Method that is called when the model has been aborted.  Note that this method may be called from a
         * thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] abortReason     The reason the model was aborted.
         *
         * \param[in] operationHandle The handle to the operation where the abort occurred.
         */
        void aborted(
            Model::Api*            modelApi,
            Model::AbortReason     abortReason,
            Model::OperationHandle operationHandle
        ) final;

        /**
         * Method that is called when a thread has paused due to a call to Model::Api::pause.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] operationHandle The operation handle where the model paused.
         */
        void pausedOnUserRequest(Model::Api* modelApi, Model::OperationHandle operationHandle) final;

        /**
         * Method that is called when a thread has paused due to an instruction breakpoint.  Note that this method
         * may be called from a thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi        A pointer to the model API class.
         *
         * \param[in] operationHandle The operation handle where the model paused.
         */
        void pausedAtOperation(Model::Api* modelApi, Model::OperationHandle operationHandle) final;

        /**
         * Method that is called when a thread has paused due to a data breakpoint.  Note that this method may be
         * called from a thread other than the thread that started the model and *must* be thread safe.
         *
         * \param[in] modelApi         A pointer to the model API class.
         *
         * \param[in] operationHandle  The operation handle where the model paused.
         *
         * \param[in] identifierHandle The identifier handle that triggered the pause condition.
         */
        void pausedOnVariableUpdate(
            Model::Api*             modelApi,
            Model::OperationHandle  operationHandle,
            Model::IdentifierHandle identifierHandle
        ) final;

        /**
         * Method that is called when the model resumes operation.
         *
         * \param[in] modelApi A pointer to the model API class.
         */
        void resumed(Model::Api* modelApi) final;

        /**
         * Method that is called by a model to report immediate output.  You can use this to stream output from
         * the model to a given device and channel.  This method is called from the same thread(s) that the model
         * is running in and will block the thread.
         *
         * \param[in] device  The device to receive the data.
         *
         * \param[in] value   The reported value.
         */
        void sendToDevice(Model::Device device, const Model::Variant& value) final;
};

#endif
