/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref SetInspectorWidget class.
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
#include "set_inspector_model.h"
#include "inspector_widget.h"
#include "inspector_dialog.h"
#include "set_inspector_widget.h"

InspectorWidget* SetInspectorWidget::creator(QWidget* parent) {
    return new SetInspectorWidget(parent);
}


SetInspectorWidget::SetInspectorWidget(QWidget* parent):InspectorWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);

    currentTableView = new QTableView;
    mainLayout->addWidget(currentTableView);

    SetInspectorModel*  model    = new SetInspectorModel(currentTableView);
    QAbstractItemModel* oldModel = currentTableView->model();

    currentTableView->setModel(model);
    if (oldModel != Q_NULLPTR) {
        oldModel->deleteLater();
    }

    currentTableView->horizontalHeader()->setVisible(false);
    currentTableView->verticalHeader()->setVisible(false);
    currentTableView->setShowGrid(false);
    currentTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    currentTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    currentTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    currentTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    currentTableView->setSelectionBehavior(QTableView::SelectRows);

    connect(model, &SetInspectorModel::valueChanged, this, &SetInspectorWidget::valueChanged);
    connect(model, &SetInspectorModel::valueRestored, this, &SetInspectorWidget::valueRestored);

    connect(currentTableView, &QTableView::doubleClicked, this, &SetInspectorWidget::itemDoubleClicked);
    connect(
        currentTableView,
        &QTableView::customContextMenuRequested,
        this,
        &SetInspectorWidget::customContextMenuRequested
    );

    connect(
        currentTableView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &SetInspectorWidget::selectionChanged
    );


    QHBoxLayout* buttonLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonLayout);

    insertButton = new QPushButton(Application::icon("add_entry"), QString());
    deleteButton = new QPushButton(Application::icon("remove_entry"), QString());

    buttonLayout->addWidget(insertButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch(1);

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

    connect(deleteButton, &QPushButton::clicked, this, &SetInspectorWidget::deleteItems);

    connect(insertBoolean,       &QAction::triggered, this, &SetInspectorWidget::insertBoolean);
    connect(insertInteger,       &QAction::triggered, this, &SetInspectorWidget::insertInteger);
    connect(insertReal,          &QAction::triggered, this, &SetInspectorWidget::insertReal);
    connect(insertComplex,       &QAction::triggered, this, &SetInspectorWidget::insertComplex);
    connect(insertSet,           &QAction::triggered, this, &SetInspectorWidget::insertSet);
    connect(insertTuple,         &QAction::triggered, this, &SetInspectorWidget::insertTuple);
    connect(insertMatrixBoolean, &QAction::triggered, this, &SetInspectorWidget::insertMatrixBoolean);
    connect(insertMatrixInteger, &QAction::triggered, this, &SetInspectorWidget::insertMatrixInteger);
    connect(insertMatrixReal,    &QAction::triggered, this, &SetInspectorWidget::insertMatrixReal);
    connect(insertMatrixComplex, &QAction::triggered, this, &SetInspectorWidget::insertMatrixComplex);

    deleteButton->setEnabled(false);
}


SetInspectorWidget::~SetInspectorWidget() {}


Ld::CalculatedValue SetInspectorWidget::calculatedValue() const {
    const SetInspectorModel* model = dynamic_cast<const SetInspectorModel*>(currentTableView->model());
    return model->currentCalculatedValue();
}


bool SetInspectorWidget::calculatedValueChanged() const {
    const SetInspectorModel* model = dynamic_cast<const SetInspectorModel*>(currentTableView->model());
    return model->calculatedValueChanged();
}


void SetInspectorWidget::setCalculatedValue(const Ld::CalculatedValue& calculatedValue) {
    SetInspectorModel* model = dynamic_cast<SetInspectorModel*>(currentTableView->model());
    model->setCalculatedValue(calculatedValue);
}


void SetInspectorWidget::resetCalculatedValue() {
    SetInspectorModel* model = dynamic_cast<SetInspectorModel*>(currentTableView->model());
    model->resetModel();
}


void SetInspectorWidget::selectionChanged(const QItemSelection& selection, const QItemSelection& /* deselected */) {
    deleteButton->setEnabled(!selection.isEmpty());
}


void SetInspectorWidget::itemDoubleClicked(const QModelIndex& index) {
    unsigned long            rowIndex = static_cast<unsigned long>(index.row());
    const SetInspectorModel* model    = dynamic_cast<const SetInspectorModel*>(currentTableView->model());
    const Model::Variant&    value    = model->valueAt(rowIndex);

    modifyInsert(value, true);
}


void SetInspectorWidget::customContextMenuRequested(const QPoint& position) {
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

    connect(inspectModifyAction, &QAction::triggered, this, &SetInspectorWidget::inspectModifyItem);
    connect(deleteAction,        &QAction::triggered, this, &SetInspectorWidget::deleteItems);
    connect(insertBoolean,       &QAction::triggered, this, &SetInspectorWidget::insertBoolean);
    connect(insertInteger,       &QAction::triggered, this, &SetInspectorWidget::insertInteger);
    connect(insertReal,          &QAction::triggered, this, &SetInspectorWidget::insertReal);
    connect(insertComplex,       &QAction::triggered, this, &SetInspectorWidget::insertComplex);
    connect(insertSet,           &QAction::triggered, this, &SetInspectorWidget::insertSet);
    connect(insertTuple,         &QAction::triggered, this, &SetInspectorWidget::insertTuple);
    connect(insertMatrixBoolean, &QAction::triggered, this, &SetInspectorWidget::insertMatrixBoolean);
    connect(insertMatrixInteger, &QAction::triggered, this, &SetInspectorWidget::insertMatrixInteger);
    connect(insertMatrixReal,    &QAction::triggered, this, &SetInspectorWidget::insertMatrixReal);
    connect(insertMatrixComplex, &QAction::triggered, this, &SetInspectorWidget::insertMatrixComplex);

    QPoint globalPosition = currentTableView->mapToGlobal(position);
    menu.exec(globalPosition);
}


void SetInspectorWidget::inspectModifyItem() {
    QModelIndex modelIndex = currentTableView->selectionModel()->currentIndex();
    itemDoubleClicked(modelIndex);
}


void SetInspectorWidget::deleteItems() {
    QModelIndexList    indexes = currentTableView->selectionModel()->selectedIndexes();
    SetInspectorModel* model   = dynamic_cast<SetInspectorModel*>(currentTableView->model());


    QMap<unsigned long, char> selectedRows;
    for (  QModelIndexList::const_iterator indexIterator = indexes.constBegin(), indexEndIterator = indexes.constEnd()
         ; indexIterator != indexEndIterator
         ; ++indexIterator
        ) {
        unsigned long rowIndex = indexIterator->row();
        selectedRows.insert(rowIndex, 0);
    }

    QList<Model::Variant> valuesToRemove;
    for (  QMap<unsigned long, char>::const_iterator rowIterator    = selectedRows.constBegin(),
                                                     rowEndIterator = selectedRows.constEnd()
         ; rowIterator != rowEndIterator
         ; ++rowIterator
        ) {
        unsigned long rowIndex = rowIterator.key();
        valuesToRemove.append(model->valueAt(rowIndex));
    }

    model->removeValues(valuesToRemove);
}


void SetInspectorWidget::insertBoolean() {
    modifyInsert(Model::Variant(Model::Boolean(false)), false);
}


void SetInspectorWidget::insertInteger() {
    modifyInsert(Model::Variant(Model::Integer(0)), false);
}


void SetInspectorWidget::insertReal() {
    modifyInsert(Model::Variant(Model::Real(0)), false);
}


void SetInspectorWidget::insertComplex() {
    modifyInsert(Model::Variant(Model::Complex(0, 0)), false);
}


void SetInspectorWidget::insertMatrixBoolean() {
    modifyInsert(Model::Variant(Model::MatrixBoolean(2, 2)), false);
}


void SetInspectorWidget::insertMatrixInteger() {
    modifyInsert(Model::Variant(Model::MatrixInteger(2, 2)), false);
}


void SetInspectorWidget::insertMatrixReal() {
    modifyInsert(Model::Variant(Model::MatrixReal(2, 2)), false);
}


void SetInspectorWidget::insertMatrixComplex() {
    modifyInsert(Model::Variant(Model::MatrixComplex(2, 2)), false);
}


void SetInspectorWidget::insertSet() {
    modifyInsert(Model::Variant(Model::Set()), false);
}


void SetInspectorWidget::insertTuple() {
    modifyInsert(Model::Variant(Model::Tuple()), false);
}


void SetInspectorWidget::modifyInsert(const Model::Variant& currentValue, bool removeCurrent) {
    Ld::CalculatedValue cv(Ld::VariableName(), currentValue);

    InspectorDialog dialog(cv, this);
    dialog.setOkConditionallyEnabled(removeCurrent);

    int result = dialog.exec();
    if (result == InspectorDialog::Accepted) {
        SetInspectorModel*    model    = dynamic_cast<SetInspectorModel*>(currentTableView->model());
        Ld::CalculatedValue   nv       = dialog.calculatedValue();
        const Model::Variant& newValue = nv.variant();

        if (removeCurrent) {
            model->removeValue(currentValue);
        }

        bool success = model->addValue(newValue);

        if (!success) {
            QMessageBox::information(this, tr("Duplicate Entry"), tr("The set already contains the provided value."));
        }
    }
}
