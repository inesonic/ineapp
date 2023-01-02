/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PasteCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "clipboard.h"
#include "selection_data.h"
#include "command_container.h"
#include "command_base_with_undo.h"
#include "paste_command.h"

PasteCommand::PasteCommand(Clipboard* clipboard) {
    capturedSelectionData = clipboard->content();
}


PasteCommand::PasteCommand(QSharedPointer<Cursor> newCursor, Clipboard* clipboard):CommandBaseWithUndo(newCursor) {
    capturedSelectionData = clipboard->content();
}


PasteCommand::~PasteCommand() {}


PasteCommand::CommandType PasteCommand::commandType() const {
    return Command::CommandType::PASTE;
}


bool PasteCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    const Ld::ElementCursor& startingCursor = cursorAtIssue().startCursor();
    saveParagraphs(cursorAtIssue());

    Ld::ElementCursor   pasteEndingPosition;
    Cursor::PasteResult pasteResult = Cursor::pasteSelection(
        capturedSelectionData,
        startingCursor,
        cursorStateCollection,
        &pasteEndingPosition
    );

    result = (pasteResult == Cursor::PasteResult::SUCCESS);

    if (result == true) {
        cursorStateCollection->updateCursorState(false);

        cursor()->updateFromElementCursor(startingCursor);
        cursor()->updateSelectionFromElementCursor(pasteEndingPosition);

        markChangedParagraphs(startingCursor, pasteEndingPosition);
    }

    return result;
}


bool PasteCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    return removeChangedParagraphs(cursorStateCollection) && restoreSavedParagraphs(cursorStateCollection);
}


QString PasteCommand::description() const {
    return tr("paste clipboard");
}
