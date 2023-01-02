/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CommandQueue class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_QUEUE_BASE_H
#define COMMAND_QUEUE_BASE_H

#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QList>
#include <QSet>

#include <ld_cursor_weak_collection.h>
#include <ld_cursor_collection.h>

#include "app_common.h"
#include "cursor.h"
#include "command_container.h"

namespace Ld {
    class Cursor;
}

class Command;

/**
 * Class that manages a queue of \ref Command instances.  THe class provides support for the concepts of an undo stack
 * and redo stack and allows the GUI application to have visibility into the queue.
 */
class APP_PUBLIC_API CommandQueueBase {
    public:
        /**
         * Value used to indicate that the class should support infinite stack depth.
         */
        static constexpr unsigned long infiniteStackDepth = 0;

        /**
         * Value indicating the default stack depth.
         */
        static constexpr unsigned long defaultStackDepth = infiniteStackDepth;

        /**
         * Constructor
         *
         * \param[in] newStackDepth The depth to apply to the command queue's undo/redo stack.
         */
        CommandQueueBase(unsigned long newStackDepth = infiniteStackDepth);

        ~CommandQueueBase();

        /**
         * Method you can use to update the current stack depth.  If needed, the undo/redo queues will be modified to
         * meet the new constraint.
         *
         * \param[in] newStackDepth The new stack depth to apply to the queues.
         */
        void setMaximumStackDepth(unsigned long newStackDepth);

        /**
         * Method you can use to obtain the current maximum stack depth.
         *
         * \return Returns the current maximum stack depth.
         */
        unsigned long maximumStackDepth() const;

        /**
         * Method you can use to notify the command queue about a new cursor.
         *
         * \param[in] newCursor Pointer to the new cursor to be modified during operations.
         *
         * \return Returns true if the cursor is new.  Returns false if the cursor is already being tracked.
         */
        bool addCursor(CursorPointer newCursor);

        /**
         * Method you can use to notify the command queue that a cursor is not longer to be modified.
         *
         * \param[in] oldCursor Pointer to the new cursor to be modified during operations.
         *
         * \return Returns true if the cursor was removed from the list.  Returns false if the cursor was not being
         *         updated by commands.
         */
        bool removeCursor(CursorPointer oldCursor);

        /**
         * Method you can use to obtain a set of pointers to cursors.
         *
         * \return Returns a set of cursor pointers.
         */
        Ld::CursorCollection cursors() const;

        /**
         * Method that returns the current size of the underlying undo stack.
         *
         * \return Returns the size of the undo stack.
         */
        unsigned long undoStackSize() const;

        /**
         * Method that returns the current size of the redo stack.
         *
         * \return Returns the size of the redo stack.
         */
        unsigned long redoStackSize() const;

        /**
         * Method you can use to determine if undo operations are available.
         *
         * \return Returns true if undo operations are available.  Returns false if no undo operations are available.
         */
        bool canUndo() const;

        /**
         * Method you can use to determine if redo operations are available.
         *
         * \return Returns true if undo operations are available.  Returns false if no undo operations are available.
         */
        bool canRedo() const;

        /**
         * Method that returns the number of pending commands.
         *
         * \return Returns the number of pending commands.
         */
        unsigned numberPendingCommands() const;

        /**
         * Method that returns a specific command in the undo or redo stack.
         *
         * \param[in] index The index to the desired command.  Entry 0 is the most recently completed command.  Values
         *                  less than 0 return older executed commands.  Values greater than zero return commands in
         *                  the redo stack.
         *
         * \return Returns a reference to a command container representing the command.  An invalid command container
         *         is returned if the index value is invalid.
         */
        const CommandContainer& at(long index) const;

        /**
         * Convenience array operator.
         *
         * \param[in] index The index to the desired command.  Entry 0 is the most recently completed command.  Values
         *                  less than 0 return older executed commands.  Values greater than zero return commands in
         *                  the redo stack.
         *
         * \return Returns a reference to a command container representing the command.  An invalid command container
         *         is returned if the index value is invalid.
         */
        inline const CommandContainer& operator[](long index) const {
            return at(index);
        }

        /**
         * Method that inserts a new command into the command queue.
         *
         * \param[in] newCommand The command to be inserted into the command queue.  The class will take ownership of
         *                       the command.
         */
        void insertCommand(Command* newCommand);

        /**
         * Method that inserts a new command into the command queue.
         *
         * \param[in] newCommand The command to be inserted into the command queue.
         */
        void insertCommand(QSharedPointer<Command> newCommand);

        /**
         * Method that inserts a new command into the command queue.
         *
         * \param[in] newCommand The command to be inserted into the command queue.
         */
        void insertCommand(const CommandContainer& newCommand);

        /**
         * Method that performs an undo operation.
         */
        void undo();

        /**
         * Method that performs a redo operation.
         */
        void redo();

    protected:
        /**
         * Virtual method you can overload to receive notification whenever the undo status changes.  The default
         * implementation simply returns.
         *
         * \param[in] nowCanUndo If true, we can now perform an undo operation.  If false, there are no operations to
         *                       be undone.
         */
        virtual void undoAvailable(bool nowCanUndo);

        /**
         * Virtual method you can overload to receive notification whenever the redo status changes.  The default
         * implementation simply returns.
         *
         * \param[in] nowCanRedo If true, we can now perform an redo operation.  If false, there are no operations to
         *                       be redone.
         */
        virtual void redoAvailable(bool nowCanRedo);

        /**
         * Virtual method you can overload to receive notification whenever the undo/redo stack changes.
         *
         * \param[out] undoStackSize The new size of the undo stack.
         *
         * \param[out] redoStackSize The new size of the redo stack.
         */
        virtual void changed(unsigned long undoStackSize, unsigned long redoStackSize);

        /**
         * Virtual method that is called whenever a command fails.
         *
         * \param[out] failedCommand The command that could not be performed.
         */
        virtual void commandFailed(const CommandContainer& failedCommand);

        /**
         * Virtual method that is called whenever an undo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.  This parameter will be an invalid
         *                           container if an undo was requested when no undo operation was available.
         */
        virtual void undoFailed(const CommandContainer& failedCommand);

        /**
         * Virtual method that is called that is emitted when a redo operation fails.
         *
         * \param[out] failedCommand The command that could not be performed.  This parameter will be an invalid
         *                           container if an redo was requested when no redo operation was available.
         */
        virtual void redoFailed(const CommandContainer& failedCommand);

    private:
        /**
         * Method that will attempt to restore the cursor state for a command.
         *
         * \param[in] container The command container of the command to restore the cursor for.
         */
        void restoreCursor(const CommandContainer& container);

        /**
         * Method that conditionally calls the appropriate virtual methods to report status changes.
         *
         * \param[in] oldUndoStackSize The size of the undo stack prior to the operation being performed.
         *
         * \param[in] oldRedoStackSize The size of the redo stack prior to the operation being performed.
         *
         * \param[in] forceCallbacks   If true, callbacks will be invoked even if the sizes do not change.  This
         *                             parameter is primarily used after a successful merge.
         */
        void generateUndoRedoSignals(
            unsigned long oldUndoStackSize,
            unsigned long oldRedoStackSize,
            bool          forceCallbacks = false
        );

        /**
         * Value used as a dummy container.
         */
        static const CommandContainer dummyContainer;

        /**
         * The current undo stack.
         */
        QList<CommandContainer> currentUndoStack;

        /**
         * The current redo stack.
         */
        QList<CommandContainer> currentRedoStack;

        /**
         * The current maximum stack depth.
         */
        unsigned long currentMaximumStackDepth;

        /**
         * A set containing every cursor that may need adjustment while processing commands in this queue.
         */
        Ld::CursorWeakCollection currentCursors;
};

#endif
