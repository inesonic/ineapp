/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CharacterFormatBuilderLate class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QKeySequence>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_font_selector_widget.h>
#include <eqt_paper_size_popup_menu.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_combo_box.h>
#include <eqt_tool_button.h>

#include "main_window.h"
#include "character_format_main_window_proxy.h"
#include "character_format_view_proxy.h"
#include "character_format_builder_late.h"

const char CharacterFormatBuilderLate::builderName[] = "CharacterFormatBuilderLate";
const char CharacterFormatBuilderLate::characterFormatDockName[] = "character_format";

CharacterFormatBuilderLate::CharacterFormatBuilderLate(QObject* parent):EQt::Builder(parent) {}


CharacterFormatBuilderLate::~CharacterFormatBuilderLate() {}


unsigned CharacterFormatBuilderLate::priority() const {
    return builderPriority;
}


const char* CharacterFormatBuilderLate::builderIdentifier() const {
    return builderName;
}


const char* CharacterFormatBuilderLate::plugInName() const {
    return "";
}


void CharacterFormatBuilderLate::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* boldAction = window->addMenuAction(
        EQt::Application::icon("format_font_bold"),
        tr("F&ormat | &Text | &Bold"),
        "format_character_bold_action"
    );
    boldAction->setCheckable(true);
    boldAction->setToolTip(tr("Boldface text"));
    boldAction->setWhatsThis(tr("You can use this option to toggle between bold and normal text weight."));
    boldAction->setShortcut(QKeySequence::Bold);

    QAction* italicsAction = window->addMenuAction(
        EQt::Application::icon("format_font_italic"),
        tr("F&ormat | &Text | &Italics"),
        "format_character_italics_action"
    );
    italicsAction->setCheckable(true);
    italicsAction->setToolTip(tr("Italicize text"));
    italicsAction->setWhatsThis(tr("You can use this option to add or remove italics from text."));
    italicsAction->setShortcut(QKeySequence::Italic);

    QAction* underlineAction = window->addMenuAction(
        EQt::Application::icon("format_font_underline"),
        tr("F&ormat | &Text | &Underline"),
        "format_character_underline_action"
    );
    underlineAction->setCheckable(true);
    underlineAction->setToolTip(tr("Underline text"));
    underlineAction->setWhatsThis(tr("You can use this option to underline text."));
    underlineAction->setShortcut(QKeySequence::Underline);

    QAction* overlineAction = window->addMenuAction(
        EQt::Application::icon("format_font_overline"),
        tr("F&ormat | &Text | &Overline"),
        "format_character_overline_action"
    );
    overlineAction->setCheckable(true);
    overlineAction->setToolTip(tr("Overline text"));
    overlineAction->setWhatsThis(tr("You can use this option to overline text."));

    QAction* strikethroughAction = window->addMenuAction(
        EQt::Application::icon("format_font_strikethrough"),
        tr("F&ormat | &Text | S&trikethrough"),
        "format_character_strikethrough_action"
    );
    strikethroughAction->setCheckable(true);
    strikethroughAction->setToolTip(tr("Strikethrough text"));
    strikethroughAction->setWhatsThis(tr("You can use this option to strikethrough text."));

    QAction* subscriptAction = window->addMenuAction(
        EQt::Application::icon("format_font_subscript"),
        tr("F&ormat | &Text | B&aseline | &Subscript"),
        "format_character_subscript_action"
    );
    subscriptAction->setCheckable(true);
    subscriptAction->setToolTip(tr("subscript text"));
    subscriptAction->setWhatsThis(tr("You can use this option to subscript text."));

    QAction* superscriptAction = window->addMenuAction(
        EQt::Application::icon("format_font_superscript"),
        tr("F&ormat | &Text | B&aseline | S&uperscript"),
        "format_character_superscript_action"
    );
    superscriptAction->setCheckable(true);
    superscriptAction->setToolTip(tr("superscript text"));
    superscriptAction->setWhatsThis(tr("You can use this option to superscript text."));

    QAction* normalBaselineAction = window->addMenuAction(
        tr("F&ormat | &Text | B&aseline | &Normal"),
        "format_character_normal_baseline_action"
    );
    normalBaselineAction->setCheckable(true);
    normalBaselineAction->setToolTip(tr("Restore normal baseline"));
    normalBaselineAction->setWhatsThis(
        tr("You can use this option to restore superscript or subscript text back to its normal baseline.")
    );

    QAction* fontColorAction = window->addMenuAction(
        EQt::Application::icon("format_font_color"),
        tr("F&ormat | &Text | &Color..."),
        "format_character_color_action"
    );
    fontColorAction->setCheckable(true);
    fontColorAction->setToolTip(tr("Font color"));
    fontColorAction->setWhatsThis(tr("You can use this option to set the font color."));


    QAction* fontBackgroundColorAction = window->addMenuAction(
        EQt::Application::icon("format_font_background_color"),
        tr("F&ormat | &Text | &Highlighting..."),
        "format_character_background_color_action"
    );
    fontBackgroundColorAction->setCheckable(true);
    fontBackgroundColorAction->setToolTip(tr("Highlighting/Font background color"));
    fontBackgroundColorAction->setWhatsThis(
        tr("You can use this option to highlight text by adding a background color.")
    );
}


void CharacterFormatBuilderLate::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    window->addAction(new QAction(window), "format_font_family_action");
    window->addAction(new QAction(window), "format_font_size_action");
}


void CharacterFormatBuilderLate::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        characterFormatDockName,
        tr("Font"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION, "function", true)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        characterFormatDockName,
        tr("&View | &Docks"),
        "character_format_dock_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Character dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Character dock.  The Character dock includes functions "
            "used to set fonts and font styles."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");
    dock->setHorizontalLayout(horizontalLayout);

    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* CharacterFormatBuilderLate::createMainWindowProxy(
        EQt::ProgrammaticMainWindow* window
    ) {
    return new CharacterFormatMainWindowProxy(window);
}


void CharacterFormatBuilderLate::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QMenu*   rightClickMenu              = mainWindow->widget<QMenu>("right_click_menu");
    QAction* formatCharacterDialogAction = mainWindow->action("format_character_dialog");
    rightClickMenu->addAction(formatCharacterDialogAction);

    QAction* characterFormatDockToggleAction = mainWindow->action("character_format_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("cdock"), characterFormatDockToggleAction);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("cfmt"), formatCharacterDialogAction);
    Q_ASSERT(success);

    QAction* boldAction = mainWindow->action("format_character_bold_action");
    success = mainWindow->setCommand(tr("textbf"), boldAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("bf"), boldAction);
    Q_ASSERT(success);

    QAction* italicsAction = mainWindow->action("format_character_italics_action");
    success = mainWindow->setCommand(tr("textit"), italicsAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("it"), italicsAction);
    Q_ASSERT(success);

    QAction* underlineAction = mainWindow->action("format_character_underline_action");
    success = mainWindow->setCommand(tr("underline"), underlineAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("ul"), underlineAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("uline"), underlineAction);
    Q_ASSERT(success);

    QAction* overlineAction = mainWindow->action("format_character_overline_action");
    success = mainWindow->setCommand(tr("overline"), overlineAction);
    Q_ASSERT(success);

    QAction* strikethroughAction = mainWindow->action("format_character_strikethrough_action");
    success = mainWindow->setCommand(tr("sout"), strikethroughAction);
    Q_ASSERT(success);

    QAction* subscriptAction = mainWindow->action("format_character_subscript_action");
    success = mainWindow->setCommand(tr("textsubscript"), subscriptAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("sub"), subscriptAction);
    Q_ASSERT(success);

    QAction* superscriptAction = mainWindow->action("format_character_superscript_action");
    success = mainWindow->setCommand(tr("textsuperscript"), superscriptAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("super"), superscriptAction);
    Q_ASSERT(success);

    QAction* normalBaselineAction = mainWindow->action("format_character_normal_baseline_action");
    success = mainWindow->setCommand(tr("textnormal"), normalBaselineAction);
    Q_ASSERT(success);

    QAction* fontColorAction = mainWindow->action("format_character_color_action");
    success = mainWindow->setCommand(tr("color"), fontColorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("xcolor"), fontColorAction);
    Q_ASSERT(success);

    QAction* fontBackgroundColorAction = mainWindow->action("format_character_background_color_action");
    success = mainWindow->setCommand(tr("bgcolor"), fontBackgroundColorAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* CharacterFormatBuilderLate::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new CharacterFormatViewProxy(view, window);
}


void CharacterFormatBuilderLate::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("horizontal_controls");
    horizontalLayout->addLayout(controlsLayout);

    EQt::ToolButton* boldToolButton = new EQt::ToolButton(window->action("format_character_bold_action"));
    controlsLayout->addWidget(boldToolButton, 0, 0);
    dock->registerWidget(boldToolButton, "horizontal_bold_tool_button");

    EQt::ToolButton* italicsToolButton = new EQt::ToolButton(window->action("format_character_italics_action"));
    controlsLayout->addWidget(italicsToolButton, 0, 1);
    dock->registerWidget(italicsToolButton, "horizontal_italics_tool_button");

    EQt::ToolButton* underlineToolButton = new EQt::ToolButton(window->action("format_character_underline_action"));
    controlsLayout->addWidget(underlineToolButton, 0, 2);
    dock->registerWidget(underlineToolButton, "horizontal_underline_tool_button");

    EQt::ToolButton* overlineToolButton = new EQt::ToolButton(window->action("format_character_overline_action"));
    controlsLayout->addWidget(overlineToolButton, 0, 3);
    dock->registerWidget(overlineToolButton, "horizontal_overline_tool_button");

    EQt::ToolButton* strikethroughToolButton = new EQt::ToolButton(
        window->action("format_character_strikethrough_action")
    );
    controlsLayout->addWidget(strikethroughToolButton, 0, 4);
    dock->registerWidget(strikethroughToolButton, "horizontal_strikethrough_tool_button");

    EQt::ToolButton* subscriptToolButton = new EQt::ToolButton(window->action("format_character_subscript_action"));
    controlsLayout->addWidget(subscriptToolButton, 0, 5);
    dock->registerWidget(subscriptToolButton, "horizontal_subscript_tool_button");

    EQt::ToolButton* superscriptToolButton = new EQt::ToolButton(window->action("format_character_superscript_action"));
    controlsLayout->addWidget(superscriptToolButton, 0, 6);
    dock->registerWidget(superscriptToolButton, "horizontal_superscript_tool_button");

    EQt::FontSelectorWidget* fontSelectorWidget = new EQt::FontSelectorWidget(
        window->action("format_font_family_action"),
        window->action("format_font_size_action")
    );
    fontSelectorWidget->setFontSizeRange(Ld::FontFormat::minimumFontSize, Ld::FontFormat::maximumFontSize);
    fontSelectorWidget->setFontSizeSuffix(tr(" pt."));
    controlsLayout->addWidget(fontSelectorWidget, 1, 0, 1, 6);
    dock->registerWidget(fontSelectorWidget, "horizontal_font_selector_widget");

    EQt::ToolButton* formatFontDialogToolButton = new EQt::ToolButton(window->action("format_character_dialog"));
    controlsLayout->addWidget(formatFontDialogToolButton, 1, 6);
    dock->registerWidget(formatFontDialogToolButton, "horizontal_format_font_dialog_tool_button");

    EQt::ToolButton* fontColorToolButton = new EQt::ToolButton(window->action("format_character_color_action"));
    controlsLayout->addWidget(fontColorToolButton, 0, 7);
    dock->registerWidget(fontColorToolButton, "horizontal_font_color_tool_button");

    EQt::ToolButton* fontBackgroundColorToolButton = new EQt::ToolButton(
        window->action("format_character_background_color_action")
    );
    controlsLayout->addWidget(fontBackgroundColorToolButton, 1, 7);
    dock->registerWidget(fontBackgroundColorToolButton, "horizontal_font_background_color_tool_button");

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void CharacterFormatBuilderLate::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("vertical_controls");
    verticalLayout->addLayout(controlsLayout);

    EQt::ToolButton* boldToolButton = new EQt::ToolButton(window->action("format_character_bold_action"));
    controlsLayout->addWidget(boldToolButton, 0, 0);
    dock->registerWidget(boldToolButton, "vertical_bold_tool_button");

    EQt::ToolButton* italicsToolButton = new EQt::ToolButton(window->action("format_character_italics_action"));
    controlsLayout->addWidget(italicsToolButton, 0, 1);
    dock->registerWidget(italicsToolButton, "vertical_italics_tool_button");

    EQt::ToolButton* underlineToolButton = new EQt::ToolButton(window->action("format_character_underline_action"));
    controlsLayout->addWidget(underlineToolButton, 0, 2);
    dock->registerWidget(underlineToolButton, "vertical_underline_tool_button");

    EQt::ToolButton* overlineToolButton = new EQt::ToolButton(window->action("format_character_overline_action"));
    controlsLayout->addWidget(overlineToolButton,0, 3);
    dock->registerWidget(overlineToolButton, "vertical_overline_tool_button");

    EQt::ToolButton* strikethroughToolButton = new EQt::ToolButton(
        window->action("format_character_strikethrough_action")
    );
    controlsLayout->addWidget(strikethroughToolButton, 0, 4);
    dock->registerWidget(strikethroughToolButton, "vertical_strikethrough_tool_button");

    EQt::ToolButton* subscriptToolButton = new EQt::ToolButton(window->action("format_character_subscript_action"));
    controlsLayout->addWidget(subscriptToolButton, 1, 0);
    dock->registerWidget(subscriptToolButton, "vertical_subscript_tool_button");

    EQt::ToolButton* superscriptToolButton = new EQt::ToolButton(window->action("format_character_superscript_action"));
    controlsLayout->addWidget(superscriptToolButton, 1, 1);
    dock->registerWidget(superscriptToolButton, "vertical_superscript_tool_button");

    EQt::ToolButton* formatFontDialogToolButton = new EQt::ToolButton(window->action("format_character_dialog"));
    controlsLayout->addWidget(formatFontDialogToolButton, 1, 2);
    dock->registerWidget(formatFontDialogToolButton, "vertical_format_font_dialog_tool_button");

    EQt::ToolButton* fontColorToolButton = new EQt::ToolButton(window->action("format_character_color_action"));
    controlsLayout->addWidget(fontColorToolButton, 1, 3);
    dock->registerWidget(fontColorToolButton, "vertical_font_color_tool_button");

    EQt::ToolButton* fontBackgroundColorToolButton = new EQt::ToolButton(
        window->action("format_character_background_color_action")
    );
    controlsLayout->addWidget(fontBackgroundColorToolButton, 1, 4);
    dock->registerWidget(fontBackgroundColorToolButton, "vertical_font_background_color_tool_button");

    EQt::FontSelectorWidget* fontSelectorWidget = new EQt::FontSelectorWidget(
        window->action("format_font_family_action"),
        window->action("format_font_size_action")
    );
    fontSelectorWidget->setSizePolicy(QSizePolicy::Policy::Ignored, QSizePolicy::Policy::Preferred);
    verticalLayout->addWidget(fontSelectorWidget);
    dock->registerWidget(fontSelectorWidget, "vertical_font_selector_widget");

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}
