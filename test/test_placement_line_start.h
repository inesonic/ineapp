/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PlacementLineStart class.
***********************************************************************************************************************/

#ifndef TEST_PLACEMENT_LINE_START_H
#define TEST_PLACEMENT_LINE_START_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>

#include <presentation_with_fixed_children.h>

class TestPlacementLineStart:public QObject {
    Q_OBJECT

    public:
        TestPlacementLineStart();

        ~TestPlacementLineStart() override;

    private slots:
        void initTestCase();
        void testConstructorsAndDestructors();
        void testAccessors();
        void testAssignmentOperator();
};

#endif
