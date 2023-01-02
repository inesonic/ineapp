/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PrintingEngine class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PRINTING_ENGINE_H
#define PRINTING_ENGINE_H

#include <QString>
#include <QThread>
#include <QList>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QSizeF>

#include "app_common.h"

class QObject;
class QPrinter;
class QPainter;

namespace Ld {
    class RootElement;
};

/**
 * Class you can use to perform printing operations in a background thread.  Note that this class does not take
 * ownership of any object and assumes objects remain valid and invariant during an entire printing operation.
 */
class APP_PUBLIC_API PrintingEngine:public QThread {
    Q_OBJECT

    public:
        /**
         * Enumeration of printer completion status values.
         */
        enum class Status {
            /**
             * Indicates the printing engine is not running and no error was detected.
             */
            IDLE,

            /**
             * Indicates the printing engine is active.
             */
            ACTIVE,

            /**
             * Indicates the printing engine is not running an an error was reported.
             */
            FAULT,

            /**
             * Indicates the printing engine is not running and was previously aborted.
             */
            ABORTED
        };

        /**
         * Type used to represent a list of pages to be printed.
         */
        typedef QList<unsigned> PageList;

        /**
         * Constructor.
         *
         * \param[in] parent Pointer to the parent object.
         */
        PrintingEngine(QObject* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] printer     A pointer to the printer object to be used by this class.
         *
         * \param[in] rootElement The root element of the program to be printed.
         *
         * \param[in] parent      Pointer to the parent object.
         */
        PrintingEngine(QPrinter* printer, QSharedPointer<Ld::RootElement> rootElement, QObject* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] printer     A pointer to the printer object to be used by this class.
         *
         * \param[in] rootElement The root element of the program to be printed.
         *
         * \param[in] pageList    The list of pages to be printed.
         *
         * \param[in] parent      Pointer to the parent object.
         */
        PrintingEngine(
            QPrinter*                       printer,
            QSharedPointer<Ld::RootElement> rootElement,
            const PageList&                 pageList,
            QObject*                        parent = Q_NULLPTR
        );

        ~PrintingEngine() override;

        /**
         * Method you can use to determine if the printing status.
         *
         * \return Returns the current status of the engine.
         */
        Status status() const;

        /**
         * Method you can use to obtain a pointer to the printer assigned to this print engine.
         *
         * \return Returns a pointer to the printer being used by this engine.
         */
        QPrinter* printer() const;

        /**
         * Method you can use to set the printer to be used by this print engine on the next run.
         *
         * \param[in] printer A pointer to the printer to be used by this engine.
         */
        void setPrinter(QPrinter* printer);

        /**
         * Method you can use to obtain a shared pointer to the root element being used by this printing engine.
         *
         * Note that the pointer is tracked internally as a weak pointer.  You do not need to clear the pointer to the
         * root element in order to destroy the root.  For this reason, this method may return a null pointer.
         *
         * \return Returns a shared pointer to the root element to be printed.
         */
        QSharedPointer<Ld::RootElement> rootElement() const;

        /**
         * Method you can use to set the root element being used by the printing engine.
         *
         * Note that the pointer is tracked internally as a weak pointer.  You do not need to clear the pointer to the
         * root element in order to destroy the root.
         *
         * \param[in] newRootElement The new root element to be printed.
         */
        void setRootElement(QSharedPointer<Ld::RootElement> newRootElement);

        /**
         * Method you can use to obtain the print engine's expection for the current page number.
         *
         * \return Returns the one-based page number of the current page.
         */
        unsigned currentPageNumber() const;

        /**
         * Method you can use to set the page number of the page the print engine should use as the current page.
         *
         * \param[in] newCurrentPageNumber The new one-based current page number.
         */
        void setCurrentPageNumber(unsigned newCurrentPageNumber);

        /**
         * Method you can use to obtain the current list of pages to be printed.  If the list is empty, then the
         * printing engine will determine the page list directly from the supplied printer object.
         *
         * \return Returns the current page list.
         */
        PageList pageList() const;

        /**
         * Method you can use to set the page list which indicates which overrides the QPrinter object to determine
         * which pages should be printed.
         *
         * \param[in] newPageList The new list of pages to be printed.
         */
        void setPageList(const PageList& newPageList);

        /**
         * Method you can use to clear the internal page list.  Clearing the page list will cause the printing engine
         * to print the pages indicated in the supplied printer object.
         */
        void clearPageList();

        /**
         * Method you can call to determine if this printing engine is active.
         *
         * \return Returns true if the printing engine is active.  Returns false if the printing engine is idle.
         */
        bool isActive() const;

        /**
         * Method you can call to determine if this printing engine is idle.
         *
         * \return Returns true if the printing engine is idle.  Returns false if the printing engine is active.
         */
        bool isIdle() const;

        /**
         * Method you can call to wait until the printing operation completes.
         */
        void waitComplete();

        /**
         * Method you can call to wait until the printing operation completes.
         *
         * \param[in] maximumTimeInMilliseconds The maximum time wait, in milliseconds.
         *
         * \return Returns true on success, returns false if the printing operation timed out.
         */
        bool waitComplete(unsigned long maximumTimeInMilliseconds);

    signals:
        /**
         * Signal that is emitted when the printing engine starts.
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
         * Signal that is emitted when we have finished spooling a page to the printer.
         *
         * \param[out] pageOrderIndex A zero based index used to count pages.  This value will start at 0 and increase
         *                            by one for each printed page.  You can use this value to indicate percentage
         *                            complete or other similar useful information.
         *
         * \param[out] pageNumber     The one-based page number of the page we just finished printing.
         */
        void completedPage(unsigned pageOrderIndex, unsigned pageNumber);

        /**
         * Signal that is emitted when we have completed the entire printing operation.
         *
         * \param[out] success Flag that indicates true on success, false if an error occured.
         */
        void completed(bool success);

        /**
         * Signal that is emitted when the print operation has aborted.
         *
         * \param[out] success Flag that indicates true if the abort was successful.  False if the abort failed.
         */
        void aborted(bool success);

        /**
         * Signal that is emitted when an error occurs.
         *
         * \param[out] errorText A textual description of the error provided by the printing engine.
         */
        void errorDetected(const QString& errorText);

    public slots:
        /**
         * Slot you can trigger to start the printing operation.
         */
        void start();

        /**
         * Slot you can trigger to stop the printing operation.
         */
        void abort();

    private:
        /**
         * Value indicating the allowed error in page sizes before page scaling is applied.
         */
        static const QSizeF maximumNoScalingPageSizeDifference;

        /**
         * Method that contains the printer's background thread.
         */
        void run() final;

        /**
         * Method that is called to print the contents of a single page.
         *
         * \param[in] printer        A pointer to the printer object to be used by this class.
         *
         * \param[in] painter        A pointer to the painter we should paint on.
         *
         * \param[in] rootElement    The root element of the program to be printed.
         *
         * \param[in] pageIndex      The zero based page index.
         *
         * \return Returns a string indicating the print error that occurred.  An empty string indicates no error.
         */
        QString printPage(
            QPrinter*                       printer,
            QPainter*&                      painter,
            QSharedPointer<Ld::RootElement> rootElement,
            unsigned                        pageIndex
        );

        /**
         * The currently selected printer.
         */
        QPrinter* currentPrinter;

        /**
         * Weak pointer to the current root element.
         */
        QWeakPointer<Ld::RootElement> currentRoot;

        /**
         * The current page number.
         */
        unsigned currentPage;

        /**
         * Current list of pages to be printed.
         */
        PageList currentPageList;

        /**
         * Flag that is used to signal that an abort has been requested.
         */
        volatile bool abortRequested;

        /**
         * The current printer status.
         */
        volatile Status currentStatus;
};

#endif
