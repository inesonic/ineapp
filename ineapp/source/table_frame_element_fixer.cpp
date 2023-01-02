/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFrameElementFixer class.
***********************************************************************************************************************/

#include <QString>
#include <QHash>

#include <algorithm>

#include <util_bit_array.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_table_frame_element.h>
#include <ld_paragraph_element.h>
#include <ld_text_element.h>
#include <ld_paragraph_format.h>
#include <ld_character_format.h>

#include "fixer.h"
#include "table_frame_element_fixer.h"

const TableFrameElementFixer TableFrameElementFixer::instance;

TableFrameElementFixer::TableFrameElementFixer() {}


TableFrameElementFixer::~TableFrameElementFixer() {}


bool TableFrameElementFixer::isDeleteAllowed(Ld::ElementPointer /* element */, DeleteReason /* reason */) const {
    return false;
}


bool TableFrameElementFixer::isMergeAllowed(
        Ld::ElementPointer leftElement,
        Ld::ElementPointer rightElement,
        bool               checkingLeft,
        bool               checkFormats
    ) const {
    bool result = false;

    if (checkingLeft) {
        Q_ASSERT(leftElement->typeName() == Ld::TableFrameElement::elementName);

        if (rightElement->typeName() == Ld::TableFrameElement::elementName) {
            if (checkFormats) {
                QString leftFormatString  = leftElement->format()->toString();
                QString rightFormatString = rightElement->format()->toString();

                result = (leftFormatString == rightFormatString);
            } else {
                result = true;
            }
        }
    }

    return result;
}


bool TableFrameElementFixer::isMergeChildrenAllowed(
        Ld::ElementPointer firstChild,
        Ld::ElementPointer secondChild
    ) const {
    QSharedPointer<Ld::TableFrameElement> parent = firstChild->parent().dynamicCast<Ld::TableFrameElement>();

    unsigned long firstChildIndex  = parent->indexOfChild(firstChild);
    unsigned long secondChildIndex = parent->indexOfChild(secondChild);

    unsigned      firstGroup       = parent->groupContainingChild(firstChildIndex);
    unsigned      secondGroup      = parent->groupContainingChild(secondChildIndex);

    return firstGroup == secondGroup;
}


bool TableFrameElementFixer::isSplitAllowed(
        Ld::ElementPointer                  element,
        TableFrameElementFixer::SplitReason reason,
        const Ld::ElementCursor&            cursor
    ) const {
    bool result = false;

    if (cursor.element() != element) {
        // We can't split tables but we can handle splits of children of tables.  Allow splits that don't also
        // require that this table split.
        //
        // Longer term, we may want to look at mechanisms that also allow table splits.

        switch (reason) {
            case SplitReason::NEWLINE:                  { result = true;    break; }
            case SplitReason::INSERT_ELEMENT:           { result = true;    break; }
            case SplitReason::INSERT_ELEMENT_TOP_LEVEL: { result = false;   break; }
            case SplitReason::FORMAT_CHANGE:            { result = true;    break; }
            case SplitReason::CUT:                      { result = true;    break; }
            default: {
                Q_ASSERT(false);
                break;
            }
        }
    }

    return result;
}


Ld::ElementCursor TableFrameElementFixer::mergeElements(
        Ld::ElementPointer         leftElement,
        Ld::ElementPointer         rightElement,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    QSharedPointer<Ld::TableFrameElement> leftTable  = leftElement.dynamicCast<Ld::TableFrameElement>();
    QSharedPointer<Ld::TableFrameElement> rightTable = rightElement.dynamicCast<Ld::TableFrameElement>();

    Q_ASSERT(!leftTable.isNull());
    Q_ASSERT(!rightTable.isNull());

    unsigned leftNumberRows    = leftTable->numberRows();
    unsigned leftNumberColumns = leftTable->numberColumns();
    unsigned rightNumberRows   = rightTable->numberRows();
    unsigned rightNumberColumns = rightTable->numberColumns();

    unsigned newNumberColumns = std::max(leftNumberColumns, rightNumberColumns);
    if (leftNumberColumns < newNumberColumns) {
        leftTable->insertColumnsAfter(leftNumberColumns, newNumberColumns - leftNumberColumns, true);
    }

    leftTable->insertRowsAfter(leftNumberRows, rightNumberRows, true);

    Util::BitArray processedGroups(rightTable->numberGroups());
    for (unsigned rowIndex=0 ; rowIndex<rightNumberRows ; ++rowIndex) {
        for (unsigned columnIndex=0 ; columnIndex<rightNumberColumns ; ++columnIndex) {
            unsigned groupAtLocation = rightTable->groupAt(rowIndex, columnIndex);
            if (processedGroups.isClear(groupAtLocation)) {
                processedGroups.setBit(groupAtLocation);

                unsigned bottomRow   = rightTable->bottomRow(rowIndex, columnIndex);
                unsigned rightColumn = rightTable->rightColumn(rowIndex, columnIndex);

                if ((rightColumn != Ld::TableFrameElement::invalidColumn &&
                     rightColumn != columnIndex                             ) ||
                    (bottomRow != Ld::TableFrameElement::invalidRow &&
                     bottomRow != rowIndex                             )         ) {
                    leftTable->mergeCells(
                        rowIndex + leftNumberRows,
                        columnIndex,
                        rightColumn - columnIndex + 1,
                        bottomRow - (rowIndex + leftNumberRows) + 1,
                        true,
                        Q_NULLPTR
                    );
                }

                // By using the children in the right table as the new children, the cursor state collection will
                // automatically point to the new locations without any work being done here.  To preserve the right
                // table, just in case we need it, we insert clones of all the elements back into the right table.

                Ld::ElementPointerList replacementChildren;
                Ld::ElementPointerList childrenInCell = rightTable->childrenInCell(rowIndex, columnIndex);
                for (  Ld::ElementPointerList::const_iterator childIterator    = childrenInCell.constBegin(),
                                                              childEndIterator = childrenInCell.constEnd()
                     ; childIterator != childEndIterator
                     ; ++childIterator
                    ) {
                    Ld::ElementPointer child          = *childIterator;
                    Ld::ElementPointer clonedChild    = !child.isNull() ? child->clone(true) : Ld::ElementPointer();
                    unsigned           leftGroupIndex = leftTable->groupAt(rowIndex + leftNumberRows , columnIndex);
                    leftTable->appendToGroup(leftGroupIndex, child, cursorStateCollection);
                    replacementChildren << clonedChild;
                }

                for (  Ld::ElementPointerList::const_iterator childIterator    = replacementChildren.constBegin(),
                                                              childEndIterator = replacementChildren.constEnd()
                     ; childIterator != childEndIterator
                     ; ++childIterator
                    ) {
                    Ld::ElementPointer child = *childIterator;
                    rightTable->appendToGroup(groupAtLocation, child, Q_NULLPTR);
                }
            }
        }
    }

    populateEmptyCells(leftTable, cursorStateCollection);

    Ld::ElementCursor result(leftTable->childInCell(leftNumberColumns, 0, 0));
    result.fixPosition(false, false);

    return result;
}


void TableFrameElementFixer::postPaste(
        Ld::ElementPointer         /* element */,
        Ld::CursorStateCollection* /* cursorStateCollection */
    ) const {}


void TableFrameElementFixer::postPasteChild(
        Ld::ElementPointer         parent,
        Ld::ElementPointer         /* child */,
        unsigned long              childIndex,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    QSharedPointer<Ld::TableFrameElement> tableParent             = parent.dynamicCast<Ld::TableFrameElement>();
    unsigned                              groupIndex              = tableParent->groupContainingChild(childIndex);
    unsigned long                         numberChildrenThisGroup = tableParent->numberChildrenInGroup(groupIndex);

    if (numberChildrenThisGroup == 2) {
        Ld::ElementPointer secondChild = tableParent->child(childIndex + 1);
        if (isDefaultParagraph(secondChild)) {
            tableParent->removeChild(childIndex + 1, cursorStateCollection);
            // Because the newly inserted element is supposed to replace the default element, we don't adjust
            // childIndex;
        }
    }
}


void TableFrameElementFixer::preInsert(Ld::ElementPointer element) const {
    populateEmptyCells(element);
}


void TableFrameElementFixer::removeDefaultChildren(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    QSharedPointer<Ld::TableFrameElement> tableElement = element.dynamicCast<Ld::TableFrameElement>();
    unsigned                              numberGroups = tableElement->numberGroups();

    for (unsigned groupIndex=0 ; groupIndex<numberGroups ; ++groupIndex) {
        unsigned long numberChildrenInGroup = tableElement->numberChildrenInGroup(groupIndex);
        if (numberChildrenInGroup == 1) {
            Ld::ElementPointer child = tableElement->childInGroup(groupIndex, 0);
            if (isDefaultParagraph(child)) {
                tableElement->removeChildFromGroup(groupIndex, 0, cursorStateCollection);
            }
        }
    }
}


void TableFrameElementFixer::processSplitChildren(
        Ld::ElementPointer                  parent,
        Ld::ElementPointer                  leftChild,
        Ld::ElementPointer                  rightChild,
        TableFrameElementFixer::SplitReason /* reason */,
        const Ld::ElementCursor&            /* cursor */,
        Ld::CursorStateCollection*          cursorStateCollection
    ) const {
    // Right now, we can't split tables so we assume all operations require adding the right child into the same
    // group as the left child, just after the left child.

    QSharedPointer<Ld::TableFrameElement> table          = parent.dynamicCast<Ld::TableFrameElement>();
    unsigned long                         leftChildIndex = table->indexOfChild(leftChild);

    unsigned long childIndexInGroup;
    unsigned      groupIndex = table->groupContainingChild(leftChildIndex, &childIndexInGroup);

    table->insertIntoGroupAfter(groupIndex, childIndexInGroup, rightChild, cursorStateCollection);
}


void TableFrameElementFixer::processCopiedClone(Ld::ElementPointer element) const {
    removeUnusedRowsAndColumns(element, Q_NULLPTR);
    // Note that copied tables may contain empty cells which we don't allow.  We address these empty cells during paste
    // operations.
}


void TableFrameElementFixer::removeUnusedRowsAndColumns(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    QSharedPointer<Ld::TableFrameElement> table = element.dynamicCast<Ld::TableFrameElement>();
    unsigned numberRows    = table->numberRows();
    unsigned numberColumns = table->numberColumns();
    unsigned numberGroups  = table->numberGroups();

    QSet<unsigned> emptyGroups;
    for (unsigned groupIndex=0 ; groupIndex<numberGroups ; ++groupIndex) {
        if (table->numberChildrenInGroup(groupIndex) == 0) {
            emptyGroups.insert(groupIndex);
        }
    }

    QSet<unsigned> occupiedRows;
    QSet<unsigned> occupiedColumns;

    unsigned rowIndex    = 0;
    unsigned columnIndex = 0;
    while (rowIndex<numberRows) {
        for (columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            unsigned groupIndex = table->groupAt(rowIndex, columnIndex);
            if (!emptyGroups.contains(groupIndex)) {
                occupiedRows.insert(rowIndex);
                occupiedColumns.insert(columnIndex);
            }
        }

        ++rowIndex;
    }

    // rowIndex = numberRows;
    while (rowIndex > 0) {
        --rowIndex;
        if (!occupiedRows.contains(rowIndex)) {
            table->removeRow(rowIndex, true, cursorStateCollection);
        }
    }

    // columnIndex = numberColumns;
    while (columnIndex > 0) {
        --columnIndex;
        if (!occupiedColumns.contains(columnIndex)) {
            table->removeColumn(columnIndex, true, cursorStateCollection);
        }
    }
}


void TableFrameElementFixer::populateEmptyCells(
        Ld::ElementPointer         element,
        Ld::CursorStateCollection* cursorStateCollection
    ) const {
    QSharedPointer<Ld::TableFrameElement> tableElement = element.dynamicCast<Ld::TableFrameElement>();
    unsigned                              numberGroups = tableElement->numberGroups();
    for (unsigned groupIndex=0 ; groupIndex<numberGroups ; ++groupIndex) {
        unsigned long numberChildrenInGroup = tableElement->numberChildrenInGroup(groupIndex);
        if (numberChildrenInGroup == 0) {
            Ld::ElementPointer newChild = createDefaultParagraph();
            tableElement->insertIntoGroupBefore(groupIndex, 0, newChild, cursorStateCollection);
        }
    }
}
