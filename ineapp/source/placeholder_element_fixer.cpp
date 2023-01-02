/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlaceholderElementFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>

#include "fixer.h"
#include "placeholder_element_fixer.h"

const PlaceholderElementFixer PlaceholderElementFixer::instance;

PlaceholderElementFixer::PlaceholderElementFixer() {}


PlaceholderElementFixer::~PlaceholderElementFixer() {}


bool PlaceholderElementFixer::isDeleteAllowed(
        Ld::ElementPointer                    /* element */,
        PlaceholderElementFixer::DeleteReason /* reason */
    ) const {
    return false;
}
