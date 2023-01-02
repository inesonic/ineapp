/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFormatViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FORMAT_VIEW_PROXY_H
#define TABLE_FORMAT_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPoint>

#include <ld_aggregations_by_capability.h>
#include <ld_table_frame_format.h>

#include "update_format_command.h"
#include "view_proxy.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
    class ColorDialog;
}

namespace Ld {
    class Element;
    class ParagraphElement;
    class TableFrameFormat;
}

class TableFormatDialog;
class TableLineStyleDialog;
class TableDefaultLineStyleDialog;

/**
 * View proxy used to manage the block formatting functions.  These functions include:
 *
 *     * Basic block format menu items
 *     * Basic block formatting dock widget functions.
 */
class TableFormatViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        TableFormatViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~TableFormatViewProxy() override;

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
         * Slot that is triggered when the user requests a new table.
         */
        void insertTable();

        /**
         * Slot that is triggered when the user requests the table format settings dialog.
         */
        void formatSettingsDialog();

        /**
         * Slot that is triggered when the table format dialog Apply button is pressed or when auto-update is enabled
         * and a change occurs within the dialog.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void tableFormatDialogApplyUpdates(TableFormatDialog* dialog);

        /**
         * Slot that is triggered when the user requests cells be merged together.
         */
        void mergeCells();

        /**
         * Slot that is triggered when the user requests a table be left aligned.
         */
        void leftAlign();

        /**
         * Slot that is triggered when the user requests a table be centered.
         */
        void center();

        /**
         * Slot that is triggered when the user requests a table be right aligned.
         */
        void rightAlign();

        /**
         * Slot that is triggered when the user requests rows be added to a table.
         */
        void addRows();

        /**
         * Slot that is triggered when the user requests columns be added to the table.
         */
        void addColumns();

        /**
         * Slot that is triggered when the user requests rows be removed from a table.
         */
        void removeRows();

        /**
         * Slot that is triggered when the user requests columns be removed from a table.
         */
        void removeColumns();

        /**
         * Slot that is triggered when the user requests that an entire table be removed.
         */
        void removeTable();

        /**
         * Slot that is triggered when the user requests a row color be updated.
         */
        void rowColorDialog();

        /**
         * Slot that is triggered when the user requests a column color be updated.
         */
        void columnColorDialog();

        /**
         * Slot that is triggered when the user requests a cell color be updated.
         */
        void cellColorDialog();

        /**
         * Slot that is triggered when the user requests the row horizontal line dialog.
         */
        void horizontalLineDialog();

        /**
         * Slot that is triggered when the horizontal line dialog Apply button is pressed.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void horizontalLineDialogApplyUpdates(TableLineStyleDialog* dialog);

        /**
         * Slot that is triggered when the user requests the column vertical line dialog.
         */
        void verticalLineDialog();

        /**
         * Slot that is triggered when the vertical line dialog Apply button is pressed.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void verticalLineDialogApplyUpdates(TableLineStyleDialog* dialog);

        /**
         * Slot that is triggered when the user wants to change the table default cell color.
         */
        void defaultCellColor();

        /**
         * Slot that is triggered when the color dialog settings need to be updates.
         *
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void tableDefaultCellColorApplyUpdates(EQt::ColorDialog* dialog);

        /**
         * Slot that is triggered when the user adjusts the table top spacing.
         */
        void topSpacing();

        /**
         * Slot that is triggered when the user adjusts the table bottom spacing.
         */
        void bottomSpacing();

        /**
         * Slot that is triggered when the user adjusts the table left indentation.
         */
        void leftIndentation();

        /**
         * Slot that is triggered when the user adjusts the table right indentation.
         */
        void rightIndentation();

        /**
         * Slot that is triggered when the user wants to change the table default line line style.
         */
        void defaultLineStyle();

        /**
         * Slot that is triggered when the table default line style Apply button is pressed or when auto-update is
         * enabled and a change occurs within the dialog.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void tableDefaultLineStyleDialogApplyUpdates(TableDefaultLineStyleDialog* dialog);

    private:
        /**
         * The default indentation amount, in points.
         */
        static constexpr float defaultIndentation = 18.0F;

        /**
         * Type used to represent table cells touched by the cursor.
         */
        typedef QMap<QSharedPointer<Ld::TableFrameElement>, QSet<Ld::TableFrameElement::CellPosition>> CellsByTable;

        /**
         * Type used to represent table rows touched by the cursor.
         */
        typedef QMap<QSharedPointer<Ld::TableFrameElement>, QSet<unsigned>> RowsByTable;

        /**
         * Type used to represent table columns touched by the cursor.
         */
        typedef QMap<QSharedPointer<Ld::TableFrameElement>, QSet<unsigned>> ColumnsByTable;

        /**
         * Method that updates the table dock state.
         *
         * \param[in] aggregationsByCapability Format aggregations by format capability.
         */
        void updateTableFormatDockState(const Ld::AggregationsByCapability& aggregationsByCapability);

        /**
         * Method that is called to connect signals.
         */
        void connectSignals();

        /**
         * Method that is called to disconnect signals.
         */
        void disconnectSignals();

        /**
         * Method that updates the justification setting of tables.
         *
         * \param[in] newJustification The new table justification setting.
         */
        void updateTableJustification(Ld::TableFrameFormat::Justification newJustification);

        /**
         * Method that updates a format instance from a table format dialog instance.
         *
         * \param[in] format            The table format instance to be updated.
         *
         * \param[in] tableFormatDialog The table format dialog to update the table format from.
         *
         * \return Returns true if the format has been changed.  Returns false if the format has not been changed.
         */
        bool updateFormatFromDialog(QSharedPointer<Ld::TableFrameFormat> format, TableFormatDialog* tableFormatDialog);

        /**
         * Method that issues an \ref UpdateFormatCommand to the view.
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
         * Map listing cell positions by table.
         */
        CellsByTable currentCellsByTable;

        /**
         * Map listing row indexes by table.
         */
        RowsByTable currentRowsByTable;

        /**
         * Map listing column indexes by table.
         */
        ColumnsByTable currentColumnsByTable;
};

#endif
