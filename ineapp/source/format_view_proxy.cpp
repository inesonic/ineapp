/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FormatViewProxy class.
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

#include <QDebug> // Debug

#include <util_page_size.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_dimension_line_edit.h>

#include <ld_element.h>
#include <ld_format.h>
#include <ld_page_format.h>
#include <ld_paragraph_format.h>
#include <ld_character_format.h>
#include <ld_table_format.h>
#include <ld_table_cell_format.h>
#include <ld_update_page_format_command.h>

#include "application.h"
#include "document.h"
#include "page_list.h"
#include "main_window.h"
#include "view_widget.h"
#include "page_format_dialog.h"
#include "format_view_proxy.h"

FormatViewProxy::FormatViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):EQt::ProgrammaticViewProxy(
        view,
        window
    ) {}


FormatViewProxy::~FormatViewProxy() {}


void FormatViewProxy::bindDynamic() {
    MainWindow* window = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view   = dynamic_cast<ViewWidget*>(FormatViewProxy::view());

    /* View */

    connect(view, &ViewWidget::formatsAtCursorChanged, this, &FormatViewProxy::formatsUnderCursorChanged);
    formatsUnderCursorChanged(view->aggregationsByCapability());

    connect(view, &ViewWidget::pagesUnderCursorChanged, this, &FormatViewProxy::pagesUnderCursorChanged);
    pagesUnderCursorChanged(view->pagesUnderCursor());

    /* Page */

    QAction* pageOrientationAction = window->action("format_page_orientation");
    connect(pageOrientationAction, &QAction::triggered, this, &FormatViewProxy::pageOrientationToolButtonClicked);

    QAction* pageSizeAction = window->action("format_page_size");
    connect(pageSizeAction, &QAction::triggered, this, &FormatViewProxy::pageSizeMenuOptionSelected);

    QAction* pageDialogAction = window->action("format_page_dialog");
    connect(pageDialogAction, &QAction::triggered, this, &FormatViewProxy::pageFormatDialogRequested);

    QAction* pageLeftMarginAction = window->action("format_page_left_margin");
    connect(pageLeftMarginAction, &QAction::triggered, this, &FormatViewProxy::pageLeftMarginChanged);

    QAction* pageRightMarginAction = window->action("format_page_right_margin");
    connect(pageRightMarginAction, &QAction::triggered, this, &FormatViewProxy::pageRightMarginChanged);

    QAction* pageTopMarginAction = window->action("format_page_top_margin");
    connect(pageTopMarginAction, &QAction::triggered, this, &FormatViewProxy::pageTopMarginChanged);

    QAction* pageBottomMarginAction = window->action("format_page_bottom_margin");
    connect(pageBottomMarginAction, &QAction::triggered, this, &FormatViewProxy::pageBottomMarginChanged);

    /* Paragraph */

    QAction* paragraphLeftAlignAction = window->action("format_left_align_text");
    connect(paragraphLeftAlignAction, &QAction::triggered, this, &FormatViewProxy::paragraphLeftAlign);

    QAction* paragraphCenterAction = window->action("format_center_text");
    connect(paragraphCenterAction, &QAction::triggered, this, &FormatViewProxy::paragraphCenter);

    QAction* paragraphRightAlignAction = window->action("format_right_align_text");
    connect(paragraphRightAlignAction, &QAction::triggered, this, &FormatViewProxy::paragraphRightAlign);

    QAction* paragraphJustifyAction = window->action("format_justify_text");
    connect(paragraphJustifyAction, &QAction::triggered, this, &FormatViewProxy::paragraphJustify);

    QAction* paragraphDecreaseIndentAction = window->action("format_paragraph_decrease_indent");
    connect(paragraphDecreaseIndentAction, &QAction::triggered, this, &FormatViewProxy::paragraphDecreaseIndent);

    QAction* paragraphIncreaseIndentAction = window->action("format_paragraph_increase_indent");
    connect(paragraphIncreaseIndentAction, &QAction::triggered, this, &FormatViewProxy::paragraphIncreaseIndent);

    QAction* paragraphLeftIndentationAction = window->action("format_paragraph_left_indentation");
    connect(
        paragraphLeftIndentationAction,
        &QAction::triggered,
        this,
        &FormatViewProxy::paragraphChangeLeftIndentation
    );

    QAction* paragraphRightIndentationAction = window->action("format_paragraph_right_indentation");
    connect(
        paragraphRightIndentationAction,
        &QAction::triggered,
        this,
        &FormatViewProxy::paragraphChangeRightIndentation
    );

    QAction* paragraphTopSpacingAction = window->action("format_paragraph_top_spacing");
    connect(paragraphTopSpacingAction, &QAction::triggered, this, &FormatViewProxy::paragraphChangeTopSpacing);

    QAction* paragraphBottomSpacingAction = window->action("format_paragraph_bottom_spacing");
    connect(paragraphBottomSpacingAction, &QAction::triggered, this, &FormatViewProxy::paragraphChangeBottomSpacing);

    QAction* paragraphFirstLineIndentationAction = window->action("format_paragraph_first_line_indentation");
    connect(
        paragraphFirstLineIndentationAction,
        &QAction::triggered,
        this,
        &FormatViewProxy::paragraphChangeFirstLineIndentation
    );

    QAction* paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");
    connect(paragraphLineSpacingAction, &QAction::triggered, this, &FormatViewProxy::paragraphChangeLineSpacing);

    QAction* paragraphDialogAction = window->action("format_paragraph_dialog");
    connect(paragraphDialogAction, &QAction::triggered, this, &FormatViewProxy::paragraphFormatDialogRequested);
}


void FormatViewProxy::unbindDynamic() {
    MainWindow* window = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view   = dynamic_cast<ViewWidget*>(FormatViewProxy::view());

    /* View */
    disconnect(view, &ViewWidget::formatsAtCursorChanged, this, &FormatViewProxy::formatsUnderCursorChanged);
    disconnect(view, &ViewWidget::pagesUnderCursorChanged, this, &FormatViewProxy::pagesUnderCursorChanged);

    /* Page */

    QAction* pageOrientationAction = window->action("format_page_orientation");
    disconnect(pageOrientationAction, &QAction::triggered, this, &FormatViewProxy::pageOrientationToolButtonClicked);

    QAction* pageSizeAction  = window->action("format_page_size");
    disconnect(pageSizeAction, &QAction::triggered, this, &FormatViewProxy::pageSizeMenuOptionSelected);

    QAction* pageDialogAction = window->action("format_page_dialog");
    disconnect(pageDialogAction, &QAction::triggered, this, &FormatViewProxy::pageFormatDialogRequested);

    QAction* pageLeftMarginAction = window->action("format_page_left_margin");
    disconnect(pageLeftMarginAction, &QAction::triggered, this, &FormatViewProxy::pageLeftMarginChanged);

    QAction* pageRightMarginAction = window->action("format_page_right_margin");
    disconnect(pageRightMarginAction, &QAction::triggered, this, &FormatViewProxy::pageRightMarginChanged);

    QAction* pageTopMarginAction = window->action("format_page_top_margin");
    disconnect(pageTopMarginAction, &QAction::triggered, this, &FormatViewProxy::pageTopMarginChanged);

    QAction* pageBottomMarginAction = window->action("format_page_bottom_margin");
    disconnect(pageBottomMarginAction, &QAction::triggered, this, &FormatViewProxy::pageBottomMarginChanged);

    /* Paragraph */

    QAction* paragraphCenterAction = window->action("format_center_text");
    disconnect(paragraphCenterAction, &QAction::triggered, this, &FormatViewProxy::paragraphCenter);

    QAction* paragraphRightAlignAction = window->action("format_right_align_text");
    disconnect(paragraphRightAlignAction, &QAction::triggered, this, &FormatViewProxy::paragraphRightAlign);

    QAction* paragraphJustifyAction = window->action("format_justify_text");
    disconnect(paragraphJustifyAction, &QAction::triggered, this, &FormatViewProxy::paragraphJustify);

    QAction* paragraphDecreaseIndentAction = window->action("format_paragraph_decrease_indent");
    disconnect(paragraphDecreaseIndentAction, &QAction::triggered, this, &FormatViewProxy::paragraphDecreaseIndent);

    QAction* paragraphIncreaseIndentAction = window->action("format_paragraph_increase_indent");
    disconnect(paragraphIncreaseIndentAction, &QAction::triggered, this, &FormatViewProxy::paragraphIncreaseIndent);

    QAction* paragraphLeftIndentationAction = window->action("format_paragraph_left_indentation");
    disconnect(
        paragraphLeftIndentationAction,
        &QAction::triggered,
        this,
        &FormatViewProxy::paragraphChangeLeftIndentation
    );

    QAction* paragraphRightIndentationAction = window->action("format_paragraph_right_indentation");
    disconnect(
        paragraphRightIndentationAction,
        &QAction::triggered,
        this,
        &FormatViewProxy::paragraphChangeRightIndentation
    );

    QAction* paragraphTopSpacingAction = window->action("format_paragraph_top_spacing");
    disconnect(paragraphTopSpacingAction, &QAction::triggered, this, &FormatViewProxy::paragraphChangeTopSpacing);

    QAction* paragraphBottomSpacingAction = window->action("format_paragraph_bottom_spacing");
    disconnect(paragraphBottomSpacingAction, &QAction::triggered, this, &FormatViewProxy::paragraphChangeBottomSpacing);

    QAction* paragraphFirstLineIndentationAction = window->action("format_paragraph_first_line_indentation");
    disconnect(
        paragraphFirstLineIndentationAction,
        &QAction::triggered,
        this,
        &FormatViewProxy::paragraphChangeFirstLineIndentation
    );

    QAction* paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");
    disconnect(paragraphLineSpacingAction, &QAction::triggered, this, &FormatViewProxy::paragraphChangeLineSpacing);

    QAction* paragraphDialogAction = window->action("format_paragraph_dialog");
    disconnect(paragraphDialogAction, &QAction::triggered, this, &FormatViewProxy::paragraphFormatDialogRequested);
}


void FormatViewProxy::bind() {
//    ViewWidget* view = dynamic_cast<ViewWidget*>(FormatViewProxy::view());

//    connect(view, &ViewWidget::titleChanged, this, &FormatViewProxy::titleChanged);
//    connect(view, &ViewWidget::shortformNameChanged, this, &FormatViewProxy::shortformNameChanged);
//    connect(view, &ViewWidget::documentModified, this, &FormatViewProxy::documentModified);
}


void FormatViewProxy::formatsUnderCursorChanged(const Ld::AggregationsByCapability& aggregationsByCapability) {
    MainWindow* window = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view   = dynamic_cast<ViewWidget*>(FormatViewProxy::view());

    /* Block formats */

    QAction* paragraphLeftAlignAction = window->action("format_left_align_text");
    QAction* paragraphCenterAction = window->action("format_center_text");
    QAction* paragraphRightAlignAction = window->action("format_right_align_text");
    QAction* paragraphJustifyAction = window->action("format_justify_text");
    QAction* paragraphDecreaseIndentAction = window->action("format_paragraph_decrease_indent");
    QAction* paragraphIncreaseIndentAction = window->action("format_paragraph_increase_indent");
    QAction* paragraphLeftIndentationAction = window->action("format_paragraph_left_indentation");
    QAction* paragraphRightIndentationAction = window->action("format_paragraph_right_indentation");
    QAction* paragraphTopSpacingAction = window->action("format_paragraph_top_spacing");
    QAction* paragraphBottomSpacingAction = window->action("format_paragraph_bottom_spacing");
    QAction* paragraphFirstLineIndentationAction = window->action("format_paragraph_first_line_indentation");
    QAction* paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");
    QAction* paragraphDialogAction = window->action("format_paragraph_dialog");

    bool hasJustifiedBlockFormat = aggregationsByCapability.contains(Ld::JustifiedBlockFormat::formatName);
    bool hasTextBlockFormat      = aggregationsByCapability.contains(Ld::TextBlockFormat::formatName);
    bool hasParagraphFormat      = aggregationsByCapability.contains(Ld::ParagraphFormat::formatName);
    bool hasTableFormat          = aggregationsByCapability.contains(Ld::TableFormat::formatName);
    bool hasTableCellFormat      = aggregationsByCapability.contains(Ld::TableCellFormat::formatName);
    bool hasCharacterFormat      = aggregationsByCapability.contains(Ld::CharacterFormat::formatName);

    if (aggregationsByCapability.contains(Ld::BlockFormat::formatName)) {
        const Ld::BlockFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        paragraphDecreaseIndentAction->setEnabled(true);
        paragraphIncreaseIndentAction->setEnabled(true);
        paragraphLeftIndentationAction->setEnabled(true);
        paragraphRightIndentationAction->setEnabled(true);
        paragraphTopSpacingAction->setEnabled(true);
        paragraphBottomSpacingAction->setEnabled(true);

        if (aggregation.leftIndentations().count() == 1) {
            float leftIndentation = *aggregation.leftIndentations().begin();
            paragraphLeftIndentationAction->setData(QVariant::fromValue(leftIndentation));
        } else {
            paragraphLeftIndentationAction->setData(QVariant());
        }
        paragraphLeftIndentationAction->trigger();

        if (aggregation->rightIndentations().count() == 1) {
            float rightIndentation = *aggregation.rightIndentations().begin();
            paragraphRightIndentationAction->setData(QVariant::fromValue(rightIndentation));
        } else {
            paragraphRightIndentationAction->setData(QVariant());
        }
        paragraphRightIndentationAction->trigger();

        if (aggregation->topSpacings().count() == 1) {
            float topSpacing = *aggregation->topSpacings().begin();
            paragraphTopSpacingAction->setData(QVariant::fromValue(topSpacing));
        } else {
            paragraphTopSpacingAction->setData(QVariant());
        }
        paragraphTopSpacingAction->trigger();

        if (aggregation->bottomSpacings().count() == 1) {
            float bottomSpacing = *aggregation->bottomSpacings().begin();
            paragraphBottomSpacingAction->setData(QVariant::fromValue(bottomSpacing));
        } else {
            paragraphBottomSpacingAction->setData(QVariant());
        }
        paragraphBottomSpacingAction->trigger();
    } else {
        paragraphLeftIndentationAction->setData(QVariant());
        paragraphLeftIndentationAction->trigger();

        paragraphRightIndentationAction->setData(QVariant());
        paragraphRightIndentationAction->trigger();

        paragraphTopSpacingAction->setData(QVariant());
        paragraphTopSpacingAction->trigger();

        paragraphBottomSpacingAction->setData(QVariant());
        paragraphBottomSpacingAction->trigger();

        paragraphDecreaseIndentAction->setEnabled(false);
        paragraphIncreaseIndentAction->setEnabled(false);
        paragraphLeftIndentationAction->setEnabled(false);
        paragraphRightIndentationAction->setEnabled(false);
        paragraphTopSpacingAction->setEnabled(false);
        paragraphBottomSpacingAction->setEnabled(false);
    }

    if (hasJustifiedBlockFormat) {
        QSharedPointer<Ld::JustifiedBlockFormat::Aggregation> aggregation = aggregationsByCapability.value(
            Ld::JustifiedBlockFormat::formatName
        ).dynamicCast<Ld::JustifiedBlockFormat::Aggregation>();

        paragraphLeftAlignAction->setEnabled(
            aggregation->fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::LEFT)
        );

        paragraphRightAlignAction->setEnabled(
            aggregation->fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::RIGHT)
        );

        paragraphCenterAction->setEnabled(
            aggregation->fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::CENTER)
        );

        paragraphJustifyAction->setEnabled(
            aggregation->fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::JUSTIFY)
        );

        QSet<Ld::JustifiedBlockFormat::Justification> justifications = aggregation->justifications();
        paragraphLeftAlignAction->setChecked(justifications.contains(Ld::JustifiedBlockFormat::Justification::LEFT));
        paragraphCenterAction->setChecked(justifications.contains(Ld::JustifiedBlockFormat::Justification::CENTER));
        paragraphRightAlignAction->setChecked(justifications.contains(Ld::JustifiedBlockFormat::Justification::RIGHT));
        paragraphJustifyAction->setChecked(justifications.contains(Ld::JustifiedBlockFormat::Justification::JUSTIFY));
    } else {
        paragraphLeftAlignAction->setChecked(false);
        paragraphCenterAction->setChecked(false);
        paragraphRightAlignAction->setChecked(false);
        paragraphJustifyAction->setChecked(false);

        paragraphLeftAlignAction->setEnabled(false);
        paragraphRightAlignAction->setEnabled(false);
        paragraphCenterAction->setEnabled(false);
        paragraphJustifyAction->setEnabled(false);
    }

    // Continue here with TextBlockFormat stuff
}


void FormatViewProxy::pagesUnderCursorChanged(const QList<PageList::Index>& pagesUnderCursor) {
    MainWindow*                  window = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget*                  view   = dynamic_cast<ViewWidget*>(FormatViewProxy::view());
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget("page_format");

    bool hasPageFormat = !pagesUnderCursor.isEmpty();

    QAction* pageDialogAction       = window->action("format_page_dialog");
    QAction* pageOrientationAction  = window->action("format_page_orientation");
    QAction* paperSizeButtonAction  = window->action("format_page_size_button");
    QAction* paperSizeAction        = window->action("format_page_size");
    QAction* pageLeftMarginAction   = window->action("format_page_left_margin");
    QAction* pageRightMarginAction  = window->action("format_page_right_margin");
    QAction* pageTopMarginAction    = window->action("format_page_top_margin");
    QAction* pageBottomMarginAction = window->action("format_page_bottom_margin");

    QGroupBox* horizontalPageGroupBox = dock->EQt::ProgrammaticWidget::widget<QGroupBox>(
        "horizontal_page_group_box"
    );
//    QGroupBox* verticalPageGroupBox = dock->EQt::ProgrammaticWidget::widget<QGroupBox>(
//        "vertical_page_group_box"
//    );

    pageDialogAction->setEnabled(hasPageFormat);
    pageOrientationAction->setEnabled(hasPageFormat);
    paperSizeButtonAction->setEnabled(hasPageFormat);
    pageLeftMarginAction->setEnabled(hasPageFormat);
    pageRightMarginAction->setEnabled(hasPageFormat);
    pageTopMarginAction->setEnabled(hasPageFormat);
    pageBottomMarginAction->setEnabled(hasPageFormat);

    horizontalPageGroupBox->setEnabled(hasPageFormat);
//    verticalPageGroupBox->setEnabled(hasPageFormat);

    currentPageFormatAggregation.clear();
    currentPagesUnderCursor = pagesUnderCursor;

    updateAggregationValues();

    Ld::Format::BooleanAggregation portraitOrientation = currentPageFormatAggregation.portrait();
    setPageOrientationButtonState(portraitOrientation);

    QSet<Util::PageSize> pageSizes = currentPageFormatAggregation.pageSizes();
    if (pageSizes.count() == 1) {
        QVariant newValue = QVariant(pageSizes.begin()->toString());
        paperSizeAction->setData(newValue);
    } else {
        paperSizeAction->setData(QVariant());
    }

    disconnect(pageLeftMarginAction, &QAction::triggered, this, &FormatViewProxy::pageLeftMarginChanged);
    disconnect(pageRightMarginAction, &QAction::triggered, this, &FormatViewProxy::pageRightMarginChanged);
    disconnect(pageTopMarginAction, &QAction::triggered, this, &FormatViewProxy::pageTopMarginChanged);
    disconnect(pageBottomMarginAction, &QAction::triggered, this, &FormatViewProxy::pageBottomMarginChanged);

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

    connect(pageLeftMarginAction, &QAction::triggered, this, &FormatViewProxy::pageLeftMarginChanged);
    connect(pageRightMarginAction, &QAction::triggered, this, &FormatViewProxy::pageRightMarginChanged);
    connect(pageTopMarginAction, &QAction::triggered, this, &FormatViewProxy::pageTopMarginChanged);
    connect(pageBottomMarginAction, &QAction::triggered, this, &FormatViewProxy::pageBottomMarginChanged);
}


void FormatViewProxy::pageOrientationToolButtonClicked() {
    ViewWidget* view = dynamic_cast<ViewWidget*>(FormatViewProxy::view());

    Ld::Format::BooleanAggregation newPortraitPageOrientation;
    bool portraitOrientation = !currentPortraitPageOrientation.allTrue();
    newPortraitPageOrientation << portraitOrientation;

    setPageOrientationButtonState(newPortraitPageOrientation);

    Ld::UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

    for (  QList<PageList::Index>::const_iterator pageIndexIterator    = currentPagesUnderCursor.constBegin(),
                                                  pageIndexEndIterator = currentPagesUnderCursor.constEnd()
         ; pageIndexIterator != pageIndexEndIterator
         ; ++pageIndexIterator
        ) {
        PageList::Index pageIndex = *pageIndexIterator;
        QSharedPointer<Ld::PageFormat> pageFormat = view->pageFormat(pageIndex);
        Q_ASSERT(!pageFormat.isNull());

        if (pageFormat->portrait() != portraitOrientation) {
            QSharedPointer<Ld::PageFormat> newPageFormat = pageFormat->clone().dynamicCast<Ld::PageFormat>();

            newPageFormat->setPortrait(portraitOrientation);
            pageFormatsByPageIndex.insert(pageIndex, newPageFormat);
        }
    }

    issueUpdatePageFormatCommand(pageFormatsByPageIndex);
    updateAggregationValues();
}


void FormatViewProxy::pageSizeMenuOptionSelected() {
    MainWindow* window = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view   = dynamic_cast<ViewWidget*>(FormatViewProxy::view());

    QAction* paperSizeAction = window->action("format_page_size");
    QVariant variant         = paperSizeAction->data();
    if (variant.canConvert<QString>()) {
        QString description = variant.toString();
        Util::PageSize pageSize(description);
        Q_ASSERT(pageSize.isValid());

        Ld::UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

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


void FormatViewProxy::pageLeftMarginChanged() {
    bool        ok;
    MainWindow* window               = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view                 = dynamic_cast<ViewWidget*>(FormatViewProxy::view());
    QAction*    pageLeftMarginAction = window->action("format_page_left_margin");
    QVariant    variant              = pageLeftMarginAction->data();
    double      newLeftMargin        = variant.toDouble(&ok);

    if (ok) {
        Ld::UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

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
}


void FormatViewProxy::pageRightMarginChanged() {
    bool        ok;
    MainWindow* window                = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view                  = dynamic_cast<ViewWidget*>(FormatViewProxy::view());
    QAction*    pageRightMarginAction = window->action("format_page_right_margin");
    QVariant    variant               = pageRightMarginAction->data();
    double      newRightMargin        = variant.toDouble(&ok);

    if (ok) {
        Ld::UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

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
}


void FormatViewProxy::pageTopMarginChanged() {
    bool        ok;
    MainWindow* window              = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view                = dynamic_cast<ViewWidget*>(FormatViewProxy::view());
    QAction*    pageTopMarginAction = window->action("format_page_top_margin");
    QVariant    variant             = pageTopMarginAction->data();
    double      newTopMargin        = variant.toDouble(&ok);

    if (ok) {
        Ld::UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

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
}


void FormatViewProxy::pageBottomMarginChanged() {
    bool        ok;
    MainWindow* window                 = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    ViewWidget* view                   = dynamic_cast<ViewWidget*>(FormatViewProxy::view());
    QAction*    pageBottomMarginAction = window->action("format_page_bottom_margin");
    QVariant    variant                = pageBottomMarginAction->data();
    double      newBottomMargin        = variant.toDouble(&ok);

    if (ok) {
        Ld::UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

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
}


void FormatViewProxy::setPageOrientationButtonState(const Ld::Format::BooleanAggregation& newPortraitOrientationState) {
    MainWindow* window                = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    QAction*    pageOrientationAction = window->action("format_page_orientation");

    if (newPortraitOrientationState.allTrue()) {
        pageOrientationAction->setIcon(EQt::Application::icon("format_page_orientation_landscape"));
        pageOrientationAction->setText(tr("Change to landscape page orientation."));
        pageOrientationAction->setToolTip(tr("Click here to set pages to landscape orientation."));
        pageOrientationAction->setWhatsThis(
            tr(
                "You can use this button to change the page orientation to landscape.  Multiple pages will be updated "
                "if they are selected."
            )
        );
    } else {
        if (newPortraitOrientationState.allFalse()) {
            pageOrientationAction->setIcon(EQt::Application::icon("format_page_orientation_portrait"));
        } else {
            pageOrientationAction->setIcon(EQt::Application::icon("format_page_orientation_undefined"));
        }

        pageOrientationAction->setText(tr("Change to portrait page orientation."));
        pageOrientationAction->setToolTip(tr("Click here to set pages to portrait orientation."));
        pageOrientationAction->setWhatsThis(
            tr(
                "You can use this button to change the page orientation to portrait.  Multiple pages will be updated "
                "if they are selected."
            )
        );
    }

    currentPortraitPageOrientation = newPortraitOrientationState;
}


void FormatViewProxy::pageFormatDialogRequested() {
    MainWindow* window = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    PageFormatDialog pageFormatDialog(&currentPageFormatAggregation, window);

    int result = pageFormatDialog.exec();
    if (result == PageFormatDialog::Accepted) {
        const Ld::PageFormat::Aggregation& aggregation = pageFormatDialog.outputAggregation();
        ViewWidget*                        view        = dynamic_cast<ViewWidget*>(FormatViewProxy::view());
        Document*                          document    = view->document();
        QSharedPointer<Ld::RootElement>    rootElement = document->element();

        Ld::UpdatePageFormatCommand::PageFormatsByPageIndex pageFormatsByPageIndex;

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
}


void FormatViewProxy::paragraphLeftAlign() {}

void FormatViewProxy::paragraphCenter() {}

void FormatViewProxy::paragraphRightAlign() {}

void FormatViewProxy::paragraphJustify() {}

void FormatViewProxy::paragraphDecreaseIndent() {}

void FormatViewProxy::paragraphIncreaseIndent() {}

void FormatViewProxy::paragraphChangeLeftIndentation() {}

void FormatViewProxy::paragraphChangeRightIndentation() {}

void FormatViewProxy::paragraphChangeTopSpacing() {}

void FormatViewProxy::paragraphChangeBottomSpacing() {}

void FormatViewProxy::paragraphChangeFirstLineIndentation() {}

void FormatViewProxy::paragraphChangeLineSpacing() {}

void FormatViewProxy::paragraphFormatDialogRequested() {}


void FormatViewProxy::updateAggregationValues() {
    ViewWidget* view = dynamic_cast<ViewWidget*>(FormatViewProxy::view());

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


void FormatViewProxy::updatePageMarginLimitsValues() {
    MainWindow*                  window = dynamic_cast<MainWindow*>(FormatViewProxy::window());
    EQt::ProgrammaticDockWidget* dock   = window->dockWidget("format");

    EQt::DimensionLineEdit* horizontalPageLeftMarginLineEdit;
    EQt::DimensionLineEdit* horizontalPageRightMarginLineEdit;
    EQt::DimensionLineEdit* horizontalPageTopMarginLineEdit;
    EQt::DimensionLineEdit* horizontalPageBottomMarginLineEdit;

    EQt::DimensionLineEdit* verticalPageLeftMarginLineEdit;
    EQt::DimensionLineEdit* verticalPageRightMarginLineEdit;
    EQt::DimensionLineEdit* verticalPageTopMarginLineEdit;
    EQt::DimensionLineEdit* verticalPageBottomMarginLineEdit;

    horizontalPageLeftMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_page_left_margin_line_edit"
    );

    horizontalPageRightMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_page_right_margin_line_edit"
    );

    horizontalPageTopMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_page_top_margin_line_edit"
    );

    horizontalPageBottomMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "horizontal_page_bottom_margin_line_edit"
    );

    verticalPageLeftMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_page_left_margin_line_edit"
    );

    verticalPageRightMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_page_right_margin_line_edit"
    );

    verticalPageTopMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_page_top_margin_line_edit"
    );

    verticalPageBottomMarginLineEdit = dock->EQt::ProgrammaticWidget::widget<EQt::DimensionLineEdit>(
        "vertical_page_bottom_margin_line_edit"
    );

    float maximumLeftMargin   = currentPageFormatAggregation.maximumAllowableLeftMargin();
    float maximumRightMargin  = currentPageFormatAggregation.maximumAllowableRightMargin();
    float maximumTopMargin    = currentPageFormatAggregation.maximumAllowableTopMargin();
    float maximumBottomMargin = currentPageFormatAggregation.maximumAllowableBottomMargin();

    horizontalPageLeftMarginLineEdit->setMinimumValue(0);
    horizontalPageRightMarginLineEdit->setMinimumValue(0);
    horizontalPageTopMarginLineEdit->setMinimumValue(0);
    horizontalPageBottomMarginLineEdit->setMinimumValue(0);

    horizontalPageLeftMarginLineEdit->setMaximumValue(maximumLeftMargin);
    horizontalPageRightMarginLineEdit->setMaximumValue(maximumRightMargin);
    horizontalPageTopMarginLineEdit->setMaximumValue(maximumTopMargin);
    horizontalPageBottomMarginLineEdit->setMaximumValue(maximumBottomMargin);

//    verticalPageLeftMarginLineEdit->setMinimumValue(0);
//    verticalPageRightMarginLineEdit->setMinimumValue(0);
//    verticalPageTopMarginLineEdit->setMinimumValue(0);
//    verticalPageBottomMarginLineEdit->setMinimumValue(0);

//    verticalPageLeftMarginLineEdit->setMaximumValue(maximumLeftMargin);
//    verticalPageRightMarginLineEdit->setMaximumValue(maximumRightMargin);
//    verticalPageTopMarginLineEdit->setMaximumValue(maximumTopMargin);
//    verticalPageBottomMarginLineEdit->setMaximumValue(maximumBottomMargin);
}


void FormatViewProxy::issueUpdatePageFormatCommand(
        const Ld::UpdatePageFormatCommand::PageFormatsByPageIndex newPageFormats
    ) {
    ViewWidget*                     view        = dynamic_cast<ViewWidget*>(FormatViewProxy::view());
    QSharedPointer<Ld::RootElement> rootElement = view->document()->element();

    view->insertCommand(new Ld::UpdatePageFormatCommand(rootElement, newPageFormats));
}
