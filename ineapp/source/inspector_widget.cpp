/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref InspectorWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QHash>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>

#include <ld_data_type.h>
#include <ld_calculated_value.h>

#include "inspector_widget.h"

QHash<Ld::DataType::ValueType, InspectorWidget::CreatorFunction> InspectorWidget::creatorFunctionsByValueType;

bool InspectorWidget::registerCreator(Ld::DataType::ValueType valueType, CreatorFunction creatorFunction) {
    bool result;

    if (!creatorFunctionsByValueType.contains(valueType)) {
        creatorFunctionsByValueType.insert(valueType, creatorFunction);
        result = true;
    } else {
        result = false;
    }

    return result;
}


InspectorWidget* InspectorWidget::createWidget(Ld::DataType::ValueType valueType, QWidget* parent) {
    InspectorWidget* widget;

    CreatorFunction creatorFunction = creatorFunctionsByValueType.value(valueType);
    if (creatorFunction != Q_NULLPTR) {
        widget = (*creatorFunction)(parent);
    } else {
        widget = Q_NULLPTR;
    }

    return widget;
}


InspectorWidget* InspectorWidget::createWidget(const Ld::CalculatedValue& calculatedValue, QWidget* parent) {
    InspectorWidget* widget = createWidget(calculatedValue.valueType(), parent);
    if (widget != Q_NULLPTR) {
        widget->setCalculatedValue(calculatedValue);
    }

    return widget;
}


InspectorWidget::InspectorWidget(QWidget* parent):QWidget(parent) {}


InspectorWidget::~InspectorWidget() {}


bool InspectorWidget::includeLoadSaveSupport() const {
    return false;
}


void InspectorWidget::loadValue() {
    #if (defined(Q_OS_WINDOWS))

        QString filename = QFileDialog::getOpenFileName(
            this,
            tr("Select the file holding the variable to be loaded"),
            QString(),
            tr("CSV Files (*.csv *.txt);;Binary files (*.dat *.bin);;All files (*)")
        );

    #elif (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

        QString filename = QFileDialog::getOpenFileName(
            this,
            tr("Select the file holding the variable to be loaded"),
            QString(),
            tr("CSV Files (*.csv *.txt);;Binary files (*.dat *.bin);;All files (*)")
        );

    #else

        #error Unknown platform

    #endif

    if (!filename.isEmpty()) {
        QString suffix = QFileInfo(filename).suffix();
        if (suffix.isEmpty()) {
            suffix = tr(".csv");
            filename += suffix;
        }

        bool binaryFormat = (suffix.toLower() == tr("dat") || suffix.toLower() == tr("bin"));
        if (!loadFromFile(filename, binaryFormat)) {
            QMessageBox::warning(this, tr("Could Not Load"), tr("Could not load variable from: %1").arg(filename));
        }
    }
}


void InspectorWidget::saveValue() {
    #if (defined(Q_OS_WINDOWS))

        QString filename = QFileDialog::getSaveFileName(
            this,
            tr("Select the file holding the variable to be loaded"),
            QString(),
            tr("CSV Files (*.csv *.txt);;Binary files (*.dat *.bin);;All files (*)")
        );

    #elif (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

        QString filename = QFileDialog::getSaveFileName(
            this,
            tr("Select the file holding the variable to be loaded"),
            QString(),
            tr("CSV Files (*.csv *.txt);;Binary files (*.dat *.bin);;All files (*)")
        );

    #else

        #error Unknown platform

    #endif

    if (!filename.isEmpty()) {
        QString suffix = QFileInfo(filename).suffix();
        if (suffix.isEmpty()) {
            suffix = tr(".csv");
            filename += suffix;
        }

        bool binaryFormat = (suffix.toLower() == tr("dat") || suffix.toLower() == tr("bin"));
        if (!saveToFile(filename, binaryFormat)) {
            QMessageBox::warning(this, tr("Could Not Load"), tr("Could not load variable from: %1").arg(filename));
        }
    }
}


bool InspectorWidget::loadFromFile(const QString& /* filename */, bool /* binaryFormat */) {
    return false;
}


bool InspectorWidget::saveToFile(const QString& /* filename */, bool /* binaryFormat */) {
    return false;
}
