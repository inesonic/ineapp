/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PageFormatViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FORMAT_VIEW_PROXY_H
#define FORMAT_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>

#include <ld_page_format.h>

#include "page_list.h"
#include "update_page_format_command.h"
#include "view_widget.h"
#include "view_proxy.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

namespace Ld {
    class Element;
}

class PageFormatDialog;

/**
 * View proxy used to manage the page formatting functions.  These functions include:
 *
 *     * Launching and managing the page format dialog
 *     * Managing the page format dock widget functions
 */
class PageFormatViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        PageFormatViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~PageFormatViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

    private slots:
        /**
         * Slot that is triggered whenever the list of pages under the cursor changes.
         *
         * \param[in] pagesUnderCursor A list of page index values indicating the pages that are now under the cursor.
         */
        void pagesUnderCursorChanged(const QList<PageList::Index>& pagesUnderCursor);

        /**
         * Slot that is triggered whenever the user selects portrait page orientation.
         */
        void pagePortraitOrientationSelected();

        /**
         * Slot that is triggered whenever the user selects landscape page orientation.
         */
        void pageLandscapeOrientationSelected();

        /**
         * Slot that is triggered when the user selects a new page size.
         */
        void pageSizeMenuOptionSelected();

        /**
         * Slot that is triggered when the user requests a page format dialog.
         */
        void pageFormatDialogRequested();

        /**
         * Slot that is triggered when the page's left margin is changed.
         */
        void pageLeftMarginChanged();

        /**
         * Slot that is triggered when the page's right margin is changed.
         */
        void pageRightMarginChanged();

        /**
         * Slot that is triggered when the page's top margin is changed.
         */
        void pageTopMarginChanged();

        /**
         * Slot that is triggered when the page's bottom margin is changed.
         */
        void pageBottomMarginChanged();

        /**
         * Slot that is triggered when the page format dialog Apply button is pressed or when auto-update is enabled
         * and a change occurs within the dialog.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void pageFormatDialogApplyUpdates(PageFormatDialog* dialog);

    private:
        /**
         * Method you can use to update the page orientation button and menu state.
         *
         * \param[in] newPortraitOrientationState Value indicating if the button should show the page in portrait
         *                                        orientation, landscape orientation, or both.
         */
        void setPageOrientationButtonAndMenuState(const Ld::Format::BooleanAggregation& newPortraitOrientationState);

        /**
         * Method that updates the page orientation.
         *
         * \param[in] nowPortrait If true, the page orientation will be set to portrait.  If false, the page
         *                        orientation will be set to landscape.
         */
        void setPageOrientation(bool nowPortrait);

        /**
         * Method that updates the page extents and aggregation data for every page under the cursor.
         */
        void updateAggregationValues();

        /**
         * Method that updates the maximum page margin values that can be used.
         */
        void updatePageMarginLimitsValues();

        /**
         * Method that issues an \ref UpdatePageFormatCommand to the view.
         *
         * \param[in] newPageFormats A map of new page formats keyed by page index.
         */
        void issueUpdatePageFormatCommand(const UpdatePageFormatCommand::PageFormatsByPageIndex& newPageFormats);

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * The current page format aggregation.
         */
        Ld::PageFormat::Aggregation currentPageFormatAggregation;

        /**
         * The current list of pages under the cursor.
         */
        QList<PageList::Index> currentPagesUnderCursor;

        /**
         * Value indicating the current state of the page orientation button.
         */
        Ld::Format::BooleanAggregation currentPortraitPageOrientation;
};

#endif
