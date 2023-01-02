/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref UpdateFormatCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QMap>
#include <QList>

#include <ld_format_structures.h>
#include <ld_element_structures.h>
#include <ld_element_cursor.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>

#include "command.h"
#include "fixer.h"
#include "update_format_command.h"

/***********************************************************************************************************************
 * UpdateFormatCommand::FormatsByElement
 */

UpdateFormatCommand::FormatsByElement::FormatsByElement() {}


UpdateFormatCommand::FormatsByElement::FormatsByElement(
        const FormatsByElement& other
    ):QMap<Ld::ElementPointer, Ld::FormatPointer>(
        other
    ) {}


UpdateFormatCommand::FormatsByElement::FormatsByElement(
        FormatsByElement&& other
    ):QMap<Ld::ElementPointer, Ld::FormatPointer>(
        other
    ) {}


UpdateFormatCommand::FormatsByElement::~FormatsByElement() {}


void UpdateFormatCommand::FormatsByElement::add(const UpdateFormatCommand::FormatsByElement& other) {
    for (FormatsByElement::const_iterator it=other.constBegin(),end=other.constEnd() ; it!=end ; ++it) {
        insert(it.key(), it.value());
    }
}


UpdateFormatCommand::FormatsByElement& UpdateFormatCommand::FormatsByElement::operator=(
        const UpdateFormatCommand::FormatsByElement& other
    ) {
    QMap<Ld::ElementPointer, Ld::FormatPointer>::operator=(other);
    return *this;
}


UpdateFormatCommand::FormatsByElement& UpdateFormatCommand::FormatsByElement::operator=(
        UpdateFormatCommand::FormatsByElement&& other
    ) {
    QMap<Ld::ElementPointer, Ld::FormatPointer>::operator=(other);
    return *this;
}

/***********************************************************************************************************************
 * UpdateFormatCommand
 */

UpdateFormatCommand::UpdateFormatCommand() {}


UpdateFormatCommand::UpdateFormatCommand(
        const UpdateFormatCommand::FormatsByElement& formatsByElement,
        bool                                         allowMerge
    ) {
    currentFormats       = toFormatsByPosition(formatsByElement);
    currentMergesAllowed = allowMerge;
}


UpdateFormatCommand::~UpdateFormatCommand() {}


Command::CommandType UpdateFormatCommand::commandType() const {
    return CommandType::UPDATE_FORMAT;
}


void UpdateFormatCommand::setFormatsByElement(const FormatsByElement& newFormatsByElement) {
    currentFormats = toFormatsByPosition(newFormatsByElement);
}


UpdateFormatCommand::FormatsByElement UpdateFormatCommand::formatsByElement() const {
    return toFormatsByElement(currentFormats);
}


void UpdateFormatCommand::setMergeAllowed(bool nowMergeAllowed) {
    currentMergesAllowed = nowMergeAllowed;
}


void UpdateFormatCommand::setMergeNotAllowed(bool nowMergeNotAllowed) {
    setMergeAllowed(!nowMergeNotAllowed);
}


bool UpdateFormatCommand::mergesAllowed() const {
    return currentMergesAllowed;
}


bool UpdateFormatCommand::mergesNotAllowed() const {
    return !currentMergesAllowed;
}


bool UpdateFormatCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;

    deletedFormats = updateFormats(currentFormats, currentMergesAllowed, cursorAtIssue(), cursorStateCollection);
    if (!deletedFormats.isEmpty()) {
        currentFormats.clear();
        success = true;
    } else {
        success = false;
    }

    return success;
}


bool UpdateFormatCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;
    if (!deletedFormats.isEmpty()) {
        currentFormats = updateFormats(deletedFormats, currentMergesAllowed, cursorAtIssue(), cursorStateCollection);
        if (!currentFormats.isEmpty()) {
            deletedFormats.clear();
            success = true;
        } else {
            success = false;
        }
    } else {
        success = false;
    }

    return success;
}


QString UpdateFormatCommand::description() const {
    QString result;

    if (currentFormats.count() == 1) {
        result = tr("Update format");
    } else {
        result = tr("Update multiple formats");
    }

    return result;
}


UpdateFormatCommand::FormatsByPosition UpdateFormatCommand::toFormatsByPosition(
        const UpdateFormatCommand::FormatsByElement& formatsByElement
    ) {
    FormatsByPosition result;

    for (  FormatsByElement::const_iterator pos = formatsByElement.constBegin(),
                                            end = formatsByElement.constEnd()
         ; pos != end
         ; ++pos
        ) {
        Ld::ElementPointer element = pos.key();
        Ld::FormatPointer  format  = pos.value();

        result.insert(Ld::ElementPosition(element), format);
    }

    return result;
}


UpdateFormatCommand::FormatsByElement UpdateFormatCommand::toFormatsByElement(
        const UpdateFormatCommand::FormatsByPosition& formatsByElement
    ) {
    FormatsByElement result;

    for (  FormatsByPosition::const_iterator pos = formatsByElement.constBegin(),
                                             end = formatsByElement.constEnd()
         ; pos != end
         ; ++pos
        ) {
        Ld::ElementPointer element = pos.key().element();
        Ld::FormatPointer  format  = pos.value();

        result.insert(element, format);
    }

    return result;
}


UpdateFormatCommand::FormatsByPosition UpdateFormatCommand::updateFormats(
        const UpdateFormatCommand::FormatsByPosition& formatsByPosition,
        bool                                          mergesAllowed,
        const Cursor&                                 cursor,
        Ld::CursorStateCollection*                    cursorStateCollection
    ) {
    FormatsByElement  formatsByElement = toFormatsByElement(formatsByPosition);
    FormatsByPosition oldFormats;

    bool cursorRestoreNeeded = false;
    bool success             = true;

    if (cursor.cursorType() == Cursor::Type::SELECTION) {
        const Ld::ElementCursor& elementCursor1 = cursor.elementCursor();
        const Ld::ElementCursor& elementCursor2 = cursor.selectionCursor();

        Ld::ElementCursor startCursor = elementCursor1 < elementCursor2  ? elementCursor1 : elementCursor2;
        Ld::ElementCursor endCursor   = elementCursor1 >= elementCursor2 ? elementCursor1 : elementCursor2;

        bool splitEndPosition   = false;
        bool splitStartPosition = false;

        if (!endCursor.isWholeElement()                    &&
            !endCursor.atEndOfElement()                    &&
            formatsByElement.contains(endCursor.element())    ) {
            if (isSplitAllowed(Fixer::SplitReason::FORMAT_CHANGE, endCursor)) {
                splitEndPosition    = true;
                cursorRestoreNeeded = true;
            } else {
                success = false;
            }
        }

        if (success                                           &&
            !startCursor.isWholeElement()                     &&
            !startCursor.atStartOfElement()                   &&
            formatsByPosition.contains(startCursor.element())    ) {
            if (isSplitAllowed(Fixer::SplitReason::FORMAT_CHANGE, startCursor)) {
                splitStartPosition  = true;
                cursorRestoreNeeded = true;
            } else {
                success = false;
            }
        }

        if (success) {
            if (splitEndPosition) {
                splitElement(Fixer::SplitReason::FORMAT_CHANGE, endCursor, cursorStateCollection);
            }

            if (splitStartPosition) {
                Ld::ElementPointer elementAtStart = startCursor.element();

                splitElement(Fixer::SplitReason::FORMAT_CHANGE, startCursor, cursorStateCollection);
                startCursor.moveForwardByCharacter();

                Ld::ElementPointer newStartElement = startCursor.element();

                Ld::FormatPointer newFormat = formatsByElement.value(elementAtStart);
                formatsByElement.insert(newStartElement, newFormat);
                formatsByElement.remove(elementAtStart);
            }
        }
    }

    if (success) {
        for (  FormatsByElement::const_iterator pos = formatsByElement.constBegin(),
                                                end = formatsByElement.constEnd()
             ; pos != end
             ; ++pos
            ) {
            Ld::ElementPointer element   = pos.key();
            Ld::FormatPointer  format    = pos.value();
            Ld::FormatPointer  oldFormat = element->format();

            element->setFormat(format);

            const Fixer* fixer = Fixer::fixer(element->typeName());
            if (fixer != Q_NULLPTR) {
                const Fixer* f = dynamic_cast<const Fixer*>(fixer);
                if (f != Q_NULLPTR) {
                    f->postFormatChange(element, oldFormat, format, cursorStateCollection);
                }
            }

            bool               lastChild;
            Ld::ElementPointer nextSibling = element->nextSibling(&lastChild);
            if (!lastChild && mergesAllowed && isMergeAllowed(element, nextSibling, true)) {
                Ld::ElementPointer parent = element->parent();

                mergeElements(element, nextSibling, cursorStateCollection);
                parent->removeChild(nextSibling, cursorStateCollection);

                cursorRestoreNeeded = true;
            }

            bool firstChild;
            Ld::ElementPointer previousSibling = element->previousSibling(&firstChild);
            if (!firstChild && mergesAllowed && isMergeAllowed(previousSibling, element, true)) {
                Ld::ElementPointer parent = element->parent();

                mergeElements(previousSibling, element, cursorStateCollection);
                parent->removeChild(element, cursorStateCollection);

                cursorRestoreNeeded = true;
            }

            oldFormats.insert(Ld::ElementPosition(element), oldFormat);
        }

        if (cursorRestoreNeeded) {
            cursorStateCollection->updateCursorState(false);
        }
    }

    return oldFormats;
}
