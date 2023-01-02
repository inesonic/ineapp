/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BlockFormatBuilderMid class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_main_window_proxy.h>

#include "main_window.h"
#include "block_format_builder_mid.h"

const char BlockFormatBuilderMid::builderName[] = "BlockFormatBuilderMid";

BlockFormatBuilderMid::BlockFormatBuilderMid(QObject* parent):EQt::Builder(parent) {}


BlockFormatBuilderMid::~BlockFormatBuilderMid() {}


unsigned BlockFormatBuilderMid::priority() const {
    return builderPriority;
}


const char* BlockFormatBuilderMid::builderIdentifier() const {
    return builderName;
}


const char* BlockFormatBuilderMid::plugInName() const {
    return "";
}


void BlockFormatBuilderMid::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addSeparator(tr("F&ormat"));

    window->addMenuAction(
        EQt::Application::icon("format_list"),
        tr("F&ormat | &Bullets And Numbering..."),
        "format_list_dialog"
    );
    window->setToolTip("format_list_dialog", tr("Enable and modify list formatting of a paragraph"));
    window->setWhatsThis(
        "format_list_dialog",
        tr("Use this option to enable, update or change bullets or numbers associated with a list.")
    );
}
