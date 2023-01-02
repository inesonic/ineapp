/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LiteralElementFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format.h>
#include <ld_literal_element.h>

#include "fixer.h"
#include "literal_element_fixer.h"

const LiteralElementFixer LiteralElementFixer::instance;

LiteralElementFixer::LiteralElementFixer() {}


LiteralElementFixer::~LiteralElementFixer() {}


bool LiteralElementFixer::isDeleteAllowed(
        Ld::ElementPointer                 /* element */,
        LiteralElementFixer::DeleteReason reason
    ) const {
    return reason == DeleteReason::ELEMENT_SELECTED_FOR_DELETE || reason == DeleteReason::SELECTION_DELETE;
}


bool LiteralElementFixer::isMergeAllowed(
        Ld::ElementPointer /* leftElement */,
        Ld::ElementPointer /* rightElement */,
        bool               /* checkingLeft */,
        bool               /* checkFormats */
    ) const {
    // TODO: Longer term we may want to look at the right element and allow the merge if the result would create a
    //       valid iteral value.
    //
    //       For now, we prevent merging to protect against allowing invalid literals.

    return false;
}
