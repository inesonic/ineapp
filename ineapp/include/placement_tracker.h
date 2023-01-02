/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlacementTracker class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEMENT_TRACKER_H
#define PLACEMENT_TRACKER_H

#include "app_common.h"

namespace EQt {
    class Builder;
}

/**
 * Pure virtual base class you can overload to provide placement process tracking and feedback during widget placement.  The
 * class presents the concept of "jobs" that must be performed. The rendering engine can add a specified number of
 * jobs and can report when one or more jobs are completed.
 *
 * This class also includes method you can use to force the placement operation to be "aborted" or terminated early.
 * This hook is provided to allow faster response during time consuming placement operations.
 */
class APP_PUBLIC_API PlacementTracker {
    public:
        PlacementTracker();

        virtual ~PlacementTracker();

        /**
         * Pure virtual method you can call to add new work to the placement reporter's tracking engine.
         *
         * \param[in] numberNewJobs The number of additional jobs to be added to the workload.
         */
        virtual void addNewJobs(unsigned long numberNewJobs) = 0;

        /**
         * Pure virtual method you can call to indicate that one or more jobs have been completed.
         *
         * \param[in] numberCompletedJobs The number of jobs that were just completed.
         */
        virtual void completedJobs(unsigned long numberCompletedJobs) = 0;

        /**
         * Convenience method you can call to indicate that a single job was completed.
         */
        inline void completedJob() {
            completedJobs(1);;
        }

        /**
         * Method you can call to determine if the placement operation should be aborted.
         *
         * \return Returns true if the placement operation should be aborted.  Returns false if the placement operation
         *         should not be aborted.
         */
        inline bool abortPlacement() const {
            return currentAbortPlacement;
        }

        /**
         * Method you can call to abort the placement operation.
         */
        virtual void requestPlacementAborted();

        /**
         * Method you can call to clear the abort condition.
         */
        virtual void clearAbort();

    private:
        /**
         * Flag that is used to indicate if the placement operation should be aborted.
         */
        bool currentAbortPlacement;
};

#endif
