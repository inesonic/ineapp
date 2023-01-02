/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref InsertStringCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>
#include <QChar>

#include <ld_element_structures.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>
#include <ld_element.h>

#include "command_container.h"
#include "command.h"
#include "insert_string_command.h"

InsertStringCommand::InsertStringCommand(InsertStringCommand::UnicodeValue newUnicodeValue) {
    currentText = QString(QChar(newUnicodeValue));
}


InsertStringCommand::InsertStringCommand(const QString& newString) {
    currentText = newString;
}


InsertStringCommand::InsertStringCommand(
        InsertStringCommand::UnicodeValue newUnicodeValue,
        CursorPointer                     newCursor
    ):CommandBase(
        newCursor
    ) {
    currentText = QString(QChar(newUnicodeValue));
}


InsertStringCommand::InsertStringCommand(
        const QString& newString,
        CursorPointer  newCursor
    ):CommandBase(
        newCursor
    ) {
    currentText = newString;
}


InsertStringCommand::~InsertStringCommand() {}


Command::CommandType InsertStringCommand::commandType() const {
    return Command::CommandType::INSERT_STRING;
}


bool InsertStringCommand::merge(const CommandContainer& commandToMerge) {
    bool mergeSuccessful = false;

    if (commandToMerge.commandType() == Command::CommandType::INSERT_STRING &&
        static_cast<unsigned>(currentText.length()) < maximumLengthPreMerge &&
        cursorAtIssue().cursorType() != Cursor::Type::SELECTION                ) {
        InsertStringCommand& otherCommand = dynamic_cast<InsertStringCommand&>(*commandToMerge);

        if (currentText != "\n" && otherCommand.currentText != "\n") {
            const Ld::ElementCursor& cursor  = cursorAtIssue().elementCursor();
            Ld::ElementPointer       element = cursor.element();

            if (!element.isNull()) {
                unsigned      regionIndex = cursor.regionIndex();
                unsigned long textIndex   = cursor.textIndex() + currentText.length();

                Ld::ElementCursor expectedCursor(textIndex, regionIndex, element);

                if (otherCommand.cursorAtIssue().elementCursor() == expectedCursor) {
                    QString commandText  = otherCommand.currentText;

                    bool lastWasSpace = currentText.isEmpty() ? false : currentText.back().isSpace();
                    bool doMerge      = true;

                    if (lastWasSpace) {
                        QString::const_iterator it  = commandText.constBegin();
                        QString::const_iterator end = commandText.constEnd();
                        while (doMerge && it != end) {
                            doMerge = it->isSpace();
                            ++it;
                        }
                    }

                    if (doMerge) {
                        currentText += commandText;
                        mergeSuccessful = true;
                    }
                }
            }
        }
    }

    return mergeSuccessful;
}


bool InsertStringCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    const Ld::ElementCursor& elementCursor = cursorAtIssue().elementCursor();
    Ld::ElementPointer       element       = elementCursor.element();
    if (!element.isNull()) {
        // NOTE: Currently the editor does not allow text insertion while a selection is active.
        Q_ASSERT(cursorAtIssue().cursorType() != Cursor::Type::SELECTION);

        if (currentText == QString("\n")) {
            result = isSplitAllowed(Fixer::SplitReason::NEWLINE, elementCursor);
            if (result == true) {
                splitElement(Fixer::SplitReason::NEWLINE, elementCursor, cursorStateCollection);
                cursorStateCollection->updateCursorState(false);
                cursor()->moveForwardByCharacter(false);
            }
        } else {
            if (!elementCursor.isWholeElement()) {
                unsigned long textIndex   = elementCursor.textIndex();
                unsigned      regionIndex = elementCursor.regionIndex();

                element->insertText(currentText, textIndex, regionIndex, cursorStateCollection, false);
                cursorStateCollection->updateCursorState(false);

                result = true;
            } else {
                result = false;
            }
        }
    } else {
        result = false;
    }

    return result;
}


bool InsertStringCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    // NOTE: Currently the editor does not allow text insertion while a selection is active.
    Q_ASSERT(cursorAtIssue().cursorType() != Cursor::Type::SELECTION);

    const Ld::ElementCursor& elementCursor = cursorAtIssue().elementCursor();
    Ld::ElementPointer       element       = elementCursor.element();
    if (!element.isNull()) {
        if (currentText == QString("\n")) {
            result = mergeAcrossNextParent(elementCursor, cursorStateCollection);
            if (result == true) {
                cursorStateCollection->updateCursorState(false);
            }
        } else {
            if (!elementCursor.isWholeElement()) {
                unsigned long insertedLength = static_cast<unsigned>(currentText.length());
                unsigned long textIndex      = elementCursor.textIndex();
                unsigned      regionIndex    = elementCursor.regionIndex();

                result = element->removeText(
                    textIndex,
                    regionIndex,
                    textIndex + insertedLength,
                    regionIndex,
                    cursorStateCollection
                );

                if (result == true) {
                    cursorStateCollection->updateCursorState(false);
                }
            } else {
                result = false;
            }
        }
    } else {
        result = false;
    }

    return result;
}


QString InsertStringCommand::description() const {
    QString descriptionText;

    if (currentText == QString("\n")) {
        descriptionText = tr("Insert paragraph");
    } else {
        descriptionText = tr("Insert \"%1\"").arg(currentText);
    }

    return descriptionText;
}


void InsertStringCommand::setText(const QString& newString) {
    currentText = newString;
}


QString InsertStringCommand::text() const {
    return currentText;
}


/***********************************************************************************************************************
 * Alternate implementation using CommandBaseWithUndo -- Conceptually simpler but more memory intensive:
 */

//InsertStringCommand::InsertStringCommand(InsertStringCommand::UnicodeValue newUnicodeValue) {
//    currentText       = QString(QChar(newUnicodeValue));
//    currentMergeCount = 0;
//}


//InsertStringCommand::InsertStringCommand(const QString& newString) {
//    currentText       = newString;
//    currentMergeCount = 0;
//}


//InsertStringCommand::InsertStringCommand(
//        InsertStringCommand::UnicodeValue newUnicodeValue,
//        QSharedPointer<Ld::Cursor>        newCursor
//    ):CommandBaseWithUndo(
//        newCursor
//    ) {
//    currentText       = QString(QChar(newUnicodeValue));
//    currentMergeCount = 0;
//}


//InsertStringCommand::InsertStringCommand(
//        const QString&            newString,
//        QSharedPointer<Ld::Cursor> newCursor
//    ):CommandBaseWithUndo(
//        newCursor
//    ) {
//    currentText       = newString;
//    currentMergeCount = 0;
//}


//InsertStringCommand::~InsertStringCommand() {}


//Command::CommandType InsertStringCommand::commandType() const {
//    return Command::CommandType::INSERT_STRING;
//}


//bool InsertStringCommand::merge(const CommandContainer& commandToMerge) {
//    bool mergeSuccessful = false;

//    if (commandToMerge.commandType() == Command::CommandType::INSERT_STRING && currentMergeCount < maximumMergeCount) {
//        InsertStringCommand&  otherCommand = dynamic_cast<InsertStringCommand&>(*commandToMerge);

//        if (otherCommand.currentText != tr("\n") && otherCommand.currentText != tr(" ")) {
//            const Ld::Cursor& otherCursor = otherCommand.cursorAtIssue();

//            if (otherCursor.cursorType() == Ld::Cursor::Type::TEXT) {
//                const Ld::ElementCursor& otherElementCursor = otherCursor.elementCursor();
//                const Ld::ElementCursor& elementCursor      = cursorAtIssue().elementCursor();
//                Ld::ElementCursor expectedCursor(
//                    elementCursor.textIndex() + currentMergeCount + 1,
//                    elementCursor.regionIndex(),
//                    elementCursor.element()
//                );

//                if (expectedCursor == otherElementCursor) {
//                    ++currentMergeCount;
//                    mergeSuccessful = true;
//                }
//            }
//        }
//    }

//    return mergeSuccessful;
//}


//bool InsertStringCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
//    bool result;

//    const Ld::ElementCursor& elementCursor = cursorAtIssue().elementCursor();
//    Ld::ElementPointer       element       = elementCursor.element();
//    if (!element.isNull()) {
//        result = saveParagraphs(cursorAtIssue());

//        if (result) {
//            Ld::Cursor::Type cursorType = cursor()->cursorType();
//            if (cursorType == Ld::Cursor::Type::SELECTION) {
//                cursor()->removeSelection(cursorStateCollection);
//            }

//            if (currentText == QString("\n")) {
//                result = splitParentAtCursor(elementCursor, cursorStateCollection, true);

//                if (result == true) {
//                    markChangedParagraphs(element, 0, 1);

//                    cursorStateCollection->updateCursorState(false);
//                    cursor()->moveForwardByCharacter(false);
//                }
//            } else {
//                unsigned long textIndex   = elementCursor.textIndex();
//                unsigned      regionIndex = elementCursor.regionIndex();

//                element->insertText(currentText, textIndex, regionIndex, cursorStateCollection, false);
//                cursorStateCollection->updateCursorState(false);

//                markChangedParagraphs(element);

//                result = true;
//            }
//        }
//    } else {
//        result = false;
//    }

//    return result;
//}


//bool InsertStringCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
//    return removeChangedParagraphs(cursorStateCollection) && restoreSavedParagraphs(cursorStateCollection);
//}


//QString InsertStringCommand::description() const {
//    QString descriptionText;

//    if (currentText == QString("\n")) {
//        descriptionText = tr("Insert paragraph");
//    } else {
//        descriptionText = tr("Insert \"%1\"").arg(currentText);
//    }

//    return descriptionText;
//}


//void InsertStringCommand::setText(const QString& newString) {
//    currentText = newString;
//}


//QString InsertStringCommand::text() const {
//    return currentText;
//}
