/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ApplicationStatusBar class.
***********************************************************************************************************************/

#include <QWidget>
#include <QColor>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>

#include <eqt_progress_bar.h>

#include "placement_status_notifier.h"
#include "application_status_bar.h"

const QColor ApplicationStatusBar::progressBarRenderingColor(Qt::darkGreen);
const QColor ApplicationStatusBar::progressBarSuccessColor(Qt::darkGreen);
const QColor ApplicationStatusBar::progressBarSuccessTextColor(Qt::white);
const QColor ApplicationStatusBar::progressBarWarningColor(Qt::darkYellow);
const QColor ApplicationStatusBar::progressBarWarningTextColor(Qt::white);
const QColor ApplicationStatusBar::progressBarErrorColor(Qt::darkRed);
const QColor ApplicationStatusBar::progressBarErrorTextColor(Qt::white);
const QColor ApplicationStatusBar::progressBarRunningColor;
const QColor ApplicationStatusBar::progressBarPausedColor(Qt::darkYellow);
const QColor ApplicationStatusBar::progressBarAbortedColor(Qt::darkYellow);

ApplicationStatusBar::ApplicationStatusBar(QWidget* parent):QStatusBar(parent) {
    QWidget* barContainerWidget = new QWidget;

    QHBoxLayout* barContainerLayout = new QHBoxLayout;
    barContainerLayout->setContentsMargins(0, 0, 0, 0);
    barContainerWidget->setLayout(barContainerLayout);

    buildProgressBar = new EQt::ProgressBar;
    barContainerLayout->addWidget(buildProgressBar);

    runStatusProgressBar = new QProgressBar;
    runStatusProgressBar->setMinimum(0);
    runStatusProgressBar->setMaximum(0);
    runStatusProgressBar->setValue(0);
    barContainerLayout->addWidget(runStatusProgressBar);
    insertWidget(0, barContainerWidget, 1);

    statusLabel = new QLabel;
    insertWidget(1, statusLabel, 0);

    autoHideTimer = new QTimer(this);
    autoHideTimer->setSingleShot(true);

    currentMode               = Mode::IDLE;
    currentBuildPhase         = 0;
    currentNumberBuildPhases  = 1;
    displayingPlacementStatus = false;
    lastCompletedJob          = 0;

    connect(autoHideTimer, &QTimer::timeout, this, &ApplicationStatusBar::autoHide);
}


ApplicationStatusBar::~ApplicationStatusBar() {}


void ApplicationStatusBar::setNumberBuildPhases(unsigned numberBuildPhases) {
    currentNumberBuildPhases = numberBuildPhases;

    if (!displayingPlacementStatus && currentMode == Mode::BUILDING) {
        buildProgressBar->setMaximumValue(numberBuildPhases);
    }
}


void ApplicationStatusBar::setBuildPhase(unsigned buildPhaseNumber) {
    if (currentMode != Mode::BUILDING) {
        currentMessage.clear();

        currentBuildPhase = buildPhaseNumber;
        currentMode       = Mode::BUILDING;

        if (!displayingPlacementStatus) {
            configureDisplay(Mode::BUILDING);
        }
    } else {
        currentBuildPhase = buildPhaseNumber;

        if (!displayingPlacementStatus) {
            buildProgressBar->setValue(currentBuildPhase);
        }
    }
}


void ApplicationStatusBar::setBuildWarning(const QString& warningText) {
    if (currentMode != Mode::BUILD_WARNING && currentMode != Mode::BUILD_ERROR) {
        currentMessage = warningText;
        currentMode    = Mode::BUILD_WARNING;

        if (!displayingPlacementStatus) {
            configureDisplay(Mode::BUILD_WARNING);
        }
    }
}


void ApplicationStatusBar::setBuildError(const QString& errorText) {
    currentMessage = errorText;
    currentMode    = Mode::BUILD_ERROR;

    if (!displayingPlacementStatus) {
        configureDisplay(Mode::BUILD_ERROR);
    }
}


void ApplicationStatusBar::setRunning() {
    if (currentMode != Mode::RUNNING) {
        currentMode = Mode::RUNNING;

        if (!displayingPlacementStatus) {
            configureDisplay(Mode::RUNNING);
        }
    }
}


void ApplicationStatusBar::setPaused() {
    if (currentMode != Mode::PAUSED) {
        currentMode = Mode::PAUSED;

        if (!displayingPlacementStatus) {
            configureDisplay(Mode::PAUSED);
        }
    }
}


void ApplicationStatusBar::setAborted() {
    if (currentMode != Mode::ABORTED) {
        currentMode = Mode::ABORTED;

        if (!displayingPlacementStatus) {
            configureDisplay(Mode::ABORTED);
        }
    }
}


void ApplicationStatusBar::setRuntimeError(const QString& errorText) {
    if (currentMode != Mode::RUNTIME_ERROR) {
        currentMessage = errorText;
        currentMode    = Mode::RUNTIME_ERROR;

        if (!displayingPlacementStatus) {
            configureDisplay(Mode::RUNTIME_ERROR);
        }
    }
}


void ApplicationStatusBar::operationCompleted() {
    if (currentMode != Mode::IDLE) {
        currentMode = Mode::IDLE;
    }

    if (!displayingPlacementStatus) {
        configureDisplay(Mode::IDLE);
    }
}


void ApplicationStatusBar::renderingContextChanged() {
    if (displayingPlacementStatus) {
        displayingPlacementStatus = false;
        if (currentMode == Mode::IDLE || autoHideTimer->isActive()) {
            autoHideTimer->stop();
            setVisible(false);
        } else {
            configureDisplay(currentMode);
        }
    }
}


void ApplicationStatusBar::statusChanged(
        unsigned long      completedJobs,
        unsigned long      totalNumberJobs,
        unsigned long long elapsedTimeMilliSeconds
    ) {
    lastCompletedJob = completedJobs;

    if (elapsedTimeMilliSeconds >= renderAutoDisplayDelayMilliseconds) {
        if (!displayingPlacementStatus) {
            autoHideTimer->stop();
            setVisible(true);

            buildProgressBar->show();
            runStatusProgressBar->hide();
            statusLabel->show();

            buildProgressBar->setMinimumValue(0);
            buildProgressBar->setText(QString());

            buildProgressBar->setProgressBarColor(progressBarRenderingColor);
            buildProgressBar->setTextColor(progressBarSuccessTextColor);

            statusLabel->setText(tr("Rendering"));

            displayingPlacementStatus = true;
        }
    }

    if (displayingPlacementStatus) {
        buildProgressBar->setValue(lastCompletedJob);
        buildProgressBar->setMaximumValue(totalNumberJobs);
    }
}


void ApplicationStatusBar::pendingJobsChanged(unsigned long newNumberPendingJobs) {
    if (lastCompletedJob > newNumberPendingJobs) {
        lastCompletedJob = newNumberPendingJobs;
    }

    if (displayingPlacementStatus) {
        buildProgressBar->setMaximumValue(newNumberPendingJobs);
        buildProgressBar->setValue(lastCompletedJob);
    }
}


void ApplicationStatusBar::placementStarted(unsigned long /* initialNumberJobs */) {
    lastCompletedJob = 0;
}


void ApplicationStatusBar::placementCompleted() {
    if (displayingPlacementStatus) {
        buildProgressBar->setValue(1);
        buildProgressBar->setMaximumValue(1);
        displayingPlacementStatus = false;

        configureDisplay(currentMode);
    }
}


void ApplicationStatusBar::placementAborted() {}


void ApplicationStatusBar::abortCleared() {}


void ApplicationStatusBar::autoHide() {
    setVisible(false);
}


void ApplicationStatusBar::setMode(ApplicationStatusBar::Mode newMode) {
    if (!displayingPlacementStatus && newMode != currentMode) {
        configureDisplay(newMode);
    }

    currentMode = newMode;
}


void ApplicationStatusBar::configureDisplay(ApplicationStatusBar::Mode mode) {
    switch (mode) {
        case Mode::IDLE: {
            if (isVisible()) {
                autoHideTimer->start(autoHideDelayMilliseconds);
            }

            break;
        }

        case Mode::BUILDING: {
            autoHideTimer->stop();
            setVisible(true);

            buildProgressBar->show();
            runStatusProgressBar->hide();
            statusLabel->show();

            buildProgressBar->setMinimumValue(0);
            buildProgressBar->setMaximumValue(currentNumberBuildPhases);
            buildProgressBar->setValue(currentBuildPhase);
            buildProgressBar->setText(currentMessage);

            buildProgressBar->setProgressBarColor(progressBarSuccessColor);
            buildProgressBar->setTextColor(progressBarSuccessTextColor);

            statusLabel->setText(tr("Building"));

            break;
        }

        case Mode::BUILD_WARNING: {
            autoHideTimer->stop();
            setVisible(true);

            buildProgressBar->setMinimumValue(0);
            buildProgressBar->setMaximumValue(1);
            buildProgressBar->setValue(1);

            buildProgressBar->setProgressBarColor(progressBarWarningColor);
            buildProgressBar->setTextColor(progressBarWarningTextColor);
            buildProgressBar->setText(currentMessage);

            runStatusProgressBar->hide();

            statusLabel->hide();

            break;
        }

        case Mode::BUILD_ERROR: {
            autoHideTimer->stop();
            setVisible(true);

            buildProgressBar->setMinimumValue(0);
            buildProgressBar->setMaximumValue(1);
            buildProgressBar->setValue(1);

            buildProgressBar->setProgressBarColor(progressBarErrorColor);
            buildProgressBar->setTextColor(progressBarErrorTextColor);
            buildProgressBar->setText(currentMessage);

            runStatusProgressBar->hide();

            statusLabel->hide();

            break;
        }

        case Mode::RUNNING: {
            autoHideTimer->stop();

            buildProgressBar->hide();
            runStatusProgressBar->show();
            statusLabel->show();

            statusLabel->setText(tr("Running"));

            setVisible(true);

            break;
        }

        case Mode::PAUSED: {
            autoHideTimer->stop();

            buildProgressBar->hide();
            runStatusProgressBar->show();

            statusLabel->setText(tr("Paused"));
            statusLabel->show();

            setVisible(true);

            break;
        }

        case Mode::ABORTED: {
            autoHideTimer->start(autoHideDelayMilliseconds);

            buildProgressBar->hide();
            runStatusProgressBar->show();

            statusLabel->setText(tr("Aborted"));
            statusLabel->show();

            setVisible(true);

            break;
        }

        case Mode::RUNTIME_ERROR: {
            autoHideTimer->stop();

            buildProgressBar->setMinimumValue(0);
            buildProgressBar->setMaximumValue(1);
            buildProgressBar->setValue(1);

            buildProgressBar->setProgressBarColor(progressBarErrorColor);
            buildProgressBar->setTextColor(progressBarErrorTextColor);
            buildProgressBar->setText(currentMessage);

            runStatusProgressBar->hide();

            statusLabel->hide();

            setVisible(true);

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}
