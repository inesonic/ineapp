/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PresentationArea class.
***********************************************************************************************************************/

#ifndef TEST_PRESENTATION_AREA_H
#define TEST_PRESENTATION_AREA_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>

#include <presentation_with_fixed_children.h>

class TestPresentationArea:public QObject {
    Q_OBJECT

    public:
        TestPresentationArea();

        ~TestPresentationArea() override;

    private slots:
        void initTestCase();
        void testConstructorsAndDestructors();
        void testAccessors();
        void testAssignmentOperator();
};

#endif
