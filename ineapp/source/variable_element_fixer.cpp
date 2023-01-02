/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref VariableElementFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format.h>
#include <ld_variable_element.h>

#include "fixer.h"
#include "variable_element_fixer.h"

const VariableElementFixer VariableElementFixer::instance;

VariableElementFixer::VariableElementFixer() {}


VariableElementFixer::~VariableElementFixer() {}


bool VariableElementFixer::isDeleteAllowed(
        Ld::ElementPointer                 /* element */,
        VariableElementFixer::DeleteReason reason
    ) const {
    return reason == DeleteReason::ELEMENT_SELECTED_FOR_DELETE || reason == DeleteReason::SELECTION_DELETE;
}


bool VariableElementFixer::isMergeAllowed(
        Ld::ElementPointer /* leftElement */,
        Ld::ElementPointer /* rightElement */,
        bool               /* checkingLeft */,
        bool               /* checkFormats */
    ) const {
    // TODO: Longer term we may want to look at the right element and allow the merge if the result would create a
    //       valid variable name.
    //
    //       For now, we prevent merging to protect against allowing invalid variable names.

    return false;
}
