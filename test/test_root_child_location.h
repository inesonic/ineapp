/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref RootChildLocation class.
***********************************************************************************************************************/

#ifndef TEST_ROOT_CHILD_LOCATION_H
#define TEST_ROOT_CHILD_LOCATION_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>

#include <presentation_with_fixed_children.h>

class TestRootChildLocation:public QObject {
    Q_OBJECT

    public:
        TestRootChildLocation();

        ~TestRootChildLocation() override;

    private slots:
        void initTestCase();
        void testConstructorsAndDestructors();
        void testAccessors();
        void testAssignmentOperator();
        void testComparisonOperators();
};

#endif
