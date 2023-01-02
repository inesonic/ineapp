/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref InsertStringCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef INSERT_STRING_COMMAND_H
#define INSERT_STRING_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QList>

#include <ld_element_cursor.h>
#include <ld_element.h>

#include <cstdint>

#include "app_common.h"
#include "cursor.h"
#include "selection_data.h"
#include "command_base.h"
#include "command_base_with_undo.h"

namespace Ld {
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command that can be issued to insert a string at a specified character location.
 */
class APP_PUBLIC_API InsertStringCommand:public CommandBase {
    Q_DECLARE_TR_FUNCTIONS(InsertStringCommand)

    public:
        /**
         * Type used to represent unicode character values.
         */
        typedef std::uint16_t UnicodeValue;

        /**
         * Constructor
         *
         * \param[in] newUnicodeValue The unicode value of the character to be inserted.
         */
        InsertStringCommand(UnicodeValue newUnicodeValue);

        /**
         * Constructor
         *
         * \param[in] newString The string to be inserted.
         */
        InsertStringCommand(const QString& newString);

        /**
         * Constructor
         *
         * \param[in] newUnicodeValue The unicode value of the character to be inserted.
         *
         * \param[in] newCursor       Cursor being used to track the position in the document.  Note that the command
         *                            will update the cursor position, if needed.
         */
        InsertStringCommand(UnicodeValue newUnicodeValue, CursorPointer newCursor);

        /**
         * Constructor
         *
         * \param[in] newString The string to be inserted.
         *
         * \param[in] newCursor Cursor being used to track the position in the document.  Note that the command will
         *                      update the cursor position, if needed.
         */
        InsertStringCommand(const QString& newString, CursorPointer newCursor);

        ~InsertStringCommand() override;

        /**
         * Method that returns the command type.
         *
         * \return Returns the command type value.
         */
        CommandType commandType() const final;

        /**
         * Method that attempts to merge another command with this command.  If the merge is successful, the merged
         * command can be successfully destroyed, information will be included into this object instance.
         *
         * \param[in] commandToMerge Shared pointer to the command to be merged into this command.
         *
         * \return Returns true on success, returns false if the commands could not be merged.
         */
        bool merge(const CommandContainer& commandToMerge) final;

        /**
         * Method that executes the command.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool execute(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that performs an undo of the command using stored information.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undo(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that returns a description of the command suitable for displaying in an undo/redo stack window.
         *
         * \return Returns a description of the command.
         */
        QString description() const final;

        /**
         * Method you can use to set the string to be inserted.
         *
         * \param[in] newString The new string to be inserted.
         */
        void setText(const QString& newString);

        /**
         * Method that returns the string that is going to be inserted.
         *
         * \return Returns the string to be inserted.
         */
        QString text() const;

    private:
        /**
         * Maximum merged text length.
         */
        static constexpr unsigned maximumLengthPreMerge = 32;

        /**
         * The inserted string.
         */
        QString currentText;
};

#endif
