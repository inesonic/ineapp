/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PresentationArea class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>
#include <QPointF>

#include <ld_root_element.h>

#include <presentation_with_fixed_children.h>
#include <presentation_area.h>

#include "test_presentation_area.h"

TestPresentationArea::TestPresentationArea() {}


TestPresentationArea::~TestPresentationArea() {}


void TestPresentationArea::initTestCase() {}


void TestPresentationArea::testConstructorsAndDestructors() {
    PresentationArea area1;
    QVERIFY(area1.childIdentifier() == 0);
    QVERIFY(area1.presentationAreaId() == 0);

    PresentationArea area2(1, 2);
    QVERIFY(area2.childIdentifier() == 1);
    QVERIFY(area2.presentationAreaId() == 2);

    PresentationArea area3(area2);
    QVERIFY(area3.childIdentifier() == 1);
    QVERIFY(area3.presentationAreaId() == 2);
}


void TestPresentationArea::testAccessors() {
    PresentationArea area(1, 2);
    QVERIFY(area.childIdentifier() == 1);
    QVERIFY(area.presentationAreaId() == 2);

    area.setChildIdentifier(4);
    QVERIFY(area.childIdentifier() == 4);

    area.setPresentationAreaId(5);
    QVERIFY(area.presentationAreaId() == 5);
}


void TestPresentationArea::testAssignmentOperator() {
    PresentationArea area1(1, 2);
    QVERIFY(area1.childIdentifier() == 1);
    QVERIFY(area1.presentationAreaId() == 2);

    PresentationArea area2(3, 4);
    QVERIFY(area2.childIdentifier() == 3);
    QVERIFY(area2.presentationAreaId() == 4);

    area1 = area2;
    QVERIFY(area1.childIdentifier() == 3);
    QVERIFY(area1.presentationAreaId() == 4);

    area2.setChildIdentifier(5);
    area2.setPresentationAreaId(6);

    QVERIFY(area1.childIdentifier() == 3);
    QVERIFY(area1.presentationAreaId() == 4);

    QVERIFY(area2.childIdentifier() == 5);
    QVERIFY(area2.presentationAreaId() == 6);
}
