/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Cursor class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QWeakPointer>
#include <QPainter>
#include <QPointF>
#include <QRectF>

#include <ld_element_structures.h>
#include <ld_element.h>
#include <ld_element_cursor.h>
#include <ld_paragraph_element.h>
#include <ld_text_element.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_page_break_element.h>
#include <ld_table_frame_element.h>
#include <ld_root_element.h>
#include <ld_special_characters.h>
#include <ld_cursor_state_collection_entry.h>
#include <ld_cursor_state_collection.h>
#include <ld_element_with_fixed_children.h>
#include <ld_element_with_positional_children.h>
#include <ld_element_with_grouped_children.h>
#include <ld_element_with_grid_children.h>
#include <ld_element_with_floating_children.h>

#include <cmath>
#include <limits>

#include <QDebug>

#include "presentation.h"
#include "fixer.h"
#include "root_presentation.h"
#include "cursor.h"

const QPen                      Cursor::cursorPen(Qt::GlobalColor::black);
const QBrush                    Cursor::cursorBrush(Qt::GlobalColor::white);
const QPainter::CompositionMode Cursor::compositionMode = QPainter::CompositionMode::CompositionMode_Difference;

Cursor::Cursor() {
    currentCursorType = Type::INVALID;
    lastCursorType    = Type::INVALID;

    lastSelectedElement.clear();
}


Cursor::Cursor(
        RootPresentation* newRootPresentation,
        QObject*          parent
    ):QObject(
        parent
    ),Ld::Cursor(
          newRootPresentation != Q_NULLPTR
        ? newRootPresentation->element().dynamicCast<Ld::RootElement>().toWeakRef()
        : QWeakPointer<Ld::RootElement>()
    ) {
    currentCursorType    = Type::INVALID;
    lastCursorType       = Type::INVALID;
    currentSelectionType = SelectionType::NO_SELECTION;

    lastSelectedElement.clear();
}


Cursor::Cursor(const Cursor& other):QObject(), Ld::Cursor(other) {
    currentCursorType          = other.currentCursorType;
    lastCursorType             = other.lastCursorType;
    currentSelectionType       = other.currentSelectionType;
    reportedSelectionCursor    = other.reportedSelectionCursor;
    lastSelectedElement        = other.lastSelectedElement;
    currentSelectionRectangles = other.currentSelectionRectangles;
    currentBoundingRectangle   = other.currentBoundingRectangle;
}


Cursor::~Cursor() {}


const Ld::ElementCursor& Cursor::selectionCursor() const {
    return reportedSelectionCursor;
}


Cursor::Type Cursor::cursorType() const {
    return currentCursorType;
}


Cursor::SelectionType Cursor::selectionType() const {
    return currentSelectionType;
}


const Ld::ElementCursor& Cursor::startCursor() const {
    const Ld::ElementCursor& elementCursor = Ld::Cursor::elementCursor();

    bool   useSelectionCursor = reportedSelectionCursor.isValid() && reportedSelectionCursor < elementCursor;
    return useSelectionCursor ? reportedSelectionCursor : elementCursor;
}


const Ld::ElementCursor& Cursor::endCursor() const {
    const Ld::ElementCursor& elementCursor = Ld::Cursor::elementCursor();

    bool   useSelectionCursor = reportedSelectionCursor.isValid() && reportedSelectionCursor >= elementCursor;
    return useSelectionCursor ? reportedSelectionCursor : elementCursor;
}


bool Cursor::moveForwardByElement(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveForwardByElement();
}


bool Cursor::moveBackwardByElement(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveBackwardByElement();
}


bool Cursor::moveForwardByRegion(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveForwardByRegion();
}


bool Cursor::moveBackwardByRegion(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveBackwardByRegion();
}


bool Cursor::moveForwardByWhitespace(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveForwardByWhitespace();
}


bool Cursor::moveBackwardByWhitespace(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveBackwardByWhitespace();
}


bool Cursor::moveForwardByCharacter(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveForwardByCharacter();
}


bool Cursor::moveBackwardByCharacter(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveBackwardByCharacter();
}


bool Cursor::moveToStartOfDocumentView(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveToStartOfDocumentView();
}


bool Cursor::moveToEndOfDocumentView(bool selection) {
    handleSelection(selection);
    return Ld::Cursor::moveToEndOfDocumentView();
}


bool Cursor::moveToEndOfLine(bool selection) {
    handleSelection(selection);

    bool isLast;

    if (okToAdjustCursor(isLast)) {
        Ld::ElementCursor newCursor   = Cursor::elementCursor();
        QPointF           newLocation = locationOfCursor(newCursor);
        Ld::ElementCursor lastCursor;
        QPointF           lastLocation;
        bool              isLast;

        do {
            lastCursor   = newCursor;
            lastLocation = newLocation;

            isLast      = newCursor.moveForwardByCharacter();
            newLocation = locationOfCursor(newCursor);
        } while (!isLast && newLocation.x() >= lastLocation.x());

        isLast = newCursor.fixPosition(false, isLast); // False keeps us from moving to the next line.
        updateFromElementCursor(newCursor, true);
    }

    return isLast;
}


bool Cursor::moveToStartOfLine(bool selection) {
    handleSelection(selection);

    bool isFirst = false;

    if (okToAdjustCursor(isFirst)) {
        Ld::ElementCursor newCursor   = Cursor::elementCursor();
        QPointF           newLocation = locationOfCursor(newCursor);
        Ld::ElementCursor lastCursor;
        QPointF           lastLocation;
        bool              isFirst;

        do {
            lastCursor   = newCursor;
            lastLocation = newLocation;

            isFirst      = newCursor.moveBackwardByCharacter();
            newLocation  = locationOfCursor(newCursor);
        } while (!isFirst && newLocation.x() <= lastLocation.x());

        isFirst = newCursor.fixPosition(true, isFirst); // True keeps us from moving to the previous line.
        updateFromElementCursor(newCursor, true);
    }

    return isFirst;
}


bool Cursor::moveForwardByLine(bool selection) {
    handleSelection(selection);

    bool isLast = false;

    if (okToAdjustCursor(isLast)) {
        Ld::ElementCursor newCursor        = Cursor::elementCursor();
        QPointF           startingLocation = locationOfCursor(newCursor);
        QPointF           newLocation      = startingLocation;
        Ld::ElementCursor lastCursor;
        QPointF           lastLocation;

        do {
            lastCursor   = newCursor;
            lastLocation = newLocation;

            isLast      = newCursor.moveForwardByCharacter();
            newLocation = locationOfCursor(newCursor);
        } while (!isLast && newLocation.x() >= lastLocation.x());

        bool iterated = false;
        lastCursor    = newCursor;
        lastLocation  = newLocation;

        while (!isLast && newLocation.x() < startingLocation.x() && newLocation.x() >= lastLocation.x()) {
            iterated = true;

            lastCursor   = newCursor;
            lastLocation = newLocation;

            isLast      = newCursor.moveForwardByCharacter();
            newLocation = locationOfCursor(newCursor);
        }

        if (!isLast && iterated                                                                    &&
            (lastLocation.x() > newLocation.x()                                               ||
             startingLocation.x() - lastLocation.x() < newLocation.x() - startingLocation.x()    )    ) {
            newCursor = lastCursor;
        }

        isLast = newCursor.fixPosition(false, isLast);  // False keeps us from moving past the end of the next line.
        updateFromElementCursor(newCursor, true);
    }

    return isLast;
}


bool Cursor::moveBackwardByLine(bool selection) {
    handleSelection(selection);

    bool isFirst = false;

    if (okToAdjustCursor(isFirst)) {
        Ld::ElementCursor newCursor        = Cursor::elementCursor();
        QPointF           startingLocation = locationOfCursor(newCursor);
        QPointF           newLocation      = startingLocation;
        Ld::ElementCursor lastCursor;
        QPointF           lastLocation;

        do {
            lastCursor   = newCursor;
            lastLocation = newLocation;

            isFirst      = newCursor.moveBackwardByCharacter();
            newLocation  = locationOfCursor(newCursor);
        } while (!isFirst && newLocation.x() <= lastLocation.x());

        bool iterated = false;
        lastCursor    = newCursor;
        lastLocation  = newLocation;

        while (!isFirst && newLocation.x() > startingLocation.x() && newLocation.x() <= lastLocation.x()) {
            iterated = true;

            lastCursor   = newCursor;
            lastLocation = newLocation;

            isFirst      = newCursor.moveBackwardByCharacter();
            newLocation  = locationOfCursor(newCursor);
        }

        if (!isFirst && iterated                                                                   &&
            (lastLocation.x() < newLocation.x()                                               ||
             lastLocation.x() - startingLocation.x() < startingLocation.x() - newLocation.x()    )    ) {
            newCursor = lastCursor;
        }

        isFirst = newCursor.fixPosition(true, isFirst); // true keeps us from moving past the start of the line.
        updateFromElementCursor(newCursor, true);
    }

    return isFirst;
}


bool Cursor::moveToPosition(const QPointF& newLocation, bool selection) {
    bool unresolvable = true;

    handleSelection(selection);

    if (okToAdjustCursor(unresolvable)) {
        Ld::ElementCursor cursor = cursorAtLocation(newLocation);

        if (cursor.isValid()) {
            unresolvable = cursor.fixPosition(false, false);
            updateFromElementCursor(cursor, true);
        }
    }

    return unresolvable;
}


bool Cursor::moveNearPosition(
        const QPointF& areaUpperLeft,
        const QPointF& areaLowerRight,
        const QPointF& newLocation,
        bool           selection
    ) {
    bool unresolvable = true;

    handleSelection(selection);

    if (okToAdjustCursor(unresolvable)) {
        double   horizontalStepSize = (areaLowerRight.x() - areaUpperLeft.x()) / 10.0;
        double   verticalStepSize   = (areaLowerRight.y() - areaUpperLeft.y()) / 10.0;
        double   distanceStepSize   = std::min(horizontalStepSize, verticalStepSize);
        unsigned distance           = 0;
        double   angleStepSize      = std::atan(1) / 2; // 45 degrees in radians.
        double   angle              = 0;
        double   angleOffset        = angleStepSize * 7; // below is first.

        Ld::ElementCursor cursor;
        bool              done;
        do {
            QPointF trialLocation = (
                  newLocation
                + QPointF(std::cos(angle + angleOffset), std::sin(angle + angleOffset)) * distance
            );

            cursor = cursorAtLocation(trialLocation);

            done = cursor.isValid();
            if (!done) {
                angle += angleStepSize;
                if (angle > 8 * angleStepSize || distance == 0) {
                    angle     = 0;
                    distance += distanceStepSize;
                }
            }
        } while (!done);

        unresolvable = cursor.fixPosition(false, false);
        updateFromElementCursor(cursor);
    }

    return unresolvable;
}


bool Cursor::moveNearPosition(const QRectF& areaBoundingRectangle, const QPointF& newLocation, bool selection) {
    return moveNearPosition(
        areaBoundingRectangle.topLeft(),
        areaBoundingRectangle.bottomRight(),
        newLocation,
        selection
    );
}


SelectionDataPointer Cursor::copySelection() const {
    SelectionDataPointer result(new SelectionData(currentSelectionType));
    result->setWeakThis(result);

    const Ld::ElementCursor& selectionCursor = Cursor::selectionCursor();
    const Ld::ElementCursor& elementCursor   = Cursor::elementCursor();

    if (elementCursor.isValid()) {
        Ld::ElementPointerList elementsToFixUp;

        if (selectionCursor.isValid()) {
            Ld::ElementCursor  startCursor  = Cursor::startCursor();
            Ld::ElementCursor  endCursor    = Cursor::endCursor();
            Ld::ElementPointer startElement = startCursor.element();
            Ld::ElementPointer endElement   = endCursor.element();

            if (startElement == endElement) {
                if (startElement->numberTextRegions() > 0 &&
                    startElement->numberChildren() == 0   &&
                    startCursor.atStartOfElement()        &&
                    endCursor.atEndOfElement(false)          ) {
                    startCursor.setWholeElement();
                    endCursor.setWholeElement();
                }

                // Copying a single element or text within the element.
                if (startCursor.isWholeElement() || endCursor.isWholeElement()) {
                    Ld::ElementPointer clone = startElement->clone(true);
                    result->append(clone, Q_NULLPTR);

                    Ld::ElementPointerSet descendants = clone->descendants();
                    elementsToFixUp.append(Ld::ElementPointerList(descendants.constBegin(), descendants.constEnd()));

                    addSidebandImageData(clone, startElement, result, true);
                    addSidebandCalculatedValues(clone, startElement, result);
                } else {
                    Ld::ElementPointer clone = startElement->clone(
                        startCursor.textIndex(),
                        startCursor.regionIndex(),
                        endCursor.textIndex(),
                        endCursor.regionIndex(),
                        false
                    );
                    result->append(clone, Q_NULLPTR);
                    elementsToFixUp << clone;

                    addSidebandImageData(clone, startElement, result, true);
                    addSidebandCalculatedValues(clone, startElement, result);
                }
            } else {
                // Copying over multiple elements.

                Ld::ElementCursor      cursor            = startCursor;
                Ld::ElementPointer     cursorElement     = startElement;
                Ld::ElementPointerList sourceEndingStack = endCursor.elementStack();
                Ld::ElementPointerSet  sourceEndingSet(sourceEndingStack.constBegin(), sourceEndingStack.constEnd());

                if (!startCursor.isWholeElement()) {
                    Ld::ElementPointer clone = cursorElement->clone(
                        cursor.textIndex(),
                        cursor.regionIndex(),
                        Ld::Element::invalidTextIndex,
                        Ld::Element::invalidRegionIndex,
                        false // Rules for selection ranges guarantee this element will have no children.
                    );
                    result->append(clone, Q_NULLPTR);
                    elementsToFixUp << clone;

                    addSidebandImageData(clone, cursorElement, result, true);
                    addSidebandCalculatedValues(clone, startElement, result);

                    advanceSelectionCursor(cursor, cursorElement);
                }

                while (!sourceEndingSet.contains(cursorElement)) {
                    Ld::ElementPointer clone = cursorElement->clone(true);
                    result->append(clone, Q_NULLPTR);
                    elementsToFixUp << clone;

                    Ld::ElementPointerSet descendants = clone->descendants();
                    elementsToFixUp.append(Ld::ElementPointerList(descendants.constBegin(), descendants.constEnd()));

                    addSidebandImageData(clone, cursorElement, result, true);
                    addSidebandCalculatedValues(clone, cursorElement, result);

                    advanceSelectionCursor(cursor, cursorElement);
                }

                unsigned               referenceDepth = cursor.depth();
                Ld::ElementPointerList copiedEndingStack;

                Ld::ElementPointer topTailElement;
                Ld::ElementPointer topTailClone;

                if (cursorElement != endElement) {
                    Ld::ElementPointer clone = cursorElement->clone(false);
                    copiedEndingStack << clone;

                    result->append(clone, Q_NULLPTR);
                    elementsToFixUp << clone;

                    topTailElement = cursorElement;
                    topTailClone   = clone;

                    cursor.moveForwardByElement();
                    cursorElement = cursor.element();
                }

                while (cursorElement != endElement) {
                    Ld::ElementPointer clone = cursorElement->clone(false);
                    addTailElement(referenceDepth, cursor, cursorElement, clone, copiedEndingStack);
                    elementsToFixUp << clone;

                    cursor.moveForwardByElement();
                    cursorElement = cursor.element();
                }

                Ld::ElementPointer clone;
                if (endCursor.isWholeElement()) {
                    clone = cursorElement->clone(true);
                } else {
                    clone = cursorElement->clone(
                        0,
                        0,
                        endCursor.textIndex(),
                        endCursor.regionIndex(),
                        false
                    );
                }

                if (topTailClone.isNull()) {
                    result->append(clone, Q_NULLPTR);

                    topTailElement = cursorElement;
                    topTailClone = clone;
                } else {
                    addTailElement(referenceDepth, cursor, cursorElement, clone, copiedEndingStack);
                }

                elementsToFixUp << clone;
                addSidebandImageData(topTailClone, topTailElement, result, true);
                addSidebandCalculatedValues(topTailClone, topTailElement, result);
            }
        } else if (elementCursor.isWholeElement()) {
            Ld::ElementPointer element = elementCursor.element();
            Ld::ElementPointer clone   = element->clone(true);
            result->append(clone, Q_NULLPTR);

            Ld::ElementPointerSet descendants = clone->descendants();
            elementsToFixUp = Ld::ElementPointerList(descendants.constBegin(), descendants.constEnd());

            addSidebandImageData(clone, element, result, true);
            addSidebandCalculatedValues(element, clone, result);
        }

        fixUpElements(elementsToFixUp);
    }

    return result;
}


bool Cursor::removeSelection(Ld::CursorStateCollection* cursorStateCollection) {
    bool                     success;

    const Ld::ElementCursor& elementCursor = Cursor::elementCursor();
    if (elementCursor.isValid()) {
        Ld::ElementPointerList elementsToRemove;
        Ld::ElementPointer     leftToMerge;
        Ld::ElementPointer     rightToMerge;
        Ld::ElementPointer     startToTextModify;
        Ld::ElementPointer     endToTextModify;

        Ld::ElementCursor      startCursor  = Cursor::startCursor();
        Ld::ElementCursor      endCursor    = Cursor::endCursor();
        Ld::ElementPointer     startElement = startCursor.element();
        Ld::ElementPointer     endElement   = endCursor.element();
        Ld::ElementPointerList endStack     = endCursor.elementStack();

        if (startElement == endElement            &&
            startElement->numberTextRegions() > 0 &&
            startElement->numberChildren() == 0   &&
            startCursor.atStartOfElement()        &&
            endCursor.atEndOfElement(false)          ) {
            startCursor.setWholeElement();
            endCursor.setWholeElement();
        }

        if (startElement == endElement && !startCursor.isWholeElement() && !endCursor.isWholeElement()) {
            startToTextModify = startElement;
            endToTextModify   = endElement;
        } else {
            Ld::ElementCursor     cursor        = startCursor;
            Ld::ElementPointer    cursorElement = startElement;
            Ld::ElementPointerSet endingSet(endStack.constBegin(), endStack.constEnd());

            if (!startCursor.isWholeElement()) {
                startToTextModify = startElement;
                leftToMerge       = startElement;

                if (!endingSet.contains(cursorElement)) {
                    advanceSelectionCursor(cursor, cursorElement);
                } else {
                    cursor.moveForwardByElement();
                    cursorElement = cursor.element();
                }
            } else {
                leftToMerge = startElement->previousSibling();
            }

            while (!endingSet.contains(cursorElement)) {
                elementsToRemove << cursorElement;
                advanceSelectionCursor(cursor, cursorElement);
            }

            while (cursorElement != endElement) {
                if (!endingSet.contains(cursorElement)) {
                    elementsToRemove << cursorElement;
                }

                cursor.moveForwardByElement();
                cursorElement = cursor.element();
            }

            if (!endCursor.isWholeElement()) {
                endToTextModify = endElement;
                rightToMerge    = endElement;
            } else {
                elementsToRemove << endElement;
                rightToMerge = endElement->nextSibling();
            }
        }

        Ld::ElementPointerSet removalSet(elementsToRemove.constBegin(), elementsToRemove.constEnd());

        Ld::ElementPointerList::const_iterator elementIterator    = elementsToRemove.constBegin();
        Ld::ElementPointerList::const_iterator elementEndIterator = elementsToRemove.constEnd();

        success = true;
        while (success && elementIterator != elementEndIterator) {
            Ld::ElementPointer element = *elementIterator;
            Ld::ElementPointer parent  = element->parent();

            // We only explicitly remove an element if we're not removing the parent.
            if (!removalSet.contains(parent)) {
                success = isDeleteAllowed(element, Fixer::DeleteReason::SELECTION_DELETE);
            }

            ++elementIterator;
        }

        if (success) {
            for (  Ld::ElementPointerList::const_iterator elementIterator    = elementsToRemove.constBegin(),
                                                          elementEndIterator = elementsToRemove.constEnd()
                 ; elementIterator != elementEndIterator
                 ; ++elementIterator
                ) {
                const Ld::ElementPointer& element = *elementIterator;
                Ld::ElementPointer        parent  = element->parent();

                // Don't bother explicitly removing children if the parent's also being removed.
                if (!removalSet.contains(parent)) {
                    deleteElementInSelection(parent, element, cursorStateCollection);
                }
            }

            if (!startToTextModify.isNull() && !endToTextModify.isNull() && startToTextModify == endToTextModify) {
                startToTextModify->removeText(
                    startCursor.textIndex(),
                    startCursor.regionIndex(),
                    endCursor.textIndex(),
                    endCursor.regionIndex(),
                    cursorStateCollection
                );
            } else {
                if (!startToTextModify.isNull()) {
                    startToTextModify->removeText(
                        startCursor.textIndex(),
                        startCursor.regionIndex(),
                        Ld::Element::invalidTextIndex,
                        Ld::Element::invalidRegionIndex,
                        cursorStateCollection
                    );
                }

                if (!endToTextModify.isNull()) {
                    endToTextModify->removeText(
                        0,
                        0,
                        endCursor.textIndex(),
                        endCursor.regionIndex(),
                        cursorStateCollection
                    );
                }
            }

            if (!leftToMerge.isNull()                    &&
                !rightToMerge.isNull()                   &&
                leftToMerge != rightToMerge              &&
                startCursor.depth() == endCursor.depth()    ) {
                Ld::ElementPointerList startStack = startCursor.elementStack();
                unsigned               treeDepth  = static_cast<unsigned>(startStack.size());
                unsigned               mergeDepth = treeDepth;
                while (mergeDepth > 0                                                                          &&
                       endStack.at(mergeDepth - 1) != startStack.at(mergeDepth - 1)                            &&
                       isMergeAllowed(startStack.at(mergeDepth - 1), endStack.at(mergeDepth - 1), true, false)    ) {
                    --mergeDepth;
                }

                Q_ASSERT(mergeDepth > 0);

                unsigned depth = mergeDepth;
                while (depth < treeDepth                                                    &&
                       startStack.at(depth - 1) == endStack.at(depth - 1)                   &&
                       isMergeAllowed(startStack.at(depth), endStack.at(depth), true, true)    ) {
                    Ld::ElementPointer parent     = endStack.at(depth - 1);
                    Ld::ElementPointer leftChild  = startStack.at(depth);
                    Ld::ElementPointer rightChild = endStack.at(depth);

                    mergeElements(leftChild, rightChild, cursorStateCollection);
                    parent->removeChild(rightChild, cursorStateCollection);

                    ++depth;
                }
            }
        }
    } else {
        success = false;
    }

    return success;
}


SelectionDataPointer Cursor::cutSelection(Ld::CursorStateCollection* cursorStateCollection) {
    SelectionDataPointer result = copySelection();
    if (!removeSelection(cursorStateCollection)) {
        result.reset();
    }

    return result;
}


Cursor::PasteResult Cursor::pasteSelection(
        SelectionDataPointer       selectionData,
        const Ld::ElementCursor&   elementCursor,
        Ld::CursorStateCollection* cursorStateCollection,
        Ld::ElementCursor*         endingCursor
    ) {
    PasteResult result = PasteResult::CAPABILITY_MISMATCH;

    if (elementCursor.isValid()) {
        // Validate that we can safely paste at the cursor position.  To simplfiy this somewhat, we rely on the
        // selection type to identify the type of data we're pasting.

        SelectionType      selectionType = selectionData->selectionType();
        Ld::ElementPointer element       = elementCursor.element();

        switch (selectionType) {
            case SelectionType::NO_SELECTION: {
                result = PasteResult::INVALID_PASTE_CURSOR;
                break;
            }

            case SelectionType::TEXT_ANNOTATION: {
                if (!elementCursor.isWholeElement()) {
                    if (element->typeName() == Ld::TextElement::elementName) {
                        result = pasteTextAnnotation(
                            selectionData,
                            elementCursor,
                            cursorStateCollection,
                            endingCursor
                        );
                    } else {
                        if (selectionData->numberChildren() == 1) {
                            result = pasteTextFromElement(
                                selectionData,
                                elementCursor,
                                cursorStateCollection,
                                endingCursor
                            );

                        }
                    }
                }

                break;
            }

            case SelectionType::TEXT_IN_OPERATOR: {
                Q_ASSERT(selectionData->numberChildren() == 1);

                if (element->typeName() == Ld::TextElement::elementName) {
                    result = pasteTextFromElementIntoAnnotation(
                        selectionData,
                        elementCursor,
                        cursorStateCollection,
                        endingCursor
                    );
                 } else if (!elementCursor.isWholeElement()) {
                    result = pasteTextFromElement(
                        selectionData,
                        elementCursor,
                        cursorStateCollection,
                        endingCursor
                    );
                } else {
                    result = pasteTextAsElement(
                        selectionData,
                        elementCursor,
                        cursorStateCollection,
                        endingCursor
                    );

                }

                break;
            }

            case SelectionType::OPERATOR_LIST: {
                result = pasteElementList(
                    selectionData,
                    elementCursor,
                    cursorStateCollection,
                    endingCursor
                );

                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }
    }

    return result;
}


Cursor::PasteResult Cursor::pasteSelection(
        SelectionDataPointer       selectionData,
        Ld::CursorStateCollection* cursorStateCollection,
        Ld::ElementCursor*         endingCursor
    ) {
    return pasteSelection(selectionData, elementCursor(), cursorStateCollection, endingCursor);
}


void Cursor::setRootPresentation(RootPresentation* newRootPresentation) {
    Q_ASSERT(newRootPresentation != Q_NULLPTR);

    QSharedPointer<Ld::RootElement> rootElement = newRootPresentation->element().dynamicCast<Ld::RootElement>();
    setRootElement(rootElement);
}


RootPresentation* Cursor::rootPresentation() const {
    QSharedPointer<Ld::RootElement> element = rootElement().toStrongRef();
    return dynamic_cast<RootPresentation*>(element->visual());
}


QRectF Cursor::cursorRectangle() {
    QRectF rectangle;

    if (!currentSelectionRectangles.isEmpty()) {
        rectangle = currentSelectionRectangles.last();
    }

    return rectangle;
}


const QList<QRectF>& Cursor::selectionRectangles() const {
    if (currentSelectionRectangles.isEmpty()) {
        updateSelectionRectangles();
    }

    return currentSelectionRectangles;
}


const QRectF& Cursor::boundingRectangle() const {
    if (currentSelectionRectangles.isEmpty()) {
        updateSelectionRectangles();
    }

    return currentBoundingRectangle;
}


QPointF Cursor::cursorPosition() const {
    QPointF result;

    if (elementCursor().isValid()) {
        result = locationOfCursor(elementCursor()).toQPointF();
    }

    return result;
}


void Cursor::drawCursor(QPainter* painter, const QRectF& sceneRectangle, bool makeVisible) {
    if (currentSelectionRectangles.isEmpty()) {
        updateSelectionRectangles();
    }

    if (!currentSelectionRectangles.isEmpty()) {
        Type currentCursorType = cursorType();

        if (currentCursorType == Type::TEXT) {
            if (makeVisible) {
                Q_ASSERT(currentSelectionRectangles.size() == 1);
                const QRectF& cursorRectangle = currentSelectionRectangles.last();

                painter->setPen(cursorPen);
                painter->drawLine(cursorRectangle.topLeft(), cursorRectangle.bottomRight());
            }
        } else if (currentCursorType == Type::ELEMENT        ||
                   currentCursorType == Type::EMPTY_POSITION ||
                   currentCursorType == Type::SELECTION         ) {
            painter->setPen(cursorPen);
            painter->setBrush(cursorBrush);
            painter->setCompositionMode(compositionMode);

            QList<QRectF>::const_iterator it  = currentSelectionRectangles.constBegin();
            QList<QRectF>::const_iterator end = currentSelectionRectangles.constEnd();
            while (it != end) {
                QRectF cursorRectangle = *it;
                if (cursorRectangle.width() == 0) {
                    cursorRectangle.setWidth(1);
                }

                if (cursorRectangle.intersects(sceneRectangle)) {
                    painter->drawRect(cursorRectangle);
                }

                ++it;
            }
        }
    }
}


QString Cursor::selectionAsText(bool* ok, QMap<unsigned long, Ld::ElementCursor>* breakdown) const {
    QString result;
    bool    valid = true;

    if (breakdown != nullptr) {
        breakdown->clear();
    }

    const Ld::ElementCursor& selectionCursor = Cursor::selectionCursor();
    const Ld::ElementCursor& elementCursor   = Cursor::elementCursor();
    if (elementCursor.isValid() && selectionCursor.isValid()) {
        bool                     endAtSelectionCursor = selectionCursor > elementCursor;
        const Ld::ElementCursor& startCursor = endAtSelectionCursor ? elementCursor : selectionCursor;
        const Ld::ElementCursor& endCursor   = endAtSelectionCursor ? selectionCursor : elementCursor;

        Ld::ElementCursor  activeCursor = startCursor;
        Ld::ElementPointer previousElement;
        unsigned           previousRegionIndex = 0;

        while (valid && activeCursor != endCursor) {
            Ld::ElementPointer element = activeCursor.element();
            if (element.isNull() || activeCursor.isWholeElement()) {
                valid = false;
                result.clear();
            } else {
                unsigned regionIndex = activeCursor.regionIndex();

                if (previousElement.isNull()                           ||
                    previousElement->aggregateTextDuringSearchAllowed(
                        element,
                        regionIndex,
                        previousRegionIndex
                    )                                                      ) {
                    unsigned long textIndex = activeCursor.textIndex();

                    QString newText;
                    if (textIndex == 0) {
                        newText = element->text(regionIndex);
                    } else {
                        newText = element->text(regionIndex).mid(textIndex);
                    }

                    if (breakdown != nullptr) {
                        breakdown->insert(static_cast<unsigned long>(result.size()), activeCursor);
                    }

                    if (element == endCursor.element() && regionIndex == endCursor.regionIndex()) {
                        result += newText.left(endCursor.textIndex() - textIndex);
                        activeCursor = endCursor;
                    } else {
                        result += newText;

                        activeCursor.moveForwardByRegion();
                        activeCursor.fixPosition(true, false);

                        previousElement     = element;
                        previousRegionIndex = regionIndex;
                    }
                } else {
                    valid = false;
                    result.clear();
                }
            }
        }
    } else {
        valid = false;
    }

    if (ok != nullptr) {
        *ok = valid;
    }

    return result;
}


void Cursor::forceScreenUpdate(bool emitSignals) {
    currentSelectionRectangles.clear();
    currentBoundingRectangle = QRectF();

    if (emitSignals) {
        emit cursorUpdated();
    }
}


void Cursor::cursorChanged(bool movingForward) {
    performSelectionCursorAdjustments(movingForward);
    updateCursorType();

    Ld::ElementPointer newlySelectedElement = elementCursor().element();

    if (newlySelectedElement != lastSelectedElement || currentCursorType != lastCursorType) {
        lastSelectedElement = newlySelectedElement;
        lastCursorType      = currentCursorType;

        emit elementStackChanged();
    }

    forceScreenUpdate(true);
}


Cursor& Cursor::operator=(const Cursor& other) {
    Ld::Cursor::operator=(other);

    currentCursorType          = other.currentCursorType;
    lastCursorType             = other.lastCursorType;
    currentSelectionType       = other.currentSelectionType;
    reportedSelectionCursor    = other.reportedSelectionCursor;
    lastSelectedElement        = other.lastSelectedElement;
    currentSelectionRectangles = other.currentSelectionRectangles;
    currentBoundingRectangle   = other.currentBoundingRectangle;

    return *this;
}


Cursor& Cursor::operator=(const Ld::Cursor& other) {
    QSharedPointer<Ld::RootElement> rootElement = other.rootElement();
    Cursor upcastOther(dynamic_cast<RootPresentation*>(rootElement->visual()));
    upcastOther.updateFromElementCursor(other.elementCursor());
    upcastOther.updateSelectionFromElementCursor(other.selectionCursor());

    return operator=(upcastOther);
}


void Cursor::updateCursorType() {
    const Ld::ElementCursor& elementCursor = Cursor::elementCursor();
    const Ld::ElementCursor& selectionCursor = Cursor::selectionCursor();

    Ld::ElementCursor::PositionType elementCursorType = elementCursor.positionType();

    if (elementCursorType == Ld::ElementCursor::PositionType::INVALID) {
        currentCursorType = Type::INVALID;
    } else {
        if (selectionCursor.isValid()) {
            currentCursorType = Type::SELECTION;
        } else {
            if (elementCursorType == Ld::ElementCursor::PositionType::ELEMENT) {
                if (elementCursor.element().isNull()) {
                    currentCursorType = Type::EMPTY_POSITION;
                } else {
                    currentCursorType = Type::ELEMENT;
                }
            } else {
                currentCursorType = Type::TEXT;
            }
        }
    }
}


void Cursor::performSelectionCursorAdjustments(bool movingForward) {
    // Rules:
    // - Do nothing if we don't currently have a selection.
    // - If one cursor is in a text field, the other cursor must also be in a text field.  Depth does not
    //   matter.
    // - If one cursor is parked on a whole element, the other cursor must also be parked on a whole element, both
    //   cursors must have the same depth and must have the same parent.
    // - Whenever possible, the selection cursor must be adjusted to meet the rules.  We do not adjust the
    //   element cursor.
    // - The reported selection cursor is not the same selection cursor managed by the child and we always
    //   calculate a new reported selection cursor.

    const Ld::ElementCursor& selectionCursor = Ld::Cursor::selectionCursor();
    const Ld::ElementCursor& elementCursor   = Cursor::elementCursor();
    if (selectionCursor.isValid() && elementCursor.isValid()) {
        Ld::ElementCursor  cursor                 = selectionCursor;
        Ld::ElementPointer elementCursorElement   = elementCursor.element();
        Ld::ElementPointer selectionCursorElement = selectionCursor.element();

        if (!elementCursorElement.isNull() && elementCursorElement->typeName() == Ld::TextElement::elementName) {
            if (selectionCursorElement.isNull()                                    ||
                selectionCursorElement->typeName() != Ld::TextElement::elementName    ) {
                // The element cursor points into text but the selection cursor does not.  Move the selection cursor
                // until it does point to text.  Note that, due to other mechanisms, this should always be possible.

                bool moveSelectionBackwards = movingForward;
                bool atEnd;
                do {
                    atEnd = moveSelectionBackwards ? cursor.moveBackwardByElement() : cursor.moveForwardByElement();
                    selectionCursorElement = cursor.element();
                } while (!atEnd                                                                   &&
                         (selectionCursorElement.isNull()                                    ||
                          selectionCursorElement->typeName() != Ld::TextElement::elementName    )    );

                if (moveSelectionBackwards) {
                    cursor.moveToLastPositionInElement();
                } else {
                    cursor.moveToFirstPositionInElement();
                }
            } else {
                // Selection cursor is on a text element so it's good where it is.
            }

            currentSelectionType = SelectionType::TEXT_ANNOTATION;
        } else if (!elementCursor.isWholeElement()) {
            // Element cursor does not point to a text element but it does point to text.  Force the selection
            // cursor to also point to text within this element.

            if (selectionCursorElement == elementCursorElement) {
                // Both cursors point to the same element.  Make certain the selection cursor is pointing to text.

                if (cursor.isWholeElement()) {
                    cursor.moveToFirstPositionInElement();
                }
            } else {
                // The cursors are pointing to different elements.  Adjust the selection cursor.

                if (cursor < elementCursor) {
                    cursor = elementCursor;
                    cursor.moveToFirstPositionInElement();
                } else /* if (cursor > elementCursor) */ {
                    cursor = elementCursor;
                    cursor.moveToLastPositionInElement();
                }
            }

            currentSelectionType = SelectionType::TEXT_IN_OPERATOR;
        } else {
            // Element cursor points to a non-text whole element.  Both cursors must be under the same parent element.

            Ld::ElementPointer elementCursorParent   = elementCursorElement->parent();
            Ld::ElementPointer selectionCursorParent = selectionCursorElement->parent();

            if (selectionCursorParent != elementCursorParent) {
                // Elements point to different parents, adjust selection cursor.

                if (cursor < elementCursor) {
                    cursor.setElement(elementCursorParent->child(0));
                } else {
                    cursor.setElement(elementCursorParent->child(elementCursorParent->numberChildren() - 1));
                }
            } else {
                // Elements point to the same parent.
            }

            cursor.setWholeElement();
            currentSelectionType = SelectionType::OPERATOR_LIST;
        }

        reportedSelectionCursor = cursor;
    } else{
        // No selection.
        reportedSelectionCursor.setInvalid();

        if (elementCursor.isWholeElement()) {
            currentSelectionType = SelectionType::OPERATOR_LIST;
        } else {
            currentSelectionType = SelectionType::NO_SELECTION;
        }
    }
}


void Cursor::handleSelection(bool selection) {
    const Ld::ElementCursor elementCursor   = Cursor::elementCursor();

    if (elementCursor.isValid()) {
        const Ld::ElementCursor selectionCursor = Cursor::selectionCursor();
        bool selectionCursorValid = selectionCursor.isValid();
        if (!selectionCursorValid && selection) {
            startSelection();
        } else if (selectionCursorValid && !selection) {
            clearSelection();
        }
    }
}


Ld::ElementCursor Cursor::cursorAtLocation(const QPointF& location) const {
    Ld::ElementPointer  rootElement      = Ld::Cursor::rootElement();
    RootPresentation*   rootPresentation = dynamic_cast<RootPresentation*>(rootElement->visual());

    const Presentation* bestPresentation;
    unsigned long       bestPresentationArea;
    QPointF             bestLocation;
    double bestDistance = rootPresentation->distanceToClosestPresentationArea(
        location,
        &bestPresentation,
        &bestPresentationArea,
        &bestLocation
    );

    (void) bestDistance; // For debug

    Ld::ElementCursor cursor;
    if (bestPresentation != Q_NULLPTR) {
        cursor = bestPresentation->presentationAtLocation(bestLocation, true);
    }

    return cursor;
}


Ld::Location Cursor::locationOfCursor(const Ld::ElementCursor& cursor) const {
    QPointF result;

    if (cursor.isValid()) {
        Ld::ElementPointer element = cursor.element();
        if (!element.isNull()) {
            Presentation* presentation = dynamic_cast<Presentation*>(element->visual());
            Q_ASSERT(presentation != Q_NULLPTR);

            QList<QRectF> cursorRectangles = presentation->cursorRangeToScene(cursor);
            if (!cursorRectangles.isEmpty()) {
                result = cursorRectangles.first().topLeft();
            }
        }
    }

    return Ld::Location(result);
}


void Cursor::updateSelectionRectangles() const {
    const Ld::ElementCursor elementCursor = Cursor::elementCursor();
    if (elementCursor.isValid()) {
        if (reportedSelectionCursor.isInvalid()) {
            Ld::ElementPointer element = elementCursor.element();
            Q_ASSERT(element != Q_NULLPTR);

            Presentation* presentation = dynamic_cast<Presentation*>(element->visual());
            Q_ASSERT(presentation != Q_NULLPTR);

            currentSelectionRectangles = presentation->cursorRangeToScene(elementCursor);
        } else {
            const Ld::ElementCursor& leftElementCursor  = startCursor();
            const Ld::ElementCursor& rightElementCursor = endCursor();
            Ld::ElementPointer       cursorElement      = leftElementCursor.element();
            Ld::ElementPointer       endingElement      = rightElementCursor.element();

            if (cursorElement != endingElement) {
                Ld::ElementCursor        cursor       = leftElementCursor;
                const Ld::ElementCursor& endingCursor = rightElementCursor;
                Ld::ElementPointerList   endingStack  = endingCursor.elementStack();
                Ld::ElementPointerSet    endingStackSet(endingStack.constBegin(), endingStack.constEnd());

                while (!endingStackSet.contains(cursorElement)) {
                    currentSelectionRectangles += selectionRectanglesForElement(cursor, cursorElement, endingCursor);
                }

                while (cursorElement != endingElement && cursor < endingCursor) {
                    currentSelectionRectangles += selectionRectanglesForElement(cursor, cursorElement, endingElement);
                }

                if (cursorElement == endingElement) {
                    cursor.moveToFirstPositionInElement();
                    currentSelectionRectangles += calculateSelectionRectanglesForElement(
                        cursorElement,
                        cursor,
                        endingCursor
                    );
                }
            } else {
                currentSelectionRectangles += calculateSelectionRectanglesForElement(
                    cursorElement,
                    leftElementCursor,
                    rightElementCursor
                );
            }
        }

        unsigned long numberRectangles = static_cast<unsigned long>(currentSelectionRectangles.size());
        if (numberRectangles > 0) {
            currentBoundingRectangle = currentSelectionRectangles.at(0);
            for (unsigned long i=1 ; i<numberRectangles ; ++i) {
                currentBoundingRectangle = currentBoundingRectangle.united(currentSelectionRectangles.at(i));
            }
        }
    } else {
        currentSelectionRectangles.clear();
        currentBoundingRectangle = QRectF();
    }
}


void Cursor::advanceSelectionCursor(Ld::ElementCursor& cursor, Ld::ElementPointer& element) {
    Ld::ElementPointer nextSibling = element->nextSibling();
    if (nextSibling.isNull()) {
        unsigned currentDepth = cursor.depth();
        do {
            cursor.moveForwardByElement();
        } while (cursor.depth() > currentDepth);
        element = cursor.element();
    } else {
        cursor.setElement(nextSibling);
        element = nextSibling;
    }
}


QList<QRectF> Cursor::selectionRectanglesForElement(
        Ld::ElementCursor&  cursor,
        Ld::ElementPointer& element,
        const Ld::ElementCursor& endingCursor
    ) {
    QList<QRectF> result;

    Presentation* presentation = dynamic_cast<Presentation*>(element->visual());
    if (presentation->cursorCanHighlight()) {
        result = presentation->cursorRangeToScene(endingCursor, cursor);
        advanceSelectionCursor(cursor, element);
    } else {
        cursor.moveForwardByElement();
        element = cursor.element();
    }

    return result;
}


QList<QRectF> Cursor::calculateSelectionRectanglesForElement(
        Ld::ElementPointer       element,
        const Ld::ElementCursor& startingCursor,
        const Ld::ElementCursor& endingCursor
    ) {
    Presentation* presentation = dynamic_cast<Presentation*>(element->visual());
    return   presentation->cursorCanHighlight()
           ? presentation->cursorRangeToScene(endingCursor, startingCursor)
           : QList<QRectF>();
}


void Cursor::addTailElement(
        unsigned                 referenceDepth,
        const Ld::ElementCursor& cursor,
        Ld::ElementPointer       cursorElement,
        Ld::ElementPointer       clone,
        Ld::ElementPointerList&  copiedEndingStack
    ) {
    unsigned sourceDepth = cursor.depth();
    Q_ASSERT(sourceDepth > referenceDepth);

    unsigned relativeDepth = sourceDepth - referenceDepth;
    Q_ASSERT(relativeDepth <= static_cast<unsigned>(copiedEndingStack.size()));

    Ld::ElementPointer          newParent      = copiedEndingStack.at(relativeDepth - 1);
    Ld::Element::ChildPlacement childPlacement = newParent->childPlacement();

    switch (childPlacement) {
        case Ld::Element::ChildPlacement::NONE: {
            Q_ASSERT(false);
            break;
        }

        case Ld::Element::ChildPlacement::FIXED: {
            unsigned long parentNumberChildren = newParent->numberChildren();
            newParent.dynamicCast<Ld::ElementWithFixedChildren>()->setChild(
                parentNumberChildren,
                clone,
                Q_NULLPTR
            );

            break;
        }

        case Ld::Element::ChildPlacement::POSITIONAL: {
            newParent.dynamicCast<Ld::ElementWithPositionalChildren>()->append(clone, Q_NULLPTR);
            break;
        }

        case Ld::Element::ChildPlacement::GROUPED: {
            QSharedPointer<Ld::ElementWithGroupedChildren>
                groupedParent = newParent.dynamicCast<Ld::ElementWithGroupedChildren>();

            QSharedPointer<Ld::ElementWithGroupedChildren>
                sourceParent = cursorElement->parent().dynamicCast<Ld::ElementWithGroupedChildren>();

            unsigned long sourceChildIndex = sourceParent->indexOfChild(cursorElement);
            unsigned      groupIndex       = sourceParent->groupContainingChild(sourceChildIndex);
            groupedParent->appendToGroup(groupIndex, clone, Q_NULLPTR);

            break;
        }

        case Ld::Element::ChildPlacement::GRID: {
            QSharedPointer<Ld::ElementWithGridChildren>
                gridParent = newParent.dynamicCast<Ld::ElementWithGridChildren>();

            QSharedPointer<Ld::ElementWithGridChildren>
                sourceParent = cursorElement->parent().dynamicCast<Ld::ElementWithGridChildren>();

            unsigned long sourceChildIndex = sourceParent->indexOfChild(cursorElement);
            unsigned long rowIndex         = sourceParent->rowIndexOfChild(sourceChildIndex);
            unsigned long columnIndex      = sourceParent->columnIndexOfChild(sourceChildIndex);

            gridParent->setChild(rowIndex, columnIndex, clone, Q_NULLPTR);

            break;
        }

        case Ld::Element::ChildPlacement::FLOATING: {
            QSharedPointer<Ld::ElementWithFloatingChildren>
                floatingParent = newParent.dynamicCast<Ld::ElementWithFloatingChildren>();

            QSharedPointer<Ld::ElementWithFloatingChildren>
                sourceParent = cursorElement->parent().dynamicCast<Ld::ElementWithFloatingChildren>();

            Ld::Location childLocation = sourceParent->childLocation(cursorElement);
            floatingParent->insertChild(clone, childLocation, Q_NULLPTR);

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    if (relativeDepth >= static_cast<unsigned>(copiedEndingStack.size())) {
        copiedEndingStack.append(clone);
    } else {
        copiedEndingStack[relativeDepth] = clone;
    }
}


void Cursor::addSidebandImageData(
        Ld::ElementPointer   clone,
        Ld::ElementPointer   original,
        SelectionDataPointer selectionData,
        bool                 isTopLevel
    ) {
    if (clone->exportImageCapability() == Ld::Element::ExportImageCapability::THROUGH_VISUAL_EPHEMERAL ||
        (isTopLevel                                                                           &&
         clone->exportImageCapability() == Ld::Element::ExportImageCapability::THROUGH_VISUAL    )        ) {
        QByteArray imagePayload = original->exportImage(defaultExportDpi);
        if (!imagePayload.isEmpty()) {
            selectionData->addImagePayload(clone, imagePayload);
        }
    }  else {
        unsigned long numberChildren = clone->numberChildren();
        if (clone->exportImageCapability() == Ld::Element::ExportImageCapability::NONE && numberChildren > 0) {
            for (unsigned childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
                Ld::ElementPointer cloneChild    = clone->child(childIndex);
                Ld::ElementPointer originalChild = original->child(childIndex);

                addSidebandImageData(cloneChild, originalChild, selectionData, false);
            }
        }
    }
}


void Cursor::addSidebandCalculatedValues(
        Ld::ElementPointer   clone,
        Ld::ElementPointer   original,
        SelectionDataPointer selectionData
    ) {
    if (!clone.isNull()) {
        if (clone->requiresCalculatedDataToRender()) {
            unsigned numberCalculatedValues = original->numberCalculatedValues();
            for (unsigned valueIndex=0 ; valueIndex<numberCalculatedValues ; ++valueIndex) {
                Ld::CalculatedValue calculatedValue = original->calculatedValue(valueIndex);
                selectionData->addCalculatedValue(clone, valueIndex, calculatedValue);
            }
        }

        unsigned long numberChildren = clone->numberChildren();
        for (unsigned long childIndex=0 ; childIndex<numberChildren ; ++childIndex) {
            Ld::ElementPointer originalChild = original->child(childIndex);
            Ld::ElementPointer cloneChild    = clone->child(childIndex);

            addSidebandCalculatedValues(cloneChild, originalChild, selectionData);
        }
    }
}


void Cursor::fixUpElements(const Ld::ElementPointerList& elementsToFixUp) {
    for (  Ld::ElementPointerList::const_iterator elementIterator    = elementsToFixUp.constBegin(),
                                                  elementEndIterator = elementsToFixUp.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        const Ld::ElementPointer& element = *elementIterator;
        const Fixer*              fixer   = Fixer::fixer(element->typeName());

        if (fixer != Q_NULLPTR) {
            fixer->processCopiedClone(element);
        }
    }
}


Cursor::PasteResult Cursor::pasteTextAnnotation(
        SelectionDataPointer       selectionData,
        const Ld::ElementCursor&   elementCursor,
        Ld::CursorStateCollection* cursorStateCollection,
        Ld::ElementCursor*         endingCursor
    ) {
    unsigned    rightCursorIndex = static_cast<unsigned>(-1);
    PasteResult result           = PasteResult::SUCCESS;

    Ld::ElementPointer insertionElement = elementCursor.element();
    Q_ASSERT(!insertionElement.isNull() && insertionElement->typeName() == Ld::TextElement::elementName);

    Ld::ElementPointer insertionParent = insertionElement->parent();
    Q_ASSERT(!insertionParent.isNull() && insertionParent->typeName() == Ld::ParagraphElement::elementName);

    Ld::ElementPointer insertionGrandparent = insertionParent->parent();
    Q_ASSERT(!insertionParent.isNull());

    bool          isTopLevel              = (insertionGrandparent->typeName() == Ld::RootElement::elementName);
    unsigned long numberSelectionChildren = selectionData->numberChildren();
    bool          requiresTopLevel        = false;
    bool          containsParagraphBreak  = false;

    for (unsigned long childIndex=0 ; childIndex<numberSelectionChildren ; ++childIndex) {
        Ld::ElementPointer selectionChild = selectionData->child(childIndex);
        QString            typeName       = selectionChild->typeName();

        if (typeName == Ld::ParagraphElement::elementName) {
            containsParagraphBreak = true;
        } else if (typeName == Ld::TableFrameElement::elementName || typeName == Ld::PageBreakElement::elementName) {
            requiresTopLevel       = true;
            containsParagraphBreak = true;
        }
    }

    if (isTopLevel || !requiresTopLevel) {
        unsigned long textInsertionIndex = elementCursor.childIndex() + 1; // We insert after this location.

        Ld::ElementPointer                   leftSibling   = insertionElement;
        QSharedPointer<Ld::ParagraphElement> leftParagraph = insertionParent.dynamicCast<Ld::ParagraphElement>();
        Ld::ElementPointer                   rightSibling;
        QSharedPointer<Ld::ParagraphElement> rightParagraph;
        Ld::ElementPointer                   firstInserted;
        Ld::ElementPointer                   lastInserted;

        if (!containsParagraphBreak) {
            if (isSplitAllowed(Fixer::SplitReason::INSERT_ELEMENT, elementCursor)) {
                splitElement(Fixer::SplitReason::INSERT_ELEMENT, elementCursor, cursorStateCollection);

                rightSibling = leftSibling->nextSibling();

                Ld::ElementCursor rightCursor(rightSibling);
                rightCursor.moveToFirstPositionInElement();
                rightCursorIndex = addCursorToCollection(rightCursor, cursorStateCollection);
            } else {
                result = PasteResult::SPLIT_NOT_ALLOWED;
            }
        } else {
            if (isSplitAllowed(Fixer::SplitReason::NEWLINE, elementCursor)) {
                splitElement(Fixer::SplitReason::NEWLINE, elementCursor, cursorStateCollection);

                rightParagraph = leftParagraph->nextSibling().dynamicCast<Ld::ParagraphElement>();
                rightSibling   = rightParagraph->child(0);

                Ld::ElementCursor rightCursor(rightSibling);
                rightCursor.moveToFirstPositionInElement();
                rightCursorIndex = addCursorToCollection(rightCursor, cursorStateCollection);
            } else {
                result = PasteResult::SPLIT_NOT_ALLOWED;
            }
        }

        if (result == PasteResult::SUCCESS && selectionData->numberChildren() > 0) {
            unsigned long      lastSelectionChildIndex = numberSelectionChildren - 1;
            Ld::ElementPointer lastSelectionChild      = selectionData->child(lastSelectionChildIndex);
            QString            lastSelectionChildType  = lastSelectionChild->typeName();
            unsigned long      childIndex              = 0;
            QString            selectionElementType;

            do {
                Ld::ElementPointer selectionElement = selectionData->child(childIndex);

                selectionElementType = selectionElement->typeName();
                if (selectionElementType != Ld::ParagraphElement::elementName  &&
                    selectionElementType != Ld::PageBreakElement::elementName  &&
                    selectionElementType != Ld::TableFrameElement::elementName    ) {
                    lastInserted = selectionElement->clone(true);

                    leftParagraph->insertBefore(textInsertionIndex + childIndex, lastInserted, cursorStateCollection);
                    pasteFixer(lastInserted, cursorStateCollection);

                    ++childIndex;
                }
            } while (childIndex < numberSelectionChildren                     &&
                     selectionElementType != Ld::ParagraphElement::elementName  &&
                     selectionElementType != Ld::PageBreakElement::elementName  &&
                     selectionElementType != Ld::TableFrameElement::elementName    );

            if (childIndex < numberSelectionChildren) {
                long paragraphInsertionOffset = (
                      insertionGrandparent->indexOfChild(insertionParent)
                    - childIndex
                    + 1
                );

                QSharedPointer<Ld::ElementWithPositionalChildren>
                    positionalGrandparent = insertionGrandparent.dynamicCast<Ld::ElementWithPositionalChildren>();

                unsigned long lastTopLevelChild;
                if (lastSelectionChildType == Ld::PageBreakElement::elementName  ||
                    lastSelectionChildType == Ld::TableFrameElement::elementName    ) {
                    lastTopLevelChild = lastSelectionChildIndex;
                } else {
                    lastTopLevelChild = lastSelectionChildIndex - 1;
                }

                while (childIndex <= lastTopLevelChild) {
                    Ld::ElementPointer selectionClone = selectionData->child(childIndex)->clone(true);

                    positionalGrandparent->insertBefore(
                        paragraphInsertionOffset + childIndex,
                        selectionClone,
                        cursorStateCollection
                    );
                    pasteFixer(selectionClone, cursorStateCollection);

                    ++childIndex;
                }

                if (childIndex <= lastSelectionChildIndex) {
                    if (lastSelectionChildType == Ld::ParagraphElement::elementName) {
                        QSharedPointer<Ld::ParagraphElement>
                            lastSelectionParagraph = lastSelectionChild.dynamicCast<Ld::ParagraphElement>();

                        unsigned long lastSelectionNumberChildren = lastSelectionParagraph->numberChildren();
                        for (unsigned long childIndex=0 ; childIndex<lastSelectionNumberChildren ; ++childIndex) {
                            lastInserted = lastSelectionParagraph->child(childIndex)->clone(true);

                            rightParagraph->insertBefore(childIndex, lastInserted, cursorStateCollection);
                            pasteFixer(lastInserted, cursorStateCollection);
                        }
                    } else {
                        lastInserted = lastSelectionChild->clone(true);
                        rightParagraph->insertBefore(0, lastInserted, cursorStateCollection);
                        pasteFixer(lastInserted, cursorStateCollection);
                    }
                }
            }

            if (rightCursorIndex == static_cast<unsigned>(-1)) {
                Ld::ElementCursor rightCursor(lastInserted);
                rightCursor.moveToLastPositionInElement();
                rightCursorIndex = addCursorToCollection(rightCursor, cursorStateCollection);
            }

            Ld::ElementPointer firstInserted = leftSibling->nextSibling();
            if (!rightSibling.isNull()) {
                Ld::ElementPointer rightParent = rightSibling->parent();
                if (!rightParent.isNull()                                  &&
                    lastInserted->parent() == rightParent                  &&
                    isMergeAllowed(lastInserted, rightSibling, true, true)    ) {
                    mergeElements(lastInserted, rightSibling, cursorStateCollection);
                    rightParent->removeChild(rightSibling, Q_NULLPTR);
                }
            }

            if (isMergeAllowed(leftSibling, firstInserted, true, true)) {
                mergeElements(leftSibling, firstInserted, cursorStateCollection);
                leftParagraph->removeChild(firstInserted, Q_NULLPTR);
            }
        }
    } else {
        // Can't insert tables or page breaks if we're not a top level paragraph or in a top level paragraph.
        result = PasteResult::REQUIRES_LOCATION_UNDER_ROOT;
    }

    if (rightCursorIndex != static_cast<unsigned>(-1)) {
        if (endingCursor != Q_NULLPTR) {
            *endingCursor = cursorStateCollection->at(rightCursorIndex).toElementCursor();
        }

        cursorStateCollection->removeAt(rightCursorIndex);
    }

    return result;
}


Cursor::PasteResult Cursor::pasteTextFromElementIntoAnnotation(
        SelectionDataPointer       selectionData,
        const Ld::ElementCursor&   elementCursor,
        Ld::CursorStateCollection* cursorStateCollection,
        Ld::ElementCursor*         endingCursor
    ) {
    unsigned rightCursorIndex = static_cast<unsigned>(-1);

    Ld::ElementPointer selectionElement  = selectionData->child(0);
    unsigned long      insertionIndex    = elementCursor.childIndex();
    Ld::ElementPointer leftSibling       = elementCursor.element();
    Ld::ElementPointer insertionParent   = leftSibling->parent();
    Ld::ElementPointer rightSibling;

    Q_ASSERT(!insertionParent.isNull() && insertionParent->typeName() == Ld::ParagraphElement::elementName);
    QSharedPointer<Ld::ParagraphElement> paragraph = insertionParent.dynamicCast<Ld::ParagraphElement>();

    if (isSplitAllowed(Fixer::SplitReason::INSERT_ELEMENT, elementCursor)) {
        splitElement(Fixer::SplitReason::INSERT_ELEMENT, elementCursor, cursorStateCollection);

        rightSibling = leftSibling->nextSibling();

        Ld::ElementCursor rightCursor(rightSibling);
        rightCursor.moveToFirstPositionInElement();
        rightCursorIndex = addCursorToCollection(rightCursor, cursorStateCollection);
    }

    Ld::FormatPointer                   sourceFormat = selectionElement->format();
    QSharedPointer<Ld::CharacterFormat> selectionFormat;
    if (sourceFormat.isNull() || !sourceFormat->capabilities().contains(Ld::FontFormat::formatName)) {
        selectionFormat = leftSibling->format().dynamicCast<Ld::CharacterFormat>();
    } else {
        selectionFormat.reset(new Ld::CharacterFormat(*sourceFormat.dynamicCast<Ld::FontFormat>()));
    }

    Ld::ElementPointer lastInserted;

    unsigned numberSelectionRegions = selectionElement->numberTextRegions();
    for (unsigned regionIndex=0 ; regionIndex<numberSelectionRegions ; ++regionIndex) {
        QString regionText = selectionElement->text(regionIndex);
        if (!regionText.isEmpty()) {
            QSharedPointer<Ld::CharacterFormat>
                insertionFormat = selectionFormat->clone().dynamicCast<Ld::CharacterFormat>();

            if (regionIndex != 0) {
                insertionFormat->setPosition(Ld::CharacterFormat::Position::SUBSCRIPT);

                if (regionIndex > 1) {
                    regionText = tr(",%1").arg(regionText);
                }
            }
            lastInserted = Ld::Element::create(Ld::TextElement::elementName);
            lastInserted->setText(regionText);
            lastInserted->setFormat(insertionFormat);

            ++insertionIndex;
            paragraph->insertBefore(insertionIndex, lastInserted, cursorStateCollection);
        }
    }

    if (!lastInserted.isNull()) {
        if (rightCursorIndex == static_cast<unsigned>(-1)) {
            Ld::ElementCursor rightCursor(lastInserted);
            rightCursor.moveToLastPositionInElement();
            rightCursorIndex = addCursorToCollection(rightCursor, cursorStateCollection);
        }

        Ld::ElementPointer firstInserted = leftSibling->nextSibling();

        if (!rightSibling.isNull() && isMergeAllowed(lastInserted, rightSibling, true, true)) {
            mergeElements(lastInserted, rightSibling, cursorStateCollection);
            paragraph->removeChild(rightSibling, cursorStateCollection);
        }

        if (!firstInserted.isNull() && isMergeAllowed(leftSibling, firstInserted, true, true)) {
            mergeElements(leftSibling, firstInserted, cursorStateCollection);
            paragraph->removeChild(firstInserted, cursorStateCollection);
        }

        if (rightCursorIndex != static_cast<unsigned>(-1)) {
            if (endingCursor != Q_NULLPTR) {
                *endingCursor = cursorStateCollection->at(rightCursorIndex).toElementCursor();
            }

            cursorStateCollection->removeAt(rightCursorIndex);
        }
    } else {
        if (endingCursor != Q_NULLPTR) {
            *endingCursor = elementCursor;
        }
    }

    return PasteResult::SUCCESS;
}


Cursor::PasteResult Cursor::pasteTextFromElement(
        SelectionDataPointer       selectionData,
        const Ld::ElementCursor&   elementCursor,
        Ld::CursorStateCollection* cursorStateCollection,
        Ld::ElementCursor*         endingCursor
    ) {
    PasteResult result = PasteResult::SUCCESS;

    // Create a string list holding the content we expect to end up with after the paste into this element.

    Ld::ElementPointer insertionElement       = elementCursor.element();
    unsigned           insertionRegionIndex   = elementCursor.regionIndex();
    unsigned long      insertionTextIndex     = elementCursor.textIndex();
    unsigned           insertionNumberRegions = insertionElement->numberTextRegions();
    unsigned           insertionLastRegion    = insertionNumberRegions - 1;

    unsigned             originalContentLength = 0;
    QList<unsigned long> originalStartIndexes;
    QStringList          originalContent;

    for (unsigned regionIndex=0 ; regionIndex<insertionNumberRegions ; ++regionIndex) {
        QString insertionText = insertionElement->text(regionIndex);

        originalContent << insertionElement->text(regionIndex);

        originalStartIndexes << originalContentLength;
        originalContentLength += insertionText.length();
    }

    Ld::ElementPointer selectionElement       = selectionData->child(0);
    unsigned           selectionNumberRegions = selectionElement->numberTextRegions();
    unsigned long      newContentLength       = 0;
    QStringList        newContent;
    for (unsigned regionIndex=0 ; regionIndex<selectionNumberRegions ; ++regionIndex) {
        QString content = selectionElement->text(regionIndex);
        if (!content.isEmpty()) {
            newContent << content;
            newContentLength += content.length();
        }
    }

    unsigned    originalContentNumberRegions = static_cast<unsigned>(originalContent.size());
    unsigned    newContentNumberRegions      = static_cast<unsigned>(newContent.size());
    unsigned    originalRegionIndex          = 0;
    unsigned    newContentIndex              = 0;
    QStringList mergedContent;
    while (originalRegionIndex < insertionRegionIndex) {
        mergedContent << originalContent.at(originalRegionIndex);
        ++originalRegionIndex;
    }

    unsigned      endingTextIndex;
    unsigned long endingRegionIndex;

    if (newContentNumberRegions < 2) {
        QString mergedRegionContent = (
              originalContent.at(originalRegionIndex).left(insertionTextIndex)
            + newContent.at(newContentIndex)
            + originalContent.at(originalRegionIndex).mid(insertionTextIndex)
        );

        endingRegionIndex = static_cast<unsigned>(mergedContent.size());
        endingTextIndex   = static_cast<unsigned long>(insertionTextIndex + newContent.at(newContentIndex).length());

        mergedContent << mergedRegionContent;
    } else {
        QString mergedRegionContent = (
              originalContent.at(originalRegionIndex).left(insertionTextIndex)
            + newContent.at(newContentIndex)
        );

        mergedContent << mergedRegionContent;
        ++newContentIndex;

        unsigned lastContentIndex = newContentNumberRegions - 1;
        while (newContentIndex < lastContentIndex) {
            mergedContent << newContent.at(newContentIndex);
            ++newContentIndex;
        }

        endingRegionIndex = static_cast<unsigned>(mergedContent.size());
        endingTextIndex   = static_cast<unsigned long>(newContent.last().length());

        mergedRegionContent = newContent.last() + originalContent.at(originalRegionIndex).mid(insertionTextIndex);
        mergedContent << mergedRegionContent;
    }

    ++originalRegionIndex;

    while (originalRegionIndex < originalContentNumberRegions) {
        mergedContent << originalContent.at(originalRegionIndex);
        ++originalRegionIndex;
    }

    unsigned numberMergedRegions = static_cast<unsigned>(mergedContent.size());
    Q_ASSERT(numberMergedRegions >= insertionNumberRegions);

    if (numberMergedRegions > insertionNumberRegions) {
        QString& lastRegion = mergedContent[insertionNumberRegions - 1];
        for (unsigned regionIndex=insertionNumberRegions ; regionIndex<numberMergedRegions ; ++regionIndex) {
            lastRegion += mergedContent.at(regionIndex);
        }
    }

    // We have an image of what the resulting element will look like.  Let's see if it's allowed by trying to apply it
    // to a clone of the original element.

    Ld::ElementPointer clone = insertionElement->clone(true);
    for (unsigned regionIndex=0 ; regionIndex<insertionNumberRegions ; ++regionIndex) {
        clone->setText(QString(), regionIndex);
    }

    unsigned mergeRegionIndex = 0;
    while (result == PasteResult::SUCCESS && mergeRegionIndex < insertionNumberRegions) {
        QString       mergedRegionText = mergedContent.at(mergeRegionIndex);
        unsigned long mergeTextIndex   = 0;
        unsigned long mergeTextLength  = static_cast<unsigned long>(mergedRegionText.length());

        while (result == PasteResult::SUCCESS && mergeTextIndex < mergeTextLength) {
            Ld::Capabilities requires = clone->textCapabilities(mergeTextIndex, mergeRegionIndex);
            if (!Ld::SpecialCharacters::textMeetsRequirements(mergedRegionText.at(mergeTextIndex), requires)) {
                result = PasteResult::TEXT_VIOLATES_RULES_FOR_ELEMENT;
            }

            clone->setText(mergedRegionText.left(mergeTextIndex + 1), mergeRegionIndex);
            ++mergeTextIndex;
        }

        ++mergeRegionIndex;
    }

    if (result == PasteResult::SUCCESS) {
        unsigned             mergeContentsLength = 0;
        QList<unsigned long> mergeStartIndexes;
        for (unsigned regionIndex=0 ; regionIndex<insertionNumberRegions ; ++regionIndex) {
            const QString& regionText = mergedContent.at(regionIndex);
            insertionElement->setText(regionText, regionIndex);

            mergeStartIndexes << mergeContentsLength;
            mergeContentsLength += regionText.length();
        }

        Ld::CursorStateCollection::iterator cursorIterator    = cursorStateCollection->begin();
        Ld::CursorStateCollection::iterator cursorEndIterator = cursorStateCollection->end();
        while (cursorIterator != cursorEndIterator) {
            if (cursorIterator->element() == selectionElement && !cursorIterator->isWholeElement()) {
                unsigned long oldTextIndex   = cursorIterator->textIndex();
                unsigned      oldRegionIndex = cursorIterator->regionIndex();

                if (oldRegionIndex > insertionRegionIndex                                           ||
                    (oldRegionIndex == insertionNumberRegions && oldTextIndex > insertionTextIndex)    ) {
                    unsigned long originalIndex = originalStartIndexes.at(oldRegionIndex) + oldTextIndex;
                    unsigned long newIndex      = originalIndex + newContentLength;

                    unsigned regionIndex = 0;
                    while (regionIndex < insertionLastRegion && newIndex <= mergeStartIndexes.at(regionIndex + 1)) {
                        ++regionIndex;
                    }

                    unsigned long textIndex = newIndex - mergeStartIndexes.at(regionIndex);
                    cursorIterator->update(textIndex, regionIndex, selectionElement);
                }
            }

            ++cursorIterator;
        }

        if (endingCursor != Q_NULLPTR) {
            endingCursor->setElement(insertionElement);
            endingCursor->setPositionInElement(endingTextIndex, endingRegionIndex);
        }
    }

    return result;
}


Cursor::PasteResult Cursor::pasteTextAsElement(
        SelectionDataPointer       selectionData,
        const Ld::ElementCursor&   elementCursor,
        Ld::CursorStateCollection* cursorStateCollection,
        Ld::ElementCursor*         endingCursor
    ) {
    PasteResult result = PasteResult::SUCCESS;

    // Extract the text from the selection element.

    Ld::ElementPointer selectionElement       = selectionData->child(0);
    unsigned           selectionNumberRegions = selectionElement->numberTextRegions();
    if (!selectionElement->text().isEmpty()) {
        QStringList newContent;
        for (unsigned regionIndex=0 ; regionIndex<selectionNumberRegions ; ++regionIndex) {
            QString content = selectionElement->text(regionIndex);
            newContent << content;
        }

        Ld::ElementPointer clone = selectionElement->clone(true);
        for (unsigned regionIndex=0 ; regionIndex<selectionNumberRegions ; ++regionIndex) {
            clone->setText(QString(), regionIndex);
        }

        unsigned regionIndex = 0;
        while (result == PasteResult::SUCCESS && regionIndex < selectionNumberRegions) {
            QString       regionText = newContent.at(regionIndex);
            unsigned long textIndex   = 0;
            unsigned long textLength  = static_cast<unsigned long>(regionText.length());

            while (result == PasteResult::SUCCESS && textIndex < textLength) {
                Ld::Capabilities requires = clone->textCapabilities(textIndex, regionIndex);
                if (!Ld::SpecialCharacters::textMeetsRequirements(regionText.at(textIndex), requires)) {
                    result = PasteResult::TEXT_VIOLATES_RULES_FOR_ELEMENT;
                }

                clone->setText(regionText.left(textIndex + 1), regionIndex);
                ++textIndex;
            }

            ++regionIndex;
        }
    } else {
        result = PasteResult::TEXT_VIOLATES_RULES_FOR_ELEMENT;
    }

    if (result == PasteResult::SUCCESS) {
        result = pasteElementList(selectionData, elementCursor, cursorStateCollection, endingCursor);
    }

    return result;
}


Cursor::PasteResult Cursor::pasteElementList(
        SelectionDataPointer       selectionData,
        const Ld::ElementCursor&   elementCursor,
        Ld::CursorStateCollection* cursorStateCollection,
        Ld::ElementCursor*         endingCursor
    ) {
    PasteResult            result                  = PasteResult::SUCCESS;
    Ld::ElementPointer     cursorParent            = elementCursor.parent();
    unsigned long          insertionIndex          = elementCursor.childIndex();
    unsigned long          numberSelectionItems    = selectionData->numberChildren();
    unsigned long          selectionIndex          = 0;
    Ld::ElementCursor      insertionCursor         = elementCursor;
    bool                   insertAfterLocation     = false;
    bool                   performWholeElementTest;
    Ld::ElementPointerList elementsToClone;

    // Like every other scenario, we need to handle things a bit differently if the cursor is pointed to text.

    if (!insertionCursor.isWholeElement()) {
        Ld::ElementPointer cursorElement = insertionCursor.element();
        if (cursorElement->typeName() == Ld::TextElement::elementName) {
            Q_ASSERT(!cursorParent.isNull() && cursorParent->typeName() == Ld::ParagraphElement::elementName);
            QSharedPointer<Ld::ParagraphElement> paragraph = cursorParent.dynamicCast<Ld::ParagraphElement>();

            unsigned parentIndex = insertionIndex + 1;
            while (result == PasteResult::SUCCESS && selectionIndex < numberSelectionItems) {
                Ld::ElementPointer selectionElement = selectionData->child(selectionIndex);
                Ld::Capabilities   childProvides    = selectionElement->childProvidesCapabilities();
                Ld::Capabilities   parentRequires   = paragraph->parentRequires(parentIndex);

                if (!parentRequires.intersects(childProvides)) {
                    result = PasteResult::CAPABILITY_MISMATCH;
                } else {
                    elementsToClone << selectionElement;
                }

                parentIndex += 2;
                ++selectionIndex;
            }

            if (result == PasteResult::SUCCESS && !isSplitAllowed(Fixer::SplitReason::INSERT_ELEMENT, insertionCursor)) {
                result = PasteResult::SPLIT_NOT_ALLOWED;
            }

            if (result == PasteResult::SUCCESS) {
                result = insertPastedElementsIntoText(
                    elementsToClone,
                    insertionCursor,
                    cursorStateCollection,
                    endingCursor
                );
            }

            performWholeElementTest = false;
        } else if (insertionCursor.atStartOfElement()) {
            performWholeElementTest = true;
            insertionCursor.setWholeElement();
        } else if (insertionCursor.atEndOfElement(false)) {
            insertAfterLocation     = true;
            performWholeElementTest = true;
            insertionCursor.setWholeElement();
        } else {
            performWholeElementTest = false;
            result = PasteResult::CAPABILITY_MISMATCH;
        }
    } else {
        performWholeElementTest = true;
    }

    if (performWholeElementTest) {
        Ld::Element::ChildPlacement parentPlacement = cursorParent->childPlacement();
        switch (parentPlacement) {
            case Ld::Element::ChildPlacement::NONE: {
                result = PasteResult::PARENT_DOES_NOT_ALLOW_CHILDREN;
                break;
            }

            case Ld::Element::ChildPlacement::FIXED:
            case Ld::Element::ChildPlacement::GRID: {
                // For this scenario, all locations that will be populated need to be empty or placeholders.

                if (insertAfterLocation) {
                    ++insertionIndex;
                }

                unsigned long parentNumberChildren = cursorParent->numberChildren();
                while (result == PasteResult::SUCCESS && selectionIndex < numberSelectionItems) {
                    unsigned long      parentIndex    = insertionIndex + selectionIndex;
                    Ld::ElementPointer childToReplace = cursorParent->child(parentIndex);
                    if (childToReplace.isNull() || childToReplace->isPlaceholder()) {
                        // Slot is not occupied.

                        Ld::ElementPointer selectionElement = selectionData->child(selectionIndex);
                        if (selectionElement->typeName() != Ld::TextElement::elementName ||
                            selectionElement->text().trimmed().isEmpty()                    ) {
                            if (parentIndex < parentNumberChildren) {
                                Ld::Capabilities childProvides  = selectionElement->childProvidesCapabilities();
                                Ld::Capabilities parentRequires = cursorParent->parentRequires(parentIndex);

                                if (!parentRequires.intersects(childProvides)) {
                                    result = PasteResult::CAPABILITY_MISMATCH;
                                } else {
                                    elementsToClone << selectionElement;
                                }
                            } else {
                                result = PasteResult::TOO_MANY_ELEMENTS;
                            }
                        }

                        ++selectionIndex;
                    } else {
                        result = PasteResult::LOCATION_OCCUPIED;
                    }
                }

                if (result == PasteResult::SUCCESS) {
                    result = insertPastedElementsIntoFixedElement(
                        elementsToClone,
                        insertionCursor,
                        insertAfterLocation,
                        cursorStateCollection,
                        endingCursor
                    );
                }

                break;
            }

            case Ld::Element::ChildPlacement::POSITIONAL: {
                // For this scenario, we insert before the current cursor position.

                unsigned long parentIndex    = insertionIndex;
                unsigned long oldParentIndex = insertionIndex;

                if (insertAfterLocation) {
                    ++parentIndex;
                    ++oldParentIndex;
                }

                unsigned long parentNumberChildren = cursorParent->numberChildren();
                while (result == PasteResult::SUCCESS && selectionIndex < numberSelectionItems) {
                    Ld::ElementPointer selectionElement = selectionData->child(selectionIndex);
                    if ((selectionElement->typeName() != Ld::TextElement::elementName ||
                         selectionElement->text().trimmed().isEmpty()                    ) &&
                        !selectionElement->isPlaceholder()                                    ) {
                        Ld::Capabilities childProvides  = selectionElement->childProvidesCapabilities();
                        Ld::Capabilities parentRequires = cursorParent->parentRequires(parentIndex);

                        if (!parentRequires.intersects(childProvides)) {
                            result = PasteResult::CAPABILITY_MISMATCH;
                        } else {
                            elementsToClone << selectionElement;
                        }
                    }

                    ++selectionIndex;
                    ++parentIndex;
                }

                while (result == PasteResult::SUCCESS && oldParentIndex < parentNumberChildren) {
                    Ld::ElementPointer child = cursorParent->child(oldParentIndex);
                    if (!child->isPlaceholder()) {
                        Ld::Capabilities   childProvides  = child->childProvidesCapabilities();
                        Ld::Capabilities   parentRequires = cursorParent->parentRequires(parentIndex);

                        if (!parentRequires.intersects(childProvides)) {
                            result = PasteResult::CAPABILITY_MISMATCH;
                        }
                    }

                    ++oldParentIndex;
                    ++parentIndex;
                }

                if (result == PasteResult::SUCCESS) {
                    result = insertPastedElementsIntoPositionalElement(
                        elementsToClone,
                        insertionCursor,
                        insertAfterLocation,
                        cursorStateCollection,
                        endingCursor
                    );
                }

                break;
            }

            case Ld::Element::ChildPlacement::GROUPED:
            case Ld::Element::ChildPlacement::FLOATING: {
                // At this time, these scenarios should not be possible.
                result = PasteResult::INVALID_PASTE_CURSOR;
                break;
            }

            default: {
                Q_ASSERT(false);
                break;
            }
        }
    }

    return result;
}


Cursor::PasteResult Cursor::insertPastedElementsIntoText(
        const Ld::ElementPointerList& elementsToClone,
        const Ld::ElementCursor&      elementCursor,
        Ld::CursorStateCollection*    cursorStateCollection,
        Ld::ElementCursor*            endingCursor
    ) {
    Ld::ElementPointer parent                  = elementCursor.parent();
    Ld::ElementPointer leftSibling             = elementCursor.element();
    unsigned long      insertionIndex          = elementCursor.childIndex();
    unsigned long      numberSelectionElements = static_cast<unsigned long>(elementsToClone.size());

    Q_ASSERT(parent->typeName() == Ld::ParagraphElement::elementName);
    Q_ASSERT(leftSibling->typeName() == Ld::TextElement::elementName);
    Q_ASSERT(isSplitAllowed(Fixer::SplitReason::INSERT_ELEMENT, elementCursor));

    splitElement(Fixer::SplitReason::INSERT_ELEMENT, elementCursor, cursorStateCollection);

    if (endingCursor != Q_NULLPTR) {
        endingCursor->setElement(leftSibling->nextSibling());
        endingCursor->moveToFirstPositionInElement();
    }

    QSharedPointer<Ld::ParagraphElement> paragraph = parent.dynamicCast<Ld::ParagraphElement>();
    for (unsigned childIndex=0 ; childIndex<numberSelectionElements ; ++childIndex) {
        Ld::ElementPointer elementToClone = elementsToClone.at(childIndex);
        if (!elementToClone.isNull()                                   &&
            elementToClone->typeName() != Ld::TextElement::elementName &&
            childIndex != 0                                               ) {
            Ld::ElementPointer textSeparator = Ld::Element::create(Ld::TextElement::elementName);
            textSeparator->setText(tr(" "));
            textSeparator->setFormat(leftSibling->format()->clone());

            ++insertionIndex;
            paragraph->insertBefore(insertionIndex, textSeparator, cursorStateCollection);
        }

        Ld::ElementPointer cloneToInsert = elementToClone->clone(true);

        ++insertionIndex;
        paragraph->insertBefore(insertionIndex, cloneToInsert, cursorStateCollection);

        pasteFixer(cloneToInsert, cursorStateCollection);
    }

    return PasteResult::SUCCESS;
}


Cursor::PasteResult Cursor::insertPastedElementsIntoFixedElement(
        const Ld::ElementPointerList& elementsToClone,
        const Ld::ElementCursor&      elementCursor,
        bool                          insertAfterLocation,
        Ld::CursorStateCollection*    cursorStateCollection,
        Ld::ElementCursor*            endingCursor
    ) {
    Ld::ElementPointer parent                  = elementCursor.parent();
    unsigned long      insertionIndex          = elementCursor.childIndex();
    unsigned long      numberSelectionElements = static_cast<unsigned long>(elementsToClone.size());

    if (insertAfterLocation) {
        ++insertionIndex;
    }

    Q_ASSERT(parent->childPlacement() == Ld::Element::ChildPlacement::FIXED);
    QSharedPointer<Ld::ElementWithFixedChildren> fixedParent = parent.dynamicCast<Ld::ElementWithFixedChildren>();

    for (unsigned long selectionIndex=0 ; selectionIndex<numberSelectionElements ; ++selectionIndex) {
        Ld::ElementPointer selectionElement = elementsToClone.at(selectionIndex);
        Ld::ElementPointer clone            = selectionElement->clone(true);
        fixedParent->setChild(selectionIndex + insertionIndex, clone, cursorStateCollection);
        pasteFixer(clone, cursorStateCollection);
    }

    if (endingCursor != Q_NULLPTR) {
        endingCursor->setElement(parent->child(insertionIndex + numberSelectionElements - 1));
        endingCursor->moveToFirstPositionInElement();
    }

    return PasteResult::SUCCESS;
}


Cursor::PasteResult Cursor::insertPastedElementsIntoPositionalElement(
        const Ld::ElementPointerList& elementsToClone,
        const Ld::ElementCursor&      elementCursor,
        bool                          insertAfterLocation,
        Ld::CursorStateCollection*    cursorStateCollection,
        Ld::ElementCursor*            endingCursor
    ) {
    Ld::ElementPointer parent                  = elementCursor.parent();
    unsigned long      insertionIndex          = elementCursor.childIndex();
    unsigned long      numberSelectionElements = static_cast<unsigned long>(elementsToClone.size());

    if (insertAfterLocation) {
        ++insertionIndex;
    }

    Q_ASSERT(parent->childPlacement() == Ld::Element::ChildPlacement::POSITIONAL);
    QSharedPointer<Ld::ElementWithPositionalChildren>
        positionalParent = parent.dynamicCast<Ld::ElementWithPositionalChildren>();

    for (unsigned long selectionIndex=0 ; selectionIndex<numberSelectionElements ; ++selectionIndex) {
        Ld::ElementPointer selectionElement = elementsToClone.at(selectionIndex);
        Ld::ElementPointer clone            = selectionElement->clone(true);

        positionalParent->insertBefore(insertionIndex + selectionIndex, clone, cursorStateCollection);

        pasteFixer(clone, cursorStateCollection);
    }

    if (endingCursor != Q_NULLPTR) {
        endingCursor->setElement(parent->child(insertionIndex + numberSelectionElements - 1));
        endingCursor->moveToFirstPositionInElement();
    }

    return PasteResult::SUCCESS;
}


unsigned Cursor::addCursorToCollection(
        const Ld::ElementCursor&   cursor,
        Ld::CursorStateCollection* cursorStateCollection
    ) {
    unsigned result;

    if (cursorStateCollection != Q_NULLPTR) {
        result = static_cast<unsigned>(cursorStateCollection->size());
        cursorStateCollection->append(Ld::CursorStateCollectionEntry(cursor));
    } else {
        result = static_cast<unsigned>(-1);
    }

    return result;
}


void Cursor::pasteFixer(Ld::ElementPointer element, Ld::CursorStateCollection* cursorStateCollection) {
    Q_ASSERT(!element.isNull());

    const Fixer* elementFixer = Fixer::fixer(element->typeName());
    if (elementFixer != Q_NULLPTR) {
        elementFixer->postPaste(element, cursorStateCollection);
    }

    Ld::ElementPointer parent = element->parent();
    Q_ASSERT(!parent.isNull());

    const Fixer* parentFixer = Fixer::fixer(parent->typeName());
    if (parentFixer != Q_NULLPTR) {
        parentFixer->postPasteChild(parent, element, parent->indexOfChild(element), cursorStateCollection);
    }
}
