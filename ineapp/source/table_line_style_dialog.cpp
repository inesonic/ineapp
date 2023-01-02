/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref TableLineStyleDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QLocale>
#include <QDialog>
#include <QSize>
#include <QSet>
#include <QChar>
#include <QPixmap>
#include <QString>
#include <QLabel>
#include <QPalette>
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
#include "table_line_style_dialog.h"

typedef TableFormatDialog::LineStyle LineStyle;


TableLineStyleDialog::TableLineStyleDialog(
        bool     horizontalOrientation,
        QWidget* parent
    ):ProgrammaticDialog(
        "TableLineStyleDialog",
        parent
    ) {
    configureWidget(horizontalOrientation);
    runBuilders();
}


TableLineStyleDialog::~TableLineStyleDialog() {}


void TableLineStyleDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void TableLineStyleDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool TableLineStyleDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool TableLineStyleDialog::autoApplyDisabled() const {
    return !currentAutoApplyEnabled;
}


bool TableLineStyleDialog::applyButtonVisible() const {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    return applyPushButton->isVisible();
}


bool TableLineStyleDialog::applyButtonHidden() const {
    return !applyButtonVisible();
}


void TableLineStyleDialog::setApplyButtonVisible(bool nowVisible) {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    applyPushButton->setVisible(nowVisible);
}


void TableLineStyleDialog::setApplyButtonHidden(bool nowHidden) {
    setApplyButtonVisible(!nowHidden);
}


bool TableLineStyleDialog::okCancelButtonVisible() const {
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
    return okPushButton->isVisible();
}


bool TableLineStyleDialog::okCancelButtonHidden() const {
    return !okCancelButtonVisible();
}


void TableLineStyleDialog::setOkCancelButtonVisible(bool nowVisible) {
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    okPushButton->setVisible(nowVisible);
    cancelPushButton->setVisible(nowVisible);
}


void TableLineStyleDialog::setOkCancelButtonHidden(bool nowHidden) {
    setOkCancelButtonVisible(!nowHidden);
}


bool TableLineStyleDialog::okButtonAlwaysEnabled() const {
    return currentOkButtonAlwaysEnabled;
}


bool TableLineStyleDialog::okButtonConditionallyEnabled() const {
    return !currentOkButtonAlwaysEnabled;
}


void TableLineStyleDialog::setOkButtonAlwaysEnabled(bool nowAlwaysEnabled) {
    currentOkButtonAlwaysEnabled = nowAlwaysEnabled;

    bool         changed      = valuesChanged();
    QPushButton* okPushButton = widget<QPushButton>("ok_push_button");

    okPushButton->setEnabled(changed || currentOkButtonAlwaysEnabled);
}


void TableLineStyleDialog::setOkButtonConditionallyEnabled(bool nowConditionallyEnabled) {
    setOkButtonAlwaysEnabled(!nowConditionallyEnabled);
}


TableLineStyleDialog::LineStyle TableLineStyleDialog::leftTopLineStyle() const {
    LineSettingsWidget* leftTopLineSettingsWidget = widget<LineSettingsWidget>("left_top_line_settings_widget");
    return leftTopLineSettingsWidget->lineStyle();
}


bool TableLineStyleDialog::leftTopLineStyleSet() const {
    LineSettingsWidget* leftTopLineSettingsWidget = widget<LineSettingsWidget>("left_top_line_settings_widget");
    return leftTopLineSettingsWidget->lineStyle() != LineStyle::INVALID;
}


bool TableLineStyleDialog::leftTopLineStyleUnset() const {
    return !leftTopLineStyleSet();
}


float TableLineStyleDialog::leftTopLineWidth() const {
    LineSettingsWidget* leftTopLineSettingsWidget = widget<LineSettingsWidget>("left_top_line_settings_widget");
    return leftTopLineSettingsWidget->lineWidth();
}


bool TableLineStyleDialog::leftTopLineWidthSet() const {
    LineSettingsWidget* leftTopLineSettingsWidget = widget<LineSettingsWidget>("left_top_line_settings_widget");
    return leftTopLineSettingsWidget->width() > 0;
}


bool TableLineStyleDialog::leftTopLineWidthUnset() const {
    return !leftTopLineWidthSet();
}


TableLineStyleDialog::LineStyle TableLineStyleDialog::rightBottomLineStyle() const {
    LineSettingsWidget* rightBottomLineSettingsWidget = widget<LineSettingsWidget>("right_bottom_line_settings_widget");
    return rightBottomLineSettingsWidget->lineStyle();
}


bool TableLineStyleDialog::rightBottomLineStyleSet() const {
    LineSettingsWidget* rightBottomLineSettingsWidget = widget<LineSettingsWidget>("right_bottom_line_settings_widget");
    return rightBottomLineSettingsWidget->lineStyle() != LineStyle::INVALID;
}


bool TableLineStyleDialog::rightBottomLineStyleUnset() const {
    return !rightBottomLineStyleSet();
}


float TableLineStyleDialog::rightBottomLineWidth() const {
    LineSettingsWidget* rightBottomLineSettingsWidget = widget<LineSettingsWidget>("right_bottom_line_settings_widget");
    return rightBottomLineSettingsWidget->lineWidth();
}


bool TableLineStyleDialog::rightBottomLineWidthSet() const {
    LineSettingsWidget* rightBottomLineSettingsWidget = widget<LineSettingsWidget>("right_bottom_line_settings_widget");
    return rightBottomLineSettingsWidget->width() > 0;
}


bool TableLineStyleDialog::rightBottomLineWidthUnset() const {
    return !rightBottomLineWidthSet();
}


void TableLineStyleDialog::populate() {
    LineSettingsWidget* leftTopLineSettingsWidget = widget<LineSettingsWidget>("left_top_line_settings_widget");
    leftTopLineSettingsWidget->setLineStyles(inputLeftTopLineStyles);

    if (inputLeftTopLineWidth > 0.0F) {
        leftTopLineSettingsWidget->setLineWidth(inputLeftTopLineWidth);
    } else {
        leftTopLineSettingsWidget->setLineWidthInvalid();
    }

    LineSettingsWidget* rightBottomLineSettingsWidget = widget<LineSettingsWidget>("right_bottom_line_settings_widget");
    rightBottomLineSettingsWidget->setLineStyles(inputRightBottomLineStyles);

    if (inputRightBottomLineWidth > 0.0F) {
        rightBottomLineSettingsWidget->setLineWidth(inputRightBottomLineWidth);
    } else {
        rightBottomLineSettingsWidget->setLineWidthInvalid();
    }
}


void TableLineStyleDialog::setLeftTopLineStyle(TableLineStyleDialog::LineStyle newLineStyle) {
    QSet<LineStyle> newLineStyles;
    newLineStyles << newLineStyle;

    setLeftTopLineStyles(newLineStyles);
}


void TableLineStyleDialog::setLeftTopLineStyles(const QSet<TableLineStyleDialog::LineStyle>& newLineStyles) {
    inputLeftTopLineStyles = newLineStyles;

    if (isVisible()) {
        populate();
    }
}


void TableLineStyleDialog::setLeftTopLineWidth(float newLineWidth) {
    inputLeftTopLineWidth = newLineWidth;

    if (isVisible()) {
        populate();
    }
}


void TableLineStyleDialog::setLeftTopLineWidths(const QSet<float>& newLineWidths) {
    if (newLineWidths.count() == 1) {
        inputLeftTopLineWidth = *newLineWidths.begin();
    } else {
        inputLeftTopLineWidth = -1.0F;
    }

    if (isVisible()) {
        populate();
    }
}


void TableLineStyleDialog::setRightBottomLineStyle(TableLineStyleDialog::LineStyle newLineStyle) {
    QSet<LineStyle> newLineStyles;
    newLineStyles << newLineStyle;

    setRightBottomLineStyles(newLineStyles);
}


void TableLineStyleDialog::setRightBottomLineStyles(const QSet<LineStyle>& newLineStyles) {
    inputRightBottomLineStyles = newLineStyles;

    if (isVisible()) {
        populate();
    }
}


void TableLineStyleDialog::setRightBottomLineWidth(float newLineWidth) {
    inputRightBottomLineWidth = newLineWidth;

    if (isVisible()) {
        populate();
    }
}


void TableLineStyleDialog::setRightBottomLineWidths(const QSet<float>& newLineWidths) {

    if (newLineWidths.count() == 1) {
        inputRightBottomLineWidth = *newLineWidths.begin();
    } else {
        inputRightBottomLineWidth = -1.0F;
    }

    if (isVisible()) {
        populate();
    }
}


void TableLineStyleDialog::bind() {
    LineSettingsWidget* leftTopLineSettingsWidget     = widget<LineSettingsWidget>("left_top_line_settings_widget");
    LineSettingsWidget* rightBottomLineSettingsWidget = widget<LineSettingsWidget>("right_bottom_line_settings_widget");

    connect(
        leftTopLineSettingsWidget,
        &LineSettingsWidget::lineStyleChanged,
        this,
        &TableLineStyleDialog::leftTopLineStyleChanged
    );
    connect(
        leftTopLineSettingsWidget,
        &LineSettingsWidget::lineWidthChanged,
        this,
        &TableLineStyleDialog::leftTopLineWidthChanged
    );

    connect(
        rightBottomLineSettingsWidget,
        &LineSettingsWidget::lineStyleChanged,
        this,
        &TableLineStyleDialog::rightBottomLineStyleChanged
    );
    connect(
        rightBottomLineSettingsWidget,
        &LineSettingsWidget::lineWidthChanged,
        this,
        &TableLineStyleDialog::rightBottomLineWidthChanged
    );

    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");
    QPushButton* cancelPushButton = widget<QPushButton>("cancel_push_button");

    connect(applyPushButton,  &QPushButton::clicked, this, &TableLineStyleDialog::applyClicked);
    connect(okPushButton,     &QPushButton::clicked, this, &TableLineStyleDialog::accept);
    connect(cancelPushButton, &QPushButton::clicked, this, &TableLineStyleDialog::reject);
}


void TableLineStyleDialog::closeEvent(QCloseEvent* event) {
    if (okCancelButtonHidden()) {
        if (valuesChanged()) {
            setResult(QDialog::Accepted);
        } else {
            setResult(QDialog::Rejected);
        }
    }

    EQt::ProgrammaticDialog::closeEvent(event);
}


void TableLineStyleDialog::leftTopLineStyleChanged(LineStyle) {
    updateDialog();
}


void TableLineStyleDialog::leftTopLineWidthChanged(float) {
    updateDialog();
}


void TableLineStyleDialog::rightBottomLineStyleChanged(LineStyle) {
    updateDialog();
}


void TableLineStyleDialog::rightBottomLineWidthChanged(float) {
    updateDialog();
}


void TableLineStyleDialog::applyClicked() {
    emit apply(this);
}


void TableLineStyleDialog::updateDialog() {
    QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
    QPushButton* okPushButton     = widget<QPushButton>("ok_push_button");

    bool changed = valuesChanged();

    applyPushButton->setEnabled(changed);
    okPushButton->setEnabled(changed || currentOkButtonAlwaysEnabled);

    if (currentAutoApplyEnabled) {
        emit apply(this);
    }
}


void TableLineStyleDialog::configureWidget(bool horizontalOrientation) {
    setWindowTitle(tr("Format Table"));
    setSizeGripEnabled(false);
    setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    currentAutoApplyEnabled    = false;

    inputLeftTopLineStyles     << TableLineStyleDialog::LineStyle::SINGLE;
    inputLeftTopLineWidth       = 1.0F;
    inputRightBottomLineStyles << TableLineStyleDialog::LineStyle::SINGLE;
    inputRightBottomLineWidth   = 1.0F;

    QString                             leftTitle;
    QString                             rightTitle;
    LineSettingsWidget::LineOrientation lineOrientation;

    if (horizontalOrientation) {
        leftTitle       = tr("Top Horizontal Line");
        rightTitle      = tr("Bottom Horizontal Line");
        lineOrientation = LineSettingsWidget::LineOrientation::HORIZONTAL;
    } else {
        leftTitle       = tr("Left Vertical Line");
        rightTitle      = tr("Right Vertical Line");
        lineOrientation = LineSettingsWidget::LineOrientation::VERTICAL;
    }

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    mainLayout->setSizeConstraint(QVBoxLayout::SetFixedSize);

    QGridLayout* mainGridLayout = newGridLayout("main_grid_layout");
    mainLayout->addLayout(mainGridLayout);

    QGroupBox* leftLineGroupBox = new QGroupBox(leftTitle);
    registerWidget(leftLineGroupBox, "left_top_line_group_box");
    mainGridLayout->addWidget(leftLineGroupBox, 2, 2);

    QVBoxLayout* leftLineLayout = newVBoxLayout("left_top_line_layout");
    leftLineGroupBox->setLayout(leftLineLayout);

    LineSettingsWidget* leftLineSettingsWidget = new LineSettingsWidget;
    leftLineSettingsWidget->setLineOrientation(lineOrientation);
    registerWidget(leftLineSettingsWidget, "left_top_line_settings_widget");
    leftLineLayout->addWidget(leftLineSettingsWidget);

    QGroupBox* rightLineGroupBox = new QGroupBox(rightTitle);
    registerWidget(rightLineGroupBox, "right_bottom_line_group_box");
    mainGridLayout->addWidget(rightLineGroupBox, 2, 3);

    QVBoxLayout* rightLineLayout = newVBoxLayout("right_bottom_line_layout");
    rightLineGroupBox->setLayout(rightLineLayout);

    LineSettingsWidget* rightLineSettingsWidget = new LineSettingsWidget;
    rightLineSettingsWidget->setLineOrientation(lineOrientation);
    registerWidget(rightLineSettingsWidget, "right_bottom_line_settings_widget");
    rightLineLayout->addWidget(rightLineSettingsWidget);

    if (horizontalOrientation) {
        QLabel* upArrowLabel = new QLabel;
        upArrowLabel->setPixmap(QPixmap(":arrow_up.png"));
        registerWidget(upArrowLabel, "left_top_arrow_label");
        mainGridLayout->addWidget(upArrowLabel, 1, 2, 1, 1, Qt::AlignCenter);

        QLabel* downArrowLabel = new QLabel;
        downArrowLabel->setPixmap(QPixmap(":arrow_down.png"));
        registerWidget(downArrowLabel, "right_bottom_arrow_label");
        mainGridLayout->addWidget(downArrowLabel, 3, 3, 1, 1, Qt::AlignCenter);

        QWidget* spacerWidget1 = new QWidget;
        spacerWidget1->setFixedSize(spacerSize, spacerSize);
        registerWidget(spacerWidget1, "spacer_widget_1");
        mainGridLayout->addWidget(spacerWidget1, 2, 1);

        QWidget* spacerWidget2 = new QWidget;
        spacerWidget2->setFixedSize(spacerSize, spacerSize);
        registerWidget(spacerWidget2, "spacer_widget_2");
        mainGridLayout->addWidget(spacerWidget2, 2, 4);

        QWidget* leftTopLineWidget = new QWidget;
        leftTopLineWidget->setFixedHeight(1);
        leftTopLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        leftTopLineWidget->setStyleSheet("background-color: #000000;");
        registerWidget(leftTopLineWidget, "left_top_line_widget");
        mainGridLayout->addWidget(leftTopLineWidget, 0, 0, 1, 5);

        QWidget* rightBottomLineWidget = new QWidget;
        rightBottomLineWidget->setFixedHeight(1);
        rightBottomLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        rightBottomLineWidget->setStyleSheet("background-color: #000000;");
        registerWidget(rightBottomLineWidget, "right_bottom_line_widget");
        mainGridLayout->addWidget(rightBottomLineWidget, 4, 0, 1, 5);
    } else {
        QLabel* leftArrowLabel = new QLabel;
        leftArrowLabel->setPixmap(QPixmap(":arrow_left.png"));
        registerWidget(leftArrowLabel, "left_top_arrow_label");
        mainGridLayout->addWidget(leftArrowLabel, 2, 1, 1, 1, Qt::AlignCenter);

        QLabel* rightArrowLabel = new QLabel;
        rightArrowLabel->setPixmap(QPixmap(":arrow_right.png"));
        registerWidget(rightArrowLabel, "right_bottom_arrow_label");
        mainGridLayout->addWidget(rightArrowLabel, 2, 4, 1, 1, Qt::AlignCenter);

        QWidget* spacerWidget1 = new QWidget;
        spacerWidget1->setFixedSize(spacerSize, spacerSize);
        registerWidget(spacerWidget1, "spacer_widget_1");
        mainGridLayout->addWidget(spacerWidget1, 1, 2);

        QWidget* spacerWidget2 = new QWidget;
        spacerWidget2->setFixedSize(spacerSize, spacerSize);
        registerWidget(spacerWidget2, "spacer_widget_2");
        mainGridLayout->addWidget(spacerWidget2, 3, 2);

        QWidget* leftTopLineWidget = new QWidget;
        leftTopLineWidget->setFixedWidth(1);
        leftTopLineWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        leftTopLineWidget->setStyleSheet("background-color: #000000;");
        registerWidget(leftTopLineWidget, "left_top_line_widget");
        mainGridLayout->addWidget(leftTopLineWidget, 0, 0, 5, 1);

        QWidget* rightBottomLineWidget = new QWidget;
        rightBottomLineWidget->setFixedWidth(1);
        rightBottomLineWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        rightBottomLineWidget->setStyleSheet("background-color: #000000;");
        registerWidget(rightBottomLineWidget, "right_bottom_line_widget");
        mainGridLayout->addWidget(rightBottomLineWidget, 0, 5, 5, 1);
    }

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


bool TableLineStyleDialog::valuesChanged() const {
    bool changed = (
           (   inputLeftTopLineStyles.count() != 1
            && leftTopLineStyleSet()
           )
        || (   inputLeftTopLineStyles.count() == 1
            && leftTopLineStyle() != *inputLeftTopLineStyles.begin()
           )
        || (   inputLeftTopLineWidth <= 0
            && leftTopLineWidthSet()
           )
        || (   inputLeftTopLineWidth > 0
            && Util::Units::pointValuesNotEqual(leftTopLineWidth(), inputLeftTopLineWidth)
           )
        || (   inputRightBottomLineStyles.count() != 1
            && rightBottomLineStyleSet()
           )
        || (   inputRightBottomLineStyles.count() == 1
            && rightBottomLineStyle() != *inputRightBottomLineStyles.begin()
           )
        || (   inputRightBottomLineWidth <= 0
            && rightBottomLineWidthSet()
           )
        || (   inputRightBottomLineWidth > 0
            && Util::Units::pointValuesNotEqual(rightBottomLineWidth(), inputRightBottomLineWidth)
           )
    );

    return changed;
}
