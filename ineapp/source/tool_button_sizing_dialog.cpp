/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ToolButtonSizingDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QShowEvent>

#include "eqt_programmatic_dock_widget.h"

#include "tool_button_sizing_dialog.h"

ToolButtonSizingDialog::ToolButtonSizingDialog(
        QWidget* parent
    ):QDialog(
        parent,
        Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint
    ) {
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QLineEdit* lineEdit1 = new QLineEdit;
    mainLayout->addWidget(lineEdit1);

    QLineEdit* lineEdit2 = new QLineEdit;
    mainLayout->addWidget(lineEdit2);

    mainLayout->setSpacing(EQt::ProgrammaticDockWidget::minimumLayoutSpacing());
    mainLayout->setContentsMargins(EQt::ProgrammaticDockWidget::minimumLayoutMargins());

    closeTimer = new QTimer(this);
    closeTimer->setSingleShot(true);

    connect(closeTimer, &QTimer::timeout, this, &ToolButtonSizingDialog::closeDialog);
}


ToolButtonSizingDialog::~ToolButtonSizingDialog() {}


unsigned ToolButtonSizingDialog::determineRecommendedButtonSize() {
    exec();
    return recommendedButtonSize;
}


void ToolButtonSizingDialog::showEvent(QShowEvent* /* event */) {
    closeTimer->start(0);
}


void ToolButtonSizingDialog::closeDialog() {
    QVBoxLayout* mainLayout = dynamic_cast<QVBoxLayout*>(layout());
    recommendedButtonSize = static_cast<unsigned>(mainLayout->geometry().height());

    accept();
}

