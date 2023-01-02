/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref UpdatePageFormatCommand class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef UPDATE_PAGE_FORMAT_COMMAND_H
#define UPDATE_PAGE_FORMAT_COMMAND_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QSharedPointer>
#include <QWeakPointer>
#include <QString>
#include <QMap>

#include <ld_root_element.h>

#include "app_common.h"
#include "command.h"

namespace Ld {
    class CursorStateCollection;
}

class CommandContainer;

/**
 * Command that can be issued to update the page format for one or more pages.  Note that we have a distinct command
 * for page formats because of how pages are managed within the root element.
 */
class APP_PUBLIC_API UpdatePageFormatCommand:public Command {
    Q_DECLARE_TR_FUNCTIONS(UpdatePageFormatCommand)

    public:
        /**
         * Type used to represent a map of page formats to be updated.
         */
        typedef QMap<unsigned long, QSharedPointer<Ld::PageFormat>> PageFormatsByPageIndex;

        UpdatePageFormatCommand();

        /**
         * Constructor
         *
         * \param[in] rootElement The root element managing the pages we wish to update.
         */
        UpdatePageFormatCommand(QSharedPointer<Ld::RootElement> rootElement);

        /**
         * Constructor
         *
         * \param[in] rootElement            The root element managing the pages we wish to update.
         *
         * \param[in] pageFormatsByPageIndex A map you can use to represent the page formats the should be updated by
         *                                   this command.
         */
        UpdatePageFormatCommand(
            QSharedPointer<Ld::RootElement> rootElement,
            const PageFormatsByPageIndex&   pageFormatsByPageIndex
        );

        ~UpdatePageFormatCommand() override;

        /**
         * Method that returns the command type.
         *
         * \return Returns the command type value.
         */
        CommandType commandType() const final;

        /**
         * Method you can use to set the root element containing the pages to be updated.
         *
         * \param[in] newRootElement The root element managing the pages we wish to update.
         */
        void setRootElement(QSharedPointer<Ld::RootElement> newRootElement);

        /**
         * Method you can use to obtain the root element containing the pages to be updated.
         *
         * \return Returns a shared pointer to the root element.  Note that a null pointer may be returned in the weird
         *         situation that the root element was deleted.
         */
        QSharedPointer<Ld::RootElement> rootElement() const;

        /**
         * Method you can use to set the map of page whose page formats should be updated.
         *
         * \param[in] newPageFormatsByPageIndex A map listing the page formats to use, keyed by the zero based page
         *                                      index.
         */
        void setPageFormatsByPageIndex(const PageFormatsByPageIndex& newPageFormatsByPageIndex);

        /**
         * Method you can use to obtain a map of page formats keyed by zero based page index.
         *
         * \return Returns a reference to a map of page formats keyed by page index.
         */
        const PageFormatsByPageIndex& pageFormatsByPageIndex() const;

        /**
         * Method that executes the command.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool execute(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that performs an undo of the command using stored information.
         *
         * \param[in,out] cursorStateCollection The collection of every cursor that may need adjustment for this
         *                                      command.
         *
         * \return Returns true on success, returns false on error.
         */
        bool undo(Ld::CursorStateCollection* cursorStateCollection) final;

        /**
         * Method that returns a description of the command suitable for displaying in an undo/redo stack window.
         *
         * \return Returns a description of the command.
         */
        QString description() const final;

    private:
        /**
         * A weak pointer to the root element containing the pages.
         */
        QWeakPointer<Ld::RootElement> currentRootElement;

        /**
         * The current map of page formats by page index.
         */
        PageFormatsByPageIndex currentPageFormats;

        /**
         * A map of the page formats that existed before we replaced them.  The map only contains pages that were
         * actually modified.
         */
        PageFormatsByPageIndex deletedPageFormats;
};

#endif
