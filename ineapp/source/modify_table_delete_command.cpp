/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ModifyTableDeleteCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_table_frame_element.h>
#include <ld_root_element.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "command_container.h"
#include "command.h"
#include "modify_table_command.h"
#include "modify_table_delete_command.h"

ModifyTableDeleteCommand::ModifyTableDeleteCommand() {
    previousTableIndex = static_cast<unsigned long>(-1);
}


ModifyTableDeleteCommand::ModifyTableDeleteCommand(
        QSharedPointer<Ld::TableFrameElement> tableElement
    ):ModifyTableCommand(
        tableElement
    ) {
    previousTableIndex = static_cast<unsigned long>(-1);
}


ModifyTableDeleteCommand::~ModifyTableDeleteCommand() {}


ModifyTableCommand::Subcommand ModifyTableDeleteCommand::subCommandType() const {
    return ModifyTableCommand::Subcommand::DELETE_TABLE_AND_CONTENTS;
}


bool ModifyTableDeleteCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool success = false;

    Ld::ElementPointer tableElement = ModifyTableDeleteCommand::tableElement();
    if (tableElement->typeName() == Ld::TableFrameElement::elementName) {
        Ld::ElementPointer rootElement = tableElement->parent();
        if (rootElement->typeName() == Ld::RootElement::elementName) {
            previousTableIndex   = rootElement->indexOfChild(tableElement);
            previousTableElement = tableElement.dynamicCast<Ld::TableFrameElement>();

            rootElement->removeChild(tableElement, cursorStateCollection);
            cursorStateCollection->updateCursorState(false);

            success = true;
        }
    }

    return success;
}


bool ModifyTableDeleteCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;

    if (previousTableIndex != static_cast<unsigned long>(-1) && !previousTableElement.isNull()) {
        const Ld::ElementCursor&        elementCursor = cursorAtIssue().elementCursor();
        QSharedPointer<Ld::RootElement> root          = elementCursor.root().dynamicCast<Ld::RootElement>();
        Q_ASSERT(!root.isNull());

        root->insertBefore(previousTableIndex, previousTableElement, cursorStateCollection);

        previousTableIndex = static_cast<unsigned long>(-1);
        previousTableElement.reset();

        cursorStateCollection->updateCursorState(false);

        success = true;
    } else {
        success = false;
    }

    return success;
}


QString ModifyTableDeleteCommand::description() const {
    return tr("delete table");
}
