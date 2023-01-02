/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TextElementFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_cursor_state_collection.h>
#include <ld_text_element.h>

#include "fixer.h"
#include "text_element_fixer.h"

const TextElementFixer TextElementFixer::instance;

TextElementFixer::TextElementFixer() {}


TextElementFixer::~TextElementFixer() {}


bool TextElementFixer::isMergeAllowed(
        Ld::ElementPointer leftElement,
        Ld::ElementPointer rightElement,
        bool               checkingLeft,
        bool               checkFormats
    ) const {
    bool result;

    if (checkingLeft) {
        Q_ASSERT(leftElement->typeName() == Ld::TextElement::elementName);

        if (rightElement->numberTextRegions() > 0) {
            if (checkFormats) {
                QString leftFormatString  = leftElement->format()->toString();
                QString rightFormatString = rightElement->format()->toString();

                result = (leftFormatString == rightFormatString);
            } else {
                result = true;
            }
        } else {
            result = false;
        }
    } else {
        result = true;
    }

    return result;
}


bool TextElementFixer::isSplitAllowed(
        Ld::ElementPointer            element,
        TextElementFixer::SplitReason reason,
        const Ld::ElementCursor&      cursor
    ) const {
    Q_ASSERT(!element.isNull());
    Q_ASSERT(element->typeName() == Ld::TextElement::elementName);

    // Text elements have no children so we know the cursors must be pointing to the element and not a child of the
    // element.

    bool canSplit = false;

    switch (reason) {
        case SplitReason::NEWLINE:
        case SplitReason::INSERT_ELEMENT:
        case SplitReason::INSERT_ELEMENT_TOP_LEVEL:
        case SplitReason::CUT: {
            if (!cursor.isWholeElement()) {
                unsigned regionIndex = cursor.regionIndex();
                if (regionIndex < element->numberTextRegions()) {
                    canSplit = isSplitAllowedByParent(element, reason, cursor);
                }
            }

            break;
        }

        case SplitReason::FORMAT_CHANGE: {
            if (!cursor.isWholeElement()) {
                unsigned regionIndex = cursor.regionIndex();
                if (regionIndex < element->numberTextRegions()) {
                    unsigned long textIndex = cursor.textIndex();
                    if (textIndex > 0) {
                        QString       text       = element->text(regionIndex);
                        unsigned long textLength = static_cast<unsigned long>(text.length());

                        if (textIndex < textLength) {
                            canSplit = isSplitAllowedByParent(element, reason, cursor);
                        }
                    }
                }
            }

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return canSplit;
}


Ld::ElementCursor TextElementFixer::mergeElements(
        Ld::ElementPointer         leftElement,
        Ld::ElementPointer         rightElement,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    Ld::ElementCursor result;
    unsigned          numberRightRegions = rightElement->numberTextRegions();

    if (numberRightRegions > 0) {
        QString       text          = leftElement->text();
        unsigned long currentLength = static_cast<unsigned long>(text.length());

        for (unsigned regionIndex=0 ; regionIndex<numberRightRegions ; ++regionIndex) {
            text += rightElement->text(regionIndex);
        }

        leftElement->setText(text);

        if (cursorStateCollection != nullptr) {
            for (  Ld::CursorStateCollection::iterator it = cursorStateCollection->begin(),
                                                       end = cursorStateCollection->end()
                 ; it != end
                 ; ++it
                ) {
                if (it->isTextInElement() && it->element() == rightElement) {
                    unsigned      cursorRegionIndex = it->regionIndex();
                    unsigned long cursorTextIndex   = it->textIndex();
                    unsigned long newTextIndex      = currentLength;

                    for (unsigned otherRegionIndex=0 ; otherRegionIndex<cursorRegionIndex ; ++otherRegionIndex) {
                        newTextIndex += rightElement->text(otherRegionIndex).length();
                    }

                    newTextIndex += cursorTextIndex;
                    it->update(newTextIndex, leftElement);
                }
            }
        }

        result.setElement(leftElement);
        result.setPositionInElement(currentLength);
    }

    return result;
}


void TextElementFixer::splitElement(
        TextElementFixer::SplitReason reason,
        const Ld::ElementCursor&      cursor,
        Ld::CursorStateCollection*    cursorStateCollection
    ) const {
    Ld::ElementPointer leftElement  = cursor.element();
    Ld::ElementPointer rightElement = Ld::Element::create(leftElement->typeName());

    Ld::FormatPointer leftFormat = leftElement->format();
    if (!leftFormat.isNull()) {
        Ld::FormatPointer rightFormat = leftFormat->clone();
        rightElement->setFormat(rightFormat);
    }

    unsigned long textIndex   = cursor.textIndex();
    QString       currentText = leftElement->text(0);
    QString       leftText    = currentText.left(textIndex);
    QString       rightText   = currentText.mid(textIndex);

    leftElement->setText(leftText);
    rightElement->setText(rightText);

    if (cursorStateCollection != Q_NULLPTR) {
        for (  Ld::CursorStateCollection::iterator it  = cursorStateCollection->begin(),
                                                   end = cursorStateCollection->end()
             ; it != end
             ; ++it
            ) {
            if (it->isTextInElement() && it->element() == leftElement) {
                unsigned long cursorTextIndex = it->textIndex();
                if (cursorTextIndex > textIndex) {
                    it->update(cursorTextIndex - textIndex, rightElement);
                }
            }
        }
    }

    Ld::ElementPointer parent = leftElement->parent();
    Q_ASSERT(!parent.isNull());

    const Fixer* parentFixer = fixer(parent->typeName());
    Q_ASSERT(parentFixer != Q_NULLPTR);

    parentFixer->processSplitChildren(parent, leftElement, rightElement, reason, cursor, cursorStateCollection);
}
