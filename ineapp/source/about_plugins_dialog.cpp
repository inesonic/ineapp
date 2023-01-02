/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref AboutPlugInsDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QStringList>
#include <QMap>

#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_plug_in_information.h>

#include "application.h"
#include "plug_in_details_dialog.h"
#include "plug_in_table_model.h"
#include "about_plugins_dialog.h"

AboutPlugInsDialog::AboutPlugInsDialog(QWidget* parent):ProgrammaticDialog("AboutPlugInsDialog", parent) {
    setWindowTitle(tr("About %1 Plug-Ins").arg(Application::applicationName()));
    setSizeGripEnabled(true);

    configureWidget();
    runBuilders();
}


AboutPlugInsDialog::~AboutPlugInsDialog() {}


void AboutPlugInsDialog::populate() {}


void AboutPlugInsDialog::bind() {
    connect(widget<QPushButton>("ok_push_button"), SIGNAL(clicked()), this, SLOT(close()));
    connect(
        widget<QTableView>("table_view"),
        SIGNAL(clicked(const QModelIndex&)),
        this,
        SLOT(itemClicked(const QModelIndex&))
    );

    ProgrammaticDialog::bind();
}


void AboutPlugInsDialog::itemClicked(const QModelIndex& index) {
    QTableView*           tableView         = widget<QTableView>("table_view");
    PlugInTableModel*     model             = static_cast<PlugInTableModel*>(tableView->model());
    Ld::PlugInInformation plugInInformation = model->plugInInformation(index);

    PlugInDetailsDialog plugInDetailsDialog(plugInInformation, this);
    plugInDetailsDialog.exec();
}


void AboutPlugInsDialog::configureWidget() {
    QVBoxLayout* verticalLayout = newVBoxLayout("vertical_layout");
    setLayout(verticalLayout);

    QGroupBox* tableGroupBox = new QGroupBox(tr("Loaded Plug-Ins"));
    registerWidget(tableGroupBox, "table_group_box");
    verticalLayout->addWidget(tableGroupBox);

    QHBoxLayout* tableLayout = newHBoxLayout("table_layout");
    tableGroupBox->setLayout(tableLayout);

    QTableView* tableView = new QTableView;
    registerWidget(tableView, "table_view");
    tableLayout->addWidget(tableView);

    PlugInTableModel* tableModel = new PlugInTableModel(tableView);
    tableView->setModel(tableModel);
    tableView->horizontalHeader()->show();
    tableView->verticalHeader()->hide();
    tableView->resizeColumnsToContents();

    QHBoxLayout* buttonLayout = newHBoxLayout("button_layout");
    verticalLayout->addLayout(buttonLayout);

    QPushButton* okPushButton = new QPushButton(tr("OK"));
    registerWidget(okPushButton, "ok_push_button");
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okPushButton);
    buttonLayout->addStretch(1);
}
