/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref OperatorFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_with_fixed_children.h>
#include <ld_element_with_positional_children.h>
#include <ld_placeholder_element.h>

#include "fixer.h"
#include "operator_fixer.h"

const OperatorFixer OperatorFixer::instance;

OperatorFixer::OperatorFixer() {}


OperatorFixer::~OperatorFixer() {}


bool OperatorFixer::isMergeAllowed(
        Ld::ElementPointer /* leftElement */,
        Ld::ElementPointer /* rightElement */,
        bool               /* checkingLeft */,
        bool               /* checkFormats */
    ) const {
    return false;
}


void OperatorFixer::postPaste(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    insertDefaultChildren(element, cursorStateCollection);
}


void OperatorFixer::postPasteChild(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         /* child */,
        unsigned long              /* childIndex */,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    insertDefaultChildren(parent, cursorStateCollection);
}


void OperatorFixer::preInsert(Ld::ElementPointer element) const {
    insertDefaultChildren(element, Q_NULLPTR);
}


void OperatorFixer::deleteChild(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    Ld::Element::ChildPlacement childPlacement = parent->childPlacement();
    if (childPlacement == Ld::Element::ChildPlacement::FIXED ||
        childPlacement == Ld::Element::ChildPlacement::GRID     ) {
        QSharedPointer<Ld::ElementWithFixedChildren>
            fixedParent = parent.dynamicCast<Ld::ElementWithFixedChildren>();

        Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::PlaceholderElement::elementName);
        unsigned long      childIndex         = parent->indexOfChild(child);

        fixedParent->setChild(childIndex, placeholderElement, cursorStateCollection);
    } else if (childPlacement == Ld::Element::ChildPlacement::POSITIONAL) {
        if (parent->numberChildren() == 1) {
            Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::PlaceholderElement::elementName);
            parent.dynamicCast<Ld::ElementWithPositionalChildren>()->append(placeholderElement, cursorStateCollection);
        }

        parent->removeChild(child, cursorStateCollection);
    } else {
        parent->removeChild(child, cursorStateCollection);
    }
}


void OperatorFixer::removeDefaultChildren(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    Ld::Element::ChildPlacement childPlacement = element->childPlacement();

    switch (childPlacement) {
        case Ld::Element::ChildPlacement::FIXED:
        case Ld::Element::ChildPlacement::GRID: {
            QSharedPointer<Ld::ElementWithFixedChildren>
                fixedElement = element.dynamicCast<Ld::ElementWithFixedChildren>();
            unsigned long numberChildren = fixedElement->numberChildren();

            for (unsigned long childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
                Ld::ElementPointer child = fixedElement->child(childIndex);
                if (!child.isNull() && child->typeName() == Ld::PlaceholderElement::elementName) {
                    fixedElement->removeChild(childIndex, cursorStateCollection);
                }
            }

            break;
        }

        case Ld::Element::ChildPlacement::POSITIONAL: {
            QSharedPointer<Ld::ElementWithPositionalChildren>
                positionalElement = element.dynamicCast<Ld::ElementWithPositionalChildren>();
            unsigned long numberChildren = positionalElement->numberChildren();
            unsigned long childIndex     = 0;

            while (childIndex < numberChildren) {
                Ld::ElementPointer child = positionalElement->child(childIndex);
                if (child->typeName() == Ld::PlaceholderElement::elementName) {
                    positionalElement->removeChild(childIndex, cursorStateCollection);
                    --numberChildren;
                } else {
                    ++childIndex;
                }
            }

            break;
        }

        case Ld::Element::ChildPlacement::FLOATING: {
            Q_ASSERT(false);
            break;
        }

        case Ld::Element::ChildPlacement::GROUPED: {
            Q_ASSERT(false);
            break;
        }

        case Ld::Element::ChildPlacement::NONE: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void OperatorFixer::insertDefaultChildren(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    Ld::Element::ChildPlacement childPlacement = element->childPlacement();

    switch (childPlacement) {
        case Ld::Element::ChildPlacement::FIXED:
        case Ld::Element::ChildPlacement::GRID: {
            QSharedPointer<Ld::ElementWithFixedChildren>
                fixedElement = element.dynamicCast<Ld::ElementWithFixedChildren>();
            unsigned long numberChildren = fixedElement->numberChildren();

            for (unsigned long childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
                Ld::ElementPointer child = fixedElement->child(childIndex);
                if (child.isNull()) {
                    Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::PlaceholderElement::elementName);
                    fixedElement->setChild(childIndex, placeholderElement, cursorStateCollection);
                }
            }

            break;
        }

        case Ld::Element::ChildPlacement::POSITIONAL: {
            QSharedPointer<Ld::ElementWithPositionalChildren>
                positionalElement = element.dynamicCast<Ld::ElementWithPositionalChildren>();

            if (positionalElement->numberChildren() == 0) {
                Ld::ElementPointer placeholderElement = Ld::Element::create(Ld::PlaceholderElement::elementName);
                positionalElement->append(placeholderElement, cursorStateCollection);
            }

            break;
        }

        case Ld::Element::ChildPlacement::FLOATING: {
            Q_ASSERT(false);
            break;
        }

        case Ld::Element::ChildPlacement::GROUPED: {
            Q_ASSERT(false);
            break;
        }

        case Ld::Element::ChildPlacement::NONE: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}
