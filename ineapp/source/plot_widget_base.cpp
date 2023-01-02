/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotWidgetBase class.
***********************************************************************************************************************/

#include <QWidget>
#include <QWizardPage>

#include "application.h"
#include "plot_widget_base.h"

PlotWidgetBase::PlotWidgetBase(QWidget* parent):QWizardPage(parent) {
    currentNextId  = -1;
    lastValidState = ValidState::UNKNOWN;
}


PlotWidgetBase::~PlotWidgetBase() {}


int PlotWidgetBase::nextId() const {
    return currentNextId;
}


void PlotWidgetBase::setNextId(int newNextId) {
    currentNextId = newNextId;
}


bool PlotWidgetBase::isInvalid() const {
    return !isValid();
}


bool PlotWidgetBase::isComplete() const {
    bool result;

    if (lastValidState == ValidState::UNKNOWN) {
        if (isValid()) {
            lastValidState = ValidState::VALID;
            result         = true;
        } else {
            lastValidState = ValidState::INVALID;
            result         = false;
        }
    } else {
        result = lastValidState == ValidState::VALID;
    }

    return result;
}


void PlotWidgetBase::updateChangedStatus() {
    emit changed();

    ValidState newValidState = isValid() ? ValidState::VALID : ValidState::INVALID;
    if (newValidState != lastValidState) {
        lastValidState = newValidState;
        emit completeChanged();
    }
}
