/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CutCopyCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_cursor.h>
#include <ld_cursor_state_collection.h>

#include "clipboard.h"
#include "cursor.h"
#include "command_container.h"
#include "command_base_with_undo.h"
#include "cut_copy_command.h"

CutCopyCommand::CutCopyCommand(CutCopyCommand::Mode mode, Clipboard* clipboard) {
    currentMode      = mode;
    currentClipboard = clipboard;
}


CutCopyCommand::CutCopyCommand(
        CursorPointer        newCursor,
        CutCopyCommand::Mode mode,
        Clipboard*           clipboard
    ):CommandBaseWithUndo(
        newCursor
    ) {
    currentMode      = mode;
    currentClipboard = clipboard;
}


CutCopyCommand::~CutCopyCommand() {}


CutCopyCommand::CommandType CutCopyCommand::commandType() const {
    return Command::CommandType::CUT_COPY;
}


bool CutCopyCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    Cursor cursorAtIssue = CutCopyCommand::cursorAtIssue();
    if (currentClipboard != Q_NULLPTR                               &&
        (cursorAtIssue.cursorType() == Cursor::Type::SELECTION ||
         cursorAtIssue.cursorType() == Cursor::Type::ELEMENT      )    ) {
        if (currentMode == Mode::CUT) {
            Ld::ElementPointer parent = cursorAtIssue.startCursor().parent();

            saveParagraphs(cursorAtIssue);
            currentSelectionData = cursorAtIssue.cutSelection(cursorStateCollection);

            if (!currentSelectionData.isNull() && !currentSelectionData->isEmpty()) {
                cursorStateCollection->updateCursorState(true);
                markChangedParagraphs(parent, 0, 0);
            }
        } else {
            Q_ASSERT(currentMode == Mode::COPY);
            currentSelectionData = cursor()->copySelection();
        }

        if (!currentSelectionData.isNull()) {
            currentClipboard->addContent(currentSelectionData);
            result = true;
        } else {
            result = false;
        }
    } else {
        result = false;
    }

    return result;
}


bool CutCopyCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;

    if (currentMode == Mode::CUT) {
        success = removeChangedParagraphs(cursorStateCollection) && restoreSavedParagraphs(cursorStateCollection);
    } else {
        success = true;
    }

    return success;
}


QString CutCopyCommand::description() const {
    QString result;

    if (currentMode == Mode::CUT) {
        result = tr("cut selection");
    } else {
        result = tr("copy selection");
    }

    return result;
}


void CutCopyCommand::setMode(CutCopyCommand::Mode newMode) {
    currentMode = newMode;
}


CutCopyCommand::Mode CutCopyCommand::mode() const {
    return currentMode;
}


void CutCopyCommand::setClipboard(Clipboard* newClipboard) {
    currentClipboard = newClipboard;
}


Clipboard* CutCopyCommand::clipboard() const {
    return currentClipboard;
}
