/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TupleInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QToolButton>
#include <QPoint>
#include <QMenu>
#include <QMessageBox>

#include <model_api_types.h>
#include <model_set.h>
#include <model_tuple.h>
#include <model_matrix_boolean.h>
#include <model_matrix_integer.h>
#include <model_matrix_real.h>
#include <model_matrix_complex.h>
#include <model_variant.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>

#include "application.h"
#include "tuple_inspector_model.h"
#include "inspector_widget.h"
#include "inspector_dialog.h"
#include "tuple_inspector_widget.h"

InspectorWidget* TupleInspectorWidget::creator(QWidget* parent) {
    return new TupleInspectorWidget(parent);
}


TupleInspectorWidget::TupleInspectorWidget(QWidget* parent):InspectorWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    QHBoxLayout* topHorizontalLayout = new QHBoxLayout;
    mainLayout->addLayout(topHorizontalLayout);

    currentTableView = new QTableView;
    topHorizontalLayout->addWidget(currentTableView);

    TupleInspectorModel*  model    = new TupleInspectorModel(currentTableView);
    QAbstractItemModel*   oldModel = currentTableView->model();

    currentTableView->setModel(model);
    if (oldModel != Q_NULLPTR) {
        oldModel->deleteLater();
    }

    currentTableView->horizontalHeader()->setVisible(false);
    currentTableView->setShowGrid(false);
    currentTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    currentTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    currentTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    currentTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    currentTableView->setSelectionBehavior(QTableView::SelectRows);

    connect(model, &TupleInspectorModel::valueChanged, this, &TupleInspectorWidget::valueChanged);
    connect(model, &TupleInspectorModel::valueRestored, this, &TupleInspectorWidget::valueRestored);

    connect(currentTableView, &QTableView::doubleClicked, this, &TupleInspectorWidget::itemDoubleClicked);
    connect(
        currentTableView,
        &QTableView::customContextMenuRequested,
        this,
        &TupleInspectorWidget::customContextMenuRequested
    );

    connect(
        currentTableView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &TupleInspectorWidget::selectionChanged
    );


    QHBoxLayout* addRemoveButtonLayout = new QHBoxLayout;
    mainLayout->addLayout(addRemoveButtonLayout);

    insertButton = new QPushButton(Application::icon("add_entry"), QString());
    deleteButton = new QPushButton(Application::icon("remove_entry"), QString());

    addRemoveButtonLayout->addWidget(insertButton);
    addRemoveButtonLayout->addWidget(deleteButton);
    addRemoveButtonLayout->addStretch(1);

    QMenu* insertMenu = new QMenu(this);
    insertButton->setMenu(insertMenu);

    QAction* insertBoolean = insertMenu->addAction("&Boolean...");
    QAction* insertInteger = insertMenu->addAction("&Integer...");
    QAction* insertReal    = insertMenu->addAction("&Real...");
    QAction* insertComplex = insertMenu->addAction("&Complex...");

    insertMenu->addSeparator();

    QAction* insertSet   = insertMenu->addAction("&Set...");
    QAction* insertTuple = insertMenu->addAction("&Tuple...");

    insertMenu->addSeparator();

    QAction* insertMatrixBoolean = insertMenu->addAction("B&oolean Matrix...");
    QAction* insertMatrixInteger = insertMenu->addAction("I&nteger Matrix...");
    QAction* insertMatrixReal    = insertMenu->addAction("R&eal Matrix...");
    QAction* insertMatrixComplex = insertMenu->addAction("Comple&x Matrix...");

    QVBoxLayout* upDownButtonLayout = new QVBoxLayout;
    topHorizontalLayout->addLayout(upDownButtonLayout);

    moveUpButton   = new QPushButton(Application::icon("move_up"), QString());
    moveDownButton = new QPushButton(Application::icon("move_down"), QString());

    upDownButtonLayout->addWidget(moveUpButton);
    upDownButtonLayout->addStretch(1);
    upDownButtonLayout->addWidget(moveDownButton);

    connect(deleteButton, &QPushButton::clicked, this, &TupleInspectorWidget::deleteItems);

    connect(insertBoolean,       &QAction::triggered, this, &TupleInspectorWidget::insertBoolean);
    connect(insertInteger,       &QAction::triggered, this, &TupleInspectorWidget::insertInteger);
    connect(insertReal,          &QAction::triggered, this, &TupleInspectorWidget::insertReal);
    connect(insertComplex,       &QAction::triggered, this, &TupleInspectorWidget::insertComplex);
    connect(insertSet,           &QAction::triggered, this, &TupleInspectorWidget::insertSet);
    connect(insertTuple,         &QAction::triggered, this, &TupleInspectorWidget::insertTuple);
    connect(insertMatrixBoolean, &QAction::triggered, this, &TupleInspectorWidget::insertMatrixBoolean);
    connect(insertMatrixInteger, &QAction::triggered, this, &TupleInspectorWidget::insertMatrixInteger);
    connect(insertMatrixReal,    &QAction::triggered, this, &TupleInspectorWidget::insertMatrixReal);
    connect(insertMatrixComplex, &QAction::triggered, this, &TupleInspectorWidget::insertMatrixComplex);

    connect(moveUpButton,   &QPushButton::clicked, this, &TupleInspectorWidget::moveItemsUp);
    connect(moveDownButton, &QPushButton::clicked, this, &TupleInspectorWidget::moveItemsDown);

    moveUpButton->setEnabled(false);
    moveDownButton->setEnabled(false);
    deleteButton->setEnabled(false);
}


TupleInspectorWidget::~TupleInspectorWidget() {}


Ld::CalculatedValue TupleInspectorWidget::calculatedValue() const {
    const TupleInspectorModel* model = dynamic_cast<const TupleInspectorModel*>(currentTableView->model());
    return model->currentCalculatedValue();
}


bool TupleInspectorWidget::calculatedValueChanged() const {
    const TupleInspectorModel* model = dynamic_cast<const TupleInspectorModel*>(currentTableView->model());
    return model->calculatedValueChanged();
}


void TupleInspectorWidget::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    TupleInspectorModel* model = dynamic_cast<TupleInspectorModel*>(currentTableView->model());
    model->setCalculatedValue(calculatedValue);
}


void TupleInspectorWidget::resetCalculatedValue() {
    TupleInspectorModel* model = dynamic_cast<TupleInspectorModel*>(currentTableView->model());
    model->resetModel();
}


void TupleInspectorWidget::selectionChanged(
        const QItemSelection& /* selection */,
        const QItemSelection& /* deselected */
    ) {
    QModelIndexList      indexes = currentTableView->selectionModel()->selection().indexes();
    QMap<unsigned, char> rows;

    for (QModelIndexList::const_iterator it=indexes.constBegin(),end=indexes.constEnd() ; it!=end ; ++it) {
        rows.insert(it->row(), 1);
    }

    TupleInspectorModel* model      = dynamic_cast<TupleInspectorModel*>(currentTableView->model());
    unsigned long        numberRows = static_cast<unsigned long>(model->rowCount());

    if (rows.isEmpty() || numberRows == 0) {
        moveUpButton->setEnabled(false);
        moveDownButton->setEnabled(false);
        deleteButton->setEnabled(false);
    } else if (numberRows == static_cast<unsigned long>(rows.size())) {
        moveUpButton->setEnabled(false);
        moveDownButton->setEnabled(false);
        deleteButton->setEnabled(true);
    } else {
        moveUpButton->setEnabled(!rows.contains(0));
        moveDownButton->setEnabled(!rows.contains(numberRows - 1));
        deleteButton->setEnabled(true);
    }

    insertButton->setEnabled(rows.size() == 1);
}


void TupleInspectorWidget::itemDoubleClicked(const QModelIndex& index) {
    unsigned long         rowIndex = static_cast<unsigned long>(index.row());
    TupleInspectorModel*  model    = dynamic_cast<TupleInspectorModel*>(currentTableView->model());
    const Model::Variant& value    = model->valueAt(rowIndex);

    Ld::CalculatedValue cv(Ld::VariableName(), value);

    InspectorDialog dialog(cv, this);
    int result = dialog.exec();

    if (result == InspectorDialog::Accepted) {
        Ld::CalculatedValue   nv       = dialog.calculatedValue();
        const Model::Variant& newValue = nv.variant();

        model->replaceValue(rowIndex, newValue);
    }
}


void TupleInspectorWidget::customContextMenuRequested(const QPoint& position) {
    QMenu menu(currentTableView);

    QAction* inspectModifyAction = menu.addAction("I&nspect/Modify...");
    QAction* deleteAction        = menu.addAction(tr("&Delete"));
    QMenu*   insertMenu          = menu.addMenu(tr("&Insert"));

    QAction* insertBoolean = insertMenu->addAction("&Boolean...");
    QAction* insertInteger = insertMenu->addAction("&Integer...");
    QAction* insertReal    = insertMenu->addAction("&Real...");
    QAction* insertComplex = insertMenu->addAction("&Complex...");

    insertMenu->addSeparator();

    QAction* insertSet   = insertMenu->addAction("&Set...");
    QAction* insertTuple = insertMenu->addAction("&Tuple...");

    insertMenu->addSeparator();

    QAction* insertMatrixBoolean = insertMenu->addAction("B&oolean Matrix...");
    QAction* insertMatrixInteger = insertMenu->addAction("I&nteger Matrix...");
    QAction* insertMatrixReal    = insertMenu->addAction("R&ealr Matrix...");
    QAction* insertMatrixComplex = insertMenu->addAction("Comple&x Matrix...");

    connect(inspectModifyAction, &QAction::triggered, this, &TupleInspectorWidget::inspectModifyItem);
    connect(deleteAction,        &QAction::triggered, this, &TupleInspectorWidget::deleteItems);
    connect(insertBoolean,       &QAction::triggered, this, &TupleInspectorWidget::insertBoolean);
    connect(insertInteger,       &QAction::triggered, this, &TupleInspectorWidget::insertInteger);
    connect(insertReal,          &QAction::triggered, this, &TupleInspectorWidget::insertReal);
    connect(insertComplex,       &QAction::triggered, this, &TupleInspectorWidget::insertComplex);
    connect(insertSet,           &QAction::triggered, this, &TupleInspectorWidget::insertSet);
    connect(insertTuple,         &QAction::triggered, this, &TupleInspectorWidget::insertTuple);
    connect(insertMatrixBoolean, &QAction::triggered, this, &TupleInspectorWidget::insertMatrixBoolean);
    connect(insertMatrixInteger, &QAction::triggered, this, &TupleInspectorWidget::insertMatrixInteger);
    connect(insertMatrixReal,    &QAction::triggered, this, &TupleInspectorWidget::insertMatrixReal);
    connect(insertMatrixComplex, &QAction::triggered, this, &TupleInspectorWidget::insertMatrixComplex);

    QPoint globalPosition = currentTableView->mapToGlobal(position);
    menu.exec(globalPosition);
}


void TupleInspectorWidget::moveItemsUp() {
    QModelIndexList      indexes = currentTableView->selectionModel()->selection().indexes();
    QMap<unsigned long, char> rows;
    for (QModelIndexList::const_iterator it=indexes.constBegin(),end=indexes.constEnd() ; it!=end ; ++it) {
        rows.insert(it->row(), 1);
    }

    if (!rows.isEmpty()) {
        TupleInspectorModel* model  = dynamic_cast<TupleInspectorModel*>(currentTableView->model());
        unsigned long        newRow = rows.firstKey();

        if (newRow > 0) {
            --newRow;
        }

        QList<unsigned long> rowList = rows.keys();
        model->mergeAndRelocate(newRow, rowList);
    }

    moveUpButton->setEnabled(false);
    moveDownButton->setEnabled(false);
    deleteButton->setEnabled(false);
    insertButton->setEnabled(false);
}


void TupleInspectorWidget::moveItemsDown() {
    QModelIndexList           indexes = currentTableView->selectionModel()->selection().indexes();
    QMap<unsigned long, char> rows;
    for (QModelIndexList::const_iterator it=indexes.constBegin(),end=indexes.constEnd() ; it!=end ; ++it) {
        rows.insert(it->row(), 1);
    }

    if (!rows.isEmpty()) {
        TupleInspectorModel* model  = dynamic_cast<TupleInspectorModel*>(currentTableView->model());
        unsigned long        newRow = rows.lastKey();

        if ((newRow + 1) < static_cast<unsigned long>(model->rowCount())) {
            newRow += 2;
        }

        QList<unsigned long> rowList = rows.keys();
        model->mergeAndRelocate(newRow, rowList);
    }

    moveUpButton->setEnabled(false);
    moveDownButton->setEnabled(false);
    deleteButton->setEnabled(false);
    insertButton->setEnabled(false);
}


void TupleInspectorWidget::inspectModifyItem() {
    QModelIndex modelIndex = currentTableView->selectionModel()->currentIndex();
    itemDoubleClicked(modelIndex);
}


void TupleInspectorWidget::deleteItems() {
    QModelIndexList      indexes = currentTableView->selectionModel()->selectedIndexes();
    TupleInspectorModel* model   = dynamic_cast<TupleInspectorModel*>(currentTableView->model());

    QMap<unsigned long, char> selectedRows;
    for (  QModelIndexList::const_iterator indexIterator = indexes.constBegin(), indexEndIterator = indexes.constEnd()
         ; indexIterator != indexEndIterator
         ; ++indexIterator
        ) {
        unsigned long rowIndex = indexIterator->row();
        selectedRows.insert(rowIndex, 0);
    }

    QMap<unsigned long, unsigned long> rowGroups;
    unsigned long                      startingRow = static_cast<unsigned long>(-1);
    unsigned long                      numberRows  = 0;

    for (  QMap<unsigned long, char>::const_iterator rowListIterator    = selectedRows.constBegin(),
                                                     rowListEndIterator = selectedRows.constEnd()
         ; rowListIterator != rowListEndIterator
         ; ++rowListIterator
        ) {
        if (numberRows == 0) {
            startingRow = rowListIterator.key();
            numberRows  = 1;
        } else if (rowListIterator.key() == startingRow + numberRows) {
            ++numberRows;
        } else {
            rowGroups.insert(startingRow, numberRows);
            startingRow = rowListIterator.key();
            numberRows  = 1;
        }
    }

    if (numberRows > 0) {
        rowGroups.insert(startingRow, numberRows);
    }

    QMap<unsigned long, unsigned long>::const_iterator rowGroupIterator = rowGroups.constEnd();
    unsigned long                                      count            = static_cast<unsigned long>(rowGroups.size());
    for (unsigned long i=0 ; i<count ; ++i) {
        --rowGroupIterator;
        model->removeRows(rowGroupIterator.key(), rowGroupIterator.value());
    }

    moveUpButton->setEnabled(false);
    moveDownButton->setEnabled(false);
    deleteButton->setEnabled(false);
    insertButton->setEnabled(false);
}


void TupleInspectorWidget::insertBoolean() {
    insert(Model::Variant(Model::Boolean(false)));
}


void TupleInspectorWidget::insertInteger() {
    insert(Model::Variant(Model::Integer(0)));
}


void TupleInspectorWidget::insertReal() {
    insert(Model::Variant(Model::Real(0)));
}


void TupleInspectorWidget::insertComplex() {
    insert(Model::Variant(Model::Complex(0, 0)));
}


void TupleInspectorWidget::insertMatrixBoolean() {
    insert(Model::Variant(Model::MatrixBoolean(2, 2)));
}


void TupleInspectorWidget::insertMatrixInteger() {
    insert(Model::Variant(Model::MatrixInteger(2, 2)));
}


void TupleInspectorWidget::insertMatrixReal() {
    insert(Model::Variant(Model::MatrixReal(2, 2)));
}


void TupleInspectorWidget::insertMatrixComplex() {
    insert(Model::Variant(Model::MatrixComplex(2, 2)));
}


void TupleInspectorWidget::insertSet() {
    insert(Model::Variant(Model::Set()));
}


void TupleInspectorWidget::insertTuple() {
    insert(Model::Variant(Model::Tuple()));
}


void TupleInspectorWidget::insert(const Model::Variant& initialValue) {
    Ld::CalculatedValue cv(Ld::VariableName(), initialValue);

    InspectorDialog dialog(cv, this);
    dialog.setOkAlwaysEnabled();

    int result = dialog.exec();

    if (result == InspectorDialog::Accepted) {
        QModelIndex           currentIndex = currentTableView->selectionModel()->currentIndex();
        unsigned long         currentRow   = static_cast<unsigned long>(currentIndex.row());
        TupleInspectorModel*  model    = dynamic_cast<TupleInspectorModel*>(currentTableView->model());
        Ld::CalculatedValue   nv       = dialog.calculatedValue();
        const Model::Variant& newValue = nv.variant();

        model->insertValue(currentRow, newValue);
    }
}
