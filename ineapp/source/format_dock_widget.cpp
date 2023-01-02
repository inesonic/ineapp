/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref FormatDockWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QString>
#include <QAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QSpacerItem>
#include <QGroupBox>
#include <QSize>
#include <QMenu>
#include <QActionGroup>
#include <QLabel>

#include <eqt_dimension_line_edit.h>
#include <eqt_font_selector_widget.h>
#include <eqt_tool_button.h>
#include <eqt_paper_size_popup_menu.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>

#include "application.h"
#include "format_dock_widget.h"

FormatDockWidget::FormatDockWidget(
        EQt::ProgrammaticMainWindow* parent,
        Qt::WindowFlags              flags
    ):EQt::ProgrammaticDockWidget(
        tr("Format"),
        parent,
        flags
    ) {}


FormatDockWidget::~FormatDockWidget() {}


void FormatDockWidget::configureDockWidget(EQt::ProgrammaticMainWindow* mainWindow) {
    pageOrientationAction = new QAction(
        Application::icon("format_page_orientation_portrait"),
        tr("Page Orientation"),
        this
    );
    pageOrientationAction->setToolTip(tr("Set landscape page orientation"));

    connect(
        mainWindow->action("format_page_landscape_orientation"),
        SIGNAL(triggered(bool)),
        this,
        SLOT(pageOrientationNowLandscape(bool))
    );
    connect(
        pageOrientationAction,
        SIGNAL(triggered()),
        mainWindow->action("format_page_landscape_orientation"),
        SLOT(trigger())
    );

    paperSizePopupMenu = new EQt::PaperSizePopupMenu(mainWindow->action("format_page_size"), this);

    buildHorizontalOrientation(mainWindow);
    buildVerticalOrientation(mainWindow);
}


void FormatDockWidget::buildHorizontalOrientation(EQt::ProgrammaticMainWindow* mainWindow) {
    QHBoxLayout* mainLayout = newHBoxLayout("horizontal");
    setHorizontalLayout(mainLayout);

    /* Page format */

    QGridLayout* pageLayout = newGridLayout("horizontal_page");
    mainLayout->addWidget(newGroupBox(tr("Page"), pageLayout));

    horizontalPageOrientationButton = new EQt::ToolButton(pageOrientationAction);
    pageLayout->addWidget(horizontalPageOrientationButton,0, 0);

    EQt::ToolButton* paperSizeButton = new EQt::ToolButton(paperSizePopupMenu);
    paperSizeButton->setIcon(Application::icon("format_page_size"));
    paperSizeButton->setToolTip(tr("Page Size"));
    pageLayout->addWidget(paperSizeButton, 0, 1);

    EQt::ToolButton* formatPageButton = new EQt::ToolButton(mainWindow->action("format_page_dialog"));
    formatPageButton->setToolTip(tr("Format Page"));
    pageLayout->addWidget(formatPageButton, 1, 1);

    /* Margins */

    QFormLayout* marginLayout = newFormLayout("horizontal_margins");
    mainLayout->addWidget(newGroupBox(tr("Margins"), marginLayout));

    EQt::DimensionLineEdit* leftLineEdit   = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* rightLineEdit  = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* topLineEdit    = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* bottomLineEdit = new EQt::DimensionLineEdit;

    leftLineEdit->setDefaultAction(mainWindow->action("format_left_margin"));
    rightLineEdit->setDefaultAction(mainWindow->action("format_right_margin"));
    topLineEdit->setDefaultAction(mainWindow->action("format_top_margin"));
    bottomLineEdit->setDefaultAction(mainWindow->action("format_bottom_margin"));

    leftLineEdit->setToolTip(tr("Left Margin"));
    rightLineEdit->setToolTip(tr("Right Margin"));
    topLineEdit->setToolTip(tr("Top Margin"));
    bottomLineEdit->setToolTip(tr("Bottom Margin"));

    marginLayout->addRow(tr("Top:"), topLineEdit);
    marginLayout->addRow(tr("Left:"), leftLineEdit);
    marginLayout->addRow(tr("Right:"), rightLineEdit);
    marginLayout->addRow(tr("Bottom:"), bottomLineEdit);

    /* Paragraph format */

    QGridLayout* paragraphLayout = newGridLayout("horizontal_paragraph");
    mainLayout->addWidget(newGroupBox(tr("Paragraph"), paragraphLayout));

    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_left_align_text")), 0, 0);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_center_text")), 0, 1);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_right_align_text")), 0, 2);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_justify_text")), 0, 3);

    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_bullets_and_numbering_dialog")), 1, 0);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_paragraph_increase_indent")), 1, 1);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_paragraph_decrease_indent")), 1, 2);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_paragraph_dialog")), 1, 3);

    /* Font */

    QGridLayout* fontLayout = newGridLayout("horizontal_font");
    mainLayout->addWidget(newGroupBox(tr("Font"), fontLayout));

    fontLayout->addWidget(
        new EQt::FontSelectorWidget(
            mainWindow->action("format_font_family"),
            mainWindow->action("format_font_size")
        ),
        1,
        0,
        1,
        4
    );

    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_boldface")), 0, 0);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_italics")), 0, 1);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_underline")), 0, 2);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_subscript")), 0, 3);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_superscript")), 0, 4);

    EQt::ToolButton* formatFontButton = new EQt::ToolButton(mainWindow->action("format_font_dialog"));
    formatFontButton->setToolTip(tr("Format Font"));
    fontLayout->addWidget(formatFontButton, 1, 4);

    /* Elements */

    QGridLayout* elementsLayout = newGridLayout("horizontal_elements");
    mainLayout->addWidget(newGroupBox(tr("Elements"), elementsLayout));

    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_table_dialog")), 0, 0);
    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_graph_dialog")), 0, 1);
    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_image_dialog")), 1, 1);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void FormatDockWidget::buildVerticalOrientation(EQt::ProgrammaticMainWindow* mainWindow) {
    QVBoxLayout* mainLayout = newVBoxLayout("vertical");
    setVerticalLayout(mainLayout);

    /* Page and margins format */

    QHBoxLayout* pageAndMarginsLayout = newHBoxLayout("vertical_page_and_margins");
    mainLayout->addLayout(pageAndMarginsLayout);

    QGridLayout* pageLayout = newGridLayout("vertical_page");
    pageAndMarginsLayout->addWidget(newGroupBox(tr("Page"), pageLayout));

    verticalPageOrientationButton = new EQt::ToolButton(pageOrientationAction);
    pageLayout->addWidget(verticalPageOrientationButton, 0, 0);

    EQt::ToolButton* paperSizeButton = new EQt::ToolButton(paperSizePopupMenu);
    paperSizeButton->setIcon(Application::icon("format_page_size"));
    paperSizeButton->setToolTip(tr("Page Size"));
    pageLayout->addWidget(paperSizeButton, 0, 1);

    EQt::ToolButton* formatPageButton = new EQt::ToolButton(mainWindow->action("format_page_dialog"));
    formatPageButton->setToolTip(tr("Format Page"));
    pageLayout->addWidget(formatPageButton, 1, 1);

    /* Margins */

    QFormLayout* marginLayout = newFormLayout("vertical_margins");
    pageAndMarginsLayout->addWidget(newGroupBox(tr("Margins"), marginLayout));

    EQt::DimensionLineEdit* leftLineEdit   = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* rightLineEdit  = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* topLineEdit    = new EQt::DimensionLineEdit;
    EQt::DimensionLineEdit* bottomLineEdit = new EQt::DimensionLineEdit;

    leftLineEdit->setDefaultAction(mainWindow->action("format_left_margin"));
    rightLineEdit->setDefaultAction(mainWindow->action("format_right_margin"));
    topLineEdit->setDefaultAction(mainWindow->action("format_top_margin"));
    bottomLineEdit->setDefaultAction(mainWindow->action("format_bottom_margin"));

    leftLineEdit->setToolTip(tr("Left Margin"));
    rightLineEdit->setToolTip(tr("Right Margin"));
    topLineEdit->setToolTip(tr("Top Margin"));
    bottomLineEdit->setToolTip(tr("Bottom Margin"));

    marginLayout->addRow(tr("Top:"), topLineEdit);
    marginLayout->addRow(tr("Left:"), leftLineEdit);
    marginLayout->addRow(tr("Right:"), rightLineEdit);
    marginLayout->addRow(tr("Bottom:"), bottomLineEdit);

    /* Paragraph format */

    QGridLayout* paragraphLayout = newGridLayout("vertical_paragraph");
    mainLayout->addWidget(newGroupBox(tr("Paragraph"), paragraphLayout));

    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_left_align_text")), 0, 0);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_center_text")), 0, 1);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_right_align_text")), 0, 2);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_justify_text")), 0, 3);

    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_bullets_and_numbering_dialog")), 1, 0);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_paragraph_increase_indent")), 1, 1);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_paragraph_decrease_indent")), 1, 2);
    paragraphLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_paragraph_dialog")), 1, 3);

    /* Font */

    QGridLayout* fontLayout = newGridLayout("vertical_font");
    mainLayout->addWidget(newGroupBox(tr("Font"), fontLayout));

    fontLayout->addWidget(
        new EQt::FontSelectorWidget(
            mainWindow->action("format_font_family"),
            mainWindow->action("format_font_size")
        ),
        1,
        0,
        1,
        4
    );

    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_boldface")), 0, 0);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_italics")), 0, 1);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_underline")), 0, 2);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_subscript")), 0, 3);
    fontLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_font_superscript")), 0, 4);

    EQt::ToolButton* formatFontButton = new EQt::ToolButton(mainWindow->action("format_font_dialog"));
    formatFontButton->setToolTip(tr("Format Font"));
    fontLayout->addWidget(formatFontButton, 1, 4);

    /* Elements */

    QGridLayout* elementsLayout = newGridLayout("vertical_elements");
    mainLayout->addWidget(newGroupBox(tr("Elements"), elementsLayout));

    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_table_dialog")), 0, 0);
    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_graph_dialog")), 0, 1);
    elementsLayout->addWidget(new EQt::ToolButton(mainWindow->action("format_image_dialog")), 0, 2);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void FormatDockWidget::pageOrientationNowLandscape(bool nowLandscape) {
    if (nowLandscape) {
        pageOrientationAction->setIcon(Application::icon("format_page_orientation_landscape"));
        pageOrientationAction->setToolTip("Set portrait page orientation");
    } else {
        pageOrientationAction->setIcon(Application::icon("format_page_orientation_portrait"));
        pageOrientationAction->setToolTip("Set landscape page orientation");
    }
}
