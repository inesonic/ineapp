/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixIntegerInspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QComboBox>
#include <QFormLayout>
#include <QFont>
#include <QFontMetrics>

#include <model_api_types.h>
#include <model_variant.h>

#include <ld_calculated_value.h>

#include "integer_delegate.h"
#include "matrix_inspector_model.h"
#include "matrix_integer_inspector_model.h"
#include "matrix_inspector_widget.h"
#include "matrix_integer_inspector_widget.h"

InspectorWidget* MatrixIntegerInspectorWidget::creator(QWidget* parent) {
    return new MatrixIntegerInspectorWidget(parent);
}


MatrixIntegerInspectorWidget::MatrixIntegerInspectorWidget(QWidget* parent):MatrixInspectorWidget(parent) {}


MatrixIntegerInspectorWidget::~MatrixIntegerInspectorWidget() {}


MatrixInspectorModel* MatrixIntegerInspectorWidget::createModel(const Ld::CalculatedValue& calculatedValue) {
    return new MatrixIntegerInspectorModel(calculatedValue, this);
}


QAbstractItemDelegate* MatrixIntegerInspectorWidget::createDelegate() {
    return new IntegerDelegate(this);
}


unsigned MatrixIntegerInspectorWidget::initialColumnWidth() const {
    QFontMetrics fontMetrics(tableFont());
    return fontMetrics.horizontalAdvance(tr("XXXXXXXXXXXXXXXXXXXX  "));
}
