/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QLocale>
#include <QDialog>
#include <QSize>
#include <QSet>
#include <QChar>
#include <QSpinBox>
#include <QIcon>
#include <QRadioButton>
#include <QLabel>
#include <QStringList>
#include <QString>
#include <QCheckBox>
#include <QToolButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QPushButton>
#include <QDialogButtonBox>

#include <cstdint>
#include <algorithm>
#include <cmath>

#include <util_units.h>

#include <eqt_font_data.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_line_sample_widget.h>
#include <eqt_color_tool_button.h>
#include <eqt_color_dialog.h>
#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include "application.h"
#include "view_widget.h"
#include "line_settings_widget.h"
#include "table_column_widths_widget.h"
#include "character_format_helpers.h"
#include "table_format_dialog.h"

typedef TableFormatDialog::Justification Justification;
typedef TableFormatDialog::LineStyle     LineStyle;

const Justification TableFormatDialog::defaultJustification         = Justification::CENTER;
const float         TableFormatDialog::defaultLeftIndentation       = 0.0F;
const bool          TableFormatDialog::defaultLeftIndentationValid  = true;
const float         TableFormatDialog::defaultRightIndentation      = 0.0F;
const bool          TableFormatDialog::defaultRightIndentationValid = true;
const float         TableFormatDialog::defaultTopSpacing            = 6.0F;
const bool          TableFormatDialog::defaultTopSpacingValid       = true;
const float         TableFormatDialog::defaultBottomSpacing         = 6.0F;
const bool          TableFormatDialog::defaultBottomSpacingValid    = true;
const float         TableFormatDialog::defaultTopGutter             = 2.5F;
const bool          TableFormatDialog::defaultTopGutterValid        = true;
const float         TableFormatDialog::defaultLeftGutter            = 2.5F;
const bool          TableFormatDialog::defaultLeftGutterValid       = true;
const float         TableFormatDialog::defaultRightGutter           = 2.5F;
const bool          TableFormatDialog::defaultRightGutterValid      = true;
const float         TableFormatDialog::defaultBottomGutter          = 2.5F;
const bool          TableFormatDialog::defaultBottomGutterValid     = true;
const unsigned      TableFormatDialog::defaultNumberRows            = 2;
const unsigned      TableFormatDialog::defaultNumberColumns         = 2;
const LineStyle     TableFormatDialog::defaultVerticalLineStyle     = LineStyle::SINGLE;
const float         TableFormatDialog::defaultVerticalLineWidth     = 1.0F;
const LineStyle     TableFormatDialog::defaultHorizontalLineStyle   = LineStyle::SINGLE;
const float         TableFormatDialog::defaultHorizontalLineWidth   = 1.0F;
const QColor        TableFormatDialog::defaultCellBackgroundColor   = QColor();


TableFormatDialog::TableFormatDialog(QWidget* parent):ProgrammaticDialog("TableFormatDialog", parent) {
    configureWidget();
    runBuilders();
}


TableFormatDialog::~TableFormatDialog() {}


void TableFormatDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::DimensionLineEdit* leftIndentationLineEdit  = widget<EQt::DimensionLineEdit>("left_indentation_line_edit");
    EQt::DimensionLineEdit* rightIndentationLineEdit = widget<EQt::DimensionLineEdit>("right_indentation_line_edit");
    EQt::DimensionLineEdit* topSpacingLineEdit       = widget<EQt::DimensionLineEdit>("top_spacing_line_edit");
    EQt::DimensionLineEdit* bottomSpacingLineEdit    = widget<EQt::DimensionLineEdit>("bottom_spacing_line_edit");

    leftIndentationLineEdit->loadSettings(
        settings,
        "left_indentation_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    rightIndentationLineEdit->loadSettings(
        settings,
        "right_indentation_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    topSpacingLineEdit->loadSettings(
        settings,
        "top_spacing_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    bottomSpacingLineEdit->loadSettings(
        settings,
        "bottom_spacing_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    QString unitsValue = settings->value("column_width_units", QString("POINTS")).toString();
    QString unitsComboBoxText;

    if (unitsValue == "POINTS") {
        unitsComboBoxText = tr("points");
    } else if (unitsValue == "PICAS") {
        unitsComboBoxText = tr("picas");
    } else if (unitsValue == "MILLIMETERS") {
        unitsComboBoxText = tr("millimeters");
    } else if (unitsValue == "CENTIMETERS") {
        unitsComboBoxText = tr("centimeters");
    } else if (unitsValue == "INCHES") {
        unitsComboBoxText = tr("inches");
    } else {
        unitsComboBoxText = tr("points");
    }

    QComboBox* unitsComboBox = widget<QComboBox>("units_combo_box");
    unitsComboBox->setCurrentText(unitsComboBoxText);

    EQt::DimensionLineEdit* topGutterLineEdit    = widget<EQt::DimensionLineEdit>("top_gutter_line_edit");
    EQt::DimensionLineEdit* leftGutterLineEdit   = widget<EQt::DimensionLineEdit>("left_gutter_line_edit");
    EQt::DimensionLineEdit* rightGutterLineEdit  = widget<EQt::DimensionLineEdit>("right_gutter_line_edit");
    EQt::DimensionLineEdit* bottomGutterLineEdit = widget<EQt::DimensionLineEdit>("bottom_gutter_line_edit");

    topGutterLineEdit->loadSettings(
        settings,
        "top_gutter_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );
    leftGutterLineEdit->loadSettings(
        settings,
        "left_gutter_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );
    rightGutterLineEdit->loadSettings(
        settings,
        "right_gutter_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );
    bottomGutterLineEdit->loadSettings(
        settings,
        "bottom_gutter_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void TableFormatDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::DimensionLineEdit* leftIndentationLineEdit  = widget<EQt::DimensionLineEdit>("left_indentation_line_edit");
    EQt::DimensionLineEdit* rightIndentationLineEdit = widget<EQt::DimensionLineEdit>("right_indentation_line_edit");
    EQt::DimensionLineEdit* topSpacingLineEdit       = widget<EQt::DimensionLineEdit>("top_spacing_line_edit");
    EQt::DimensionLineEdit* bottomSpacingLineEdit    = widget<EQt::DimensionLineEdit>("bottom_spacing_line_edit");

    leftIndentationLineEdit->saveSettings(settings,"left_indentation_line_edit");
    rightIndentationLineEdit->saveSettings(settings,"right_indentation_line_edit");
    topSpacingLineEdit->saveSettings(settings,"top_spacing_line_edit");
    bottomSpacingLineEdit->saveSettings(settings,"bottom_spacing_line_edit");

    QComboBox* unitsComboBox     = widget<QComboBox>("units_combo_box");
    QString    unitsComboBoxText = unitsComboBox->currentText();
    QString    unitsValue;
    if (unitsComboBoxText == tr("points")) {
        unitsValue = "POINTS";
    } else if (unitsComboBoxText == tr("picas")) {
        unitsValue = "PICAS";
    } else if (unitsComboBoxText == tr("millimeters")) {
        unitsValue = "MILLIMETERS";
    } else if (unitsComboBoxText == tr("centimeters")) {
        unitsValue = "CENTIMETERS";
    } else if (unitsComboBoxText == tr("inches")) {
        unitsValue = "INCHES";
    } else {
        unitsValue = "POINTS";
    }

    settings->setValue("column_width_units", unitsValue);

    EQt::DimensionLineEdit* topGutterLineEdit    = widget<EQt::DimensionLineEdit>("top_gutter_line_edit");
    EQt::DimensionLineEdit* leftGutterLineEdit   = widget<EQt::DimensionLineEdit>("left_gutter_line_edit");
    EQt::DimensionLineEdit* rightGutterLineEdit  = widget<EQt::DimensionLineEdit>("right_gutter_line_edit");
    EQt::DimensionLineEdit* bottomGutterLineEdit = widget<EQt::DimensionLineEdit>("bottom_gutter_line_edit");

    topGutterLineEdit->saveSettings(settings, "top_gutter_line_edit");
    leftGutterLineEdit->saveSettings(settings, "left_gutter_line_edit");
    rightGutterLineEdit->saveSettings(settings, "right_gutter_line_edit");
    bottomGutterLineEdit->saveSettings(settings, "bottom_gutter_line_edit");

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void TableFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void TableFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool TableFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool TableFormatDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


bool TableFormatDialog::applyButtonVisible() const {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    return applyPushButton->isVisible();
}


bool TableFormatDialog::applyButtonHidden() const {
    return !applyButtonVisible();
}


void TableFormatDialog::setApplyButtonVisible(bool nowVisible) {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    applyPushButton->setVisible(nowVisible);
}


void TableFormatDialog::setApplyButtonHidden(bool nowHidden) {
    setApplyButtonVisible(!nowHidden);
}


bool TableFormatDialog::okCancelButtonVisible() const {
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    return okPushButton->isVisible();
}


bool TableFormatDialog::okCancelButtonHidden() const {
    return !okCancelButtonVisible();
}


void TableFormatDialog::setOkCancelButtonVisible(bool nowVisible) {
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    okPushButton->setVisible(nowVisible);
    cancelPushButton->setVisible(nowVisible);
}


void TableFormatDialog::setOkCancelButtonHidden(bool nowHidden) {
    setOkCancelButtonVisible(!nowHidden);
}


bool TableFormatDialog::okButtonAlwaysEnabled() const {
    return currentOkButtonAlwaysEnabled;
}


bool TableFormatDialog::okButtonConditionallyEnabled() const {
    return !currentOkButtonAlwaysEnabled;
}


void TableFormatDialog::setOkButtonAlwaysEnabled(bool nowAlwaysEnabled) {
    currentOkButtonAlwaysEnabled = nowAlwaysEnabled;

    bool         changed      = valuesChanged();
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");

    okPushButton->setEnabled(changed || currentOkButtonAlwaysEnabled);
}


void TableFormatDialog::setOkButtonConditionallyEnabled(bool nowConditionallyEnabled) {
    setOkButtonAlwaysEnabled(!nowConditionallyEnabled);
}


bool TableFormatDialog::advancedModeEnabled() const {
    QToolButton* advancedToolButton = widget<QToolButton>("advanced_tool_button");
    return advancedToolButton->isChecked();
}


bool TableFormatDialog::advancedModeDisabled() const {
    return !advancedModeEnabled();
}


bool TableFormatDialog::advancedModeButtonVisible() const {
    QToolButton* advancedToolButton = widget<QToolButton>("advanced_tool_button");
    return advancedToolButton->isVisible();
}


bool TableFormatDialog::advancedModeButtonHidden() const {
    return !advancedModeButtonVisible();
}


bool TableFormatDialog::tableSizeControlsVisible() const {
    QGroupBox* tableSizeGroupBox = widget<QGroupBox>("table_size_group_box");
    return tableSizeGroupBox->isVisible();
}


bool TableFormatDialog::tableSizeControlsHidden() const {
    return !tableSizeControlsVisible();
}


TableFormatDialog::Justification TableFormatDialog::justification() const {
    Justification result;

    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("right_alignment_tool_button");

    bool leftAlignment   = leftAlignmentToolButton->isChecked();
    bool centerAlignment = centerAlignmentToolButton->isChecked();
    bool rightAlignment  = rightAlignmentToolButton->isChecked();

    if        ( leftAlignment && !centerAlignment && !rightAlignment) {
        result = Justification::LEFT;
    } else if (!leftAlignment &&  centerAlignment && !rightAlignment) {
        result = Justification::CENTER;
    } else if (!leftAlignment && !centerAlignment &&  rightAlignment) {
        result = Justification::RIGHT;
    } else {
        result = Justification::NONE;
    }

    return result;
}


float TableFormatDialog::leftIndentation() const {
    EQt::DimensionLineEdit* leftIndentationLineEdit = widget<EQt::DimensionLineEdit>("left_indentation_line_edit");
    return static_cast<float>(leftIndentationLineEdit->value());
}


bool TableFormatDialog::leftIndentationSet() const {
    EQt::DimensionLineEdit* leftIndentationLineEdit = widget<EQt::DimensionLineEdit>("left_indentation_line_edit");
    return leftIndentationLineEdit->valueOk();
}


bool TableFormatDialog::leftIndentationUnset() const {
    return !leftIndentationSet();
}


float TableFormatDialog::rightIndentation() const {
    EQt::DimensionLineEdit* rightIndentationLineEdit = widget<EQt::DimensionLineEdit>("right_indentation_line_edit");
    return static_cast<float>(rightIndentationLineEdit->value());
}


bool TableFormatDialog::rightIndentationSet() const {
    EQt::DimensionLineEdit* rightIndentationLineEdit = widget<EQt::DimensionLineEdit>("right_indentation_line_edit");
    return rightIndentationLineEdit->valueOk();
}


bool TableFormatDialog::rightIndentationUnset() const {
    return !rightIndentationSet();
}


float TableFormatDialog::topSpacing() const {
    EQt::DimensionLineEdit* topSpacingLineEdit = widget<EQt::DimensionLineEdit>("top_spacing_line_edit");
    return static_cast<float>(topSpacingLineEdit->value());
}


bool TableFormatDialog::topSpacingSet() const {
    EQt::DimensionLineEdit* topSpacingLineEdit = widget<EQt::DimensionLineEdit>("top_spacing_line_edit");
    return topSpacingLineEdit->valueOk();
}


bool TableFormatDialog::topSpacingUnset() const {
    return !topSpacingSet();
}


float TableFormatDialog::bottomSpacing() const {
    EQt::DimensionLineEdit* bottomSpacingLineEdit = widget<EQt::DimensionLineEdit>("bottom_spacing_line_edit");
    return static_cast<float>(bottomSpacingLineEdit->value());
}


bool TableFormatDialog::bottomSpacingSet() const {
    EQt::DimensionLineEdit* bottomSpacingLineEdit = widget<EQt::DimensionLineEdit>("bottom_spacing_line_edit");
    return bottomSpacingLineEdit->valueOk();
}


bool TableFormatDialog::bottomSpacingUnset() const {
    return !bottomSpacingSet();
}


float TableFormatDialog::topGutter() const {
    EQt::DimensionLineEdit* topGutterLineEdit = widget<EQt::DimensionLineEdit>("top_gutter_line_edit");
    return static_cast<float>(topGutterLineEdit->value());
}


bool TableFormatDialog::topGutterSet() const {
    EQt::DimensionLineEdit* topGutterLineEdit = widget<EQt::DimensionLineEdit>("top_gutter_line_edit");
    return topGutterLineEdit->valueOk();
}


bool TableFormatDialog::topGutterUnset() const {
    return !topGutterSet();
}


float TableFormatDialog::leftGutter() const {
    EQt::DimensionLineEdit* leftGutterLineEdit = widget<EQt::DimensionLineEdit>("left_gutter_line_edit");
    return static_cast<float>(leftGutterLineEdit->value());
}


bool TableFormatDialog::leftGutterSet() const {
    EQt::DimensionLineEdit* leftGutterLineEdit = widget<EQt::DimensionLineEdit>("left_gutter_line_edit");
    return leftGutterLineEdit->valueOk();
}


bool TableFormatDialog::leftGutterUnset() const {
    return !leftGutterSet();
}


float TableFormatDialog::rightGutter() const {
    EQt::DimensionLineEdit* rightGutterLineEdit = widget<EQt::DimensionLineEdit>("right_gutter_line_edit");
    return static_cast<float>(rightGutterLineEdit->value());
}


bool TableFormatDialog::rightGutterSet() const {
    EQt::DimensionLineEdit* rightGutterLineEdit = widget<EQt::DimensionLineEdit>("right_gutter_line_edit");
    return rightGutterLineEdit->valueOk();
}


bool TableFormatDialog::rightGutterUnset() const {
    return !rightGutterSet();
}


float TableFormatDialog::bottomGutter() const {
    EQt::DimensionLineEdit* bottomGutterLineEdit = widget<EQt::DimensionLineEdit>("bottom_gutter_line_edit");
    return static_cast<float>(bottomGutterLineEdit->value());
}


bool TableFormatDialog::bottomGutterSet() const {
    EQt::DimensionLineEdit* bottomGutterLineEdit = widget<EQt::DimensionLineEdit>("bottom_gutter_line_edit");
    return bottomGutterLineEdit->valueOk();
}


bool TableFormatDialog::bottomGutterUnset() const {
    return !bottomGutterSet();
}


unsigned TableFormatDialog::numberRows() const {
    QSpinBox* numberRowsSpinBox = widget<QSpinBox>("number_rows_spin_box");
    return numberRowsSpinBox->value();
}


bool TableFormatDialog::numberRowsSet() const {
    QSpinBox* numberRowsSpinBox = widget<QSpinBox>("number_rows_spin_box");
    return !numberRowsSpinBox->text().isEmpty();
}


bool TableFormatDialog::numberRowsUnset() const {
    return !numberRowsSet();
}


unsigned TableFormatDialog::numberColumns() const {
    QSpinBox* numberColumnsSpinBox = widget<QSpinBox>("number_columns_spin_box");
    return numberColumnsSpinBox->value();
}


bool TableFormatDialog::numberColumnsSet() const {
    QSpinBox* numberColumnsSpinBox = widget<QSpinBox>("number_columns_spin_box");
    return !numberColumnsSpinBox->text().isEmpty();
}


bool TableFormatDialog::numberColumnsUnset() const {
    return !numberColumnsSet();
}


TableFormatDialog::LineStyle TableFormatDialog::verticalLineStyle() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->lineStyle();
}


bool TableFormatDialog::verticalLineStyleSet() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->lineStyle() != LineStyle::INVALID;
}


bool TableFormatDialog::verticalLineStyleUnset() const {
    return !verticalLineStyleSet();
}


float TableFormatDialog::verticalLineWidth() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->lineWidth();
}


bool TableFormatDialog::verticalLineWidthSet() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->width() > 0;
}


bool TableFormatDialog::verticalLineWidthUnset() const {
    return !verticalLineWidthSet();
}


TableFormatDialog::LineStyle TableFormatDialog::horizontalLineStyle() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->lineStyle();
}


bool TableFormatDialog::horizontalLineStyleSet() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->lineStyle() != LineStyle::INVALID;
}


bool TableFormatDialog::horizontalLineStyleUnset() const {
    return !horizontalLineStyleSet();
}


float TableFormatDialog::horizontalLineWidth() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->lineWidth();
}


bool TableFormatDialog::horizontalLineWidthSet() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->width() > 0;
}


bool TableFormatDialog::horizontalLineWidthUnset() const {
    return !horizontalLineWidthSet();
}


QColor TableFormatDialog::defaultCellColor() const {
    QColor result;

    EQt::ColorToolButton* defaultCellColorToolButton = widget<EQt::ColorToolButton>("default_cell_color_tool_button");
    const QSet<QColor>&   colors                     = defaultCellColorToolButton->colors();

    if (colors.size() == 1) {
        result = *colors.begin();
    }

    return result;
}


bool TableFormatDialog::defaultCellColorSet() const {
    EQt::ColorToolButton* defaultCellColorToolButton = widget<EQt::ColorToolButton>("default_cell_color_tool_button");
    return                defaultCellColorToolButton->colors().count() == 1;
}


bool TableFormatDialog::defaultCellColorUnset() const {
    return !defaultCellColorSet();
}


QList<TableFormatDialog::ColumnWidth> TableFormatDialog::columnWidths() const {
    QList<ColumnWidth> result;

    TableColumnWidthsWidget* columnWidthTableView = widget<TableColumnWidthsWidget>("table_column_widths_widget");
    unsigned                 numberColumns        = columnWidthTableView->numberTableColumns();
    for (unsigned columnIndex=0 ; columnIndex<numberColumns ; ++columnIndex) {
        bool  isProportional = columnWidthTableView->isProportionalColumn(columnIndex);
        float widthValue     = columnWidthTableView->columnWidth(columnIndex);

        ColumnWidth columnWidth;
        if (widthValue > 0) {
            if (isProportional) {
                columnWidth.setWidthType(ColumnWidth::WidthType::PROPORTIONAL);
            } else {
                columnWidth.setWidthType(ColumnWidth::WidthType::FIXED);
            }

            columnWidth.setWidth(widthValue);
        } else {
            columnWidth.setWidthType(ColumnWidth::WidthType::INVALID);
            columnWidth.setWidth(0.0F);
        }

        result.append(columnWidth);
    }

    return result;
}


void TableFormatDialog::populate() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("right_alignment_tool_button");

    leftAlignmentToolButton->setChecked(inputJustifications.contains(Justification::LEFT));
    centerAlignmentToolButton->setChecked(inputJustifications.contains(Justification::CENTER));
    rightAlignmentToolButton->setChecked(inputJustifications.contains(Justification::RIGHT));

    EQt::DimensionLineEdit* leftIndentationLineEdit = widget<EQt::DimensionLineEdit>("left_indentation_line_edit");
    if (inputLeftIndentationValid) {
        leftIndentationLineEdit->setValue(inputLeftIndentation);
    } else {
        leftIndentationLineEdit->clear();
    }

    EQt::DimensionLineEdit* rightIndentationLineEdit = widget<EQt::DimensionLineEdit>("right_indentation_line_edit");
    if (inputRightIndentationValid) {
        rightIndentationLineEdit->setValue(inputRightIndentation);
    } else {
        rightIndentationLineEdit->clear();
    }

    EQt::DimensionLineEdit* topSpacingLineEdit = widget<EQt::DimensionLineEdit>("top_spacing_line_edit");
    if (inputTopSpacingValid) {
        topSpacingLineEdit->setValue(inputTopSpacing);
    } else {
        topSpacingLineEdit->clear();
    }

    EQt::DimensionLineEdit* bottomSpacingLineEdit = widget<EQt::DimensionLineEdit>("bottom_spacing_line_edit");
    if (inputBottomSpacingValid) {
        bottomSpacingLineEdit->setValue(inputBottomSpacing);
    } else {
        bottomSpacingLineEdit->clear();
    }

    EQt::DimensionLineEdit* topGutterLineEdit = widget<EQt::DimensionLineEdit>("top_gutter_line_edit");
    if (inputTopGutterValid) {
        topGutterLineEdit->setValue(inputTopGutter);
    } else {
        topGutterLineEdit->clear();
    }

    EQt::DimensionLineEdit* leftGutterLineEdit = widget<EQt::DimensionLineEdit>("left_gutter_line_edit");
    if (inputLeftGutterValid) {
        leftGutterLineEdit->setValue(inputLeftGutter);
    } else {
        leftGutterLineEdit->clear();
    }

    EQt::DimensionLineEdit* rightGutterLineEdit = widget<EQt::DimensionLineEdit>("right_gutter_line_edit");
    if (inputRightGutterValid) {
        rightGutterLineEdit->setValue(inputRightGutter);
    } else {
        rightGutterLineEdit->clear();
    }

    EQt::DimensionLineEdit* bottomGutterLineEdit = widget<EQt::DimensionLineEdit>("bottom_gutter_line_edit");
    if (inputBottomGutterValid) {
        bottomGutterLineEdit->setValue(inputBottomGutter);
    } else {
        bottomGutterLineEdit->clear();
    }

    QSpinBox* numberRowsSpinBox = widget<QSpinBox>("number_rows_spin_box");
    if (inputNumberRows > 0) {
        numberRowsSpinBox->setValue(inputNumberRows);
    } else {
        numberRowsSpinBox->clear();
    }

    QSpinBox* numberColumnsSpinBox = widget<QSpinBox>("number_columns_spin_box");
    if (inputNumberColumns > 0) {
        numberColumnsSpinBox->setValue(inputNumberColumns);
    } else {
        numberColumnsSpinBox->clear();
    }

    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    defaultVerticalLineSettingsWidget->setLineStyles(inputVerticalLineStyles);

    if (inputVerticalLineWidth > 0.0F) {
        defaultVerticalLineSettingsWidget->setLineWidth(inputVerticalLineWidth);
    } else {
        defaultVerticalLineSettingsWidget->setLineWidthInvalid();
    }

    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    defaultHorizontalLineSettingsWidget->setLineStyles(inputHorizontalLineStyles);

    if (inputHorizontalLineWidth > 0.0F) {
        defaultHorizontalLineSettingsWidget->setLineWidth(inputHorizontalLineWidth);
    } else {
        defaultHorizontalLineSettingsWidget->setLineWidthInvalid();
    }

    EQt::ColorToolButton* defaultCellColorToolButton = widget<EQt::ColorToolButton>("default_cell_color_tool_button");
    defaultCellColorToolButton->setColors(inputDefaultCellColors);

    TableColumnWidthsWidget* columnWidthTableView    = widget<TableColumnWidthsWidget>("table_column_widths_widget");
    unsigned                 numberColumnWidthValues = static_cast<unsigned>(inputColumnWidths.size());
    for (unsigned columnIndex=0 ; columnIndex<numberColumnWidthValues ; ++columnIndex) {
        const ColumnWidth& columnWidth = inputColumnWidths.at(columnIndex);
        if (columnWidth.widthType() == ColumnWidth::WidthType::INVALID) {
            columnWidthTableView->setColumnWidthInvalid(columnIndex);
        } else {
            columnWidthTableView->setColumnWidth(
                columnIndex,
                columnWidth.widthType() == ColumnWidth::WidthType::PROPORTIONAL,
                columnWidth.width()
            );
        }
    }
}


void TableFormatDialog::setTableSizeControlsVisible(bool nowVisible) {
    QGroupBox* tableSizeGroupBox = widget<QGroupBox>("table_size_group_box");
    tableSizeGroupBox->setVisible(nowVisible);
}


void TableFormatDialog::setTableSizeControlsHidden(bool nowHidden) {
    setTableSizeControlsVisible(!nowHidden);
}


void TableFormatDialog::setAdvancedControlsEnabled(bool nowEnabled) {
    QToolButton* advancedToolButton = widget<QToolButton>("advanced_tool_button");
    advancedToolButton->setChecked(nowEnabled);

    advancedButtonClicked(nowEnabled);
}


void TableFormatDialog::setAdvancedControlsDisabled(bool nowDisabled) {
    setAdvancedControlsEnabled(!nowDisabled);
}


void TableFormatDialog::setAdvancedModeButtonVisible(bool nowVisible) {
    QToolButton* advancedToolButton = widget<QToolButton>("advanced_tool_button");
    advancedToolButton->setVisible(nowVisible);
}


void TableFormatDialog::setAdvancedModeButtonHidden(bool nowHidden) {
    setAdvancedModeButtonVisible(!nowHidden);
}


void TableFormatDialog::setJustification(const QSet<TableFormatDialog::Justification>& justificationModes) {
    inputJustifications = justificationModes;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setJustification(TableFormatDialog::Justification justificationMode) {
    QSet<Justification> justificationModes;
    justificationModes << justificationMode;

    setJustification(justificationModes);
}


void TableFormatDialog::setLeftIndentation(float newLeftIndentation) {
    inputLeftIndentation      = newLeftIndentation;
    inputLeftIndentationValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setLeftIndentationCleared() {
    inputLeftIndentation      = 0.0F;
    inputLeftIndentationValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setRightIndentation(float newRightIndentation) {
    inputRightIndentation      = newRightIndentation;
    inputRightIndentationValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setRightIndentationCleared() {
    inputRightIndentation      = 0.0F;
    inputRightIndentationValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setTopSpacing(float newTopSpacing) {
    inputTopSpacing      = newTopSpacing;
    inputTopSpacingValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setTopSpacingCleared() {
    inputTopSpacing = 0;
    inputTopSpacingValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setBottomSpacing(float newBottomSpacing) {
    inputBottomSpacing      = newBottomSpacing;
    inputBottomSpacingValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setBottomSpacingCleared() {
    inputBottomSpacing      = 0.0F;
    inputBottomSpacingValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setTopGutter(float newTopGutter) {
    inputTopGutter      = newTopGutter;
    inputTopGutterValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setTopGutterCleared() {
    inputTopGutter      = 0.0F;
    inputTopGutterValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setLeftGutter(float newLeftGutter) {
    inputLeftGutter      = newLeftGutter;
    inputLeftGutterValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setLeftGutterCleared() {
    inputLeftGutter      = 0.0F;
    inputLeftGutterValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setRightGutter(float newRightGutter) {
    inputRightGutter      = newRightGutter;
    inputRightGutterValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setRightGutterCleared() {
    inputRightGutter      = 0.0F;
    inputRightGutterValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setBottomGutter(float newBottomGutter) {
    inputBottomGutter      = newBottomGutter;
    inputBottomGutterValid = true;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setBottomGutterCleared() {
    inputBottomGutter      = 0.0F;
    inputBottomGutterValid = false;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setNumberRows(unsigned newNumberRows) {
    inputNumberRows = newNumberRows;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setMultipleNumberRows() {
    inputNumberRows = 0;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setNumberColumns(unsigned newNumberColumns) {
    inputNumberColumns = newNumberColumns;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setMultipleNumberColumns() {
    inputNumberColumns = 0;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setVerticalLineStyle(TableFormatDialog::LineStyle newLineStyle) {
    QSet<LineStyle> newLineStyles;
    newLineStyles << newLineStyle;

    setVerticalLineStyles(newLineStyles);
}


void TableFormatDialog::setVerticalLineStyles(const QSet<TableFormatDialog::LineStyle>& newLineStyles) {
    inputVerticalLineStyles = newLineStyles;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setVerticalLineWidth(float newLineWidth) {
    inputVerticalLineWidth = newLineWidth;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setVerticalLineWidths(const QSet<float>& newLineWidths) {
    if (newLineWidths.count() == 1) {
        inputVerticalLineWidth = *newLineWidths.begin();
    } else {
        inputVerticalLineWidth = -1.0F;
    }

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setHorizontalLineStyle(TableFormatDialog::LineStyle newLineStyle) {
    QSet<LineStyle> newLineStyles;
    newLineStyles << newLineStyle;

    setHorizontalLineStyles(newLineStyles);
}


void TableFormatDialog::setHorizontalLineStyles(const QSet<LineStyle>& newLineStyles) {
    inputHorizontalLineStyles = newLineStyles;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setHorizontalLineWidth(float newLineWidth) {
    inputHorizontalLineWidth = newLineWidth;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setHorizontalLineWidths(const QSet<float>& newLineWidths) {

    if (newLineWidths.count() == 1) {
        inputHorizontalLineWidth = *newLineWidths.begin();
    } else {
        inputHorizontalLineWidth = -1.0F;
    }

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setDefaultCellColor(const QColor& newCellColor) {
    QSet<QColor> newCellColors;
    newCellColors << newCellColor;

    setDefaultCellColors(newCellColors);
}


void TableFormatDialog::setDefaultCellColors(const QSet<QColor>& newCellColors) {
    inputDefaultCellColors = newCellColors;

    if (isVisible()) {
        populate();
    }
}


void TableFormatDialog::setColumnWidths(const QList<TableFormatDialog::ColumnWidth>& newColumnWidthValues) {
    inputColumnWidths = newColumnWidthValues;

    unsigned numberColumnWidthValues = static_cast<unsigned>(newColumnWidthValues.size());
    if (numberColumnWidthValues != numberColumns()) {
        setNumberColumns(newColumnWidthValues.size());
    } else {
        if (isVisible()) {
            populate();
        }
    }
}


void TableFormatDialog::bind() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("right_alignment_tool_button");

    connect(leftAlignmentToolButton,   &QToolButton::clicked, this, &TableFormatDialog::leftAlignmentButtonClicked);
    connect(centerAlignmentToolButton, &QToolButton::clicked, this, &TableFormatDialog::centerButtonClicked);
    connect(rightAlignmentToolButton,  &QToolButton::clicked, this, &TableFormatDialog::rightAlignmentButtonClicked);

    EQt::DimensionLineEdit* leftIndentationLineEdit = widget<EQt::DimensionLineEdit>("left_indentation_line_edit");
    EQt::DimensionLineEdit* rightIndentationLineEdit = widget<EQt::DimensionLineEdit>("right_indentation_line_edit");
    EQt::DimensionLineEdit* topSpacingLineEdit = widget<EQt::DimensionLineEdit>("top_spacing_line_edit");
    EQt::DimensionLineEdit* bottomSpacingLineEdit = widget<EQt::DimensionLineEdit>("bottom_spacing_line_edit");

    connect(
        leftIndentationLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &TableFormatDialog::leftIndentationChanged
    );
    connect(
        rightIndentationLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &TableFormatDialog::rightIndentationChanged
    );
    connect(
        topSpacingLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &TableFormatDialog::topSpacingChanged
    );
    connect(
        bottomSpacingLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &TableFormatDialog::bottomSpacingChanged
    );

    EQt::DimensionLineEdit* topGutterLineEdit    = widget<EQt::DimensionLineEdit>("top_gutter_line_edit");
    EQt::DimensionLineEdit* leftGutterLineEdit   = widget<EQt::DimensionLineEdit>("left_gutter_line_edit");
    EQt::DimensionLineEdit* rightGutterLineEdit  = widget<EQt::DimensionLineEdit>("right_gutter_line_edit");
    EQt::DimensionLineEdit* bottomGutterLineEdit = widget<EQt::DimensionLineEdit>("bottom_gutter_line_edit");

    connect(topGutterLineEdit,    &EQt::DimensionLineEdit::valueChanged, this, &TableFormatDialog::topGutterChanged);
    connect(leftGutterLineEdit,   &EQt::DimensionLineEdit::valueChanged, this, &TableFormatDialog::leftGutterChanged);
    connect(rightGutterLineEdit,  &EQt::DimensionLineEdit::valueChanged, this, &TableFormatDialog::rightGutterChanged);
    connect(bottomGutterLineEdit, &EQt::DimensionLineEdit::valueChanged, this, &TableFormatDialog::bottomGutterChanged);

    QToolButton* advancedToolButton         = widget<QToolButton>("advanced_tool_button");
    connect(advancedToolButton, &QToolButton::clicked, this, &TableFormatDialog::advancedButtonClicked);

    QSpinBox* numberRowsSpinBox = widget<QSpinBox>("number_rows_spin_box");
    QSpinBox* numberColumnsSpinBox = widget<QSpinBox>("number_columns_spin_box");
    connect(
        numberRowsSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &TableFormatDialog::numberRowsChanged
    );
    connect(
        numberColumnsSpinBox,
        static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
        this,
        &TableFormatDialog::numberColumnsChanged
    );

    QComboBox* unitsComboBox = widget<QComboBox>("units_combo_box");
    connect(unitsComboBox, &QComboBox::textActivated, this, &TableFormatDialog::newUnitSelected);

    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    connect(
        defaultHorizontalLineSettingsWidget,
        &LineSettingsWidget::lineStyleChanged,
        this,
        &TableFormatDialog::horizontalLineStyleChanged
    );
    connect(
        defaultHorizontalLineSettingsWidget,
        &LineSettingsWidget::lineWidthChanged,
        this,
        &TableFormatDialog::horizontalLineWidthChanged
    );

    connect(
        defaultVerticalLineSettingsWidget,
        &LineSettingsWidget::lineStyleChanged,
        this,
        &TableFormatDialog::verticalLineStyleChanged
    );
    connect(
        defaultVerticalLineSettingsWidget,
        &LineSettingsWidget::lineWidthChanged,
        this,
        &TableFormatDialog::verticalLineWidthChanged
    );

    TableColumnWidthsWidget* columnWidthTableView = widget<TableColumnWidthsWidget>("table_column_widths_widget");
    connect(
        columnWidthTableView,
        &TableColumnWidthsWidget::columnWidthChanged,
        this,
        &TableFormatDialog::columnWidthChanged
    );

    EQt::ColorToolButton* defaultCellColorToolButton = widget<EQt::ColorToolButton>("default_cell_color_tool_button");
    connect(
        defaultCellColorToolButton,
        &QToolButton::clicked,
        this,
        &TableFormatDialog::changeDefaultCellColorButtonClicked
    );
    QPushButton* changeDefaultCellColorButton = widget<QPushButton>("change_default_cell_color_push_button");
    connect(
        changeDefaultCellColorButton,
        &QPushButton::clicked,
        this,
        &TableFormatDialog::changeDefaultCellColorButtonClicked
    );
    QPushButton* removeDefaultCellColorPushButton = widget<QPushButton>("remove_default_cell_color_push_button");
    connect(
        removeDefaultCellColorPushButton,
        &QPushButton::clicked,
        this,
        &TableFormatDialog::removeDefaultCellColorButtonClicked
    );

    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    connect(applyPushButton,  &QPushButton::clicked, this, &TableFormatDialog::applyClicked);
    connect(okPushButton,     &QPushButton::clicked, this, &TableFormatDialog::accept);
    connect(cancelPushButton, &QPushButton::clicked, this, &TableFormatDialog::reject);
}


void TableFormatDialog::closeEvent(QCloseEvent* event) {
    if (okCancelButtonHidden()) {
        if (valuesChanged()) {
            setResult(QDialog::Accepted);
        } else {
            setResult(QDialog::Rejected);
        }
    }

    EQt::ProgrammaticDialog::closeEvent(event);
}


void TableFormatDialog::numberRowsChanged(int) {
    updateDialog();
}


void TableFormatDialog::numberColumnsChanged(int newNumberColumns) {
    TableColumnWidthsWidget* columnWidthTableView = widget<TableColumnWidthsWidget>("table_column_widths_widget");
    columnWidthTableView->setNumberTableColumns(newNumberColumns);

    updateDialog();
}


void TableFormatDialog::leftAlignmentButtonClicked() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("right_alignment_tool_button");

    leftAlignmentToolButton->setChecked(true);
    centerAlignmentToolButton->setChecked(false);
    rightAlignmentToolButton->setChecked(false);

    updateDialog();
}


void TableFormatDialog::centerButtonClicked() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("right_alignment_tool_button");

    leftAlignmentToolButton->setChecked(false);
    centerAlignmentToolButton->setChecked(true);
    rightAlignmentToolButton->setChecked(false);

    updateDialog();
}


void TableFormatDialog::rightAlignmentButtonClicked() {
    QToolButton* leftAlignmentToolButton   = widget<QToolButton>("left_alignment_tool_button");
    QToolButton* centerAlignmentToolButton = widget<QToolButton>("center_alignment_tool_button");
    QToolButton* rightAlignmentToolButton  = widget<QToolButton>("right_alignment_tool_button");

    leftAlignmentToolButton->setChecked(false);
    centerAlignmentToolButton->setChecked(false);
    rightAlignmentToolButton->setChecked(true);

    updateDialog();
}


void TableFormatDialog::leftIndentationChanged(double) {
    updateDialog();
}


void TableFormatDialog::rightIndentationChanged(double) {
    updateDialog();
}


void TableFormatDialog::topSpacingChanged(double) {
    updateDialog();
}


void TableFormatDialog::bottomSpacingChanged(double) {
    updateDialog();
}


void TableFormatDialog::topGutterChanged(double) {
    updateDialog();
}


void TableFormatDialog::leftGutterChanged(double) {
    updateDialog();
}


void TableFormatDialog::rightGutterChanged(double) {
    updateDialog();
}


void TableFormatDialog::bottomGutterChanged(double) {
    updateDialog();
}


void TableFormatDialog::horizontalLineStyleChanged(LineStyle) {
    updateDialog();
}


void TableFormatDialog::horizontalLineWidthChanged(float) {
    updateDialog();
}


void TableFormatDialog::verticalLineStyleChanged(LineStyle) {
    updateDialog();
}


void TableFormatDialog::verticalLineWidthChanged(float) {
    updateDialog();
}


void TableFormatDialog::columnWidthChanged(unsigned, bool, float) {
    updateDialog();
}


void TableFormatDialog::changeDefaultCellColorButtonClicked() {
    EQt::ColorDialog* colorDialog = widget<EQt::ColorDialog>("color_dialog");
    int result = colorDialog->exec();
    if (result == EQt::ColorDialog::Accepted) {
        EQt::ColorToolButton* defaultCellColorToolButton =
            widget<EQt::ColorToolButton>("default_cell_color_tool_button");

        defaultCellColorToolButton->setColor(colorDialog->currentColor());
        updateDialog();
    }
}


void TableFormatDialog::removeDefaultCellColorButtonClicked() {
    EQt::ColorToolButton* defaultCellColorToolButton =
        widget<EQt::ColorToolButton>("default_cell_color_tool_button");

    defaultCellColorToolButton->setColor(QColor());
    updateDialog();
}


void TableFormatDialog::advancedButtonClicked(bool nowEnabled) {
    QToolButton* advancedToolButton = widget<QToolButton>("advanced_tool_button");

    widget<TableColumnWidthsWidget>("table_column_widths_widget")->setVisible(nowEnabled);
    widget<QGroupBox>("default_horizontal_line_group_box")->setVisible(nowEnabled);
    widget<QGroupBox>("default_vertical_line_group_box")->setVisible(nowEnabled);
    widget<QGroupBox>("gutters_group_box")->setVisible(nowEnabled);
    widget<QGroupBox>("default_cell_color_group_box")->setVisible(nowEnabled);
    widget<QComboBox>("units_combo_box")->setVisible(nowEnabled);

    if (nowEnabled) {
        advancedToolButton->setIcon(Application::icon("advanced_controls_visible"));
    } else {
        advancedToolButton->setIcon(Application::icon("advanced_controls_hidden"));
    }

    adjustSize();
}


void TableFormatDialog::newUnitSelected(const QString& unitText) {
    TableColumnWidthsWidget* columnWidthTableView = widget<TableColumnWidthsWidget>("table_column_widths_widget");

    if        (unitText == tr("points")) {
        columnWidthTableView->setDisplayedUnits(TableColumnWidthsWidget::Unit::POINT);
    } else if (unitText == tr("picas")) {
        columnWidthTableView->setDisplayedUnits(TableColumnWidthsWidget::Unit::PICA);
    } else if (unitText == tr("millimeters")) {
        columnWidthTableView->setDisplayedUnits(TableColumnWidthsWidget::Unit::MILLIMETER);
    } else if (unitText == tr("centimeters")) {
        columnWidthTableView->setDisplayedUnits(TableColumnWidthsWidget::Unit::CENTIMETER);
    } else if (unitText == tr("inches")) {
        columnWidthTableView->setDisplayedUnits(TableColumnWidthsWidget::Unit::INCH);
    } else {
        columnWidthTableView->setDisplayedUnits(TableColumnWidthsWidget::Unit::POINT);
    }
}


void TableFormatDialog::applyClicked() {
    emit apply(this);
}


void TableFormatDialog::updateDialog() {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");

    bool changed = valuesChanged();

    applyPushButton->setEnabled(changed);
    okPushButton->setEnabled(changed || currentOkButtonAlwaysEnabled);

    if (currentAutoApplyEnabled) {
        emit apply(this);
    }
}


void TableFormatDialog::configureWidget() {
    setWindowTitle(tr("Format Table"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentAutoApplyEnabled    = false;

    inputJustifications       << defaultJustification;
    inputLeftIndentation       = defaultLeftIndentation;
    inputLeftIndentationValid  = defaultLeftIndentationValid;
    inputRightIndentation      = defaultRightIndentation;
    inputRightIndentationValid = defaultRightIndentationValid;
    inputTopSpacing            = defaultTopSpacing;
    inputTopSpacingValid       = defaultTopSpacingValid;
    inputBottomSpacing         = defaultBottomSpacing;
    inputBottomSpacingValid    = defaultBottomSpacingValid;
    inputTopGutter             = defaultTopGutter;
    inputTopGutterValid        = defaultTopGutterValid;
    inputLeftGutter            = defaultLeftGutter;
    inputLeftGutterValid       = defaultLeftGutterValid;
    inputRightGutter           = defaultRightGutter;
    inputRightGutterValid      = defaultRightGutterValid;
    inputBottomGutter          = defaultBottomGutter;
    inputBottomGutterValid     = defaultBottomGutterValid;
    inputNumberRows            = defaultNumberRows;
    inputNumberColumns         = defaultNumberColumns;
    inputVerticalLineStyles   << defaultVerticalLineStyle;
    inputVerticalLineWidth     = defaultVerticalLineWidth;
    inputHorizontalLineStyles << defaultHorizontalLineStyle;
    inputHorizontalLineWidth   = defaultHorizontalLineWidth;
    inputDefaultCellColors    << defaultCellBackgroundColor;
    for (unsigned columnIndex=0 ; columnIndex<defaultNumberColumns ; ++columnIndex) {
        inputColumnWidths << ColumnWidth(ColumnWidth::WidthType::PROPORTIONAL, 0.125);
    }

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    mainLayout->setSizeConstraint(QVBoxLayout::SetFixedSize);

    QHBoxLayout* topHorizontalLayout = newHBoxLayout("top_horizontal_layout");
    mainLayout->addLayout(topHorizontalLayout);

    QVBoxLayout* topLeftVerticalLayout = newVBoxLayout("top_left_vertical_layout");
    topHorizontalLayout->addLayout(topLeftVerticalLayout);

    QGroupBox* tableSizeGroupBox = new QGroupBox(tr("Table Size"));
    registerWidget(tableSizeGroupBox, "table_size_group_box");
    topLeftVerticalLayout->addWidget(tableSizeGroupBox);

    QFormLayout* tableSizeLayout = newFormLayout("table_size_layout");
    tableSizeGroupBox->setLayout(tableSizeLayout);

    QSpinBox* numberRowsSpinBox = new QSpinBox;
    numberRowsSpinBox->setToolTip(tr("Sets the number of table rows"));
    numberRowsSpinBox->setWhatsThis(
        tr("You can use this field to set the initial number of table rows in the table.")
    );
    numberRowsSpinBox->setMinimum(1);
    registerWidget(numberRowsSpinBox, "number_rows_spin_box");

    tableSizeLayout->addRow(tr("Rows: "), numberRowsSpinBox);

    QSpinBox* numberColumnsSpinBox = new QSpinBox;
    numberColumnsSpinBox->setToolTip(tr("Sets the number of table columns"));
    numberColumnsSpinBox->setWhatsThis(
        tr("You can use this field to set the initial number of table columns in the table.")
    );
    numberColumnsSpinBox->setMinimum(1);
    registerWidget(numberColumnsSpinBox, "number_columns_spin_box");

    tableSizeLayout->addRow(tr("Columns: "), numberColumnsSpinBox);

    topLeftVerticalLayout->addStretch(1);

    QGroupBox* positionGroupBox = new QGroupBox(tr("Positioning And Alignment"));
    registerWidget(positionGroupBox, "position_group_box");
    topHorizontalLayout->addWidget(positionGroupBox);

    QVBoxLayout* positionVerticalLayout = newVBoxLayout("position_vertical_layout");
    positionGroupBox->setLayout(positionVerticalLayout);

    QHBoxLayout* alignmentHorizontalLayout = newHBoxLayout("alignment_horizontal_layout");
    positionVerticalLayout->addLayout(alignmentHorizontalLayout);

    alignmentHorizontalLayout->addStretch(1);

    QIcon leftAlignmentIcon(":format_table_left_align_active_128.png");
    QToolButton* leftAlignmentToolButton = new QToolButton;
    leftAlignmentToolButton->setIcon(leftAlignmentIcon);
    leftAlignmentToolButton->setIconSize(QSize(alignmentToolButtonSize, alignmentToolButtonSize));
    leftAlignmentToolButton->setCheckable(true);
    leftAlignmentToolButton->setToolTip(
        tr("Click here to left align the table within the specified margins and indentation")
    );
    leftAlignmentToolButton->setWhatsThis(
        tr("You can click this button to left align the table within the specified margins and indentation.")
    );
    alignmentHorizontalLayout->addWidget(leftAlignmentToolButton);
    registerWidget(leftAlignmentToolButton, "left_alignment_tool_button");

    alignmentHorizontalLayout->addStretch(1);

    QIcon centerAlignmentIcon(":format_table_center_active_128.png");
    QToolButton* centerAlignmentToolButton = new QToolButton;
    centerAlignmentToolButton->setIcon(centerAlignmentIcon);
    centerAlignmentToolButton->setIconSize(QSize(alignmentToolButtonSize, alignmentToolButtonSize));
    centerAlignmentToolButton->setCheckable(true);
    centerAlignmentToolButton->setToolTip(
        tr("Click here to center the table within the specified margins and indentation")
    );
    centerAlignmentToolButton->setWhatsThis(
        tr("You can click this button to center the table within the specified margins and indentation.")
    );
    alignmentHorizontalLayout->addWidget(centerAlignmentToolButton);
    registerWidget(centerAlignmentToolButton, "center_alignment_tool_button");

    alignmentHorizontalLayout->addStretch(1);

    QIcon rightAlignmentIcon(":format_table_right_align_active_128.png");
    QToolButton* rightAlignmentToolButton = new QToolButton;
    rightAlignmentToolButton->setIcon(rightAlignmentIcon);
    rightAlignmentToolButton->setIconSize(QSize(alignmentToolButtonSize, alignmentToolButtonSize));
    rightAlignmentToolButton->setCheckable(true);
    rightAlignmentToolButton->setToolTip(
        tr("Click here to right align the table within the specified margins and indentation")
    );
    rightAlignmentToolButton->setWhatsThis(
        tr("You can click this button to right align the table within the specified margins and indentation.")
    );
    alignmentHorizontalLayout->addWidget(rightAlignmentToolButton);
    registerWidget(rightAlignmentToolButton, "right_alignment_tool_button");

    alignmentHorizontalLayout->addStretch(1);

    QHBoxLayout* positionHorizontalLayout = newHBoxLayout("position_horizontal_layout");
    positionVerticalLayout->addLayout(positionHorizontalLayout);

    QVBoxLayout* indentationVerticalLayout = newVBoxLayout("indentation_vertical_layout");
    positionHorizontalLayout->addLayout(indentationVerticalLayout);

    QLabel* indentationLabel = new QLabel(tr("Horizontal Indentation:"));
    registerWidget(indentationLabel, "indentation_label");
    indentationVerticalLayout->addWidget(indentationLabel, Qt::AlignLeft);

    QFormLayout* indentationFormLayout = newFormLayout("indentation_form_layout");
    indentationVerticalLayout->addLayout(indentationFormLayout);

    EQt::DimensionLineEdit* leftIndentationLineEdit = new EQt::DimensionLineEdit;
    leftIndentationLineEdit->setToolTip(tr("Enter table left indentation here"));
    leftIndentationLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the indentation to be applied to the left side of the table.  Values can "
            "be entered in inches (\"), points (pt), picas (pc), centimeters (cm), or millimeters (mm)."
           )
    );
    leftIndentationLineEdit->setDynamicUnits(true);
    registerWidget(leftIndentationLineEdit, "left_indentation_line_edit");
    indentationFormLayout->addRow(tr("Left: "), leftIndentationLineEdit);

    EQt::DimensionLineEdit* rightIndentationLineEdit = new EQt::DimensionLineEdit;
    rightIndentationLineEdit->setToolTip(tr("Enter table right indentation here"));
    rightIndentationLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the indentation to be applied to the right side of the table.  Values "
            "can be entered in inches (\"), points (pt), picas (pc), centimeters (cm), or millimeters (mm)."
           )
    );
    rightIndentationLineEdit->setDynamicUnits(true);
    registerWidget(rightIndentationLineEdit, "right_indentation_line_edit");
    indentationFormLayout->addRow(tr("Right: "), rightIndentationLineEdit);

    positionHorizontalLayout->addSpacing(1);

    QVBoxLayout* spacingVerticalLayout = newVBoxLayout("spacing_vertical_layout");
    positionHorizontalLayout->addLayout(spacingVerticalLayout);

    QLabel* spacingLabel = new QLabel(tr("Vertical Spacing:"));
    registerWidget(spacingLabel, "spacing_label");
    spacingVerticalLayout->addWidget(spacingLabel, Qt::AlignLeft);

    QFormLayout* spacingFormLayout = newFormLayout("spacing_form_layout");
    spacingVerticalLayout->addLayout(spacingFormLayout);

    EQt::DimensionLineEdit* topSpacingLineEdit = new EQt::DimensionLineEdit;
    topSpacingLineEdit->setToolTip(tr("Enter table top spacing"));
    topSpacingLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the spacing to be applied above the table.  Actual spacing will be the "
            "maximum of this value and the bottom spacing of the previous item.  Values can be entered in inches "
            "(\"), points (pt), picas (pc), centimeters (cm), or millimeters (mm)."
           )
    );
    topSpacingLineEdit->setDynamicUnits(true);
    registerWidget(topSpacingLineEdit, "top_spacing_line_edit");
    spacingFormLayout->addRow(tr("Top: "), topSpacingLineEdit);

    EQt::DimensionLineEdit* bottomSpacingLineEdit = new EQt::DimensionLineEdit;
    bottomSpacingLineEdit->setToolTip(tr("Enter table bottom spacing"));
    bottomSpacingLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the spacing to be applied below the table.  Actual spacing will be the "
            "maximum of this value and the bottom spacing of the next item.  Values can be entered in inches (\"), "
            "points (pt), picas (pc), centimeters (cm), or millimeters (mm)."
           )
    );
    bottomSpacingLineEdit->setDynamicUnits(true);
    registerWidget(bottomSpacingLineEdit, "bottom_spacing_line_edit");
    spacingFormLayout->addRow(tr("Bottom: "), bottomSpacingLineEdit);



    QHBoxLayout* advancedOptionsButtonLayout = newHBoxLayout("advanced_options_button_layout");
    mainLayout->addLayout(advancedOptionsButtonLayout);

    advancedOptionsButtonLayout->addStretch(1);

    QToolButton* advancedToolButton = new QToolButton;
    advancedToolButton->setIcon(Application::icon("advanced_controls_hidden"));
    advancedToolButton->setIconSize(QSize(16, 16));
    advancedToolButton->setToolTip(tr("Click here to access or hide additional functions."));
    advancedToolButton->setWhatsThis(
        tr("You can click this button to enable or disable functions used to adjust individual column widths.")
    );
    advancedToolButton->setCheckable(true);
    registerWidget(advancedToolButton, "advanced_tool_button");
    advancedOptionsButtonLayout->addWidget(advancedToolButton);



    QHBoxLayout* advancedOptionHorizontalLayout = newHBoxLayout("advanced_options_horizontal_layout");
    mainLayout->addLayout(advancedOptionHorizontalLayout);

    QGroupBox* defaultHorizontalLineGroupBox = new QGroupBox(tr("Horizontal Lines (Default)"));
    registerWidget(defaultHorizontalLineGroupBox, "default_horizontal_line_group_box");
    advancedOptionHorizontalLayout->addWidget(defaultHorizontalLineGroupBox);

    QVBoxLayout* defaultHorizontalLineLayout = newVBoxLayout("default_horizontal_line_layout");
    defaultHorizontalLineGroupBox->setLayout(defaultHorizontalLineLayout);

    LineSettingsWidget* defaultHorizontalLineSettingsWidget = new LineSettingsWidget;
    defaultHorizontalLineSettingsWidget->setLineOrientation(LineSettingsWidget::LineOrientation::HORIZONTAL);
    registerWidget(defaultHorizontalLineSettingsWidget, "default_horizontal_line_settings_widget");
    defaultHorizontalLineLayout->addWidget(defaultHorizontalLineSettingsWidget);

    QGroupBox* defaultVerticalLineGroupBox = new QGroupBox(tr("Vertical Lines (Default)"));
    registerWidget(defaultVerticalLineGroupBox, "default_vertical_line_group_box");
    advancedOptionHorizontalLayout->addWidget(defaultVerticalLineGroupBox);

    QVBoxLayout* defaultVerticalLineLayout = newVBoxLayout("default_vertical_line_layout");
    defaultVerticalLineGroupBox->setLayout(defaultVerticalLineLayout);

    LineSettingsWidget* defaultVerticalLineSettingsWidget = new LineSettingsWidget;
    defaultVerticalLineSettingsWidget->setLineOrientation(LineSettingsWidget::LineOrientation::VERTICAL);
    registerWidget(defaultVerticalLineSettingsWidget, "default_vertical_line_settings_widget");
    defaultVerticalLineLayout->addWidget(defaultVerticalLineSettingsWidget);

    QGroupBox* guttersGroupBox = new QGroupBox(tr("Cell Gutters"));
    registerWidget(guttersGroupBox, "gutters_group_box");
    advancedOptionHorizontalLayout->addWidget(guttersGroupBox);

    QFormLayout* guttersFormLayout = newFormLayout("gutters_form_layout");
    guttersGroupBox->setLayout(guttersFormLayout);

    EQt::DimensionLineEdit* topGutterLineEdit = new EQt::DimensionLineEdit;
    registerWidget(topGutterLineEdit, "top_gutter_line_edit");
    guttersFormLayout->addRow(tr("Top: "), topGutterLineEdit);

    EQt::DimensionLineEdit* leftGutterLineEdit = new EQt::DimensionLineEdit;
    registerWidget(leftGutterLineEdit, "left_gutter_line_edit");
    guttersFormLayout->addRow(tr("Left: "), leftGutterLineEdit);

    EQt::DimensionLineEdit* rightGutterLineEdit = new EQt::DimensionLineEdit;
    registerWidget(rightGutterLineEdit, "right_gutter_line_edit");
    guttersFormLayout->addRow(tr("Right: "), rightGutterLineEdit);

    EQt::DimensionLineEdit* bottomGutterLineEdit = new EQt::DimensionLineEdit;
    registerWidget(bottomGutterLineEdit, "bottom_gutter_line_edit");
    guttersFormLayout->addRow(tr("Bottom: "), bottomGutterLineEdit);

    QVBoxLayout* advancedOptionsVerticalLayout = newVBoxLayout("advanced_options_vertical_layout");
    advancedOptionHorizontalLayout->addLayout(advancedOptionsVerticalLayout);
    advancedOptionsVerticalLayout->setSpacing(1);

    TableColumnWidthsWidget* columnWidthTableView = new TableColumnWidthsWidget;
    registerWidget(columnWidthTableView, "table_column_widths_widget");
    advancedOptionsVerticalLayout->addWidget(columnWidthTableView);

    QStringList unitsList;
    unitsList << tr("points") << tr("picas");
    if (Application::locale()->measurementSystem() == QLocale::MetricSystem) {
        unitsList << tr("millimeters") << tr("centimeters") << tr("inches");
    } else {
        unitsList << tr("inches") << tr("millimeters") << tr("centimeters");
    }

    QComboBox* unitsComboBox = new QComboBox;
    unitsComboBox->setEditable(false);
    unitsComboBox->addItems(unitsList);
    registerWidget(unitsComboBox, "units_combo_box");
    advancedOptionsVerticalLayout->addWidget(unitsComboBox);



    QHBoxLayout* bottomHorizontalLayout = newHBoxLayout("bottom_horizontal_layout");
    mainLayout->addLayout(bottomHorizontalLayout);

    QGroupBox* defaultCellColorGroupBox = new QGroupBox(tr("Table Background Color"));
    registerWidget(defaultCellColorGroupBox, "default_cell_color_group_box");
    bottomHorizontalLayout->addWidget(defaultCellColorGroupBox);

    QHBoxLayout* defaultCellColorLayout = newHBoxLayout("default_cell_color_layout");
    defaultCellColorGroupBox->setLayout(defaultCellColorLayout);

    EQt::ColorToolButton* defaultCellColorToolButton = new EQt::ColorToolButton;
    registerWidget(defaultCellColorToolButton, "default_cell_color_tool_button");
    defaultCellColorLayout->addWidget(defaultCellColorToolButton);

    QPushButton* changeDefaultCellColorPushButton = new QPushButton(tr("Change"));
    registerWidget(changeDefaultCellColorPushButton, "change_default_cell_color_push_button");
    defaultCellColorLayout->addWidget(changeDefaultCellColorPushButton);

    QPushButton* removeDefaultCellColorPushButton = new QPushButton(tr("Remove"));
    registerWidget(removeDefaultCellColorPushButton, "remove_default_cell_color_push_button");
    defaultCellColorLayout->addWidget(removeDefaultCellColorPushButton);

    bottomHorizontalLayout->addStretch(1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
    );

    buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

    buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
    buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

    bottomHorizontalLayout->addWidget(buttonBox);



    EQt::ColorDialog* colorDialog = new EQt::ColorDialog(this);
    registerWidget(colorDialog, "color_dialog");

    unitsComboBox->setCurrentText(tr("points"));
    columnWidthTableView->setDisplayedUnits(TableColumnWidthsWidget::Unit::POINT);

    setAdvancedControlsDisabled();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
}


bool TableFormatDialog::valuesChanged() const {
    bool changed = (
           (   inputJustifications.count() != 1
            && justification() != Justification::NONE
           )
        || (   inputJustifications.count() == 1
            && justification() != *inputJustifications.begin()
           )
        || (   !inputLeftIndentationValid
            && leftIndentationSet()
           )
        || (    inputLeftIndentationValid
            && Util::Units::pointValuesNotEqual(leftIndentation(), inputLeftIndentation)
           )
        || (   !inputRightIndentationValid
            && rightIndentationSet()
           )
        || (   inputRightIndentationValid
            && Util::Units::pointValuesNotEqual(rightIndentation(), inputRightIndentation)
           )
        || (   !inputTopSpacingValid
            && topSpacingSet()
           )
        || (   inputTopSpacingValid
            && Util::Units::pointValuesNotEqual(inputTopSpacing, topSpacing())
           )
        || (   !inputBottomSpacingValid
            && bottomSpacingSet()
           )
        || (   inputBottomSpacingValid
            && Util::Units::pointValuesNotEqual(inputBottomSpacing, bottomSpacing())
           )
        || (   !inputTopGutterValid
            && topGutterSet()
           )
        || (   inputTopGutterValid
            && Util::Units::pointValuesNotEqual(inputTopGutter, topGutter())
           )
        || (   !inputLeftGutterValid
            && leftGutterSet()
           )
        || (   inputLeftGutterValid
            && Util::Units::pointValuesNotEqual(inputLeftGutter, leftGutter())
           )
        || (   !inputRightGutterValid
            && rightGutterSet()
           )
        || (   inputRightGutterValid
            && Util::Units::pointValuesNotEqual(inputRightGutter, rightGutter())
           )
        || (   !inputBottomGutterValid
            && bottomGutterSet()
           )
        || (   inputBottomGutterValid
            && Util::Units::pointValuesNotEqual(inputBottomGutter, bottomGutter())
           )
        || (inputNumberRows != numberRows())
        || (inputNumberColumns != numberColumns())
        || (   inputVerticalLineStyles.count() != 1
            && verticalLineStyleSet()
           )
        || (   inputVerticalLineStyles.count() == 1
            && verticalLineStyle() != *inputVerticalLineStyles.begin()
           )
        || (   inputVerticalLineWidth <= 0
            && verticalLineWidthSet()
           )
        || (   inputVerticalLineWidth > 0
            && Util::Units::pointValuesNotEqual(verticalLineWidth(), inputVerticalLineWidth)
           )
        || (   inputHorizontalLineStyles.count() != 1
            && horizontalLineStyleSet()
           )
        || (   inputHorizontalLineStyles.count() == 1
            && horizontalLineStyle() != *inputHorizontalLineStyles.begin()
           )
        || (   inputHorizontalLineWidth <= 0
            && horizontalLineWidthSet()
           )
        || (   inputHorizontalLineWidth > 0
            && Util::Units::pointValuesNotEqual(horizontalLineWidth(), inputHorizontalLineWidth)
           )
        || (   inputDefaultCellColors.count() != 1
            && defaultCellColorSet()
           )
        || (   inputDefaultCellColors.count() == 1
            && defaultCellColor() != *inputDefaultCellColors.begin()
           )
        || (inputColumnWidths != columnWidths())
    );

    return changed;
}
