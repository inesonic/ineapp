/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CommandBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_BASE_H
#define COMMAND_BASE_H

#include <QString>
#include <QSharedPointer>

#include <ld_element_structures.h>
#include <ld_cursor.h>

#include "app_common.h"
#include "cursor.h"
#include "fixer.h"
#include "command.h"
#include "edit_helpers.h"

namespace Ld {
    class ElementCursor;
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Class derived from \ref Command that provides a collection of useful methods.
 */
class APP_PUBLIC_API CommandBase:public Command, protected EditHelpers {
    public:
        CommandBase();

        /**
         * Constructor
         *
         * \param[in] newCursor The cursor who's position that should be updated in response to this command.
         */
        CommandBase(CursorPointer newCursor);

        ~CommandBase() override;
};

#endif
