/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CharacterFormatBuilderEarly class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>

#include "character_format_builder_early.h"

const char CharacterFormatBuilderEarly::builderName[] = "CharacterFormatBuilderEarly";

CharacterFormatBuilderEarly::CharacterFormatBuilderEarly(QObject* parent):EQt::Builder(parent) {}


CharacterFormatBuilderEarly::~CharacterFormatBuilderEarly() {}


unsigned CharacterFormatBuilderEarly::priority() const {
    return builderPriority;
}


const char* CharacterFormatBuilderEarly::builderIdentifier() const {
    return builderName;
}


const char* CharacterFormatBuilderEarly::plugInName() const {
    return "";
}


void CharacterFormatBuilderEarly::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenuAction(
        EQt::Application::icon("format_font"),
        tr("F&ormat | &Character..."),
        "format_character_dialog"
    );
    window->setToolTip("format_character_dialog", tr("Change the character format"));
    window->setWhatsThis(
        "format_character_dialog",
        tr(
            "Use this option to update or change the text font, font size, italics, font weight, and character "
            "spacing.  This option also allows you to superscript/subscript text.  You can also use this option to "
            "change font foreground and background colors."
          )
    );
}
