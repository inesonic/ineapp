/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BlockFormatBuilderEarly class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>

#include "block_format_builder_early.h"

const char BlockFormatBuilderEarly::builderName[] = "BlockFormatBuilderEarly";

BlockFormatBuilderEarly::BlockFormatBuilderEarly(QObject* parent):EQt::Builder(parent) {}


BlockFormatBuilderEarly::~BlockFormatBuilderEarly() {}


unsigned BlockFormatBuilderEarly::priority() const {
    return builderPriority;
}


const char* BlockFormatBuilderEarly::builderIdentifier() const {
    return builderName;
}


const char* BlockFormatBuilderEarly::plugInName() const {
    return "";
}


void BlockFormatBuilderEarly::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenuAction(
        EQt::Application::icon("format_block"),
        tr("F&ormat | &Block..."),
        "format_block_dialog"
    );
    window->setToolTip("format_block_dialog", tr("Change the block/paragraph format"));
    window->setWhatsThis(
        "format_block_dialog",
        tr(
            "Use this option to update or change the block/paragraph spacing, left and right margins, indentation, "
            "and line spacing."
          )
    );
}
