/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PlacementLineStart class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>
#include <QPointF>

#include <ld_root_element.h>

#include <placement_line_start.h>

#include "test_placement_line_start.h"

TestPlacementLineStart::TestPlacementLineStart() {}


TestPlacementLineStart::~TestPlacementLineStart() {}


void TestPlacementLineStart::initTestCase() {}


void TestPlacementLineStart::testConstructorsAndDestructors() {
    PlacementLineStart area1;
    QCOMPARE(area1.lineY(), 0);
    QCOMPARE(area1.minimumTopSpacing(), PlacementNegotiator::imposeNoTopSpacing);
    QCOMPARE(area1.childIdentifier(), 0U);
    QCOMPARE(area1.presentationAreaId(), 0U);

    PlacementLineStart area2(1, 2, 3, 4);
    QCOMPARE(area2.lineY(), 1);
    QCOMPARE(area2.minimumTopSpacing(), 2);
    QCOMPARE(area2.childIdentifier(), 3U);
    QCOMPARE(area2.presentationAreaId(), 4U);

    PlacementLineStart area3(area2);
    QCOMPARE(area3.lineY(), 1);
    QCOMPARE(area3.minimumTopSpacing(), 2);
    QCOMPARE(area3.childIdentifier(), 3UL);
    QCOMPARE(area3.presentationAreaId(), 4U);
}


void TestPlacementLineStart::testAccessors() {
    PlacementLineStart area(1, 2, 3, 4);
    QCOMPARE(area.lineY(), 1);
    QCOMPARE(area.minimumTopSpacing(), 2);
    QCOMPARE(area.childIdentifier(), 3U);
    QCOMPARE(area.presentationAreaId(), 4U);

    area.setLineY(5);
    QCOMPARE(area.lineY(), 5);

    area.setMinimumTopSpacing(6);
    QCOMPARE(area.minimumTopSpacing(), 6);

    area.setChildIdentifier(7);
    QCOMPARE(area.childIdentifier(), 7U);

    area.setPresentationAreaId(8);
    QCOMPARE(area.presentationAreaId(), 8U);
}


void TestPlacementLineStart::testAssignmentOperator() {
    PlacementLineStart area1(1, 2, 3, 4);
    QCOMPARE(area1.lineY(), 1);
    QCOMPARE(area1.minimumTopSpacing(), 2);
    QCOMPARE(area1.childIdentifier(), 3U);
    QCOMPARE(area1.presentationAreaId(), 4U);

    PlacementLineStart area2(5, 6, 7, 8);
    QCOMPARE(area2.lineY(), 5);
    QCOMPARE(area2.minimumTopSpacing(), 6);
    QCOMPARE(area2.childIdentifier(), 7U);
    QCOMPARE(area2.presentationAreaId(), 8U);

    area1 = area2;
    QCOMPARE(area1.lineY(), 5);
    QCOMPARE(area1.minimumTopSpacing(), 6);
    QCOMPARE(area1.childIdentifier(), 7U);
    QCOMPARE(area1.presentationAreaId(), 8U);

    area2.setLineY(9);
    area2.setMinimumTopSpacing(10);
    area2.setChildIdentifier(11);
    area2.setPresentationAreaId(12);

    QCOMPARE(area1.lineY(), 5);
    QCOMPARE(area1.minimumTopSpacing(), 6);
    QCOMPARE(area1.childIdentifier(), 7U);
    QCOMPARE(area1.presentationAreaId(), 8U);

    QCOMPARE(area2.lineY(), 9);
    QCOMPARE(area2.minimumTopSpacing(), 10);
    QCOMPARE(area2.childIdentifier(), 11U);
    QCOMPARE(area2.presentationAreaId(), 12U);
}
