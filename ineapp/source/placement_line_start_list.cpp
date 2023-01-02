/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlacementLineStartList class.
***********************************************************************************************************************/

#include <QList>

#include "placement_line_start.h"
#include "placement_line_start_list.h"

PlacementLineStartList::PlacementLineStartList() {}


PlacementLineStartList::~PlacementLineStartList() {}


unsigned long PlacementLineStartList::childStartsAtLine(unsigned long childIdentifier) {
    unsigned long result;

    if (size() == 0) {
        result = 0;
    } else {
        unsigned long lastIndex       = size() - 1;
        unsigned long lower           = 0;
        unsigned long lowerIdentifier = first().childIdentifier();
        unsigned long upper           = lastIndex;
        unsigned long upperIdentifier = last().childIdentifier();

        unsigned long resultIdentifier;
        unsigned      numberIterations = 0;

        do {
            if (numberIterations < numberNewtonRaphsonIterations) {
                float inverseSlope = (1.0 * (upper - lower)) / (upperIdentifier - lowerIdentifier);

                result = static_cast<unsigned long>(0.5 + inverseSlope * (childIdentifier - lowerIdentifier) + lower);
                if (result > lastIndex) {
                    result = lastIndex;
                }
            } else {
                result = (upper + lower) / 2;
            }

            ++numberIterations;

            resultIdentifier = at(result).childIdentifier();

            if (resultIdentifier < childIdentifier) {
                lower           = result;
                lowerIdentifier = resultIdentifier;
            } else {
                upper           = result;
                upperIdentifier = resultIdentifier;
            }
        } while (resultIdentifier != childIdentifier && upper - lower > 1);

        // We should come out of the loop above at or very close to the correct answer but we may be one entry off due
        // to rounding or because the child may not actual start on this line.  Back up until we find the correct line.

        while (resultIdentifier > childIdentifier                                            ||
               (resultIdentifier == childIdentifier && at(result).presentationAreaId() != 0)    ) {
            --result;
            resultIdentifier = at(result).childIdentifier();
        }
    }

    return result;
}
