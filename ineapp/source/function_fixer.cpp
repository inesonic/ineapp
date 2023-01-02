/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FunctionFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_with_positional_children.h>
#include <ld_function_placeholder_element.h>
#include <ld_function_format.h>
#include <ld_function_element.h>

#include "operator_fixer.h"
#include "function_fixer.h"

const FunctionFixer FunctionFixer::instance;

FunctionFixer::FunctionFixer() {}


FunctionFixer::~FunctionFixer() {}


bool FunctionFixer::isDeleteAllowed(Ld::ElementPointer /* element */, DeleteReason reason) const {
    return reason == DeleteReason::ELEMENT_SELECTED_FOR_DELETE || reason == DeleteReason::SELECTION_DELETE;
}


void FunctionFixer::postPaste(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    insertFunctionPlaceholders(element, cursorStateCollection);
}


void FunctionFixer::postPasteChild(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         /* child */,
        unsigned long              /* childIndex */,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    insertFunctionPlaceholders(parent, cursorStateCollection);
}


void FunctionFixer::preInsert(Ld::ElementPointer element) const {
    insertFunctionPlaceholders(element, Q_NULLPTR);
}


void FunctionFixer::deleteChild(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    QSharedPointer<Ld::FunctionElement> functionParent = parent.dynamicCast<Ld::FunctionElement>();
    unsigned long                       numberChildren = parent->numberChildren();
    unsigned long                       childIndex     = parent->indexOfChild(child);

    if (numberChildren > 1) {
        // If we're deleting the last element and that element is a placeholder, do nothing.  IF we're deleting
        // the last child and the child is not a placeholder, something got horked up, add a placeholder.

        unsigned long lastChildIndex = numberChildren - 1;
        if (!child->isPlaceholder() || childIndex != lastChildIndex) {
            if (childIndex == lastChildIndex /* && !child->isPlaceholder() */) {
                Ld::ElementPointer placeholder = Ld::Element::create(Ld::FunctionPlaceholderElement::elementName);
                functionParent->append(placeholder, cursorStateCollection);
            }

            functionParent->removeChild(childIndex, cursorStateCollection);
        }
    } else {
        Q_ASSERT(numberChildren == 1 || childIndex == 0);

        // Do nothing if we're deleting the placeholder and the placeholder is the only element.  Replace the
        // element with a placeholder if we're deleting anything but the placeholder.  Note that this should
        // normally never occur but we'll handle the case anyway, just in case.

        if (!child->isPlaceholder()) {
            Ld::ElementPointer placeholder = Ld::Element::create(Ld::FunctionPlaceholderElement::elementName);
            functionParent->append(placeholder, cursorStateCollection);

            functionParent->removeChild(childIndex, cursorStateCollection);
        }
    }
}


void FunctionFixer::postFormatChange(
        Ld::ElementPointer         element,
        Ld::FormatPointer          /* oldFormat */,
        Ld::FormatPointer          /* newFormat */,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    insertFunctionPlaceholders(element, cursorStateCollection);
}


void FunctionFixer::removeFunctionPlaceholders(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    Q_ASSERT(!element.isNull() && element->typeName() == Ld::FunctionElement::elementName);
    QSharedPointer<Ld::FunctionElement> functionElement = element.dynamicCast<Ld::FunctionElement>();

    unsigned long numberChildren = functionElement->numberChildren();
    unsigned long childIndex     = 0;

    while (childIndex < numberChildren) {
        Ld::ElementPointer child = functionElement->child(childIndex);
        if (child->typeName() == Ld::FunctionPlaceholderElement::elementName) {
            functionElement->removeChild(childIndex, cursorStateCollection);
            --numberChildren;
        } else {
            ++childIndex;
        }
    }
}


void FunctionFixer::insertFunctionPlaceholders(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    Q_ASSERT(!element.isNull() && element->typeName() == Ld::FunctionElement::elementName);
    QSharedPointer<Ld::FunctionElement> functionElement = element.dynamicCast<Ld::FunctionElement>();

    unsigned      minimumNumberChildren = minimumNumberAllowedChildren(element);
    unsigned long numberChildren        = functionElement->numberChildren();
    unsigned long numberRealChildren;
    bool          needsFinalPlaceholder;

    if (numberChildren > 0) {
        unsigned long      lastChildIndex = numberChildren - 1;
        Ld::ElementPointer lastChild      = functionElement->child(lastChildIndex);

        if (!lastChild.isNull() && lastChild->typeName() == Ld::FunctionPlaceholderElement::elementName) {
            QSharedPointer<Ld::FunctionPlaceholderElement>
                placeholderElement = lastChild.dynamicCast<Ld::FunctionPlaceholderElement>();

            if (placeholderElement->fieldString().isEmpty()) {
                numberRealChildren    = lastChildIndex;
                needsFinalPlaceholder = false;
            } else {
                numberRealChildren    = numberChildren;
                needsFinalPlaceholder = true;
            }
        } else {
            numberRealChildren    = numberChildren;
            needsFinalPlaceholder = true;
        }
    } else {
        numberRealChildren    = 0;
        needsFinalPlaceholder = true;
    }

    if (numberRealChildren < minimumNumberChildren) {
        unsigned numberToAdd = minimumNumberChildren - numberRealChildren;

        for (unsigned i=0 ; i<numberToAdd ; ++i) {
            QSharedPointer<Ld::FunctionPlaceholderElement>
                placeholderElement = Ld::Element::create(Ld::FunctionPlaceholderElement::elementName)
                                     .dynamicCast<Ld::FunctionPlaceholderElement>();

            placeholderElement->setFieldString(tr("?"));
            functionElement->insertBefore(numberRealChildren, placeholderElement, cursorStateCollection);
        }
    }

    if (needsFinalPlaceholder) {
        QSharedPointer<Ld::FunctionPlaceholderElement>
            placeholderElement = Ld::Element::create(Ld::FunctionPlaceholderElement::elementName)
                                 .dynamicCast<Ld::FunctionPlaceholderElement>();

        functionElement->append(placeholderElement, cursorStateCollection);
    }
}


unsigned FunctionFixer::minimumNumberAllowedChildren(Ld::ElementPointer element) {
    unsigned result;

    QSharedPointer<Ld::FunctionElement> functionElement = element.dynamicCast<Ld::FunctionElement>();
    Ld::FormatPointer                   format          = functionElement->format();

    if (!format.isNull() && format->capabilities().contains(Ld::FunctionFormat::formatName)) {
        QSharedPointer<Ld::FunctionFormat> functionFormat = format.dynamicCast<Ld::FunctionFormat>();
        result = functionFormat->subscriptedParameter() ? 2 : 1;
    } else {
        result = 1;
    }

    return result;
}
