/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFormatViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QPoint>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QAction>
#include <QVariant>
#include <QList>
#include <QMap>
#include <QSet>
#include <QHash>

#include <algorithm>

#include <util_page_size.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_tool_button.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_double_spin_box.h>
#include <eqt_color_dialog.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_paragraph_element.h>
#include <ld_paragraph_format.h>
#include <ld_text_element.h>
#include <ld_character_format.h>
#include <ld_format.h>
#include <ld_aggregations_by_capability.h>
#include <ld_justified_block_format.h>
#include <ld_table_line_settings.h>
#include <ld_table_column_width.h>
#include <ld_table_frame_format.h>
#include <ld_element_group.h>

#include "application.h"
#include "insert_element_command.h"
#include "modify_table_add_rows_command.h"
#include "modify_table_add_columns_command.h"
#include "modify_table_remove_rows_command.h"
#include "modify_table_remove_columns_command.h"
#include "modify_table_merge_cells_command.h"
#include "modify_table_unmerge_cells_command.h"
#include "modify_table_delete_command.h"
#include "update_format_command.h"
#include "table_format_dialog.h"
#include "table_default_line_style_dialog.h"
#include "table_line_style_dialog.h"
#include "table_add_remove_dialog.h"
#include "table_merge_dialog.h"
#include "document.h"
#include "main_window.h"
#include "view_widget.h"
#include "view_proxy.h"
#include "table_format_view_proxy.h"

TableFormatViewProxy::TableFormatViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {}


TableFormatViewProxy::~TableFormatViewProxy() {}


void TableFormatViewProxy::bindDynamic() {
    ViewWidget* view = TableFormatViewProxy::view();
    updateTableFormatDockState(view->aggregationsByCapabilityWithoutAncestors());

    connectSignals();
}


void TableFormatViewProxy::unbindDynamic() {
    disconnectSignals();
}


void TableFormatViewProxy::formatsUnderCursorChanged(
        const Ld::AggregationsByCapability& aggregationsByCapabilityWithAncestors,
        const Ld::AggregationsByCapability& /* aggregationsByCapabilityWithoutAncestors */
    ) {
    // We temporarily disconnect signals to avoid a storm of superflouous activity when we update the dock widget to
    // to actions being triggered which then trigger call-backs to this class instance.
    //
    // Because of round-off, these superflouos events can in turn trigger generation of UpdateFormatCommand instances
    // modifying the document unexpectedly.

    disconnectSignals();
    updateTableFormatDockState(aggregationsByCapabilityWithAncestors);
    connectSignals();
}



void TableFormatViewProxy::insertTable() {
    MainWindow* window = TableFormatViewProxy::window();

    TableFormatDialog tableInsertDialog(window);
    tableInsertDialog.loadSettings(QString("TableInsertDialog"));
    tableInsertDialog.setApplyButtonHidden();
    tableInsertDialog.setOkCancelButtonVisible();
    tableInsertDialog.setOkButtonAlwaysEnabled();
    tableInsertDialog.setTableSizeControlsVisible();
    tableInsertDialog.setWindowTitle(tr("Insert Table"));

    int result = tableInsertDialog.exec();
    if (result == TableFormatDialog::Accepted) {
        tableInsertDialog.saveSettings(QString("TableInsertDialog"));

        QSharedPointer<Ld::TableFrameElement> tableElement = Ld::Element::create(Ld::TableFrameElement::elementName)
                                                             .dynamicCast<Ld::TableFrameElement>();

        QSharedPointer<Ld::TableFrameFormat>  tableFormat  = Ld::Format::create(Ld::TableFrameFormat::formatName)
                                                             .dynamicCast<Ld::TableFrameFormat>();

        unsigned numberRows    = tableInsertDialog.numberRows();
        unsigned numberColumns = tableInsertDialog.numberColumns();

        if (numberRows > 1) {
            tableElement->insertRowsBefore(0, numberRows - 1, false);
        }

        if (numberColumns > 1) {
            tableElement->insertColumnsBefore(0, numberColumns - 1, false);
        }

        updateFormatFromDialog(tableFormat, &tableInsertDialog);
        tableElement->setFormat(tableFormat);

        ViewWidget* view = TableFormatViewProxy::view();
        InsertElementCommand* newCommand = new InsertElementCommand(
            tableElement,
            InsertElementCommand::InsertionLocation::UNDER_ROOT,
            InsertElementCommand::FinalCursorPosition::UNCHANGED
        );

        view->insertCommand(newCommand);
    }
}


void TableFormatViewProxy::formatSettingsDialog() {
    MainWindow* window = TableFormatViewProxy::window();

    TableFormatDialog tableFormatDialog(window);
    tableFormatDialog.loadSettings(QString("TableFormatDialog"));

    #if (defined(Q_OS_DARWIN))

        tableFormatDialog.setApplyButtonHidden();

    #elif (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        tableFormatDialog.setApplyButtonVisible();

    #else

        #error Unknown platform

    #endif

    tableFormatDialog.setOkCancelButtonVisible();
    tableFormatDialog.setTableSizeControlsHidden();
    tableFormatDialog.setAdvancedControlsEnabled();
    tableFormatDialog.setAdvancedModeButtonHidden();
    tableFormatDialog.setAutoApplyDisabled();
    tableFormatDialog.setWindowTitle(tr("Format Table"));

    const Ld::TableFrameFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

    const QSet<Ld::TableFrameFormat::Justification>& justifications = aggregation.justifications();
    tableFormatDialog.setJustification(justifications);

    const QSet<float>& leftIndentations = aggregation.leftIndentations();
    if (leftIndentations.count() == 1) {
        tableFormatDialog.setLeftIndentation(*leftIndentations.begin());
    } else {
        tableFormatDialog.setLeftIndentationCleared();
    }

    const QSet<float>& rightIndentations = aggregation.rightIndentations();
    if (rightIndentations.count() == 1) {
        tableFormatDialog.setRightIndentation(*rightIndentations.begin());
    } else {
        tableFormatDialog.setRightIndentationCleared();
    }

    const QSet<float>& topSpacings = aggregation.topSpacings();
    if (topSpacings.count() == 1) {
        tableFormatDialog.setTopSpacing(*topSpacings.begin());
    } else {
        tableFormatDialog.setTopSpacingCleared();
    }

    const QSet<float>& bottomSpacings = aggregation.bottomSpacings();
    if (bottomSpacings.count() == 1) {
        tableFormatDialog.setBottomSpacing(*bottomSpacings.begin());
    } else {
        tableFormatDialog.setBottomSpacingCleared();
    }

    const QSet<float>& leftGutterValues = aggregation.leftGutterValues();
    if (leftGutterValues.count() == 1) {
        tableFormatDialog.setLeftGutter(*leftGutterValues.begin());
    } else {
        tableFormatDialog.setLeftGutterCleared();
    }

    const QSet<float>& rightGutterValues = aggregation.rightGutterValues();
    if (rightGutterValues.count() == 1) {
        tableFormatDialog.setRightGutter(*rightGutterValues.begin());
    } else {
        tableFormatDialog.setRightGutterCleared();
    }

    const QSet<float>& topGutterValues = aggregation.topGutterValues();
    if (topGutterValues.count() == 1) {
        tableFormatDialog.setTopGutter(*topGutterValues.begin());
    } else {
        tableFormatDialog.setTopGutterCleared();
    }

    const QSet<float>& bottomGutterValues = aggregation.bottomGutterValues();
    if (bottomGutterValues.count() == 1) {
        tableFormatDialog.setBottomGutter(*bottomGutterValues.begin());
    } else {
        tableFormatDialog.setBottomGutterCleared();
    }

    QSet<Ld::TableLineSettings::Style> verticalStyles;
    QSet<float>                        verticalWidths;

    QSet<Ld::TableLineSettings>        defaultVerticalLineSettings = aggregation.defaultColumnLineSettings();
    for (  QSet<Ld::TableLineSettings>::const_iterator
               verticalLineSettingsIterator    = defaultVerticalLineSettings.constBegin(),
               verticalLineSettingsEndIterator = defaultVerticalLineSettings.constEnd()
         ; verticalLineSettingsIterator != verticalLineSettingsEndIterator
         ; ++verticalLineSettingsIterator
        ) {
        verticalStyles << verticalLineSettingsIterator->lineStyle();
        verticalWidths << verticalLineSettingsIterator->width();
    }

    QSet<Ld::TableLineSettings::Style> horizontalStyles;
    QSet<float>                        horizontalWidths;

    QSet<Ld::TableLineSettings>        defaultHorizontalLineSettings = aggregation.defaultRowLineSettings();
    for (  QSet<Ld::TableLineSettings>::const_iterator
               horizontalLineSettingsIterator    = defaultHorizontalLineSettings.constBegin(),
               horizontalLineSettingsEndIterator = defaultHorizontalLineSettings.constEnd()
         ; horizontalLineSettingsIterator != horizontalLineSettingsEndIterator
         ; ++horizontalLineSettingsIterator
        ) {
        horizontalStyles << horizontalLineSettingsIterator->lineStyle();
        horizontalWidths << horizontalLineSettingsIterator->width();
    }

    tableFormatDialog.setVerticalLineStyles(verticalStyles);
    tableFormatDialog.setVerticalLineWidths(verticalWidths);

    tableFormatDialog.setHorizontalLineStyles(horizontalStyles);
    tableFormatDialog.setHorizontalLineWidths(horizontalWidths);

    QSet<QColor> defaultColors = aggregation.defaultCellColors();
    tableFormatDialog.setDefaultCellColors(defaultColors);

    QList<QSet<Ld::TableColumnWidth>> columnWidthValues;
    for (  Ld::ElementWeakPointerSet::const_iterator
               elementIterator    = aggregation.constBegin(),
               elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        QSharedPointer<Ld::TableFrameElement> tableElement  = elementIterator->toStrongRef()
                                                              .dynamicCast<Ld::TableFrameElement>();
        QSharedPointer<Ld::TableFrameFormat>  tableFormat   = tableElement->format();

        unsigned numberColumns = tableElement->numberColumns();
        for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
            const Ld::TableColumnWidth& columnWidth = tableFormat->columnWidth(columnIndex);
            if (columnIndex < static_cast<unsigned>(columnWidthValues.size())) {
                columnWidthValues[columnIndex] << columnWidth;
            } else {
                QSet<Ld::TableColumnWidth> newColumnWidths;
                newColumnWidths << columnWidth;
                columnWidthValues.append(newColumnWidths);
            }
        }
    }

    QList<TableFormatDialog::ColumnWidth> tableColumnWidths;
    unsigned                              numberTableColumnWidths = static_cast<unsigned>(columnWidthValues.size());
    for (unsigned columnIndex=0 ; columnIndex<numberTableColumnWidths ; ++columnIndex) {
        const QSet<Ld::TableColumnWidth>& columnWidths = columnWidthValues.at(columnIndex);
        if (columnWidths.size() == 1) {
            tableColumnWidths.append(*columnWidths.begin());
        } else {
            tableColumnWidths.append(Ld::TableColumnWidth(Ld::TableColumnWidth::WidthType::INVALID, 0.0F));
        }
    }

    tableFormatDialog.setColumnWidths(tableColumnWidths);

    QSet<QColor> defaultCellColors = aggregation.defaultCellColors();
    tableFormatDialog.setDefaultCellColors(defaultCellColors);

    connect(&tableFormatDialog, &TableFormatDialog::apply, this, &TableFormatViewProxy::tableFormatDialogApplyUpdates);

    int result = tableFormatDialog.exec();
    if (result == TableFormatDialog::Accepted) {
        tableFormatDialogApplyUpdates(&tableFormatDialog);
    }
}


void TableFormatViewProxy::tableFormatDialogApplyUpdates(TableFormatDialog* dialog) {
    dialog->saveSettings(QString("TableFormatDialog"));

    const Ld::TableFrameFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                     elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer                   element = elementIterator->toStrongRef();
        QSharedPointer<Ld::TableFrameFormat> format  = element->format()->clone().dynamicCast<Ld::TableFrameFormat>();

        bool changed = updateFormatFromDialog(format, dialog);
        if (changed) {
            formatsByElement.insert(element, format);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void TableFormatViewProxy::mergeCells() {
    QAction* formatTableMerge = window()->action("format_table_merge");

    if (!formatTableMerge->isChecked()) {
        Ld::TableFrameElement::CellPosition   position = *currentCellsByTable.begin().value().begin();
        QSharedPointer<Ld::TableFrameElement> table    = currentCellsByTable.begin().key();

        ModifyTableUnmergeCellsCommand* unmergeCellsCommand = new ModifyTableUnmergeCellsCommand(
            table,
            position.rowIndex(),
            position.columnIndex()
        );

        view()->insertCommand(unmergeCellsCommand);
        formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
    } else {
        Ld::TableFrameElement::CellPosition   position = *currentCellsByTable.begin().value().begin();
        QSharedPointer<Ld::TableFrameElement> table    = currentCellsByTable.begin().key();

        unsigned maximumRowSpan    = table->numberRows() - position.rowIndex();
        unsigned maximumColumnSpan = table->numberColumns() - position.columnIndex();

        TableMergeDialog tableMergeDialog(maximumRowSpan, maximumColumnSpan, window());
        int result = tableMergeDialog.exec();
        if (result == TableMergeDialog::Accepted) {
            unsigned rowSpan    = tableMergeDialog.rowSpan();
            unsigned columnSpan = tableMergeDialog.columnSpan();

            ModifyTableMergeCellsCommand* mergeCellsCommand = new ModifyTableMergeCellsCommand(
                table,
                position.rowIndex(),
                position.columnIndex(),
                rowSpan,
                columnSpan
            );

            view()->insertCommand(mergeCellsCommand);
            formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
        }
    }
}


void TableFormatViewProxy::leftAlign() {
    updateTableJustification(Ld::TableFrameFormat::Justification::LEFT);
}


void TableFormatViewProxy::center() {
    updateTableJustification(Ld::TableFrameFormat::Justification::CENTER);
}


void TableFormatViewProxy::rightAlign() {
    updateTableJustification(Ld::TableFrameFormat::Justification::RIGHT);
}


void TableFormatViewProxy::addRows() {
    TableAddRemoveDialog addRowsDialog(TableAddRemoveDialog::DialogMode::ADD_ROWS, window());
    int result = addRowsDialog.exec();
    if (result == TableAddRemoveDialog::Accepted) {
        bool     addAbove   = addRowsDialog.addBefore();
        unsigned numberRows = addRowsDialog.numberEntries();

        QSharedPointer<Ld::TableFrameElement> table    = currentRowsByTable.begin().key();
        unsigned                              rowIndex = *currentRowsByTable.begin().value().begin();

        if (!addAbove) {
            ++rowIndex;
        }

        ModifyTableAddRowsCommand* addRowsCommand = new ModifyTableAddRowsCommand(table, rowIndex, numberRows);
        view()->insertCommand(addRowsCommand);
        formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
    }
}


void TableFormatViewProxy::addColumns() {
    TableAddRemoveDialog addColumnsDialog(TableAddRemoveDialog::DialogMode::ADD_COLUMNS, window());
    int result = addColumnsDialog.exec();
    if (result == TableAddRemoveDialog::Accepted) {
        bool     addLeft      = addColumnsDialog.addBefore();
        unsigned numberColumns = addColumnsDialog.numberEntries();

        QSharedPointer<Ld::TableFrameElement> table       = currentColumnsByTable.begin().key();
        unsigned                              columnIndex = *currentColumnsByTable.begin().value().begin();

        if (!addLeft) {
            ++columnIndex;
        }

        ModifyTableAddColumnsCommand* addColumnsCommand = new ModifyTableAddColumnsCommand(
            table,
            columnIndex,
            numberColumns
        );
        view()->insertCommand(addColumnsCommand);
        formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
    }
}


void TableFormatViewProxy::removeRows() {
    QSharedPointer<Ld::TableFrameElement> table      = currentRowsByTable.begin().key();
    unsigned                              rowIndex   = *currentRowsByTable.begin().value().begin();
    unsigned                              numberRows = table->numberRows();

    TableAddRemoveDialog removeRowsDialog(TableAddRemoveDialog::DialogMode::REMOVE_ROWS, window());
    removeRowsDialog.setBeforeMaximumNumberEntries(rowIndex + 1);
    removeRowsDialog.setAfterMaximumNumberEntries(numberRows - rowIndex - 1);

    int result = removeRowsDialog.exec();
    if (result == TableAddRemoveDialog::Accepted) {
        bool     removeAbove = removeRowsDialog.addBefore();
        unsigned numberRows  = removeRowsDialog.numberEntries();

        QSharedPointer<Ld::TableFrameElement> table    = currentRowsByTable.begin().key();
        unsigned                              rowIndex = *currentRowsByTable.begin().value().begin();

        if (removeAbove) {
            rowIndex -= numberRows - 1;
        }

        ModifyTableRemoveRowsCommand* removeRowsCommand = new ModifyTableRemoveRowsCommand(
            table,
            rowIndex,
            numberRows
        );

        view()->insertCommand(removeRowsCommand);
        formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
    }
}


void TableFormatViewProxy::removeColumns() {
    QSharedPointer<Ld::TableFrameElement> table         = currentColumnsByTable.begin().key();
    unsigned                              columnIndex   = *currentColumnsByTable.begin().value().begin();
    unsigned                              numberColumns = table->numberColumns();

    TableAddRemoveDialog removeColumnsDialog(TableAddRemoveDialog::DialogMode::REMOVE_COLUMNS, window());
    removeColumnsDialog.setBeforeMaximumNumberEntries(columnIndex + 1);
    removeColumnsDialog.setAfterMaximumNumberEntries(numberColumns - columnIndex - 1);

    int result = removeColumnsDialog.exec();
    if (result == TableAddRemoveDialog::Accepted) {
        bool     removeAbove   = removeColumnsDialog.addBefore();
        unsigned numberColumns = removeColumnsDialog.numberEntries();

        QSharedPointer<Ld::TableFrameElement> table       = currentColumnsByTable.begin().key();
        unsigned                              columnIndex = *currentColumnsByTable.begin().value().begin();

        if (removeAbove) {
            columnIndex -= numberColumns - 1;
        }

        ModifyTableRemoveColumnsCommand* removeColumnsCommand = new ModifyTableRemoveColumnsCommand(
            table,
            columnIndex,
            numberColumns
        );

        view()->insertCommand(removeColumnsCommand);
        formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
    }
}


void TableFormatViewProxy::removeTable() {
    const Cursor& cursor = view()->cursor();
    if (cursor.cursorType() != Cursor::Type::SELECTION) {
        const Ld::ElementCursor& elementCursor = cursor.elementCursor();
        Ld::ElementPointer       tableElement  = elementCursor.element();

        while (!tableElement.isNull() && tableElement->typeName() != Ld::TableFrameElement::elementName) {
            tableElement = tableElement->parent();
        }

        if (!tableElement.isNull()) {
            view()->insertCommand(new ModifyTableDeleteCommand(tableElement.dynamicCast<Ld::TableFrameElement>()));
        }
    }
}


void TableFormatViewProxy::rowColorDialog() {
    QAction* formatTableRowColorAction = window()->action("format_table_row_color");
    UpdateFormatCommand::FormatsByElement formatsByElement;

    if (!formatTableRowColorAction->isChecked()) {
        MainWindow* mainWindow = window();
        EQt::ColorDialog colorDialog(mainWindow);

        int result = colorDialog.exec();
        if (result == EQt::ColorDialog::Accepted) {
            QColor newColor = colorDialog.currentColor();

            for (  RowsByTable::const_iterator rowsByTableIterator    = currentRowsByTable.constBegin(),
                                               rowsByTableEndIterator = currentRowsByTable.constEnd()
                 ; rowsByTableIterator != rowsByTableEndIterator
                 ; ++rowsByTableIterator
                ) {
                QSharedPointer<Ld::TableFrameElement> table      = rowsByTableIterator.key();
                const QSet<unsigned>&                 rowIndexes = rowsByTableIterator.value();

                QSharedPointer<Ld::TableFrameFormat>  newFormat  = table->format()->clone()
                                                                   .dynamicCast<Ld::TableFrameFormat>();

                bool                                  changed    = false;

                for (  QSet<unsigned>::const_iterator
                           rowIterator    = rowIndexes.constBegin(),
                           rowEndIterator = rowIndexes.constEnd()
                     ; rowIterator != rowEndIterator
                     ; ++rowIterator
                    ) {
                    unsigned rowIndex = *rowIterator;

                    if (newFormat->rowColorMapsToDefault(rowIndex) || newFormat->rowColor(rowIndex) != newColor) {
                        newFormat->setRowColor(rowIndex, newColor);
                        changed = true;
                    }
                }

                if (changed) {
                    formatsByElement.insert(table, newFormat);
                }
            }

            if (!formatsByElement.isEmpty()) {
                issueUpdateFormatCommand(formatsByElement);
            }
        } else {
            formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
        }
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  RowsByTable::const_iterator rowsByTableIterator    = currentRowsByTable.constBegin(),
                                           rowsByTableEndIterator = currentRowsByTable.constEnd()
             ; rowsByTableIterator != rowsByTableEndIterator
             ; ++rowsByTableIterator
            ) {
            QSharedPointer<Ld::TableFrameElement> table      = rowsByTableIterator.key();
            const QSet<unsigned>&                 rowIndexes = rowsByTableIterator.value();

            QSharedPointer<Ld::TableFrameFormat>  newFormat  = table->format()->clone()
                                                               .dynamicCast<Ld::TableFrameFormat>();

            bool                                  changed    = false;

            for (  QSet<unsigned>::const_iterator
                       rowIterator    = rowIndexes.constBegin(),
                       rowEndIterator = rowIndexes.constEnd()
                 ; rowIterator != rowEndIterator
                 ; ++rowIterator
                ) {
                unsigned rowIndex = *rowIterator;

                if (!newFormat->rowColorMapsToDefault(rowIndex)) {
                    newFormat->setRowColorToDefault(rowIndex);
                    changed = true;
                }
            }

            if (changed) {
                formatsByElement.insert(table, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void TableFormatViewProxy::columnColorDialog() {
    QAction* formatTableColumnColorAction = window()->action("format_table_column_color");
    if (!formatTableColumnColorAction->isChecked()) {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        MainWindow* mainWindow = window();
        EQt::ColorDialog colorDialog(mainWindow);

        int result = colorDialog.exec();
        if (result == EQt::ColorDialog::Accepted) {
            QColor newColor = colorDialog.currentColor();

            for (  ColumnsByTable::const_iterator columnsByTableIterator    = currentColumnsByTable.constBegin(),
                                                  columnsByTableEndIterator = currentColumnsByTable.constEnd()
                 ; columnsByTableIterator != columnsByTableEndIterator
                 ; ++columnsByTableIterator
                ) {
                QSharedPointer<Ld::TableFrameElement> table         = columnsByTableIterator.key();
                const QSet<unsigned>&                 columnIndexes = columnsByTableIterator.value();

                QSharedPointer<Ld::TableFrameFormat>  newFormat     = table->format()->clone()
                                                                      .dynamicCast<Ld::TableFrameFormat>();

                bool                                  changed       = false;

                for (  QSet<unsigned>::const_iterator
                           columnIterator    = columnIndexes.constBegin(),
                           columnEndIterator = columnIndexes.constEnd()
                     ; columnIterator != columnEndIterator
                     ; ++columnIterator
                    ) {
                    unsigned columnIndex = *columnIterator;

                    if (newFormat->columnColorMapsToDefault(columnIndex) ||
                        newFormat->columnColor(columnIndex) != newColor     ) {
                        newFormat->setColumnColor(columnIndex, newColor);
                        changed = true;
                    }
                }

                if (changed) {
                    formatsByElement.insert(table, newFormat);
                }
            }

            if (!formatsByElement.isEmpty()) {
                issueUpdateFormatCommand(formatsByElement);
            }
        } else {
            formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
        }
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  ColumnsByTable::const_iterator columnsByTableIterator    = currentColumnsByTable.constBegin(),
                                              columnsByTableEndIterator = currentColumnsByTable.constEnd()
             ; columnsByTableIterator != columnsByTableEndIterator
             ; ++columnsByTableIterator
            ) {
            QSharedPointer<Ld::TableFrameElement> table         = columnsByTableIterator.key();
            const QSet<unsigned>&                 columnIndexes = columnsByTableIterator.value();

            QSharedPointer<Ld::TableFrameFormat>  newFormat     = table->format()->clone()
                                                                  .dynamicCast<Ld::TableFrameFormat>();

            bool                                  changed       = false;

            for (  QSet<unsigned>::const_iterator
                       columnIterator    = columnIndexes.constBegin(),
                       columnEndIterator = columnIndexes.constEnd()
                 ; columnIterator != columnEndIterator
                 ; ++columnIterator
                ) {
                unsigned columnIndex = *columnIterator;

                if (!newFormat->columnColorMapsToDefault(columnIndex)) {
                    newFormat->setColumnColorToDefault(columnIndex);
                    changed = true;
                }
            }

            if (changed) {
                formatsByElement.insert(table, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void TableFormatViewProxy::cellColorDialog() {
    QAction* formatTableCellColorAction = window()->action("format_table_cell_color");
    if (!formatTableCellColorAction->isChecked()) {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        MainWindow* mainWindow = window();
        EQt::ColorDialog colorDialog(mainWindow);

        int result = colorDialog.exec();
        if (result == EQt::ColorDialog::Accepted) {
            QColor newColor = colorDialog.currentColor();

            for (  CellsByTable::const_iterator cellsByTableIterator    = currentCellsByTable.constBegin(),
                                                cellsByTableEndIterator = currentCellsByTable.constEnd()
                 ; cellsByTableIterator != cellsByTableEndIterator
                 ; ++cellsByTableIterator
                ) {
                QSharedPointer<Ld::TableFrameElement>            table         = cellsByTableIterator.key();
                const QSet<Ld::TableFrameElement::CellPosition>& cellPositions = cellsByTableIterator.value();

                QSharedPointer<Ld::TableFrameFormat>             newFormat     = table->format()->clone()
                                                                                 .dynamicCast<Ld::TableFrameFormat>();

                bool                                             changed       = false;

                for (  QSet<Ld::TableFrameElement::CellPosition>::const_iterator
                           positionIterator    = cellPositions.constBegin(),
                           positionEndIterator = cellPositions.constEnd()
                     ; positionIterator != positionEndIterator
                     ; ++positionIterator
                    ) {
                    unsigned rowIndex    = positionIterator->rowIndex();
                    unsigned columnIndex = positionIterator->columnIndex();

                    if (newFormat->cellColorMapsToDefault(rowIndex, columnIndex) ||
                        newFormat->cellColor(rowIndex, columnIndex) != newColor     ) {
                        newFormat->setCellColor(rowIndex, columnIndex, newColor);
                        changed = true;
                    }
                }

                if (changed) {
                    formatsByElement.insert(table, newFormat);
                }
            }

            if (!formatsByElement.isEmpty()) {
                issueUpdateFormatCommand(formatsByElement);
            }
        } else {
            formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
        }
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  CellsByTable::const_iterator cellsByTableIterator    = currentCellsByTable.constBegin(),
                                            cellsByTableEndIterator = currentCellsByTable.constEnd()
             ; cellsByTableIterator != cellsByTableEndIterator
             ; ++cellsByTableIterator
            ) {
            QSharedPointer<Ld::TableFrameElement>            table         = cellsByTableIterator.key();
            const QSet<Ld::TableFrameElement::CellPosition>& cellPositions = cellsByTableIterator.value();

            QSharedPointer<Ld::TableFrameFormat>             newFormat     = table->format()->clone()
                                                                             .dynamicCast<Ld::TableFrameFormat>();

            bool                                             changed       = false;

            for (  QSet<Ld::TableFrameElement::CellPosition>::const_iterator
                       positionIterator    = cellPositions.constBegin(),
                       positionEndIterator = cellPositions.constEnd()
                 ; positionIterator != positionEndIterator
                 ; ++positionIterator
                ) {
                unsigned rowIndex    = positionIterator->rowIndex();
                unsigned columnIndex = positionIterator->columnIndex();

                if (!newFormat->cellColorMapsToDefault(rowIndex, columnIndex)) {
                    newFormat->setCellColorToDefault(rowIndex, columnIndex);
                    changed = true;
                }
            }

            if (changed) {
                formatsByElement.insert(table, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void TableFormatViewProxy::horizontalLineDialog() {
    QAction* formatTableLinesHorizontalAction = window()->action("format_table_lines_horizontal");
    if (!formatTableLinesHorizontalAction->isChecked()) {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  RowsByTable::const_iterator rowsByTableIterator    = currentRowsByTable.constBegin(),
                                           rowsByTableEndIterator = currentRowsByTable.constEnd()
             ; rowsByTableIterator != rowsByTableEndIterator
             ; ++rowsByTableIterator
            ) {
            QSharedPointer<Ld::TableFrameElement> table      = rowsByTableIterator.key();
            const QSet<unsigned>&                 rowIndexes = rowsByTableIterator.value();
            QSharedPointer<Ld::TableFrameFormat>  format     = table->format();
            bool                                  changed    = false;

            for (  QSet<unsigned>::const_iterator rowIterator    = rowIndexes.constBegin(),
                                                  rowEndIterator = rowIndexes.constEnd()
                 ; rowIterator != rowEndIterator
                 ; ++rowIterator
                ) {
                unsigned rowIndex = *rowIterator;

                if (!format->rowLineSettingMapsToDefault(rowIndex + 0)) {
                    format->setRowLineSettingToDefault(rowIndex + 0);
                    changed = true;
                }

                if (!format->rowLineSettingMapsToDefault(rowIndex + 1)) {
                    format->setRowLineSettingToDefault(rowIndex + 1);
                    changed = true;
                }
            }

            if (changed) {
                formatsByElement.insert(table, format);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    } else {
        QSet<Ld::TableLineSettings::Style> topLineStyles;
        QSet<float>                        topLineWidths;

        QSet<Ld::TableLineSettings::Style> bottomLineStyles;
        QSet<float>                        bottomLineWidths;

        for (  RowsByTable::const_iterator rowsByTableIterator    = currentRowsByTable.constBegin(),
                                           rowsByTableEndIterator = currentRowsByTable.constEnd()
             ; rowsByTableIterator != rowsByTableEndIterator
             ; ++rowsByTableIterator
            ) {
            QSharedPointer<Ld::TableFrameElement> table      = rowsByTableIterator.key();
            const QSet<unsigned>&                 rowIndexes = rowsByTableIterator.value();
            QSharedPointer<Ld::TableFrameFormat>  format     = table->format();

            for (  QSet<unsigned>::const_iterator rowIterator    = rowIndexes.constBegin(),
                                                  rowEndIterator = rowIndexes.constEnd()
                 ; rowIterator != rowEndIterator
                 ; ++rowIterator
                ) {
                unsigned rowIndex = *rowIterator;

                if (!format->rowLineSettingMapsToDefault(rowIndex)) {
                    Ld::TableLineSettings lineAbove = format->rowLineSetting(rowIndex + 0);
                    topLineStyles << lineAbove.lineStyle();
                    topLineWidths << lineAbove.width();
                }

                if (!format->rowLineSettingMapsToDefault(rowIndex + 1)) {
                    Ld::TableLineSettings lineBelow = format->rowLineSetting(rowIndex + 1);
                    bottomLineStyles << lineBelow.lineStyle();
                    bottomLineWidths << lineBelow.width();
                }
            }
        }

        TableLineStyleDialog tableLineStyleDialog(true);

        #if (defined(Q_OS_DARWIN))

            tableLineStyleDialog.setApplyButtonHidden();
            tableLineStyleDialog.setAutoApplyDisabled();

        #elif (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

            tableLineStyleDialog.setApplyButtonVisible();
            tableLineStyleDialog.setAutoApplyDisabled();

        #else

            #error Unknown platform

        #endif

        tableLineStyleDialog.setWindowTitle(tr("Horizontal Line Style"));
        tableLineStyleDialog.setLeftTopLineStyles(topLineStyles);
        tableLineStyleDialog.setLeftTopLineWidths(topLineWidths);
        tableLineStyleDialog.setRightBottomLineStyles(bottomLineStyles);
        tableLineStyleDialog.setRightBottomLineWidths(bottomLineWidths);

        connect(
            &tableLineStyleDialog,
            &TableLineStyleDialog::apply,
            this,
            &TableFormatViewProxy::horizontalLineDialogApplyUpdates
        );

        int result = tableLineStyleDialog.exec();
        if (result == TableLineStyleDialog::Accepted) {
            horizontalLineDialogApplyUpdates(&tableLineStyleDialog);
        } else {
            formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
        }
    }
}


void TableFormatViewProxy::horizontalLineDialogApplyUpdates(TableLineStyleDialog* dialog) {
    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  RowsByTable::const_iterator rowsByTableIterator    = currentRowsByTable.constBegin(),
                                       rowsByTableEndIterator = currentRowsByTable.constEnd()
         ; rowsByTableIterator != rowsByTableEndIterator
         ; ++rowsByTableIterator
        ) {
        QSharedPointer<Ld::TableFrameElement> table      = rowsByTableIterator.key();
        QList<unsigned>                       rowIndexes = rowsByTableIterator.value().values();
        QSharedPointer<Ld::TableFrameFormat>  format     = table->format()->clone()
                                                           .dynamicCast<Ld::TableFrameFormat>();
        bool                                  changed    = false;

        std::sort(rowIndexes.begin(), rowIndexes.end());

        for (  QList<unsigned>::const_iterator rowIterator    = rowIndexes.constBegin(),
                                               rowEndIterator = rowIndexes.constEnd()
             ; rowIterator != rowEndIterator
             ; ++rowIterator
            ) {
            unsigned rowIndex = *rowIterator;

            Ld::TableLineSettings topLineSetting           = format->rowLineSetting(rowIndex + 0);
            bool                  topLineSettingChanged    = false;
            Ld::TableLineSettings bottomLineSetting        = format->rowLineSetting(rowIndex + 1);
            bool                  bottomLineSettingChanged = false;

            if (dialog->leftTopLineStyleSet()                                  &&
                (format->rowLineSettingMapsToDefault(rowIndex + 0)        ||
                 topLineSetting.lineStyle() != dialog->leftTopLineStyle()    )    ) {
                topLineSetting.setLineStyle(dialog->leftTopLineStyle());
                topLineSettingChanged = true;
            }

            if (dialog->leftTopLineWidthSet()                              &&
                (format->rowLineSettingMapsToDefault(rowIndex + 0)    ||
                 topLineSetting.width() != dialog->leftTopLineWidth()    )    ) {
                topLineSetting.setWidth(dialog->leftTopLineWidth());
                topLineSettingChanged = true;
            }

            if (dialog->rightBottomLineStyleSet()                                     &&
                (format->rowLineSettingMapsToDefault(rowIndex + 1)               ||
                 bottomLineSetting.lineStyle() != dialog->rightBottomLineStyle()    )    ) {
                bottomLineSetting.setLineStyle(dialog->rightBottomLineStyle());
                bottomLineSettingChanged = true;
            }

            if (dialog->rightBottomLineWidthSet()                                 &&
                (format->rowLineSettingMapsToDefault(rowIndex + 0)           ||
                 bottomLineSetting.width() != dialog->rightBottomLineWidth()    )    ) {
                bottomLineSetting.setWidth(dialog->rightBottomLineWidth());
                bottomLineSettingChanged = true;
            }

            if (topLineSettingChanged) {
                format->setRowLineSetting(rowIndex + 0, topLineSetting);
                changed = true;
            }

            if (bottomLineSettingChanged) {
                format->setRowLineSetting(rowIndex + 1, bottomLineSetting);
                changed = true;
            }
        }

        if (changed) {
            formatsByElement.insert(table, format);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void TableFormatViewProxy::verticalLineDialog() {
    QAction* formatTableLinesVerticalAction = window()->action("format_table_lines_vertical");
    if (!formatTableLinesVerticalAction->isChecked()) {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  ColumnsByTable::const_iterator columnsByTableIterator    = currentColumnsByTable.constBegin(),
                                              columnsByTableEndIterator = currentColumnsByTable.constEnd()
             ; columnsByTableIterator != columnsByTableEndIterator
             ; ++columnsByTableIterator
            ) {
            QSharedPointer<Ld::TableFrameElement> table          = columnsByTableIterator.key();
            const QSet<unsigned>&                 columnsIndexes = columnsByTableIterator.value();
            QSharedPointer<Ld::TableFrameFormat>  format         = table->format();
            bool                                  changed        = false;

            for (  QSet<unsigned>::const_iterator columnIterator    = columnsIndexes.constBegin(),
                                                  columnEndIterator = columnsIndexes.constEnd()
                 ; columnIterator != columnEndIterator
                 ; ++columnIterator
                ) {
                unsigned columnIndex = *columnIterator;

                if (!format->columnLineSettingMapsToDefault(columnIndex + 0)) {
                    format->setColumnLineSettingToDefault(columnIndex + 0);
                    changed = true;
                }

                if (!format->columnLineSettingMapsToDefault(columnIndex + 1)) {
                    format->setColumnLineSettingToDefault(columnIndex + 1);
                    changed = true;
                }
            }

            if (changed) {
                formatsByElement.insert(table, format);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    } else {
        QSet<Ld::TableLineSettings::Style> leftLineStyles;
        QSet<float>                        leftLineWidths;

        QSet<Ld::TableLineSettings::Style> rightLineStyles;
        QSet<float>                        rightLineWidths;

        for (  ColumnsByTable::const_iterator columnsByTableIterator    = currentRowsByTable.constBegin(),
                                              columnsByTableEndIterator = currentRowsByTable.constEnd()
             ; columnsByTableIterator != columnsByTableEndIterator
             ; ++columnsByTableIterator
            ) {
            QSharedPointer<Ld::TableFrameElement> table         = columnsByTableIterator.key();
            const QSet<unsigned>&                 columnIndexes = columnsByTableIterator.value();
            QSharedPointer<Ld::TableFrameFormat>  format        = table->format();

            for (  QSet<unsigned>::const_iterator columnIterator    = columnIndexes.constBegin(),
                                                  columnEndIterator = columnIndexes.constEnd()
                 ; columnIterator != columnEndIterator
                 ; ++columnIterator
                ) {
                unsigned columnIndex = *columnIterator;

                if (!format->columnLineSettingMapsToDefault(columnIndex)) {
                    Ld::TableLineSettings lineLeft = format->columnLineSetting(columnIndex + 0);
                    leftLineStyles << lineLeft.lineStyle();
                    leftLineWidths << lineLeft.width();
                }

                if (!format->columnLineSettingMapsToDefault(columnIndex + 1)) {
                    Ld::TableLineSettings lineRight = format->columnLineSetting(columnIndex + 1);
                    rightLineStyles << lineRight.lineStyle();
                    rightLineWidths << lineRight.width();
                }
            }
        }

        TableLineStyleDialog tableLineStyleDialog(false);

        #if (defined(Q_OS_DARWIN))

            tableLineStyleDialog.setApplyButtonHidden();
            tableLineStyleDialog.setAutoApplyDisabled();

        #elif (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

            tableLineStyleDialog.setApplyButtonVisible();
            tableLineStyleDialog.setAutoApplyDisabled();

        #else

            #error Unknown platform

        #endif

        tableLineStyleDialog.setWindowTitle(tr("Vertical Line Style"));
        tableLineStyleDialog.setLeftTopLineStyles(leftLineStyles);
        tableLineStyleDialog.setLeftTopLineWidths(leftLineWidths);
        tableLineStyleDialog.setRightBottomLineStyles(rightLineStyles);
        tableLineStyleDialog.setRightBottomLineWidths(rightLineWidths);

        connect(
            &tableLineStyleDialog,
            &TableLineStyleDialog::apply,
            this,
            &TableFormatViewProxy::verticalLineDialogApplyUpdates
        );

        int result = tableLineStyleDialog.exec();
        if (result == TableLineStyleDialog::Accepted) {
            verticalLineDialogApplyUpdates(&tableLineStyleDialog);
        } else {
            formatsUnderCursorChanged(currentAggregationsByCapability, Ld::AggregationsByCapability());
        }
    }
}


void TableFormatViewProxy::verticalLineDialogApplyUpdates(TableLineStyleDialog* dialog) {
    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  ColumnsByTable::const_iterator columnsByTableIterator    = currentColumnsByTable.constBegin(),
                                          columnsByTableEndIterator = currentColumnsByTable.constEnd()
         ; columnsByTableIterator != columnsByTableEndIterator
         ; ++columnsByTableIterator
        ) {
        QSharedPointer<Ld::TableFrameElement> table      = columnsByTableIterator.key();
        QList<unsigned>                       rowIndexes = columnsByTableIterator.value().values();
        QSharedPointer<Ld::TableFrameFormat>  format     = table->format()->clone()
                                                           .dynamicCast<Ld::TableFrameFormat>();
        bool                                  changed    = false;

        std::sort(rowIndexes.begin(), rowIndexes.end());

        for (  QList<unsigned>::const_iterator columnIterator    = rowIndexes.constBegin(),
                                               columnEndIterator = rowIndexes.constEnd()
             ; columnIterator != columnEndIterator
             ; ++columnIterator
            ) {
            unsigned columnIndex = *columnIterator;

            Ld::TableLineSettings leftLineSetting         = format->columnLineSetting(columnIndex + 0);
            bool                  leftLineSettingChanged  = false;
            Ld::TableLineSettings rightLineSetting        = format->columnLineSetting(columnIndex + 1);
            bool                  rightLineSettingChanged = false;

            if (dialog->leftTopLineStyleSet()                                   &&
                (format->columnLineSettingMapsToDefault(columnIndex + 0)   ||
                 leftLineSetting.lineStyle() != dialog->leftTopLineStyle()    )    ) {
                leftLineSetting.setLineStyle(dialog->leftTopLineStyle());
                leftLineSettingChanged = true;
            }

            if (dialog->leftTopLineWidthSet()                                  &&
                (format->columnLineSettingMapsToDefault(columnIndex + 0)  ||
                 leftLineSetting.width() != dialog->leftTopLineWidth()       )    ) {
                leftLineSetting.setWidth(dialog->leftTopLineWidth());
                leftLineSettingChanged = true;
            }

            if (dialog->rightBottomLineStyleSet()                                    &&
                (format->columnLineSettingMapsToDefault(columnIndex + 1)        ||
                 rightLineSetting.lineStyle() != dialog->rightBottomLineStyle()    )    ) {
                rightLineSetting.setLineStyle(dialog->rightBottomLineStyle());
                rightLineSettingChanged = true;
            }

            if (dialog->rightBottomLineWidthSet()                                &&
                (format->columnLineSettingMapsToDefault(columnIndex + 0)    ||
                 rightLineSetting.width() != dialog->rightBottomLineWidth()    )    ) {
                rightLineSetting.setWidth(dialog->rightBottomLineWidth());
                rightLineSettingChanged = true;
            }

            if (leftLineSettingChanged) {
                format->setColumnLineSetting(columnIndex + 0, leftLineSetting);
                changed = true;
            }

            if (rightLineSettingChanged) {
                format->setColumnLineSetting(columnIndex + 1, rightLineSetting);
                changed = true;
            }
        }

        if (changed) {
            formatsByElement.insert(table, format);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void TableFormatViewProxy::defaultCellColor() {
    MainWindow* mainWindow = window();

    EQt::ColorDialog colorDialog(mainWindow);

    const Ld::TableFrameFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

    QSet<QColor> defaultColors = aggregation.defaultCellColors();

    if (defaultColors.count() == 1) {
        colorDialog.setCurrentColor(*defaultColors.begin());
    }

    int result = colorDialog.exec();
    if (result == EQt::ColorDialog::Accepted) {
        tableDefaultCellColorApplyUpdates(&colorDialog);
    }
}


void TableFormatViewProxy::tableDefaultCellColorApplyUpdates(EQt::ColorDialog* dialog) {
    QColor newColor = dialog->currentColor();

    const Ld::TableFrameFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                     elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer                   element = elementIterator->toStrongRef();
        QSharedPointer<Ld::TableFrameFormat> format  = element->format().dynamicCast<Ld::TableFrameFormat>();

        QColor currentColor = format->defaultColor();
        if (currentColor != newColor) {
            QSharedPointer<Ld::TableFrameFormat> newFormat = format->clone().dynamicCast<Ld::TableFrameFormat>();
            newFormat->setDefaultColor(newColor);

            formatsByElement.insert(element, newFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void TableFormatViewProxy::topSpacing() {
    QAction* tableTopSpacingAction = window()->action("format_table_top_spacing");

    bool  ok;
    float newTopSpacing = tableTopSpacingAction->data().toFloat(&ok);
    if (ok) {
        const Ld::TableFrameFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                         elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer                   element = elementIterator->toStrongRef();
            QSharedPointer<Ld::TableFrameFormat> format  = element->format().dynamicCast<Ld::TableFrameFormat>();

            float currentTopSpacing = format->topSpacing();
            if (currentTopSpacing != newTopSpacing) {
                QSharedPointer<Ld::TableFrameFormat> newFormat  = format->clone().dynamicCast<Ld::TableFrameFormat>();
                newFormat->setTopSpacing(newTopSpacing);

                formatsByElement.insert(element, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void TableFormatViewProxy::bottomSpacing() {
    QAction* tableBottomSpacingAction = window()->action("format_table_bottom_spacing");

    bool  ok;
    float newBottomSpacing = tableBottomSpacingAction->data().toFloat(&ok);
    if (ok) {
        const Ld::TableFrameFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                         elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer                   element = elementIterator->toStrongRef();
            QSharedPointer<Ld::TableFrameFormat> format  = element->format().dynamicCast<Ld::TableFrameFormat>();

            float currentBottomSpacing = format->bottomSpacing();
            if (currentBottomSpacing != newBottomSpacing) {
                QSharedPointer<Ld::TableFrameFormat> newFormat  = format->clone().dynamicCast<Ld::TableFrameFormat>();
                newFormat->setBottomSpacing(newBottomSpacing);

                formatsByElement.insert(element, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void TableFormatViewProxy::leftIndentation() {
    QAction* tableLeftIndentationAction = window()->action("format_table_left_indentation");

    bool  ok;
    float newLeftIndentation = tableLeftIndentationAction->data().toFloat(&ok);
    if (ok) {
        const Ld::TableFrameFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                         elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer                   element = elementIterator->toStrongRef();
            QSharedPointer<Ld::TableFrameFormat> format  = element->format().dynamicCast<Ld::TableFrameFormat>();

            float currentLeftIndentation = format->leftIndentation();
            if (currentLeftIndentation != newLeftIndentation) {
                QSharedPointer<Ld::TableFrameFormat> newFormat  = format->clone().dynamicCast<Ld::TableFrameFormat>();
                newFormat->setLeftIndentation(newLeftIndentation);

                formatsByElement.insert(element, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }
}


void TableFormatViewProxy::rightIndentation() {
    QAction* tableRightIndentationAction = window()->action("format_table_right_indentation");

    bool  ok;
    float newRightIndentation = tableRightIndentationAction->data().toFloat(&ok);
    if (ok) {
        const Ld::TableFrameFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                         elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer                   element = elementIterator->toStrongRef();
            QSharedPointer<Ld::TableFrameFormat> format  = element->format().dynamicCast<Ld::TableFrameFormat>();

            float currentRightIndentation = format->rightIndentation();
            if (currentRightIndentation != newRightIndentation) {
                QSharedPointer<Ld::TableFrameFormat> newFormat  = format->clone().dynamicCast<Ld::TableFrameFormat>();
                newFormat->setRightIndentation(newRightIndentation);

                formatsByElement.insert(element, newFormat);
            }
        }

        if (!formatsByElement.isEmpty()) {
            issueUpdateFormatCommand(formatsByElement);
        }
    }

}


void TableFormatViewProxy::defaultLineStyle() {
    MainWindow* window = TableFormatViewProxy::window();

    TableDefaultLineStyleDialog tableDefaultLineStyleDialog(window);
    tableDefaultLineStyleDialog.loadSettings(QString("TableDefaultLineStyleDialog"));

    #if (defined(Q_OS_DARWIN))

        tableDefaultLineStyleDialog.setApplyButtonHidden();

    #elif (defined(Q_OS_LINUX) || defined(Q_OS_WIN))

        tableDefaultLineStyleDialog.setApplyButtonVisible();

    #else

        #error Unknown platform

    #endif

    tableDefaultLineStyleDialog.setAutoApplyDisabled();
    tableDefaultLineStyleDialog.setWindowTitle(tr("Default Table Line Styles"));
    tableDefaultLineStyleDialog.setOkCancelButtonVisible();

    const Ld::TableFrameFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();


    QSet<Ld::TableLineSettings::Style> verticalStyles;
    QSet<float>                        verticalWidths;

    QSet<Ld::TableLineSettings>        defaultVerticalLineSettings = aggregation.defaultColumnLineSettings();
    for (  QSet<Ld::TableLineSettings>::const_iterator
               verticalLineSettingsIterator    = defaultVerticalLineSettings.constBegin(),
               verticalLineSettingsEndIterator = defaultVerticalLineSettings.constEnd()
         ; verticalLineSettingsIterator != verticalLineSettingsEndIterator
         ; ++verticalLineSettingsIterator
        ) {
        verticalStyles << verticalLineSettingsIterator->lineStyle();
        verticalWidths << verticalLineSettingsIterator->width();
    }

    QSet<Ld::TableLineSettings::Style> horizontalStyles;
    QSet<float>                        horizontalWidths;

    QSet<Ld::TableLineSettings>        defaultHorizontalLineSettings = aggregation.defaultRowLineSettings();
    for (  QSet<Ld::TableLineSettings>::const_iterator
               horizontalLineSettingsIterator    = defaultHorizontalLineSettings.constBegin(),
               horizontalLineSettingsEndIterator = defaultHorizontalLineSettings.constEnd()
         ; horizontalLineSettingsIterator != horizontalLineSettingsEndIterator
         ; ++horizontalLineSettingsIterator
        ) {
        horizontalStyles << horizontalLineSettingsIterator->lineStyle();
        horizontalWidths << horizontalLineSettingsIterator->width();
    }

    tableDefaultLineStyleDialog.setVerticalLineStyles(verticalStyles);
    tableDefaultLineStyleDialog.setVerticalLineWidths(verticalWidths);

    tableDefaultLineStyleDialog.setHorizontalLineStyles(horizontalStyles);
    tableDefaultLineStyleDialog.setHorizontalLineWidths(horizontalWidths);

    connect(
        &tableDefaultLineStyleDialog,
        &TableDefaultLineStyleDialog::apply,
        this,
        &TableFormatViewProxy::tableDefaultLineStyleDialogApplyUpdates
    );

    int result = tableDefaultLineStyleDialog.exec();
    if (result == TableDefaultLineStyleDialog::Accepted) {
        tableDefaultLineStyleDialogApplyUpdates(&tableDefaultLineStyleDialog);
    }
}


void TableFormatViewProxy::tableDefaultLineStyleDialogApplyUpdates(TableDefaultLineStyleDialog* dialog) {
    dialog->saveSettings(QString("TableDefaultLineStyleDialog"));

    const Ld::TableFrameFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                     elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer                   element = elementIterator->toStrongRef();
        QSharedPointer<Ld::TableFrameFormat> format  = element->format()->clone().dynamicCast<Ld::TableFrameFormat>();
        bool                                 changed = false;

        Ld::TableLineSettings verticalLineSetting   = format->defaultColumnLineSetting();
        Ld::TableLineSettings horizontalLineSetting = format->defaultRowLineSetting();

        if (dialog->verticalLineStyleSet()) {
            TableFormatDialog::LineStyle newLineStyle     = dialog->verticalLineStyle();
            Ld::TableLineSettings::Style currentLineStyle = verticalLineSetting.lineStyle();

            if (newLineStyle != currentLineStyle) {
                verticalLineSetting.setLineStyle(newLineStyle);
                format->setDefaultColumnLineSetting(verticalLineSetting);
                changed = true;
            }
        }

        if (dialog->verticalLineWidthSet()) {
            float newLineWidth     = dialog->verticalLineWidth();
            float currentLineWidth = verticalLineSetting.width();

            if (newLineWidth != currentLineWidth) {
                verticalLineSetting.setWidth(newLineWidth);
                format->setDefaultColumnLineSetting(verticalLineSetting);
                changed = true;
            }
        }

        if (dialog->horizontalLineStyleSet()) {
            TableFormatDialog::LineStyle newLineStyle     = dialog->horizontalLineStyle();
            Ld::TableLineSettings::Style currentLineStyle = horizontalLineSetting.lineStyle();

            if (newLineStyle != currentLineStyle) {
                horizontalLineSetting.setLineStyle(newLineStyle);
                format->setDefaultRowLineSetting(horizontalLineSetting);
                changed = true;
            }
        }

        if (dialog->horizontalLineWidthSet()) {
            float newLineWidth     = dialog->horizontalLineWidth();
            float currentLineWidth = horizontalLineSetting.width();

            if (newLineWidth != currentLineWidth) {
                horizontalLineSetting.setWidth(newLineWidth);
                format->setDefaultRowLineSetting(horizontalLineSetting);
                changed = true;
            }
        }

        if (changed) {
            formatsByElement.insert(element, format);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


void TableFormatViewProxy::updateTableFormatDockState(
        const Ld::AggregationsByCapability& aggregationsByCapability
    ) {
    currentAggregationsByCapability = aggregationsByCapability;

    MainWindow* window = TableFormatViewProxy::window();

    QAction* insertTableAction = window->action("insert_table_dialog");
    QAction* formatTableSettingsDialogAction = window->action("format_table_settings_dialog");
    QAction* formatTableAlignLeftAction = window->action("format_table_align_left");
    QAction* formatTableAlignCenterAction = window->action("format_table_align_center");
    QAction* formatTableAlignRightAction = window->action("format_table_align_right");
    QAction* formatTableMerge = window->action("format_table_merge");
    QAction* formatTableAddRowsDialogAction = window->action("format_table_add_rows_dialog");
    QAction* formatTableAddColumnsDialogAction = window->action("format_table_add_columns_dialog");
    QAction* formatTableRemoveRowsDialogAction = window->action("format_table_remove_rows_dialog");
    QAction* formatTableRemoveColumnsDialogAction = window->action("format_table_remove_columns_dialog");
    QAction* formatTableRemoveTableDialogAction = window->action("format_table_remove_table_dialog");
    QAction* formatTableRowColorAction = window->action("format_table_row_color");
    QAction* formatTableColumnColorAction = window->action("format_table_column_color");
    QAction* formatTableCellColorAction = window->action("format_table_cell_color");
    QAction* formatTableLinesHorizontalAction = window->action("format_table_lines_horizontal");
    QAction* formatTableLinesVerticalAction = window->action("format_table_lines_vertical");

    QAction* tableLeftIndentationAction = window->action("format_table_left_indentation");
    QAction* tableRightIndentationAction = window->action("format_table_right_indentation");
    QAction* tableTopSpacingAction = window->action("format_table_top_spacing");
    QAction* tableBottomSpacingAction = window->action("format_table_bottom_spacing");
    QAction* defaultCellColorAction = window->action("format_default_cell_color_dialog");
    QAction* defaultTableRowLineStyleAction = window->action("format_table_default_line_style");

    if (currentAggregationsByCapability.contains(Ld::TableFrameFormat::formatName)) {
        currentCellsByTable.clear();
        currentRowsByTable.clear();
        currentColumnsByTable.clear();

        TableFormatViewProxy::CellsByTable result;

        bool cellHasColor   = false;
        bool rowHasColor    = false;
        bool columnHasColor = false;

        bool customHorizontalLineSetting = false;
        bool customVerticalLineSetting   = false;

        Ld::ElementPointerList elements = view()->elementsUnderCursor();
        for (  Ld::ElementPointerList::const_iterator elementIterator    = elements.constBegin(),
                                                      elementEndIterator = elements.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = *elementIterator;
            if (!element.isNull()) {
                Ld::ElementPointer parent = element->parent();
                if (!parent.isNull() && parent->typeName() == Ld::TableFrameElement::elementName) {
                    QSharedPointer<Ld::TableFrameElement> table = parent.dynamicCast<Ld::TableFrameElement>();
                    Q_ASSERT(!table.isNull());

                    Ld::TableFrameElement::CellPosition cellPosition = table->cellContainingChild(element);
                    Q_ASSERT(cellPosition.isValid());

                    unsigned rowIndex    = cellPosition.rowIndex();
                    unsigned columnIndex = cellPosition.columnIndex();

                    currentCellsByTable[table]   << cellPosition;
                    currentRowsByTable[table]    << rowIndex;
                    currentColumnsByTable[table] << columnIndex;

                    QSharedPointer<Ld::TableFrameFormat> format = table->format();
                    if (format->cellColorMapsToDefault(rowIndex, columnIndex)) {
                        cellHasColor = true;
                    }

                    if (format->rowColorMapsToDefault(rowIndex)) {
                        rowHasColor = true;
                    }

                    if (format->columnColorMapsToDefault(columnIndex)) {
                        columnHasColor = true;
                    }

                    if (!format->rowLineSettingMapsToDefault(rowIndex + 0) ||
                        !format->rowLineSettingMapsToDefault(rowIndex + 1)    ) {
                        customHorizontalLineSetting = true;
                    }

                    if (!format->columnLineSettingMapsToDefault(columnIndex + 0) ||
                        !format->columnLineSettingMapsToDefault(columnIndex + 1)    ) {
                        customVerticalLineSetting = true;
                    }
                }
            }
        }

        bool singleCell  = currentCellsByTable.count() == 1 && currentCellsByTable.begin().value().count() == 1;
        bool singleTable = currentCellsByTable.count() == 1;

        const Ld::TableFrameFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

        insertTableAction->setEnabled(false);

        const QSet<Ld::TableFrameFormat::Justification>& justificationModes = aggregation.justifications();

        bool leftAlign = (
               justificationModes.contains(Ld::TableFrameFormat::Justification::LEFT)
            || justificationModes.contains(Ld::TableFrameFormat::Justification::JUSTIFY)
        );

        bool centerAlign = justificationModes.contains(Ld::TableFrameFormat::Justification::CENTER);
        bool rightAlign  = justificationModes.contains(Ld::TableFrameFormat::Justification::RIGHT);

        formatTableAlignLeftAction->setChecked(!leftAlign);
        formatTableAlignCenterAction->setChecked(!centerAlign);
        formatTableAlignRightAction->setChecked(!rightAlign);

        const QSet<float>& leftIndentations = aggregation.leftIndentations();
        if (leftIndentations.count() == 1) {
            tableLeftIndentationAction->setData(QVariant(*leftIndentations.begin()));
        } else {
            tableLeftIndentationAction->setData(QVariant());
        }

        const QSet<float>& rightIndentations = aggregation.rightIndentations();
        if (rightIndentations.count() == 1) {
            tableRightIndentationAction->setData(QVariant(*rightIndentations.begin()));
        } else {
            tableRightIndentationAction->setData(QVariant());
        }

        const QSet<float>& topSpacings = aggregation.topSpacings();
        if (topSpacings.count() == 1) {
            tableTopSpacingAction->setData(QVariant(*topSpacings.begin()));
        } else {
            tableTopSpacingAction->setData(QVariant());
        }

        const QSet<float>& bottomSpacings = aggregation.bottomSpacings();
        if (bottomSpacings.count() == 1) {
            tableBottomSpacingAction->setData(QVariant(*bottomSpacings.begin()));
        } else {
            tableBottomSpacingAction->setData(QVariant());
        }

        bool hasSingleRow    = false;
        bool hasSingleColumn = false;
        Ld::TableFrameFormat::Aggregation::const_iterator elementIterator    = aggregation.constBegin();
        Ld::TableFrameFormat::Aggregation::const_iterator elementEndIterator = aggregation.constEnd();
        while (elementIterator != elementEndIterator && (!hasSingleRow || !hasSingleColumn)) {
            Ld::ElementPointer element = *elementIterator;
            if (element->typeName() == Ld::TableFrameElement::elementName) {
                QSharedPointer<Ld::TableFrameElement> tableElement = element.dynamicCast<Ld::TableFrameElement>();
                if (tableElement->numberColumns() == 1) {
                    hasSingleColumn = true;
                }

                if (tableElement->numberRows() == 1) {
                    hasSingleRow = true;
                }
            }

            ++elementIterator;
        }

        formatTableRemoveRowsDialogAction->setEnabled(!hasSingleRow);
        formatTableRemoveColumnsDialogAction->setEnabled(!hasSingleColumn);

        formatTableSettingsDialogAction->setEnabled(true);
        formatTableAlignLeftAction->setEnabled(true);
        formatTableAlignCenterAction->setEnabled(true);
        formatTableAlignRightAction->setEnabled(true);
        formatTableRowColorAction->setEnabled(true);
        formatTableColumnColorAction->setEnabled(true);
        formatTableCellColorAction->setEnabled(true);
        formatTableLinesHorizontalAction->setEnabled(true);
        formatTableLinesVerticalAction->setEnabled(true);

        formatTableRowColorAction->setChecked(rowHasColor);
        formatTableColumnColorAction->setChecked(columnHasColor);
        formatTableCellColorAction->setChecked(cellHasColor);

        formatTableLinesHorizontalAction->setChecked(customHorizontalLineSetting);
        formatTableLinesVerticalAction->setChecked(customVerticalLineSetting);

        tableLeftIndentationAction->setEnabled(true);
        tableRightIndentationAction->setEnabled(true);
        tableTopSpacingAction->setEnabled(true);
        tableBottomSpacingAction->setEnabled(true);
        defaultCellColorAction->setEnabled(true);
        defaultTableRowLineStyleAction->setEnabled(true);

        formatTableAlignLeftAction->trigger();
        formatTableAlignCenterAction->trigger();
        formatTableAlignRightAction->trigger();

        tableLeftIndentationAction->trigger();
        tableRightIndentationAction->trigger();
        tableTopSpacingAction->trigger();
        tableBottomSpacingAction->trigger();

        if (singleCell) {
            formatTableAddRowsDialogAction->setEnabled(true);
            formatTableAddColumnsDialogAction->setEnabled(true);

            QSharedPointer<Ld::TableFrameElement>      tableElement = currentCellsByTable.begin().key();
            const Ld::TableFrameElement::CellPosition& cellPosition = *currentCellsByTable.begin().value().begin();

            formatTableRemoveRowsDialogAction->setEnabled(tableElement->numberRows() > 1);
            formatTableRemoveColumnsDialogAction->setEnabled(tableElement->numberColumns() > 1);

            if (tableElement->isMerged(cellPosition.rowIndex(), cellPosition.columnIndex())) {
                formatTableMerge->setEnabled(true);
                formatTableMerge->setChecked(true);
                formatTableMerge->setToolTip(tr("Unmerge previous merged table cells."));
                formatTableMerge->setWhatsThis(
                    tr("Use this option to unmerge table cells that were previously merged together.")
                );
                formatTableMerge->setText(tr("Un&merge"));
            } else {
                unsigned maximumRowSpan    = tableElement->numberRows() - cellPosition.rowIndex();
                unsigned maximumColumnSpan = tableElement->numberColumns() - cellPosition.columnIndex();

                formatTableMerge->setEnabled(maximumRowSpan > 1 || maximumColumnSpan > 1);
                formatTableMerge->setChecked(false);
                formatTableMerge->setToolTip(tr("Merge table cells together."));
                formatTableMerge->setWhatsThis(tr("Use this option to merge table cells together."));
                formatTableMerge->setText(tr("&Merge..."));
            }
        } else {
            formatTableMerge->setEnabled(false);
            formatTableAddRowsDialogAction->setEnabled(false);
            formatTableAddColumnsDialogAction->setEnabled(false);
            formatTableRemoveRowsDialogAction->setEnabled(false);
            formatTableRemoveColumnsDialogAction->setEnabled(false);

            formatTableMerge->setEnabled(false);
            formatTableMerge->setChecked(false);
            formatTableMerge->setToolTip(tr("Merge table cells together."));
            formatTableMerge->setWhatsThis(tr("Use this option to merge table cells together."));
            formatTableMerge->setText(tr("&Merge..."));
        }

        if (singleTable) {
            formatTableRemoveTableDialogAction->setEnabled(true);
        } else {
            formatTableRemoveTableDialogAction->setEnabled(false);
        }
    } else {
        currentCellsByTable.clear();
        currentRowsByTable.clear();
        currentColumnsByTable.clear();

        insertTableAction->setEnabled(true);

        formatTableAlignLeftAction->setChecked(false);
        formatTableAlignCenterAction->setChecked(false);
        formatTableAlignRightAction->setChecked(false);

        tableLeftIndentationAction->setData(QVariant());
        tableRightIndentationAction->setData(QVariant());
        tableTopSpacingAction->setData(QVariant());
        tableBottomSpacingAction->setData(QVariant());

        formatTableSettingsDialogAction->setEnabled(false);
        formatTableAlignLeftAction->setEnabled(false);
        formatTableAlignCenterAction->setEnabled(false);
        formatTableAlignRightAction->setEnabled(false);
        formatTableMerge->setEnabled(false);
        formatTableAddRowsDialogAction->setEnabled(false);
        formatTableAddColumnsDialogAction->setEnabled(false);
        formatTableRemoveRowsDialogAction->setEnabled(false);
        formatTableRemoveColumnsDialogAction->setEnabled(false);
        formatTableRemoveTableDialogAction->setEnabled(false);
        formatTableRowColorAction->setEnabled(false);
        formatTableColumnColorAction->setEnabled(false);
        formatTableCellColorAction->setEnabled(false);
        formatTableLinesHorizontalAction->setEnabled(false);
        formatTableLinesVerticalAction->setEnabled(false);

        formatTableRowColorAction->setChecked(false);
        formatTableColumnColorAction->setChecked(false);
        formatTableCellColorAction->setChecked(false);

        formatTableLinesHorizontalAction->setChecked(false);
        formatTableLinesVerticalAction->setChecked(false);

        formatTableMerge->setChecked(false);

        tableLeftIndentationAction->setEnabled(false);
        tableRightIndentationAction->setEnabled(false);
        tableTopSpacingAction->setEnabled(false);
        tableBottomSpacingAction->setEnabled(false);
        defaultCellColorAction->setEnabled(false);
        defaultTableRowLineStyleAction->setEnabled(false);

        formatTableAlignLeftAction->trigger();
        formatTableAlignCenterAction->trigger();
        formatTableAlignRightAction->trigger();

        tableLeftIndentationAction->trigger();
        tableRightIndentationAction->trigger();
        tableTopSpacingAction->trigger();
        tableBottomSpacingAction->trigger();
    }
}


void TableFormatViewProxy::connectSignals() {
    MainWindow* window = TableFormatViewProxy::window();
    ViewWidget* view   = TableFormatViewProxy::view();

    connect(view, &ViewWidget::formatsAtCursorChanged, this, &TableFormatViewProxy::formatsUnderCursorChanged);

    QAction* insertTableAction = window->action("insert_table_dialog");
    connect(insertTableAction, &QAction::triggered, this, &TableFormatViewProxy::insertTable);

    QAction* formatTableSettingsDialogAction = window->action("format_table_settings_dialog");
    connect(formatTableSettingsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::formatSettingsDialog);

    QAction* formatTableMerge = window->action("format_table_merge");
    connect(formatTableMerge, &QAction::triggered, this, &TableFormatViewProxy::mergeCells);

    QAction* formatTableAlignLeftAction = window->action("format_table_align_left");
    connect(formatTableAlignLeftAction, &QAction::triggered, this, &TableFormatViewProxy::leftAlign);

    QAction* formatTableAlignCenterAction = window->action("format_table_align_center");
    connect(formatTableAlignCenterAction, &QAction::triggered, this, &TableFormatViewProxy::center);

    QAction* formatTableAlignRightAction = window->action("format_table_align_right");
    connect(formatTableAlignRightAction, &QAction::triggered, this, &TableFormatViewProxy::rightAlign);

    QAction* formatTableAddRowsDialogAction = window->action("format_table_add_rows_dialog");
    connect(formatTableAddRowsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::addRows);

    QAction* formatTableAddColumnsDialogAction = window->action("format_table_add_columns_dialog");
    connect(formatTableAddColumnsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::addColumns);

    QAction* formatTableRemoveRowsDialogAction = window->action("format_table_remove_rows_dialog");
    connect(formatTableRemoveRowsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::removeRows);

    QAction* formatTableRemoveColumnsDialogAction = window->action("format_table_remove_columns_dialog");
    connect(formatTableRemoveColumnsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::removeColumns);

    QAction* formatTableRemoveTableDialogAction = window->action("format_table_remove_table_dialog");
    connect(formatTableRemoveTableDialogAction, &QAction::triggered, this, &TableFormatViewProxy::removeTable);

    QAction* formatTableRowColorAction = window->action("format_table_row_color");
    connect(formatTableRowColorAction, &QAction::triggered, this, &TableFormatViewProxy::rowColorDialog);

    QAction* formatTableColumnColorAction = window->action("format_table_column_color");
    connect(formatTableColumnColorAction, &QAction::triggered, this, &TableFormatViewProxy::columnColorDialog);

    QAction* formatTableCellColorAction = window->action("format_table_cell_color");
    connect(formatTableCellColorAction, &QAction::triggered, this, &TableFormatViewProxy::cellColorDialog);

    QAction* formatTableLinesHorizontalAction = window->action("format_table_lines_horizontal");
    connect(formatTableLinesHorizontalAction, &QAction::triggered, this, &TableFormatViewProxy::horizontalLineDialog);

    QAction* formatTableLinesVerticalAction = window->action("format_table_lines_vertical");
    connect(formatTableLinesVerticalAction, &QAction::triggered, this, &TableFormatViewProxy::verticalLineDialog);

    QAction* tableLeftIndentationAction = window->action("format_table_left_indentation");
    connect(tableLeftIndentationAction, &QAction::triggered, this, &TableFormatViewProxy::leftIndentation);

    QAction* tableRightIndentationAction = window->action("format_table_right_indentation");
    connect(tableRightIndentationAction, &QAction::triggered, this, &TableFormatViewProxy::rightIndentation);

    QAction* tableTopSpacingAction = window->action("format_table_top_spacing");
    connect(tableTopSpacingAction, &QAction::triggered, this, &TableFormatViewProxy::topSpacing);

    QAction* tableBottomSpacingAction = window->action("format_table_bottom_spacing");
    connect(tableBottomSpacingAction, &QAction::triggered, this, &TableFormatViewProxy::bottomSpacing);

    QAction* defaultCellColorAction = window->action("format_default_cell_color_dialog");
    connect(defaultCellColorAction, &QAction::triggered, this, &TableFormatViewProxy::defaultCellColor);

    QAction* defaultTableLineStyleAction = window->action("format_table_default_line_style");
    connect(
        defaultTableLineStyleAction,
        &QAction::triggered,
        this,
        &TableFormatViewProxy::defaultLineStyle
    );
}


void TableFormatViewProxy::disconnectSignals() {
    MainWindow* window = TableFormatViewProxy::window();
    ViewWidget* view   = TableFormatViewProxy::view();

    disconnect(view, &ViewWidget::formatsAtCursorChanged, this, &TableFormatViewProxy::formatsUnderCursorChanged);

    QAction* insertTableAction = window->action("insert_table_dialog");
    disconnect(insertTableAction, &QAction::triggered, this, &TableFormatViewProxy::insertTable);

    QAction* formatTableSettingsDialogAction = window->action("format_table_settings_dialog");
    disconnect(formatTableSettingsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::formatSettingsDialog);

    QAction* formatTableMerge = window->action("format_table_merge");
    disconnect(formatTableMerge, &QAction::triggered, this, &TableFormatViewProxy::mergeCells);

    QAction* formatTableAlignLeftAction = window->action("format_table_align_left");
    disconnect(formatTableAlignLeftAction, &QAction::triggered, this, &TableFormatViewProxy::leftAlign);

    QAction* formatTableAlignCenterAction = window->action("format_table_align_center");
    disconnect(formatTableAlignCenterAction, &QAction::triggered, this, &TableFormatViewProxy::center);

    QAction* formatTableAlignRightAction = window->action("format_table_align_right");
    disconnect(formatTableAlignRightAction, &QAction::triggered, this, &TableFormatViewProxy::rightAlign);

    QAction* formatTableAddRowsDialogAction = window->action("format_table_add_rows_dialog");
    disconnect(formatTableAddRowsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::addRows);

    QAction* formatTableAddColumnsDialogAction = window->action("format_table_add_columns_dialog");
    disconnect(formatTableAddColumnsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::addColumns);

    QAction* formatTableRemoveRowsDialogAction = window->action("format_table_remove_rows_dialog");
    disconnect(formatTableRemoveRowsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::removeRows);

    QAction* formatTableRemoveColumnsDialogAction = window->action("format_table_remove_columns_dialog");
    disconnect(formatTableRemoveColumnsDialogAction, &QAction::triggered, this, &TableFormatViewProxy::removeColumns);

    QAction* formatTableRemoveTableDialogAction = window->action("format_table_remove_table_dialog");
    disconnect(formatTableRemoveTableDialogAction, &QAction::triggered, this, &TableFormatViewProxy::removeTable);

    QAction* formatTableRowColorAction = window->action("format_table_row_color");
    disconnect(formatTableRowColorAction, &QAction::triggered, this, &TableFormatViewProxy::rowColorDialog);

    QAction* formatTableColumnColorAction = window->action("format_table_column_color");
    disconnect(formatTableColumnColorAction, &QAction::triggered, this, &TableFormatViewProxy::columnColorDialog);

    QAction* formatTableCellColorAction = window->action("format_table_cell_color");
    disconnect(formatTableCellColorAction, &QAction::triggered, this, &TableFormatViewProxy::cellColorDialog);

    QAction* formatTableLinesHorizontalAction = window->action("format_table_lines_horizontal");
    disconnect(
        formatTableLinesHorizontalAction,
        &QAction::triggered,
        this,
        &TableFormatViewProxy::horizontalLineDialog
    );

    QAction* formatTableLinesVerticalAction = window->action("format_table_lines_vertical");
    disconnect(formatTableLinesVerticalAction, &QAction::triggered, this, &TableFormatViewProxy::verticalLineDialog);

    QAction* tableLeftIndentationAction = window->action("format_table_left_indentation");
    disconnect(tableLeftIndentationAction, &QAction::triggered, this, &TableFormatViewProxy::leftIndentation);

    QAction* tableRightIndentationAction = window->action("format_table_right_indentation");
    disconnect(tableRightIndentationAction, &QAction::triggered, this, &TableFormatViewProxy::rightIndentation);

    QAction* tableTopSpacingAction = window->action("format_table_top_spacing");
    disconnect(tableTopSpacingAction, &QAction::triggered, this, &TableFormatViewProxy::topSpacing);

    QAction* tableBottomSpacingAction = window->action("format_table_bottom_spacing");
    disconnect(tableBottomSpacingAction, &QAction::triggered, this, &TableFormatViewProxy::bottomSpacing);

    QAction* defaultCellColorAction = window->action("format_default_cell_color_dialog");
    disconnect(defaultCellColorAction, &QAction::triggered, this, &TableFormatViewProxy::defaultCellColor);

    QAction* defaultTableLineStyleAction = window->action("format_table_default_line_style");
    disconnect(
        defaultTableLineStyleAction,
        &QAction::triggered,
        this,
        &TableFormatViewProxy::defaultLineStyle
    );
}


void TableFormatViewProxy::updateTableJustification(Ld::TableFrameFormat::Justification newJustification) {
    const Ld::TableFrameFormat::Aggregation& aggregation =
        currentAggregationsByCapability.aggregationForFormat<Ld::TableFrameFormat>();

    UpdateFormatCommand::FormatsByElement formatsByElement;

    for (  Ld::ElementWeakPointerSet::const_iterator elementIterator    = aggregation.constBegin(),
                                                     elementEndIterator = aggregation.constEnd()
         ; elementIterator != elementEndIterator
         ; ++elementIterator
        ) {
        Ld::ElementPointer                   element = elementIterator->toStrongRef();
        QSharedPointer<Ld::TableFrameFormat> format  = element->format().dynamicCast<Ld::TableFrameFormat>();

        Ld::TableFrameFormat::Justification currentJustification = format->justificationMode();
        if (currentJustification != newJustification) {
            QSharedPointer<Ld::TableFrameFormat> newFormat  = format->clone().dynamicCast<Ld::TableFrameFormat>();
            newFormat->setJustification(newJustification);

            formatsByElement.insert(element, newFormat);
        }
    }

    if (!formatsByElement.isEmpty()) {
        issueUpdateFormatCommand(formatsByElement);
    }
}


bool TableFormatViewProxy::updateFormatFromDialog(
        QSharedPointer<Ld::TableFrameFormat> format,
        TableFormatDialog*                   tableFormatDialog
    ) {
    bool changed = false;

    TableFormatDialog::Justification    newJustification     = tableFormatDialog->justification();
    Ld::TableFrameFormat::Justification currentJustification = format->justificationMode();
    if (newJustification != TableFormatDialog::Justification::NONE && currentJustification != newJustification) {
        format->setJustification(newJustification);
        changed = true;
    }

    if (tableFormatDialog->leftIndentationSet()) {
        float newLeftIndentation     = tableFormatDialog->leftIndentation();
        float currentLeftIndentation = format->leftIndentation();

        if (newLeftIndentation != currentLeftIndentation) {
            format->setLeftIndentation(newLeftIndentation);
            changed = true;
        }
    }

    if (tableFormatDialog->rightIndentationSet()) {
        float newRightIndentation     = tableFormatDialog->rightIndentation();
        float currentRightIndentation = format->rightIndentation();

        if (newRightIndentation != currentRightIndentation) {
            format->setRightIndentation(newRightIndentation);
            changed = true;
        }
    }

    if (tableFormatDialog->topSpacingSet()) {
        float newTopSpacing     = tableFormatDialog->topSpacing();
        float currentTopSpacing = format->topSpacing();

        if (newTopSpacing != currentTopSpacing) {
            format->setTopSpacing(newTopSpacing);
            changed = true;
        }
    }

    if (tableFormatDialog->bottomSpacingSet()) {
        float newBottomSpacing     = tableFormatDialog->bottomSpacing();
        float currentBottomSpacing = format->bottomSpacing();

        if (newBottomSpacing != currentBottomSpacing) {
            format->setBottomSpacing(newBottomSpacing);
            changed = true;
        }
    }

    if (tableFormatDialog->leftGutterSet()) {
        float newLeftGutter     = tableFormatDialog->leftGutter();
        float currentLeftGutter = format->leftGutter();

        if (newLeftGutter != currentLeftGutter) {
            format->setLeftGutter(newLeftGutter);
            changed = true;
        }
    }

    if (tableFormatDialog->rightGutterSet()) {
        float newRightGutter     = tableFormatDialog->rightGutter();
        float currentRightGutter = format->rightGutter();

        if (newRightGutter != currentRightGutter) {
            format->setRightGutter(newRightGutter);
            changed = true;
        }
    }

    if (tableFormatDialog->topGutterSet()) {
        float newTopGutter     = tableFormatDialog->topGutter();
        float currentTopGutter = format->topGutter();

        if (newTopGutter != currentTopGutter) {
            format->setTopGutter(newTopGutter);
            changed = true;
        }
    }

    if (tableFormatDialog->bottomGutterSet()) {
        float newBottomGutter     = tableFormatDialog->bottomGutter();
        float currentBottomGutter = format->bottomGutter();

        if (newBottomGutter != currentBottomGutter) {
            format->setBottomGutter(newBottomGutter);
            changed = true;
        }
    }

    Ld::TableLineSettings verticalLineSetting        = format->defaultColumnLineSetting();
    bool                  verticalLineSettingChanged = false;

    if (tableFormatDialog->verticalLineStyleSet()) {
        TableFormatDialog::LineStyle newLineStyle     = tableFormatDialog->verticalLineStyle();
        Ld::TableLineSettings::Style currentLineStyle = verticalLineSetting.lineStyle();

        if (newLineStyle != currentLineStyle) {
            verticalLineSetting.setLineStyle(newLineStyle);
            verticalLineSettingChanged = true;
        }
    }

    if (tableFormatDialog->verticalLineWidthSet()) {
        float newLineWidth     = tableFormatDialog->verticalLineWidth();
        float currentLineWidth = verticalLineSetting.width();

        if (newLineWidth != currentLineWidth) {
            verticalLineSetting.setWidth(newLineWidth);
            verticalLineSettingChanged = true;
        }
    }

    if (verticalLineSettingChanged) {
        format->setDefaultColumnLineSetting(verticalLineSetting);
        changed = true;
    }

    Ld::TableLineSettings horizontalLineSetting        = format->defaultRowLineSetting();
    bool                  horizontalLineSettingChanged = false;

    if (tableFormatDialog->horizontalLineStyleSet()) {
        TableFormatDialog::LineStyle newLineStyle     = tableFormatDialog->horizontalLineStyle();
        Ld::TableLineSettings::Style currentLineStyle = horizontalLineSetting.lineStyle();

        if (newLineStyle != currentLineStyle) {
            horizontalLineSetting.setLineStyle(newLineStyle);
            horizontalLineSettingChanged = true;
        }
    }

    if (tableFormatDialog->horizontalLineWidthSet()) {
        float newLineWidth     = tableFormatDialog->horizontalLineWidth();
        float currentLineWidth = horizontalLineSetting.width();

        if (newLineWidth != currentLineWidth) {
            horizontalLineSetting.setWidth(newLineWidth);
            horizontalLineSettingChanged = true;
        }
    }

    if (horizontalLineSettingChanged) {
        format->setDefaultRowLineSetting(horizontalLineSetting);
        changed = true;
    }

    if (tableFormatDialog->defaultCellColorSet()) {
        QColor newDefaultCellColor     = tableFormatDialog->defaultCellColor();
        QColor currentDefaultCellColor = format->defaultColor();

        if (newDefaultCellColor != currentDefaultCellColor) {
            format->setDefaultColor(newDefaultCellColor);
            changed = true;
        }
    }

    QList<TableFormatDialog::ColumnWidth>           newColumnWidths = tableFormatDialog->columnWidths();
    unsigned                                        numberColumns   = static_cast<unsigned>(newColumnWidths.size());
    QHash<TableFormatDialog::ColumnWidth, unsigned> columnWidthCounts;

    for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
        const TableFormatDialog::ColumnWidth& columnWidth = newColumnWidths.at(columnIndex);
        if (columnWidthCounts.contains(columnWidth)) {
            ++columnWidthCounts[columnWidth];
        } else {
            columnWidthCounts.insert(columnWidth, 1);
        }
    }

    TableFormatDialog::ColumnWidth bestDefaultColumn      = columnWidthCounts.constBegin().key();
    unsigned                       bestDefaultColumnCount = columnWidthCounts.constBegin().value();

    for (  QHash<TableFormatDialog::ColumnWidth, unsigned>::const_iterator
               widthsIterator    = columnWidthCounts.constBegin(),
               widthsEndIterator = columnWidthCounts.constEnd()
         ; widthsIterator != widthsEndIterator
         ; ++widthsIterator
        ) {
        if (widthsIterator.value() > bestDefaultColumnCount) {
            bestDefaultColumn      = widthsIterator.key();
            bestDefaultColumnCount = widthsIterator.value();
        }
    }

    if (bestDefaultColumn != format->defaultColumnWidth()) {
        format->setDefaultColumnWidth(bestDefaultColumn);
        changed = true;
    }

    for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
        const Ld::TableColumnWidth&           oldColumnWidth = format->columnWidth(columnIndex);
        const TableFormatDialog::ColumnWidth& newColumnWidth = newColumnWidths.at(columnIndex);

        if (oldColumnWidth != newColumnWidth) {
            if (newColumnWidth == bestDefaultColumn) {
                format->setColumnToDefaultWidth(columnIndex);
            } else {
                format->setColumnWidth(columnIndex, newColumnWidth);
            }

            changed = true;
        }
    }

    return changed;
}


void TableFormatViewProxy::issueUpdateFormatCommand(
        const UpdateFormatCommand::FormatsByElement& formatsByElement,
        bool                                         allowMerge
    ) {
    ViewWidget* view = TableFormatViewProxy::view();
    view->insertCommand(new UpdateFormatCommand(formatsByElement, allowMerge));
}


void TableFormatViewProxy::restoreFocus() {
    view()->restoreFocus();
}
