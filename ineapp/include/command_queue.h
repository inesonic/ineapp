/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CommandQueue class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_QUEUE_H
#define COMMAND_QUEUE_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include "app_common.h"
#include "command_container.h"
#include "command_queue_base.h"

/**
 * Class that extends the \ref CommandQueueBase class to pull it into the Qt signal/slot framework.
 */
class APP_PUBLIC_API CommandQueue:public QObject, public CommandQueueBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] newStackDepth The depth to apply to the command queue's undo/redo stack.
         *
         * \param[in] parent        Pointer to the parent object.
         */
        CommandQueue(unsigned long newStackDepth = infiniteStackDepth, QObject* parent = Q_NULLPTR);

        ~CommandQueue();

    signals:
        /**
         * Signal you can use to receive notification whenever the undo status changes.  The default
         * implementation simply returns.
         *
         * \param[in] nowCanUndo If true, we can now perform an undo operation.  If false, there are no operations
         *                       to be undone.
         */
        void undoIsAvailable(bool nowCanUndo);

        /**
         * Signal you can use to receive notification whenever the redo status changes.  The default
         * implementation simply returns.
         *
         * \param[in] nowCanRedo If true, we can now perform an redo operation.  If false, there are no operations
         *                       to be redone.
         */
        void redoIsAvailable(bool nowCanRedo);

        /**
         * Signal you can use to receive notification whenever the undo/redo stack changes.
         *
         * \param[out] undoStackSize The new size of the undo stack.
         *
         * \param[out] redoStackSize The new size of the redo stack.
         */
        void queueChanged(unsigned long undoStackSize, unsigned long redoStackSize);

        /**
         * Signal that is emitted whenever a command fails.
         *
         * \param[out] failedCommand The command that could not be performed.
         */
        void commandHasFailed(const CommandContainer& failedCommand);

        /**
         * Signal that is emitted whenever an undo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.  This signal will be emitted with an
         *                           invalid container if an undo was requested when no undo operation was
         *                           available.
         */
        void undoHasFailed(const CommandContainer& failedCommand);

        /**
         * Signal that is emitted when a redo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.  This signal will be emitted with an
         *                           invalid container if an redo was requested when no redo operation was
         *                           available.
         */
        void redoHasFailed(const CommandContainer& failedCommand);

    private:
        /**
         * Virtual method you can overload to receive notification whenever the undo status changes.  The default
         * implementation simply returns.
         *
         * \param[in] nowCanUndo If true, we can now perform an undo operation.  If false, there are no operations
         *                       to be undone.
         */
        void undoAvailable(bool nowCanUndo) final;

        /**
         * Virtual method you can overload to receive notification whenever the redo status changes.  The default
         * implementation simply returns.
         *
         * \param[in] nowCanRedo If true, we can now perform an redo operation.  If false, there are no operations
         *                       to be redone.
         */
        void redoAvailable(bool nowCanRedo) final;

        /**
         * Virtual method you can overload to receive notification whenever the undo/redo stack changes.
         *
         * \param[out] undoStackSize The new size of the undo stack.
         *
         * \param[out] redoStackSize The new size of the redo stack.
         */
        void changed(unsigned long undoStackSize, unsigned long redoStackSize) final;

        /**
         * Virtual method that is called whenever a command fails.
         *
         * \param[out] failedCommand The command that could not be performed.
         */
        void commandFailed(const CommandContainer& failedCommand) final;

        /**
         * Virtual method that is called whenever an undo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.  This parameter will be an invalid
         *                           container if an undo was requested when no undo operation was available.
         */
        void undoFailed(const CommandContainer& failedCommand) final;

        /**
         * Virtual method that is called that is emitted when a redo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.  This parameter will be an invalid
         *                           container if an redo was requested when no redo operation was available.
         */
        void redoFailed(const CommandContainer& failedCommand) final;
};

#endif
