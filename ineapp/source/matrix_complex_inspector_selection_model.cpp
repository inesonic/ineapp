/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref MatrixComplexInspectorSelectionModel class.
***********************************************************************************************************************/

#include <QObject>
#include <QModelIndex>
#include <QItemSelection>
#include <QItemSelectionRange>
#include <QItemSelectionModel>

#include "matrix_inspector_model.h"
#include "matrix_complex_inspector_selection_model.h"

MatrixComplexInspectorSelectionModel::MatrixComplexInspectorSelectionModel(
        MatrixInspectorModel* model,
        QObject*              parent
    ):QItemSelectionModel(
        model,
        parent
    ),currentModel(
        model
    ) {}


MatrixComplexInspectorSelectionModel::MatrixComplexInspectorSelectionModel(
        MatrixInspectorModel* model
    ):QItemSelectionModel(
        model
    ),currentModel(
        model
    ) {}


MatrixComplexInspectorSelectionModel::~MatrixComplexInspectorSelectionModel() {}


void MatrixComplexInspectorSelectionModel::clear() {
    QItemSelectionModel::clear();
}


void MatrixComplexInspectorSelectionModel::clearCurrentIndex() {
    QItemSelectionModel::clearCurrentIndex();
}


void MatrixComplexInspectorSelectionModel::reset() {
    QItemSelectionModel::reset();
}


void MatrixComplexInspectorSelectionModel::select(const QItemSelection& selection, SelectionFlags command) {
    QModelIndexList indexes         = selection.indexes();
    bool            containsUpdates = false;
    QItemSelection  newSelections;

    for (  QModelIndexList::const_iterator indexIterator    = indexes.constBegin(),
                                           indexEndIterator = indexes.constEnd()
         ; indexIterator != indexEndIterator
         ; ++indexIterator
        ) {
        const QModelIndex& index   = *indexIterator;
        QModelIndex        sibling = currentModel->siblingIndex(index);
        if (!selection.contains(sibling)) {
            newSelections.select(sibling, sibling);
            containsUpdates = true;
        }
    }

    if (containsUpdates) {
        QItemSelection modifiedSelection = selection;
        modifiedSelection.merge(newSelections, QItemSelectionModel::Select);

        QItemSelectionModel::select(modifiedSelection, command);
    } else {
        QItemSelectionModel::select(selection, command);
    }
}


void MatrixComplexInspectorSelectionModel::select(const QModelIndex& index, SelectionFlags command) {
    QItemSelectionModel::select(index, command);
}


void MatrixComplexInspectorSelectionModel::setCurrentIndex(const QModelIndex& index, SelectionFlags command) {
    QItemSelectionModel::setCurrentIndex(index, command);
}
