/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref EditHelpers class.
***********************************************************************************************************************/

#include <QPointer>
#include <QSharedPointer>
#include <QString>

#include <ld_element_structures.h>
#include <ld_cursor_state_collection.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_element_with_positional_children.h>
#include <ld_element_with_grouped_children.h>
#include <ld_root_element.h>
#include <ld_paragraph_element.h>

#include "cursor.h"
#include "fixer.h"
#include "edit_helpers.h"

EditHelpers::EditHelpers() {}


EditHelpers::~EditHelpers() {}


bool EditHelpers::atStartOfParagraph(const Ld::ElementCursor& cursor) {
    bool result = false;

    Ld::ElementPointer element = cursor.element();
    if (!element.isNull() && (cursor.isWholeElement() || cursor.atStartOfElement())) {
        Ld::ElementPointer parent = element->parent();
        while (!parent.isNull()                                        &&
               parent->typeName() != Ld::ParagraphElement::elementName &&
               parent->indexOfChild(element) == 0                         ) {
            element = parent;
            parent  = element->parent();
        }

        result = (
               !parent.isNull()
            && parent->typeName() == Ld::ParagraphElement::elementName
            && parent->indexOfChild(element) == 0
        );
    }

    return result;
}


bool EditHelpers::atEndOfParagraph(const Ld::ElementCursor& cursor) {
    bool result = false;

    Ld::ElementPointer element = cursor.element();
    if (!element.isNull() && cursor.atEndOfElement()) {
        Ld::ElementPointer parent = element->parent();
        while (!parent.isNull()                                                &&
               parent->typeName() != Ld::ParagraphElement::elementName         &&
               parent->indexOfChild(element) == (parent->numberChildren() - 1)    ) {
            element = parent;
            parent  = element->parent();
        }

        result = (
               !parent.isNull()
            && parent->typeName() == Ld::ParagraphElement::elementName
            && parent->indexOfChild(element) == (parent->numberChildren() - 1)
        );
    }

    return result;
}


bool EditHelpers::isDeleteAllowed(Ld::ElementPointer element, Fixer::DeleteReason deleteReason) {
    bool         canDelete;
    const Fixer* elementFixer = Fixer::fixer(element->typeName());
    if (elementFixer != Q_NULLPTR) {
        canDelete = elementFixer->isDeleteAllowed(element, deleteReason);
    } else {
        canDelete = true;
    }

    if (canDelete) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            const Fixer* parentFixer = Fixer::fixer(parent->typeName());
            if (parentFixer != Q_NULLPTR) {
                canDelete = parentFixer->isDeleteChildAllowed(parent, element, deleteReason);
            }
        }
    }

    return canDelete;
}


bool EditHelpers::isMergeAllowed(
        Ld::ElementPointer leftElement,
        Ld::ElementPointer rightElement,
        bool               checkFormats,
        bool               checkParent
    ) {
    bool result = false;

    if (!leftElement.isNull() && !rightElement.isNull()) {
        const Fixer* leftFixer  = Fixer::fixer(leftElement->typeName());
        const Fixer* rightFixer = Fixer::fixer(rightElement->typeName());

        if (leftFixer != Q_NULLPTR                                                    &&
            rightFixer != Q_NULLPTR                                                   &&
            leftFixer->isMergeAllowed(leftElement, rightElement, true, checkFormats)  &&
            rightFixer->isMergeAllowed(leftElement, rightElement, true, checkFormats)    ) {
            Ld::ElementPointer leftParent  = leftElement->parent();
            Ld::ElementPointer rightParent = rightElement->parent();

            if (checkParent && leftParent == rightParent) {
                const Fixer* parentFixer = Fixer::fixer(leftParent->typeName());
                if (parentFixer != Q_NULLPTR) {
                    result = parentFixer->isMergeChildrenAllowed(leftElement, rightElement);
                } else {
                    result = true;
                }
            } else {
                result = true;
            }
        }
    }

    return result;
}


bool EditHelpers::isSplitAllowed(Fixer::SplitReason reason, const Ld::ElementCursor& cursor) {
    bool canSplit = false;

    Ld::ElementPointer element = cursor.element();
    if (!element.isNull()) {
        const Fixer* fixer = Fixer::fixer(element->typeName());
        if (fixer != Q_NULLPTR) {
            canSplit = fixer->isSplitAllowed(element, reason, cursor);
        }
    }

    return canSplit;
}


Ld::ElementCursor EditHelpers::mergeElements(
        Ld::ElementPointer         leftElement,
        Ld::ElementPointer         rightElement,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    const Fixer* leftFixer = Fixer::fixer(leftElement->typeName());
    return leftFixer->mergeElements(leftElement, rightElement, cursorStateCollection);
}


void EditHelpers::splitElement(
        Fixer::SplitReason         reason,
        const Ld::ElementCursor&   cursor,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    Ld::ElementPointer element = cursor.element();
    const Fixer*       fixer   = Fixer::fixer(element->typeName());

    fixer->splitElement(reason, cursor, cursorStateCollection);
}


bool EditHelpers::splitElementAndInsert(
        Fixer::SplitReason         reason,
        const Ld::ElementCursor&   cursor,
        Ld::ElementPointer         elementToInsert,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    bool success = false;

    if (isSplitAllowed(reason, cursor)) {
        if (reason != Fixer::SplitReason::INSERT_ELEMENT_TOP_LEVEL) {
            Ld::ElementPointer element = cursor.element();
            Ld::ElementPointer parent  = element->parent();

            if (parent->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL) {
                QSharedPointer<Ld::ElementWithPositionalChildren>
                    positionalParent = parent.dynamicCast<Ld::ElementWithPositionalChildren>();

                unsigned long indexOfChild = positionalParent->indexOfChild(element);

                splitElement(reason, cursor, cursorStateCollection);

                positionalParent->insertAfter(indexOfChild, elementToInsert, cursorStateCollection);
                success = true;
            }
        } else {
            Ld::ElementPointerList elementStack = cursor.elementStack();
            if (elementStack.size() >= 2) {
                QSharedPointer<Ld::RootElement> rootElement = elementStack.at(0).dynamicCast<Ld::RootElement>();
                Q_ASSERT(!rootElement.isNull());

                Ld::ElementPointer leftSibling = elementStack.at(1);
                unsigned long leftSiblingIndex = rootElement->indexOfChild(leftSibling);

                splitElement(reason, cursor, cursorStateCollection);

                rootElement->insertAfter(leftSiblingIndex, elementToInsert, cursorStateCollection);
                success = true;
            }
        }
    }

    return success;
}


Ld::ElementPointer EditHelpers::removeTopLevelElementAndMerge(
        const Ld::ElementCursor&   cursor,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    // Note that some classes, such as paragraphs, will automatically merge children when they're re-combined.  We
    // check this scenario by checking for merges down the stack and allowing merges so long as the formats are
    // compatible.  This should work so long as we're consistent about merging mergeable elements with compatible
    // formats.

    Ld::ElementPointer     result;
    Ld::ElementPointerList elementStack = cursor.elementStack();
    unsigned               stackDepth   =  static_cast<unsigned>(elementStack.size());

    if (stackDepth >= 2) {
        Ld::ElementPointer parent = elementStack.at(0);

        Q_ASSERT(parent->typeName() == Ld::RootElement::elementName);

        QSharedPointer<Ld::RootElement> rootElement        = parent.dynamicCast<Ld::RootElement>();
        Ld::ElementPointer              leftChild          = elementStack.at(1);
        unsigned long                   childIndex         = rootElement->indexOfChild(leftChild);
        unsigned long                   rootNumberChildren = rootElement->numberChildren();

        if ((childIndex + 1) < rootNumberChildren) {
            result = rootElement->child(childIndex + 1);
            rootElement->removeChild(childIndex + 1, cursorStateCollection);

            unsigned stackIndex = 1;
            while (stackIndex < stackDepth && !parent.isNull()) {
                Ld::Element::ChildPlacement childPlacement = parent->childPlacement();
                if (childPlacement == Ld::Element::ChildPlacement::POSITIONAL ||
                    childPlacement == Ld::Element::ChildPlacement::GROUPED       ) {
                    Ld::ElementPointer leftChild  = elementStack.at(stackIndex);
                    Ld::ElementPointer rightChild = leftChild->nextSibling();

                    if (!leftChild.isNull() && !rightChild.isNull()) {
                        if (isMergeAllowed(leftChild, rightChild, true)) {
                            mergeElements(leftChild, rightChild, cursorStateCollection);
                            parent->removeChild(rightChild, Q_NULLPTR);

                            parent = leftChild;
                            ++stackIndex;
                        } else {
                            parent.reset();
                        }
                    } else {
                        parent = leftChild;
                        ++stackIndex;
                    }
                } else {
                    parent.reset();
                }
            }

            cursorStateCollection->updateCursorState(false);
        }
    }

    return result;
}


bool EditHelpers::mergeAcrossNextParent(
        const Ld::ElementCursor&   cursor,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    bool success = false;

    Ld::ElementPointer element = cursor.element();
    if (!element.isNull()) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            Ld::ElementPointer grandparent = parent->parent();
            if (!grandparent.isNull()) {
                Ld::Element::ChildPlacement grandparentChildPlacement = grandparent->childPlacement();
                if (grandparentChildPlacement == Ld::Element::ChildPlacement::POSITIONAL ||
                    grandparentChildPlacement == Ld::Element::ChildPlacement::GROUPED       ) {
                    unsigned long parentIndex = grandparent->indexOfChild(parent);
                    if (parentIndex < grandparent->numberChildren() - 1) {
                        Ld::ElementPointer nextParentSibling = grandparent->child(parentIndex + 1);
                        if (isMergeAllowed(parent, nextParentSibling)) {
                            Ld::ElementCursor mergeCursor = mergeElements(
                                parent,
                                nextParentSibling,
                                cursorStateCollection
                            );

                            grandparent->removeChild(nextParentSibling, cursorStateCollection);

                            success = true;
                        }
                    }
                }
            }
        }
    }

    return success;
}


Ld::ElementCursor EditHelpers::removeElementAndMerge(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    Ld::ElementCursor result;

    Ld::ElementPointer          parent    = element->parent();
    unsigned long               index     = parent->indexOfChild(element);
    Ld::Element::ChildPlacement placement = parent->childPlacement();

    if (placement == Ld::Element::ChildPlacement::POSITIONAL || placement == Ld::Element::ChildPlacement::GROUPED) {
        deleteElement(parent, element, cursorStateCollection);
        unsigned long numberChildren = parent->numberChildren();

        if (index > 0) {
            if ((index + 1) < numberChildren) {
                Ld::ElementPointer previous = parent->child(index - 1);
                Ld::ElementPointer next     = parent->child(index);

                if (isMergeAllowed(previous, next, true)) {
                    result = mergeElements(previous, next, cursorStateCollection);
                    parent->removeChild(next, Q_NULLPTR);
                } else {
                    result.setElementUnderParent(parent, index - 1);
                    result.moveToLastPositionInElement();
                }
            } else {
                result.setElementUnderParent(parent, index - 1);
                result.moveToLastPositionInElement();
            }
        } else {
            result.setElementUnderParent(parent, index);
            result.moveToFirstPositionInElement();
        }
    } else {
        deleteElement(parent, element, cursorStateCollection);
        result.setElementUnderParent(parent, index);
    }

    return result;
}


void EditHelpers::deleteElement(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    const Fixer* fixer = Fixer::fixer(parent->typeName());
    if (fixer != Q_NULLPTR) {
        fixer->deleteChild(parent, child, cursorStateCollection);
    } else {
        Q_ASSERT(parent->childPlacement() == Ld::Element::ChildPlacement::FIXED);
        Q_ASSERT(parent->childPlacement() == Ld::Element::ChildPlacement::GRID);

        parent->removeChild(child, cursorStateCollection);
    }
}


void EditHelpers::deleteElementInSelection(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    const Fixer* fixer = Fixer::fixer(parent->typeName());
    if (fixer != Q_NULLPTR) {
        fixer->deleteChild(parent, child, cursorStateCollection);
    } else {
        Q_ASSERT(parent->childPlacement() == Ld::Element::ChildPlacement::FIXED);
        Q_ASSERT(parent->childPlacement() == Ld::Element::ChildPlacement::GRID);

        parent->removeChild(child, cursorStateCollection);
    }
}


bool EditHelpers::hasNoOtherChildren(Ld::ElementPointer element, Ld::ElementPointer childToIgnore) {
    bool noChildren = true;

    Ld::ElementPointerList                 children   = element->children();
    Ld::ElementPointerList::const_iterator it         = children.constBegin();
    Ld::ElementPointerList::const_iterator end        = children.constEnd();

    while (noChildren && it != end) {
        Ld::ElementPointer child = *it;
        noChildren = child.isNull() || child == childToIgnore;
        ++it;
    }

    return noChildren;
}
