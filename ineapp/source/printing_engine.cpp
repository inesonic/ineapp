/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PrintingEngine class.
***********************************************************************************************************************/

#include <QString>
#include <QThread>
#include <QList>
#include <QSharedPointer>
#include <QPrinter>
#include <QSizeF>
#include <QRectF>
#include <QPageSize>
#include <QPageLayout>
#include <QPainter>

#include <QDebug>

#include <algorithm>
#include <cmath>

#include <ld_element_structures.h>
#include <ld_root_element.h>
#include <ld_page_format.h>

#include "root_presentation.h"
#include "printing_engine.h"

const QSizeF PrintingEngine::maximumNoScalingPageSizeDifference(2, 2);

PrintingEngine::PrintingEngine(QObject* parent):QThread(parent) {
    currentPrinter = Q_NULLPTR;
    currentRoot.clear();
    currentPageList.clear();

    abortRequested = false;
    currentStatus  = Status::IDLE;
}


PrintingEngine::PrintingEngine(
        QPrinter*                       printer,
        QSharedPointer<Ld::RootElement> rootElement,
        QObject*                        parent
    ):QThread(
        parent
    ) {
    currentPrinter = printer;
    currentRoot    = rootElement.toWeakRef();
    currentPageList.clear();

    abortRequested = false;
    currentStatus  = Status::IDLE;
}


PrintingEngine::PrintingEngine(
        QPrinter*                       printer,
        QSharedPointer<Ld::RootElement> rootElement,
        const PageList&                 pageList,
        QObject*                        parent
    ):QThread(
        parent
    ) {
    currentPrinter  = printer;
    currentRoot     = rootElement.toWeakRef();
    currentPageList = pageList;

    abortRequested = false;
    currentStatus  = Status::IDLE;
}


PrintingEngine::~PrintingEngine() {}


PrintingEngine::Status PrintingEngine::status() const {
    return currentStatus;
}


QPrinter* PrintingEngine::printer() const {
    return currentPrinter;
}


void PrintingEngine::setPrinter(QPrinter* printer) {
    currentPrinter = printer;
}


QSharedPointer<Ld::RootElement> PrintingEngine::rootElement() const {
    return currentRoot.toStrongRef();
}


void PrintingEngine::setRootElement(QSharedPointer<Ld::RootElement> newRootElement) {
    currentRoot = newRootElement.toWeakRef();
}


unsigned PrintingEngine::currentPageNumber() const {
    return currentPage;
}


void PrintingEngine::setCurrentPageNumber(unsigned int newCurrentPageNumber) {
    currentPage = newCurrentPageNumber;
}


PrintingEngine::PageList PrintingEngine::pageList() const {
    return currentPageList;
}


void PrintingEngine::setPageList(const PrintingEngine::PageList& newPageList) {
    currentPageList = newPageList;
}


void PrintingEngine::clearPageList() {
    currentPageList.clear();
}


bool PrintingEngine::isActive() const {
    return QThread::isRunning();
}


bool PrintingEngine::isIdle() const {
    return !QThread::isRunning();
}


void PrintingEngine::waitComplete() {
    wait();
}


bool PrintingEngine::waitComplete(unsigned long maximumTimeInMilliseconds) {
    return wait(maximumTimeInMilliseconds);
}


void PrintingEngine::start() {
    abortRequested = false;
    currentStatus  = Status::ACTIVE;

    QThread::start();
}


void PrintingEngine::abort() {
    abortRequested = true;
}


void PrintingEngine::run() {
    QPrinter*                       printer  = currentPrinter;
    QSharedPointer<Ld::RootElement> root     = currentRoot;
    PageList                        pageList = currentPageList;

    QString error;

    printer->setFullPage(true);
    printer->setFontEmbeddingEnabled(true);

    if (printer != Q_NULLPTR && !root.isNull()) {
        QString  outputFilename      = printer->outputFileName();
        bool     printingToFile      = !outputFilename.isEmpty();
        QString  printerNameOrFile   = printingToFile ? outputFilename : printer->printerName();
        unsigned numberDocumentPages = root->numberPages();

        if (pageList.isEmpty()) {
            switch (printer->printRange()) {
                case QPrinter::PrintRange::AllPages: {
                    for (unsigned pageNumber=1 ; pageNumber<=numberDocumentPages ; ++pageNumber) {
                        pageList.append(pageNumber);
                    }

                    break;
                }

                case QPrinter::PrintRange::PageRange: {
                    unsigned startingPageNumber = static_cast<unsigned>(printer->fromPage());
                    unsigned endingPageNumber   = static_cast<unsigned>(printer->toPage());

                    for (unsigned pageNumber=startingPageNumber ; pageNumber<=endingPageNumber ; ++pageNumber) {
                        pageList.append(pageNumber);
                    }

                    break;
                }


                case QPrinter::PrintRange::CurrentPage: {
                    pageList.append(currentPage);
                    break;
                }

                case QPrinter::PrintRange::Selection: {
                    error = tr("Invalid print range");
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }
        }

        QPrinter::PageOrder pageOrder = printer->pageOrder();
        if (pageOrder == QPrinter::LastPageFirst) {
            std::reverse(pageList.begin(), pageList.end());
        }

        unsigned pageOrderIndex     = 0;
        unsigned numberPagesToPrint = static_cast<unsigned>(pageList.size());

        while (error.isEmpty() && pageOrderIndex < numberPagesToPrint) {
            unsigned pageNumber = pageList.at(pageOrderIndex);
            if (pageNumber == 0 || pageNumber > numberDocumentPages) {
                error = tr("Invalid page range");
            }

            ++pageOrderIndex;
        }

        QPainter* painter = Q_NULLPTR;

        pageOrderIndex = 0;
        while (!abortRequested && error.isEmpty() && pageOrderIndex < numberPagesToPrint) {
            unsigned pageNumber  = pageList.at(pageOrderIndex);
            unsigned pageIndex   = pageNumber - 1;

            error = printPage(printer, painter, root, pageIndex);

            if (error.isEmpty()) {
                emit completedPage(pageOrderIndex, pageNumber);
                ++pageOrderIndex;
            }
        }

        if (painter != Q_NULLPTR) {
            painter->end();
            delete painter;
        }
    } else {
        error = tr("Printer incorrectly setup");
    }

    if (!error.isEmpty()) {
        emit errorDetected(error);
    }

    if (!abortRequested) {
        bool success = error.isEmpty();

        emit completed(success);
        currentStatus = success ? Status::IDLE : Status::FAULT;
    } else {
        bool abortSuccessful = printer->abort();

        emit aborted(abortSuccessful);
        currentStatus = abortSuccessful ? Status::ABORTED : Status::FAULT;
    }
}


QString PrintingEngine::printPage(
        QPrinter*                       printer,
        QPainter*&                      painter,
        QSharedPointer<Ld::RootElement> rootElement,
        unsigned                        pageIndex
    ) {
    RootPresentation*              rootPresentation = dynamic_cast<RootPresentation*>(rootElement->visual());
    QSharedPointer<Ld::PageFormat> pageFormat       = rootElement->pageFormat(pageIndex);

    QPageSize requiredPaperSize = pageFormat->toQPageLayout().pageSize();

    if (printer->outputFormat() == QPrinter::PdfFormat) {
        bool success = printer->setPageSize(requiredPaperSize);
        Q_ASSERT(success);
    } else {
        QPageLayout pageLayout = printer->pageLayout();

        QPageSize paperSize = pageLayout.pageSize();
        if (!paperSize.isEquivalentTo(requiredPaperSize)) {
            printer->setPageSize(requiredPaperSize);
        }

        #if (defined(Q_OS_WIN) || defined(Q_OS_DARWIN))

        #elif (!defined(Q_OS_LINUX))

            #error Unknown platform

        #endif
    }

    bool portraitPaper         = (printer->pageLayout().orientation() == QPageLayout::Portrait);
    bool requiresPortraitPaper = pageFormat->portrait();

    if (requiresPortraitPaper != portraitPaper) {
        printer->setPageOrientation(requiresPortraitPaper ? QPageLayout::Portrait : QPageLayout::Landscape);
    }

    QRectF pageBoundarySceneUnits = rootPresentation->boundarySceneUnits(pageIndex);

    QRectF paperRectangle = printer->paperRect(QPrinter::DevicePixel);

    if (painter == Q_NULLPTR) {
        painter = new QPainter;
        bool success = painter->begin(printer);
        Q_ASSERT(success);
    } else {
        printer->newPage();
    }

    rootPresentation->render(painter, paperRectangle, pageBoundarySceneUnits, Qt::KeepAspectRatio);

    QString result;
    if (printer->printerState() == QPrinter::Error) {
        result = tr("Printer error");
    }

    return result;
}

