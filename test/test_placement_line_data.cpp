/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PlacementLineData class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>

#include <placement_line_data.h>

#include "test_placement_line_data.h"

TestPlacementLineData::TestPlacementLineData() {}


TestPlacementLineData::~TestPlacementLineData() {}


void TestPlacementLineData::initTestCase() {}


void TestPlacementLineData::testConstructorsAndDestructors() {
    PlacementLineData lineData1;
    QVERIFY(lineData1.childIdentifier() == 0);
    QVERIFY(lineData1.presentationAreaId() == 0);
    QVERIFY(lineData1.width() == 0);
    QVERIFY(lineData1.height() == 0);
    QVERIFY(lineData1.ascent() == 0);
    QVERIFY(lineData1.canStretch() == false);

    PlacementLineData lineData2(1, 2, 3, 4, 5, true);
    QVERIFY(lineData2.childIdentifier() == 1);
    QVERIFY(lineData2.presentationAreaId() == 2);
    QVERIFY(lineData2.width() == 3);
    QVERIFY(lineData2.height() == 4);
    QVERIFY(lineData2.ascent() == 5);
    QVERIFY(lineData2.canStretch() == true);
}


void TestPlacementLineData::testAccessors() {
    PlacementLineData lineData;
    QVERIFY(lineData.childIdentifier() == 0);
    QVERIFY(lineData.presentationAreaId() == 0);
    QVERIFY(lineData.width() == 0);
    QVERIFY(lineData.height() == 0);
    QVERIFY(lineData.ascent() == 0);
    QVERIFY(lineData.canStretch() == false);

    lineData.setChildIdentifier(1);
    QVERIFY(lineData.childIdentifier() == 1);

    lineData.setPresentationAreaId(2);
    QVERIFY(lineData.presentationAreaId() == 2);

    lineData.setWidth(3);
    QVERIFY(lineData.width() == 3);

    lineData.setHeight(4);
    QVERIFY(lineData.height() == 4);

    lineData.setAscent(5);
    QVERIFY(lineData.ascent() == 5);

    lineData.setStretchable(true);
    QVERIFY(lineData.canStretch() == true);

    QVERIFY(lineData.childIdentifier() == 1);
    QVERIFY(lineData.presentationAreaId() == 2);
    QVERIFY(lineData.width() == 3);
    QVERIFY(lineData.height() == 4);
    QVERIFY(lineData.ascent() == 5);
    QVERIFY(lineData.canStretch() == true);
}


void TestPlacementLineData::testAssignmentOperators() {
    PlacementLineData lineData1;
    QVERIFY(lineData1.childIdentifier() == 0);
    QVERIFY(lineData1.presentationAreaId() == 0);
    QVERIFY(lineData1.width() == 0);
    QVERIFY(lineData1.height() == 0);
    QVERIFY(lineData1.ascent() == 0);
    QVERIFY(lineData1.canStretch() == false);

    PlacementLineData lineData2(1, 2, 3, 4, 5, true);
    QVERIFY(lineData2.childIdentifier() == 1);
    QVERIFY(lineData2.presentationAreaId() == 2);
    QVERIFY(lineData2.width() == 3);
    QVERIFY(lineData2.height() == 4);
    QVERIFY(lineData2.ascent() == 5);
    QVERIFY(lineData2.canStretch() == true);

    lineData1 = lineData2;

    QVERIFY(lineData1.childIdentifier() == 1);
    QVERIFY(lineData1.presentationAreaId() == 2);
    QVERIFY(lineData1.width() == 3);
    QVERIFY(lineData1.height() == 4);
    QVERIFY(lineData1.ascent() == 5);
    QVERIFY(lineData1.canStretch() == true);

    lineData2.setChildIdentifier(5);
    lineData2.setPresentationAreaId(6);
    lineData2.setWidth(7);
    lineData2.setHeight(8);
    lineData2.setAscent(9);
    lineData2.setStretchable(false);

    QVERIFY(lineData1.childIdentifier() == 1);
    QVERIFY(lineData1.presentationAreaId() == 2);
    QVERIFY(lineData1.width() == 3);
    QVERIFY(lineData1.height() == 4);
    QVERIFY(lineData1.ascent() == 5);
    QVERIFY(lineData1.canStretch() == true);

    QVERIFY(lineData2.childIdentifier() == 5);
    QVERIFY(lineData2.presentationAreaId() == 6);
    QVERIFY(lineData2.width() == 7);
    QVERIFY(lineData2.height() == 8);
    QVERIFY(lineData2.ascent() == 9);
    QVERIFY(lineData2.canStretch() == false);
}
