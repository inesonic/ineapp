/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BlockFormatViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BLOCK_FORMAT_VIEW_PROXY_H
#define BLOCK_FORMAT_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPoint>

#include <ld_justified_block_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_ordered_list_paragraph_format.h>

#include "page_list.h"
#include "update_format_command.h"
#include "view_widget.h"
#include "view_proxy.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

namespace Ld {
    class Element;
    class ParagraphElement;
}

class BlockFormatDialog;
class BulletNumberPopupDialog;
class CharacterFormatDialog;
class ListFormatDialog;

/**
 * View proxy used to manage the block formatting functions.  These functions include:
 *
 *     * Basic block format menu items
 *     * Basic block formatting dock widget functions.
 */
class BlockFormatViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        BlockFormatViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~BlockFormatViewProxy() override;

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
         * Slot that is triggered whenever the element stack for the current view changes.
         *
         * \param[in] aggregationsByCapabilityWithAncestors    A database of aggregations organized by capability.
         *                                                     This version includes ancestors.
         *
         * \param[in] aggregationsByCapabilityWithoutAncestors A database of aggregations organized by capability.
         *                                                     This version excludes any ancestors.
         */
        void formatsUnderCursorChanged(
            const Ld::AggregationsByCapability& aggregationsByCapabilityWithAncestors,
            const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
        );

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
         * Slot that is triggered when the user adjusts the list indentation.
         */
        void paragraphChangeListIndentation();

        /**
         * Slot that is triggered when the user adjusts the line spacing.
         */
        void paragraphChangeLineSpacing();

        /**
         * Slot that is triggered for the Format | Paragraph menu item.
         */
        void paragraphFormatDialogRequested();

        /**
         * Slot that is triggered when the block format dialog Apply button is pressed or when auto-update is enabled
         * and a change occurs within the dialog.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void blockFormatDialogApplyUpdates(BlockFormatDialog* dialog);

        /**
         * Slot that is triggered when the user clicks the horizontal dock bullet/list style button.
         */
        void horizontalDockBulletNumberStyleButtonClicked();

        /**
         * Slot that is triggered when the user clicks the vertical dock bullet/list style button.
         */
        void verticalDockBulletNumberStyleButtonClicked();

        /**
         * Slot that is triggered when the user selects a new bullet style.
         *
         * \param[in] newBulletStyle A string holding the newly selected bullet style.
         */
        void newBulletStyleSelected(const QString& newBulletStyle);

        /**
         * Slot that is triggered when the user selects a new list number style.
         *
         * \param[in] newNumberStyle The newly selected number style.
         */
        void newNumberStyleSelected(Ld::OrderedListParagraphFormat::Style newNumberStyle);

        /**
         * Slot that is triggered when list formatting should be removed.
         */
        void removeListFormatting();

        /**
         * Slot that is triggered when the user aborts the bullet style selection.
         */
        void noNewBulletStyleSelected();

        /**
         * Slot that is triggered when the user selects the show bullet/number popup menu item.
         */
        void showBulletNumberRequested();

        /**
         * Slot that is triggered when the user selects the hide bullet/number popup menu item.
         */
        void hideBulletNumberRequested();

        /**
         * Slot that is triggered when the user requests the list font format dialog.
         */
        void listFontFormatDialogRequested();

        /**
         * Slot that is triggered when the user requests that list font updates be applied.
         *
         * \param[in] dialog The dialog reporting the updates to be applied.
         */
        void listFontFormatDialogApplyUpdates(CharacterFormatDialog* dialog);

        /**
         * Slot that is triggered when the user requests the list format dialog.
         */
        void listFormatDialogRequested();

        /**
         * Slot that is triggered when the user requests that list formats updates be applied.
         *
         * \param[in] dialog The dialog reporting the updates to be applied.
         */
        void listFormatDialogApplyUpdates(ListFormatDialog* dialog);

        /**
         * Slot that is triggered when the user changes the list starting number.
         */
        void listStartingNumberChanged();

        /**
         * Slot that is triggered when the user changes the list prefix string.
         */
        void listPrefixChanged();

        /**
         * Slot that is triggered when the user changes the list suffix string.
         */
        void listSuffixChanged();

    private:
        /**
         * The default indentation amount, in points.
         */
        static constexpr float defaultIndentation = 18.0F;

        /**
         * Method that generates a set containing every element the represents the top of a list touched by the
         * current pending operation.
         *
         * \return Returns a set of Ld::ParagraphElement instances that represent the top paragraph in each touched
         *         list.
         */
        QSet<QSharedPointer<Ld::ParagraphElement>> listTopParagraphs();

        /**
         * Method that updates the block format dock state.
         *
         * \param[in] aggregationsByCapability A map of format aggregations by capability.
         */
        void updateBlockFormatDockState(const Ld::AggregationsByCapability& aggregationsByCapability);

        /**
         * Method that is called to display the bullet/number popup dialog.
         *
         * \param[in] position The position where the dialog should be displayed.
         */
        void displayBulletNumberPopupDialog(const QPoint& position);

        /**
         * Method that is called to connect signals.
         */
        void connectSignals();

        /**
         * Method that is called to disconnect signals.
         */
        void disconnectSignals();

        /**
         * Method that is used to update paragraph/block justification.
         *
         * \param[in] newJustification The new justification setting.
         */
        void updateJustification(Ld::JustifiedBlockFormat::Justification newJustification);

        /**
         * Method you can call to show or hide numbers/bullets over the current aggregation.
         *
         * \param[in] nowShow If true, bullets or numbers will be displayed.  If false, bullets or numbers will be
         *                    hidden.
         */
        void updateBulletNumberDisplayState(bool nowShow);

        /**
         * Method you can call to update the current block format dialog state.
         *
         * \param[in] dialog The dialog instance to be updated.
         */
        void updateBlockFormatDialog(BlockFormatDialog* dialog);

        /**
         * Method you can call to update a list format dialog to use new values.
         *
         * \param[in] dialog The dialog instance to be updated.
         */
        void updateListFormatDialog(ListFormatDialog* dialog);

        /**
         * Method you can call to update a list format dialog to use new values.
         *
         * \param[in] dialog The dialog instance to be updated.
         */
        void updateListFontFormatDialog(CharacterFormatDialog* dialog);

        /**
         * Method that issues an Ld::UpdateFormatCommand to the view.
         *
         * \param[in] formatsByElement A map of the formats to be updated, keyed by the element that needs updating.
         *
         * \param[in] allowMerge       If true, elements will be merged, if possible.  If false, elements will never
         *                             be merged.
         */
        void issueUpdateFormatCommand(
            const UpdateFormatCommand::FormatsByElement& formatsByElement,
            bool                                         allowMerge = false
        );

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * The most recently reported aggregations by format capability.
         */
        Ld::AggregationsByCapability currentAggregationsByCapability;

        /**
         * The bullet number popup modeless dialog.
         */
        BulletNumberPopupDialog* bulletNumberPopupDialog;

        /**
         * A ppointer to the currently active block format dialog.
         */
        BlockFormatDialog* currentBlockFormatDialog;

        /**
         * A pointer to the currently active list format dialog.
         */
        ListFormatDialog* currentListFormatDialog;

        /**
         * A pointer to the currently active list font format dialog.
         */
        CharacterFormatDialog* currentListFontFormatDialog;
};

#endif
