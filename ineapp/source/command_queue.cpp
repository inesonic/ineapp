/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CommandQueue class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>
#include <QList>

#include "command.h"
#include "command_container.h"
#include "command_queue_base.h"
#include "command_queue.h"

CommandQueue::CommandQueue(
        unsigned long newStackDepth,
        QObject*      parent
    ):QObject(
        parent
    ),CommandQueueBase(
        newStackDepth
    ) {}


CommandQueue::~CommandQueue() {}


void CommandQueue::undoAvailable(bool nowCanUndo) {
    emit undoIsAvailable(nowCanUndo);
}


void CommandQueue::redoAvailable(bool nowCanRedo) {
    emit redoIsAvailable(nowCanRedo);
}


void CommandQueue::changed(unsigned long undoStackSize, unsigned long redoStackSize) {
    emit queueChanged(undoStackSize, redoStackSize);
}


void CommandQueue::commandFailed(const CommandContainer& failedCommand) {
    emit commandHasFailed(failedCommand);
}


void CommandQueue::undoFailed(const CommandContainer& failedCommand) {
    emit undoHasFailed(failedCommand);
}


void CommandQueue::redoFailed(const CommandContainer& failedCommand) {
    emit redoHasFailed(failedCommand);
}
