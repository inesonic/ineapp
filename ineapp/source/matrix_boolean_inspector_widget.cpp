/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixBooleanInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QFont>
#include <QFontMetrics>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "boolean_delegate.h"
#include "matrix_inspector_model.h"
#include "matrix_boolean_inspector_model.h"
#include "matrix_inspector_widget.h"
#include "matrix_boolean_inspector_widget.h"

InspectorWidget* MatrixBooleanInspectorWidget::creator(QWidget* parent) {
    return new MatrixBooleanInspectorWidget(parent);
}


MatrixBooleanInspectorWidget::MatrixBooleanInspectorWidget(QWidget* parent):MatrixInspectorWidget(parent) {}


MatrixBooleanInspectorWidget::~MatrixBooleanInspectorWidget() {}


MatrixInspectorModel* MatrixBooleanInspectorWidget::createModel(const Ld::CalculatedValue& calculatedValue) {
    return new MatrixBooleanInspectorModel(calculatedValue, this);
}


QAbstractItemDelegate* MatrixBooleanInspectorWidget::createDelegate() {
    return new BooleanDelegate(this);
}


unsigned MatrixBooleanInspectorWidget::initialColumnWidth() const {
    QFontMetrics fontMetrics(tableFont());
    return fontMetrics.horizontalAdvance(tr("false")) * 2.25;
}
