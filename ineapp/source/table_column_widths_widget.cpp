/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableColumnWidthsWidget class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QWidget>
#include <QHeaderView>
#include <QTableView>

#include <util_units.h>

#include "table_column_widths_model.h"
#include "table_column_widths_delegate.h"
#include "table_column_widths_widget.h"

TableColumnWidthsWidget::TableColumnWidthsWidget(QWidget* parent):QTableView(parent) {
    TableColumnWidthsModel* model = new TableColumnWidthsModel(this);
    setModel(model);
    connect(model, &TableColumnWidthsModel::columnWidthChanged, this, &TableColumnWidthsWidget::columnWidthChanged);

    TableColumnWidthsDelegate* delegate = new TableColumnWidthsDelegate(this);
    setItemDelegate(delegate);

    QHeaderView* topHeader = horizontalHeader();
    topHeader->setSectionResizeMode(QHeaderView::Stretch);
}


TableColumnWidthsWidget::~TableColumnWidthsWidget() {}


TableColumnWidthsWidget::Unit TableColumnWidthsWidget::displayedUnits() const {
    return dynamic_cast<TableColumnWidthsModel*>(model())->units();
}


unsigned TableColumnWidthsWidget::numberTableColumns() const {
    return dynamic_cast<TableColumnWidthsModel*>(model())->numberTableColumns();
}


bool TableColumnWidthsWidget::isProportionalColumn(unsigned tableColumnIndex) const {
    return dynamic_cast<TableColumnWidthsModel*>(model())->isProportionalColumn(tableColumnIndex);
}


bool TableColumnWidthsWidget::isFixedColumn(unsigned tableColumnIndex) const {
    return dynamic_cast<TableColumnWidthsModel*>(model())->isFixedColumn(tableColumnIndex);
}


float TableColumnWidthsWidget::columnWidth(unsigned tableColumnIndex) const {
    return dynamic_cast<TableColumnWidthsModel*>(model())->columnWidth(tableColumnIndex);
}


void TableColumnWidthsWidget::setDisplayedUnits(Unit newUnits) {
    dynamic_cast<TableColumnWidthsModel*>(model())->setUnits(newUnits);
}


void TableColumnWidthsWidget::setNumberTableColumns(unsigned newNumberColumns) {
    dynamic_cast<TableColumnWidthsModel*>(model())->setNumberTableColumns(newNumberColumns);
}


void TableColumnWidthsWidget::setProportionalColumn(unsigned tableColumnIndex, bool nowProportional) {
    dynamic_cast<TableColumnWidthsModel*>(model())->setProportionalColumn(tableColumnIndex, nowProportional);
}


void TableColumnWidthsWidget::setFixedColumn(unsigned tableColumnIndex, bool nowFixed) {
    dynamic_cast<TableColumnWidthsModel*>(model())->setFixedColumn(tableColumnIndex, nowFixed);
}


void TableColumnWidthsWidget::setColumnWidth(unsigned tableColumnIndex, float newWidth) {
    dynamic_cast<TableColumnWidthsModel*>(model())->setColumnWidth(tableColumnIndex, newWidth);
}


void TableColumnWidthsWidget::setColumnWidth(unsigned tableColumnIndex, bool nowProportional, float newWidth) {
    dynamic_cast<TableColumnWidthsModel*>(model())->setColumnWidth(tableColumnIndex, nowProportional, newWidth);
}


void TableColumnWidthsWidget::setColumnWidthInvalid(unsigned tableColumnIndex) {
    dynamic_cast<TableColumnWidthsModel*>(model())->setColumnWidth(tableColumnIndex, false, -1.0F);
}
