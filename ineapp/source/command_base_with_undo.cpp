/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CommandBaseWithUndo class.
***********************************************************************************************************************/

#include <QPointer>
#include <QSharedPointer>
#include <QString>

#include <algorithm>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_element_cursor.h>
#include <ld_paragraph_element.h>
#include <ld_table_frame_element.h>
#include <ld_page_break_element.h>
#include <ld_element_with_positional_children.h>

#include "command_container.h"
#include "cursor.h"
#include "command_base.h"
#include "command_base_with_undo.h"

CommandBaseWithUndo::CommandBaseWithUndo() {
    countChangedParagraphs = 0;
}


CommandBaseWithUndo::CommandBaseWithUndo(CursorPointer newCursor):CommandBase(newCursor) {
    countChangedParagraphs = 0;
}


CommandBaseWithUndo::~CommandBaseWithUndo() {}


void CommandBaseWithUndo::saveParagraphs(
        Ld::ElementPointer element,
        long               startingParagraphOffset,
        long               endingParagraphOffset
    ) {
    Ld::ElementCursor  referenceCursor = findReferenceParagraph(element);
    Ld::ElementPointer parentElement   = referenceCursor.parent();
    unsigned long      referenceIndex  = referenceCursor.childIndex();
    unsigned long      lastChildIndex  = parentElement->numberChildren() - 1;

    Q_ASSERT(
           parentElement->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL
        || parentElement->childPlacement() == Ld::Element::ChildPlacement::GROUPED
    );

    unsigned long startingIndex = std::min(
        lastChildIndex,
        static_cast<unsigned long>(std::max(0L, static_cast<long>(referenceIndex + startingParagraphOffset)))
    );

    unsigned long endingIndex = std::min(
        lastChildIndex,
        static_cast<unsigned long>(std::max(0L, static_cast<long>(referenceIndex + endingParagraphOffset)))
    );

    saveParagraphsHelper(parentElement, startingIndex, endingIndex);
}


bool CommandBaseWithUndo::saveParagraphs(const Cursor& cursor) {
    bool success;

    unsigned long      startingIndex = static_cast<unsigned long>(-1);
    unsigned long      endingIndex   = static_cast<unsigned long>(-1);
    Ld::ElementPointer parentElement;

    Cursor::Type   cursorType = cursor.cursorType();
    if (cursorType == Cursor::Type::SELECTION) {
        Ld::ElementCursor cursor1 = findReferenceParagraph(cursor.elementCursor().element());
        Ld::ElementCursor cursor2 = findReferenceParagraph(cursor.selectionCursor().element());

        Ld::ElementPointer p1  = cursor1.parent();
        unsigned long      ci1 = cursor1.childIndex();

        Ld::ElementPointer p2  = cursor2.parent();
        unsigned long      ci2 = cursor2.childIndex();

        if (p1 == p2) {
            parentElement = p1;

            if (ci1 < ci2) {
                startingIndex = ci1;
                endingIndex   = ci2;
            } else {
                startingIndex = ci2;
                endingIndex   = ci1;
            }

            success = true;
        } else {
            success = false;
        }
    } else if (cursorType == Cursor::Type::TEXT           ||
               cursorType == Cursor::Type::ELEMENT        ||
               cursorType == Cursor::Type::EMPTY_POSITION    ) {
        Ld::ElementCursor elementCursor = findReferenceParagraph(cursor.elementCursor().element());

        parentElement = elementCursor.parent();
        startingIndex = elementCursor.childIndex();
        endingIndex   = startingIndex;

        success = true;
    } else {
        success = false;
    }

    if (success) {
        saveParagraphsHelper(parentElement, startingIndex, endingIndex);
    }

    return success;
}


const Ld::ElementPosition& CommandBaseWithUndo::firstSavedParagraph() const {
    return startingParagraphCursor;
}


unsigned long CommandBaseWithUndo::numberSavedParagraphs() const {
    return static_cast<unsigned long>(savedParagraphs.size());
}


void CommandBaseWithUndo::markChangedParagraphs(
        Ld::ElementPointer element,
        long               startingParagraphOffset,
        long               endingParagraphOffset
    ) {
    Ld::ElementCursor  referenceCursor = findReferenceParagraph(element);
    Ld::ElementPointer parentElement   = referenceCursor.parent();
    unsigned long      referenceIndex  = referenceCursor.childIndex();
    unsigned long      lastChildIndex  = parentElement->numberChildren() - 1;

    unsigned long startingIndex = std::min(
        lastChildIndex,
        static_cast<unsigned long>(std::max(0L, static_cast<long>(referenceIndex + startingParagraphOffset)))
    );

    unsigned long endingIndex = std::min(
        lastChildIndex,
        static_cast<unsigned long>(std::max(0L, static_cast<long>(referenceIndex + endingParagraphOffset)))
    );

    Q_ASSERT(endingIndex >= startingIndex);

    startingChangedParagraph = Ld::ElementCursor(parentElement, startingIndex);
    countChangedParagraphs   = endingIndex - startingIndex + 1;
}


void CommandBaseWithUndo::markChangedParagraphs(const Ld::ElementCursor& cursor1, const Ld::ElementCursor& cursor2) {
    bool                     cursor1Leads   = cursor2.isInvalid() || (cursor1 < cursor2);
    bool                     cursor1Trails  = cursor2.isValid()   || (cursor1 >= cursor2);
    const Ld::ElementCursor& leadingCursor  = cursor1Leads ? cursor1 : cursor2;
    const Ld::ElementCursor& trailingCursor = cursor1Trails ? cursor2 : cursor1;

    Ld::ElementPointer leadingElement  = leadingCursor.element();
    Ld::ElementPointer trailingElement = trailingCursor.element();

    if (leadingElement.isNull()) {
        leadingElement = leadingCursor.parent();
    }

    if (trailingElement.isNull()) {
        trailingElement = trailingCursor.parent();
    }

    Ld::ElementCursor leadingParagraphCursor  = findReferenceParagraph(leadingElement);
    Ld::ElementCursor trailingParagraphCursor = findReferenceParagraph(trailingElement);

    Q_ASSERT(leadingParagraphCursor.depth() == trailingParagraphCursor.depth());
    Ld::ElementPointer referenceParagraph    = leadingParagraphCursor.element();
    long               endingParagraphOffset = (
          trailingParagraphCursor.childIndex()
        - leadingParagraphCursor.childIndex()
    );

    markChangedParagraphs(referenceParagraph, 0, endingParagraphOffset);
}


void CommandBaseWithUndo::markNoChangedParagraphs(Ld::ElementPointer element) {
    startingChangedParagraph = Ld::ElementCursor(element);
    countChangedParagraphs = 0;
}


const Ld::ElementPosition& CommandBaseWithUndo::firstChangedParagraph() const {
    return startingChangedParagraph;
}


unsigned long CommandBaseWithUndo::numberChangedParagraphs() const {
    return countChangedParagraphs;
}


bool CommandBaseWithUndo::removeChangedParagraphs(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;

    if (startingChangedParagraph.isValid()) {
        if (countChangedParagraphs > 0) {
            Ld::ElementPointer parent     = startingChangedParagraph.parent();
            unsigned long      childIndex = startingChangedParagraph.childIndex();

            Q_ASSERT(
                   parent->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL
                || parent->childPlacement() == Ld::Element::ChildPlacement::GROUPED
            );

            QSharedPointer<Ld::ElementWithPositionalChildren> positionalParent =
                parent.dynamicCast<Ld::ElementWithPositionalChildren>();

            for (unsigned long i=0 ; i<countChangedParagraphs ; ++i) {
                positionalParent->removeChild(childIndex, cursorStateCollection);
            }
        }

        clearModifiedParagraphsInformation();
        success = true;
    } else {
        success = false;
    }

    return success;
}


bool CommandBaseWithUndo::restoreSavedParagraphs(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;

    Ld::ElementPointer parent = startingParagraphCursor.parent();
    if (!savedParagraphs.isEmpty() && !parent.isNull()) {
        Q_ASSERT(
               parent->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL
            || parent->childPlacement() == Ld::Element::ChildPlacement::GROUPED
        );

        unsigned long childIndex           = startingParagraphCursor.childIndex();
        unsigned long parentNumberChildren = parent->numberChildren();
        if (childIndex > parentNumberChildren) {
            childIndex = parentNumberChildren;
        }

        QSharedPointer<Ld::ElementWithPositionalChildren> positionalParent =
            parent.dynamicCast<Ld::ElementWithPositionalChildren>();

        unsigned long numberParagraphs = static_cast<unsigned long>(savedParagraphs.size());
        for (unsigned long i=0 ; i<numberParagraphs ; ++i) {
            positionalParent->insertBefore(childIndex + i, savedParagraphs[i], cursorStateCollection);
        }

        clearSavedParagraphsInformation();

        success = true;
    } else {
        success = false;
    }

    return success;
}



void CommandBaseWithUndo::clearSavedParagraphsInformation() {
    savedParagraphs.clear();
    startingParagraphCursor = Ld::ElementCursor();
}


void CommandBaseWithUndo::clearModifiedParagraphsInformation() {
    startingChangedParagraph = Ld::ElementCursor();
    countChangedParagraphs   = 0;
}


Ld::ElementCursor CommandBaseWithUndo::findReferenceParagraph(Ld::ElementPointer element) {
    Ld::ElementCursor result;

    while (!element.isNull()                                         &&
           element->typeName() != Ld::ParagraphElement::elementName  &&
           element->typeName() != Ld::TableFrameElement::elementName &&
           element->typeName() != Ld::PageBreakElement::elementName     ) {
        element = element->parent();
    }

    if (!element.isNull()) {
        result = Ld::ElementCursor(element);
    }

    return result;
}


void CommandBaseWithUndo::saveParagraphsHelper(
        Ld::ElementPointer parentElement,
        unsigned long      startingIndex,
        unsigned long      endingIndex
    ) {
    savedParagraphs.clear();
    savedParagraphs.reserve(endingIndex - startingIndex + 1);
    for (unsigned long index=startingIndex ; index<=endingIndex ; ++index) {
        savedParagraphs.append(parentElement->child(index)->clone(true));
    }

    startingParagraphCursor = Ld::ElementCursor(parentElement, startingIndex);
}
