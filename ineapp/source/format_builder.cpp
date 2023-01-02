/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FormatBuilder class.
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
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_paper_size_popup_menu.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_combo_box.h>
#include <eqt_tool_button.h>

#include "format_view_proxy.h"
#include "format_builder.h"

const char FormatBuilder::builderName[] = "FormatBuilder";

FormatBuilder::FormatBuilder(QObject* parent):EQt::Builder(parent) {}


FormatBuilder::~FormatBuilder() {}


unsigned FormatBuilder::priority() const {
    return builderPriority;
}


const char* FormatBuilder::builderIdentifier() const {
    return builderName;
}


const char* FormatBuilder::plugInName() const {
    return "";
}


void FormatBuilder::registerResources(EQt::ProgrammaticApplication* application) {}


void FormatBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    window->addMenuAction(
        EQt::Application::icon("format_paragraph"),
        tr("F&ormat | &Paragraph"),
        "format_paragraph_dialog"
    );
    window->setToolTip("format_paragraph_dialog", tr("Change the paragraph format"));
    window->setWhatsThis(
        "format_paragraph_dialog",
        tr(
            "Use this option to update or change the paragraph spacing, left and right margins, indentation, and "
            "line spacing."
          )
    );

    window->addMenuAction(
        EQt::Application::icon("format_font"),
        tr("F&ormat | &Character"),
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

    window->addSeparator(tr("F&ormat"));

    window->addMenuAction(tr("F&ormat | &Table"), "format_table_dialog");
    window->setToolTip("format_table_dialog", tr("Change the table format"));
    window->setWhatsThis(
        "format_table_dialog",
        tr(
            "Use this option to update or change table global formatting such as top and bottom spacing, "
            "table indentation, and column widths."
          )
    );

    window->addMenuAction(tr("F&ormat | Table &Cell"), "format_table_cell_dialog");
    window->setToolTip("format_table_cell_dialog", tr("Change the table cell format"));
    window->setWhatsThis(
        "format_table_cell_dialog",
        tr("Use this option to update or change table cell properties such as borders and cell background color.")
    );
}


void FormatBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* leftAlignTextAction = new QAction(
        EQt::Application::icon("format_left_align_text"),
        tr("Left align"),
        window
    );
    window->addAction(leftAlignTextAction, "format_left_align_text");
    leftAlignTextAction->setToolTip(tr("Click here to left align text and other elements."));
    leftAlignTextAction->setWhatsThis(
        tr("You can use this button to left align text and other items within a paragraph.")
    );
    leftAlignTextAction->setCheckable(true);

    QAction* centerTextAction = new QAction(EQt::Application::icon("format_center_text"), tr("Center"), window);
    window->addAction(centerTextAction, "format_center_text");
    centerTextAction->setToolTip(tr("Click here to center text and other elements."));
    centerTextAction->setWhatsThis(
        tr("You can use this button to center text and other items within a paragraph.")
    );
    centerTextAction->setCheckable(true);

    QAction* rightAlignTextAction = new QAction(
        EQt::Application::icon("format_right_align_text"),
        tr("Right align"),
        window
    );
    window->addAction(rightAlignTextAction, "format_right_align_text");
    rightAlignTextAction->setToolTip(tr("Click here to right align text and other elements."));
    rightAlignTextAction->setWhatsThis(
        tr("You can use this button to right align text and other items within a paragraph.")
    );
    rightAlignTextAction->setCheckable(true);

    QAction* justifyTextAction = new QAction(EQt::Application::icon("format_justify_text"), tr("Justify"), window);
    window->addAction(justifyTextAction, "format_justify_text");
    justifyTextAction->setToolTip(tr("Click here to justify text."));
    justifyTextAction->setWhatsThis(
        tr(
           "You can use this button to justify text between margins in a paragraph.  Text fragments and other "
           "elements that can not be justified will be left aligned."
          )
    );
    justifyTextAction->setCheckable(true);

    QAction* decreaseIndentAction = new QAction(
        EQt::Application::icon("format_paragraph_decrease_indent"),
        tr("Decrease indentation"),
        window
    );
    window->addAction(decreaseIndentAction, "format_paragraph_decrease_indent");
    decreaseIndentAction->setToolTip(tr("Click here to decrease paragraph indentation."));
    decreaseIndentAction->setWhatsThis(
        tr("You can use this button to decrease the indentation of the selected paragraph(s) by a fixed amount.")
    );

    QAction* increaseIndentAction = new QAction(
        EQt::Application::icon("format_paragraph_increase_indent"),
        tr("Increase indentation"),
        window
    );
    window->addAction(increaseIndentAction, "format_paragraph_increase_indent");
    increaseIndentAction->setToolTip(tr("Click here to increase paragraph indentation."));
    increaseIndentAction->setWhatsThis(
        tr("You can use this button to increase the indentation of the selected paragraph(s) by a fixed amount.")
    );

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

    QAction* paragraphLineSpacingAction = new QAction(window);
    window->addAction(paragraphLineSpacingAction, "format_paragraph_line_spacing");
    paragraphLineSpacingAction->setToolTip("Use this to change line spacing.");
    paragraphLineSpacingAction->setWhatsThis(
        tr("You can use this combo box to change paragraph line spacing.")
    );
}


void FormatBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget("format", tr("Format"));

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");
    dock->setHorizontalLayout(horizontalLayout);

    buildFormatDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildFormatDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* FormatBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return Q_NULLPTR;
}


void FormatBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow* window) {
}


EQt::ProgrammaticViewProxy* FormatBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new FormatViewProxy(view, window);
}


void FormatBuilder::buildFormatDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    /* List format */

    /* Paragraph format */

    QGridLayout* paragraphLayout = dock->newGridLayout("horizontal_paragraph");
    horizontalLayout->addWidget(dock->newGroupBox(tr("Block"), paragraphLayout));

    paragraphLayout->addWidget(new EQt::ToolButton(window->action("format_left_align_text")), 0, 0);
    paragraphLayout->addWidget(new EQt::ToolButton(window->action("format_center_text")), 0, 1);
    paragraphLayout->addWidget(new EQt::ToolButton(window->action("format_right_align_text")), 0, 2);
    paragraphLayout->addWidget(new EQt::ToolButton(window->action("format_justify_text")), 0, 3);

    paragraphLayout->addWidget(new EQt::ToolButton(window->action("format_paragraph_decrease_indent")), 1, 0);
    paragraphLayout->addWidget(new EQt::ToolButton(window->action("format_paragraph_increase_indent")), 1, 1);
    paragraphLayout->addWidget(new EQt::ToolButton(window->action("format_paragraph_dialog")), 1, 3);

    QFormLayout* paragraphMarginLayout = dock->newFormLayout("horizontal_paragraph_margins");
    paragraphLayout->addLayout(paragraphMarginLayout, 0, 4, 2, 1);

    EQt::DimensionLineEdit* paragraphLeftIndentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphRightIdentationLineEdit = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphTopSpacingLineEdit      = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* paragraphBottomSpacingLineEdit   = new EQt::DimensionLineEdit;

    dock->registerWidget(paragraphLeftIndentationLineEdit, "horizontal_paragraph_left_indentation_line_edit");
    dock->registerWidget(paragraphRightIdentationLineEdit, "horizontal_paragraph_right_indentation_line_edit");
    dock->registerWidget(paragraphTopSpacingLineEdit, "horizontal_paragraph_top_spacing_line_edit");
    dock->registerWidget(paragraphBottomSpacingLineEdit, "horizontal_paragraph_bottom_spacing_line_edit");

    paragraphLeftIndentationLineEdit->setDefaultAction(window->action("format_paragraph_left_indentation"));
    paragraphRightIdentationLineEdit->setDefaultAction(window->action("format_paragraph_right_indentation"));
    paragraphTopSpacingLineEdit->setDefaultAction(window->action("format_paragraph_top_spacing"));
    paragraphBottomSpacingLineEdit->setDefaultAction(window->action("format_paragraph_bottom_spacing"));

    paragraphMarginLayout->addRow(tr("Top:"), paragraphTopSpacingLineEdit);
    paragraphMarginLayout->addRow(tr("Left:"), paragraphLeftIndentationLineEdit);
    paragraphMarginLayout->addRow(tr("Right:"), paragraphRightIdentationLineEdit);
    paragraphMarginLayout->addRow(tr("Bottom:"), paragraphBottomSpacingLineEdit);

    QFormLayout* paragraphFirstLineMarginLayout = dock->newFormLayout("horizontal_paragraph_first_line_margins");
    paragraphLayout->addLayout(paragraphFirstLineMarginLayout, 0, 5);


    QAction* paragraphFirstLineIndentationAction = window->action(
        "format_paragraph_first_line_indentation"
    );

    EQt::DimensionLineEdit* paragraphFirstLineIndentationLineEdit = new EQt::DimensionLineEdit;
    dock->registerWidget(
        paragraphFirstLineIndentationLineEdit,
        "horizontal_paragraph_first_line_indentation_line_edit"
    );
    paragraphFirstLineIndentationLineEdit->setDefaultAction(paragraphFirstLineIndentationAction);
    paragraphFirstLineMarginLayout->addRow(tr("First Line:"), paragraphFirstLineIndentationLineEdit);

    QAction* paragraphLineSpacingAction = window->action("format_paragraph_line_spacing");
    EQt::ComboBox* paragraphLineSpacingComboBox = new EQt::ComboBox(EQt::ComboBox::ReportedValue::VARIANT);
    paragraphLineSpacingComboBox->addItem(tr("Single Spacing"), QVariant(1.0F));
    paragraphLineSpacingComboBox->addItem(tr("1%1 Line Spacing").arg(QChar(0xBC)), QVariant(1.25F));
    paragraphLineSpacingComboBox->addItem(tr("1%1 Line Spacing").arg(QChar(0xBD)), QVariant(1.5F));
    paragraphLineSpacingComboBox->addItem(tr("Double Spacing"), QVariant(2.0F));
    paragraphLineSpacingComboBox->setDefaultAction(paragraphLineSpacingAction);

    paragraphLayout->addWidget(paragraphLineSpacingComboBox, 1, 5);

//    /* Font */

//    QGridLayout* fontLayout = newGridLayout("horizontal_font");
//    horizontalLayout->addWidget(newGroupBox(tr("Font"), fontLayout));

//    fontLayout->addWidget(
//        new EQt::FontSelectorWidget(
//            mainWindow->action("format_font_family"),
//            mainWindow->action("format_font_size")
//        ),
//        1,
//        0,
//        1,
//        4
//    );

//    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_boldface")), 0, 0);
//    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_italics")), 0, 1);
//    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_underline")), 0, 2);
//    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_subscript")), 0, 3);
//    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_superscript")), 0, 4);

//    EQt::ToolButton* formatFontButton = new EQt::ToolButton(mainWindow->action("format_font_dialog"));
//    formatFontButton->setToolTip(tr("Format Font"));
//    fontLayout->addWidget(formatFontButton, 1, 4);

//    /* Elements */

//    QGridLayout* elementsLayout = newGridLayout("horizontal_elements");
//    horizontalLayout->addWidget(newGroupBox(tr("Elements"), elementsLayout));

//    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_table_dialog")), 0, 0);
//    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_graph_dialog")), 0, 1);
//    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_image_dialog")), 1, 1);

    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void FormatBuilder::buildFormatDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
//    /* File menu items */

//    QHBoxLayout* fileHorizontalLayout = dock->newHBoxLayout("vertical_file");
//    verticalLayout->addWidget(dock->newGroupBox(tr("File"), fileHorizontalLayout));

//    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_new")));
//    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_open_dialog")));
//    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_save")));
//    fileHorizontalLayout->addWidget(new EQt::ToolButton(window->action("file_print_dialog")));

//    /* Edit menu items */

//    QGridLayout* editLayout = dock->newGridLayout("vertical_edit");
//    verticalLayout->addWidget(dock->newGroupBox(tr("Edit"), editLayout));

//    editLayout->addWidget(new EQt::ToolButton(window->action("edit_undo")), 0, 0);
//    editLayout->addWidget(new EQt::ToolButton(window->action("edit_redo")), 0, 1);
//    editLayout->setColumnMinimumWidth(2, 6);
//    editLayout->addWidget(new EQt::ToolButton(window->action("edit_cut")), 0, 3);
//    editLayout->addWidget(new EQt::ToolButton(window->action("edit_copy")), 0, 4);
//    editLayout->addWidget(new EQt::ToolButton(window->action("edit_paste")), 0, 5);

//    EQt::SearchLineEdit* searchLineEdit = new EQt::SearchLineEdit(window->action("edit_search"), dock);
//    searchLineEdit->setCaseSensitiveSearchAction(window->action("edit_search_case_sensitive"));
//    searchLineEdit->setWholeWordsOnlyAction(window->action("edit_search_whole_words_only"));

//    editLayout->addWidget(searchLineEdit, 1, 0, 1, 4, Qt::AlignJustify);
//    editLayout->addWidget(new EQt::ToolButton(window->action("edit_search_next")), 1, 4);
//    editLayout->addWidget(new EQt::ToolButton(window->action("edit_search_previous")), 1, 5);

//    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);

//    /* View menu items */

//    QGridLayout* viewLayout = dock->newGridLayout("vertical_view");
//    verticalLayout->addWidget(dock->newGroupBox(tr("View"), viewLayout));

//    EQt::ZoomComboBox* zoomComboBox = new EQt::ZoomComboBox(window->action("zoom_action"), dock);
//    zoomComboBox->setPageWidthStringExcluded();
//    zoomComboBox->setFullPageStringExcluded();
//    window->registerWidget(zoomComboBox, "vertical_zoom_combo_box");
//    viewLayout->addWidget(zoomComboBox, 1, 3, 1, 2, Qt::AlignCenter);
//    zoomComboBox->setToolTip(tr("Zoom"));

//    viewLayout->addWidget(new EQt::ToolButton(window->action("view_margins")), 0, 0);
//    viewLayout->addWidget(new EQt::ToolButton(window->action("view_guides")), 0, 1);
//    viewLayout->setColumnMinimumWidth(2, 6);
//    viewLayout->addWidget(new EQt::ToolButton(window->action("view_zoom_in")), 0, 3);
//    viewLayout->addWidget(new EQt::ToolButton(window->action("view_zoom_out")), 0, 4);
}
