/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PresentationLocator class.
***********************************************************************************************************************/

#ifndef TEST_PRESENTATION_LOCATOR_H
#define TEST_PRESENTATION_LOCATOR_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>

#include <presentation_with_fixed_children.h>

class TestPresentationLocator:public QObject {
    Q_OBJECT

    public:
        TestPresentationLocator();

        ~TestPresentationLocator() override;

    private slots:
        void initTestCase();
        void testConstructorsAndDestructors();
        void testAccessors();
        void testAssignmentOperators();
        void testComparisonOperator();
};

#endif
