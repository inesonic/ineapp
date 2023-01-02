/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageFormatViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QAction>
#include <QVariant>
#include <QPageSize>
#include <QList>
#include <QMap>
#include <QSet>

#include <util_page_size.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>

#include <ld_element.h>
#include <ld_format.h>
#include <ld_page_format.h>

#include "application.h"
#include "document.h"
#include "page_list.h"
#include "update_page_format_command.h"
#include "main_window.h"
#include "view_widget.h"
#include "page_format_builder.h"
#include "page_format_dialog.h"
#include "page_format_view_proxy.h"

PageFormatViewProxy::PageFormatViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {}


PageFormatViewProxy::~PageFormatViewProxy() {}


void PageFormatViewProxy::bindDynamic() {
    MainWindow* window = PageFormatViewProxy::window();
    ViewWidget* view   = PageFormatViewProxy::view();

    sessionConnect(view, &ViewWidget::pagesUnderCursorChanged, &PageFormatViewProxy::pagesUnderCursorChanged);
    pagesUnderCursorChanged(view->pagesUnderCursor());

    QAction* pagePortraitOrientationAction = window->action("portrait_orientation_action");
    sessionConnect(
        pagePortraitOrientationAction,
        &QAction::triggered,
        &PageFormatViewProxy::pagePortraitOrientationSelected
    );

    QAction* pageLandscapeOrientationAction = window->action("landscape_orientation_action");
    sessionConnect(
        pageLandscapeOrientationAction,
        &QAction::triggered,
        &PageFormatViewProxy::pageLandscapeOrientationSelected
    );

    QAction* pageSizeAction = window->action("format_page_size");
    sessionConnect(pageSizeAction, &QAction::triggered, &PageFormatViewProxy::pageSizeMenuOptionSelected);

    QAction* pageDialogAction = window->action("format_page_dialog");
    sessionConnect(pageDialogAction, &QAction::triggered, &PageFormatViewProxy::pageFormatDialogRequested);

    QAction* pageLeftMarginAction = window->action("format_page_left_margin");
    sessionConnect(pageLeftMarginAction, &QAction::triggered, &PageFormatViewProxy::pageLeftMarginChanged);

    QAction* pageRightMarginAction = window->action("format_page_right_margin");
    sessionConnect(pageRightMarginAction, &QAction::triggered, &PageFormatViewProxy::pageRightMarginChanged);

    QAction* pageTopMarginAction = window->action("format_page_top_margin");
    sessionConnect(pageTopMarginAction, &QAction::triggered, &PageFormatViewProxy::pageTopMarginChanged);

    QAction* pageBottomMarginAction = window->action("format_page_bottom_margin");
    sessionConnect(pageBottomMarginAction, &QAction::triggered, &PageFormatViewProxy::pageBottomMarginChanged);
}


void PageFormatViewProxy::unbindDynamic() {
    sessionDisconnectAll();
}


void PageFormatViewProxy::pagesUnderCursorChanged(const QList<PageList::Index>& pagesUnderCursor) {
    MainWindow* window = PageFormatViewProxy::window();

    bool hasPageFormat = !pagesUnderCursor.isEmpty();

    QAction* pageDialogAction       = window->action("format_page_dialog");
    QAction* pageOrientationAction  = window->action("format_page_orientation");
    QAction* paperSizeButtonAction  = window->action("format_page_size_button");
    QAction* paperSizeAction        = window->action("format_page_size");
    QAction* pageLeftMarginAction   = window->action("format_page_left_margin");
    QAction* pageRightMarginAction  = window->action("format_page_right_margin");
    QAction* pageTopMarginAction    = window->action("format_page_top_margin");
    QAction* pageBottomMarginAction = window->action("format_page_bottom_margin");

    pageDialogAction->setEnabled(hasPageFormat);
    pageOrientationAction->setEnabled(hasPageFormat);
    paperSizeButtonAction->setEnabled(hasPageFormat);
    pageLeftMarginAction->setEnabled(hasPageFormat);
    pageRightMarginAction->setEnabled(hasPageFormat);
    pageTopMarginAction->setEnabled(hasPageFormat);
    pageBottomMarginAction->setEnabled(hasPageFormat);

    currentPageFormatAggregation.clear();
    currentPagesUnderCursor = pagesUnderCursor;

    updateAggregationValues();

    Ld::Format::BooleanAggregation portraitOrientation = currentPageFormatAggregation.portrait();
    setPageOrientationButtonAndMenuState(portraitOrientation);

    QSet<Util::PageSize> pageSizes = currentPageFormatAggregation.pageSizes();
    if (pageSizes.count() == 1) {
        QVariant newValue = QVariant(pageSizes.begin()->toString());
        paperSizeAction->setData(newValue);
    } else {
        paperSizeAction->setData(QVariant());
    }

    disconnect(pageLeftMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageLeftMarginChanged);
    disconnect(pageRightMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageRightMarginChanged);
    disconnect(pageTopMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageTopMarginChanged);
    disconnect(pageBottomMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageBottomMarginChanged);

    QSet<float> leftMargins = currentPageFormatAggregation.leftMargins();
    if (leftMargins.count() == 1) {
        float leftMargin = *leftMargins.begin();
        pageLeftMarginAction->setData(QVariant::fromValue(leftMargin));
    } else {
        pageLeftMarginAction->setData(QVariant());
    }

    pageLeftMarginAction->trigger();

    QSet<float> rightMargins = currentPageFormatAggregation.rightMargins();
    if (rightMargins.count() == 1) {
        float rightMargin = *rightMargins.begin();
        pageRightMarginAction->setData(QVariant::fromValue(rightMargin));
    } else {
        pageRightMarginAction->setData(QVariant());
    }

    pageRightMarginAction->trigger();

    QSet<float> topMargins = currentPageFormatAggregation.topMargins();
    if (topMargins.count() == 1) {
        float topMargin = *topMargins.begin();
        pageTopMarginAction->setData(QVariant::fromValue(topMargin));
    } else {
        pageTopMarginAction->setData(QVariant());
    }

    pageTopMarginAction->trigger();

    QSet<float> bottomMargins = currentPageFormatAggregation.bottomMargins();
    if (bottomMargins.count() == 1) {
        float bottomMargin = *bottomMargins.begin();
        pageBottomMarginAction->setData(QVariant::fromValue(bottomMargin));
    } else {
        pageBottomMarginAction->setData(QVariant());
    }

    pageBottomMarginAction->trigger();

    connect(pageLeftMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageLeftMarginChanged);
    connect(pageRightMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageRightMarginChanged);
    connect(pageTopMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageTopMarginChanged);
    connect(pageBottomMarginAction, &QAction::triggered, this, &PageFormatViewProxy::pageBottomMarginChanged);
}


void PageFormatViewProxy::pagePortraitOrientationSelected() {
    setPageOrientation(true);
}


void PageFormatViewProxy::pageLandscapeOrientationSelected() {
    setPageOrientation(false);
}


void PageFormatViewProxy::pageSizeMenuOptionSelected() {
    MainWindow* window = PageFormatViewProxy::window();
    ViewWidget* view   = PageFormatViewProxy::view();

    QAction* paperSizeAction = window->action("format_page_size");
    QVariant variant         = paperSizeAction->data();
    if (variant.canConvert<QString>()) {
        QString description = variant.toString();
        Util::PageSize pageSize(description);
        Q_ASSERT(pageSize.isValid());

        UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

        for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                      pageIndexEndIterator = currentPagesUnderCursor.constEnd()
             ; pageIndexIterator != pageIndexEndIterator
             ; ++pageIndexIterator
            ) {
            PageList::Index pageIndex = *pageIndexIterator;
            QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
            Q_ASSERT(!pageFormat.isNull());
            if (pageFormat->pageSize() != pageSize) {
                QSharedPointer<Ld::PageFormat> newPageFormat = pageFormat->clone().dynamicCast<Ld::PageFormat>();

                newPageFormat->setPageSize(pageSize);
                pageFormatsByPageIndex.insert(pageIndex, newPageFormat);
            }
        }

        issueUpdatePageFormatCommand(pageFormatsByPageIndex);
        updateAggregationValues();
    }
}


void PageFormatViewProxy::pageLeftMarginChanged() {
    bool        ok;
    MainWindow* window               = PageFormatViewProxy::window();
    ViewWidget* view                 = PageFormatViewProxy::view();
    QAction*    pageLeftMarginAction = window->action("format_page_left_margin");
    QVariant    variant              = pageLeftMarginAction->data();
    double      newLeftMargin        = variant.toDouble(&ok);

    if (ok) {
        UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

        for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                      pageIndexEndIterator = currentPagesUnderCursor.constEnd()
             ; pageIndexIterator != pageIndexEndIterator
             ; ++pageIndexIterator
            ) {
            PageList::Index pageIndex = *pageIndexIterator;
            QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
            Q_ASSERT(!pageFormat.isNull());
            if (pageFormat->leftMargin() != newLeftMargin) {
                QSharedPointer<Ld::PageFormat> newPageFormat = pageFormat->clone().dynamicCast<Ld::PageFormat>();

                newPageFormat->setLeftMargin(newLeftMargin);
                pageFormatsByPageIndex.insert(pageIndex, newPageFormat);
            }
        }

        issueUpdatePageFormatCommand(pageFormatsByPageIndex);
        updateAggregationValues();
    }

    restoreFocus();
}


void PageFormatViewProxy::pageRightMarginChanged() {
    bool        ok;
    MainWindow* window                = PageFormatViewProxy::window();
    ViewWidget* view                  = PageFormatViewProxy::view();
    QAction*    pageRightMarginAction = window->action("format_page_right_margin");
    QVariant    variant               = pageRightMarginAction->data();
    double      newRightMargin        = variant.toDouble(&ok);

    if (ok) {
        UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

        for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                      pageIndexEndIterator = currentPagesUnderCursor.constEnd()
             ; pageIndexIterator != pageIndexEndIterator
             ; ++pageIndexIterator
            ) {
            PageList::Index pageIndex = *pageIndexIterator;
            QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
            Q_ASSERT(!pageFormat.isNull());
            if (pageFormat->rightMargin() != newRightMargin) {
                QSharedPointer<Ld::PageFormat> newPageFormat = pageFormat->clone().dynamicCast<Ld::PageFormat>();

                newPageFormat->setRightMargin(newRightMargin);
                pageFormatsByPageIndex.insert(pageIndex, newPageFormat);
            }
        }

        issueUpdatePageFormatCommand(pageFormatsByPageIndex);
        updateAggregationValues();
    }

    restoreFocus();
}


void PageFormatViewProxy::pageTopMarginChanged() {
    bool        ok;
    MainWindow* window              = PageFormatViewProxy::window();
    ViewWidget* view                = PageFormatViewProxy::view();
    QAction*    pageTopMarginAction = window->action("format_page_top_margin");
    QVariant    variant             = pageTopMarginAction->data();
    double      newTopMargin        = variant.toDouble(&ok);

    if (ok) {
        UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

        for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                      pageIndexEndIterator = currentPagesUnderCursor.constEnd()
             ; pageIndexIterator != pageIndexEndIterator
             ; ++pageIndexIterator
            ) {
            PageList::Index pageIndex = *pageIndexIterator;
            QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
            Q_ASSERT(!pageFormat.isNull());
            if (pageFormat->topMargin() != newTopMargin) {
                QSharedPointer<Ld::PageFormat> newPageFormat = pageFormat->clone().dynamicCast<Ld::PageFormat>();

                newPageFormat->setTopMargin(newTopMargin);
                pageFormatsByPageIndex.insert(pageIndex, newPageFormat);
            }
        }

        issueUpdatePageFormatCommand(pageFormatsByPageIndex);
        updateAggregationValues();
    }

    restoreFocus();
}


void PageFormatViewProxy::pageBottomMarginChanged() {
    bool        ok;
    MainWindow* window                 = PageFormatViewProxy::window();
    ViewWidget* view                   = PageFormatViewProxy::view();
    QAction*    pageBottomMarginAction = window->action("format_page_bottom_margin");
    QVariant    variant                = pageBottomMarginAction->data();
    double      newBottomMargin        = variant.toDouble(&ok);

    if (ok) {
        UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

        for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                      pageIndexEndIterator = currentPagesUnderCursor.constEnd()
             ; pageIndexIterator != pageIndexEndIterator
             ; ++pageIndexIterator
            ) {
            PageList::Index pageIndex = *pageIndexIterator;
            QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
            Q_ASSERT(!pageFormat.isNull());
            if (pageFormat->bottomMargin() != newBottomMargin) {
                QSharedPointer<Ld::PageFormat> newPageFormat = pageFormat->clone().dynamicCast<Ld::PageFormat>();

                newPageFormat->setBottomMargin(newBottomMargin);
                pageFormatsByPageIndex.insert(pageIndex, newPageFormat);
            }
        }

        issueUpdatePageFormatCommand(pageFormatsByPageIndex);
        updateAggregationValues();
    }

    restoreFocus();
}


void PageFormatViewProxy::setPageOrientationButtonAndMenuState(
        const Ld::Format::BooleanAggregation& newPortraitOrientationState
    ) {
    MainWindow* window                         = PageFormatViewProxy::window();
    QAction*    pagePortraitOrientationAction  = window->action("portrait_orientation_action");
    QAction*    pageLandscapeOrientationAction = window->action("landscape_orientation_action");
    QAction*    pageOrientationAction          = window->action("format_page_orientation");

    if (newPortraitOrientationState.allTrue()) {
        pageOrientationAction->setIcon(EQt::Application::icon("format_page_orientation_portrait"));
        pagePortraitOrientationAction->setChecked(true);
        pageLandscapeOrientationAction->setChecked(false);

        pagePortraitOrientationAction->setEnabled(false);
        pageLandscapeOrientationAction->setEnabled(true);
    } else {
        pageLandscapeOrientationAction->setChecked(true);
        pagePortraitOrientationAction->setEnabled(true);

        if (newPortraitOrientationState.allFalse()) {
            pageOrientationAction->setIcon(EQt::Application::icon("format_page_orientation_landscape"));
            pagePortraitOrientationAction->setChecked(false);
            pageLandscapeOrientationAction->setEnabled(false);
        } else {
            pageOrientationAction->setIcon(EQt::Application::icon("format_page_orientation_undefined"));
            pagePortraitOrientationAction->setChecked(true);
            pageLandscapeOrientationAction->setEnabled(true);
        }
    }

    currentPortraitPageOrientation = newPortraitOrientationState;
}


void PageFormatViewProxy::setPageOrientation(bool nowPortrait) {
    ViewWidget* view = PageFormatViewProxy::view();

    Ld::Format::BooleanAggregation newPortraitPageOrientation;
    newPortraitPageOrientation << nowPortrait;

    setPageOrientationButtonAndMenuState(newPortraitPageOrientation);

    UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

    for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                  pageIndexEndIterator = currentPagesUnderCursor.constEnd()
         ; pageIndexIterator != pageIndexEndIterator
         ; ++pageIndexIterator
        ) {
        PageList::Index pageIndex = *pageIndexIterator;
        QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
        Q_ASSERT(!pageFormat.isNull());

        if (pageFormat->portrait() != nowPortrait) {
            QSharedPointer<Ld::PageFormat> newPageFormat = pageFormat->clone().dynamicCast<Ld::PageFormat>();

            newPageFormat->setPortrait(nowPortrait);
            pageFormatsByPageIndex.insert(pageIndex, newPageFormat);
        }
    }

    issueUpdatePageFormatCommand(pageFormatsByPageIndex);
    updateAggregationValues();
}


void PageFormatViewProxy::pageFormatDialogRequested() {
    MainWindow* window = PageFormatViewProxy::window();

    PageFormatDialog pageFormatDialog(currentPageFormatAggregation, window);
    connect(&pageFormatDialog, &PageFormatDialog::apply, this, &PageFormatViewProxy::pageFormatDialogApplyUpdates);

    pageFormatDialog.loadSettings(QString("PageFormatDialog"));

    int result = pageFormatDialog.exec();
    if (result == PageFormatDialog::Accepted) {
        pageFormatDialogApplyUpdates(&pageFormatDialog);
    }
}


void PageFormatViewProxy::pageFormatDialogApplyUpdates(PageFormatDialog* dialog) {
    dialog->saveSettings(QString("PageFormatDialog"));

    const Ld::PageFormat::Aggregation& aggregation = dialog->outputAggregation();
    ViewWidget*                        view        = PageFormatViewProxy::view();
    Document*                          document    = view->document();
    QSharedPointer<Ld::RootElement>    rootElement = document->element();

    UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

    for (  QList<PageList::Index>::const_iterator pageListIterator    = currentPagesUnderCursor.constBegin(),
                                                  pageListEndIterator = currentPagesUnderCursor.constEnd()
         ; pageListIterator != pageListEndIterator
         ; ++pageListIterator
        ) {
        PageList::Index                pageIndex = *pageListIterator;
        QSharedPointer<Ld::PageFormat> format    = rootElement->pageFormat(pageIndex)->clone()
                                                   .dynamicCast<Ld::PageFormat>();
        bool                           modified  = false;

        if (aggregation.pageSizes().count() == 1) {
            const Util::PageSize& newPageSize = *(aggregation.pageSizes().constBegin());
            if (format->pageSize() != newPageSize) {
                format->setPageSize(newPageSize);
                modified = true;
            }
        }

        if (!aggregation.portrait().bothTrueAndFalse()) {
            bool isPortrait = aggregation.portrait().allTrue();
            if (format->portrait() != isPortrait) {
                format->setPortrait(isPortrait);
                modified = true;
            }
        }

        if (aggregation.leftMargins().count() == 1 || aggregation.rightMargins().count() == 1) {
            float newLeftMargin =   aggregation.leftMargins().count() == 1
                                  ? *aggregation.leftMargins().begin()
                                  : format->leftMargin();

            float newRightMargin =   aggregation.rightMargins().count() == 1
                                   ? *aggregation.rightMargins().begin()
                                   : format->rightMargin();

            if (format->leftMargin() != newLeftMargin || format->rightMargin() != newRightMargin) {
                if (aggregation.leftMargins().count() == 1 && aggregation.rightMargins().count() == 1) {
                    format->setLeftMargin(0);
                    format->setRightMargin(0);
                }

                format->setLeftMargin(newLeftMargin);
                format->setRightMargin(newRightMargin);

                modified = true;
            }
        }

        if (aggregation.topMargins().count() == 1 || aggregation.bottomMargins().count() == 1) {
            float newTopMargin =   aggregation.topMargins().count() == 1
                                 ? *aggregation.topMargins().begin()
                                 : format->topMargin();

            float newBottomMargin =   aggregation.bottomMargins().count() == 1
                                    ? *aggregation.bottomMargins().begin()
                                    : format->bottomMargin();

            if (format->topMargin() != newTopMargin || format->bottomMargin() != newBottomMargin) {
                if (aggregation.topMargins().count() == 1 && aggregation.bottomMargins().count() == 1) {
                    format->setTopMargin(0);
                    format->setBottomMargin(0);
                }

                format->setTopMargin(newTopMargin);
                format->setBottomMargin(newBottomMargin);

                modified = true;
            }
        }

        if (modified) {
            pageFormatsByPageIndex.insert(pageIndex, format);
        }
    }

    if (!pageFormatsByPageIndex.isEmpty()) {
        issueUpdatePageFormatCommand(pageFormatsByPageIndex);
        updateAggregationValues();
    }
}


void PageFormatViewProxy::updateAggregationValues() {
    ViewWidget* view = PageFormatViewProxy::view();

    currentPageFormatAggregation.clear();

    for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                  pageIndexEndIterator = currentPagesUnderCursor.constEnd()
         ; pageIndexIterator != pageIndexEndIterator
         ; ++pageIndexIterator
        ) {
        PageList::Index                pageIndex  = *pageIndexIterator;
        QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
        Q_ASSERT(!pageFormat.isNull());

        currentPageFormatAggregation.addFormat(pageFormat);
    }

    updatePageMarginLimitsValues();
}


void PageFormatViewProxy::updatePageMarginLimitsValues() {
    MainWindow*                  window = PageFormatViewProxy::window();
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget(PageFormatBuilder::pageFormatDockName);

    EQt::DimensionLineEdit* horizontalLeftMarginLineEdit;
    EQt::DimensionLineEdit* horizontalRightMarginLineEdit;
    EQt::DimensionLineEdit* horizontalTopMarginLineEdit;
    EQt::DimensionLineEdit* horizontalBottomMarginLineEdit;

    EQt::DimensionLineEdit* verticalLeftMarginLineEdit;
    EQt::DimensionLineEdit* verticalRightMarginLineEdit;
    EQt::DimensionLineEdit* verticalTopMarginLineEdit;
    EQt::DimensionLineEdit* verticalBottomMarginLineEdit;

    horizontalLeftMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_left_margin_line_edit"
    );

    horizontalRightMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_right_margin_line_edit"
    );

    horizontalTopMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_top_margin_line_edit"
    );

    horizontalBottomMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_bottom_margin_line_edit"
    );

    verticalLeftMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_left_margin_line_edit"
    );

    verticalRightMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_right_margin_line_edit"
    );

    verticalTopMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_top_margin_line_edit"
    );

    verticalBottomMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_bottom_margin_line_edit"
    );

    float maximumLeftMargin   = currentPageFormatAggregation.maximumAllowableLeftMargin();
    float maximumRightMargin  = currentPageFormatAggregation.maximumAllowableRightMargin();
    float maximumTopMargin    = currentPageFormatAggregation.maximumAllowableTopMargin();
    float maximumBottomMargin = currentPageFormatAggregation.maximumAllowableBottomMargin();

    horizontalLeftMarginLineEdit->setMinimumValue(0);
    horizontalRightMarginLineEdit->setMinimumValue(0);
    horizontalTopMarginLineEdit->setMinimumValue(0);
    horizontalBottomMarginLineEdit->setMinimumValue(0);

    horizontalLeftMarginLineEdit->setMaximumValue(maximumLeftMargin);
    horizontalRightMarginLineEdit->setMaximumValue(maximumRightMargin);
    horizontalTopMarginLineEdit->setMaximumValue(maximumTopMargin);
    horizontalBottomMarginLineEdit->setMaximumValue(maximumBottomMargin);

    verticalLeftMarginLineEdit->setMinimumValue(0);
    verticalRightMarginLineEdit->setMinimumValue(0);
    verticalTopMarginLineEdit->setMinimumValue(0);
    verticalBottomMarginLineEdit->setMinimumValue(0);

    verticalLeftMarginLineEdit->setMaximumValue(maximumLeftMargin);
    verticalRightMarginLineEdit->setMaximumValue(maximumRightMargin);
    verticalTopMarginLineEdit->setMaximumValue(maximumTopMargin);
    verticalBottomMarginLineEdit->setMaximumValue(maximumBottomMargin);
}


void PageFormatViewProxy::issueUpdatePageFormatCommand(
        const UpdatePageFormatCommand::PageFormatsByPageIndex& newPageFormats
    ) {
    ViewWidget*                     view        = PageFormatViewProxy::view();
    QSharedPointer<Ld::RootElement> rootElement = view->document()->element();

    view->insertCommand(new UpdatePageFormatCommand(rootElement, newPageFormats));
}


void PageFormatViewProxy::restoreFocus() {
    view()->restoreFocus();
}
