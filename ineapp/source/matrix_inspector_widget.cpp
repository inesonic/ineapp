/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QInputDialog>
#include <QProgressDialog>
#include <QFont>
#include <QFontMetrics>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "inspector_widget.h"
#include "matrix_inspector_model.h"
#include "inspector_widget.h"
#include "matrix_inspector_widget.h"

MatrixInspectorWidget::MatrixInspectorWidget(QWidget* parent):InspectorWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    setLayout(layout);

    currentTableView = new QTableView;
    layout->addWidget(currentTableView);

    currentTableView->horizontalHeader()->setDefaultAlignment(Qt::AlignRight);
    currentTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    currentTableView->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    currentTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(
        currentTableView,
        &QTableView::customContextMenuRequested,
        this,
        &MatrixInspectorWidget::customContextMenuRequested
    );
}


MatrixInspectorWidget::~MatrixInspectorWidget() {}


Ld::CalculatedValue MatrixInspectorWidget::calculatedValue() const {
    MatrixInspectorModel* model = dynamic_cast<MatrixInspectorModel*>(currentTableView->model());
    return model->currentCalculatedValue();
}


bool MatrixInspectorWidget::calculatedValueChanged() const {
    return false;
}


bool MatrixInspectorWidget::includeLoadSaveSupport() const {
    return   currentTableView->model() != Q_NULLPTR
           ? dynamic_cast<const MatrixInspectorModel*>(currentTableView->model())->includeLoadSaveSupport()
           : false;
}


void MatrixInspectorWidget::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    MatrixInspectorModel* model;
    if (currentTableView->model() == Q_NULLPTR) {
        currentTableView->horizontalHeader()->setDefaultSectionSize(initialColumnWidth());
        currentTableView->verticalHeader()->setDefaultSectionSize(initialRowHeight());

        model = createModel(calculatedValue);
        currentTableView->setModel(model);

        connect(model, &MatrixInspectorModel::valueChanged,  this, &InspectorWidget::valueChanged);
        connect(model, &MatrixInspectorModel::valueRestored, this, &InspectorWidget::valueRestored);

        QAbstractItemDelegate* newDelegate = createDelegate();
        if (newDelegate != Q_NULLPTR) {
            QAbstractItemDelegate* oldDelegate = currentTableView->itemDelegate();

            if (oldDelegate != Q_NULLPTR) {
                oldDelegate->deleteLater();
            }

            currentTableView->setItemDelegate(newDelegate);
        }

        QItemSelectionModel* newSelectionModel = createSelectionModel(currentTableView, model);
        if (newSelectionModel != Q_NULLPTR) {
            QItemSelectionModel* oldSelectionModel = currentTableView->selectionModel();

            if (oldSelectionModel != Q_NULLPTR) {
                oldSelectionModel->deleteLater();
            }

            currentTableView->setSelectionModel(newSelectionModel);
        }
    } else {
        model = dynamic_cast<MatrixInspectorModel*>(currentTableView->model());
        model->setCalculatedValue(calculatedValue);
    }
}


void MatrixInspectorWidget::resetCalculatedValue() {
    if (currentTableView->model() != Q_NULLPTR) {
        MatrixInspectorModel* model = dynamic_cast<MatrixInspectorModel*>(currentTableView->model());
        model->resetModel();
    }
}


QAbstractItemDelegate* MatrixInspectorWidget::createDelegate() {
    return Q_NULLPTR;
}


QItemSelectionModel* MatrixInspectorWidget::createSelectionModel(
        QTableView*           /* tableView */,
        MatrixInspectorModel* /* model */
    ) {
    return Q_NULLPTR;
}


unsigned MatrixInspectorWidget::initialRowHeight() const {
    QFont font = currentTableView->font();
    QFontMetrics fontMetrics(font);

    return fontMetrics.height() * 1.25;
}


QFont MatrixInspectorWidget::tableFont() const {
    return currentTableView->font();
}


bool MatrixInspectorWidget::loadFromFile(const QString& filename, bool binaryFormat) {
    MatrixInspectorModel* model = dynamic_cast<MatrixInspectorModel*>(currentTableView->model());

    bool success = model->loadValue(filename, binaryFormat);

    if (success) {
        currentTableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    }

    return success;
}


bool MatrixInspectorWidget::saveToFile(const QString& filename, bool binaryFormat) {
    MatrixInspectorModel* model = dynamic_cast<MatrixInspectorModel*>(currentTableView->model());
    return model->saveValue(filename, binaryFormat);
}


void MatrixInspectorWidget::customContextMenuRequested(const QPoint& position) {
    QMenu menu(currentTableView);

    QAction* autoAdjustColumnsAction = menu.addAction(tr("Optimal column &width..."));

    menu.addSeparator();

    QAction* insertRowsBeforeAction = menu.addAction(tr("Insert &Rows Before..."));
    QAction* insertRowsAfterAction  = menu.addAction(tr("Insert &Rows After..."));
    QAction* removeRowAction        = menu.addAction(tr("Remove R&ow"));

    menu.addSeparator();

    QAction* insertColumnsBeforeAction = menu.addAction(tr("Insert &Columns Before..."));
    QAction* insertColumnsAfterAction  = menu.addAction(tr("Insert &Columns After..."));
    QAction* removeColumnAction        = menu.addAction(tr("Remove Co&lumn"));

    connect(autoAdjustColumnsAction,   &QAction::triggered, this, &MatrixInspectorWidget::autoAdjustColumns);
    connect(insertRowsBeforeAction,    &QAction::triggered, this, &MatrixInspectorWidget::insertRowsBefore);
    connect(insertRowsAfterAction,     &QAction::triggered, this, &MatrixInspectorWidget::insertRowsAfter);
    connect(removeRowAction,           &QAction::triggered, this, &MatrixInspectorWidget::removeRow);
    connect(insertColumnsBeforeAction, &QAction::triggered, this, &MatrixInspectorWidget::insertColumnsBefore);
    connect(insertColumnsAfterAction,  &QAction::triggered, this, &MatrixInspectorWidget::insertColumnsAfter);
    connect(removeColumnAction,        &QAction::triggered, this, &MatrixInspectorWidget::removeColumn);

    QPoint globalPosition = currentTableView->mapToGlobal(position);
    menu.exec(globalPosition);
}


void MatrixInspectorWidget::autoAdjustColumns() {
    MatrixInspectorModel* model         = dynamic_cast<MatrixInspectorModel*>(currentTableView->model());
    unsigned long         numberColumns = static_cast<unsigned long>(model->columnCount());

    QProgressDialog progressDialog(tr("Resizing columns..."), tr("Cancel"), 1, numberColumns, this);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setMinimumDuration(1000);

    unsigned long columnIndex = 0;
    unsigned long stepSize    = numberColumns / 200;
    if (stepSize == 0) {
        stepSize = 1;
    }

    unsigned long stepCount = 0;
    while (!progressDialog.wasCanceled() && columnIndex < numberColumns) {
        currentTableView->resizeColumnToContents(columnIndex);

        ++stepCount;
        if (stepCount == stepSize) {
            progressDialog.setValue(columnIndex + 1);
            stepCount = 0;
        }

        ++columnIndex;
    }

    progressDialog.setValue(numberColumns);
}


void MatrixInspectorWidget::insertRowsBefore() {
    QItemSelectionModel* selectionModel = currentTableView->selectionModel();

    if (!selectionModel->hasSelection()) {
        QModelIndex   currentIndex = selectionModel->currentIndex();
        unsigned long rowIndex     = static_cast<unsigned long>(currentIndex.row());

        currentTableView->model()->insertRows(rowIndex, 1);
    } else {
        QMap<unsigned long, unsigned long> selectedRows = rowRanges(selectionModel);

        for (  QMap<unsigned long, unsigned long>::const_iterator it  = selectedRows.constBegin(),
                                                                  end = selectedRows.constEnd()
             ; it != end
             ; ++it
            ) {
            currentTableView->model()->insertRows(it.key(), it.value());
        }
    }
}


void MatrixInspectorWidget::insertRowsAfter() {
    QItemSelectionModel* selectionModel = currentTableView->selectionModel();

    if (selectionModel->hasSelection()) {
        QModelIndex   currentIndex = selectionModel->currentIndex();
        unsigned long rowIndex     = static_cast<unsigned long>(currentIndex.row());

        currentTableView->model()->insertRows(rowIndex, 1);
    } else {
        QMap<unsigned long, unsigned long> selectedRows = rowRanges(selectionModel);

        for (  QMap<unsigned long, unsigned long>::const_iterator it  = selectedRows.constBegin(),
                                                                  end = selectedRows.constEnd()
             ; it != end
             ; ++it
            ) {
            currentTableView->model()->insertRows(it.key() + 1, it.value());
        }
    }
}


void MatrixInspectorWidget::insertColumnsBefore() {
    QItemSelectionModel* selectionModel = currentTableView->selectionModel();

    if (!selectionModel->hasSelection()) {
        QModelIndex   currentIndex = selectionModel->currentIndex();
        unsigned long columnIndex  = static_cast<unsigned long>(currentIndex.column());

        currentTableView->model()->insertColumns(columnIndex, 1);
    } else {
        QMap<unsigned long, unsigned long> selectedColumns = columnRanges(selectionModel);

        for (  QMap<unsigned long, unsigned long>::const_iterator it  = selectedColumns.constBegin(),
                                                                  end = selectedColumns.constEnd()
             ; it != end
             ; ++it
            ) {
            currentTableView->model()->insertColumns(it.key(), it.value());
        }
    }
}


void MatrixInspectorWidget::insertColumnsAfter() {
    QItemSelectionModel* selectionModel = currentTableView->selectionModel();

    if (!selectionModel->hasSelection()) {
        QModelIndex   currentIndex = selectionModel->currentIndex();
        unsigned long columnIndex  = static_cast<unsigned long>(currentIndex.column());

        currentTableView->model()->insertColumns(columnIndex, 1);
    } else {
        QMap<unsigned long, unsigned long> selectedColumns = columnRanges(selectionModel);

        for (  QMap<unsigned long, unsigned long>::const_iterator it  = selectedColumns.constBegin(),
                                                                  end = selectedColumns.constEnd()
             ; it != end
             ; ++it
            ) {
            currentTableView->model()->insertColumns(it.key() + 1, it.value());
        }
    }
}


void MatrixInspectorWidget::removeRow() {
    QItemSelectionModel* selectionModel = currentTableView->selectionModel();

    if (!selectionModel->hasSelection()) {
        QModelIndex   currentIndex = selectionModel->currentIndex();
        unsigned long rowIndex     = static_cast<unsigned long>(currentIndex.row());

        currentTableView->model()->removeRows(rowIndex, 1);
    } else {
        QMap<unsigned long, unsigned long> selectedRows = rowRanges(selectionModel);

        QMap<unsigned long, unsigned long>::const_iterator it    = selectedRows.constEnd();
        unsigned long                                      count = static_cast<unsigned long>(selectedRows.size());
        for (unsigned long i=0 ; i<count ; ++i) {
            --it;
            currentTableView->model()->removeRows(it.key(), it.value());
        }
    }
}


void MatrixInspectorWidget::removeColumn() {
    QItemSelectionModel* selectionModel = currentTableView->selectionModel();

    if (!selectionModel->hasSelection()) {
        QModelIndex   currentIndex = selectionModel->currentIndex();
        unsigned long columnIndex  = static_cast<unsigned long>(currentIndex.column());

        currentTableView->model()->removeColumns(columnIndex, 1);
    } else {
        QMap<unsigned long, unsigned long> selectedColumns = columnRanges(selectionModel);

        QMap<unsigned long, unsigned long>::const_iterator it    = selectedColumns.constEnd();
        unsigned long                                      count = static_cast<unsigned long>(selectedColumns.size());
        for (unsigned long i=0 ; i<count ; ++i) {
            --it;
            currentTableView->model()->removeColumns(it.key(), it.value());
        }
    }
}


QMap<unsigned long, unsigned long> MatrixInspectorWidget::rowRanges(const QItemSelectionModel* selectionModel) {
    QModelIndexList           indexList = selectionModel->selectedIndexes();
    QMap<unsigned long, char> rowList;

    for (QModelIndexList::const_iterator it=indexList.constBegin(),end=indexList.constEnd() ; it!=end ; ++it) {
        rowList.insert(it->row(), 1);
    }

    QMap<unsigned long, unsigned long> result;
    unsigned long                      startingRow = static_cast<unsigned long>(-1);
    unsigned long                      numberRows  = 0;

    for (  QMap<unsigned long, char>::const_iterator rowListIterator    = rowList.constBegin(),
                                                     rowListEndIterator = rowList.constEnd()
         ; rowListIterator != rowListEndIterator
         ; ++rowListIterator
        ) {
        if (numberRows == 0) {
            startingRow = rowListIterator.key();
            numberRows  = 1;
        } else if (rowListIterator.key() == startingRow + numberRows) {
            ++numberRows;
        } else {
            result.insert(startingRow, numberRows);
            startingRow = rowListIterator.key();
            numberRows  = 1;
        }
    }

    if (numberRows > 0) {
        result.insert(startingRow, numberRows);
    }

    return result;
}


QMap<unsigned long, unsigned long> MatrixInspectorWidget::columnRanges(const QItemSelectionModel* selectionModel) {
    QModelIndexList           indexList = selectionModel->selectedIndexes();
    QMap<unsigned long, char> columnList;

    for (QModelIndexList::const_iterator it=indexList.constBegin(),end=indexList.constEnd() ; it!=end ; ++it) {
        columnList.insert(it->column(), 1);
    }

    QMap<unsigned long, unsigned long> result;
    unsigned long                      startingColumn = static_cast<unsigned long>(-1);
    unsigned long                      numberColumns  = 0;

    for (  QMap<unsigned long, char>::const_iterator columnListIterator    = columnList.constBegin(),
                                                     columnListEndIterator = columnList.constEnd()
         ; columnListIterator != columnListEndIterator
         ; ++columnListIterator
        ) {
        if (numberColumns == 0) {
            startingColumn = columnListIterator.key();
            numberColumns  = 1;
        } else if (columnListIterator.key() == startingColumn + numberColumns) {
            ++numberColumns;
        } else {
            result.insert(startingColumn, numberColumns);
            startingColumn = columnListIterator.key();
            numberColumns  = 1;
        }
    }

    if (numberColumns > 0) {
        result.insert(startingColumn, numberColumns);
    }

    return result;
}
