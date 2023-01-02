/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref RootElementFixer class.
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
#include <ld_paragraph_element.h>
#include <ld_placeholder_element.h>

#include "fixer.h"
#include "root_element_fixer.h"

const RootElementFixer RootElementFixer::instance;

RootElementFixer::RootElementFixer() {}


RootElementFixer::~RootElementFixer() {}


bool RootElementFixer::isDeleteChildAllowed(
        Ld::ElementPointer parent,
        Ld::ElementPointer /* element */,
        DeleteReason       /* reason */
    ) const {
    // Root elements must always contain at least one child.
    return parent->numberChildren() > 1;
}


bool RootElementFixer::isSplitAllowed(
        Ld::ElementPointer            /* element */,
        RootElementFixer::SplitReason /* reason */,
        const Ld::ElementCursor&      /* cursor */
    ) const {
    // NOTE: Cursors can never point to the root (by design) so we don't bother to check if the cursor points to the
    //       root element.

    return true;
}


void RootElementFixer::processSplitChildren(
        Ld::ElementPointer            parent,
        Ld::ElementPointer            leftChild,
        Ld::ElementPointer            rightChild,
        RootElementFixer::SplitReason /* reason */,
        const Ld::ElementCursor&      /* cursor */,
        Ld::CursorStateCollection*    cursorStateCollection
    ) const {
    // There can be no element above the root so we always insert the right child.

    QSharedPointer<Ld::RootElement> root             = parent.dynamicCast<Ld::RootElement>();
    unsigned long                   indexOfLeftChild = parent->indexOfChild(leftChild);

    root->insertAfter(indexOfLeftChild, rightChild, cursorStateCollection);
}
