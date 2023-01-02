/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref UpdatePageFormatCommand class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QMap>
#include <QList>

#include <ld_page_format.h>
#include <ld_root_element.h>
#include <ld_cursor.h>
#include <ld_cursor_collection.h>
#include <ld_cursor_state_collection.h>

#include "command.h"
#include "update_page_format_command.h"

UpdatePageFormatCommand::UpdatePageFormatCommand() {}


UpdatePageFormatCommand::UpdatePageFormatCommand(QSharedPointer<Ld::RootElement> rootElement) {
    currentRootElement = rootElement.toWeakRef();
}


UpdatePageFormatCommand::UpdatePageFormatCommand(
        QSharedPointer<Ld::RootElement> rootElement,
        const PageFormatsByPageIndex&   pageFormatsByPageIndex
    ) {
    currentRootElement = rootElement.toWeakRef();
    currentPageFormats = pageFormatsByPageIndex;
}


UpdatePageFormatCommand::~UpdatePageFormatCommand() {}


Command::CommandType UpdatePageFormatCommand::commandType() const {
    return CommandType::UPDATE_PAGE_FORMAT;
}


void UpdatePageFormatCommand::setRootElement(QSharedPointer<Ld::RootElement> newRootElement) {
    currentRootElement = newRootElement.toWeakRef();
}


QSharedPointer<Ld::RootElement> UpdatePageFormatCommand::rootElement() const {
    return currentRootElement.toStrongRef();
}


void UpdatePageFormatCommand::setPageFormatsByPageIndex(const PageFormatsByPageIndex& newPageFormatsByPageIndex) {
    currentPageFormats = newPageFormatsByPageIndex;
}


const UpdatePageFormatCommand::PageFormatsByPageIndex& UpdatePageFormatCommand::pageFormatsByPageIndex() const {
    return currentPageFormats;
}


bool UpdatePageFormatCommand::execute(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;

    deletedPageFormats.clear();

    QSharedPointer<Ld::RootElement> rootElement = currentRootElement.toStrongRef();
    if (!rootElement.isNull()) {
        for (  PageFormatsByPageIndex::const_iterator pos = currentPageFormats.constBegin(),
                                                      end = currentPageFormats.constEnd()
             ; pos != end
             ; ++pos
            ) {
            unsigned long                  pageIndex     = pos.key();
            QSharedPointer<Ld::PageFormat> newPageFormat = pos.value();

            QSharedPointer<Ld::PageFormat> oldPageFormat = rootElement->pageFormat(pageIndex);
            rootElement->setPageFormat(pageIndex, newPageFormat);

            deletedPageFormats.insert(pageIndex, oldPageFormat);
        }

        cursorStateCollection->updateCursorState(false);
        success = true;
    } else {
        success = false;
    }

    return success;
}


bool UpdatePageFormatCommand::undo(Ld::CursorStateCollection* cursorStateCollection) {
    bool success;

    QSharedPointer<Ld::RootElement> rootElement = currentRootElement.toStrongRef();
    if (!rootElement.isNull()) {
        for (  PageFormatsByPageIndex::const_iterator pos = deletedPageFormats.constBegin(),
                                                      end = deletedPageFormats.constEnd()
             ; pos != end
             ; ++pos
            ) {
            unsigned long                  pageIndex     = pos.key();
            QSharedPointer<Ld::PageFormat> oldPageFormat = pos.value();

            rootElement->setPageFormat(pageIndex, oldPageFormat);
        }

        cursorStateCollection->updateCursorState(false);
        success = true;
    } else {
        success = false;
    }

    return success;
}


QString UpdatePageFormatCommand::description() const {
    QString result;

    QString              pageList;
    QList<unsigned long> pageIndexes = currentPageFormats.keys();
    unsigned long        numberPages = static_cast<unsigned>(pageIndexes.size());

    if (numberPages == 1) {
        result = tr("Update page %1 format").arg(pageIndexes.first() + 1);
    } else if (numberPages == 2) {
        result = tr("Update format of pages %1 and %2").arg(pageIndexes[0] + 1).arg(pageIndexes[1] + 1);
    } else {
        for (unsigned index=0 ; index<numberPages ; ++index) {
            unsigned long pageNumber = pageIndexes[index] + 1;
            if (index == 0) {
                pageList = QString::number(pageNumber);
            } else if (index < numberPages - 1) {
                pageList = tr("%1, %2").arg(pageList).arg(pageNumber);
            } else {
                pageList = tr("%1, and %2").arg(pageList).arg(pageNumber);
            }
        }

        result = tr("Update format of pages %1").arg(pageList);
    }

    return result;
}
