/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PlacementLineData class.
***********************************************************************************************************************/

#ifndef TEST_PLACEMENT_LINE_DATA_H
#define TEST_PLACEMENT_LINE_DATA_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>

class TestPlacementLineData:public QObject {
    Q_OBJECT

    public:
        TestPlacementLineData();

        ~TestPlacementLineData() override;

    private slots:
        void initTestCase();
        void testConstructorsAndDestructors();
        void testAccessors();
        void testAssignmentOperators();
};

#endif
