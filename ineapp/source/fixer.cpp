/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Fixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_position.h>
#include <ld_element_with_grouped_children.h>
#include <ld_paragraph_element.h>
#include <ld_text_element.h>
#include <ld_paragraph_format.h>
#include <ld_character_format.h>
#include <ld_root_element.h>
#include <ld_table_frame_element.h>
#include <ld_cursor_state_collection.h>

#include "fixer.h"

QHash<QString, const Fixer*> Fixer::providers;

Fixer::Fixer() {}


Fixer::~Fixer() {}


bool Fixer::registerFixer(const QString& elementName, const Fixer& provider) {
    return registerFixer(elementName, &provider);
}


bool Fixer::registerFixer(const QString& elementName, const Fixer* provider) {
    bool result;

    if (!providers.contains(elementName)) {
        providers.insert(elementName, provider);
        result = true;
    } else {
        result = false;
    }

    return result;
}


const Fixer* Fixer::fixer(const QString& elementName) {
    return providers.value(elementName, Q_NULLPTR);
}


bool Fixer::isDeleteAllowed(Ld::ElementPointer /* element */, Fixer::DeleteReason reason) const {
    return (
           reason == DeleteReason::ELEMENT_SELECTED_FOR_DELETE
        || reason == DeleteReason::LAST_TEXT_DELETED
        || reason == DeleteReason::SELECTION_DELETE
    );
}


bool Fixer::isDeleteChildAllowed(
        Ld::ElementPointer  /* parent */,
        Ld::ElementPointer  /* element */,
        Fixer::DeleteReason /* reason */
    ) const {
    return true;
}


bool Fixer::isMergeAllowed(
        Ld::ElementPointer /* leftElement */,
        Ld::ElementPointer /* rightElement */,
        bool               /* checkingLeft */,
        bool               /* checkFormats */
    ) const {
    return false;
}


bool Fixer::isMergeChildrenAllowed(Ld::ElementPointer /* firstChild */, Ld::ElementPointer /* secondChild */) const {
    return true;
}


bool Fixer::isSplitAllowed(
        Ld::ElementPointer       /* element */,
        Fixer::SplitReason       /* reason */,
        const Ld::ElementCursor& /* cursor1 */
    ) const {
    return false;
}


Ld::ElementCursor Fixer::mergeElements(
        Ld::ElementPointer         /* leftElement */,
        Ld::ElementPointer         /* rightElement */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {
    return Ld::ElementCursor();
}


void Fixer::splitElement(
        Fixer::SplitReason         /* reason */,
        const Ld::ElementCursor&   /* cursor */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {
    Q_ASSERT(false);
}


void Fixer::preInsert(Ld::ElementPointer /* element */) const {}


void Fixer::deleteChild(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    parent->removeChild(child, cursorStateCollection);
}


void Fixer::deleteChildInSelection(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         child,
        const Ld::ElementCursor&   /* startingCursor */,
        const Ld::ElementCursor&   /* endingCursor */,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    deleteChild(parent, child, cursorStateCollection);
}


Ld::ElementPointer Fixer::preInsertRow(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* rowIndex */
    ) const {
    return Ld::ElementPointer();
}


void Fixer::postInsertRow(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* rowIndex */
    ) const {}


Ld::ElementPointer Fixer::preInsertColumn(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* columnIndex */
    ) const {
    return Ld::ElementPointer();
}


void Fixer::postInsertColumn(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* columnIndex */
    ) const {}


void Fixer::preRemoveRow(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* rowIndex */
    ) const {}


void Fixer::preRemoveColumn(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* columnIndex */
    ) const {}


void Fixer::postFormatChange(
        Ld::ElementPointer         /* element */,
        Ld::FormatPointer          /* oldFormat */,
        Ld::FormatPointer          /* newFormat */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {}


void Fixer::processSplitChildren(
        Ld::ElementPointer         /* parent */,
        Ld::ElementPointer         /* leftChild */,
        Ld::ElementPointer         /* rightChild */,
        Fixer::SplitReason         /* reason */,
        const Ld::ElementCursor&   /* cursor */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {
    Q_ASSERT(false);
}


void Fixer::processCopiedClone(Ld::ElementPointer /* element */) const {}


void Fixer::postPaste(
        Ld::ElementPointer         /* element */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {}


void Fixer::postPasteChild(
        Ld::ElementPointer         /* parent */,
        Ld::ElementPointer         /* child */,
        unsigned long              /* childIndex */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {}


bool Fixer::isSplitAllowedByParent(
        Ld::ElementPointer       element,
        SplitReason              reason,
        const Ld::ElementCursor& cursor
    ) {
    bool result;

    if (!element.isNull()) {
        Ld::ElementPointer parent = element->parent();
        if (!parent.isNull()) {
            const Fixer* parentFixer = fixer(parent->typeName());
            if (parentFixer != Q_NULLPTR) {
                result = parentFixer->isSplitAllowed(parent, reason, cursor);
            } else {
                result = false;
            }
        } else {
            result = true;
        }
    } else {
        result = false;
    }

    return result;
}


Ld::ElementPointer Fixer::createDefaultParagraph() {
    QSharedPointer<Ld::ParagraphElement> paragraphElement = Ld::Element::createElement<Ld::ParagraphElement>();
    QSharedPointer<Ld::ParagraphFormat>
        paragraphFormat  = Ld::Format::create(Ld::ParagraphFormat::formatName)
                           .dynamicCast<Ld::ParagraphFormat>();
    paragraphElement->setFormat(paragraphFormat);

    QSharedPointer<Ld::TextElement> textElement = Ld::Element::createElement<Ld::TextElement>();
    textElement->setFormat(Ld::Format::create(Ld::CharacterFormat::formatName));

    paragraphElement->append(textElement, Q_NULLPTR);

    return paragraphElement;
}


bool Fixer::isDefaultParagraph(Ld::ElementPointer element) {
    bool result = false;

    if (!element.isNull() && element->typeName() == Ld::ParagraphElement::elementName) {
        if (element->numberChildren() == 1) {
            QSharedPointer<Ld::ParagraphElement> paragraphElement = element.dynamicCast<Ld::ParagraphElement>();
            Ld::FormatPointer                    paragraphFormat  = paragraphElement->format();
            if (paragraphFormat->typeName() == Ld::ParagraphFormat::formatName) {
                if (Ld::ParagraphFormat().toString() == paragraphFormat->toString()) {
                    Ld::ElementPointer grandChild = paragraphElement->child(0);
                    if (grandChild->typeName() == Ld::TextElement::elementName) {
                        QSharedPointer<Ld::TextElement> textGrandChild = grandChild.dynamicCast<Ld::TextElement>();
                        if (textGrandChild->text().isEmpty()) {
                            Ld::FormatPointer textFormat = textGrandChild->format();
                            result = (Ld::CharacterFormat().toString() == textFormat->toString());
                        }
                    }
                }
            }
        }
    }

    return result;
}

