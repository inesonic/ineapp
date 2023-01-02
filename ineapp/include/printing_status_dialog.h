/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PrintingStatusDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRINTING_STATUS_DIALOG_H
#define PRINTING_STATUS_DIALOG_H

#include <QString>
#include <QSharedPointer>
#include <QDialogButtonBox>

#include <cstdint>

#include "app_common.h"
#include "export_status_dialog.h"

class QWidget;
class QTimer;

namespace Ld {
    class RootElement;
}
/**
 * Class that extends the \ref ExportStatusDialog to provide status updates during printing operations.
 */
class APP_PUBLIC_API PrintingStatusDialog:public ExportStatusDialog {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        PrintingStatusDialog(QWidget* parent = Q_NULLPTR);

        ~PrintingStatusDialog() override;

        /**
         * Method you can use to obtain the current normal completion text.
         *
         * \return Returns the normal completion text.
         */
        QString normalCompletionText() const;

        /**
         * Method you can use to set the text to display in the footer area on normal completion.
         *
         * \param[in] newCompletionText The new completion text.  The default completion text is "Done".
         */
        void setNormalCompletionText(const QString& newCompletionText);

        /**
         * Method you can use to determine if this dialog should automatically close on successful completion.
         *
         * \return Returns true if this dialog should automatically close on completion.  Returns false if the dialog
         *         should remain open on completion.
         */
        bool autoCloseWhenDoneEnabled() const;

        /**
         * Method you can use to determine if this dialog should remain visible on successful completion.
         *
         * \return Returns true if this dialog should remain visible on completion.  Returns false if the dialog will
         *         auto-close on successful completion.
         */
        bool autoCloseWhenDoneDisabled() const;

        /**
         * Method you can use to enable auto-close on successful completion.
         *
         * \param[in] nowEnabled If true, auto-close will be enabled.  If false, auto-close will be disabled.
         */
        void setAutoCloseWhenDoneEnabled(bool nowEnabled = true);

        /**
         * Method you can use to disable auto-close on successful completion.
         *
         * \param[in] nowDisabled If true, auto-close will be disabled.  If false, auto-close will be enabled.
         */
        void setAutoCloseWhenDoneDisabled(bool nowDisabled = true);

    signals:
        /**
         * Signal that is emitted when the user requests that the print operation be aborted.
         */
        void abort();

    public slots:
        /**
         * Slot you can trigger when the printing engine starts.
         *
         * \param[out] printerNameOrFile  The name of the printer or file we are printing to.
         *
         * \param[out] printingToFile     Value that holds true if we are printing to a file.  Holds false if we're
         *                                printing to a device or print queue.
         *
         * \param[out] numberPagesToPrint The total number of pages we will be printing.
         */
        void started(const QString& printerNameOrFile, bool printingToFile, unsigned numberPagesToPrint);

        /**
         * Slot you can trigger when we have finished spooling a page to the printer.
         *
         * \param[out] pageOrderIndex A zero based index used to count pages.  This value will start at 0 and increase
         *                            by one for each printed page.  You can use this value to indicate percentage
         *                            complete or other similar useful information.
         *
         * \param[out] pageNumber     The one-based page number of the page we just finished printing.
         */
        void completedPage(unsigned pageOrderIndex, unsigned pageNumber);

        /**
         * Slot you can trigger when we have completed the entire printing operation.
         *
         * \param[out] success Flag that indicates true on success, false if an error occured.
         */
        void completed(bool success);

        /**
         * Slot you can trigger when the print operation has aborted.
         */
        void aborted();

        /**
         * Slot you can trigger when an error occurs.
         *
         * \param[out] errorText A textual description of the error provided by the printing engine.
         */
        void errorDetected(const QString& errorText);

    private slots:
        /**
         * Slot that is triggered when a reject button is clicked.
         */
        void reject() final;

    private:
        /**
         * The current normal completion text.
         */
        QString currentNormalCompletionText;

        /**
         * Flag that indicates if this dialog should auto-close when done.
         */
        bool currentAutoCloseEnabled;
};

#endif
