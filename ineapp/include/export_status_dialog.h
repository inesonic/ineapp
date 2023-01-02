/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ExportStatusDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef EXPORT_STATUS_DIALOG_H
#define EXPORT_STATUS_DIALOG_H

#include <QDialog>
#include <QString>
#include <QDialogButtonBox>

#include "app_common.h"

class QWidget;
class QLabel;
class QProgressBar;
class QTimer;
class QEventLoop;

/**
 * Class that provides a small dialog you can use to show the status and completion of an export operation.
 */
class APP_PUBLIC_API ExportStatusDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        ExportStatusDialog(QWidget* parent = Q_NULLPTR);

        ~ExportStatusDialog() override;

        /**
         * Method you can use to obtain the current minimum value.
         *
         * \return Returns the current minimum value.
         */
        int minimumValue() const;

        /**
         * Method you can use to obtain the current maximum value.
         *
         * \return Returns the current maximum value.
         */
        int maximumValue() const;

        /**
         * Method you can use to obtain the current completion value.
         *
         * \return Returns the current completion value.
         */
        int currentValue() const;

        /**
         * Method you can use to obtain the currently displayed progress bar text.
         *
         * \return Returns the current progress bar text.
         */
        QString progressText() const;

        /**
         * Method you can use to obtain the current progress bar format text.
         *
         * \return Returns the current progress bar format text.
         */
        QString progressFormatText() const;

        /**
         * Method you can use to determine the text being displayed in dialog above the progress bar.
         *
         * \return Returns the text displayed above the progress bar.
         */
        QString leaderText() const;

        /**
         * Method you can use to determine the text being displayed in dialog below the progress bar.
         *
         * \return Returns the text displayed above the progress bar.
         */
        QString footerText() const;

        /**
         * Method you can use to determine what buttons are visible.
         *
         * \return Returns a mask indicating what buttons are currently visible.
         */
        QDialogButtonBox::StandardButtons visibleButtons() const;

    public slots:
        /**
         * Slot you can trigger to set the current minimum value.
         *
         * \param[in] newMinimumValue The newly selected minimum value.
         */
        void setMinimumValue(int newMinimumValue);

        /**
         * Slot you can trigger to set the current maximum value.
         *
         * \param[in] newMaximumValue The newly selected maximum value.
         */
        void setMaximumValue(int newMaximumValue);

        /**
         * Convenience Slot you can trigger to set the range.
         *
         * \param[in] newMinimumValue The newly selected minimum value.
         *
         * \param[in] newMaximumValue The newly selected maximum value.
         */
        void setRange(int newMinimumValue, int newMaximumValue);

        /**
         * Slot you can trigger to set the current completion value.
         *
         * \param[in] newCompletionValue The new completion value.  The value is expected to be between the
         *                               provided minimum and maximum value.  Values outside of this range will be
         *                               capped to land at either the minimum or maximum value.
         */
        void setCurrentValue(int newCompletionValue);

        /**
         * Slot you can trigger to set the current progress bar format text.
         *
         * \param[in] newProgressBarText The new text to display in the progress bar.  Note that "%p" will be replaced
         *                               by a percentage completion, "%v" will be replaced by the current integer
         *                               value, and "%m" will be replaced by the total number of steps.
         */
        void setProgressFormatText(const QString& newProgressBarText);

        /**
         * Slot you can trigger to set the text displayed in the dialog above the progress bar.
         *
         * \param[in] newLeaderText The new leader text to display above the progress bar.
         */
        void setLeaderText(const QString& newLeaderText);

        /**
         * Slot you can trigger to set the text displayed in the dialog below the progress bar.
         *
         * \param[in] newFooterText The new leader text to display below the progress bar.
         */
        void setFooterText(const QString& newFooterText);

        /**
         * Slot you can use to update the displayed standard buttons.
         *
         * \param[in] buttons A mask indicating which buttons should be displayed.
         */
        void setVisibleButtons(QDialogButtonBox::StandardButtons buttons);

    private:
        /**
         * The dialog leader text.
         */
        QLabel* leaderLabel;

        /**
         * The dialog progress bar.
         */
        QProgressBar* progressBar;

        /**
         * The dialog footer text.
         */
        QLabel* footerLabel;

        /**
         * The dialog button box.
         */
        QDialogButtonBox* buttonBox;
};

#endif
