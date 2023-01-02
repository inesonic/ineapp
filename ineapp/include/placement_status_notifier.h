/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlacementStatusNotifier class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLACEMENT_STATUS_NOTIFIER_H
#define PLACEMENT_STATUS_NOTIFIER_H

#include <QObject>
#include <QSet>
#include <QElapsedTimer>

#include "app_common.h"
#include "placement_tracker.h"

namespace EQt {
    class Builder;
}

/**
 * Class you can use to track status of a placement operation.  The class provides the ability to maintain and update
 * multiple GUI elements based on the current placement status.
 */
class APP_PUBLIC_API PlacementStatusNotifier:public QObject, public PlacementTracker {
    Q_OBJECT

    public:
        /**
         * Class you can overload to receive placement status information.  You can use this class to both receive
         * placement status and to request that placement status be short circuited.
         */
        class APP_PUBLIC_API Receiver {
            friend class PlacementStatusNotifier;

            public:
                /**
                 * Constructor
                 *
                 * \param[in] notifier The notifier that this receiver should be bound to.  A null pointer will defer
                 *                     binding.  Note that the notifier will *not* take ownership of the reciever.
                 */
                Receiver(PlacementStatusNotifier* notifier = Q_NULLPTR);

                virtual ~Receiver();

                /**
                 * Method you should overload to receive the current placement status.
                 *
                 * \param[in] completedJobs           The new number of completed jobs.
                 *
                 * \param[in] totalNumberJobs         The total number of jobs.
                 *
                 * \param[in] elapsedTimeMilliSeconds The elapsed time for the placement, in mSec.
                 */
                virtual void statusChanged(
                    unsigned long      completedJobs,
                    unsigned long      totalNumberJobs,
                    unsigned long long elapsedTimeMilliSeconds
                );

                /**
                 * Method you should overload to receive updates to the number of pending jobs.
                 *
                 * \param[in] newNumberPendingJobs The new number of pending jobs.
                 */
                virtual void pendingJobsChanged(unsigned long newNumberPendingJobs);

                /**
                 * Method you should overload to receive notification when a placement operation has started.
                 *
                 * \param[in] initialNumberJobs The starting number of pending jobs.
                 */
                virtual void placementStarted(unsigned long initialNumberJobs);

                /**
                 * Method you should overload to receive notification when a placement operation has completed.
                 */
                virtual void placementCompleted();

                /**
                 * Method you should overload to receive notification that the placement operation was aborted.
                 */
                virtual void placementAborted();

                /**
                 * Method you should overload to receive notification that an existing abort condition has been
                 * cleared.
                 */
                virtual void abortCleared();

            protected:
                /**
                 * Method you can call to determine if the placement was aborted.
                 *
                 * \return Returns true if the placement is short circuited.  Returns false if the placement is not
                 *         short-circuited.
                 */
                bool isPlacementAborted() const;

            private:
                /**
                 * The placement status notifier that is interacting with this receiver.
                 */
                PlacementStatusNotifier* currentPlacementStatusNotifier;
        };

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PlacementStatusNotifier(QObject* parent = Q_NULLPTR);

        ~PlacementStatusNotifier() override;

        /**
         * Method you can call to report that a new placement operation has been started.  This method also clears any
         * pending abort condition.
         *
         * \param[in] initialNumberJobs The initial number of starting jobs.
         */
        void placementStarted(unsigned long initialNumberJobs);

        /**
         * Method you can call to report that a placement operation has completed.  This method also clears any
         * pending abort condition.
         */
        void placementCompleted();

        /**
         * Method you can call to add new work to the placement reporter's tracking engine.
         *
         * \param[in] numberNewJobs The number of additional jobs to be added to the workload.
         */
        void addNewJobs(unsigned long numberNewJobs) override;

        /**
         * Method you can call to indicate that one or more jobs have been completed.
         *
         * \param[in] numberCompletedJobs The number of jobs that were just completed.
         */
        void completedJobs(unsigned long numberCompletedJobs) override;

        /**
         * Method you can call to abort the placement operation.
         */
        void requestPlacementAborted() override;

        /**
         * Method you can call to clear the abort condition.
         */
        void clearAbort() override;

        /**
         * Method you can call to add a receiver to this notifier.
         *
         * \param[in] newReceiver The receiver to receive notifications from this notifier.
         */
        void addReceiver(Receiver* newReceiver);

        /**
         * Method you can call to remove a receiver from this notifier.
         *
         * \param[in] receiver The receiver to disconnect from this notifier.
         */
        void removeReceiver(Receiver* receiver);

    private:
        /**
         * The message update interval, in mSec.
         */
        static constexpr unsigned long updateMessageIntervalMilliseconds = 200;

        /**
         * The event loop polling interval, mSec.
         */
        static constexpr unsigned long pollIntervalMilliseconds = 5;

        /**
         * The placement elapsed timer.
         */
        QElapsedTimer timer;

        /**
         * The next event time.
         */
        unsigned long long nextElapsedTime;

        /**
         * The next polling time.
         */
        unsigned long long nextPollTime;

        /**
         * The current total number of jobs.
         */
        unsigned long currentTotalNumberJobs;

        /**
         * The current number of completed jobs.
         */
        unsigned long currentNumberCompletedJobs;

        /**
         * The set of outstanding receivers.
         */
        QSet<Receiver*> currentReceivers;
};

#endif
