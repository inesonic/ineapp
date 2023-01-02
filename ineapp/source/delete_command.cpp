/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DeleteCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element_position.h>
#include <ld_element_cursor.h>
#include <ld_root_element.h>
#include <ld_paragraph_element.h>
#include <ld_text_element.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>
#include <ld_element_cursor.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>
#include <ld_format.h>

#include "fixer.h"
#include "cursor.h"
#include "command_container.h"
#include "command.h"
#include "delete_command.h"

DeleteCommand::DeleteCommand(bool deleteBehindCursor) {
    currentDeleteType         = DeleteType::UNKNOWN;
    currentDeleteBehindCursor = deleteBehindCursor;
    currentDeleteCount        = 1;
}


DeleteCommand::DeleteCommand(
        bool          deleteBehindCursor,
        CursorPointer newCursor
    ):CommandBaseWithUndo(
        newCursor
    ) {
    currentDeleteBehindCursor = deleteBehindCursor;
    currentDeleteCount        = 1;
}


DeleteCommand::~DeleteCommand() {}


Command::CommandType DeleteCommand::commandType() const {
    return Command::CommandType::DELETE_CONTENT;
}


bool DeleteCommand::merge(const CommandContainer& commandToMerge) {
    bool result = false;

    if (commandToMerge.commandType() == Command::CommandType::DELETE_CONTENT) {
        const DeleteCommand& otherCommand = dynamic_cast<const DeleteCommand&>(*commandToMerge);
        if (currentDeleteType == DeleteType::TEXT                               &&
            otherCommand.currentDeleteType == DeleteType::TEXT                  &&
            currentDeleteBehindCursor == otherCommand.currentDeleteBehindCursor &&
            firstSavedParagraph() == otherCommand.firstSavedParagraph()         &&
            numberSavedParagraphs() == otherCommand.numberSavedParagraphs()     &&
            firstChangedParagraph() == otherCommand.firstChangedParagraph()     &&
            numberChangedParagraphs() == otherCommand.numberChangedParagraphs()    ) {
            unsigned newDeleteCount = currentDeleteCount + otherCommand.currentDeleteCount;
            if (newDeleteCount <= maximumDeleteCount) {
                result = true;
                currentDeleteCount = newDeleteCount;
            }
        }
    }

    return result;
}


bool DeleteCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool success = true;

    Cursor::Type cursorType = cursorAtIssue().cursorType();
    if (cursorType == Cursor::Type::SELECTION) {
        currentDeleteType = DeleteType::SELECTION;
        success           = deleteSelection(cursorStateCollection);
    } else if (cursorType == Cursor::Type::TEXT || cursorType == Cursor::Type::ELEMENT) {
        Ld::ElementCursor deleteCursor = cursorAtIssue().elementCursor();

        unsigned deleteCount = 0;
        while (success && deleteCount < currentDeleteCount) {
            enum class DeleteAction {
                NO_DELETE_POSSIBLE,
                DELETE_AT_CURSOR,
                MERGE_PARAGRAPH,
            } deleteAction = DeleteAction::NO_DELETE_POSSIBLE;

            if (currentDeleteBehindCursor) {
                if (atStartOfParagraph(deleteCursor)) {
                    bool atStartOfDocument = deleteCursor.moveBackwardByCharacter();
                    atStartOfDocument = deleteCursor.fixPosition(false, atStartOfDocument);

                    if (!atStartOfDocument) {
                        Ld::ElementPointer deleteElement = deleteCursor.element();
                        if (deleteElement->typeName() == Ld::TextElement::elementName) {
                            deleteAction = DeleteAction::MERGE_PARAGRAPH;
                        } else {
                            deleteAction = DeleteAction::DELETE_AT_CURSOR;
                        }
                    }
                } else {
                    bool atStartOfDocument;
                    do {
                        atStartOfDocument = deleteCursor.moveBackwardByCharacter();
                    } while (!atStartOfDocument && deleteCursor.atEndOfElement());

                    if (!atStartOfDocument) {
                        deleteCursor.fixPosition(false, false);

                        if (!deleteCursor.atEndOfElement()) {
                            deleteAction = DeleteAction::DELETE_AT_CURSOR;
                        } else if (!deleteCursor.isWholeElement()) {
                            deleteAction = DeleteAction::MERGE_PARAGRAPH;
                        }
                    }
                }
            } else {
                bool atEndOfDocument = false;
                while (!atEndOfDocument                                                   &&
                       deleteCursor.atEndOfElement()                                      &&
                       deleteCursor.element()->typeName() != Ld::TextElement::elementName    ) {
                    deleteCursor.moveForwardByCharacter();
                }

                if (!atEndOfDocument) {
                    if (atEndOfParagraph(deleteCursor)) {
                        deleteAction = DeleteAction::MERGE_PARAGRAPH;
                    } else {
                        deleteAction = DeleteAction::DELETE_AT_CURSOR;
                    }
                }
            }

            switch (deleteAction) {
                case DeleteAction::NO_DELETE_POSSIBLE: {
                    success = false;
                    break;
                }

                case DeleteAction::DELETE_AT_CURSOR: {
                    if (deleteCursor.isWholeElement()) {
                        currentDeleteType = DeleteType::ELEMENT;
                        success           = deleteElement(deleteCursor, cursorStateCollection);
                    } else {
                        bool saveState = (deleteCount == 0);

                        currentDeleteType = DeleteType::TEXT;
                        success           = deleteText(deleteCursor, saveState, cursorStateCollection);
                    }

                    break;
                }

                case DeleteAction::MERGE_PARAGRAPH: {
                    currentDeleteType = DeleteType::MERGE_PARAGRAPHS;
                    success           = mergeParagraphs(deleteCursor, cursorStateCollection);
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            ++deleteCount;
        }
    } else if (cursorType == Cursor::Type::EMPTY_POSITION) {
        success = true;
    }

    return success;
}


bool DeleteCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    return removeChangedParagraphs(cursorStateCollection) && restoreSavedParagraphs(cursorStateCollection);
}


QString DeleteCommand::description() const {
    QString result;

    switch (currentDeleteType) {
        case DeleteType::UNKNOWN: {
            Cursor::Type cursorType = cursorAtIssue().cursorType();
            switch (cursorType) {
                case Cursor::Type::INVALID: {
                    if (currentDeleteBehindCursor) {
                        result = tr("Backspace");
                    } else {
                        result = tr("Delete");
                    }

                    break;
                }

                case Cursor::Type::TEXT: {
                    if (currentDeleteBehindCursor) {
                        result = tr("Backspace");
                    } else {
                        result = tr("Delete");
                    }

                    break;
                }

                case Cursor::Type::ELEMENT: {
                    result = tr("Delete language element(s)");
                    break;
                }

                case Cursor::Type::EMPTY_POSITION: {
                    result = tr("Non-destructive delete");
                    break;
                }

                case Cursor::Type::SELECTION: {
                    result = tr("Delete selection");
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case DeleteType::SELECTION: {
            result = tr("Delete selection");
            break;
        }

        case DeleteType::TEXT: {
            if (currentDeleteBehindCursor) {
                if (currentDeleteCount == 1) {
                    result = tr("Backspace through character");
                } else {
                    result = tr("Backspace through %1 characters").arg(currentDeleteCount);
                }
            } else {
                if (currentDeleteCount == 1) {
                    result = tr("Delete character");
                } else {
                    result = tr("Delete %1 characters").arg(currentDeleteCount);
                }
            }

            break;
        }

        case DeleteType::ELEMENT: {
            result = tr("Delete language element(s)");
            break;
        }

        case DeleteType::MERGE_PARAGRAPHS: {
            result = tr("Combine paragraphs");
            break;
        }
    }

    return result;
}


void DeleteCommand::setDeleteMode(bool deleteBehindCursor) {
    currentDeleteBehindCursor = deleteBehindCursor;
}


bool DeleteCommand::deleteMode() const {
    return currentDeleteBehindCursor;
}


bool DeleteCommand::deleteSelection(Ld::CursorStateCollection* cursorStateCollection) {
    Cursor                   cursor          = cursorAtIssue();
    const Ld::ElementCursor& elementCursor   = cursor.elementCursor();
    const Ld::ElementCursor& selectionCursor = cursor.selectionCursor();
    Ld::ElementCursor        leadingCursor   = elementCursor < selectionCursor ? elementCursor : selectionCursor;
    Ld::FormatPointer        leadingFormat   = leadingCursor.element()->format();

    saveParagraphs(cursor);
    cursor.removeSelection(cursorStateCollection);

    // If we delete all the document contents, we can end up with a scenario where we have a paragraph element with
    // no children.  We handle this scenario here by re-inserting an empty text element.
    Ld::ElementPointer leadingParent = leadingCursor.parent();
    if (leadingParent->typeName() == Ld::ParagraphElement::elementName && leadingParent->numberChildren() == 0) {
        QSharedPointer<Ld::ParagraphElement> paragraphElement = leadingParent.dynamicCast<Ld::ParagraphElement>();

        Ld::ElementPointer emptyText = Ld::Element::create(Ld::TextElement::elementName);
        emptyText->setFormat(leadingFormat);

        paragraphElement->append(emptyText, cursorStateCollection);

        Cursor referenceCursor;
        referenceCursor.setRootElement(paragraphElement->root().dynamicCast<Ld::RootElement>().toWeakRef());
        referenceCursor.updateFromElementCursor(Ld::ElementCursor(0, 0, emptyText));

        cursorStateCollection->setCursorsTo(referenceCursor);
    } else {
        if (leadingCursor.isValid()) {
            markChangedParagraphs(leadingCursor.element());
        } else {
            markChangedParagraphs(leadingParent->child(leadingParent->numberChildren() - 1));
        }

        cursorStateCollection->updateCursorState(true);
    }

    return true;
}


bool DeleteCommand::deleteElement(
        const Ld::ElementPosition& deletePosition,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    bool success;

    Ld::ElementPointer element = deletePosition.element();
    if (isDeleteAllowed(element, Fixer::DeleteReason::ELEMENT_SELECTED_FOR_DELETE)) {
        saveParagraphs(element);

        if (element->parent()->typeName() != Ld::RootElement::elementName) {
            markChangedParagraphs(element);
            removeElementAndMerge(element, cursorStateCollection);
        } else {
            Ld::ElementPointer sibling = element->previousSibling();
            if (sibling.isNull()) {
                sibling = element->nextSibling();
            }

            markNoChangedParagraphs(sibling);

            Ld::ElementPointer root = element->parent();
            root->removeChild(element, cursorStateCollection);
        }

        cursorStateCollection->updateCursorState(true);

        success = true;
    } else {
        success = false;
    }

    return success;
}


bool DeleteCommand::deleteText(
        const Ld::ElementCursor&   deleteCursor,
        bool                       saveState,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    Ld::ElementPointer element     = deleteCursor.element();
    unsigned long      textIndex   = deleteCursor.textIndex();
    unsigned           regionIndex = deleteCursor.regionIndex();

    if (saveState) {
        saveParagraphs(element);
        markChangedParagraphs(element);
    }

    element->removeText(textIndex, regionIndex, textIndex+ 1, regionIndex, cursorStateCollection);
    if (currentDeleteBehindCursor) {
        cursor()->updateFromElementCursor(deleteCursor, false);
    }

    if (element->text(regionIndex).isEmpty()) {
        unsigned numberRegions = element->numberTextRegions();
        unsigned index         = 0;
        while (index < numberRegions && element->text(index).isEmpty()) {
            ++index;
        }

        if (index >= numberRegions && isDeleteAllowed(element, Fixer::DeleteReason::LAST_TEXT_DELETED)) {
            removeElementAndMerge(element, cursorStateCollection);
        }
    }

    cursorStateCollection->updateCursorState(true);
    return true;
}


bool DeleteCommand::mergeParagraphs(
        const Ld::ElementCursor&   deleteCursor,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    bool success = false;

    Ld::ElementPointer firstParagraph = deleteCursor.element();
    while (!firstParagraph.isNull() && firstParagraph->typeName() != Ld::ParagraphElement::elementName) {
        firstParagraph = firstParagraph->parent();
    }

    if (!firstParagraph.isNull()) {
        Ld::ElementPointer parent = firstParagraph->parent();
        if (!parent.isNull()) {
            unsigned long firstParagraphIndex = parent->indexOfChild(firstParagraph);

            if ((firstParagraphIndex + 1) < parent->numberChildren()) {
                Ld::ElementPointer secondParagraph = parent->child(firstParagraphIndex + 1);

                if ((parent->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL ||
                     parent->childPlacement() == Ld::Element::ChildPlacement::GROUPED       )          &&
                    (firstParagraph->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL ||
                     firstParagraph->childPlacement() == Ld::Element::ChildPlacement::GROUPED       )  &&
                    (secondParagraph->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL ||
                     secondParagraph->childPlacement() == Ld::Element::ChildPlacement::GROUPED       ) &&
                    isMergeAllowed(firstParagraph, secondParagraph)                                       ) {
                    saveParagraphs(firstParagraph, 0, 1);
                    markChangedParagraphs(firstParagraph);

                    unsigned long firstParagraphNumberChildren  = firstParagraph->numberChildren();
                    unsigned long secondParagraphNumberChildren = secondParagraph->numberChildren();

                    QSharedPointer<Ld::ElementWithPositionalChildren>
                        fp = firstParagraph.dynamicCast<Ld::ElementWithPositionalChildren>();

                    QSharedPointer<Ld::ElementWithPositionalChildren>
                        sp = secondParagraph.dynamicCast<Ld::ElementWithPositionalChildren>();

                    unsigned long secondParagraphIndex = 0;
                    if (secondParagraphNumberChildren > 0) {
                        if (firstParagraphNumberChildren > 0) {
                            Ld::ElementPointer left  = fp->child(firstParagraphNumberChildren - 1);
                            Ld::ElementPointer right = sp->child(0);

                            if (isMergeAllowed(left, right, true)) {
                                mergeElements(left, right, cursorStateCollection);
                                secondParagraphIndex = 1;
                            }
                        }
                    }

                    for (unsigned long i=secondParagraphIndex ; i<secondParagraphNumberChildren ; ++i) {
                        Ld::ElementPointer child = sp->child(secondParagraphIndex);
                        fp->append(child, cursorStateCollection);
                    }

                    parent->removeChild(secondParagraph, cursorStateCollection);

                    cursorStateCollection->updateCursorState(true);
                    success = true;
                }
            }
        }
    }

    return success;
}
