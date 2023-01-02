/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ListFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_element_with_positional_children.h>
#include <ld_list_placeholder_element.h>

#include "operator_fixer.h"
#include "list_fixer.h"

const ListFixer ListFixer::instance;

ListFixer::ListFixer() {}


ListFixer::~ListFixer() {}


void ListFixer::postPaste(
        Ld::ElementPointer         /* element */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {}


void ListFixer::postPasteChild(
        Ld::ElementPointer         /* parent */,
        Ld::ElementPointer         /* child */,
        unsigned long              /* childIndex */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {
//    QSharedPointer<Ld::TableFrameElement> tableParent             = parent.dynamicCast<Ld::TableFrameElement>();
//    unsigned                              groupIndex              = tableParent->groupContainingChild(childIndex);
//    unsigned long                         numberChildrenThisGroup = tableParent->numberChildrenInGroup(groupIndex);

//    if (numberChildrenThisGroup == 2) {
//        Ld::ElementPointer secondChild = tableParent->child(childIndex + 1);
//        if (isDefaultParagraph(secondChild)) {
//            tableParent->removeChild(childIndex + 1, cursorStateCollection);
//            // Because the newly inserted element is supposed to replace the default element, we don't adjust
//            // childIndex;
//        }
//    }
}


void ListFixer::preInsert(Ld::ElementPointer element) const {
    insertListPlaceholder(element, Q_NULLPTR);
}


void ListFixer::deleteChild(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    Ld::Element::ChildPlacement childPlacement = parent->childPlacement();
    if (childPlacement == Ld::Element::ChildPlacement::FIXED ||
        childPlacement == Ld::Element::ChildPlacement::GRID     ) {
        QSharedPointer<Ld::ElementWithFixedChildren>
            fixedParent = parent.dynamicCast<Ld::ElementWithFixedChildren>();

        Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::ListPlaceholderElement::elementName);
        unsigned long      childIndex         = parent->indexOfChild(child);

        fixedParent->setChild(childIndex, placeholderElement, cursorStateCollection);
    } else {
        QSharedPointer<Ld::ElementWithPositionalChildren>
            positionalParent = parent.dynamicCast<Ld::ElementWithPositionalChildren>();

        unsigned long numberChildren = parent->numberChildren();
        unsigned long childIndex     = parent->indexOfChild(child);
        if (numberChildren > 1) {
            // If we're deleting the last element and that element is a placeholder, do nothing.  IF we're deleting
            // the last child and the child is not a placeholder, something got horked up, add a placeholder.

            unsigned long lastChildIndex = numberChildren - 1;
            if (!child->isPlaceholder() || childIndex != lastChildIndex) {
                if (childIndex == lastChildIndex /* && !child->isPlaceholder() */) {
                    Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::ListPlaceholderElement::elementName);
                    positionalParent->append(placeholderElement, cursorStateCollection);
                }

                positionalParent->removeChild(childIndex, cursorStateCollection);
            }
        } else {
            Q_ASSERT(numberChildren == 1 || childIndex == 0);

            // Do nothing if we're deleting the placeholder and the placeholder is the only element.  Replace the
            // element with a placeholder if we're deleting anything but the placeholder.  Note that this should
            // normally never occur but we'll handle the case anyway, just in case.

            if (!child->isPlaceholder()) {
                Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::ListPlaceholderElement::elementName);
                positionalParent->append(placeholderElement, cursorStateCollection);

                positionalParent->removeChild(childIndex, cursorStateCollection);
            }
        }
    }
}


void ListFixer::removeListPlaceholder(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    if (!element.isNull() && element->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL) {
        QSharedPointer<Ld::ElementWithPositionalChildren>
            positionalElement = element.dynamicCast<Ld::ElementWithPositionalChildren>();

        unsigned long numberChildren = positionalElement->numberChildren();
        unsigned long childIndex     = 0;

        while (childIndex < numberChildren) {
            Ld::ElementPointer child = positionalElement->child(childIndex);
            if (child->typeName() == Ld::ListPlaceholderElement::elementName) {
                positionalElement->removeChild(childIndex, cursorStateCollection);
                --numberChildren;
            } else {
                ++childIndex;
            }
        }
    }
}


void ListFixer::insertListPlaceholder(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    if (!element.isNull() && element->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL) {
        QSharedPointer<Ld::ElementWithPositionalChildren>
            positionalElement = element.dynamicCast<Ld::ElementWithPositionalChildren>();

        unsigned long numberChildren = positionalElement->numberChildren();
        if (numberChildren > 0) {
            unsigned long lastChildIndex = numberChildren - 1;
            if (positionalElement->child(lastChildIndex)->typeName() != Ld::ListPlaceholderElement::elementName) {
                Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::ListPlaceholderElement::elementName);
                positionalElement->append(placeholderElement, cursorStateCollection);
            }
        } else {
            Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::ListPlaceholderElement::elementName);
            positionalElement->append(placeholderElement, cursorStateCollection);
        }
    }
}
