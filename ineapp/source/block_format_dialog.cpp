/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BlockFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QLocale>
#include <QDialog>
#include <QSet>
#include <QSharedPointer>
#include <QLocale>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QSizePolicy>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QToolButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QFont>
#include <QFontMetrics>
#include <QDialogButtonBox>

#include <limits>

#include <util_units.h>
#include <util_page_size.h>

#include <eqt_dimension_line_edit.h>
#include <eqt_paragraph_dimension_widget.h>
#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_block_format.h>
#include <ld_justified_block_format.h>
#include <ld_text_block_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_page_format.h>

#include "application.h"
#include "view_widget.h"
#include "unicode_marshaller.h"
#include "block_format_dialog.h"

const unsigned BlockFormatDialog::dialogMinimumWidth      = 900;
const unsigned BlockFormatDialog::alignmentToolButtonSize = 128;
const float    BlockFormatDialog::invalidMarginSetting    = std::numeric_limits<float>::lowest();

BlockFormatDialog::BlockFormatDialog(QWidget* parent):ProgrammaticDialog("BlockFormatDialog", parent) {
    configureWidget();
    runBuilders();
}


BlockFormatDialog::~BlockFormatDialog() {}


void BlockFormatDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    paragraphDimensionWidget->firstLineIndentationLineEdit()->loadSettings(
        settings,
        "block_first_line_indentation_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    paragraphDimensionWidget->leftIndentationLineEdit()->loadSettings(
        settings,
        "block_left_indentation_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    paragraphDimensionWidget->listIndentationLineEdit()->loadSettings(
        settings,
        "block_list_indentation_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    paragraphDimensionWidget->rightIndentationLineEdit()->loadSettings(
        settings,
        "block_right_indentation_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    paragraphDimensionWidget->topSpacingLineEdit()->loadSettings(
        settings,
        "block_top_spacing_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    paragraphDimensionWidget->bottomSpacingLineEdit()->loadSettings(
        settings,
        "block_bottom_spacing_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void BlockFormatDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    paragraphDimensionWidget->firstLineIndentationLineEdit()->saveSettings(
        settings,
        "block_first_line_indentation_line_edit"
    );

    paragraphDimensionWidget->leftIndentationLineEdit()->saveSettings(
        settings,
        "block_left_indentation_line_edit"
    );

    paragraphDimensionWidget->listIndentationLineEdit()->saveSettings(
        settings,
        "block_list_indentation_line_edit"
    );

    paragraphDimensionWidget->rightIndentationLineEdit()->saveSettings(
        settings,
        "block_right_indentation_line_edit"
    );

    paragraphDimensionWidget->topSpacingLineEdit()->saveSettings(
        settings,
        "block_top_spacing_line_edit"
    );

    paragraphDimensionWidget->bottomSpacingLineEdit()->saveSettings(
        settings,
        "block_bottom_spacing_line_edit"
    );

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void BlockFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void BlockFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool BlockFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool BlockFormatDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


void BlockFormatDialog::setDefaultPageFormat(QSharedPointer<Ld::PageFormat> newDefaultPageFormat) {
    inputPageFormat = newDefaultPageFormat;

    if (isVisible()) {
        populate();
    }
}


QSharedPointer<Ld::PageFormat> BlockFormatDialog::defaultPageFormat() const {
    return inputPageFormat;
}


bool BlockFormatDialog::listSettingsVisible() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->listControlsShown();
}


bool BlockFormatDialog::listSettingsHidden() const {
    return !listSettingsVisible();
}


bool BlockFormatDialog::listSettingsEnabled() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->listIndentationLineEdit()->isEnabled();
}


bool BlockFormatDialog::listSettingsDisabled() const {
    return !listSettingsEnabled();
}


bool BlockFormatDialog::bulletNumberVisible() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->bulletShown();
}


bool BlockFormatDialog::bulletNumberHidden() const {
    return !bulletNumberVisible();
}


Ld::JustifiedBlockFormat::Justification BlockFormatDialog::justification() const {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("block_left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("block_center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("block_right_alignment_tool_button");
    QToolButton* justifyToolButton         = widget<QToolButton>("block_justify_tool_button");

    bool leftAlignmentChecked = leftAlignmentToolButton->isChecked();
    bool centerAlignmenChecked = centerAlignmentToolButton->isChecked();
    bool rightAlignmentChecked = rightAlignmentToolButton->isChecked();
    bool justifyChecked        = justifyToolButton->isChecked();

    Ld::JustifiedBlockFormat::Justification justificationMode;

    if (leftAlignmentChecked) {
        if (centerAlignmenChecked || rightAlignmentChecked || justifyChecked) {
            justificationMode = Ld::JustifiedBlockFormat::Justification::NONE;
        } else {
            justificationMode = Ld::JustifiedBlockFormat::Justification::LEFT;
        }
    } else if (centerAlignmenChecked) {
        if (rightAlignmentChecked || justifyChecked) {
            justificationMode = Ld::JustifiedBlockFormat::Justification::NONE;
        } else {
            justificationMode = Ld::JustifiedBlockFormat::Justification::CENTER;
        }
    } else if (rightAlignmentChecked) {
        if (justifyChecked) {
            justificationMode = Ld::JustifiedBlockFormat::Justification::NONE;
        } else {
            justificationMode = Ld::JustifiedBlockFormat::Justification::RIGHT;
        }
    } else if (justifyChecked) {
        justificationMode = Ld::JustifiedBlockFormat::Justification::JUSTIFY;
    } else {
        justificationMode = Ld::JustifiedBlockFormat::Justification::NONE;
    }

    return justificationMode;
}


float BlockFormatDialog::leftIndentation() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->leftIndentation();
}


float BlockFormatDialog::listIndentation() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->listIndentation();
}


float BlockFormatDialog::rightIndentation() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->rightIndentation();
}


float BlockFormatDialog::topSpacing() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->topSpacing();
}


float BlockFormatDialog::bottomSpacing() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->bottomSpacing();
}


float BlockFormatDialog::firstLineIndentation() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->firstLineIndentation();
}


float BlockFormatDialog::lineSpacing() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->lineSpacing();
}


bool BlockFormatDialog::isLeftIndentationCleared() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isLeftIndentationClear();
}


bool BlockFormatDialog::isListIndentationCleared() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isListIndentationClear();
}


bool BlockFormatDialog::isRightIndentationCleared() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isRightIndentationClear();
}


bool BlockFormatDialog::isTopSpacingCleared() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isTopSpacingClear();
}


bool BlockFormatDialog::isBottomSpacingCleared() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isBottomSpacingClear();
}


bool BlockFormatDialog::isFirstLineIndentationCleared() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isFirstLineIndentationClear();
}


bool BlockFormatDialog::isLineSpacingCleared() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isLineSpacingClear();
}


bool BlockFormatDialog::isLeftIndentationSet() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isLeftIndentationSet();
}


bool BlockFormatDialog::isListIndentationSet() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isListIndentationSet();
}


bool BlockFormatDialog::isRightIndentationSet() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isRightIndentationSet();
}


bool BlockFormatDialog::isTopSpacingSet() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isTopSpacingSet();
}


bool BlockFormatDialog::isBottomSpacingSet() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isBottomSpacingSet();
}


bool BlockFormatDialog::isFirstLineIndentationSet() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isFirstLineIndentationSet();
}


bool BlockFormatDialog::isLineSpacingSet() const {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    return paragraphDimensionWidget->isLineSpacingSet();
}


void BlockFormatDialog::populate() {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    paragraphDimensionWidget->setPageWidth(inputPageFormat->horizontalExtents());
    paragraphDimensionWidget->setPageLeftMargin(inputPageFormat->leftMargin());
    paragraphDimensionWidget->setPageRightMargin(inputPageFormat->rightMargin());

    QLabel* notesLabel = widget<QLabel>("block_notes_label");

    bool    useMetric       = (Application::locale()->measurementSystem() == QLocale::MeasurementSystem::MetricSystem);
    float   scaleFactor     = useMetric ? Util::Units::mmPerPoint : Util::Units::inchesPerPoint;
    QString unitString      = useMetric ? tr(" mm") : tr("\"");
    QString pageDescription = inputPageFormat->pageSize().description(useMetric);
    float   pageLeftMargin  = scaleFactor * inputPageFormat->leftMargin();
    float   pageRightMargin = scaleFactor * inputPageFormat->rightMargin();

    notesLabel->setTextFormat(Qt::RichText);
    notesLabel->setWordWrap(true);
    notesLabel->setText(
        tr(
           "Notes:\n"
           "<ul>"
           "<li>Page dimensions are based on %1 paper size.</lt>"
           "<li>Page left and right margins are %2%3 and %4%5 respectively.</lt>"
           "<li>Block top and bottom spacing will take the adjacent paragraph spacing values into account, using "
           "the maximum of the two spacing values.</lt>"
           "</ul>"
          ).arg(pageDescription)
           .arg(pageLeftMargin)
           .arg(unitString)
           .arg(pageRightMargin)
           .arg(unitString)
    );

    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("block_left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("block_center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("block_right_alignment_tool_button");
    QToolButton* justifyToolButton         = widget<QToolButton>("block_justify_tool_button");

    leftAlignmentToolButton->setChecked(
        inputJustifications.contains(Ld::JustifiedBlockFormat::Justification::LEFT)
    );

    centerAlignmentToolButton->setChecked(
        inputJustifications.contains(Ld::JustifiedBlockFormat::Justification::CENTER)
    );

    rightAlignmentToolButton->setChecked(
        inputJustifications.contains(Ld::JustifiedBlockFormat::Justification::RIGHT)
    );

    justifyToolButton->setChecked(
        inputJustifications.contains(Ld::JustifiedBlockFormat::Justification::JUSTIFY)
    );

    if (inputLeftIndentation == invalidMarginSetting) {
        paragraphDimensionWidget->clearLeftIndentation();
    } else {
        paragraphDimensionWidget->setLeftIndentation(inputLeftIndentation);
    }

    if (inputListIndentation == invalidMarginSetting) {
        paragraphDimensionWidget->clearListIndentation();
    } else {
        paragraphDimensionWidget->setListIndentation(inputListIndentation);
    }

    if (inputRightIndentation == invalidMarginSetting) {
        paragraphDimensionWidget->clearRightIndentation();
    } else {
        paragraphDimensionWidget->setRightIndentation(inputRightIndentation);
    }

    if (inputTopSpacing == invalidMarginSetting) {
        paragraphDimensionWidget->clearTopSpacing();
    } else {
        paragraphDimensionWidget->setTopSpacing(inputTopSpacing);
    }

    if (inputBottomSpacing == invalidMarginSetting) {
        paragraphDimensionWidget->clearBottomSpacing();
    } else {
        paragraphDimensionWidget->setBottomSpacing(inputBottomSpacing);
    }

    // FIXME
    if (inputFirstLineIndentation == invalidMarginSetting) {
        paragraphDimensionWidget->clearFirstLineIndentation();
    } else {
        paragraphDimensionWidget->setFirstLineIndentation(inputFirstLineIndentation);
    }

    if (inputLineSpacing == invalidMarginSetting) {
        paragraphDimensionWidget->clearLineSpacing();
    } else {
        paragraphDimensionWidget->setLineSpacing(inputLineSpacing);
    }

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(false);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif
}


void BlockFormatDialog::setListSettingsVisible(bool nowVisible) {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    paragraphDimensionWidget->setListControlsShown(nowVisible);
}


void BlockFormatDialog::setListSettingsHidden(bool nowHidden) {
    setListSettingsVisible(!nowHidden);
}


void BlockFormatDialog::setListSettingsEnabled(bool nowEnabled) {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    paragraphDimensionWidget->listIndentationLineEdit()->setEnabled(nowEnabled);
}


void BlockFormatDialog::setListSettingsDisabled(bool nowDisabled) {
    setListSettingsEnabled(!nowDisabled);
}


void BlockFormatDialog::setBulletNumberVisible(bool nowVisible) {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    paragraphDimensionWidget->setBulletVisible(nowVisible);
}


void BlockFormatDialog::setBulletNumberHidden(bool nowHidden) {
    setBulletNumberVisible(!nowHidden);
}


void BlockFormatDialog::setJustification(Ld::JustifiedBlockFormat::Justification newJustification) {
    inputJustifications.clear();
    inputJustifications << newJustification;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setJustification(const QSet<Ld::JustifiedBlockFormat::Justification>& newJustification) {
    inputJustifications = newJustification;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setLeftJustified() {
    setJustification(Ld::JustifiedBlockFormat::Justification::LEFT);
}


void BlockFormatDialog::setCentered() {
    setJustification(Ld::JustifiedBlockFormat::Justification::CENTER);
}


void BlockFormatDialog::setRightJustified() {
    setJustification(Ld::JustifiedBlockFormat::Justification::RIGHT);
}


void BlockFormatDialog::setJustified() {
    setJustification(Ld::JustifiedBlockFormat::Justification::JUSTIFY);
}


void BlockFormatDialog::setLeftIndentation(float newLeftIndentation) {
    inputLeftIndentation = newLeftIndentation;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setListIndentation(float newListIndentation) {
    inputListIndentation = newListIndentation;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setRightIndentation(float newRightIndentation) {
    inputRightIndentation = newRightIndentation;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setTopSpacing(float newTopSpacing) {
    inputTopSpacing = newTopSpacing;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setBottomSpacing(float newBottomSpacing) {
    inputBottomSpacing = newBottomSpacing;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setFirstLineIndentation(float newFirstLineIndentation) {
    inputFirstLineIndentation = newFirstLineIndentation;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::setLineSpacing(float newLineSpacing) {
    inputLineSpacing = newLineSpacing;

    if (isVisible()) {
        populate();
    }
}


void BlockFormatDialog::clearLeftIndentation() {
    setLeftIndentation(invalidMarginSetting);
}


void BlockFormatDialog::clearListIndentation() {
    setListIndentation(invalidMarginSetting);
}


void BlockFormatDialog::clearRightIndentation() {
    setRightIndentation(invalidMarginSetting);
}


void BlockFormatDialog::clearTopSpacing() {
    setTopSpacing(invalidMarginSetting);
}


void BlockFormatDialog::clearBottomSpacing() {
    setBottomSpacing(invalidMarginSetting);
}


void BlockFormatDialog::clearFirstLineIndentation() {
    setFirstLineIndentation(invalidMarginSetting);
}


void BlockFormatDialog::clearLineSpacing() {
    setLineSpacing(invalidMarginSetting);
}


void BlockFormatDialog::bind() {
    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");

    connect(
        paragraphDimensionWidget,
        &EQt::ParagraphDimensionWidget::valueChanged,
        this,
        &BlockFormatDialog::updateDialog
    );

    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("block_left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("block_center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("block_right_alignment_tool_button");
    QToolButton* justifyToolButton         = widget<QToolButton>("block_justify_tool_button");

    connect(leftAlignmentToolButton, &QToolButton::clicked, this, &BlockFormatDialog::leftAlignedButtonClicked);
    connect(centerAlignmentToolButton, &QToolButton::clicked, this, &BlockFormatDialog::centeredButtonClicked);
    connect(rightAlignmentToolButton, &QToolButton::clicked, this, &BlockFormatDialog::rightAlignedButtonClicked);
    connect(justifyToolButton, &QToolButton::clicked, this, &BlockFormatDialog::justifiedButtonClicked);

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &BlockFormatDialog::accept);

    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
    connect(cancelButton, &QPushButton::clicked, this, &BlockFormatDialog::reject);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        connect(applyButton, &QPushButton::clicked, this, &BlockFormatDialog::applyClicked);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    ProgrammaticDialog::bind();
}


void BlockFormatDialog::leftAlignedButtonClicked() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("block_left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("block_center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("block_right_alignment_tool_button");
    QToolButton* justifyToolButton         = widget<QToolButton>("block_justify_tool_button");

    leftAlignmentToolButton->setChecked(true);
    centerAlignmentToolButton->setChecked(false);
    rightAlignmentToolButton->setChecked(false);
    justifyToolButton->setChecked(false);

    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");
    paragraphDimensionWidget->setLeftJustified();

    updateDialog();
}


void BlockFormatDialog::centeredButtonClicked() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("block_left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("block_center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("block_right_alignment_tool_button");
    QToolButton* justifyToolButton         = widget<QToolButton>("block_justify_tool_button");

    leftAlignmentToolButton->setChecked(false);
    centerAlignmentToolButton->setChecked(true);
    rightAlignmentToolButton->setChecked(false);
    justifyToolButton->setChecked(false);

    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");
    paragraphDimensionWidget->setCentered();

    updateDialog();
}


void BlockFormatDialog::rightAlignedButtonClicked() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("block_left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("block_center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("block_right_alignment_tool_button");
    QToolButton* justifyToolButton         = widget<QToolButton>("block_justify_tool_button");

    leftAlignmentToolButton->setChecked(false);
    centerAlignmentToolButton->setChecked(false);
    rightAlignmentToolButton->setChecked(true);
    justifyToolButton->setChecked(false);

    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");
    paragraphDimensionWidget->setRightJustified();

    updateDialog();
}


void BlockFormatDialog::justifiedButtonClicked() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("block_left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("block_center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("block_right_alignment_tool_button");
    QToolButton* justifyToolButton         = widget<QToolButton>("block_justify_tool_button");

    leftAlignmentToolButton->setChecked(false);
    centerAlignmentToolButton->setChecked(false);
    rightAlignmentToolButton->setChecked(false);
    justifyToolButton->setChecked(true);

    EQt::ParagraphDimensionWidget* paragraphDimensionWidget =
                         EQt::ProgrammaticWidget::widget<EQt::ParagraphDimensionWidget>("paragraph_dimension_widget");
    paragraphDimensionWidget->setJustified();

    updateDialog();
}


void BlockFormatDialog::applyClicked() {
    if (justification() != Ld::JustifiedBlockFormat::Justification::NONE) {
        inputJustifications.clear();
        inputJustifications << justification();
    }

    if (isLeftIndentationSet()) {
        inputLeftIndentation = leftIndentation();
    }

    if (isListIndentationSet()) {
        inputListIndentation = listIndentation();
    }

    if (isRightIndentationSet()) {
        inputRightIndentation = rightIndentation();
    }

    if (isTopSpacingSet()) {
        inputTopSpacing = topSpacing();
    }

    if (isBottomSpacingSet()) {
        inputBottomSpacing = bottomSpacing();
    }

    if (isFirstLineIndentationSet()) {
        inputFirstLineIndentation = firstLineIndentation();
    }

    if (isLineSpacingSet()) {
        inputLineSpacing = lineSpacing();
    }

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(false);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    emit apply(this);
}


void BlockFormatDialog::updateDialog() {
    bool valuesHaveChanged = valuesChanged();

    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    okPushButton->setEnabled(valuesHaveChanged);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(valuesHaveChanged);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    if (currentAutoApplyEnabled) {
        applyClicked();
    }
}


void BlockFormatDialog::configureWidget() {
    setWindowTitle(tr("Block Format"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
    setMinimumWidth(dialogMinimumWidth);

    currentAutoApplyEnabled   = false;

    inputPageFormat           = Ld::PageFormat::defaultPageFormat.clone().dynamicCast<Ld::PageFormat>();
    inputLeftIndentation      = invalidMarginSetting;
    inputListIndentation      = invalidMarginSetting;
    inputRightIndentation     = invalidMarginSetting;
    inputTopSpacing           = invalidMarginSetting;
    inputBottomSpacing        = invalidMarginSetting;
    inputFirstLineIndentation = invalidMarginSetting;
    inputLineSpacing          = invalidMarginSetting;

    inputJustifications.clear();

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* horizontalLayout = newHBoxLayout("horizontal_layout");
    mainLayout->addLayout(horizontalLayout);

    QGroupBox* blockAlignmentGroupBox = new QGroupBox(tr("Alignment"));
    registerWidget(blockAlignmentGroupBox, "block_alignment_group_box");
    horizontalLayout->addWidget(blockAlignmentGroupBox);

    QVBoxLayout* labelVerticalLayout = newVBoxLayout("label_vertical_layout");
    horizontalLayout->addLayout(labelVerticalLayout);

    QLabel* notesLabel = new QLabel;
    registerWidget(notesLabel, "block_notes_label");
    labelVerticalLayout->addWidget(notesLabel);
    labelVerticalLayout->addStretch();

    QHBoxLayout* blockAlignmentLayout = newHBoxLayout("alignment_layout");
    blockAlignmentGroupBox->setLayout(blockAlignmentLayout);

    QIcon blockLeftAlignmentIcon(":format_block_left_align_active_128.png");
    QToolButton* blockLeftAlignmentToolButton = new QToolButton(this);
    blockLeftAlignmentToolButton->setIcon(blockLeftAlignmentIcon);
    blockLeftAlignmentToolButton->setIconSize(QSize(alignmentToolButtonSize, alignmentToolButtonSize));
    blockAlignmentLayout->addWidget(blockLeftAlignmentToolButton);
    registerWidget(blockLeftAlignmentToolButton, "block_left_alignment_tool_button");

    QIcon blockCenterAlignmentIcon(":format_block_center_active_128.png");
    QToolButton* blockCenterAlignmentToolButton = new QToolButton(this);
    blockCenterAlignmentToolButton->setIcon(blockCenterAlignmentIcon);
    blockCenterAlignmentToolButton->setIconSize(QSize(alignmentToolButtonSize, alignmentToolButtonSize));
    blockAlignmentLayout->addWidget(blockCenterAlignmentToolButton);
    registerWidget(blockCenterAlignmentToolButton, "block_center_alignment_tool_button");

    QIcon blockRightAlignmentIcon(":format_block_right_align_active_128.png");
    QToolButton* blockRightAlignmentToolButton = new QToolButton(this);
    blockRightAlignmentToolButton->setIcon(blockRightAlignmentIcon);
    blockRightAlignmentToolButton->setIconSize(QSize(alignmentToolButtonSize, alignmentToolButtonSize));
    blockAlignmentLayout->addWidget(blockRightAlignmentToolButton);
    registerWidget(blockRightAlignmentToolButton, "block_right_alignment_tool_button");

    QIcon blockJustifiedAlignmentIcon(":format_block_justified_active_128.png");
    QToolButton* blockJustifiedAlignmentToolButton = new QToolButton(this);
    blockJustifiedAlignmentToolButton->setIcon(blockJustifiedAlignmentIcon);
    blockJustifiedAlignmentToolButton->setIconSize(QSize(alignmentToolButtonSize, alignmentToolButtonSize));
    blockAlignmentLayout->addWidget(blockJustifiedAlignmentToolButton);
    registerWidget(blockJustifiedAlignmentToolButton, "block_justify_tool_button");

    blockLeftAlignmentToolButton->setCheckable(true);
    blockCenterAlignmentToolButton->setCheckable(true);
    blockRightAlignmentToolButton->setCheckable(true);
    blockJustifiedAlignmentToolButton->setCheckable(true);

    blockLeftAlignmentToolButton->setToolTip(tr("Click this button to left align text"));
    blockLeftAlignmentToolButton->setWhatsThis(
        tr(
           "You can use this button to left align the contents of one or more block, taking into account the "
           "block's left indentation settings."
          )
    );

    blockCenterAlignmentToolButton->setToolTip(tr("Click this button to center text"));
    blockCenterAlignmentToolButton->setWhatsThis(
        tr(
           "You can use this button to center the contents of one or blocks, taking into account the block's "
           "indentation settings."
          )
        );

    blockRightAlignmentToolButton->setToolTip(tr("Click this button to right align text"));
    blockRightAlignmentToolButton->setWhatsThis(
        tr(
           "You can use this button to right align the contents of one or more blocks, taking into acocunt the "
           "block's right indentation setting."
          )
        );

    blockJustifiedAlignmentToolButton->setToolTip(tr("Click this button to justify text between margins"));
    blockJustifiedAlignmentToolButton->setWhatsThis(
        tr(
           "You can use this butotn to justify the contents of one or more blocks, taking into account the block "
           "indentation settings."
          )
    );

    EQt::ParagraphDimensionWidget* paragraphDimensionWidget = new EQt::ParagraphDimensionWidget;
    mainLayout->addWidget(paragraphDimensionWidget);
    registerWidget(paragraphDimensionWidget, "paragraph_dimension_widget");

    paragraphDimensionWidget->setMaximumHeight(1000);
    paragraphDimensionWidget->firstLineIndentationLineEdit()->setDynamicUnits(true);
    paragraphDimensionWidget->leftIndentationLineEdit()->setDynamicUnits(true);
    paragraphDimensionWidget->listIndentationLineEdit()->setDynamicUnits(true);
    paragraphDimensionWidget->rightIndentationLineEdit()->setDynamicUnits(true);
    paragraphDimensionWidget->topSpacingLineEdit()->setDynamicUnits(true);
    paragraphDimensionWidget->bottomSpacingLineEdit()->setDynamicUnits(true);

    paragraphDimensionWidget->firstLineIndentationLineEdit()->setToolTip(
        tr("Enter the additional indentation to be applied to the first line of the paragraph.")
    );
    paragraphDimensionWidget->firstLineIndentationLineEdit()->setWhatsThis(
        tr(
           "You can use this field to enter additional indentation to be applied to the first line of text in the "
           "paragraph.  A negative value can be used to create a hanging indent.  Values can be entered in inches"
           "(\"), points (pt), picas (pc), centimeters (cm), or millimeters (mm)."
         )
    );

    paragraphDimensionWidget->leftIndentationLineEdit()->setToolTip(
        tr("Enter the indentation to be applied to the left side of the paragraph.")
    );
    paragraphDimensionWidget->leftIndentationLineEdit()->setWhatsThis(
        tr(
           "You can use this field to enter the indentation to be applied to the left side of the paragraph.  When "
           "the paragraph is part of a list, this indentation value will indicate the location of the bullet or "
           "number.  Values can be entered in inches (\"), points (pt), picas (pc), centimeters (cm), or millimeters "
           "(mm)."
         )
    );

    paragraphDimensionWidget->listIndentationLineEdit()->setToolTip(
        tr(
           "Enter the additional indentation to be applied between the left edge of the list bullet or number and the "
           "paragraph text."
          )
    );
    paragraphDimensionWidget->listIndentationLineEdit()->setWhatsThis(
        tr(
           "You can use this field to enter the additional indentation to be applied between the left edge of the "
           "list bullet or number and the paragraph text.  Values can be entered in inches (\"), points (pt), picas "
           "(pc), centimeters (cm), or millimeters (mm)."
         )
    );

    paragraphDimensionWidget->rightIndentationLineEdit()->setToolTip(
        tr("Enter the additional indentation to be applied between the right edge of paragraph and the page margin.")
    );
    paragraphDimensionWidget->rightIndentationLineEdit()->setWhatsThis(
        tr(
           "You can use this field to enter indentation to be applied between the right edge of the paragraph and the"
           "page margin.  Values can be entered in inches (\"), points (pt), picas (pc), centimeters (cm), or "
           "millimeters (mm)."
         )
    );

    paragraphDimensionWidget->topSpacingLineEdit()->setToolTip(
        tr("Enter the spacing to be applied between this paragraph and the previous paragraph.")
    );
    paragraphDimensionWidget->topSpacingLineEdit()->setWhatsThis(
        tr(
           "You can use this field to enter spacing to be applied between this paragraph and the previous paragraph.  "
           "Actual spacing will be the maximum of this paragraph's top spacing and the previous paragraph's bottom "
           "spacing.  Values can be entered in inches (\"), points (pt), picas (pc), centimeters (cm), or "
           "millimeters (mm)."
         )
    );

    paragraphDimensionWidget->bottomSpacingLineEdit()->setToolTip(
        tr("Enter the spacing to be applied between this paragraph and the next paragraph.")
    );
    paragraphDimensionWidget->bottomSpacingLineEdit()->setWhatsThis(
        tr(
           "You can use this field to enter spacing to be applied between this paragraph and the next paragraph.  "
           "Actual spacing will be the maximum of this paragraph's bottom spacing and the next paragraph's top "
           "spacing.  Values can be entered in inches (\"), points (pt), picas (pc), centimeters (cm), or "
           "millimeters (mm)."
         )
    );

    paragraphDimensionWidget->lineSpacingComboBox()->setToolTip(
        tr("Select the desired spacing between lines of text in the paragraph.")
    );
    paragraphDimensionWidget->lineSpacingComboBox()->setWhatsThis(
        tr("You can use this combo box to select your desired paragraph line spacing.")
    );

    #if (defined(Q_OS_DARWIN))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    #elif (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
        );

    #else

        #error Unknown platform

    #endif

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
        registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    mainLayout->addWidget(buttonBox);
}


bool BlockFormatDialog::valuesChanged() const {
    Ld::JustifiedBlockFormat::Justification justificationMode = justification();
    bool valueHasChanged = (
           (   inputJustifications.count() != 1
            && justificationMode != Ld::JustifiedBlockFormat::Justification::NONE
           )
        || (   inputJustifications.count() == 1
            && justificationMode != *inputJustifications.begin()
           )
        || (   inputLeftIndentation == invalidMarginSetting
            && isLeftIndentationSet()
           )
        || (   inputLeftIndentation != invalidMarginSetting
            && Util::Units::pointValuesNotEqual(inputLeftIndentation, leftIndentation())
           )
        || (   inputListIndentation == invalidMarginSetting
            && isListIndentationSet()
           )
        || (   inputListIndentation != invalidMarginSetting
            && Util::Units::pointValuesNotEqual(inputListIndentation, listIndentation())
           )
        || (   inputRightIndentation == invalidMarginSetting
            && isRightIndentationSet()
           )
        || (   inputRightIndentation != invalidMarginSetting
            && Util::Units::pointValuesNotEqual(inputRightIndentation, rightIndentation())
           )
        || (   inputTopSpacing == invalidMarginSetting
            && isTopSpacingSet()
           )
        || (   inputTopSpacing != invalidMarginSetting
            && Util::Units::pointValuesNotEqual(inputTopSpacing, topSpacing())
           )
        || (   inputBottomSpacing == invalidMarginSetting
            && isBottomSpacingSet()
           )
        || (   inputBottomSpacing != invalidMarginSetting
            && Util::Units::pointValuesNotEqual(inputBottomSpacing, bottomSpacing())
           )
        || (   inputFirstLineIndentation == invalidMarginSetting
            && isFirstLineIndentationSet()
           )
        || (   inputFirstLineIndentation >=0
            && Util::Units::pointValuesNotEqual(inputFirstLineIndentation, firstLineIndentation())
           )
        || (   inputLineSpacing == invalidMarginSetting
            && isLineSpacingSet()
           )
        || (   inputLineSpacing != invalidMarginSetting
            && inputLineSpacing != lineSpacing()
           )
    );

    return valueHasChanged;
}
