/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CommandQueueBase class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>
#include <QList>

#include "ld_cursor_weak_collection.h"
#include "ld_cursor_collection.h"
#include "ld_cursor_state_collection.h"

#include "command.h"
#include "cursor.h"
#include "command_container.h"
#include "command_queue_base.h"

const CommandContainer CommandQueueBase::dummyContainer;

CommandQueueBase::CommandQueueBase(unsigned long newStackDepth) {
    currentMaximumStackDepth = newStackDepth;
}


CommandQueueBase::~CommandQueueBase() {}


void CommandQueueBase::setMaximumStackDepth(unsigned long newStackDepth) {
    currentMaximumStackDepth = newStackDepth;

    if (newStackDepth != 0) {
        unsigned long oldRedoStackSize = static_cast<unsigned long>(currentRedoStack.size());
        unsigned long oldUndoStackSize = static_cast<unsigned long>(currentUndoStack.size());

        unsigned long totalStackDepth = oldUndoStackSize + oldRedoStackSize;

        if (totalStackDepth > newStackDepth) {
            if (newStackDepth >= oldRedoStackSize) {
                unsigned long idealUndoStackSize = newStackDepth - oldRedoStackSize;
                if (idealUndoStackSize > 0) {
                    currentUndoStack.erase(currentUndoStack.begin() + idealUndoStackSize, currentUndoStack.end());
                } else {
                    currentUndoStack.clear();
                }
            } else {
                currentRedoStack.clear();

                if (newStackDepth < oldUndoStackSize) {
                    currentUndoStack.erase(currentUndoStack.begin() + newStackDepth, currentUndoStack.end());
                }
            }

            generateUndoRedoSignals(oldUndoStackSize, oldRedoStackSize);
        }
    }
}


unsigned long CommandQueueBase::maximumStackDepth() const {
    return currentMaximumStackDepth;
}


bool CommandQueueBase::addCursor(CursorPointer newCursor) {
    bool success;

    if (!currentCursors.contains(newCursor.toWeakRef())) {
        currentCursors.insert(newCursor.toWeakRef());
        success = true;
    } else {
        success = false;
    }

    return success;
}


bool CommandQueueBase::removeCursor(CursorPointer oldCursor) {
    bool success;

    if (currentCursors.contains(oldCursor.toWeakRef())) {
        currentCursors.remove(oldCursor.toWeakRef());
        success = true;
    } else {
        success = false;
    }

    return success;
}


Ld::CursorCollection CommandQueueBase::cursors() const {
    return currentCursors;
}


unsigned long CommandQueueBase::undoStackSize() const {
    return static_cast<unsigned long>(currentUndoStack.size());
}


unsigned long CommandQueueBase::redoStackSize() const {
    return static_cast<unsigned long>(currentRedoStack.size());
}


bool CommandQueueBase::canUndo() const {
    return !currentUndoStack.isEmpty();
}


bool CommandQueueBase::canRedo() const {
    return !currentRedoStack.isEmpty();
}


const CommandContainer& CommandQueueBase::at(long index) const {
    if (index <= 0) {
        unsigned long undoIndex = static_cast<unsigned long>(-index);

        if (undoIndex < static_cast<unsigned long>(currentUndoStack.size())) {
            return currentUndoStack.at(undoIndex);
        } else{
            return dummyContainer;
        }
    } else {
        unsigned long redoIndex = static_cast<unsigned long>(index - 1);

        if (redoIndex < static_cast<unsigned long>(currentRedoStack.size())) {
            return currentRedoStack.at(redoIndex);
        } else {
            return dummyContainer;
        }
    }
}


void CommandQueueBase::insertCommand(Command* newCommand) {
    insertCommand(CommandContainer(newCommand));
}


void CommandQueueBase::insertCommand(QSharedPointer<Command> newCommand) {
    insertCommand(CommandContainer(newCommand));
}


void CommandQueueBase::insertCommand(const CommandContainer& newCommand) {
    CommandContainer container = newCommand;

    Ld::CursorStateCollection cursorStateCollection(currentCursors);
    bool success = container.execute(&cursorStateCollection);

    if (!success) {
        commandFailed(newCommand);
    } else {
        unsigned long oldRedoStackSize = static_cast<unsigned long>(currentRedoStack.size());
        unsigned long oldUndoStackSize = static_cast<unsigned long>(currentUndoStack.size());
        bool          forceCallbacks   = false;

        if (!currentRedoStack.isEmpty()) {
            currentRedoStack.clear();
        }

        if (currentUndoStack.isEmpty()) {
            currentUndoStack.prepend(container);
        } else {
            bool couldMerge = currentUndoStack.first().merge(newCommand);
            if (!couldMerge) {
                currentUndoStack.prepend(container);
            } else {
                forceCallbacks = true;
            }
        }

        unsigned long undoStackSize = static_cast<unsigned long>(currentUndoStack.size());
        if (currentMaximumStackDepth != infiniteStackDepth && undoStackSize > currentMaximumStackDepth) {
            currentUndoStack.removeLast();
        }

        generateUndoRedoSignals(oldUndoStackSize, oldRedoStackSize, forceCallbacks);
    }
}


void CommandQueueBase::undo() {
    if (!currentUndoStack.isEmpty()) {
        CommandContainer container = currentUndoStack.first();

        Ld::CursorStateCollection cursorStateCollection(currentCursors);
        bool success = container.undo(&cursorStateCollection);

        if (success) {
            restoreCursor(container);

            unsigned long oldRedoStackSize = static_cast<unsigned long>(currentRedoStack.size());
            unsigned long oldUndoStackSize = static_cast<unsigned long>(currentUndoStack.size());

            currentUndoStack.removeFirst();
            currentRedoStack.prepend(container);

            generateUndoRedoSignals(oldUndoStackSize, oldRedoStackSize);
        } else {
            undoFailed(container);
        }
    } else {
        undoFailed(CommandContainer());
    }
}


void CommandQueueBase::redo() {
    if (!currentRedoStack.isEmpty()) {
        CommandContainer container = currentRedoStack.first();

        restoreCursor(container);
        Ld::CursorStateCollection cursorStateCollection(currentCursors);
        bool success = container.redo(&cursorStateCollection);

        if (success) {
            unsigned long oldRedoStackSize = static_cast<unsigned long>(currentRedoStack.size());
            unsigned long oldUndoStackSize = static_cast<unsigned long>(currentUndoStack.size());

            currentRedoStack.removeFirst();
            currentUndoStack.prepend(container);

            generateUndoRedoSignals(oldUndoStackSize, oldRedoStackSize);
        } else {
            redoFailed(container);
        }
    } else {
        redoFailed(CommandContainer());
    }
}


void CommandQueueBase::undoAvailable(bool) {}


void CommandQueueBase::redoAvailable(bool) {}


void CommandQueueBase::changed(unsigned long, unsigned long) {}


void CommandQueueBase::commandFailed(const CommandContainer&) {}


void CommandQueueBase::undoFailed(const CommandContainer&) {}


void CommandQueueBase::redoFailed(const CommandContainer&) {}


void CommandQueueBase::generateUndoRedoSignals(
        unsigned long oldUndoStackSize,
        unsigned long oldRedoStackSize,
        bool          forceCallbacks
    ) {
    unsigned long newRedoStackSize = static_cast<unsigned long>(currentRedoStack.size());
    unsigned long newUndoStackSize = static_cast<unsigned long>(currentUndoStack.size());

    if (oldRedoStackSize != newRedoStackSize || oldUndoStackSize != newUndoStackSize || forceCallbacks) {
        changed(newUndoStackSize, newRedoStackSize);

        if ((oldUndoStackSize == 0) != (newUndoStackSize == 0) || forceCallbacks) {
            undoAvailable(newUndoStackSize != 0);
        }

        if ((oldRedoStackSize == 0) != (newRedoStackSize == 0) || forceCallbacks) {
            redoAvailable(newRedoStackSize != 0);
        }
    }
}


void CommandQueueBase::restoreCursor(const CommandContainer& container) {
    CursorPointer cursor = container.cursor();
    if (!cursor.isNull()) {
        const Ld::Cursor&        cursorAtIssue   = container.cursorAtIssue();
        const Ld::ElementCursor& elementCursor   = cursorAtIssue.elementCursor();
        const Ld::ElementCursor& selectionCursor = cursorAtIssue.selectionCursor();

        if (selectionCursor.isValid()) {
            cursor->updateFromElementCursor(elementCursor);
            cursor->updateSelectionFromElementCursor(selectionCursor);
        } else {
            cursor->updateFromElementCursor(elementCursor, false);
        }
    }
}
