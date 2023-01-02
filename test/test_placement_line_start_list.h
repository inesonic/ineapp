/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header provides tests for the \ref PlacementLineStartList class.
***********************************************************************************************************************/

#ifndef TEST_PLACEMENT_LINE_START_LIST_H
#define TEST_PLACEMENT_LINE_START_LIST_H

#include <QtGlobal>
#include <QObject>
#include <QtTest/QtTest>

#include <presentation_with_fixed_children.h>

class TestPlacementLineStartList:public QObject {
    Q_OBJECT

    public:
        TestPlacementLineStartList();

        ~TestPlacementLineStartList() override;

    private slots:
        void initTestCase();
        void testConstructorsAndDestructors();
        void testChildStartsAtLineMethodBasic();
        void testChildStartsAtLineMethodFuzz();

    private:
        static constexpr unsigned minimumFuzzListSize           = 0;
        static constexpr unsigned maximumFuzzListSize           = 10000;
        static constexpr unsigned numberListIterations          = 100;
        static constexpr unsigned numberSearchIterationsPerList = 200;
        static constexpr double   meanIdentifierDistance        = 1.0;
};

#endif
