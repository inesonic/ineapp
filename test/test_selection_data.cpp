/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests of the Ld::SelectionData class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QtTest/QtTest>

#include <ld_selection_data.h>

#include "test_element_position.h"
#include "test_selection_data.h"

TestSelectionData::TestSelectionData() {}


TestSelectionData::~TestSelectionData() {}


void TestSelectionData::initTestCase() {
    Ld::Handle::initialize(0x12345678UL);
}


void TestSelectionData::testConstructors() {
    PositionTestElement* rawPointer1 = new PositionTestElement;
    PositionTestElement* rawPointer2 = new PositionTestElement;

    QSharedPointer<PositionTestElement> sharedPointer1(rawPointer1);
    QSharedPointer<PositionTestElement> sharedPointer2(rawPointer2);

    Ld::SelectionData::Entry entry2(sharedPointer1, 2);
    QCOMPARE(entry2.data(), sharedPointer1);
    QCOMPARE(entry2.relativePosition(), 2);

    Ld::SelectionData::Entry entry3(entry2);
    QCOMPARE(entry3.data(), sharedPointer1);
    QCOMPARE(entry3.relativePosition(), 2);

    QSharedPointer<PositionTestElement> pointer2(new PositionTestElement);
    Ld::SelectionData::Entry entry4(pointer2, 3);

    QCOMPARE(entry4.element(), pointer2);
    QCOMPARE(entry4.relativePosition(), 3);

    Ld::SelectionDataPointer selectionData1(new Ld::SelectionData);
    selectionData1->setWeakThis(selectionData1.toWeakRef());

    selectionData1->append(entry2);

    Ld::SelectionDataPointer selectionData2(new Ld::SelectionData(*selectionData1));
    QCOMPARE(*selectionData1, *selectionData2);
}


void TestSelectionData::testPositionAccessors() {
    QSharedPointer<PositionTestElement> pointer(new PositionTestElement);
    Ld::SelectionData::Entry entry(pointer, 3);

    QCOMPARE(entry.relativePosition(), 3);

    entry.setRelativePosition(4);
    QCOMPARE(entry.relativePosition(), 4);
}


void TestSelectionData::testListMethods() {
    Ld::SelectionDataPointer selectionData(new Ld::SelectionData);
    selectionData->setWeakThis(selectionData.toWeakRef());

    QSharedPointer<PositionTestElement> sharedPointer1(new PositionTestElement);
    QSharedPointer<PositionTestElement> sharedPointer2(new PositionTestElement);

    selectionData->append(sharedPointer1, Ld::Element::ChildPlacement::FIXED, 3, true);
    selectionData->append(sharedPointer2, Ld::Element::ChildPlacement::POSITIONAL, 4, false);

    QCOMPARE(selectionData->size(), 2U);

    QCOMPARE(selectionData->at(0).data(), sharedPointer1);
    QCOMPARE(selectionData->at(0).sourcePlacement(), Ld::Element::ChildPlacement::FIXED);
    QCOMPARE(selectionData->at(0).relativePosition(), 3);
    QCOMPARE(selectionData->at(0).isDistinct(), true);

    QCOMPARE(selectionData->at(1).data(), sharedPointer2);
    QCOMPARE(selectionData->at(0).sourcePlacement(), Ld::Element::ChildPlacement::POSITIONAL);
    QCOMPARE(selectionData->at(1).relativePosition(), 4);
    QCOMPARE(selectionData->at(1).isDistinct(), false);
}


void TestSelectionData::testAssignmentOperator() {
    QSharedPointer<PositionTestElement> pointer1(new PositionTestElement);
    QSharedPointer<PositionTestElement> pointer2(new PositionTestElement);

    Ld::SelectionData::Entry entry1(pointer1, 2);
    Ld::SelectionData::Entry entry2(pointer2, 3);

    QVERIFY(entry1 != entry2);
    QVERIFY(entry1.data() != entry2.data());
    QVERIFY(entry1.relativePosition() != entry2.relativePosition());

    entry1 = entry2;
    QCOMPARE(entry1.element(), pointer2);
    QCOMPARE(entry1.relativePosition(), 3);
}


void TestSelectionData::testComparisonOperators() {
    // These have been effectively tested above.
}
