/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref CommandContainer class.
***********************************************************************************************************************/

#ifndef TEST_COMMAND_CONTAINER_H
#define TEST_COMMAND_CONTAINER_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>

class TestCommandContainer:public QObject {
    Q_OBJECT

    public:
        TestCommandContainer();

        ~TestCommandContainer() override;

    private slots:
        void initTestCase();
        void testConstructorsAndDestructors();
        void testAccessors();
        void testPointerReferenceOperator();
        void testAssignmentOperators();
};

#endif
