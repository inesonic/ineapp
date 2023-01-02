/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ParagraphElementFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_position.h>
#include <ld_root_element.h>
#include <ld_text_element.h>
#include <ld_character_format.h>
#include <ld_paragraph_format.h>
#include <ld_paragraph_element.h>
#include <ld_placeholder_element.h>

#include "fixer.h"
#include "paragraph_element_fixer.h"

const ParagraphElementFixer ParagraphElementFixer::instance;

ParagraphElementFixer::ParagraphElementFixer() {}


ParagraphElementFixer::~ParagraphElementFixer() {}


bool ParagraphElementFixer::isDeleteAllowed(Ld::ElementPointer element, DeleteReason /* reason */) const {
    Ld::ElementPointer parent = element->parent();
    return (parent.isNull() || parent->typeName() != Ld::RootElement::elementName || parent->numberChildren() > 1);
}


bool ParagraphElementFixer::isDeleteChildAllowed(
        Ld::ElementPointer parent,
        Ld::ElementPointer element,
        DeleteReason       reason
    ) const {
    bool result;

    if (reason == DeleteReason::LAST_TEXT_DELETED || reason == DeleteReason::DELETING_CHILD) {
        if (element->typeName() == Ld::TextElement::elementName) {
            if (parent->numberChildren() > 1) {
                unsigned long childIndex = parent->indexOfChild(element);

                if (childIndex == 0) {
                    Ld::ElementPointer rightSibling = parent->child(1);
                    result = (rightSibling->typeName() == Ld::TextElement::elementName);
                } else if (childIndex == (parent->numberChildren() - 1)) {
                    Ld::ElementPointer leftSibling = parent->child(childIndex - 1);
                    result = (leftSibling->typeName() == Ld::TextElement::elementName);
                } else {
                    Ld::ElementPointer leftSibling  = parent->child(childIndex - 1);
                    Ld::ElementPointer rightSibling = parent->child(childIndex + 1);

                    result = (
                           leftSibling->typeName() == Ld::TextElement::elementName
                        || rightSibling->typeName() == Ld::TextElement::elementName
                    );
                }
            } else {
                result = false;
            }
        } else {
            result = true;
        }
    } else {
        result = true;
    }

    return result;
}


bool ParagraphElementFixer::isMergeAllowed(
        Ld::ElementPointer leftElement,
        Ld::ElementPointer rightElement,
        bool               checkingLeft,
        bool               checkFormats
    ) const {
    bool result;

    if (checkingLeft) {
        Q_ASSERT(leftElement->typeName() == Ld::ParagraphElement::elementName);

        if (rightElement->typeName() == Ld::ParagraphElement::elementName) {
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


bool ParagraphElementFixer::isSplitAllowed(
        Ld::ElementPointer                 element,
        ParagraphElementFixer::SplitReason reason,
        const Ld::ElementCursor&           cursor
    ) const {
    bool result;

    if (cursor.element() != element) {
        // We can't split paragraphs but we can split the children of paragraphs which in turn can cause paragraphs
        // to be split.  Allow splits so long as the cursor isn't pointing directly at us.

        if (reason == SplitReason::INSERT_ELEMENT_TOP_LEVEL) {
            result = isSplitAllowedByParent(element, reason, cursor);
        } else {
            result = true;
        }
    } else {
        result = false;
    }

    return result;
}


Ld::ElementCursor ParagraphElementFixer::mergeElements(
        Ld::ElementPointer         leftElement,
        Ld::ElementPointer         rightElement,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    Ld::ElementCursor result;

    if (!leftElement.isNull() && !rightElement.isNull()) {
        unsigned long                          leftNumberChildren = leftElement->numberChildren();
        Ld::ElementPointerList                 newChildren        = rightElement->children();
        Ld::ElementPointerList::const_iterator it                 = newChildren.constBegin();
        Ld::ElementPointerList::const_iterator end                = newChildren.constEnd();

        if (it != end && leftNumberChildren > 0) {
            Ld::ElementPointer lastChildLeftElement   = leftElement->child(leftNumberChildren - 1);
            Ld::ElementPointer firstChildRightElement = *it;

            const Fixer* leftFixer   = fixer(lastChildLeftElement->typeName());
            const Fixer* rightFixer = fixer(firstChildRightElement->typeName());

            if (leftFixer != Q_NULLPTR                                                                &&
                rightFixer != Q_NULLPTR                                                               &&
                leftFixer->isMergeAllowed(lastChildLeftElement, firstChildRightElement, true, true)   &&
                rightFixer->isMergeAllowed(lastChildLeftElement, firstChildRightElement, false, true) &&
                isMergeChildrenAllowed(lastChildLeftElement, firstChildRightElement)                     ) {
                result = leftFixer->mergeElements(lastChildLeftElement, firstChildRightElement, cursorStateCollection);
                Q_ASSERT(result.isValid());

                ++it;
            }
        }

        if (it != end) {
            QSharedPointer<Ld::ParagraphElement> leftParagraph = leftElement.dynamicCast<Ld::ParagraphElement>();
            do {
                Ld::ElementPointer child = *it;
                leftParagraph->append(child, nullptr); // FIXME:
                ++it;
            } while (it != end);
        }

        // Note that Ld::ElementWithPositionChildren classes will never have null children so our cursor state
        // collection will point to the correct children independent of which element is their parent.  We
        // therefore do not need to update the cursors when we relocate the children.

        if (result.isInvalid()) {
            // Should never end up with an empty paragraph.
            Q_ASSERT(leftElement->numberChildren() > 0);

            if (leftNumberChildren > 0) {
                result.setElement(leftElement->child(leftNumberChildren - 1));
                result.moveToLastPositionInElement();
            } else {
                result.setElement(leftElement->child(0));
                result.moveToFirstPositionInElement();
            }
        }
    }

    return result;
}


void ParagraphElementFixer::preInsert(Ld::ElementPointer /* element */) const {}


void ParagraphElementFixer::processSplitChildren(
        Ld::ElementPointer                 parent,
        Ld::ElementPointer                 leftChild,
        Ld::ElementPointer                 rightChild,
        ParagraphElementFixer::SplitReason reason,
        const Ld::ElementCursor&           cursor,
        Ld::CursorStateCollection*         cursorStateCollection
    ) const {
    if (reason != SplitReason::NEWLINE && reason != SplitReason::INSERT_ELEMENT_TOP_LEVEL) {
        // This represents the cases where we want to split the insert a new child into this paragraph.  For this
        // scenario, we modify this paragraph by inserting the right child after the left child.

        QSharedPointer<Ld::ParagraphElement> paragraph        = parent.dynamicCast<Ld::ParagraphElement>();
        unsigned long                        indexOfLeftChild = parent->indexOfChild(leftChild);

        paragraph->insertAfter(indexOfLeftChild, rightChild, cursorStateCollection);
    } else {
        // This represents the cases where we need to split this paragraph in two, adding the right child to the
        // newly created paragraph.  We then need to ask the paragraph's parent to process the split children.

        QSharedPointer<Ld::ParagraphElement> leftParagraph    = parent.dynamicCast<Ld::ParagraphElement>();
        unsigned long                        indexOfLeftChild = parent->indexOfChild(leftChild);
        QSharedPointer<Ld::ParagraphElement> rightParagraph   = leftParagraph->create(leftParagraph->typeName())
                                                                .dynamicCast<Ld::ParagraphElement>();

        QSharedPointer<Ld::ParagraphFormat> leftFormat = leftParagraph->format();
        if (!leftFormat.isNull()) {
            QSharedPointer<Ld::ParagraphFormat> rightFormat = leftFormat->clone()
                                                              .dynamicCast<Ld::ParagraphFormat>();
            rightParagraph->setFormat(rightFormat);
        }

        rightParagraph->append(rightChild, cursorStateCollection);

        unsigned long numberChildren = leftParagraph->numberChildren();
        unsigned long firstToRemove  = indexOfLeftChild + 1;
        unsigned long numberToRemove = numberChildren - firstToRemove;
        for (unsigned long i=0 ; i<numberToRemove ; ++i) {
            Ld::ElementPointer child = leftParagraph->child(firstToRemove);
            rightParagraph->append(child, cursorStateCollection);
        }

        Ld::ElementPointer grandparent = leftParagraph->parent();
        Q_ASSERT(!grandparent.isNull());

        const Fixer* grandparentFixer = fixer(grandparent->typeName());
        Q_ASSERT(grandparentFixer != Q_NULLPTR);

        grandparentFixer->processSplitChildren(
            grandparent,
            leftParagraph,
            rightParagraph,
            reason,
            cursor,
            cursorStateCollection
        );
    }
}


Ld::FormatPointer ParagraphElementFixer::selectTextFormat(
        Ld::ElementPointer paragraphElement,
        bool               scanBackwardsFirst,
        Ld::ElementPointer removedElement
     ) {
    Ld::FormatPointer result;
    if (!removedElement.isNull() && removedElement->typeName() == Ld::TextElement::elementName) {
        result = removedElement->format();
    } else {
        bool                movingBakwards  = scanBackwardsFirst;
        unsigned            remainingPasses = 2;
        Ld::ElementPointer  candidateElement;
        bool atEnd;
        do {
            Ld::ElementPosition cursor(paragraphElement);

            do {
                if (movingBakwards) {
                    atEnd = cursor.moveBackwardByElement();
                } else {
                    atEnd = cursor.moveForwardByElement();
                }

                candidateElement = cursor.element();
            } while (!atEnd                                                             &&
                     (candidateElement.isNull()                                    ||
                      candidateElement->typeName() != Ld::TextElement::elementName    )    );

            --remainingPasses;
            movingBakwards = !movingBakwards;
        } while (!atEnd                                                             &&
                 remainingPasses > 0                                                &&
                 (candidateElement.isNull()                                    ||
                  candidateElement->typeName() != Ld::TextElement::elementName    )    );

        if (!candidateElement.isNull() && candidateElement->typeName() == Ld::TextElement::elementName) {
            result = candidateElement->format();
        }

        if (result.isNull()) {
            result = Ld::CharacterFormat::applicationDefaultTextFont();
        }
    }

    return result;
}


void ParagraphElementFixer::fixupAfterContentsChanged(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    QSharedPointer<Ld::ParagraphElement> paragraph = parent.dynamicCast<Ld::ParagraphElement>();

    if (paragraph->numberChildren() == 0) {
        Ld::FormatPointer templateFormat = selectTextFormat(parent, true, child);
        Ld::FormatPointer format         =   templateFormat.isNull()
                                           ? Ld::CharacterFormat::applicationDefaultTextFont()->clone()
                                           : templateFormat->clone();

        Ld::ElementPointer textElement = Ld::Element::create(Ld::TextElement::elementName);
        textElement->setFormat(format);

        paragraph->append(textElement, cursorStateCollection);
    } else {
        if (paragraph->child(0)->typeName() != Ld::TextElement::elementName) {
            Ld::FormatPointer templateFormat = selectTextFormat(parent, true, child);
            Ld::FormatPointer format         =   templateFormat.isNull()
                                               ? Ld::CharacterFormat::applicationDefaultTextFont()->clone()
                                               : templateFormat->clone();

            Ld::ElementPointer textElement = Ld::Element::create(Ld::TextElement::elementName);
            textElement->setFormat(format);

            paragraph->prepend(textElement, cursorStateCollection);
        }

        if (paragraph->child(paragraph->numberChildren() - 1)->typeName() != Ld::TextElement::elementName) {
            Ld::FormatPointer templateFormat = selectTextFormat(parent, false, child);
            Ld::FormatPointer format         =   templateFormat.isNull()
                                               ? Ld::CharacterFormat::applicationDefaultTextFont()->clone()
                                               : templateFormat->clone();

            Ld::ElementPointer textElement = Ld::Element::create(Ld::TextElement::elementName);
            textElement->setFormat(format);

            paragraph->append(textElement, cursorStateCollection);
        }
    }
}
