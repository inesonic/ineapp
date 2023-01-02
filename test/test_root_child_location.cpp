/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref RootChildLocation class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>
#include <QPointF>

#include <ld_root_element.h>

#include <presentation_with_fixed_children.h>
#include <root_child_location.h>

#include "test_root_child_location.h"

TestRootChildLocation::TestRootChildLocation() {}


TestRootChildLocation::~TestRootChildLocation() {}


void TestRootChildLocation::initTestCase() {}


void TestRootChildLocation::testConstructorsAndDestructors() {
    RootChildLocation childLocation1(1, 2, 3);
    QCOMPARE(childLocation1.topPageIndex(), 1U);
    QCOMPARE(childLocation1.topY(), 2);
    QCOMPARE(childLocation1.minimumTopSpacing(), 3);

    RootChildLocation childLocation2(childLocation1);
    QCOMPARE(childLocation2.topPageIndex(), 1U);
    QCOMPARE(childLocation2.topY(), 2);
    QCOMPARE(childLocation2.minimumTopSpacing(), 3);
}


void TestRootChildLocation::testAccessors() {
    RootChildLocation childLocation(1, 2, 3);
    QCOMPARE(childLocation.topPageIndex(), 1U);
    QCOMPARE(childLocation.topY(), 2);
    QCOMPARE(childLocation.minimumTopSpacing(), 3);

    childLocation.setTopPageIndex(4);
    QCOMPARE(childLocation.topPageIndex(), 4U);
    QCOMPARE(childLocation.topY(), 2);
    QCOMPARE(childLocation.minimumTopSpacing(), 3);

    childLocation.setTopY(5);
    QCOMPARE(childLocation.topPageIndex(), 4U);
    QCOMPARE(childLocation.topY(), 5);
    QCOMPARE(childLocation.minimumTopSpacing(), 3);

    childLocation.setMinimumTopSpacing(6);
    QCOMPARE(childLocation.topPageIndex(), 4U);
    QCOMPARE(childLocation.topY(), 5);
    QCOMPARE(childLocation.minimumTopSpacing(), 6);
}


void TestRootChildLocation::testAssignmentOperator() {
    RootChildLocation childLocation1(1, 2, 3);
    QCOMPARE(childLocation1.topPageIndex(), 1U);
    QCOMPARE(childLocation1.topY(), 2);
    QCOMPARE(childLocation1.minimumTopSpacing(), 3);

    RootChildLocation childLocation2(4, 5, 6);
    QCOMPARE(childLocation2.topPageIndex(), 4U);
    QCOMPARE(childLocation2.topY(), 5);
    QCOMPARE(childLocation2.minimumTopSpacing(), 6);

    childLocation2 = childLocation1;

    QCOMPARE(childLocation2.topPageIndex(), 1U);
    QCOMPARE(childLocation2.topY(), 2);
    QCOMPARE(childLocation2.minimumTopSpacing(), 3);
}


void TestRootChildLocation::testComparisonOperators() {
    RootChildLocation childLocation1(1, 2, 3);
    QCOMPARE(childLocation1.topPageIndex(), 1U);
    QCOMPARE(childLocation1.topY(), 2);
    QCOMPARE(childLocation1.minimumTopSpacing(), 3);

    RootChildLocation childLocation2(1, 2, 3);
    QCOMPARE(childLocation2.topPageIndex(), 1U);
    QCOMPARE(childLocation2.topY(), 2);
    QCOMPARE(childLocation2.minimumTopSpacing(), 3);

    QCOMPARE(childLocation1 == childLocation2, true);
    QCOMPARE(childLocation1 != childLocation2, false);
    QCOMPARE(childLocation1 < childLocation2,  false);
    QCOMPARE(childLocation1 > childLocation2,  false);
    QCOMPARE(childLocation1 <= childLocation2, true);
    QCOMPARE(childLocation1 >= childLocation2, true);

    childLocation2.setTopPageIndex(2);
    QCOMPARE(childLocation1 == childLocation2, false);
    QCOMPARE(childLocation1 != childLocation2, true);
    QCOMPARE(childLocation1 < childLocation2,  true);
    QCOMPARE(childLocation1 > childLocation2,  false);
    QCOMPARE(childLocation1 <= childLocation2, true);
    QCOMPARE(childLocation1 >= childLocation2, false);
    QCOMPARE(childLocation2 < childLocation1,  false);
    QCOMPARE(childLocation2 > childLocation1,  true);
    QCOMPARE(childLocation2 <= childLocation1, false);
    QCOMPARE(childLocation2 >= childLocation1, true);

    childLocation2.setTopPageIndex(1);

    childLocation2.setTopY(3);
    QCOMPARE(childLocation1 == childLocation2, false);
    QCOMPARE(childLocation1 != childLocation2, true);
    QCOMPARE(childLocation1 < childLocation2,  true);
    QCOMPARE(childLocation1 > childLocation2,  false);
    QCOMPARE(childLocation1 <= childLocation2, true);
    QCOMPARE(childLocation1 >= childLocation2, false);
    QCOMPARE(childLocation2 < childLocation1,  false);
    QCOMPARE(childLocation2 > childLocation1,  true);
    QCOMPARE(childLocation2 <= childLocation1, false);
    QCOMPARE(childLocation2 >= childLocation1, true);

    childLocation2.setTopY(2);

    childLocation2.setMinimumTopSpacing(4);
    QCOMPARE(childLocation1 == childLocation2, false);
    QCOMPARE(childLocation1 != childLocation2, true);
    QCOMPARE(childLocation1 < childLocation2,  true);
    QCOMPARE(childLocation1 > childLocation2,  false);
    QCOMPARE(childLocation1 <= childLocation2, true);
    QCOMPARE(childLocation1 >= childLocation2, false);
    QCOMPARE(childLocation2 < childLocation1,  false);
    QCOMPARE(childLocation2 > childLocation1,  true);
    QCOMPARE(childLocation2 <= childLocation1, false);
    QCOMPARE(childLocation2 >= childLocation1, true);
}
