/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BlockFormatBuilderLate class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_combo_box.h>
#include <eqt_tool_button.h>
#include <eqt_spin_box.h>
#include <eqt_line_edit.h>

#include <ld_unordered_list_paragraph_format.h>
#include <ld_ordered_list_paragraph_format.h>

#include "application.h"
#include "main_window.h"
#include "block_format_main_window_proxy.h"
#include "block_format_view_proxy.h"
#include "block_format_builder_late.h"

const char BlockFormatBuilderLate::builderName[] = "BlockFormatBuilderLate";
const char BlockFormatBuilderLate::blockFormatDockName[] = "block_format";
const char BlockFormatBuilderLate::listFormatDockName[] = "list_format";

BlockFormatBuilderLate::BlockFormatBuilderLate(QObject* parent):EQt::Builder(parent) {}


BlockFormatBuilderLate::~BlockFormatBuilderLate() {}


unsigned BlockFormatBuilderLate::priority() const {
    return builderPriority;
}


const char* BlockFormatBuilderLate::builderIdentifier() const {
    return builderName;
}


const char* BlockFormatBuilderLate::plugInName() const {
    return "";
}


void BlockFormatBuilderLate::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addSeparator(tr("F&ormat"));

    QAction* leftAlignTextAction = window->addMenuAction(
        EQt::Application::icon("format_block_left_align"),
        tr("F&ormat | &Align | &Left"),
        "format_left_align_text"
    );
    leftAlignTextAction->setToolTip(tr("Left align text and other elements."));
    leftAlignTextAction->setWhatsThis(
        tr("You can use this button to left align text and other items within a paragraph.")
    );
    leftAlignTextAction->setCheckable(true);

    QAction* centerTextAction = window->addMenuAction(
        EQt::Application::icon("format_block_center"),
        tr("F&ormat | &Align | &Center"),
        "format_center_text"
    );
    centerTextAction->setToolTip(tr("Center text and other elements."));
    centerTextAction->setWhatsThis(
        tr("You can use this button to center text and other items within a paragraph.")
    );
    centerTextAction->setCheckable(true);

    QAction* rightAlignTextAction = window->addMenuAction(
        EQt::Application::icon("format_block_right_align"),
        tr("F&ormat | &Align | &Right"),
        "format_right_align_text"
    );
    rightAlignTextAction->setToolTip(tr("Right align text and other elements."));
    rightAlignTextAction->setWhatsThis(
        tr("You can use this button to right align text and other items within a paragraph.")
    );
    rightAlignTextAction->setCheckable(true);

    QAction* justifyTextAction = window->addMenuAction(
        EQt::Application::icon("format_block_justified"),
        tr("F&ormat | &Align | &Justify"),
        "format_justify_text"
    );
    justifyTextAction->setToolTip(tr("Justify text."));
    justifyTextAction->setWhatsThis(
        tr(
           "You can use this button to justify text between margins in a paragraph.  Text fragments and other "
           "elements that can not be justified will be left aligned."
          )
    );
    justifyTextAction->setCheckable(true);

    QAction* decreaseIndentAction = window->addMenuAction(
        EQt::Application::icon("format_block_decrease_indentation"),
        tr("F&ormat | &Indent | Move &Left"),
        "format_paragraph_decrease_indent"
    );
    decreaseIndentAction->setToolTip(tr("Decrease paragraph indentation."));
    decreaseIndentAction->setWhatsThis(
        tr("You can use this button to decrease the indentation of the selected paragraph(s) by a fixed amount.")
    );

    QAction* increaseIndentAction = window->addMenuAction(
        EQt::Application::icon("format_block_increase_indentation"),
        tr("F&ormat | &Indent | Move &Right"),
        "format_paragraph_increase_indent"
    );
    increaseIndentAction->setToolTip(tr("Increase paragraph indentation."));
    increaseIndentAction->setWhatsThis(
        tr("You can use this button to increase the indentation of the selected paragraph(s) by a fixed amount.")
    );

    QMenu* showHideListBulletNumberPopupMenu = new QMenu(window);
    window->registerWidget(showHideListBulletNumberPopupMenu, "show_hide_list_button_number_popup_menu");

    QAction* showListBulletNumberAction = showHideListBulletNumberPopupMenu->addAction(tr("&Show Bullet/Number"));
    window->addAction(showListBulletNumberAction, "show_list_bullet_number_action");
    showListBulletNumberAction->setCheckable(true);

    QAction* hideListBulletNumberAction = showHideListBulletNumberPopupMenu->addAction(tr("&Hide Bullet/Number"));
    window->addAction(hideListBulletNumberAction, "hide_list_bullet_number_action");
    hideListBulletNumberAction->setCheckable(true);
}


void BlockFormatBuilderLate::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* paragraphLeftIndentationAction = new QAction(window);
    window->addAction(paragraphLeftIndentationAction, "format_paragraph_left_indentation");
    paragraphLeftIndentationAction->setToolTip(tr("Use this to change paragraph left indentation."));
    paragraphLeftIndentationAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the paragraph left side identation.  Values can be "
            "entered in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* paragraphRightIndentationAction = new QAction(window);
    window->addAction(paragraphRightIndentationAction, "format_paragraph_right_indentation");
    paragraphRightIndentationAction->setToolTip(tr("Use this to change paragraph right indentation."));
    paragraphRightIndentationAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the paragraph right side identation.  Values can be "
            "entered in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* paragraphTopSpacingAction = new QAction(window);
    window->addAction(paragraphTopSpacingAction, "format_paragraph_top_spacing");
    paragraphTopSpacingAction->setToolTip(tr("Use this to change paragraph top spacing."));
    paragraphTopSpacingAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the paragraph top spacing.  Values can be entered in "
            "inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* paragraphBottomSpacingAction = new QAction(window);
    window->addAction(paragraphBottomSpacingAction, "format_paragraph_bottom_spacing");
    paragraphBottomSpacingAction->setToolTip(tr("Use this to change paragraph bottom spacing."));
    paragraphBottomSpacingAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the paragraph bottom spacing.  Values can be entered in "
            "inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, if you do not "
            "provide a unit."
        )
    );

    QAction* paragraphFirstLineIndentationAction = new QAction(window);
    window->addAction(paragraphFirstLineIndentationAction, "format_paragraph_first_line_indentation");
    paragraphFirstLineIndentationAction->setToolTip(tr("Use this to change the first line indentation."));
    paragraphFirstLineIndentationAction->setWhatsThis(
        tr(
            "You can use this area to enter a new value for the paragraph first line indentation.  By adjusting this "
            "value relative to the left indentation, you can indent the first line or create a hanging indentation.  "
            "Values can be entered in inches, cm, mm, or points.  Inches or cm will be assumed, based on your locale, "
            "if you do not provide a unit."
        )
    );

    QAction* listIndentationAction = new QAction(window);
    window->addAction(listIndentationAction, "format_list_indentation");
    listIndentationAction->setToolTip(
        tr("Use this to set the indentation from the list bullet/number to paragraph text.")
    );
    listIndentationAction->setWhatsThis(
        tr(
           "You can use this area to enter an indentation from the left edge of a list's bullet or number to the left "
           "edge of a lists's paragraph text.  Values can be entered in inches, cm, mm, points, or picas.  Inches or"
           "cm will be assumed, based on your locale."
          )
    );

    QAction* paragraphLineSpacingAction = new QAction(window);
    window->addAction(paragraphLineSpacingAction, "format_paragraph_line_spacing");
    paragraphLineSpacingAction->setToolTip("Use this to change line spacing.");
    paragraphLineSpacingAction->setWhatsThis(
        tr("You can use this combo box to change paragraph line spacing.")
    );

    QAction* listBulletNumberStyleAction = new QAction(window);
    listBulletNumberStyleAction->setIcon(EQt::Application::icon("format_list_bullet_number_style"));
    listBulletNumberStyleAction->setToolTip(tr("Use this button to set the bullet or number style."));
    listBulletNumberStyleAction->setWhatsThis(
        tr(
           "You can use this button to add bullets or numbers to a paragraph.  Clicking this button will bring up a "
           "pop-up dialog you can use to select a bullet or number style."
           )
    );
    window->addAction(listBulletNumberStyleAction, "format_list_bullet_number_style_action");

    QAction* showHideBulletNumberAction = new QAction(window);
    showHideBulletNumberAction->setIcon(EQt::Application::icon("format_list_show_hide_bullets_numbers"));
    showHideBulletNumberAction->setCheckable(true);
    showHideBulletNumberAction->setToolTip(
        tr("Use this button to display or hide list bullets or numbers for a paragraph.")
    );
    showHideBulletNumberAction->setWhatsThis(
        tr(
           "You can use this button to display or hide the list paragraph bullet or number.  The paragraph will "
           "remain part of the list."
          )
    );
    window->addAction(showHideBulletNumberAction, "format_list_show_hide_bullet_numbers");

    QAction* listBulletNumberFontFormatDialogAction = new QAction(window);
    listBulletNumberFontFormatDialogAction->setIcon(EQt::Application::icon("format_list_bullet_number_font"));
    listBulletNumberFontFormatDialogAction->setToolTip(
        tr("You can use this button to change the font used to render list bullets or numbers.")
    );
    listBulletNumberFontFormatDialogAction->setWhatsThis(
        tr(
           "You can click this button to display a dialog allowing you to change the font used to render the list's "
           "bullets or numbers."
          )
    );
    window->addAction(listBulletNumberFontFormatDialogAction, "list_bullet_number_font_format_dialog_action");

    QAction* listStartingNumberAction = new QAction(window);
    listStartingNumberAction->setToolTip(tr("Use this control to set or change the starting number of a list."));
    listStartingNumberAction->setWhatsThis(
        tr(
           "You can use this spin box to set or change the starting paragraph number in a list.  The number \"1\" "
           "will indicating \"1\", \"A\", \"a\", \"I\", or \"i\" depending on the selected numbering style.  The "
           "value \"0\" will be displayed as \"0\" or \"nulla\" for Roman numerals."
          )
    );
    window->addAction(listStartingNumberAction, "format_list_starting_number_action");

    QAction* listPrefixAction = new QAction(window);
    listPrefixAction->setToolTip(tr("Use this control to add a prefix in front of list numbers."));
    listPrefixAction->setWhatsThis(
        tr(
           "You can use this field to set or change the prefix text to be placed in front of list numbers.  For"
           "example, if you wanted list numbers to appear as \"(1)\", you would place \"(\" in this field."
        )
    );
    window->addAction(listPrefixAction, "format_list_prefix_action");

    QAction* listSuffixAction = new QAction(window);
    listSuffixAction->setToolTip(tr("Use this control to add a suffix in front of list numbers."));
    listSuffixAction->setWhatsThis(
        tr(
           "You can use this field to set or change the suffix text to be placed in front of list numbers.  For"
           "example, if you wanted list numbers to appear as \"(1)\", you would place \")\" in this field."
        )
    );
    window->addAction(listSuffixAction, "format_list_suffix_action");
}


void BlockFormatBuilderLate::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* blockDock = window->addDockWidget(
        blockFormatDockName,
        tr("Block"),
        EQt::DockWidgetDefaults(
            EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION,
            "character_format",
            true
        )
    );
    blockDock->setCloseButtonEnabled();

    QAction* blockDockToggleAction = window->addDockToggleMenuItem(
        blockFormatDockName,
        tr("&View | &Docks"),
        "block_format_dock_toggle_action"
    );
    blockDockToggleAction->setToolTip(tr("Show/hide the Block dock"));
    blockDockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Block dock.  The Block dock includes functions used to "
            "the spacing and formats of blocks, such as paragraphs."
        )
    );

    QHBoxLayout* blockDockHorizontalLayout = blockDock->newHBoxLayout("horizontal");
    blockDock->setHorizontalLayout(blockDockHorizontalLayout);

    buildBlockDockHorizontalOrientation(blockDock, blockDockHorizontalLayout, window);

    QVBoxLayout* blockDockVerticalLayout = blockDock->newVBoxLayout("vertical");
    blockDock->setVerticalLayout(blockDockVerticalLayout);

    buildBlockDockVerticalOrientation(blockDock, blockDockVerticalLayout, window);

    EQt::ProgrammaticDockWidget* listDock = window->addDockWidget(
        listFormatDockName,
        tr("List"),
        EQt::DockWidgetDefaults(
            EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION,
            blockFormatDockName,
            true
        )
    );
    listDock->setCloseButtonEnabled();

    QAction* listDockToggleAction = window->addDockToggleMenuItem(
        listFormatDockName,
        tr("&View | &Docks"),
        "list_format_dock_toggle_action"
    );
    listDockToggleAction->setToolTip(tr("Show/hide the List dock"));
    listDockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the List dock.  The List dock includes functions used to "
            "add, modify, or remove bullets from numbers from paragraphs making up a list."
        )
    );

    QHBoxLayout* listDockHorizontalLayout = listDock->newHBoxLayout("horizontal");
    listDock->setHorizontalLayout(listDockHorizontalLayout);

    buildListDockHorizontalOrientation(listDock, listDockHorizontalLayout, window);

    QVBoxLayout* listDockVerticalLayout = listDock->newVBoxLayout("vertical");
    listDock->setVerticalLayout(listDockVerticalLayout);

    buildListDockVerticalOrientation(listDock, listDockVerticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* BlockFormatBuilderLate::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new BlockFormatMainWindowProxy(window);
}


void BlockFormatBuilderLate::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QMenu*   rightClickMenu          = mainWindow->widget<QMenu>("right_click_menu");
    QAction* formatBlockDialogAction = mainWindow->action("format_block_dialog");

    rightClickMenu->addSeparator();
    rightClickMenu->addAction(formatBlockDialogAction);

    QAction* blockFormatDockToggleAction = mainWindow->action("block_format_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("bdock"), blockFormatDockToggleAction);
    Q_ASSERT(success);

    success = mainWindow->setCommand(tr("bfmt"), formatBlockDialogAction);
    Q_ASSERT(success);

    QAction* leftAlignTextAction = mainWindow->action("format_left_align_text");
    success = mainWindow->setCommand(tr("alignl"), leftAlignTextAction);
    Q_ASSERT(success);

    QAction* centerTextAction = mainWindow->action("format_center_text");
    success = mainWindow->setCommand(tr("alignc"), centerTextAction);
    Q_ASSERT(success);

    QAction* rightAlignTextAction = mainWindow->action("format_right_align_text");
    success = mainWindow->setCommand(tr("alignr"), rightAlignTextAction);
    Q_ASSERT(success);

    QAction* justifyTextAction = mainWindow->action("format_justify_text");
    success = mainWindow->setCommand(tr("alignj"), justifyTextAction);
    Q_ASSERT(success);

    QAction* listFormatDockToggleAction = mainWindow->action("list_format_dock_toggle_action");
    success = mainWindow->setCommand(tr("ldock"), listFormatDockToggleAction);
    Q_ASSERT(success);

    QAction* formatListDialogAction = mainWindow->action("format_list_dialog");
    success = mainWindow->setCommand(tr("lfmt"), formatListDialogAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* BlockFormatBuilderLate::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new BlockFormatViewProxy(view, window);
}


void BlockFormatBuilderLate::buildBlockDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("horizontal_controls");
    horizontalLayout->addLayout(controlsLayout);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_left_align_text")), 0, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_center_text")), 0, 1);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_right_align_text")), 0, 2);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_justify_text")), 0, 3);

    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_paragraph_decrease_indent")), 1, 0);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_paragraph_increase_indent")), 1, 1);
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_block_dialog")), 1, 3);

    QFormLayout* paragraphMarginLayout = dock->newFormLayout("horizontal_margins");
    horizontalLayout->addLayout(paragraphMarginLayout);

    EQt::DimensionLineEdit* paragraphLeftIndentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphRightIdentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphTopSpacingLineEdit      = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphBottomSpacingLineEdit   = new EQt::DimensionLineEdit;

    dock->registerWidget(paragraphLeftIndentationLineEdit, "horizontal_left_indentation_line_edit");
    dock->registerWidget(paragraphRightIdentationLineEdit, "horizontal_right_indentation_line_edit");
    dock->registerWidget(paragraphTopSpacingLineEdit, "horizontal_top_spacing_line_edit");
    dock->registerWidget(paragraphBottomSpacingLineEdit, "horizontal_bottom_spacing_line_edit");

    paragraphLeftIndentationLineEdit->setDynamicUnits();
    paragraphRightIdentationLineEdit->setDynamicUnits();
    paragraphTopSpacingLineEdit->setDynamicUnits();
    paragraphBottomSpacingLineEdit->setDynamicUnits();

    paragraphLeftIndentationLineEdit->setMinimumValue(0);
    paragraphRightIdentationLineEdit->setMinimumValue(0);
    paragraphTopSpacingLineEdit->setMinimumValue(0);
    paragraphBottomSpacingLineEdit->setMinimumValue(0);

    paragraphLeftIndentationLineEdit->setDefaultAction(window->action("format_paragraph_left_indentation"));
    paragraphRightIdentationLineEdit->setDefaultAction(window->action("format_paragraph_right_indentation"));
    paragraphTopSpacingLineEdit->setDefaultAction(window->action("format_paragraph_top_spacing"));
    paragraphBottomSpacingLineEdit->setDefaultAction(window->action("format_paragraph_bottom_spacing"));

    paragraphMarginLayout->addRow(tr("Top:"), paragraphTopSpacingLineEdit);
    paragraphMarginLayout->addRow(tr("Left:"), paragraphLeftIndentationLineEdit);
    paragraphMarginLayout->addRow(tr("Right:"), paragraphRightIdentationLineEdit);
    paragraphMarginLayout->addRow(tr("Bottom:"), paragraphBottomSpacingLineEdit);

    QVBoxLayout* paragraphControlsLayout = dock->newVBoxLayout("horizontal_controls_layout");
    horizontalLayout->addLayout(paragraphControlsLayout);

    QFormLayout* paragraphFirstLineMarginLayout = dock->newFormLayout("horizontal_first_line_margins");
    paragraphControlsLayout->addLayout(paragraphFirstLineMarginLayout);

    QAction* paragraphFirstLineIndentationAction = window->action(
        "format_paragraph_first_line_indentation"
    );

    EQt::DimensionLineEdit* paragraphFirstLineIndentationLineEdit = new EQt::DimensionLineEdit;
    dock->registerWidget(paragraphFirstLineIndentationLineEdit, "horizontal_first_line_indentation_line_edit");

    paragraphFirstLineIndentationLineEdit->setDynamicUnits();
    paragraphFirstLineIndentationLineEdit->setDefaultAction(paragraphFirstLineIndentationAction);

    paragraphFirstLineMarginLayout->addRow(tr("First Line:"), paragraphFirstLineIndentationLineEdit);

    QAction* listIndentationAction = window->action("format_list_indentation");
    EQt::DimensionLineEdit* listIndentationLineEdit = new EQt::DimensionLineEdit(listIndentationAction);
    dock->registerWidget(listIndentationLineEdit, "horizontal_list_indentation_line_edit");

    listIndentationLineEdit->setDynamicUnits();
    listIndentationLineEdit->setMinimumValue(0);

    paragraphFirstLineMarginLayout->addRow(tr("List:"), listIndentationLineEdit);

    QAction* paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");
    EQt::ComboBox* paragraphLineSpacingComboBox = new EQt::ComboBox(EQt::ComboBox::ReportedValue::VARIANT);
    paragraphLineSpacingComboBox->addItem(tr("Single Spacing"), QVariant(1.0F));
    paragraphLineSpacingComboBox->addItem(tr("1%1 Line Spacing").arg(QChar(0xBC)), QVariant(1.25F));
    paragraphLineSpacingComboBox->addItem(tr("1%1 Line Spacing").arg(QChar(0xBD)), QVariant(1.5F));
    paragraphLineSpacingComboBox->addItem(tr("Double Spacing"), QVariant(2.0F));
    paragraphLineSpacingComboBox->setDefaultAction(paragraphLineSpacingAction);

    dock->registerWidget(paragraphLineSpacingComboBox, "horizontal_line_spacing_combo_box");
    paragraphControlsLayout->addWidget(paragraphLineSpacingComboBox);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void BlockFormatBuilderLate::buildBlockDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QHBoxLayout* topHorizontalLayout = dock->newHBoxLayout("vertical_top_horizontal");
    verticalLayout->addLayout(topHorizontalLayout);

    topHorizontalLayout->addWidget(new EQt::ToolButton(window->action("format_left_align_text")));
    topHorizontalLayout->addWidget(new EQt::ToolButton(window->action("format_center_text")));
    topHorizontalLayout->addWidget(new EQt::ToolButton(window->action("format_right_align_text")));
    topHorizontalLayout->addWidget(new EQt::ToolButton(window->action("format_justify_text")));

    QHBoxLayout* middleHorizontalLayout = dock->newHBoxLayout("vertical_middle_horizontal");
    verticalLayout->addLayout(middleHorizontalLayout);

    QGridLayout* topControlsLayout = dock->newGridLayout("vertical_controls");
    middleHorizontalLayout->addLayout(topControlsLayout);

    EQt::ToolButton* decreaseIndentButton = new EQt::ToolButton(window->action("format_paragraph_decrease_indent"));
    dock->registerWidget(decreaseIndentButton, "decrease_indent_tool_button");
    topControlsLayout->addWidget(decreaseIndentButton, 0, 0);
    topControlsLayout->addWidget(new EQt::ToolButton(window->action("format_paragraph_increase_indent")), 0, 1);
    topControlsLayout->addWidget(new EQt::ToolButton(window->action("format_block_dialog")), 1, 0);

    QFormLayout* paragraphMarginLayout = dock->newFormLayout("vertical_margins");
    middleHorizontalLayout->addLayout(paragraphMarginLayout);

    EQt::DimensionLineEdit* paragraphLeftIndentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphRightIdentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphTopSpacingLineEdit      = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphBottomSpacingLineEdit   = new EQt::DimensionLineEdit;

    dock->registerWidget(paragraphLeftIndentationLineEdit, "vertical_left_indentation_line_edit");
    dock->registerWidget(paragraphRightIdentationLineEdit, "vertical_right_indentation_line_edit");
    dock->registerWidget(paragraphTopSpacingLineEdit, "vertical_top_spacing_line_edit");
    dock->registerWidget(paragraphBottomSpacingLineEdit, "vertical_bottom_spacing_line_edit");

    paragraphLeftIndentationLineEdit->setDynamicUnits();
    paragraphRightIdentationLineEdit->setDynamicUnits();
    paragraphTopSpacingLineEdit->setDynamicUnits();
    paragraphBottomSpacingLineEdit->setDynamicUnits();

    paragraphLeftIndentationLineEdit->setMinimumValue(0);
    paragraphRightIdentationLineEdit->setMinimumValue(0);
    paragraphTopSpacingLineEdit->setMinimumValue(0);
    paragraphBottomSpacingLineEdit->setMinimumValue(0);

    paragraphLeftIndentationLineEdit->setDefaultAction(window->action("format_paragraph_left_indentation"));
    paragraphRightIdentationLineEdit->setDefaultAction(window->action("format_paragraph_right_indentation"));
    paragraphTopSpacingLineEdit->setDefaultAction(window->action("format_paragraph_top_spacing"));
    paragraphBottomSpacingLineEdit->setDefaultAction(window->action("format_paragraph_bottom_spacing"));

    paragraphMarginLayout->addRow(tr("Top:"), paragraphTopSpacingLineEdit);
    paragraphMarginLayout->addRow(tr("Left:"), paragraphLeftIndentationLineEdit);
    paragraphMarginLayout->addRow(tr("Right:"), paragraphRightIdentationLineEdit);
    paragraphMarginLayout->addRow(tr("Bottom:"), paragraphBottomSpacingLineEdit);

    QAction* paragraphFirstLineIndentationAction = window->action(
        "format_paragraph_first_line_indentation"
    );

    EQt::DimensionLineEdit* paragraphFirstLineIndentationLineEdit = new EQt::DimensionLineEdit;
    dock->registerWidget(paragraphFirstLineIndentationLineEdit, "vertical_first_line_indentation_line_edit");

    QFormLayout* firstLineFormLayout = dock->newFormLayout("vertical_first_line_margin");
    verticalLayout->addLayout(firstLineFormLayout);

    paragraphFirstLineIndentationLineEdit->setDynamicUnits();
    paragraphFirstLineIndentationLineEdit->setDefaultAction(paragraphFirstLineIndentationAction);

    firstLineFormLayout->addRow(tr("First Line:"), paragraphFirstLineIndentationLineEdit);

    QAction* listIndentationAction = window->action("format_list_indentation");

    EQt::DimensionLineEdit* listIndentationLineEdit = new EQt::DimensionLineEdit;
    dock->registerWidget(listIndentationLineEdit, "vertical_list_indentation_line_edit");

    listIndentationLineEdit->setDynamicUnits();
    listIndentationLineEdit->setMinimumValue(0);
    listIndentationLineEdit->setDefaultAction(listIndentationAction);

    firstLineFormLayout->addRow(tr("List:"), listIndentationLineEdit);

    QAction* paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");
    EQt::ComboBox* paragraphLineSpacingComboBox = new EQt::ComboBox(EQt::ComboBox::ReportedValue::VARIANT);
    paragraphLineSpacingComboBox->addItem(tr("Single Spacing"), QVariant(1.0F));
    paragraphLineSpacingComboBox->addItem(tr("1%1 Line Spacing").arg(QChar(0xBC)), QVariant(1.25F));
    paragraphLineSpacingComboBox->addItem(tr("1%1 Line Spacing").arg(QChar(0xBD)), QVariant(1.5F));
    paragraphLineSpacingComboBox->addItem(tr("Double Spacing"), QVariant(2.0F));
    paragraphLineSpacingComboBox->addItem(tr("2%1 Line Spacing").arg(QChar(0xBD)), QVariant(2.5F));
    paragraphLineSpacingComboBox->addItem(tr("Triple Line Spacing"), QVariant(3.0F));
    paragraphLineSpacingComboBox->setDefaultAction(paragraphLineSpacingAction);

    dock->registerWidget(paragraphLineSpacingComboBox, "vertical_line_spacing_combo_box");
    verticalLayout->addWidget(paragraphLineSpacingComboBox);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void BlockFormatBuilderLate::buildListDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* controlsLayout = dock->newGridLayout("horizontal_controls");
    horizontalLayout->addLayout(controlsLayout);

    QAction* listBulletNumberStyleAction = window->action("format_list_bullet_number_style_action");
    EQt::ToolButton* listBulletNumberStyleToolButton = new EQt::ToolButton(listBulletNumberStyleAction);
    window->registerWidget(listBulletNumberStyleToolButton, "horizontal_list_bullet_number_style_tool_button");
    controlsLayout->addWidget(listBulletNumberStyleToolButton, 0, 0);

    QMenu* showHideListBulletNumberPopupMenu = window->widget<QMenu>("show_hide_list_button_number_popup_menu");
    EQt::ToolButton* showHideListBulletNumberToolButton = new EQt::ToolButton(
        window->action("format_list_show_hide_bullet_numbers"),
        showHideListBulletNumberPopupMenu
    );
    controlsLayout->addWidget(showHideListBulletNumberToolButton, 0, 1);

    controlsLayout->addWidget(
        new EQt::ToolButton(window->action("list_bullet_number_font_format_dialog_action")),
        1,
        0
    );
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_list_dialog")), 1, 1);

    controlsLayout->setColumnMinimumWidth(2, 10);

    QFormLayout* fieldLayout = dock->newFormLayout("horizontal_fields");
    controlsLayout->addLayout(fieldLayout, 0, 3, 2, 1);

    QFontMetrics fontMetrics(EQt::Application::font());
    unsigned lineEditWidth = static_cast<unsigned>(fontMetrics.horizontalAdvance("MMMM"));

    EQt::SpinBox* startingNumberSpinBox = new EQt::SpinBox(window->action("format_list_starting_number_action"));
    startingNumberSpinBox->setMinimum(0);
    startingNumberSpinBox->setMaximum(3999);
    fieldLayout->addRow(tr("Starts At:"), startingNumberSpinBox);
    window->registerWidget(startingNumberSpinBox, "horizontal_starting_number_spin_box");

    EQt::LineEdit* prefixLineEdit = new EQt::LineEdit(window->action("format_list_prefix_action"));
    prefixLineEdit->setFixedWidth(lineEditWidth);
    fieldLayout->addRow(tr("Prefix:"), prefixLineEdit);
    window->registerWidget(prefixLineEdit, "horizontal_prefix_line_edit");

    EQt::LineEdit* suffixLineEdit = new EQt::LineEdit(window->action("format_list_suffix_action"));
    suffixLineEdit->setFixedWidth(lineEditWidth);
    fieldLayout->addRow(tr("Suffix:"), suffixLineEdit);
    window->registerWidget(suffixLineEdit, "horizontal_suffix_line_edit");

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void BlockFormatBuilderLate::buildListDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("vertical_horizontal_layout");
    verticalLayout->addLayout(horizontalLayout);

    QGridLayout* controlsLayout = dock->newGridLayout("vertical_controls");
    horizontalLayout->addLayout(controlsLayout);

    QAction* listBulletNumberStyleAction = window->action("format_list_bullet_number_style_action");
    EQt::ToolButton* listBulletNumberStyleToolButton = new EQt::ToolButton(listBulletNumberStyleAction);
    window->registerWidget(listBulletNumberStyleToolButton, "vertical_list_bullet_number_style_tool_button");
    controlsLayout->addWidget(listBulletNumberStyleToolButton, 0, 0);

    QMenu* showHideListBulletNumberPopupMenu = window->widget<QMenu>("show_hide_list_button_number_popup_menu");
    EQt::ToolButton* showHideListBulletNumberToolButton = new EQt::ToolButton(
        window->action("format_list_show_hide_bullet_numbers"),
        showHideListBulletNumberPopupMenu
    );
    controlsLayout->addWidget(showHideListBulletNumberToolButton, 0, 1);

    controlsLayout->addWidget(new EQt::ToolButton(
        window->action("list_bullet_number_font_format_dialog_action")),
        1,
        0
    );
    controlsLayout->addWidget(new EQt::ToolButton(window->action("format_list_dialog")), 1, 1);

    controlsLayout->setColumnMinimumWidth(2, 10);

    QFormLayout* fieldLayout = dock->newFormLayout("vertical_fields");
    horizontalLayout->addLayout(fieldLayout);

    QFontMetrics fontMetrics(EQt::Application::font());
    unsigned lineEditWidth = static_cast<unsigned>(fontMetrics.horizontalAdvance("MMMM"));

    EQt::SpinBox* startingNumberSpinBox = new EQt::SpinBox(window->action("format_list_starting_number_action"));
    startingNumberSpinBox->setMinimum(0);
    startingNumberSpinBox->setMaximum(3999);
    fieldLayout->addRow(tr("Starts At:"), startingNumberSpinBox);
    window->registerWidget(startingNumberSpinBox, "vertical_starting_number_spin_box");

    EQt::LineEdit* prefixLineEdit = new EQt::LineEdit(window->action("format_list_prefix_action"));
    prefixLineEdit->setFixedWidth(lineEditWidth);
    fieldLayout->addRow(tr("Prefix:"), prefixLineEdit);
    window->registerWidget(prefixLineEdit, "vertical_prefix_line_edit");

    EQt::LineEdit* suffixLineEdit = new EQt::LineEdit(window->action("format_list_suffix_action"));
    suffixLineEdit->setFixedWidth(lineEditWidth);
    fieldLayout->addRow(tr("Suffix:"), suffixLineEdit);
    window->registerWidget(suffixLineEdit, "vertical_suffix_line_edit");

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}
