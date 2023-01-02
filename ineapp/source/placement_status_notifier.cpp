/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlacementStatusNotifier class.
***********************************************************************************************************************/

#include <QObject>
#include <QSet>
#include <QElapsedTimer>
#include <QApplication>

#include "placement_tracker.h"
#include "placement_status_notifier.h"

/***********************************************************************************************************************
 * PlacementStatusNotifier::Receiver
 */

PlacementStatusNotifier::Receiver::Receiver(PlacementStatusNotifier* notifier) {
    currentPlacementStatusNotifier = Q_NULLPTR;

    if (notifier != Q_NULLPTR) {
        notifier->addReceiver(this);
    }
}


PlacementStatusNotifier::Receiver::~Receiver() {
    if (currentPlacementStatusNotifier != Q_NULLPTR) {
        currentPlacementStatusNotifier->removeReceiver(this);
    }
}


void PlacementStatusNotifier::Receiver::statusChanged(
        unsigned long      /* completedJobs */,
        unsigned long      /* totalNumberJobs */,
        unsigned long long /* elapsedTimeMilliSeconds */
    ) {}


void PlacementStatusNotifier::Receiver::pendingJobsChanged(unsigned long /* newNumberPendingJobs */) {}


void PlacementStatusNotifier::Receiver::placementStarted(unsigned long /* initialNumberJobs */) {}


void PlacementStatusNotifier::Receiver::placementCompleted() {}


void PlacementStatusNotifier::Receiver::placementAborted() {}


void PlacementStatusNotifier::Receiver::abortCleared() {}


bool PlacementStatusNotifier::Receiver::isPlacementAborted() const {
    return   currentPlacementStatusNotifier != Q_NULLPTR
           ? currentPlacementStatusNotifier->abortPlacement()
           : false;
}

/***********************************************************************************************************************
 * PlacementStatusNotifier
 */

PlacementStatusNotifier::PlacementStatusNotifier(QObject* parent):QObject(parent) {
    currentTotalNumberJobs     = 0;
    currentNumberCompletedJobs = 0;
}


PlacementStatusNotifier::~PlacementStatusNotifier() {
    for (  QSet<Receiver*>::const_iterator it = currentReceivers.constBegin(), end = currentReceivers.constEnd()
         ; it != end
         ; ++it
        ) {
        Receiver* receiver = *it;
        receiver->currentPlacementStatusNotifier = Q_NULLPTR;
    }
}


void PlacementStatusNotifier::placementStarted(unsigned long initialNumberJobs) {
    clearAbort();

    currentTotalNumberJobs     = initialNumberJobs;
    currentNumberCompletedJobs = 0;

    for (  QSet<Receiver*>::const_iterator it = currentReceivers.constBegin(), end = currentReceivers.constEnd()
         ; it != end
         ; ++it
        ) {
        Receiver* receiver = *it;
        receiver->placementStarted(currentTotalNumberJobs);
    }

    timer.start();
    nextElapsedTime = updateMessageIntervalMilliseconds;
    nextPollTime    = pollIntervalMilliseconds;
}


void PlacementStatusNotifier::placementCompleted() {
    for (  QSet<Receiver*>::const_iterator it = currentReceivers.constBegin(), end = currentReceivers.constEnd()
         ; it != end
         ; ++it
        ) {
        Receiver* receiver = *it;
        receiver->placementCompleted();
    }

    clearAbort();
}


void PlacementStatusNotifier::addNewJobs(unsigned long numberNewJobs) {
    currentTotalNumberJobs += numberNewJobs;

    for (  QSet<Receiver*>::const_iterator it = currentReceivers.constBegin(), end = currentReceivers.constEnd()
         ; it != end
         ; ++it
        ) {
        Receiver* receiver = *it;
        receiver->pendingJobsChanged(currentTotalNumberJobs);
    }
}


void PlacementStatusNotifier::completedJobs(unsigned long numberCompletedJobs) {
    currentNumberCompletedJobs += numberCompletedJobs;

    unsigned long long currentElapsedTime = static_cast<unsigned long long>(timer.elapsed());
    if (currentElapsedTime >= nextElapsedTime) {
        for (  QSet<Receiver*>::const_iterator it = currentReceivers.constBegin(), end = currentReceivers.constEnd()
             ; it != end
             ; ++it
            ) {
            Receiver* receiver = *it;
            receiver->statusChanged(currentNumberCompletedJobs, currentTotalNumberJobs, currentElapsedTime);
        }

        nextElapsedTime = (
              currentElapsedTime
            + updateMessageIntervalMilliseconds - (currentElapsedTime % updateMessageIntervalMilliseconds)
        );
    }

    if (currentElapsedTime >= nextPollTime) {
        QApplication::processEvents();
        QApplication::sendPostedEvents();
        nextPollTime = currentElapsedTime + pollIntervalMilliseconds - (currentElapsedTime % pollIntervalMilliseconds);
    }
}


void PlacementStatusNotifier::requestPlacementAborted() {
    if (!abortPlacement()) {
        PlacementTracker::requestPlacementAborted();

        for (  QSet<Receiver*>::const_iterator it = currentReceivers.constBegin(), end = currentReceivers.constEnd()
             ; it != end
             ; ++it
            ) {
            Receiver* receiver = *it;
            receiver->placementAborted();
        }
    }
}


void PlacementStatusNotifier::clearAbort() {
    if (abortPlacement()) {
        PlacementTracker::clearAbort();

        for (  QSet<Receiver*>::const_iterator it = currentReceivers.constBegin(), end = currentReceivers.constEnd()
             ; it != end
             ; ++it
            ) {
            Receiver* receiver = *it;
            receiver->abortCleared();
        }
    }
}


void PlacementStatusNotifier::addReceiver(Receiver* newReceiver) {
    if (newReceiver != Q_NULLPTR && newReceiver->currentPlacementStatusNotifier != this) {
        if (newReceiver->currentPlacementStatusNotifier != Q_NULLPTR) {
            newReceiver->currentPlacementStatusNotifier->removeReceiver(newReceiver);
        }

        newReceiver->currentPlacementStatusNotifier = this;
        currentReceivers.insert(newReceiver);
    }
}


void PlacementStatusNotifier::removeReceiver(Receiver* receiver) {
    if (receiver != Q_NULLPTR) {
        currentReceivers.remove(receiver);
        receiver->currentPlacementStatusNotifier = Q_NULLPTR;
    }
}
