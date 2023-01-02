/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlacementLineStartList class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEMENT_LINE_START_LIST_H
#define PLACEMENT_LINE_START_LIST_H

#include <QList>

#include "app_common.h"
#include "placement_line_start.h"

/**
 * You can use this class to manage a list of \ref PlacementLineStart instances.  The class extends the QList class to
 * include several additional methods to perform sorted inserts, fast searches, and list-wide adjustments.
 */
class APP_PUBLIC_API PlacementLineStartList:public QList<PlacementLineStart> {
    public:
        PlacementLineStartList();

        ~PlacementLineStartList();

        /**
         * Method you can call to determine the line where a child's first presentation area should reside.
         *
         * \param[in] childIdentifier The zero based identifier of the child.
         *
         * \return Returns the line index of the line that should contain the start of the requested child.
         */
        unsigned long childStartsAtLine(unsigned long childIdentifier);

    private:
        /**
         * Value indicating the number of iterations before we transition from Newton-Raphson method to binary search
         * method.
         */
        static constexpr unsigned numberNewtonRaphsonIterations = 5;
};

#endif
