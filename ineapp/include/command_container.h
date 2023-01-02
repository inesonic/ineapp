/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CommandContainer class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_CONTAINER_H
#define COMMAND_CONTAINER_H

#include <QSharedPointer>
#include <QString>

#include "app_common.h"
#include "cursor.h"
#include "command.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * Class that can be used to more easily manage \ref Command instances within containers and other similar structures.
 */
class APP_PUBLIC_API CommandContainer {
    public:
        CommandContainer();

        /**
         * Constructor
         *
         * \param[in] command Shared pointer to the command instance to be held within this container.
         */
        CommandContainer(QSharedPointer<Command> command);

        /**
         * Constructor
         *
         * \param[in] command A pointer to the command instance to be held within this container.  Note that the
         *                    container will take shared ownership of the command instance.
         */
        CommandContainer(Command* command);

        /**
         * Copy constructor
         *
         * \param[in] other The instance to be copied.
         */
        CommandContainer(const CommandContainer& other);

        ~CommandContainer();

        /**
         * Method that indicates if this container holds valid data.
         *
         * \return Returns true if the container is valid.  Returns false if the container is invalid.
         */
        bool isValid() const;

        /**
         * Method that indicates if this container holds invalid data.
         *
         * \return Returns true if the container is invalid.  Returns false if the container is valid.
         */
        bool isInvalid() const;

        /**
         * Method that provides access to the underlying command container instance.
         *
         * \return Returns a reference to the underlying command instance.
         */
        Command& command();

        /**
         * Method that returns the command type as a string.
         *
         * \return Returns the command type string.  By convention, the returned string is the name of the class.
         */
        Command::CommandType commandType() const;

        /**
         * Method that attempts to merge another command with this command.  If the merge is successful, the merged
         * command can be successfully destroyed, information will be included into this object instance.
         *
         * \param[in] commandToMerge Shared pointer to the command to be merged into this command.
         *
         * \return Returns true on success, returns false if the commands could not be merged.
         */
        bool merge(QSharedPointer<Command> commandToMerge);

        /**
         * Method that attempts to merge another command with this command.  If the merge is successful, the merged
         * command can be successfully destroyed, information will be included into this object instance.
         *
         * \param[in] commandToMerge Container holding the command to be merged with this command.
         *
         * \return Returns true on success, returns false if the commands could not be merged.
         */
        bool merge(const CommandContainer& commandToMerge);

        /**
         * Method that executes the command.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool execute(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that performs an undo of the command using stored information.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undo(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that performs a redo of the command using stored information.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool redo(Ld::CursorStateCollection* cursorStateCollection);

        /**
         * Method that returns a description of the command suitable for displaying in an undo/redo stack window.
         *
         * \return Returns a description of the command.
         */
        QString description() const;

        /**
         * Method that returns a detailed description of the command suitable for displaying in an undo/redo stack
         * window.  This detailed description is suitable for use in fly-over text and other more verbose
         * presentations.
         *
         * \return Returns a detailed description of the command.
         */
        QString detailedDescription() const;

        /**
         * Method that returns the cursor being used by this command.
         *
         * \return Returns a QPointer instance to the cursor.
         */
        CursorPointer cursor() const;

        /**
         * Method you can call to obtain a copy of the cursor when the command was issued.
         *
         * \return Returns a copy of the cursor at the point that the cursor was set.
         */
        const Cursor& cursorAtIssue() const;

        /**
         * Method that returns a reference to the underlying command instance.
         *
         * \return Returns a reference to the underlying command instance.
         */
        Command& operator*() const;

        /**
         * Method that returns a shared pointer to the underlying command instance.
         *
         * \return Returns a shared pointer to the underlying command instance.
         */
        Command* operator->() const;

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be assigned to this instance.
         *
         * \return Returns a reference to this object.
         */
        CommandContainer& operator=(const CommandContainer& other);

        /**
         * Assignment operator.
         *
         * \param[in] other The instance to be assigned to this instance.
         *
         * \return Returns a reference to this object.
         */
        CommandContainer& operator=(QSharedPointer<Command> other);

    private:
        /**
         * A default, invalid cursor.
         */
        static const Cursor invalidCursor;

        /**
         * Shared pointer to the underlying command instance.
         */
        QSharedPointer<Command> currentCommand;
};

#endif
