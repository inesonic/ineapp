/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableDefaultLineStyleDialog class.
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
#include "table_format_dialog.h"
#include "table_default_line_style_dialog.h"

typedef TableFormatDialog::LineStyle LineStyle;

const LineStyle TableDefaultLineStyleDialog::defaultVerticalLineStyle   = TableFormatDialog::defaultVerticalLineStyle;
const float     TableDefaultLineStyleDialog::defaultVerticalLineWidth   = TableFormatDialog::defaultVerticalLineWidth;
const LineStyle TableDefaultLineStyleDialog::defaultHorizontalLineStyle = TableFormatDialog::defaultHorizontalLineStyle;
const float     TableDefaultLineStyleDialog::defaultHorizontalLineWidth = TableFormatDialog::defaultHorizontalLineWidth;


TableDefaultLineStyleDialog::TableDefaultLineStyleDialog(
        QWidget* parent
    ):ProgrammaticDialog(
        "TableDefaultLineStyleDialog",
        parent
    ) {
    configureWidget();
    runBuilders();
}


TableDefaultLineStyleDialog::~TableDefaultLineStyleDialog() {}


void TableDefaultLineStyleDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void TableDefaultLineStyleDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool TableDefaultLineStyleDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool TableDefaultLineStyleDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


bool TableDefaultLineStyleDialog::applyButtonVisible() const {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    return applyPushButton->isVisible();
}


bool TableDefaultLineStyleDialog::applyButtonHidden() const {
    return !applyButtonVisible();
}


void TableDefaultLineStyleDialog::setApplyButtonVisible(bool nowVisible) {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    applyPushButton->setVisible(nowVisible);
}


void TableDefaultLineStyleDialog::setApplyButtonHidden(bool nowHidden) {
    setApplyButtonVisible(!nowHidden);
}


bool TableDefaultLineStyleDialog::okCancelButtonVisible() const {
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    return okPushButton->isVisible();
}


bool TableDefaultLineStyleDialog::okCancelButtonHidden() const {
    return !okCancelButtonVisible();
}


void TableDefaultLineStyleDialog::setOkCancelButtonVisible(bool nowVisible) {
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    okPushButton->setVisible(nowVisible);
    cancelPushButton->setVisible(nowVisible);
}


void TableDefaultLineStyleDialog::setOkCancelButtonHidden(bool nowHidden) {
    setOkCancelButtonVisible(!nowHidden);
}


bool TableDefaultLineStyleDialog::okButtonAlwaysEnabled() const {
    return currentOkButtonAlwaysEnabled;
}


bool TableDefaultLineStyleDialog::okButtonConditionallyEnabled() const {
    return !currentOkButtonAlwaysEnabled;
}


void TableDefaultLineStyleDialog::setOkButtonAlwaysEnabled(bool nowAlwaysEnabled) {
    currentOkButtonAlwaysEnabled = nowAlwaysEnabled;

    bool         changed      = valuesChanged();
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");

    okPushButton->setEnabled(changed || currentOkButtonAlwaysEnabled);
}


void TableDefaultLineStyleDialog::setOkButtonConditionallyEnabled(bool nowConditionallyEnabled) {
    setOkButtonAlwaysEnabled(!nowConditionallyEnabled);
}


TableDefaultLineStyleDialog::LineStyle TableDefaultLineStyleDialog::verticalLineStyle() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->lineStyle();
}


bool TableDefaultLineStyleDialog::verticalLineStyleSet() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->lineStyle() != LineStyle::INVALID;
}


bool TableDefaultLineStyleDialog::verticalLineStyleUnset() const {
    return !verticalLineStyleSet();
}


float TableDefaultLineStyleDialog::verticalLineWidth() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->lineWidth();
}


bool TableDefaultLineStyleDialog::verticalLineWidthSet() const {
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    return defaultVerticalLineSettingsWidget->width() > 0;
}


bool TableDefaultLineStyleDialog::verticalLineWidthUnset() const {
    return !verticalLineWidthSet();
}


TableDefaultLineStyleDialog::LineStyle TableDefaultLineStyleDialog::horizontalLineStyle() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->lineStyle();
}


bool TableDefaultLineStyleDialog::horizontalLineStyleSet() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->lineStyle() != LineStyle::INVALID;
}


bool TableDefaultLineStyleDialog::horizontalLineStyleUnset() const {
    return !horizontalLineStyleSet();
}


float TableDefaultLineStyleDialog::horizontalLineWidth() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->lineWidth();
}


bool TableDefaultLineStyleDialog::horizontalLineWidthSet() const {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");

    return defaultHorizontalLineSettingsWidget->width() > 0;
}


bool TableDefaultLineStyleDialog::horizontalLineWidthUnset() const {
    return !horizontalLineWidthSet();
}


void TableDefaultLineStyleDialog::populate() {
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
}


void TableDefaultLineStyleDialog::setVerticalLineStyle(TableDefaultLineStyleDialog::LineStyle newLineStyle) {
    QSet<LineStyle> newLineStyles;
    newLineStyles << newLineStyle;

    setVerticalLineStyles(newLineStyles);
}


void TableDefaultLineStyleDialog::setVerticalLineStyles(const QSet<TableDefaultLineStyleDialog::LineStyle>& newLineStyles) {
    inputVerticalLineStyles = newLineStyles;

    if (isVisible()) {
        populate();
    }
}


void TableDefaultLineStyleDialog::setVerticalLineWidth(float newLineWidth) {
    inputVerticalLineWidth = newLineWidth;

    if (isVisible()) {
        populate();
    }
}


void TableDefaultLineStyleDialog::setVerticalLineWidths(const QSet<float>& newLineWidths) {
    if (newLineWidths.count() == 1) {
        inputVerticalLineWidth = *newLineWidths.begin();
    } else {
        inputVerticalLineWidth = -1.0F;
    }

    if (isVisible()) {
        populate();
    }
}


void TableDefaultLineStyleDialog::setHorizontalLineStyle(TableDefaultLineStyleDialog::LineStyle newLineStyle) {
    QSet<LineStyle> newLineStyles;
    newLineStyles << newLineStyle;

    setHorizontalLineStyles(newLineStyles);
}


void TableDefaultLineStyleDialog::setHorizontalLineStyles(const QSet<LineStyle>& newLineStyles) {
    inputHorizontalLineStyles = newLineStyles;

    if (isVisible()) {
        populate();
    }
}


void TableDefaultLineStyleDialog::setHorizontalLineWidth(float newLineWidth) {
    inputHorizontalLineWidth = newLineWidth;

    if (isVisible()) {
        populate();
    }
}


void TableDefaultLineStyleDialog::setHorizontalLineWidths(const QSet<float>& newLineWidths) {

    if (newLineWidths.count() == 1) {
        inputHorizontalLineWidth = *newLineWidths.begin();
    } else {
        inputHorizontalLineWidth = -1.0F;
    }

    if (isVisible()) {
        populate();
    }
}


void TableDefaultLineStyleDialog::bind() {
    LineSettingsWidget* defaultHorizontalLineSettingsWidget =
        widget<LineSettingsWidget>("default_horizontal_line_settings_widget");
    LineSettingsWidget* defaultVerticalLineSettingsWidget =
        widget<LineSettingsWidget>("default_vertical_line_settings_widget");

    connect(
        defaultHorizontalLineSettingsWidget,
        &LineSettingsWidget::lineStyleChanged,
        this,
        &TableDefaultLineStyleDialog::horizontalLineStyleChanged
    );
    connect(
        defaultHorizontalLineSettingsWidget,
        &LineSettingsWidget::lineWidthChanged,
        this,
        &TableDefaultLineStyleDialog::horizontalLineWidthChanged
    );

    connect(
        defaultVerticalLineSettingsWidget,
        &LineSettingsWidget::lineStyleChanged,
        this,
        &TableDefaultLineStyleDialog::verticalLineStyleChanged
    );
    connect(
        defaultVerticalLineSettingsWidget,
        &LineSettingsWidget::lineWidthChanged,
        this,
        &TableDefaultLineStyleDialog::verticalLineWidthChanged
    );

    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    connect(applyPushButton,  &QPushButton::clicked, this, &TableDefaultLineStyleDialog::applyClicked);
    connect(okPushButton,     &QPushButton::clicked, this, &TableDefaultLineStyleDialog::accept);
    connect(cancelPushButton, &QPushButton::clicked, this, &TableDefaultLineStyleDialog::reject);
}


void TableDefaultLineStyleDialog::closeEvent(QCloseEvent* event) {
    if (okCancelButtonHidden()) {
        if (valuesChanged()) {
            setResult(QDialog::Accepted);
        } else {
            setResult(QDialog::Rejected);
        }
    }

    EQt::ProgrammaticDialog::closeEvent(event);
}


void TableDefaultLineStyleDialog::horizontalLineStyleChanged(LineStyle) {
    updateDialog();
}


void TableDefaultLineStyleDialog::horizontalLineWidthChanged(float) {
    updateDialog();
}


void TableDefaultLineStyleDialog::verticalLineStyleChanged(LineStyle) {
    updateDialog();
}


void TableDefaultLineStyleDialog::verticalLineWidthChanged(float) {
    updateDialog();
}


void TableDefaultLineStyleDialog::applyClicked() {
    emit apply(this);
}


void TableDefaultLineStyleDialog::updateDialog() {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");

    bool changed = valuesChanged();

    applyPushButton->setEnabled(changed);
    okPushButton->setEnabled(changed || currentOkButtonAlwaysEnabled);

    if (currentAutoApplyEnabled) {
        emit apply(this);
    }
}


void TableDefaultLineStyleDialog::configureWidget() {
    setWindowTitle(tr("Format Table"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentAutoApplyEnabled    = false;

    inputVerticalLineStyles   << defaultVerticalLineStyle;
    inputVerticalLineWidth     = defaultVerticalLineWidth;
    inputHorizontalLineStyles << defaultHorizontalLineStyle;
    inputHorizontalLineWidth   = defaultHorizontalLineWidth;

    QBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    mainLayout->setSizeConstraint(QVBoxLayout::SetFixedSize);

    QHBoxLayout* topHorizontalLayout = newHBoxLayout("top_horizontal_layout");
    mainLayout->addLayout(topHorizontalLayout);

    QGroupBox* defaultHorizontalLineGroupBox = new QGroupBox(tr("Horizontal Lines (Default)"));
    registerWidget(defaultHorizontalLineGroupBox, "default_horizontal_line_group_box");
    topHorizontalLayout->addWidget(defaultHorizontalLineGroupBox);

    QVBoxLayout* defaultHorizontalLineLayout = newVBoxLayout("default_horizontal_line_layout");
    defaultHorizontalLineGroupBox->setLayout(defaultHorizontalLineLayout);

    LineSettingsWidget* defaultHorizontalLineSettingsWidget = new LineSettingsWidget;
    defaultHorizontalLineSettingsWidget->setLineOrientation(LineSettingsWidget::LineOrientation::HORIZONTAL);
    registerWidget(defaultHorizontalLineSettingsWidget, "default_horizontal_line_settings_widget");
    defaultHorizontalLineLayout->addWidget(defaultHorizontalLineSettingsWidget);

    QGroupBox* defaultVerticalLineGroupBox = new QGroupBox(tr("Vertical Lines (Default)"));
    registerWidget(defaultVerticalLineGroupBox, "default_vertical_line_group_box");
    topHorizontalLayout->addWidget(defaultVerticalLineGroupBox);

    QVBoxLayout* defaultVerticalLineLayout = newVBoxLayout("default_vertical_line_layout");
    defaultVerticalLineGroupBox->setLayout(defaultVerticalLineLayout);

    LineSettingsWidget* defaultVerticalLineSettingsWidget = new LineSettingsWidget;
    defaultVerticalLineSettingsWidget->setLineOrientation(LineSettingsWidget::LineOrientation::VERTICAL);
    registerWidget(defaultVerticalLineSettingsWidget, "default_vertical_line_settings_widget");
    defaultVerticalLineLayout->addWidget(defaultVerticalLineSettingsWidget);


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

    mainLayout->addWidget(buttonBox);

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
}


bool TableDefaultLineStyleDialog::valuesChanged() const {
    bool changed = (
           (   inputVerticalLineStyles.count() != 1
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
    );

    return changed;
}
