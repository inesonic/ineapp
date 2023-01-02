/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixComplexInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QFont>
#include <QFontMetrics>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "real_delegate.h"
#include "matrix_inspector_model.h"
#include "matrix_complex_inspector_model.h"
#include "matrix_complex_inspector_selection_model.h"
#include "matrix_inspector_widget.h"
#include "matrix_complex_inspector_widget.h"

InspectorWidget* MatrixComplexInspectorWidget::creator(QWidget* parent) {
    return new MatrixComplexInspectorWidget(parent);
}


MatrixComplexInspectorWidget::MatrixComplexInspectorWidget(QWidget* parent):MatrixInspectorWidget(parent) {}


MatrixComplexInspectorWidget::~MatrixComplexInspectorWidget() {}


MatrixInspectorModel* MatrixComplexInspectorWidget::createModel(const Ld::CalculatedValue& calculatedValue) {
    return new MatrixComplexInspectorModel(calculatedValue, this);
}


QAbstractItemDelegate* MatrixComplexInspectorWidget::createDelegate() {
    return new RealDelegate(this);
}


QItemSelectionModel* MatrixComplexInspectorWidget::createSelectionModel(
        QTableView*           tableView,
        MatrixInspectorModel* model
    ) {
    return new MatrixComplexInspectorSelectionModel(model, tableView);
}


unsigned MatrixComplexInspectorWidget::initialColumnWidth() const {
    QFontMetrics fontMetrics(tableFont());
    return fontMetrics.horizontalAdvance(tr("XXXXXXXXXXXXXXXX  "));
}
