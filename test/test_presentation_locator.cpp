/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PresentationLocator class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>
#include <QPointF>

#include <ld_root_element.h>

#include <presentation_with_fixed_children.h>
#include <presentation_locator.h>

#include "test_presentation_locator.h"

/***********************************************************************************************************************
 * TestPresentationLocator
 */


TestPresentationLocator::TestPresentationLocator() {}


TestPresentationLocator::~TestPresentationLocator() {}


void TestPresentationLocator::initTestCase() {}


void TestPresentationLocator::testConstructorsAndDestructors() {
    PresentationLocator locator1;
    QVERIFY(locator1.x() == 0);
    QVERIFY(locator1.y() == 0);

    PresentationLocator locator2(2, 3);
    QVERIFY(locator2.x() == 2);
    QVERIFY(locator2.y() == 3);

    PresentationLocator locator3(3, 4);
    QVERIFY(locator3.x() == 3);
    QVERIFY(locator3.y() == 4);

    PresentationLocator locator4(QPointF(2, 3));
    QVERIFY(locator4.x() == 2);
    QVERIFY(locator4.y() == 3);

    PresentationLocator locator5(QPointF(3, 4));
    QVERIFY(locator5.x() == 3);
    QVERIFY(locator5.y() == 4);

    PresentationLocator locator6(locator2);
    QVERIFY(locator6.x() == 2);
    QVERIFY(locator6.y() == 3);
}


void TestPresentationLocator::testAccessors() {
    PresentationLocator locator(2, 3);
    QVERIFY(locator.x() == 2);
    QVERIFY(locator.y() == 3);

    locator.setPosition(4, 5);
    QVERIFY(locator.x() == 4);
    QVERIFY(locator.y() == 5);
    QVERIFY(locator.position() == QPointF(4, 5));

    locator.setPosition(QPointF(6, 7));
    QVERIFY(locator.x() == 6);
    QVERIFY(locator.y() == 7);
    QVERIFY(locator.position() == QPointF(6, 7));
}


void TestPresentationLocator::testAssignmentOperators() {
    PresentationLocator locator1(2, 3);
    QVERIFY(locator1.x() == 2);
    QVERIFY(locator1.y() == 3);

    PresentationLocator locator2(5, 6);
    QVERIFY(locator2.x() == 5);
    QVERIFY(locator2.y() == 6);

    locator1 = locator2;
    QVERIFY(locator1.x() == 5);
    QVERIFY(locator1.y() == 6);

    locator2.setPosition(8, 9);

    QVERIFY(locator1.x() == 5);
    QVERIFY(locator1.y() == 6);

    QVERIFY(locator2.x() == 8);
    QVERIFY(locator2.y() == 9);
}


void TestPresentationLocator::testComparisonOperator() {
    PresentationLocator::setLeftToRightOrdering();

    PresentationLocator locator11(QPointF(1, 1));
    PresentationLocator locator21(QPointF(2, 1));
    PresentationLocator locator12(QPointF(1, 2));
    PresentationLocator locator22(QPointF(2, 2));

    PresentationLocator locator11x(QPointF(1, 1));

    QVERIFY(locator11 == locator11x);
    QVERIFY((locator11 == locator21) == false);
    QVERIFY(locator11 != locator21);
    QVERIFY((locator11 != locator11x) == false);
    QVERIFY(locator11 != locator12);
    QVERIFY(locator11 != locator22);

    QVERIFY(locator11 < locator12);
    QVERIFY(locator11 < locator21);
    QVERIFY((locator12 < locator11) == false);
    QVERIFY((locator21 < locator11) == false);

    QVERIFY(locator12 > locator11);
    QVERIFY(locator21 > locator11);
    QVERIFY((locator11 > locator12) == false);
    QVERIFY((locator11 > locator21) == false);

    QVERIFY(locator11 <= locator11x);
    QVERIFY(locator11 <= locator12);
    QVERIFY(locator11 <= locator21);
    QVERIFY((locator12 <= locator11) == false);
    QVERIFY((locator21 <= locator11) == false);

    QVERIFY(locator11 >= locator11x);
    QVERIFY(locator12 >= locator11);
    QVERIFY(locator21 >= locator11);
    QVERIFY((locator11 >= locator12) == false);
    QVERIFY((locator11 >= locator21) == false);
}
