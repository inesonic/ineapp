/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixRealInspectorWidget class.
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
#include "matrix_real_inspector_model.h"
#include "matrix_inspector_widget.h"
#include "matrix_real_inspector_widget.h"

InspectorWidget* MatrixRealInspectorWidget::creator(QWidget* parent) {
    return new MatrixRealInspectorWidget(parent);
}


MatrixRealInspectorWidget::MatrixRealInspectorWidget(QWidget* parent):MatrixInspectorWidget(parent) {}


MatrixRealInspectorWidget::~MatrixRealInspectorWidget() {}


MatrixInspectorModel* MatrixRealInspectorWidget::createModel(const Ld::CalculatedValue& calculatedValue) {
    return new MatrixRealInspectorModel(calculatedValue, this);
}


QAbstractItemDelegate* MatrixRealInspectorWidget::createDelegate() {
    return new RealDelegate(this);
}


unsigned MatrixRealInspectorWidget::initialColumnWidth() const {
    QFontMetrics fontMetrics(tableFont());
    return fontMetrics.horizontalAdvance(tr("XXXXXXXXXXXXXXXX  "));
}
