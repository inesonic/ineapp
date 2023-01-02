/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements tests for the \ref PlacementLineStartList class.
***********************************************************************************************************************/

#include <QDebug>
#include <QObject>
#include <QtTest/QtTest>

#include <random>

#include <placement_line_start.h>
#include <placement_line_start_list.h>

#include "test_placement_line_start_list.h"

TestPlacementLineStartList::TestPlacementLineStartList() {}


TestPlacementLineStartList::~TestPlacementLineStartList() {}


void TestPlacementLineStartList::initTestCase() {}


void TestPlacementLineStartList::testConstructorsAndDestructors() {}


void TestPlacementLineStartList::testChildStartsAtLineMethodBasic() {
    PlacementLineStartList list;

    unsigned long result = list.childStartsAtLine(0);
    QVERIFY(result == 0);

    list << PlacementLineStart( 3, PlacementNegotiator::imposeNoTopSpacing,  0, 0); // 0
    list << PlacementLineStart( 6, PlacementNegotiator::imposeNoTopSpacing,  1, 1); // 1
    list << PlacementLineStart( 7, PlacementNegotiator::imposeNoTopSpacing,  3, 1); // 2
    list << PlacementLineStart( 8, PlacementNegotiator::imposeNoTopSpacing,  4, 0); // 3
    list << PlacementLineStart( 9, PlacementNegotiator::imposeNoTopSpacing,  6, 1); // 4
    list << PlacementLineStart(10, PlacementNegotiator::imposeNoTopSpacing,  7, 2); // 5
    list << PlacementLineStart(12, PlacementNegotiator::imposeNoTopSpacing,  9, 3); // 6
    list << PlacementLineStart(17, PlacementNegotiator::imposeNoTopSpacing, 11, 3); // 7
    list << PlacementLineStart(21, PlacementNegotiator::imposeNoTopSpacing, 15, 0); // 8

    result = list.childStartsAtLine(1);
    QVERIFY(result == 0);

    result = list.childStartsAtLine(4);
    QVERIFY(result == 3);

    result = list.childStartsAtLine(9);
    QVERIFY(result == 5);

    result = list.childStartsAtLine(17);
    QVERIFY(result == 8);
}


void TestPlacementLineStartList::testChildStartsAtLineMethodFuzz() {
    std::mt19937 rng;
    std::uniform_int_distribution<> numberEntriesDistribution(minimumFuzzListSize, maximumFuzzListSize);
    std::exponential_distribution<> pointDistanceDistribution(1.0 / meanIdentifierDistance);

    for (unsigned listIteration=0 ; listIteration<numberListIterations ; ++listIteration) {
        unsigned numberEntries = numberEntriesDistribution(rng);

        unsigned long presentationAreaId = 0;
        unsigned long childIdentifier    = 0;

        PlacementLineStartList list;
        for (unsigned index=0 ; index<numberEntries ; ++index) {
            list << PlacementLineStart(
                index,
                PlacementNegotiator::imposeNoTopSpacing,
                childIdentifier,
                presentationAreaId
            );

            unsigned long nextDelta = static_cast<unsigned long>(pointDistanceDistribution(rng) + 0.5);
            if (nextDelta == 0) {
                presentationAreaId += static_cast<unsigned long>(pointDistanceDistribution(rng) + 1.0);
            } else {
                childIdentifier += nextDelta;
                presentationAreaId = static_cast<unsigned long>(pointDistanceDistribution(rng) + 0.5);
            }
        }

        unsigned long lastChildIdentifier = list.last().childIdentifier() + 2; // +2 to search past the end of list.
        std::uniform_int_distribution<> searchValueDistribution(0, lastChildIdentifier);

        for (unsigned searchIndex=0 ; searchIndex<numberSearchIterationsPerList ; ++searchIndex) {
            unsigned long searchValue = searchValueDistribution(rng);

            unsigned expectedIndex = 0;
            while(expectedIndex < (numberEntries - 1) && list[expectedIndex].childIdentifier() < searchValue) {
                ++expectedIndex;
            }

            while (expectedIndex > 0                                                &&
                   ((list[expectedIndex].childIdentifier() > searchValue)      ||
                    (list[expectedIndex].childIdentifier() == searchValue &&
                     list[expectedIndex].presentationAreaId() != 0           )    )    ) {
                --expectedIndex;
            }

            unsigned long testResult = list.childStartsAtLine(searchValue);
            QCOMPARE(testResult, expectedIndex);
        }
    }
}
