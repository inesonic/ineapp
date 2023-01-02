/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FormatViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FORMAT_VIEW_PROXY_H
#define FORMAT_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>

#include <eqt_programmatic_view_proxy.h>

#include <ld_page_format.h>
#include <ld_update_page_format_command.h>

#include "page_list.h"
#include "view_widget.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

namespace Ld {
    class Element;
}

/**
 * View proxy used to manage the formatting functions.  These functions include:
 *
 *     * Basic format menu items
 *     * Basic formatting dock widget functions.
 */
class FormatViewProxy:public EQt::ProgrammaticViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        FormatViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~FormatViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         *
         * The default implementation simply returns.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         *
         * The default implementation simply returns.
         */
        void unbindDynamic() final;

        /**
         * Method that is called to perform binding of static actions to slots used to perform those actions.  This
         * method is called once each time a new view is created.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered whenever the element stack for the current view changes.
         *
         * \param[in] elementListsByFormatType A map of lists of elements based on the formats they contain.
         */
        void formatsUnderCursorChanged(const Ld::AggregationsByCapability& elementListsByFormatType);

        /**
         * Slot that is triggered whenever the list of pages under the cursor changes.
         *
         * \param[in] pagesUnderCursor A list of page index values indicating the pages that are now under the cursor.
         */
        void pagesUnderCursorChanged(const QList<PageList::Index>& pagesUnderCursor);

        /**
         * Slot that is triggered whenever the user clicks the page orientation button.
         */
        void pageOrientationToolButtonClicked();

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
         * Slot that is triggered when the user requests text be left aligned.
         */
        void paragraphLeftAlign();

        /**
         * Slot that is triggered when the user requests text be centered.
         */
        void paragraphCenter();

        /**
         * Slot that is triggered when the user requests text be right aligned.
         */
        void paragraphRightAlign();

        /**
         * Slot that is triggered when the user requests text be justified between the margins.
         */
        void paragraphJustify();

        /**
         * Slot that is triggered when the user requests decreased paragraph indentations.
         */
        void paragraphDecreaseIndent();

        /**
         * Slot that is triggered when the user requests increased paragraph indentations.
         */
        void paragraphIncreaseIndent();

        /**
         * Slot that is triggered when the user adjusts the left side indentation.
         */
        void paragraphChangeLeftIndentation();

        /**
         * Slot that is triggered when the user adjusts the right side indentation.
         */
        void paragraphChangeRightIndentation();

        /**
         * Slot that is triggered when the user adjusts the paragraph top spacing.
         */
        void paragraphChangeTopSpacing();

        /**
         * Slot that is triggered when the user adjusts the paragraph bottom spacing.
         */
        void paragraphChangeBottomSpacing();

        /**
         * Slot that is triggered when the user adjusts the first line indentation.
         */
        void paragraphChangeFirstLineIndentation();

        /**
         * Slot that is triggered when the user adjusts the line spacing.
         */
        void paragraphChangeLineSpacing();

        /**
         * Slot that is triggered for the Format | Paragraph menu item.
         */
        void paragraphFormatDialogRequested();

    private:
        /**
         * Static value indicating the maximum fraction of a page the margins are allowed to take up.
         */
        static constexpr float maximumMarginFraction = 0.95F;

        /**
         * Method you can use to update the page orientation button state.
         *
         * \param[in] newPortraitOrientationState Value indicating if the button should show the page in portrait
         *                                        orientation, landscape orientation, or both.
         */
        void setPageOrientationButtonState(const Ld::Format::BooleanAggregation& newPortraitOrientationState);

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
         */
        void issueUpdatePageFormatCommand(const Ld::UpdatePageFormatCommand::PageFormatsByPageIndex newPageFormats);

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
