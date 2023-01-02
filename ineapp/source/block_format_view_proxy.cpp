/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BlockFormatViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>
#include <QPoint>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QSettings>
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QAction>
#include <QVariant>
#include <QPageSize>
#include <QList>
#include <QMap>
#include <QSet>
#include <QScreen>

#include <util_page_size.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_tool_button.h>
#include <eqt_dimension_line_edit.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_aggregations_by_capability.h>
#include <ld_block_format.h>
#include <ld_justified_block_format.h>
#include <ld_text_block_format.h>
#include <ld_paragraph_format.h>
#include <ld_paragraph_element.h>
#include <ld_list_paragraph_format_base.h>
#include <ld_ordered_list_paragraph_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_list_additional_paragraph_format.h>
#include <ld_element_group.h>

#include "application.h"
#include "document.h"
#include "page_list.h"
#include "bullet_number_popup_dialog.h"
#include "main_window.h"
#include "view_widget.h"
#include "remove_list_format_updater.h"
#include "ordered_list_style_format_updater.h"
#include "unordered_list_style_format_updater.h"
#include "full_list_format_updater.h"
#include "character_format_dialog.h"
#include "character_format_helpers.h"
#include "update_format_command.h"
#include "list_format_dialog.h"
#include "block_format_dialog.h"
#include "view_proxy.h"
#include "block_format_view_proxy.h"

BlockFormatViewProxy::BlockFormatViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {
    bulletNumberPopupDialog = new BulletNumberPopupDialog(window);
    connect(
        bulletNumberPopupDialog,
        &BulletNumberPopupDialog::bulletSelected,
        this,
        &BlockFormatViewProxy::newBulletStyleSelected
    );
    connect(
        bulletNumberPopupDialog,
        &BulletNumberPopupDialog::numberStyleSelected,
        this,
        &BlockFormatViewProxy::newNumberStyleSelected
    );
    connect(
        bulletNumberPopupDialog,
        &BulletNumberPopupDialog::noListStyleSelected,
        this,
        &BlockFormatViewProxy::removeListFormatting
    );
    connect(
        bulletNumberPopupDialog,
        &BulletNumberPopupDialog::focusLost,
        this,
        &BlockFormatViewProxy::noNewBulletStyleSelected
    );

    currentBlockFormatDialog    = Q_NULLPTR;
    currentListFormatDialog     = Q_NULLPTR;
    currentListFontFormatDialog = Q_NULLPTR;
}


BlockFormatViewProxy::~BlockFormatViewProxy() {}


void BlockFormatViewProxy::bindDynamic() {
    ViewWidget* view = BlockFormatViewProxy::view();
    updateBlockFormatDockState(view->aggregationsByCapabilityWithAncestors());

    connectSignals();
}


void BlockFormatViewProxy::unbindDynamic() {
    // Loss of focus should cause the bullet number popup dialog to hide so the lines below are likely not needed.
    if (bulletNumberPopupDialog->isVisible()) {
        bulletNumberPopupDialog->hide();
    }

    disconnectSignals();
}


void BlockFormatViewProxy::formatsUnderCursorChanged(
        const Ld::AggregationsByCapability& aggregationsByCapabilityWithAncestors,
        const Ld::AggregationsByCapability& /* aggregationsByCapabilityWithoutAncestors */
    ) {
    // We temporarily disconnect signals to avoid a storm of superflouous activity when we update the dock widget to
    // to actions being triggered which then trigger call-backs to this class instance.
    //
    // Because of round-off, these superflouos events can in turn trigger generation of UpdateFormatCommand instances
    // modifying the document unexpectedly.

    disconnectSignals();
    updateBlockFormatDockState(aggregationsByCapabilityWithAncestors);
    connectSignals();

    if (currentBlockFormatDialog != Q_NULLPTR) {
        updateBlockFormatDialog(currentBlockFormatDialog);
    }

    if (currentListFormatDialog != Q_NULLPTR) {
        updateListFormatDialog(currentListFormatDialog);
    }

    if (currentListFontFormatDialog != Q_NULLPTR) {
        updateListFontFormatDialog(currentListFontFormatDialog);
    }
}


void BlockFormatViewProxy::paragraphLeftAlign() {
    updateJustification(Ld::JustifiedBlockFormat::Justification::LEFT);
}


void BlockFormatViewProxy::paragraphCenter() {
    updateJustification(Ld::JustifiedBlockFormat::Justification::CENTER);
}


void BlockFormatViewProxy::paragraphRightAlign() {
    updateJustification(Ld::JustifiedBlockFormat::Justification::RIGHT);
}


void BlockFormatViewProxy::paragraphJustify() {
    updateJustification(Ld::JustifiedBlockFormat::Justification::JUSTIFY);
}


void BlockFormatViewProxy::paragraphDecreaseIndent() {
    const Ld::BlockFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::BlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::BlockFormat> format      = element->format().dynamicCast<Ld::BlockFormat>();
            float                           indentation = format->leftIndentation();
            if (indentation > 0) {
                if (indentation > defaultIndentation) {
                    indentation -= defaultIndentation;
                } else {
                    indentation = 0;
                }

                QSharedPointer<Ld::BlockFormat> newFormat = format->clone().dynamicCast<Ld::BlockFormat>();
                newFormat->setLeftIndentation(indentation);

                if (newFormat->capabilities().contains(Ld::TextBlockFormat::formatName)) {
                    QSharedPointer<Ld::TextBlockFormat> textFormat = newFormat.dynamicCast<Ld::TextBlockFormat>();
                    indentation = textFormat->firstLineLeftIndentation();
                    if (indentation > 0) {
                        if (indentation > defaultIndentation) {
                            indentation -= defaultIndentation;
                        } else {
                            indentation = 0;
                        }

                        textFormat->setFirstLineLeftIndentation(indentation);
                    }
                }

                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void BlockFormatViewProxy::paragraphIncreaseIndent() {
    const Ld::BlockFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::BlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                        elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::BlockFormat> format      = element->format().dynamicCast<Ld::BlockFormat>();
            QSharedPointer<Ld::BlockFormat> newFormat = format->clone().dynamicCast<Ld::BlockFormat>();
            newFormat->setLeftIndentation(format->leftIndentation() + defaultIndentation);

            if (newFormat->capabilities().contains(Ld::TextBlockFormat::formatName)) {
                QSharedPointer<Ld::TextBlockFormat> textFormat = newFormat.dynamicCast<Ld::TextBlockFormat>();
                textFormat->setFirstLineLeftIndentation(textFormat->firstLineLeftIndentation() + defaultIndentation);
            }

            formatsByElement.insert(element, newFormat);
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


void BlockFormatViewProxy::paragraphChangeLeftIndentation() {
    MainWindow* window                         = BlockFormatViewProxy::window();
    QAction*    paragraphLeftIndentationAction = window->action("format_paragraph_left_indentation");

    bool  ok;
    float newLeftIndentation = paragraphLeftIndentationAction->data().toFloat(&ok);

    if (ok) {
        const Ld::BlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::BlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::BlockFormat> format = element->format().dynamicCast<Ld::BlockFormat>();
                if (format->leftIndentation() != newLeftIndentation) {
                    QSharedPointer<Ld::BlockFormat> newFormat = format->clone().dynamicCast<Ld::BlockFormat>();
                    newFormat->setLeftIndentation(newLeftIndentation);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::paragraphChangeRightIndentation() {
    MainWindow* window                       = BlockFormatViewProxy::window();
    QAction* paragraphRightIndentationAction = window->action("format_paragraph_right_indentation");

    bool  ok;
    float newRightIndentation = paragraphRightIndentationAction->data().toFloat(&ok);

    if (ok) {
        const Ld::BlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::BlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::BlockFormat> format = element->format().dynamicCast<Ld::BlockFormat>();
                if (format->rightIndentation() != newRightIndentation) {
                    QSharedPointer<Ld::BlockFormat> newFormat = format->clone().dynamicCast<Ld::BlockFormat>();
                    newFormat->setRightIndentation(newRightIndentation);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::paragraphChangeTopSpacing() {
    MainWindow* window                    = BlockFormatViewProxy::window();
    QAction*    paragraphTopSpacingAction = window->action("format_paragraph_top_spacing");

    bool  ok;
    float newTopSpacing = paragraphTopSpacingAction->data().toFloat(&ok);

    if (ok) {
        const Ld::BlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::BlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::BlockFormat> format = element->format().dynamicCast<Ld::BlockFormat>();
                if (format->topSpacing() != newTopSpacing) {
                    QSharedPointer<Ld::BlockFormat> newFormat = format->clone().dynamicCast<Ld::BlockFormat>();
                    newFormat->setTopSpacing(newTopSpacing);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::paragraphChangeBottomSpacing() {
    MainWindow* window                       = BlockFormatViewProxy::window();
    QAction*    paragraphBottomSpacingAction = window->action("format_paragraph_bottom_spacing");

    bool  ok;
    float newBottomSpacing = paragraphBottomSpacingAction->data().toFloat(&ok);

    if (ok) {
        const Ld::BlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::BlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::BlockFormat> format = element->format().dynamicCast<Ld::BlockFormat>();
                if (format->bottomSpacing() != newBottomSpacing) {
                    QSharedPointer<Ld::BlockFormat> newFormat = format->clone().dynamicCast<Ld::BlockFormat>();
                    newFormat->setBottomSpacing(newBottomSpacing);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::paragraphChangeFirstLineIndentation() {
    MainWindow* window                               = BlockFormatViewProxy::window();
    QAction*    paragraphFirstLineIndentationAction = window->action("format_paragraph_first_line_indentation");

    bool  ok;
    float newFirstLineIndentation = paragraphFirstLineIndentationAction->data().toFloat(&ok);

    if (ok) {
        const Ld::TextBlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TextBlockFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::TextBlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                                elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::TextBlockFormat> format = element->format().dynamicCast<Ld::TextBlockFormat>();
                if (format->firstLineLeftIndentation() != newFirstLineIndentation) {
                    QSharedPointer<Ld::TextBlockFormat> newFormat = format->clone().dynamicCast<Ld::TextBlockFormat>();
                    newFormat->setFirstLineLeftIndentation(newFirstLineIndentation);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::paragraphChangeListIndentation() {
    MainWindow* window                = BlockFormatViewProxy::window();
    QAction*    listIndentationAction = window->action("format_list_indentation");

    bool  ok;
    float newListIndentation = listIndentationAction->data().toFloat(&ok);

    if (ok) {
        const Ld::ListParagraphFormatBase::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ListParagraphFormatBase::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                                        elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::ListParagraphFormatBase> format = element->format()
                                                                     .dynamicCast<Ld::ListParagraphFormatBase>();

                if (format->listIndentation() != newListIndentation) {
                    QSharedPointer<Ld::ListParagraphFormatBase> newFormat =
                        format->clone().dynamicCast<Ld::ListParagraphFormatBase>();
                    newFormat->setListIndentation(newListIndentation);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::paragraphChangeLineSpacing() {
    MainWindow* window                     = BlockFormatViewProxy::window();
    QAction*    paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");

    bool  ok;
    float newLineSpacing = paragraphLineSpacingAction->data().toFloat(&ok);

    if (ok) {
        const Ld::TextBlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TextBlockFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::TextBlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                                elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                QSharedPointer<Ld::TextBlockFormat> format = element->format().dynamicCast<Ld::TextBlockFormat>();
                if (format->lineSpacing() != newLineSpacing) {
                    QSharedPointer<Ld::TextBlockFormat> newFormat = format->clone().dynamicCast<Ld::TextBlockFormat>();
                    newFormat->setLineSpacing(newLineSpacing);
                    formatsByElement.insert(element, newFormat);
                }
            }
        }

        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::paragraphFormatDialogRequested() {
    MainWindow* window = BlockFormatViewProxy::window();

    BlockFormatDialog blockFormatDialog(window);
    currentBlockFormatDialog = &blockFormatDialog;
    blockFormatDialog.loadSettings(QString("BlockFormatDialog"));

    connect(&blockFormatDialog, &BlockFormatDialog::apply, this, &BlockFormatViewProxy::blockFormatDialogApplyUpdates);

    updateBlockFormatDialog(&blockFormatDialog);

    int result = blockFormatDialog.exec();
    currentBlockFormatDialog = Q_NULLPTR;
    if (result == BlockFormatDialog::Accepted) {
        blockFormatDialogApplyUpdates(&blockFormatDialog);
    }
}


void BlockFormatViewProxy::blockFormatDialogApplyUpdates(BlockFormatDialog* dialog) {
    dialog->saveSettings(QString("BlockFormatDialog"));

    QSet<Ld::ElementWeakPointer> weakElements;
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::BlockFormat::formatName));
    {
        const Ld::BlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        weakElements |= aggregation;
    }

    if (currentAggregationsByCapability.contains(Ld::JustifiedBlockFormat::formatName)) {
        const Ld::JustifiedBlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::JustifiedBlockFormat>();

        weakElements |= aggregation;
    }

    if (currentAggregationsByCapability.contains(Ld::TextBlockFormat::formatName)) {
        const Ld::TextBlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TextBlockFormat>();

        weakElements |= aggregation;
    }

    if (currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName)) {
        const Ld::ListParagraphFormatBase::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

        weakElements |= aggregation;
    }

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  QSet<Ld::ElementWeakPointer>::const_iterator it = weakElements.constBegin(), end = weakElements.constEnd()
         ; it != end
         ; ++it
        ) {
        Ld::ElementPointer element = it->toStrongRef();
        if (!element.isNull()) {
            Ld::FormatPointer oldFormat = element->format();

            if (!oldFormat.isNull()) {
                Ld::FormatPointer        newFormat    = oldFormat->clone();
                Ld::Format::Capabilities capabilities = newFormat->capabilities();
                bool                     updated      = false;

                if (capabilities.contains(Ld::BlockFormat::formatName)) {
                    QSharedPointer<Ld::BlockFormat> format = newFormat.dynamicCast<Ld::BlockFormat>();
                    Q_ASSERT(!format.isNull());

                    if (dialog->isLeftIndentationSet()) {
                        format->setLeftIndentation(dialog->leftIndentation());
                        updated = true;
                    }

                    if (dialog->isRightIndentationSet()) {
                        format->setRightIndentation(dialog->rightIndentation());
                        updated = true;
                    }

                    if (dialog->isTopSpacingSet()) {
                        format->setTopSpacing(dialog->topSpacing());
                        updated = true;
                    }

                    if (dialog->isBottomSpacingSet()) {
                        format->setBottomSpacing(dialog->bottomSpacing());
                        updated = true;
                    }
                }

                if (capabilities.contains(Ld::JustifiedBlockFormat::formatName)) {
                    QSharedPointer<Ld::JustifiedBlockFormat> format = newFormat.dynamicCast<Ld::JustifiedBlockFormat>();
                    Q_ASSERT(!format.isNull());

                    if (dialog->justification() != Ld::JustifiedBlockFormat::Justification::NONE) {
                        format->setJustification(dialog->justification());
                        updated = true;
                    }
                }

                if (capabilities.contains(Ld::TextBlockFormat::formatName)) {
                    QSharedPointer<Ld::TextBlockFormat> format = newFormat.dynamicCast<Ld::TextBlockFormat>();
                    Q_ASSERT(!format.isNull());

                    if (dialog->isFirstLineIndentationSet()) {
                        format->setFirstLineLeftIndentation(dialog->firstLineIndentation());
                        updated = true;
                    }

                    if (dialog->isLineSpacingSet()) {
                        format->setLineSpacing(dialog->lineSpacing());
                        updated = true;
                    }
                }

                if (capabilities.contains(Ld::ListParagraphFormatBase::formatName)) {
                    QSharedPointer<Ld::ListParagraphFormatBase> format = newFormat
                                                                         .dynamicCast<Ld::ListParagraphFormatBase>();
                    Q_ASSERT(!format.isNull());

                    if (dialog->isListIndentationSet()) {
                        format->setListIndentation(dialog->listIndentation());
                        updated = true;
                    }
                }

                if (updated) {
                    formatsByElement.insert(element, newFormat);
                }
            }
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void BlockFormatViewProxy::horizontalDockBulletNumberStyleButtonClicked() {
    EQt::ToolButton* horizontalListBulletNumberToolButton =
        window()->widget<EQt::ToolButton>("horizontal_list_bullet_number_style_tool_button");

    QPoint buttonBottomLeft  = horizontalListBulletNumberToolButton->rect().bottomLeft();
    QPoint buttonBottomRight = horizontalListBulletNumberToolButton->rect().bottomRight();
    QPoint buttonBottom      = QLine(buttonBottomLeft, buttonBottomRight).center();

    QPoint dialogPosition = horizontalListBulletNumberToolButton->mapToGlobal(buttonBottom);

    displayBulletNumberPopupDialog(dialogPosition);
}


void BlockFormatViewProxy::verticalDockBulletNumberStyleButtonClicked() {
    EQt::ToolButton* verticalListBulletNumberToolButton =
        window()->widget<EQt::ToolButton>("vertical_list_bullet_number_style_tool_button");

    QPoint buttonBottomLeft  = verticalListBulletNumberToolButton->rect().bottomLeft();
    QPoint buttonBottomRight = verticalListBulletNumberToolButton->rect().bottomRight();
    QPoint buttonBottom      = QLine(buttonBottomLeft, buttonBottomRight).center();

    QPoint dialogPosition = verticalListBulletNumberToolButton->mapToGlobal(buttonBottom);

    displayBulletNumberPopupDialog(dialogPosition);
}


void BlockFormatViewProxy::newBulletStyleSelected(const QString& newBulletStyle) {
    UnorderedListStyleFormatUpdater       formatUpdater(newBulletStyle);
    UpdateFormatCommand::FormatsByElement formatsByElement = formatUpdater.update(currentAggregationsByCapability);

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    bulletNumberPopupDialog->hide();
    restoreFocus();
}


void BlockFormatViewProxy::newNumberStyleSelected(Ld::OrderedListParagraphFormat::Style newNumberStyle) {
    OrderedListStyleFormatUpdater         formatUpdater(newNumberStyle);
    UpdateFormatCommand::FormatsByElement formatsByElement = formatUpdater.update(currentAggregationsByCapability);

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    bulletNumberPopupDialog->hide();
    restoreFocus();
}


void BlockFormatViewProxy::removeListFormatting() {
    RemoveListFormatUpdater               formatUpdater;
    UpdateFormatCommand::FormatsByElement formatsByElement = formatUpdater.update(currentAggregationsByCapability);

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    bulletNumberPopupDialog->hide();
    restoreFocus();
}


void BlockFormatViewProxy::noNewBulletStyleSelected() {
    bulletNumberPopupDialog->hide();
    restoreFocus();
}


void BlockFormatViewProxy::showBulletNumberRequested() {
    updateBulletNumberDisplayState(true);
}


void BlockFormatViewProxy::hideBulletNumberRequested() {
    updateBulletNumberDisplayState(false);
}


void BlockFormatViewProxy::listFontFormatDialogRequested() {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName));

    MainWindow* window = BlockFormatViewProxy::window();

    CharacterFormatDialog characterFormatDialog(window);
    currentListFontFormatDialog = &characterFormatDialog;

    characterFormatDialog.loadSettings(QString("ListFontFormatDialog"));

    connect(
        &characterFormatDialog,
        &CharacterFormatDialog::apply,
        this,
        &BlockFormatViewProxy::listFontFormatDialogApplyUpdates
    );

    updateListFontFormatDialog(&characterFormatDialog);

    int result = characterFormatDialog.exec();
    currentListFontFormatDialog = Q_NULLPTR;

    if (result == CharacterFormatDialog::Accepted) {
        listFontFormatDialogApplyUpdates(&characterFormatDialog);
    }
}


void BlockFormatViewProxy::listFontFormatDialogApplyUpdates(CharacterFormatDialog* dialog) {
    dialog->saveSettings(QString("ListFontFormatDialog"));

    const Ld::ListParagraphFormatBase::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ListParagraphFormatBase::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                                    elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer                          element   = elementIterator->toStrongRef();
        QSharedPointer<Ld::ListParagraphFormatBase> oldFormat = element->format()
                                                                .dynamicCast<Ld::ListParagraphFormatBase>();

        QSharedPointer<Ld::ListParagraphFormatBase> newFormat;

        QString fontFamily = dialog->fontFamily();
        if (!fontFamily.isEmpty() && oldFormat->family() != fontFamily) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setFamily(fontFamily);
        }

        unsigned fontSize = dialog->fontSize();
        if (fontSize >= Ld::FontFormat::minimumFontSize && oldFormat->fontSize() != fontSize) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setFontSize(fontSize);
        }

        Ld::FontFormat::Weight weight = dialog->fontWeight();
        if (weight != Ld::FontFormat::Weight::INVALID && oldFormat->fontWeight() != weight) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setFontWeight(weight);
        }

        if (!dialog->italicsBothEnabledAndDisabled() && oldFormat->italics() != dialog->italicsEnabled()) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setItalics(dialog->italicsEnabled());
        }

        if (!dialog->underlineBothEnabledAndDisabled() && oldFormat->underline() != dialog->underlineEnabled()) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setUnderline(dialog->underlineEnabled());
        }

        if (!dialog->overlineBothEnabledAndDisabled() && oldFormat->overline() != dialog->overlineEnabled()) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setOverline(dialog->overlineEnabled());
        }

        if (!dialog->strikethroughBothEnabledAndDisabled()            &&
            oldFormat->strikeout() != dialog->strikethroughEnabled()    ) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setStrikeout(dialog->strikethroughEnabled());
        }

        if (dialog->fontColorSelected() && oldFormat->fontColor() != dialog->fontColor()) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setFontColor(dialog->fontColor());
        }

        if (dialog->fontBackgroundColorSelected()                              &&
            oldFormat->fontBackgroundColor() != dialog->fontBackgroundColor()    ) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setFontBackgroundColor(dialog->fontBackgroundColor());
        }

        if (dialog->letterSpacingValid() && oldFormat->letterSpacing() != dialog->letterSpacing()) {
            if (newFormat.isNull()) {
                newFormat = oldFormat->clone().dynamicCast<Ld::ListParagraphFormatBase>();
            }

            newFormat->setLetterSpacing(dialog->letterSpacing());
        }

        if (!newFormat.isNull()) {
            if (newFormat->capabilities().contains(Ld::UnorderedListParagraphFormat::formatName)) {
                QSharedPointer<Ld::UnorderedListParagraphFormat> newBulletListFormat =
                    newFormat.dynamicCast<Ld::UnorderedListParagraphFormat>();

                QFontMetrics fontMetrics(newFormat->toQFont());
                if (!fontMetrics.inFont(newBulletListFormat->bulletStyle()[0])) {
                    newBulletListFormat->setBulletStyle(Ld::UnorderedListParagraphFormat::fallbackBulletStyle);
                }
            }

            formatsByElement.insert(element, newFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void BlockFormatViewProxy::listFormatDialogRequested() {
    ListFormatDialog listFormatDialog(window());

    currentListFormatDialog = &listFormatDialog;

    listFormatDialog.loadSettings(QString("ListFormatDialog"));
    connect(&listFormatDialog, &ListFormatDialog::apply, this ,&BlockFormatViewProxy::listFormatDialogApplyUpdates);

    updateListFormatDialog(&listFormatDialog);

    int result = listFormatDialog.exec();
    currentListFormatDialog = Q_NULLPTR;

    if (result == ListFormatDialog::Accepted) {
        listFormatDialogApplyUpdates(&listFormatDialog);
    }
}


void BlockFormatViewProxy::listFormatDialogApplyUpdates(ListFormatDialog* dialog) {
    dialog->saveSettings(QString("ListFormatDialog"));

    UpdateFormatCommand::FormatsByElement formatsByElement;

    if (dialog->selectedBulletStyle().isNull()                                  &&
        dialog->selectedNumberStyle() == ListFormatDialog::NumberStyle::INVALID    ) {
        RemoveListFormatUpdater updater;
        formatsByElement = updater.update(currentAggregationsByCapability);
    } else {
        FullListFormatUpdater updater;

        bool updateFonts = currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName);
        updater.setFontUpdatesEnabled(updateFonts);

        ListFormatDialog::CheckState showBulletOrNumberCheckState = dialog->showBulletOrNumberCheckState();
        if (dialog->showBulletOrNumberCheckBoxEnabled()                                     &&
            showBulletOrNumberCheckState != ListFormatDialog::CheckState::PARTIALLY_CHECKED    ) {
            bool nowChecked = (showBulletOrNumberCheckState == ListFormatDialog::CheckState::CHECKED);
            updater.setShowBulletOrNumberState(
                nowChecked ? FullListFormatUpdater::TriState::TRUE : FullListFormatUpdater::TriState::FALSE
            );
        } else {
            updater.setShowBulletOrNumberState(FullListFormatUpdater::TriState::DO_NOT_CHANGE);
        }

        if (dialog->selectedBulletStyle().isEmpty()) {
            if (dialog->selectedNumberStyle() == ListFormatDialog::NumberStyle::INVALID) {
                Q_ASSERT(false); // We should never get here due to the test above.
            } else {
                updater.setNumberStyle(dialog->selectedNumberStyle());
            }
        } else {
            if (dialog->selectedNumberStyle() == ListFormatDialog::NumberStyle::INVALID) {
                updater.setBulletStyle(dialog->selectedBulletStyle());
            } else {
                updater.clearBulletAndNumberStyles();
            }
        }

        if (dialog->isStartingNumberSet()) {
            updater.setStartingNumber(dialog->startingNumber());
        } else {
            updater.clearStartingNumber();
        }

        if (dialog->isPrefixStringSet()) {
            updater.setPrefixString(dialog->prefixString());
        } else {
            updater.clearPrefixString();
        }

        if (dialog->isSuffixStringSet()) {
            updater.setSuffixString(dialog->suffixString());
        } else {
            updater.clearSuffixString();
        }

        if (updateFonts) {
            if (!dialog->fontFamily().isEmpty()) {
                updater.setFontFamily(dialog->fontFamily());
            } else {
                updater.clearFontFamily();
            }

            if (dialog->fontSize() != 0) {
                updater.setFontSize(dialog->fontSize());
            } else {
                updater.clearFontSize();
            }

            if (dialog->fontWeight() != Ld::FontFormat::Weight::INVALID) {
                updater.setFontWeight(dialog->fontWeight());
            } else {
                updater.clearFontWeight();
            }

            if (!dialog->italicsBothEnabledAndDisabled()) {
                updater.setItalics(
                      dialog->italicsEnabled()
                    ? FullListFormatUpdater::TriState::TRUE
                    : FullListFormatUpdater::TriState::FALSE
                );
            } else {
                updater.setItalics(FullListFormatUpdater::TriState::DO_NOT_CHANGE);
            }

            if (!dialog->underlineBothEnabledAndDisabled()) {
                updater.setUnderline(
                      dialog->underlineEnabled()
                    ? FullListFormatUpdater::TriState::TRUE
                    : FullListFormatUpdater::TriState::FALSE
                );
            } else {
                updater.setUnderline(FullListFormatUpdater::TriState::DO_NOT_CHANGE);
            }

            if (!dialog->overlineBothEnabledAndDisabled()) {
                updater.setOverline(
                      dialog->overlineEnabled()
                    ? FullListFormatUpdater::TriState::TRUE
                    : FullListFormatUpdater::TriState::FALSE
                );
            } else {
                updater.setOverline(FullListFormatUpdater::TriState::DO_NOT_CHANGE);
            }

            if (!dialog->strikethroughBothEnabledAndDisabled()) {
                updater.setStrikethrough(
                      dialog->strikethroughEnabled()
                    ? FullListFormatUpdater::TriState::TRUE
                    : FullListFormatUpdater::TriState::FALSE
                );
            } else {
                updater.setStrikethrough(FullListFormatUpdater::TriState::DO_NOT_CHANGE);
            }

            if (dialog->fontColorSelected()) {
                updater.setFontBackgroundColor(dialog->fontColor());
            } else {
                updater.clearFontColor();
            }

            if (dialog->fontBackgroundColorSelected()) {
                updater.setFontBackgroundColor(dialog->fontColor());
            } else {
                updater.clearFontBackgroundColor();
            }

            if (dialog->letterSpacingValid()) {
                updater.setLetterSpacing(dialog->letterSpacing());
            } else {
                updater.clearLetterSpacing();
            }
        }

        formatsByElement = updater.update(currentAggregationsByCapability);
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }

    restoreFocus();
}


void BlockFormatViewProxy::listStartingNumberChanged() {
    QAction* listStartingNumberAction = window()->action("format_list_starting_number_action");
    QVariant variant = listStartingNumberAction->data();

    bool     ok;
    unsigned newStartingNumber = variant.toUInt(&ok);

    if (!ok) {
        QMessageBox::warning(
            window(),
            tr("Invalid Starting Number"),
            tr("Encountered invalid starting number \"%1\"").arg(variant.toString())
        );
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        const Ld::OrderedListParagraphFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::OrderedListParagraphFormat>();

        for (  Ld::OrderedListParagraphFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                                           end = aggregation.constEnd()
             ; it != end
             ; ++it
            ) {
            Ld::ElementPointer                             element = *it;
            QSharedPointer<Ld::OrderedListParagraphFormat> format  =
                element->format().dynamicCast<Ld::OrderedListParagraphFormat>();
            Q_ASSERT(!format.isNull());

            if (format->startingNumber() != newStartingNumber) {
                QSharedPointer<Ld::OrderedListParagraphFormat> newFormat =
                    format->clone().dynamicCast<Ld::OrderedListParagraphFormat>();

                newFormat->setStartingNumber(newStartingNumber);
                formatsByElement.insert(element, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void BlockFormatViewProxy::listPrefixChanged() {
    QAction* listNumberPrefixAction = window()->action("format_list_prefix_action");
    QString  newPrefix              = listNumberPrefixAction->data().toString();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    const Ld::OrderedListParagraphFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::OrderedListParagraphFormat>();

    for (  Ld::OrderedListParagraphFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                                       end = aggregation.constEnd()
         ; it != end
         ; ++it
        ) {
        Ld::ElementPointer                             element = *it;
        QSharedPointer<Ld::OrderedListParagraphFormat> format  =
            element->format().dynamicCast<Ld::OrderedListParagraphFormat>();
        Q_ASSERT(!format.isNull());

        if (format->prefix() != newPrefix) {
            QSharedPointer<Ld::OrderedListParagraphFormat> newFormat =
                format->clone().dynamicCast<Ld::OrderedListParagraphFormat>();

            newFormat->setPrefix(newPrefix);
            formatsByElement.insert(element, newFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void BlockFormatViewProxy::listSuffixChanged() {
    QAction* listNumberSuffixAction = window()->action("format_list_suffix_action");
    QString  newSuffix              = listNumberSuffixAction->data().toString();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    const Ld::OrderedListParagraphFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::OrderedListParagraphFormat>();

    for (  Ld::OrderedListParagraphFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                                       end = aggregation.constEnd()
         ; it != end
         ; ++it
        ) {
        Ld::ElementPointer                             element = *it;
        QSharedPointer<Ld::OrderedListParagraphFormat> format  =
            element->format().dynamicCast<Ld::OrderedListParagraphFormat>();
        Q_ASSERT(!format.isNull());

        if (format->suffix() != newSuffix) {
            QSharedPointer<Ld::OrderedListParagraphFormat> newFormat =
                format->clone().dynamicCast<Ld::OrderedListParagraphFormat>();

            newFormat->setSuffix(newSuffix);
            formatsByElement.insert(element, newFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void BlockFormatViewProxy::updateJustification(Ld::JustifiedBlockFormat::Justification newJustification) {
    const Ld::JustifiedBlockFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::JustifiedBlockFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::JustifiedBlockFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin(),
                                                                 elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = elementIterator->toStrongRef();
        if (!element.isNull()) {
            QSharedPointer<Ld::JustifiedBlockFormat> format = element->format().dynamicCast<Ld::JustifiedBlockFormat>();
            if (format->justificationMode() != newJustification) {
                QSharedPointer<Ld::JustifiedBlockFormat> newFormat = format->clone()
                                                                     .dynamicCast<Ld::JustifiedBlockFormat>();
                newFormat->setJustification(newJustification);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    issueUpdateFormatCommand(formatsByElement);
}


QSet<QSharedPointer<Ld::ParagraphElement>> BlockFormatViewProxy::listTopParagraphs() {
    Ld::ElementPointerSet elements;
    if (currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName)) {
        const Ld::ListParagraphFormatBase::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

        for (  QSet<Ld::ElementWeakPointer>::const_iterator weakElementIterator    = aggregation.constBegin(),
                                                            weakElementEndIterator = aggregation.constEnd()
             ; weakElementIterator != weakElementEndIterator
             ; ++weakElementIterator
            ) {
            Ld::ElementPointer element = weakElementIterator->toStrongRef();
            if (!element.isNull() && element->typeName() == Ld::ParagraphElement::elementName) {
                elements.insert(element);
            }
        }
    }

    if (currentAggregationsByCapability.contains(Ld::ListAdditionalParagraphFormat::formatName)) {
        const Ld::ListAdditionalParagraphFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ListAdditionalParagraphFormat>();

        for (  QSet<Ld::ElementWeakPointer>::const_iterator weakElementIterator    = aggregation.constBegin(),
                                                            weakElementEndIterator = aggregation.constEnd()
             ; weakElementIterator != weakElementEndIterator
             ; ++weakElementIterator
            ) {
            Ld::ElementPointer element = weakElementIterator->toStrongRef();
            if (!element.isNull() && element->typeName() == Ld::ParagraphElement::elementName) {
                elements.insert(element);
            }
        }
    }

    QSet<QSharedPointer<Ld::ParagraphElement>> result;
    for (  Ld::ElementPointerSet::const_iterator elementIterator    = elements.constBegin(),
                                                 elementEndIterator = elements.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        Ld::ElementPointer sibling = element->previousSibling();

        if (!element->format().isNull() &&
            element->format()->capabilities().contains(Ld::ListParagraphFormatBase::formatName)    ) {
            result.insert(element.dynamicCast<Ld::ParagraphElement>());
        } else {
            if (!sibling.isNull() && !elements.contains(sibling)) {
                while (!sibling.isNull()                                                                         &&
                       sibling->typeName() == Ld::ParagraphElement::elementName                                  &&
                       !sibling->format().isNull()                                                               &&
                       sibling->format()->capabilities().contains(Ld::ListAdditionalParagraphFormat::formatName)    ) {
                    sibling = sibling->previousSibling();
                }

                if (!sibling.isNull()                                                                   &&
                    sibling->typeName() == Ld::ParagraphElement::elementName                            &&
                    !sibling->format().isNull()                                                         &&
                    sibling->format()->capabilities().contains(Ld::ListParagraphFormatBase::formatName)    ) {
                    result.insert(sibling.dynamicCast<Ld::ParagraphElement>());
                }
            }
        }
    }

    return result;
}


void BlockFormatViewProxy::updateBlockFormatDockState(
        const Ld::AggregationsByCapability& aggregationsByCapability
    ) {
    currentAggregationsByCapability = aggregationsByCapability;

    MainWindow* window = BlockFormatViewProxy::window();

    /* Block formats */

    QAction* paragraphLeftAlignAction               = window->action("format_left_align_text");
    QAction* paragraphCenterAction                  = window->action("format_center_text");
    QAction* paragraphRightAlignAction              = window->action("format_right_align_text");
    QAction* paragraphJustifyAction                 = window->action("format_justify_text");
    QAction* paragraphDecreaseIndentAction          = window->action("format_paragraph_decrease_indent");
    QAction* paragraphIncreaseIndentAction          = window->action("format_paragraph_increase_indent");
    QAction* paragraphLeftIndentationAction         = window->action("format_paragraph_left_indentation");
    QAction* paragraphRightIndentationAction        = window->action("format_paragraph_right_indentation");
    QAction* paragraphTopSpacingAction              = window->action("format_paragraph_top_spacing");
    QAction* paragraphBottomSpacingAction           = window->action("format_paragraph_bottom_spacing");
    QAction* paragraphFirstLineIndentationAction    = window->action("format_paragraph_first_line_indentation");
    QAction* listIndentationAction                  = window->action("format_list_indentation");
    QAction* paragraphLineSpacingAction             = window->action("format_paragraph_line_spacing");
    QAction* paragraphDialogAction                  = window->action("format_block_dialog");

//    QAction* listBulletNumberStyleAction         = window->action("format_list_bullet_number_style_action");
    QAction* showHideBulletNumberAction             = window->action("format_list_show_hide_bullet_numbers");
    QAction* showListBulletNumberAction             = window->action("show_list_bullet_number_action");
    QAction* hideListBulletNumberAction             = window->action("hide_list_bullet_number_action");
    QAction* listBulletNumberFontFormatDialogAction = window->action("list_bullet_number_font_format_dialog_action");
//    QAction* formatListDialogAction                 = window->action("format_list_dialog");
    QAction* listStartingNumberAction               = window->action("format_list_starting_number_action");
    QAction* listNumberPrefixAction                 = window->action("format_list_prefix_action");
    QAction* listNumberSuffixAction                 = window->action("format_list_suffix_action");

    if (aggregationsByCapability.contains(Ld::BlockFormat::formatName)) {
        const Ld::BlockFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        paragraphIncreaseIndentAction->setEnabled(true);
        paragraphLeftIndentationAction->setEnabled(true);
        paragraphRightIndentationAction->setEnabled(true);
        paragraphTopSpacingAction->setEnabled(true);
        paragraphBottomSpacingAction->setEnabled(true);

        QSet<float> leftIndentations = aggregation.leftIndentations();
        if (leftIndentations.count() == 1) {
            float leftIndentation = *aggregation.leftIndentations().begin();
            paragraphLeftIndentationAction->setData(QVariant::fromValue(leftIndentation));
            paragraphDecreaseIndentAction->setEnabled(leftIndentation > 0);
        } else {
            paragraphLeftIndentationAction->setData(QVariant());

            bool enableDecreaseIndentation = false;
            QSet<float>::const_iterator it  = leftIndentations.constBegin();
            QSet<float>::const_iterator end = leftIndentations.constEnd();
            while (it != end&& !enableDecreaseIndentation) {
                enableDecreaseIndentation = (*it > 0);
                ++it;
            }

            paragraphDecreaseIndentAction->setEnabled(enableDecreaseIndentation);
        }

        paragraphLeftIndentationAction->trigger();

        if (aggregation.rightIndentations().count() == 1) {
            float rightIndentation = *aggregation.rightIndentations().begin();
            paragraphRightIndentationAction->setData(QVariant::fromValue(rightIndentation));
        } else {
            paragraphRightIndentationAction->setData(QVariant());
        }
        paragraphRightIndentationAction->trigger();

        if (aggregation.topSpacings().count() == 1) {
            float topSpacing = *aggregation.topSpacings().begin();
            paragraphTopSpacingAction->setData(QVariant::fromValue(topSpacing));
        } else {
            paragraphTopSpacingAction->setData(QVariant());
        }
        paragraphTopSpacingAction->trigger();

        if (aggregation.bottomSpacings().count() == 1) {
            float bottomSpacing = *aggregation.bottomSpacings().begin();
            paragraphBottomSpacingAction->setData(QVariant::fromValue(bottomSpacing));
        } else {
            paragraphBottomSpacingAction->setData(QVariant());
        }
        paragraphBottomSpacingAction->trigger();

        paragraphDialogAction->setEnabled(true);
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
        paragraphDialogAction->setEnabled(false);
    }

    if (aggregationsByCapability.contains(Ld::JustifiedBlockFormat::formatName)) {
        const Ld::JustifiedBlockFormat::Aggregation&
            aggregation = aggregationsByCapability.aggregationForFormat<Ld::JustifiedBlockFormat>();

        paragraphLeftAlignAction->setEnabled(
            aggregation.fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::LEFT)
        );

        paragraphRightAlignAction->setEnabled(
            aggregation.fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::RIGHT)
        );

        paragraphCenterAction->setEnabled(
            aggregation.fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::CENTER)
        );

        paragraphJustifyAction->setEnabled(
            aggregation.fullySupportedJustifications().contains(Ld::JustifiedBlockFormat::Justification::JUSTIFY)
        );

        QSet<Ld::JustifiedBlockFormat::Justification> justifications = aggregation.justifications();
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

    if (aggregationsByCapability.contains(Ld::TextBlockFormat::formatName)) {
        const Ld::TextBlockFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::TextBlockFormat>();

        if (aggregation.firstLineLeftIndentations().count() == 1) {
            float firstLineLeftIndentation = *aggregation.firstLineLeftIndentations().begin();
            paragraphFirstLineIndentationAction->setData(QVariant::fromValue(firstLineLeftIndentation));
        } else {
            paragraphFirstLineIndentationAction->setData(QVariant());
        }

        if (aggregation.lineSpacings().count() == 1) {
            float lineSpacing = *aggregation.lineSpacings().begin();
            paragraphLineSpacingAction->setData(QVariant::fromValue(lineSpacing));
        } else {
            paragraphLineSpacingAction->setData(QVariant());
        }

        paragraphFirstLineIndentationAction->trigger();
        paragraphLineSpacingAction->trigger();

        paragraphFirstLineIndentationAction->setEnabled(true);
        paragraphLineSpacingAction->setEnabled(true);
    } else {
        paragraphFirstLineIndentationAction->setData(QVariant());
        paragraphLineSpacingAction->setData(QVariant());

        paragraphFirstLineIndentationAction->trigger();
        paragraphLineSpacingAction->trigger();

        paragraphFirstLineIndentationAction->setEnabled(false);
        paragraphLineSpacingAction->setEnabled(false);
    }

    if (aggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName)) {
        const Ld::ListParagraphFormatBase::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

        if (aggregation.listIndentations().count() == 1) {
            listIndentationAction->setData(QVariant::fromValue(*aggregation.listIndentations().begin()));
        } else {
            listIndentationAction->setData(QVariant());
        }

        listIndentationAction->trigger();

        listIndentationAction->setEnabled(true);

        listBulletNumberFontFormatDialogAction->setEnabled(true);
    } else {
        listIndentationAction->setData(QVariant());
        listIndentationAction->trigger();

        listIndentationAction->setEnabled(false);

        listBulletNumberFontFormatDialogAction->setEnabled(false);
    }

    if (aggregationsByCapability.contains(Ld::OrderedListParagraphFormat::formatName)) {
        const Ld::OrderedListParagraphFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::OrderedListParagraphFormat>();

        listStartingNumberAction->setEnabled(true);
        listNumberPrefixAction->setEnabled(true);
        listNumberSuffixAction->setEnabled(true);

        if (aggregation.startingNumbers().count() == 1) {
            listStartingNumberAction->setData(QVariant::fromValue(*aggregation.startingNumbers().begin()));
        } else {
            listStartingNumberAction->setData(QVariant());
        }

        if (aggregation.prefixes().count() == 1) {
            listNumberPrefixAction->setData(QVariant::fromValue(*aggregation.prefixes().begin()));
        } else {
            listNumberPrefixAction->setData(QVariant());
        }

        if (aggregation.suffixes().count() == 1) {
            listNumberSuffixAction->setData(QVariant::fromValue(*aggregation.suffixes().begin()));
        } else {
            listNumberSuffixAction->setData(QVariant());
        }

        listStartingNumberAction->trigger();
        listNumberPrefixAction->trigger();
        listNumberSuffixAction->trigger();
    } else {
        listStartingNumberAction->setData(QVariant());
        listNumberPrefixAction->setData(QVariant());
        listNumberSuffixAction->setData(QVariant());

        listStartingNumberAction->trigger();
        listNumberPrefixAction->trigger();
        listNumberSuffixAction->trigger();

        listStartingNumberAction->setEnabled(false);
        listNumberPrefixAction->setEnabled(false);
        listNumberSuffixAction->setEnabled(false);
    }

    if (aggregationsByCapability.contains(Ld::ListAdditionalParagraphFormat::formatName)) {
        const Ld::ListAdditionalParagraphFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::ListAdditionalParagraphFormat>();

        showHideBulletNumberAction->setEnabled(true);

        showListBulletNumberAction->setChecked(aggregation.listBulletNumberStates().includesTrue());
        hideListBulletNumberAction->setChecked(aggregation.listBulletNumberStates().includesFalse());
    } else {
        showHideBulletNumberAction->setEnabled(false);
    }
}


void BlockFormatViewProxy::displayBulletNumberPopupDialog(const QPoint& position) {
    unsigned dialogWidth  = static_cast<unsigned>(bulletNumberPopupDialog->width());
    unsigned dialogHeight = static_cast<unsigned>(bulletNumberPopupDialog->height());

    int      rightEdge   = position.x() + dialogWidth / 2;
    int      leftEdge    = rightEdge - dialogWidth;
    int      topEdge     = position.y();
    int      bottomEdge  = position.y() + dialogHeight;

    QScreen* screen = Application::screenAt(position);
    if (screen == Q_NULLPTR) {
        Q_ASSERT(!Application::screens().isEmpty());
        screen = Application::screens().at(0);
    }

    QRect screenRectangle = screen->geometry();

    if (topEdge < screenRectangle.top()) {
        topEdge     = screenRectangle.top();
        bottomEdge = topEdge + dialogHeight;
    } else if (bottomEdge > screenRectangle.bottom()) {
        bottomEdge = screenRectangle.bottom();
        topEdge    = bottomEdge - dialogHeight;
    }

    if (leftEdge < screenRectangle.left()) {
        leftEdge = screenRectangle.left();
        rightEdge = leftEdge + dialogWidth;
    } else if (rightEdge > screenRectangle.right()) {
        rightEdge = screenRectangle.right();
        leftEdge  = rightEdge - dialogWidth;
    }
    if (leftEdge < 0) {
        leftEdge = 0;
    }

    QSet<QString>                              bulletStyles;
    QSet<BulletNumberPopupDialog::NumberStyle> numberStyles;
    bool                                       showRemoveButton = false;

    Ld::FormatPointerSet formats;

    if (currentAggregationsByCapability.contains(Ld::UnorderedListParagraphFormat::formatName)) {
        const Ld::UnorderedListParagraphFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::UnorderedListParagraphFormat>();

        bulletStyles     = aggregation.bulletStyles();
        showRemoveButton = true;

        formats += aggregation.formats();
    }

    if (currentAggregationsByCapability.contains(Ld::OrderedListParagraphFormat::formatName)) {
        const Ld::OrderedListParagraphFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::OrderedListParagraphFormat>();

        numberStyles     = aggregation.styles();
        showRemoveButton = true;

        formats += aggregation.formats();
    }

    if (currentAggregationsByCapability.contains(Ld::ListAdditionalParagraphFormat::formatName)) {
        showRemoveButton = true;
    }

    QSet<QFont> fonts;
    for (  Ld::FormatPointerSet::const_iterator formatIterator    = formats.constBegin(),
                                                formatEndIterator = formats.constEnd()
         ; formatIterator != formatEndIterator
         ; ++formatIterator
        ) {
        QSharedPointer<Ld::ListParagraphFormatBase> listFormat =
            formatIterator->dynamicCast<Ld::ListParagraphFormatBase>();

        if (!listFormat.isNull()) {
            fonts << listFormat->toQFont();
        }
    }

    if (fonts.count() == 1) {
        bulletNumberPopupDialog->setListFont(*fonts.begin());
    } else {
        bulletNumberPopupDialog->setListFont(Application::font());
    }

    bulletNumberPopupDialog->setSelectedBulletStyles(bulletStyles);
    bulletNumberPopupDialog->setSelectedNumberStyles(numberStyles);
    bulletNumberPopupDialog->setRemoveListStyleButtonVisible(showRemoveButton);

    bulletNumberPopupDialog->move(leftEdge, topEdge);
    bulletNumberPopupDialog->show();
    bulletNumberPopupDialog->raise();
    bulletNumberPopupDialog->setFocus(Qt::ActiveWindowFocusReason);
}


void BlockFormatViewProxy::connectSignals() {
    MainWindow* window = BlockFormatViewProxy::window();
    ViewWidget* view   = BlockFormatViewProxy::view();

    sessionConnect(view, &ViewWidget::formatsAtCursorChanged, &BlockFormatViewProxy::formatsUnderCursorChanged);

    QAction* paragraphLeftAlignAction = window->action("format_left_align_text");
    sessionConnect(paragraphLeftAlignAction, &QAction::triggered, &BlockFormatViewProxy::paragraphLeftAlign);

    QAction* paragraphCenterAction = window->action("format_center_text");
    sessionConnect(paragraphCenterAction, &QAction::triggered, &BlockFormatViewProxy::paragraphCenter);

    QAction* paragraphRightAlignAction = window->action("format_right_align_text");
    sessionConnect(paragraphRightAlignAction, &QAction::triggered, &BlockFormatViewProxy::paragraphRightAlign);

    QAction* paragraphJustifyAction = window->action("format_justify_text");
    sessionConnect(paragraphJustifyAction, &QAction::triggered, &BlockFormatViewProxy::paragraphJustify);

    QAction* paragraphDecreaseIndentAction = window->action("format_paragraph_decrease_indent");
    sessionConnect(paragraphDecreaseIndentAction, &QAction::triggered, &BlockFormatViewProxy::paragraphDecreaseIndent);

    QAction* paragraphIncreaseIndentAction = window->action("format_paragraph_increase_indent");
    sessionConnect(paragraphIncreaseIndentAction, &QAction::triggered, &BlockFormatViewProxy::paragraphIncreaseIndent);

    QAction* paragraphLeftIndentationAction = window->action("format_paragraph_left_indentation");
    sessionConnect(
        paragraphLeftIndentationAction,
        &QAction::triggered,
        &BlockFormatViewProxy::paragraphChangeLeftIndentation
    );

    QAction* paragraphRightIndentationAction = window->action("format_paragraph_right_indentation");
    sessionConnect(
        paragraphRightIndentationAction,
        &QAction::triggered,
        &BlockFormatViewProxy::paragraphChangeRightIndentation
    );

    QAction* paragraphTopSpacingAction = window->action("format_paragraph_top_spacing");
    sessionConnect(paragraphTopSpacingAction, &QAction::triggered, &BlockFormatViewProxy::paragraphChangeTopSpacing);

    QAction* paragraphBottomSpacingAction = window->action("format_paragraph_bottom_spacing");
    sessionConnect(
        paragraphBottomSpacingAction,
        &QAction::triggered,
        &BlockFormatViewProxy::paragraphChangeBottomSpacing
    );

    QAction* paragraphFirstLineIndentationAction = window->action("format_paragraph_first_line_indentation");
    sessionConnect(
        paragraphFirstLineIndentationAction,
        &QAction::triggered,
        &BlockFormatViewProxy::paragraphChangeFirstLineIndentation
    );

    QAction* listIndentationAction = window->action("format_list_indentation");
    sessionConnect(listIndentationAction, &QAction::triggered, &BlockFormatViewProxy::paragraphChangeListIndentation);

    QAction* paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");
    sessionConnect(paragraphLineSpacingAction, &QAction::triggered, &BlockFormatViewProxy::paragraphChangeLineSpacing);

    QAction* paragraphDialogAction = window->action("format_block_dialog");
    sessionConnect(paragraphDialogAction, &QAction::triggered, &BlockFormatViewProxy::paragraphFormatDialogRequested);

//    QAction* listBulletNumberStyleAction = window->action("format_list_bullet_number_style_action");

    EQt::ToolButton* horizontalListBulletNumberToolButton =
        window->widget<EQt::ToolButton>("horizontal_list_bullet_number_style_tool_button");

    sessionConnect(
        horizontalListBulletNumberToolButton,
        &EQt::ToolButton::clicked,
        &BlockFormatViewProxy::horizontalDockBulletNumberStyleButtonClicked
    );

    EQt::ToolButton* verticalListBulletNumberToolButton =
        window->widget<EQt::ToolButton>("vertical_list_bullet_number_style_tool_button");

    sessionConnect(
        verticalListBulletNumberToolButton,
        &EQt::ToolButton::clicked,
        &BlockFormatViewProxy::verticalDockBulletNumberStyleButtonClicked
    );

    QAction* showListBulletNumberAction = window->action("show_list_bullet_number_action");
    sessionConnect(
        showListBulletNumberAction,
        &QAction::triggered,
        &BlockFormatViewProxy::showBulletNumberRequested
    );

    QAction* hideListBulletNumberAction = window->action("hide_list_bullet_number_action");
    sessionConnect(
        hideListBulletNumberAction,
        &QAction::triggered,
        &BlockFormatViewProxy::hideBulletNumberRequested
    );

    QAction* listBulletNumberFontFormatDialogAction = window->action("list_bullet_number_font_format_dialog_action");
    sessionConnect(
        listBulletNumberFontFormatDialogAction,
        &QAction::triggered,
        &BlockFormatViewProxy::listFontFormatDialogRequested
    );

    QAction* formatListDialogAction = window->action("format_list_dialog");
    sessionConnect(formatListDialogAction, &QAction::triggered, &BlockFormatViewProxy::listFormatDialogRequested);

    QAction* listStartingNumberAction = window->action("format_list_starting_number_action");
    sessionConnect(listStartingNumberAction, &QAction::triggered, &BlockFormatViewProxy::listStartingNumberChanged);

    QAction* listNumberPrefixAction = window->action("format_list_prefix_action");
    sessionConnect(listNumberPrefixAction, &QAction::triggered, &BlockFormatViewProxy::listPrefixChanged);

    QAction* listNumberSuffixAction = window->action("format_list_suffix_action");
    sessionConnect(listNumberSuffixAction, &QAction::triggered, &BlockFormatViewProxy::listSuffixChanged);
}


void BlockFormatViewProxy::disconnectSignals() {
    sessionDisconnectAll();
}


void BlockFormatViewProxy::updateBulletNumberDisplayState(bool nowShow) {
    Ld::ElementPointerSet elements = currentAggregationsByCapability.elements(
        Ld::ListAdditionalParagraphFormat::formatName
    );

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ElementPointerSet::const_iterator elementIterator    = elements.constBegin(),
                                                 elementEndIterator = elements.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer element = *elementIterator;
        if (element->typeName() == Ld::ParagraphElement::elementName) {
            QSharedPointer<Ld::ListAdditionalParagraphFormat> oldFormat =
                element->format().dynamicCast<Ld::ListAdditionalParagraphFormat>();
            Q_ASSERT(!oldFormat.isNull());

            if (oldFormat->bulletNumberDisplayEnabled() != nowShow) {
                QSharedPointer<Ld::ListAdditionalParagraphFormat> newFormat =
                    oldFormat->clone().dynamicCast<Ld::ListAdditionalParagraphFormat>();

                newFormat->setBulletNumberDisplayEnabled(nowShow);
                formatsByElement.insert(element, newFormat);
            }
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void BlockFormatViewProxy::updateBlockFormatDialog(BlockFormatDialog* dialog) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::BlockFormat::formatName));
    {
        const Ld::BlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::BlockFormat>();

        if (aggregation.leftIndentations().count() == 1) {
            dialog->setLeftIndentation(*aggregation.leftIndentations().begin());
        } else {
            dialog->clearLeftIndentation();
        }

        if (aggregation.rightIndentations().count() == 1) {
            dialog->setRightIndentation(*aggregation.rightIndentations().begin());
        } else {
            dialog->clearRightIndentation();
        }

        if (aggregation.topSpacings().count() == 1) {
            dialog->setTopSpacing(*aggregation.topSpacings().begin());
        } else {
            dialog->clearTopSpacing();
        }

        if (aggregation.bottomSpacings().count() == 1) {
            dialog->setBottomSpacing(*aggregation.bottomSpacings().begin());
        } else {
            dialog->clearBottomSpacing();
        }
    }

    if (currentAggregationsByCapability.contains(Ld::JustifiedBlockFormat::formatName)) {
        const Ld::JustifiedBlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::JustifiedBlockFormat>();

        dialog->setJustification(aggregation.justifications());
    } else {
        // Todo hide justification controls
    }

    if (currentAggregationsByCapability.contains(Ld::TextBlockFormat::formatName)) {
        const Ld::TextBlockFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TextBlockFormat>();

        if (aggregation.firstLineLeftIndentations().count() == 1) {
            dialog->setFirstLineIndentation(*aggregation.firstLineLeftIndentations().begin());
        } else {
            dialog->clearFirstLineIndentation();
        }

        if (aggregation.lineSpacings().count() == 1) {
            dialog->setLineSpacing(*aggregation.lineSpacings().begin());
        } else {
            dialog->clearLineSpacing();
        }
    } else {
        // Todo hide indentation and line spacing controls.
    }

    if (currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName)       ||
        currentAggregationsByCapability.contains(Ld::ListAdditionalParagraphFormat::formatName)    ) {
        dialog->setListSettingsVisible();

        bool showListBulletNumber = false;

        QSet<float> listIndentations;
        if (currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName)) {
            if (currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName)) {
                dialog->setListSettingsEnabled();
            } else {
                dialog->setListSettingsDisabled();
            }

            showListBulletNumber = true;

            const Ld::ListParagraphFormatBase::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

            listIndentations = aggregation.listIndentations();
        } else {
            dialog->setListSettingsDisabled();

            QSet<QSharedPointer<Ld::ParagraphElement>> listTops = listTopParagraphs();
            for (  QSet<QSharedPointer<Ld::ParagraphElement>>::const_iterator it  = listTops.constBegin(),
                                                                              end = listTops.constEnd()
                 ; it != end
                 ; ++it
                ) {
                QSharedPointer<Ld::ParagraphElement>        element = *it;
                QSharedPointer<Ld::ListParagraphFormatBase> format  = element->format()
                                                                      .dynamicCast<Ld::ListParagraphFormatBase>();
                Q_ASSERT(!format.isNull());

                listIndentations << format->listIndentation();
            }
        }

        if (listIndentations.count() == 1) {
            dialog->setListIndentation(*listIndentations.begin());
        } else {
            dialog->clearListIndentation();
        }

        if (currentAggregationsByCapability.contains(Ld::ListAdditionalParagraphFormat::formatName)) {
            const Ld::ListAdditionalParagraphFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::ListAdditionalParagraphFormat>();

            if (!showListBulletNumber) {
                showListBulletNumber = aggregation.listBulletNumberStates().includesTrue();
            }
        }

        dialog->setBulletNumberVisible(showListBulletNumber);
    } else {
        dialog->setListSettingsHidden();
        dialog->setListSettingsDisabled();
    }
}


void BlockFormatViewProxy::updateListFormatDialog(ListFormatDialog* dialog) {
    if (currentAggregationsByCapability.contains(Ld::ListParagraphFormatBase::formatName)) {
        const Ld::ListParagraphFormatBase::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

        dialog->setFontFamilies(aggregation.families());
        dialog->setFontSizes(aggregation.sizes());
        dialog->setFontWeights(aggregation.weights());
        dialog->setSupportedFontWeights(supportedFontWeights(aggregation));

        if (aggregation.italics().bothTrueAndFalse()) {
            dialog->setItalicsBothEnabledAndDisabled();
        } else {
            dialog->setItalicsEnabled(aggregation.italics().allTrue());
        }

        if (aggregation.underline().bothTrueAndFalse()) {
            dialog->setUnderlineBothEnabledAndDisabled();
        } else {
            dialog->setUnderlineEnabled(aggregation.underline().allTrue());
        }

        if (aggregation.overline().bothTrueAndFalse()) {
            dialog->setOverlineBothEnabledAndDisabled();
        } else {
            dialog->setOverlineEnabled(aggregation.overline().allTrue());
        }

        if (aggregation.strikeOut().bothTrueAndFalse()) {
            dialog->setStrikethroughBothEnabledAndDisabled();
        } else {
            dialog->setStrikethroughEnabled(aggregation.strikeOut().allTrue());
        }

        dialog->setFontColors(aggregation.fontColor());
        dialog->setFontBackgroundColors(aggregation.fontBackgroundColor());
        dialog->setLetterSpacing(aggregation.letterSpacing());
        dialog->setListFontControlsEnabled();
    } else {
        dialog->setListFontControlsDisabled();
    }

    if (currentAggregationsByCapability.contains(Ld::UnorderedListParagraphFormat::formatName)) {
        const Ld::UnorderedListParagraphFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::UnorderedListParagraphFormat>();

        dialog->setSelectedBulletStyle(aggregation.bulletStyles());
    } else {
        dialog->setSelectedBulletStyle(QSet<QString>());
    }

    if (currentAggregationsByCapability.contains(Ld::OrderedListParagraphFormat::formatName)) {
        const Ld::OrderedListParagraphFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::OrderedListParagraphFormat>();

        dialog->setSelectedNumberStyles(aggregation.styles());

        if (aggregation.startingNumbers().count() == 1) {
            dialog->setStartingNumber(*aggregation.startingNumbers().begin());
        } else {
            dialog->clearStartingNumber();
        }

        if (aggregation.prefixes().count() == 1) {
            dialog->setPrefixString(*aggregation.prefixes().begin());
        } else {
            dialog->clearPrefixString();
        }

        if (aggregation.suffixes().count() == 1) {
            dialog->setSuffixString(*aggregation.suffixes().begin());
        } else {
            dialog->clearSuffixString();
        }
    } else {
        dialog->setSelectedNumberStyles(QSet<ListFormatDialog::NumberStyle>());
        dialog->clearStartingNumber();
        dialog->clearPrefixString();
        dialog->clearSuffixString();
    }

    if (currentAggregationsByCapability.contains(Ld::ListAdditionalParagraphFormat::formatName)) {
        const Ld::ListAdditionalParagraphFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::ListAdditionalParagraphFormat>();

        if (aggregation.listBulletNumberStates().allTrue()) {
            dialog->setShowBulletOrNumberCheckState(ListFormatDialog::CheckState::CHECKED);
        } else if (aggregation.listBulletNumberStates().allFalse()) {
            dialog->setShowBulletOrNumberCheckState(ListFormatDialog::CheckState::UNCHECKED);
        } else {
            dialog->setShowBulletOrNumberCheckState(ListFormatDialog::CheckState::PARTIALLY_CHECKED);
        }

        dialog->setShowBulletOrNumberCheckBoxEnabled();
    } else {
        dialog->setShowBulletOrNumberCheckBoxDisabled();
    }
}


void BlockFormatViewProxy::updateListFontFormatDialog(CharacterFormatDialog* dialog) {
    const Ld::ListParagraphFormatBase::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::ListParagraphFormatBase>();

    dialog->setFontFamilies(aggregation.families());
    dialog->setFontSizes(aggregation.sizes());
    dialog->setFontWeights(aggregation.weights());
    dialog->setSupportedFontWeights(supportedFontWeights(aggregation));

    if (aggregation.italics().bothTrueAndFalse()) {
        dialog->setItalicsBothEnabledAndDisabled();
    } else {
        dialog->setItalicsEnabled(aggregation.italics().allTrue());
    }

    if (aggregation.underline().bothTrueAndFalse()) {
        dialog->setUnderlineBothEnabledAndDisabled();
    } else {
        dialog->setUnderlineEnabled(aggregation.underline().allTrue());
    }

    if (aggregation.overline().bothTrueAndFalse()) {
        dialog->setOverlineBothEnabledAndDisabled();
    } else {
        dialog->setOverlineEnabled(aggregation.overline().allTrue());
    }

    if (aggregation.strikeOut().bothTrueAndFalse()) {
        dialog->setStrikethroughBothEnabledAndDisabled();
    } else {
        dialog->setStrikethroughEnabled(aggregation.strikeOut().allTrue());
    }

    dialog->setFontColors(aggregation.fontColor());
    dialog->setFontBackgroundColors(aggregation.fontBackgroundColor());
    dialog->setLetterSpacing(aggregation.letterSpacing());

    dialog->setPositionControlsDisabled();
    dialog->setDemoTextMode(CharacterFormatDialog::DemoTextMode::LIST);
}


void BlockFormatViewProxy::issueUpdateFormatCommand(
        const UpdateFormatCommand::FormatsByElement& formatsByElement,
        bool                                         allowMerge
    ) {
    ViewWidget* view = BlockFormatViewProxy::view();
    view->insertCommand(new UpdateFormatCommand(formatsByElement, allowMerge));
}


void BlockFormatViewProxy::restoreFocus() {
    view()->restoreFocus();
}
