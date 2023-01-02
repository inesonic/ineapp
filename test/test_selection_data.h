/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the Ld::SelectionData class.
***********************************************************************************************************************/

#ifndef TEST_SELECTION_DATA_H
#define TEST_SELECTION_DATA_H

#include <QObject>
#include <QtTest/QtTest>

class TestSelectionData:public QObject {
    Q_OBJECT

    public:
        TestSelectionData();

        ~TestSelectionData() override;

    private slots:
        void initTestCase();

        void testConstructors();

        void testPositionAccessors();

        void testListMethods();

        void testAssignmentOperator();

        void testComparisonOperators();
};

#endif
