/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref GridOperatorFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_element_with_grid_children.h>
#include <ld_placeholder_element.h>

#include "operator_fixer.h"
#include "grid_operator_fixer.h"

const GridOperatorFixer GridOperatorFixer::instance;

GridOperatorFixer::GridOperatorFixer() {}


GridOperatorFixer::~GridOperatorFixer() {}


Ld::ElementPointer GridOperatorFixer::preInsertRow(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* rowIndex */
    ) const {
    return Ld::ElementPointer(Ld::Element::create(Ld::PlaceholderElement::elementName));
}


Ld::ElementPointer GridOperatorFixer::preInsertColumn(
        QSharedPointer<Ld::ElementWithGridChildren> /* element */,
        unsigned long                               /* columnIndex */
    ) const {
    return Ld::ElementPointer(Ld::Element::create(Ld::PlaceholderElement::elementName));
}


void GridOperatorFixer::processCopiedClone(Ld::ElementPointer element) const {
    QSharedPointer<Ld::ElementWithGridChildren> gridElement = element.dynamicCast<Ld::ElementWithGridChildren>();
    unsigned long numberRows    = gridElement->numberRows();
    unsigned long numberColumns = gridElement->numberColumns();

    QSet<unsigned long> occupiedRows;
    QSet<unsigned long> occupiedColumns;

    unsigned long rowIndex;
    unsigned long columnIndex;

    for (rowIndex=0 ; rowIndex<numberRows ; ++rowIndex) {
        for (columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            if (!gridElement->child(rowIndex, columnIndex).isNull()) {
                occupiedRows.insert(rowIndex);
                occupiedColumns.insert(columnIndex);
            }
        }
    }

    rowIndex = numberRows;
    while (rowIndex > 0) {
        --rowIndex;
        if (!occupiedRows.contains(rowIndex)) {
            gridElement->removeRow(rowIndex, Q_NULLPTR);
        }
    }

    columnIndex = numberColumns;
    while (columnIndex > 0) {
        --columnIndex;
        if (!occupiedColumns.contains(columnIndex)) {
            gridElement->removeColumn(columnIndex, Q_NULLPTR);
        }
    }
}
