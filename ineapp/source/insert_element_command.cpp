/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref InsertElementCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_element_with_floating_children.h>
#include <ld_element_with_positional_children.h>
#include <ld_element_with_grouped_children.h>
#include <ld_text_element.h>
#include <ld_placeholder_element.h>
#include <ld_root_element.h>

#include "fixer.h"
#include "cursor.h"
#include "command_container.h"
#include "command.h"
#include "insert_element_command.h"

InsertElementCommand::InsertElementCommand() {
    currentFinalCursorPosition = FinalCursorPosition::UNCHANGED;
    currentInsertionLocation   = InsertionLocation::AT_CURSOR;
    currentInsertionMode       = InsertionMode::NO_INSERTION_PERFORMED;
}


InsertElementCommand::InsertElementCommand(
        Ld::ElementPointer                        newElement,
        InsertElementCommand::InsertionLocation   insertionLocation,
        InsertElementCommand::FinalCursorPosition finalCursorPosition
    ) {
    currentElement             = newElement;
    currentFinalCursorPosition = finalCursorPosition;
    currentInsertionLocation   = insertionLocation;
    currentInsertionMode       = InsertionMode::NO_INSERTION_PERFORMED;
}


InsertElementCommand::InsertElementCommand(
        Ld::ElementPointer                        newElement,
        CursorPointer                             newCursor,
        InsertElementCommand::InsertionLocation   insertionLocation,
        InsertElementCommand::FinalCursorPosition finalCursorPosition
    ):CommandBase(
          newCursor
    ) {
    currentElement             = newElement;
    currentFinalCursorPosition = finalCursorPosition;
    currentInsertionLocation   = insertionLocation;
    currentInsertionMode       = InsertionMode::NO_INSERTION_PERFORMED;
}


InsertElementCommand::~InsertElementCommand() {}


Command::CommandType InsertElementCommand::commandType() const {
    return Command::CommandType::INSERT_ELEMENT;
}


bool InsertElementCommand::merge(const CommandContainer&) {
    return false;
}


bool InsertElementCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    // NOTE: The editor does not allow this operation to be performed while a selection is active.
    Q_ASSERT(cursorAtIssue().cursorType() != Cursor::Type::SELECTION);

    Ld::ElementPointer newElement = InsertElementCommand::element();
    const Fixer*       fixer      = Fixer::fixer(newElement->typeName());
    if (fixer != Q_NULLPTR) {
        fixer->preInsert(newElement);
    }

    const Ld::ElementCursor& elementCursor = cursorAtIssue().elementCursor();
    Ld::ElementPointer       element       = elementCursor.element();

    if (element.isNull()) {
        result = insertIntoEmptyPosition(cursorStateCollection);
    } else if (currentInsertionLocation == InsertionLocation::AT_CURSOR    ||
               currentInsertionLocation == InsertionLocation::AFTER_CURSOR    ) {
        if (isSplitAllowed(Fixer::SplitReason::INSERT_ELEMENT, elementCursor)             &&
            element->parent()->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL   ) {
            result = splitAndInsert(cursorStateCollection);
        } else {
            result = insertAtPosition(cursorStateCollection);
        }
    } else if (currentInsertionLocation == InsertionLocation::REPLACE_ELEMENT) {
        result = insertAtPosition(cursorStateCollection);
    } else if (currentInsertionLocation == InsertionLocation::UNDER_ROOT                   &&
               isSplitAllowed(Fixer::SplitReason::INSERT_ELEMENT_TOP_LEVEL, elementCursor)    ) {
        result = splitToRootAndInsert(cursorStateCollection);
    } else {
        result = false;
    }

    return result;
}


bool InsertElementCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool result;

    // NOTE: The editor does not allow the original operation to be performed while a selection is active.
    Q_ASSERT(cursorAtIssue().cursorType() != Cursor::Type::SELECTION);

    switch (currentInsertionMode) {
        case InsertionMode::NO_INSERTION_PERFORMED: {
            result = false;
            break;
        }

        case InsertionMode::INSERT_INTO_EMPTY_POSITION: {
            result = undoInsertIntoEmptyPosition(cursorStateCollection);
            break;
        }

        case InsertionMode::SPLIT_AND_INSERT: {
            result = undoSplitAndInsert(cursorStateCollection);
            break;
        }

        case InsertionMode::SPLIT_AT_ROOT_AND_INSERT: {
            result = undoSplitAtRootAndInsert(cursorStateCollection);
            break;
        }

        case InsertionMode::INSERT_AT_POSITION: {
            result = undoInsertAtPosition(cursorStateCollection);
            break;
        }

        default: {
            Q_ASSERT(false);
            result = false;
            break;
        }
    }

    return result;
}


QString InsertElementCommand::description() const {
    return !currentElement.isNull() ? tr("insert %1").arg(currentElement->description())
                                    : currentDescription;
}


void InsertElementCommand::setElement(Ld::ElementPointer newElement) {
    currentElement = newElement;
}


Ld::ElementPointer InsertElementCommand::element() const {
    return currentElement;
}


void InsertElementCommand::setInsertionLocation(InsertionLocation newInsertionLocation) {
    currentInsertionLocation = newInsertionLocation;
}


InsertElementCommand::InsertionLocation InsertElementCommand::insertionLocation() const {
    return currentInsertionLocation;
}


void InsertElementCommand::setFinalCursorPosition(
        InsertElementCommand::FinalCursorPosition newFinalCursorPosition
    ) {
    currentFinalCursorPosition = newFinalCursorPosition;
}


InsertElementCommand::FinalCursorPosition InsertElementCommand::finalCursorPosition() const {
    return currentFinalCursorPosition;
}


bool InsertElementCommand::insertIntoEmptyPosition(Ld::CursorStateCollection* /* cursorStateCollection */) {
    // Because of the fixers, we should neve have an empty position so we assert.

    Q_ASSERT(false);
    return false;
}


bool InsertElementCommand::splitAndInsert(Ld::CursorStateCollection* cursorStateCollection) {
    bool success = splitElementAndInsert(
        Fixer::SplitReason::INSERT_ELEMENT,
        cursorAtIssue().elementCursor(),
        currentElement,
        cursorStateCollection
    );

    if (success) {
       cursorStateCollection->updateCursorState(false);

        currentInsertionMode = InsertionMode::SPLIT_AND_INSERT;
        currentDescription   = description();

        oldContents.reset();

        updateFinalCursorPosition();
    }

    return success;
}


bool InsertElementCommand::splitToRootAndInsert(Ld::CursorStateCollection* cursorStateCollection) {
    bool success = splitElementAndInsert(
        Fixer::SplitReason::INSERT_ELEMENT_TOP_LEVEL,
        cursorAtIssue().elementCursor(),
        currentElement,
        cursorStateCollection
    );

    if (success) {
        cursorStateCollection->updateCursorState(false);

        currentInsertionMode = InsertionMode::SPLIT_AT_ROOT_AND_INSERT;
        currentDescription   = description();

        oldContents.reset();

        updateFinalCursorPosition();
    }

    return success;
}


bool InsertElementCommand::insertAtPosition(Ld::CursorStateCollection* cursorStateCollection) {
    bool success = false;

    const Cursor& cursor = cursorAtIssue();
    Q_ASSERT(
           cursor.cursorType() == Cursor::Type::ELEMENT
        || cursor.cursorType() == Cursor::Type::EMPTY_POSITION
        || cursor.cursorType() == Cursor::Type::TEXT
    );

    const Ld::ElementCursor& elementCursor = cursor.elementCursor();
    Ld::ElementPointer       parent        = elementCursor.parent();
    unsigned long            childIndex    = elementCursor.childIndex();

    switch (parent->childPlacement()) {
        case Ld::Element::ChildPlacement::NONE: {
            Q_ASSERT(false);
            break;
        }

        case Ld::Element::ChildPlacement::FIXED:
        case Ld::Element::ChildPlacement::GRID: {
            QSharedPointer<Ld::ElementWithFixedChildren>
                fixedParent = parent.dynamicCast<Ld::ElementWithFixedChildren>();

            oldContents = fixedParent->child(childIndex);
            fixedParent->setChild(childIndex, currentElement, cursorStateCollection);

            updateFinalCursorPosition();

            currentInsertionMode = InsertionMode::INSERT_AT_POSITION;
            success = true;

            break;
        }

        case Ld::Element::ChildPlacement::POSITIONAL: {
            QSharedPointer<Ld::ElementWithPositionalChildren>
                positionalElement = parent.dynamicCast<Ld::ElementWithPositionalChildren>();

            if (currentInsertionLocation == InsertionLocation::AT_CURSOR) {
                if (positionalElement->child(childIndex)->isPlaceholder() && !currentElement->isPlaceholder()) {
                    oldContents = positionalElement->child(childIndex);
                    positionalElement->removeChild(childIndex, nullptr);
                } else {
                    oldContents.clear();
                }

                positionalElement->insertBefore(childIndex, currentElement, cursorStateCollection);
            } else if (currentInsertionLocation == InsertionLocation::REPLACE_ELEMENT) {
                oldContents = positionalElement->child(childIndex);

                positionalElement->removeChild(childIndex, nullptr);
                positionalElement->insertBefore(childIndex, currentElement, cursorStateCollection);
            } else {
                Q_ASSERT(currentInsertionLocation == InsertionLocation::AFTER_CURSOR);
                positionalElement->insertAfter(childIndex, currentElement, cursorStateCollection);
                oldContents.clear();
            }

            updateFinalCursorPosition();

            currentInsertionMode = InsertionMode::INSERT_AT_POSITION;
            success = true;

            break;
        }

        case Ld::Element::ChildPlacement::FLOATING: {
            success = false; // FIXME
            break;
        }

        case Ld::Element::ChildPlacement::GROUPED: {
            success = false; // FIXME
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return success;
}


bool InsertElementCommand::undoInsertIntoEmptyPosition(Ld::CursorStateCollection* /* cursorStateCollection */) {
    // We should never have an empty position so this function simply asserts.
    Q_ASSERT(false);
    return false;
}


bool InsertElementCommand::undoSplitAndInsert(Ld::CursorStateCollection* cursorStateCollection) {
    bool success = false;

    const Ld::ElementCursor& elementCursor = cursorAtIssue().elementCursor();
    Ld::ElementPointer       leftElement   = elementCursor.element();
    if (!leftElement.isNull()) {
        bool                    lastSibling;
        Ld::ElementPointer insertedElement = leftElement->nextSibling(&lastSibling);
        if (!lastSibling && !insertedElement.isNull()) {
            Ld::ElementPointer rightElement = insertedElement->nextSibling(&lastSibling);
            if (!lastSibling && !insertedElement.isNull()) {
                Ld::ElementPointer rawParent = insertedElement->parent();
                if (rawParent->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL) {
                    QSharedPointer<Ld::ElementWithPositionalChildren> parent =
                        rawParent.dynamicCast<Ld::ElementWithPositionalChildren>();

                    if (isMergeAllowed(leftElement, rightElement)) {
                        mergeElements(leftElement, rightElement, cursorStateCollection);
                        parent->removeChild(insertedElement, cursorStateCollection);
                        parent->removeChild(rightElement, cursorStateCollection);

                        currentInsertionMode = InsertionMode::NO_INSERTION_PERFORMED;
                        currentElement       = insertedElement;
                        currentDescription.clear();

                        *cursor() = cursorAtIssue();
                        success = true;
                    }
                }
            }
        }
    }

    return success;
}


bool InsertElementCommand::undoSplitAtRootAndInsert(Ld::CursorStateCollection* cursorStateCollection) {
    Ld::ElementPointer removedElement = removeTopLevelElementAndMerge(
        cursorAtIssue().elementCursor(),
        cursorStateCollection
    );

    bool success;
    if (!removedElement.isNull()) {
        currentElement = removedElement;
        currentDescription.clear();

        success = true;
    } else {
        success = false;
    }

    return success;
}


bool InsertElementCommand::undoInsertAtPosition(Ld::CursorStateCollection* cursorStateCollection) {
    bool success = false;

    const Cursor& cursor = cursorAtIssue();
    Q_ASSERT(
           cursor.cursorType() == Cursor::Type::ELEMENT
        || cursor.cursorType() == Cursor::Type::EMPTY_POSITION
        || cursor.cursorType() == Cursor::Type::TEXT
    );

    const Ld::ElementCursor& elementCursor = cursor.elementCursor();
    Ld::ElementPointer       parent        = elementCursor.parent();
    unsigned long            childIndex    = elementCursor.childIndex();

    switch (parent->childPlacement()) {
        case Ld::Element::ChildPlacement::NONE: {
            Q_ASSERT(false);
            break;
        }

        case Ld::Element::ChildPlacement::FIXED: {
            QSharedPointer<Ld::ElementWithFixedChildren>
                fixedParent = parent.dynamicCast<Ld::ElementWithFixedChildren>();

            currentElement = fixedParent->child(childIndex);
            fixedParent->setChild(childIndex, oldContents, cursorStateCollection);

            success = true;
            break;
        }

        case Ld::Element::ChildPlacement::POSITIONAL: {
            QSharedPointer<Ld::ElementWithPositionalChildren>
                positionalParent = parent.dynamicCast<Ld::ElementWithPositionalChildren>();

            currentElement = positionalParent->child(childIndex);
            if (currentInsertionLocation == InsertionLocation::AT_CURSOR) {
                currentElement = positionalParent->child(childIndex);

                if (!oldContents.isNull()) {
                    positionalParent->removeChild(childIndex, Q_NULLPTR);
                    positionalParent->insertBefore(childIndex, oldContents, cursorStateCollection);

                    oldContents.clear();
                } else {
                    positionalParent->removeChild(childIndex, cursorStateCollection);
                }
            } else {
                Q_ASSERT(currentInsertionLocation == InsertionLocation::AFTER_CURSOR);
                positionalParent->removeChild(childIndex + 1, cursorStateCollection);
            }

            success = true;
            break;
        }

        case Ld::Element::ChildPlacement::FLOATING: {
            success = false; // FIXME
            break;
        }

        case Ld::Element::ChildPlacement::GROUPED: {
            success = false; // FIXME
            break;
        }

        case Ld::Element::ChildPlacement::GRID: {
            success = false; // FIXME
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return success;
}


void InsertElementCommand::updateFinalCursorPosition() {
    CursorPointer cursor = InsertElementCommand::cursor();
    switch (currentFinalCursorPosition) {
        case FinalCursorPosition::UNCHANGED: {
            break;
        }

        case FinalCursorPosition::ELEMENT_WHOLE: {
            cursor->updateFromElementCursor(Ld::ElementCursor(currentElement), false);
            break;
        }

        case FinalCursorPosition::ELEMENT_START_OF_TEXT: {
            if (currentElement->numberTextRegions() > 0) {
                Ld::ElementCursor newElementCursor(currentElement);
                newElementCursor.moveToFirstPositionInElement();

                cursor->updateFromElementCursor(newElementCursor, false);
            }

            break;
        }

        case FinalCursorPosition::ELEMENT_END_OF_TEXT: {
            if (currentElement->numberTextRegions() > 0) {
                Ld::ElementCursor newElementCursor(currentElement);
                newElementCursor.moveToLastPositionInElement();

                cursor->updateFromElementCursor(newElementCursor, false);
            }

            break;
        }

        case FinalCursorPosition::FIRST_CHILD_ELEMENT: {
            if (currentElement->numberChildren() > 0) {
                Ld::ElementCursor newElementCursor(currentElement->child(0));
                newElementCursor.moveToFirstPositionInElement();

                cursor->updateFromElementCursor(newElementCursor, false);
            } else {
                Q_ASSERT(false); // Can't point to null positions.
            }

            break;
        }

        case FinalCursorPosition::FIRST_CHILD_START_OF_TEXT: {
            if (currentElement->numberChildren() > 0) {
                Ld::ElementCursor newElementCursor(currentElement->child(0));
                newElementCursor.moveToFirstPositionInElement();

                cursor->updateFromElementCursor(newElementCursor, false);
            }

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}
