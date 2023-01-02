/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ApplicationStatusBar class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef APPLICATION_STATUS_BAR_H
#define APPLICATION_STATUS_BAR_H

#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QString>
#include <QTimer>

#include <eqt_progress_bar.h>

#include "app_common.h"
#include "placement_status_notifier.h"

namespace EQt {
    class Builder;
}

/**
 * Status bar widget you can use to display status during compilation, running, and placement operations.  The widget
 * will render as a progress bar with optional status text.  The status bar will also provide feedback during time
 * consuming placement operations.
 */
class APP_PUBLIC_API ApplicationStatusBar:public QStatusBar, public PlacementStatusNotifier::Receiver {
    Q_OBJECT

    public:
        /**
         * The delay before displaying the rendering progress.
         */
        static const unsigned renderAutoDisplayDelayMilliseconds = 2000;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        ApplicationStatusBar(QWidget* parent = Q_NULLPTR);

        ~ApplicationStatusBar() override;

    public slots:
        /**
         * Slot you can trigger to specify the number of reported build phases.
         *
         * \param[in] numberBuildPhases The number of build phases.
         */
        void setNumberBuildPhases(unsigned numberBuildPhases);

        /**
         * Slot you can trigger to update the current build status.  Calling this method will cause the status bar to
         * display build status.
         *
         * \param[in] buildPhaseNumber The zero based build phase number.
         */
        void setBuildPhase(unsigned buildPhaseNumber);

        /**
         * Slot you can trigger to update the current build status to show a build warning.  Calling this method will
         * cause the status bar to display 100% build status with an error message.
         *
         * Note that this call will be ignored if a build error is currently being presented.
         *
         * \param[in] warningText The warning text.
         */
        void setBuildWarning(const QString& warningText);

        /**
         * Slot you can trigger to update the current build status to show a build error.  Calling this method will
         * cause the status bar to display 100% build status with an error message.
         *
         * \param[in] errorText The error message.
         */
        void setBuildError(const QString& errorText);

        /**
         * Slot you can trigger to specify that we are now running the model
         */
        void setRunning();

        /**
         * Slot you can trigger to specify that we are now paused the model
         */
        void setPaused();

        /**
         * Slot you can trigger to specify that we have now aborted the model.
         */
        void setAborted();

        /**
         * Slot you can trigger to specify run-time error text.  Calling this method will cause the status bar to
         * display execution status.
         *
         * \param[in] newErrorText The new error text to be displayed.
         */
        void setRuntimeError(const QString& newErrorText);

        /**
         * Slot you can trigger to indicate that the build or run operation has completed.  This will trigger an
         * auto-hide timer.
         */
        void operationCompleted();

        /**
         * Slot you can trigger to indicate that the currently displayed rendering context has changed.
         */
        void renderingContextChanged();

    protected:
        /**
         * Method you should overload to receive the current placement status.
         *
         * \param[in] completedJobs           The new number of completed jobs.
         *
         * \param[in] totalNumberJobs         The total number of jobs.
         *
         * \param[in] elapsedTimeMilliSeconds The elapsed time for the placement, in mSec.
         */
        void statusChanged(
            unsigned long      completedJobs,
            unsigned long      totalNumberJobs,
            unsigned long long elapsedTimeMilliSeconds
        ) final;

        /**
         * Method you should overload to receive updates to the number of pending jobs.
         *
         * \param[in] newNumberPendingJobs The new number of pending jobs.
         */
        void pendingJobsChanged(unsigned long newNumberPendingJobs) final;

        /**
         * Method you should overload to receive notification when a placement operation has started.
         *
         * \param[in] initialNumberJobs The starting number of pending jobs.
         */
        void placementStarted(unsigned long initialNumberJobs) final;

        /**
         * Method you should overload to receive notification when a placement operation has completed.
         */
        void placementCompleted() final;

        /**
         * Method you should overload to receive notification that the placement operation was aborted.
         */
        void placementAborted() final;

        /**
         * Method you should overload to receive notification that an existing abort condition has been
         * cleared.
         */
        void abortCleared() final;

    private slots:
        /**
         * Slot that is triggered when the auto-hide timer times out.
         */
        void autoHide();

    private:
        /**
         * The delay to auto-hide the status bar.
         */
        static constexpr unsigned autoHideDelayMilliseconds = 2000;

        /**
         * The progress bar color used for success.
         */
        static const QColor progressBarRenderingColor;

        /**
         * The progress bar color used for success.
         */
        static const QColor progressBarSuccessColor;

        /**
         * The progress bar success text color.
         */
        static const QColor progressBarSuccessTextColor;

        /**
         * The progress bar color used to indicate a warning condition.
         */
        static const QColor progressBarWarningColor;

        /**
         * The progress bar color used for text to indicate a warning condition.
         */
        static const QColor progressBarWarningTextColor;

        /**
         * The progress bar color used to indicate an error condition.
         */
        static const QColor progressBarErrorColor;

        /**
         * The progress bar color used for text to indicate an error condition.
         */
        static const QColor progressBarErrorTextColor;

        /**
         * The progress bar color used while running.
         */
        static const QColor progressBarRunningColor;

        /**
         * The progress bar color used while paused.
         */
        static const QColor progressBarPausedColor;

        /**
         * The progress bar color used when aborted.
         */
        static const QColor progressBarAbortedColor;

        /**
         * Enumeration indicating the current operating state.
         */
        enum class Mode {
            /**
             * Indicates that we're idle.
             */
            IDLE,

            /**
             * Indicates we're displaying build status.
             */
            BUILDING,

            /**
             * Indicates that we're displaying a build warning.
             */
            BUILD_WARNING,

            /**
             * Indicates that we're displaying a build error.
             */
            BUILD_ERROR,

            /**
             * Indicates that we're displaying run status.
             */
            RUNNING,

            /**
             * Indicates that we've paused.
             */
            PAUSED,

            /**
             * Indicates that the model has been aborted.
             */
            ABORTED,

            /**
             * Indicates that we're displaying a run-time error.
             */
            RUNTIME_ERROR
        };

        /**
         * Method that is called to update the current mode.
         *
         * \param[in] newMode The new display mode.
         */
        void setMode(Mode newMode);

        /**
         * Method that is called to update the displayed controls.
         *
         * \param[in] mode The new display mode.
         */
        void configureDisplay(Mode mode);

        /**
         * The current mode.
         */
        Mode currentMode;

        /**
         * The current build phase.
         */
        unsigned currentBuildPhase;

        /**
         * The current message.
         */
        QString currentMessage;

        /**
         * The reported number of build phases.
         */
        unsigned currentNumberBuildPhases;

        /**
         * A timer used to auto-hide the status bar.
         */
        QTimer* autoHideTimer;

        /**
         * The underlying progress bar.
         */
        EQt::ProgressBar* buildProgressBar;

        /**
         * The run status progress bar.
         */
        QProgressBar* runStatusProgressBar;

        /**
         * Label used to display run status.
         */
        QLabel* statusLabel;

        /**
         * Flag indicating if a placement is in progress.
         */
        bool displayingPlacementStatus;

        /**
         * The last reported completed placement job.
         */
        unsigned long lastCompletedJob;
};

#endif
