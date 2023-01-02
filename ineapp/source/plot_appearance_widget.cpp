/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotAppearanceWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QColor>
#include <QFont>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSettings>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QTimer>

#include <eqt_dimension_line_edit.h>
#include <eqt_color_tool_button.h>
#include <eqt_font_button.h>
#include <eqt_color_dialog.h>

#include "application.h"
#include "metatypes.h"
#include "line_style_widget.h"
#include "character_format_dialog.h"
#include "plot_appearance_widget.h"

PlotAppearanceWidget::PlotAppearanceWidget(QWidget* parent):PlotWidgetBase(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QHBoxLayout* topLayout = new QHBoxLayout;
    mainLayout->addLayout(topLayout);

    QGroupBox* plotSizeGroupBox = new QGroupBox(tr("Plot Size"));
    topLayout->addWidget(plotSizeGroupBox);

    QFormLayout* plotSizeFormLayout = new QFormLayout;
    plotSizeGroupBox->setLayout(plotSizeFormLayout);

    plotWidthDimensionLineEdit = new EQt::DimensionLineEdit;
    plotWidthDimensionLineEdit->setUnits(EQt::DimensionLineEdit::Unit::PER_REGION);
    plotWidthDimensionLineEdit->setDynamicUnits();
    plotWidthDimensionLineEdit->setMinimumValue(0);
    plotSizeFormLayout->addRow(tr("Width: "), plotWidthDimensionLineEdit);

    plotHeightDimensionLineEdit = new EQt::DimensionLineEdit;
    plotHeightDimensionLineEdit->setUnits(EQt::DimensionLineEdit::Unit::PER_REGION);
    plotHeightDimensionLineEdit->setDynamicUnits();
    plotHeightDimensionLineEdit->setMinimumValue(0);
    plotSizeFormLayout->addRow(tr("Height: "), plotHeightDimensionLineEdit);

    topLayout->addStretch(1);

    QGroupBox* bordersAndBackgroundGroupBox = new QGroupBox(tr("Borders && Background"));
    topLayout->addWidget(bordersAndBackgroundGroupBox);

    QFormLayout* bordersAndBackgroundLayout = new QFormLayout;
    bordersAndBackgroundGroupBox->setLayout(bordersAndBackgroundLayout);

    borderLineStyleWidget = new LineStyleWidget;
    bordersAndBackgroundLayout->addRow(tr("Border Line: "), borderLineStyleWidget);

    backgroundColorToolButton = new EQt::ColorToolButton;
    backgroundColorToolButtonLabel = new QLabel(tr("Background Color: "));
    bordersAndBackgroundLayout->addRow(backgroundColorToolButtonLabel, backgroundColorToolButton);

    QHBoxLayout* middleLayout = new QHBoxLayout;
    mainLayout->addLayout(middleLayout);

    plotAreaGroupBox = new QGroupBox(tr("Plot Active Area"));
    middleLayout->addWidget(plotAreaGroupBox);

    QFormLayout* plotAreaLayout = new QFormLayout;
    plotAreaGroupBox->setLayout(plotAreaLayout);

    plotAreaBorderLineStyleWidget = new LineStyleWidget;
    plotAreaLayout->addRow(tr("Border Line: "), plotAreaBorderLineStyleWidget);

    plotAreaBackgroundColorToolButton = new EQt::ColorToolButton;
    plotAreaBackgroundColorToolButtonLabel = new QLabel(tr("background Color: "));
    plotAreaLayout->addRow(plotAreaBackgroundColorToolButtonLabel, plotAreaBackgroundColorToolButton);

    legendGroupBox = new QGroupBox(tr("Legend"));
    legendGroupBox->setCheckable(true);
    middleLayout->addWidget(legendGroupBox);

    QVBoxLayout* legendVerticalLayout = new QVBoxLayout;
    legendGroupBox->setLayout(legendVerticalLayout);

    QFormLayout* legendFormLayout = new QFormLayout;
    legendVerticalLayout->addLayout(legendFormLayout);

    legendLocationComboBox = new QComboBox;
    legendLocationComboBox->setEditable(false);
    legendLocationComboBox->addItem(tr("Right"), QVariant::fromValue(LegendLocation::RIGHT));
    legendLocationComboBox->addItem(tr("Bottom"), QVariant::fromValue(LegendLocation::BOTTOM));
    legendLocationComboBox->addItem(tr("Left"), QVariant::fromValue(LegendLocation::LEFT));
    legendLocationComboBox->addItem(tr("Top"), QVariant::fromValue(LegendLocation::TOP));
    legendFormLayout->addRow(tr("Location: "), legendLocationComboBox);

    legendBorderLineStyleWidget = new LineStyleWidget;
    legendFormLayout->addRow(tr("Border Line: "), legendBorderLineStyleWidget);

    legendBackgroundColorToolButton = new EQt::ColorToolButton;
    legendFormLayout->addRow(tr("Background Color: "), legendBackgroundColorToolButton);

    legendFontButton = new EQt::FontButton;
    legendFontButton->setGlyphText(tr("Abc"));
    legendFontButton->setDescriptionText(tr("Legend Font"));
    legendVerticalLayout->addWidget(legendFontButton);

    mainLayout->addStretch(1);

    titleGroupBox = new QGroupBox(tr("Title"));
    mainLayout->addWidget(titleGroupBox);

    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleGroupBox->setLayout(titleLayout);

    titleTextLineEdit = new QLineEdit;
    titleLayout->addWidget(titleTextLineEdit);

    titleFontButton = new EQt::FontButton;
    titleFontButton->setGlyphText(tr("Abc"));
    titleFontButton->setDescriptionText(tr("Title Font"));
    titleLayout->addWidget(titleFontButton);

    mainLayout->addStretch(1);

    showTimer = new QTimer(this);
    showTimer->setSingleShot(true);

    connect(
        plotWidthDimensionLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &PlotAppearanceWidget::plotWidthChanged
    );
    connect(
        plotHeightDimensionLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &PlotAppearanceWidget::plotHeightChanged
    );
    connect(
        borderLineStyleWidget,
        &LineStyleWidget::lineStyleChanged,
        this,
        &PlotAppearanceWidget::borderLineStyleWasChanged
    );
    connect(
        borderLineStyleWidget,
        &LineStyleWidget::lineWidthChanged,
        this,
        &PlotAppearanceWidget::borderLineWidthWasChanged
    );
    connect(
        borderLineStyleWidget,
        &LineStyleWidget::lineColorChanged,
        this,
        &PlotAppearanceWidget::borderLineColorWasChanged
    );
    connect(
        backgroundColorToolButton,
        &EQt::ColorToolButton::clicked,
        this,
        &PlotAppearanceWidget::backgroundColorButtonClicked
    );
    connect(
        plotAreaBorderLineStyleWidget,
        &LineStyleWidget::lineStyleChanged,
        this,
        &PlotAppearanceWidget::plotAreaBorderLineStyleWasChanged
    );
    connect(
        plotAreaBorderLineStyleWidget,
        &LineStyleWidget::lineWidthChanged,
        this,
        &PlotAppearanceWidget::borderLineWidthWasChanged
    );
    connect(
        plotAreaBorderLineStyleWidget,
        &LineStyleWidget::lineColorChanged,
        this,
        &PlotAppearanceWidget::borderLineColorWasChanged
    );
    connect(
        plotAreaBackgroundColorToolButton,
        &EQt::ColorToolButton::clicked,
        this,
        &PlotAppearanceWidget::plotAreaBackgroundColorButtonClicked
    );
    connect(
        legendLocationComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotAppearanceWidget::legendAreaComboBoxChanged
    );
    connect(
        legendGroupBox,
        &QGroupBox::clicked,
        this,
        &PlotAppearanceWidget::legendAreaGroupBoxClicked
    );
    connect(
        legendBorderLineStyleWidget,
        &LineStyleWidget::lineStyleChanged,
        this,
        &PlotAppearanceWidget::legendAreaBorderLineStyleWasChanged
    );
    connect(
        legendBorderLineStyleWidget,
        &LineStyleWidget::lineWidthChanged,
        this,
        &PlotAppearanceWidget::legendAreaBorderLineWidthWasChanged
    );
    connect(
        legendBorderLineStyleWidget,
        &LineStyleWidget::lineColorChanged,
        this,
        &PlotAppearanceWidget::legendAreaBorderLineColorWasChanged
    );
    connect(
        legendFontButton,
        &EQt::FontButton::clicked,
        this,
        &PlotAppearanceWidget::legendFontButtonClicked
    );
    connect(
        titleTextLineEdit,
        &QLineEdit::textEdited,
        this,
        &PlotAppearanceWidget::titleTextEdited
    );
    connect(
        titleFontButton,
        &EQt::FontButton::clicked,
        this,
        &PlotAppearanceWidget::titleFontButtonClicked
    );

    connect(showTimer, &QTimer::timeout, this, &PlotAppearanceWidget::nowVisible);
}


PlotAppearanceWidget::~PlotAppearanceWidget() {}


bool PlotAppearanceWidget::isValid() const {
    return (
           plotWidthDimensionLineEdit->value() > 0
        && plotHeightDimensionLineEdit->value() > 0
    );
}


void PlotAppearanceWidget::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    if (!groupName.isEmpty()) {
        settings->beginGroup(groupName);
    }

    plotWidthDimensionLineEdit->loadSettings(settings, "width_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);
    plotHeightDimensionLineEdit->loadSettings(settings, "height_line_edit", EQt::DimensionLineEdit::Unit::PER_REGION);

    if (!groupName.isEmpty()) {
        settings->endGroup();
    }
}


void PlotAppearanceWidget::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    if (!groupName.isEmpty()) {
        settings->beginGroup(groupName);
    }

    plotWidthDimensionLineEdit->saveSettings(settings, "width_line_edit");
    plotHeightDimensionLineEdit->saveSettings(settings, "height_line_edit");

    if (!groupName.isEmpty()) {
        settings->endGroup();
    }
}


float PlotAppearanceWidget::plotWidth() const {
    return static_cast<float>(plotWidthDimensionLineEdit->value());
}


float PlotAppearanceWidget::plotHeight() const {
    return static_cast<float>(plotHeightDimensionLineEdit->value());
}


float PlotAppearanceWidget::minimumPlotWidth() const {
    return static_cast<float>(plotWidthDimensionLineEdit->minimumValue());
}


float PlotAppearanceWidget::minimumPlotHeight() const {
    return static_cast<float>(plotHeightDimensionLineEdit->minimumValue());
}


PlotAppearanceWidget::LineStyle PlotAppearanceWidget::borderLineStyle() const {
    return borderLineStyleWidget->lineStyle();
}


float PlotAppearanceWidget::borderLineWidth() const {
    return borderLineStyleWidget->lineWidth();
}


QColor PlotAppearanceWidget::borderLineColor() const {
    return borderLineStyleWidget->lineColor();
}


QColor PlotAppearanceWidget::backgroundColor() const {
    return *backgroundColorToolButton->colors().begin();
}


PlotAppearanceWidget::LineStyle PlotAppearanceWidget::activeAreaBorderLineStyle() const {
    return plotAreaBorderLineStyleWidget->lineStyle();
}


float PlotAppearanceWidget::activeAreaBorderLineWidth() const {
    return plotAreaBorderLineStyleWidget->lineWidth();
}


QColor PlotAppearanceWidget::activeAreaBorderLineColor() const {
    return plotAreaBorderLineStyleWidget->lineColor();
}


QColor PlotAppearanceWidget::activeAreaColor() const {
    return *plotAreaBackgroundColorToolButton->colors().begin();
}


PlotAppearanceWidget::LegendLocation PlotAppearanceWidget::legendLocation() const {
    LegendLocation result;

    if (legendGroupBox->isChecked()) {
        if (legendLocationComboBox->currentIndex() >= 0) {
            result = legendLocationComboBox->currentData().value<LegendLocation>();
        } else {
            result = LegendLocation::NO_LEGEND;
        }
    } else {
        result = LegendLocation::NO_LEGEND;
    }

    return result;
}


PlotAppearanceWidget::LineStyle PlotAppearanceWidget::legendLineStyle() const {
    return legendBorderLineStyleWidget->lineStyle();
}


float PlotAppearanceWidget::legendLineWidth() const {
    return legendBorderLineStyleWidget->lineWidth();
}


QColor PlotAppearanceWidget::legendLineColor() const {
    return legendBorderLineStyleWidget->lineColor();
}


QColor PlotAppearanceWidget::legendBackgroundColor() const {
    return *legendBackgroundColorToolButton->colors().begin();
}


QFont PlotAppearanceWidget::legendFont() const {
    return legendFontButton->glyphFont();
}


QColor PlotAppearanceWidget::legendFontColor() const {
    return legendFontButton->fontColor();
}


QColor PlotAppearanceWidget::legendFontBackgroundColor() const {
    return legendFontButton->fontBackgroundColor();
}


QString PlotAppearanceWidget::titleText() const {
    return titleTextLineEdit->text();
}


QFont PlotAppearanceWidget::titleFont() const {
    return titleFontButton->glyphFont();
}


QColor PlotAppearanceWidget::titleFontColor() const {
    return titleFontButton->fontColor();
}


QColor PlotAppearanceWidget::titleFontBackgroundColor() const {
    return titleFontButton->fontBackgroundColor();
}


void PlotAppearanceWidget::setPlotWidth(float newPlotWidth) {
    plotWidthDimensionLineEdit->setValue(newPlotWidth);
}


void PlotAppearanceWidget::setPlotHeight(float newPlotHeight) {
    plotHeightDimensionLineEdit->setValue(newPlotHeight);
}


void PlotAppearanceWidget::setMinimumPlotWidth(float newMinimumWidth) {
    plotWidthDimensionLineEdit->setMinimumValue(newMinimumWidth);

    if (plotWidthDimensionLineEdit->value() < newMinimumWidth) {
        plotWidthDimensionLineEdit->setValue(newMinimumWidth);
    }
}


void PlotAppearanceWidget::setMinimumPlotHeight(float newMinimumHeight) {
    plotHeightDimensionLineEdit->setMinimumValue(newMinimumHeight);

    if (plotHeightDimensionLineEdit->value() < newMinimumHeight) {
        plotHeightDimensionLineEdit->setValue(newMinimumHeight);
    }
}


void PlotAppearanceWidget::setBorderLineStyle(PlotAppearanceWidget::LineStyle newBorderLineStyle) {
    borderLineStyleWidget->setLineStyle(newBorderLineStyle);
}


void PlotAppearanceWidget::setBorderLineWidth(float newBorderWidth) {
    borderLineStyleWidget->setLineWidth(newBorderWidth);
}


void PlotAppearanceWidget::setBorderLineColor(const QColor& newBorderColor) {
    borderLineStyleWidget->setLineColor(newBorderColor);
}


void PlotAppearanceWidget::setBackgroundColor(const QColor& newBackgroundColor) {
    backgroundColorToolButton->setColor(newBackgroundColor);
}


void PlotAppearanceWidget::setActiveAreaBorderLineStyle(PlotAppearanceWidget::LineStyle newLineStyle) {
    plotAreaBorderLineStyleWidget->setLineStyle(newLineStyle);
}


void PlotAppearanceWidget::setActiveAreaBorderLineWidth(float newLineWidth) {
    plotAreaBorderLineStyleWidget->setLineWidth(newLineWidth);
}


void PlotAppearanceWidget::setActiveAreaBorderLineColor(const QColor& newLineColor) {
    plotAreaBorderLineStyleWidget->setLineColor(newLineColor);
}


void PlotAppearanceWidget::setPlotAreaColor(const QColor& newPlotAreaColor) {
    plotAreaBackgroundColorToolButton->setColor(newPlotAreaColor);
}


void PlotAppearanceWidget::setLegendLocation(PlotAppearanceWidget::LegendLocation newLocation) {
    if (newLocation == LegendLocation::NO_LEGEND) {
        legendGroupBox->setChecked(false);
    } else {
        legendGroupBox->setChecked(true);

        unsigned numberEntries = static_cast<unsigned>(legendLocationComboBox->count());
        unsigned i             = 0;

        while (i < numberEntries && legendLocationComboBox->itemData(i).value<LegendLocation>() != newLocation) {
            ++i;
        }

        if (i < numberEntries) {
            legendLocationComboBox->setCurrentIndex(i);
        }
    }
}


void PlotAppearanceWidget::setLegendLineStyle(PlotAppearanceWidget::LineStyle newLineStyle) {
    legendBorderLineStyleWidget->setLineStyle(newLineStyle);
}


void PlotAppearanceWidget::setLegendLineWidth(float newLineWidth) {
    legendBorderLineStyleWidget->setLineWidth(newLineWidth);
}


void PlotAppearanceWidget::setLegendLineColor(const QColor& newLineColor) {
    legendBorderLineStyleWidget->setLineColor(newLineColor);
}


void PlotAppearanceWidget::setLegendFont(const QFont& newFont) {
    legendFontButton->setGlyphFont(newFont);
}


void PlotAppearanceWidget::setLegendFontColor(const QColor& newFontColor) {
    legendFontButton->setFontColor(newFontColor);
}


void PlotAppearanceWidget::setLegendFontBackgroundColor(const QColor& newFontBackgroundColor) {
    legendFontButton->setFontBackgroundColor(newFontBackgroundColor);
}


void PlotAppearanceWidget::setLegendBackgroundColor(const QColor& newLegendBackgroundColor) {
    legendBackgroundColorToolButton->setColor(newLegendBackgroundColor);
}


void PlotAppearanceWidget::setTitleText(const QString& newText) {
    titleTextLineEdit->setText(newText);
}


void PlotAppearanceWidget::setTitleFont(const QFont& newFont) {
    titleTextLineEdit->setFont(newFont);
    titleFontButton->setGlyphFont(newFont);
}


void PlotAppearanceWidget::setTitleFontColor(const QColor& newFontColor) {
    titleFontButton->setFontColor(newFontColor);
}


void PlotAppearanceWidget::setTitleFontBackgroundColor(const QColor& newFontBackgroundColor) {
    titleFontButton->setFontBackgroundColor(newFontBackgroundColor);
}


void PlotAppearanceWidget::setLegendControlsEnabled(bool nowEnabled) {
    legendGroupBox->setEnabled(nowEnabled);
    legendGroupBox->setVisible(nowEnabled);
}


void PlotAppearanceWidget::setLegendControlsDisabled(bool nowDisabled) {
    setLegendControlsEnabled(!nowDisabled);
}


void PlotAppearanceWidget::setActiveAreaControlsEnabled(bool nowEnabled) {
    plotAreaGroupBox->setEnabled(nowEnabled);
    plotAreaGroupBox->setVisible(nowEnabled);
}


void PlotAppearanceWidget::setActiveAreaControlsDisabled(bool nowDisabled) {
    setActiveAreaControlsEnabled(!nowDisabled);
}


void PlotAppearanceWidget::setActiveAreaBackgroundControlsEnabled(bool nowEnabled) {
    plotAreaBackgroundColorToolButtonLabel->setVisible(nowEnabled);
    plotAreaBackgroundColorToolButton->setVisible(nowEnabled);
}


void PlotAppearanceWidget::setActiveAreaBackgroundControlsDisabled(bool nowDisabled) {
    setActiveAreaBackgroundControlsEnabled(!nowDisabled);
}


void PlotAppearanceWidget::setPlotTitleEnabled(bool nowEnabled) {
    titleGroupBox->setEnabled(nowEnabled);
    titleGroupBox->setVisible(nowEnabled);
}


void PlotAppearanceWidget::setPlotTitleDisabled(bool nowDisabled) {
    setPlotTitleEnabled(!nowDisabled);
}


void PlotAppearanceWidget::setBackgroundColorButtonEnabled(bool nowEnabled) {
    backgroundColorToolButton->setEnabled(nowEnabled);
    backgroundColorToolButton->setVisible(nowEnabled);
    backgroundColorToolButtonLabel->setVisible(nowEnabled);
}


void PlotAppearanceWidget::setBackgroundColorButtonDisabled(bool nowDisabled) {
    setBackgroundColorButtonEnabled(!nowDisabled);
}


void PlotAppearanceWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    showTimer->start(0);
}


void PlotAppearanceWidget::plotWidthChanged(double newValue) {
    emit plotSizeChanged(newValue, plotHeightDimensionLineEdit->value());
    updateChangedStatus();
}


void PlotAppearanceWidget::plotHeightChanged(double newValue) {
    emit plotSizeChanged(plotWidthDimensionLineEdit->value(), newValue);
    updateChangedStatus();
}


void PlotAppearanceWidget::borderLineStyleWasChanged(PlotAppearanceWidget::LineStyle newLineStyle) {
    emit borderLineStyleChanged(newLineStyle);
    updateChangedStatus();
}


void PlotAppearanceWidget::borderLineWidthWasChanged(float newLineWidth) {
    emit borderLineWidthChanged(newLineWidth);
    updateChangedStatus();
}


void PlotAppearanceWidget::borderLineColorWasChanged(const QColor& newLineColor) {
    emit borderLineColorChanged(newLineColor);
    updateChangedStatus();
}


void PlotAppearanceWidget::backgroundColorButtonClicked() {
    EQt::ColorDialog dialog(*backgroundColorToolButton->colors().begin(), this);
    dialog.loadSettings();

    int result = dialog.exec();
    if (result == EQt::ColorDialog::DialogCode::Accepted) {
        QColor selectedColor = dialog.currentColor();
        backgroundColorToolButton->setColor(selectedColor);

        emit backgroundColorChanged(selectedColor);
        updateChangedStatus();
    }

    dialog.saveSettings();
}


void PlotAppearanceWidget::plotAreaBorderLineStyleWasChanged(PlotAppearanceWidget::LineStyle newLineStyle) {
    emit activeAreaBorderLineStyleChanged(newLineStyle);
    updateChangedStatus();
}


void PlotAppearanceWidget::plotAreaBorderLineWidthWasChanged(float newLineWidth) {
    emit activeAreaBorderLineWidthChanged(newLineWidth);
    updateChangedStatus();
}


void PlotAppearanceWidget::plotAreaBorderLineColorWasChanged(const QColor& newLineColor) {
    emit activeAreaBorderLineColorChanged(newLineColor);
    updateChangedStatus();
}


void PlotAppearanceWidget::plotAreaBackgroundColorButtonClicked() {
    EQt::ColorDialog dialog(*plotAreaBackgroundColorToolButton->colors().begin(), this);
    dialog.loadSettings();

    int result = dialog.exec();
    if (result == EQt::ColorDialog::DialogCode::Accepted) {
        QColor selectedColor = dialog.currentColor();
        plotAreaBackgroundColorToolButton->setColor(selectedColor);

        emit activeAreaColorChanged(selectedColor);
        updateChangedStatus();
    }

    dialog.saveSettings();
}


void PlotAppearanceWidget::legendAreaGroupBoxClicked(bool nowChecked) {
    if (nowChecked) {
        LegendLocation location =   legendLocationComboBox->currentIndex() >= 0
                                  ? legendLocationComboBox->currentData().value<LegendLocation>()
                                  : LegendLocation::NO_LEGEND;

        emit legendLocationChanged(location);
        updateChangedStatus();
    } else {
        emit legendLocationChanged(LegendLocation::NO_LEGEND);
        updateChangedStatus();
    }
}


void PlotAppearanceWidget::legendAreaComboBoxChanged(int index) {
    LegendLocation location =   index >= 0
                              ? legendLocationComboBox->itemData(index).value<LegendLocation>()
                              : LegendLocation::NO_LEGEND;

    emit legendLocationChanged(location);
    updateChangedStatus();
}


void PlotAppearanceWidget::legendAreaBorderLineStyleWasChanged(PlotAppearanceWidget::LineStyle newLineStyle) {
    emit legendLineStyleChanged(newLineStyle);
    updateChangedStatus();
}


void PlotAppearanceWidget::legendAreaBorderLineWidthWasChanged(float newLineWidth) {
    emit legendLineWidthChanged(newLineWidth);
    updateChangedStatus();
}


void PlotAppearanceWidget::legendAreaBorderLineColorWasChanged(const QColor& newLineColor) {
    emit legendLineColorChanged(newLineColor);
    updateChangedStatus();
}


void PlotAppearanceWidget::legendAreaBackgroundColorButtonClicked() {
    EQt::ColorDialog dialog(*legendBackgroundColorToolButton->colors().begin(), this);
    dialog.loadSettings();

    int result = dialog.exec();
    if (result == EQt::ColorDialog::DialogCode::Accepted) {
        QColor selectedColor = dialog.currentColor();
        legendBackgroundColorToolButton->setColor(selectedColor);

        emit legendBackgroundColorChanged(selectedColor);
        updateChangedStatus();
    }

    dialog.saveSettings();
}


void PlotAppearanceWidget::legendFontButtonClicked() {
    CharacterFormatDialog dialog(this);

    dialog.loadSettings(QString("CharacterFormatDialog"));
    dialog.setAutoApplyDisabled();
    dialog.setPositionControlsDisabled();
    dialog.setFontBackgroundColorControlsDisabled();
    dialog.setApplyButtonHidden();

    QFont  currentFont         = legendFontButton->glyphFont();
    QColor fontColor           = legendFontButton->fontColor();
    QColor fontBackgroundColor = legendFontButton->fontBackgroundColor();

    dialog.setFontFamily(currentFont.family());
    dialog.setFontSize(currentFont.pointSizeF());
    dialog.setFontWeight(static_cast<Ld::FontFormat::Weight>(currentFont.weight()));
    dialog.setItalicsEnabled(currentFont.italic());
    dialog.setUnderlineEnabled(currentFont.underline());
    dialog.setOverlineEnabled(currentFont.overline());
    dialog.setStrikethroughEnabled(currentFont.strikeOut());
    dialog.setLetterSpacing((currentFont.letterSpacing() / 100.0) - 1.0);
    dialog.setFontColor(fontColor);
    dialog.setFontBackgroundColor(fontBackgroundColor);

    int result = dialog.exec();
    if (result == CharacterFormatDialog::DialogCode::Accepted) {
        QFont newFont(
            dialog.fontFamily(),
            dialog.fontSize(),
            static_cast<int>(dialog.fontWeight()),
            dialog.italicsEnabled()
        );
        newFont.setUnderline(dialog.underlineEnabled());
        newFont.setOverline(dialog.overlineEnabled());
        newFont.setStrikeOut(dialog.strikethroughEnabled());
        newFont.setLetterSpacing(QFont::SpacingType::PercentageSpacing, (dialog.letterSpacing() + 1.0) * 100.0);

        QColor newFontColor = dialog.fontColor();
        QColor newFontBackgroundColor = dialog.fontBackgroundColor();

        legendFontButton->setGlyphFont(newFont);
        legendFontButton->setFontColor(newFontColor);
        legendFontButton->setFontBackgroundColor(newFontBackgroundColor);

        bool changeDetected = false;
        if (newFont != currentFont) {
            emit legendFontChanged(newFont);
            changeDetected = true;
        }

        if (newFontColor != fontColor) {
            emit legendFontColorChanged(newFontColor);
            changeDetected = true;
        }

        if (newFontBackgroundColor != fontBackgroundColor) {
            emit legendFontBackgroundColorChanged(newFontBackgroundColor);
            changeDetected = true;
        }

        if (changeDetected) {
            updateChangedStatus();
        }
    }

    dialog.saveSettings("CharacterFormatDialog");
}


void PlotAppearanceWidget::titleTextEdited(const QString& newText) {
    emit titleTextChanged(newText);
    updateChangedStatus();
}


void PlotAppearanceWidget::titleFontButtonClicked() {
    CharacterFormatDialog dialog(this);

    dialog.loadSettings(QString("CharacterFormatDialog"));
    dialog.setAutoApplyDisabled();
    dialog.setPositionControlsDisabled();
    dialog.setFontBackgroundColorControlsDisabled();
    dialog.setApplyButtonHidden();

    QFont  currentFont         = titleFontButton->glyphFont();
    QColor fontColor           = titleFontButton->fontColor();
    QColor fontBackgroundColor = titleFontButton->fontBackgroundColor();

    dialog.setFontFamily(currentFont.family());
    dialog.setFontSize(currentFont.pointSizeF());
    dialog.setFontWeight(static_cast<Ld::FontFormat::Weight>(currentFont.weight()));
    dialog.setItalicsEnabled(currentFont.italic());
    dialog.setUnderlineEnabled(currentFont.underline());
    dialog.setOverlineEnabled(currentFont.overline());
    dialog.setStrikethroughEnabled(currentFont.strikeOut());
    dialog.setLetterSpacing((currentFont.letterSpacing() / 100.0) - 1.0);
    dialog.setFontColor(fontColor);
    dialog.setFontBackgroundColor(fontBackgroundColor);

    int result = dialog.exec();
    if (result == CharacterFormatDialog::DialogCode::Accepted) {
        QFont newFont(
            dialog.fontFamily(),
            dialog.fontSize(),
            static_cast<int>(dialog.fontWeight()),
            dialog.italicsEnabled()
        );
        newFont.setUnderline(dialog.underlineEnabled());
        newFont.setOverline(dialog.overlineEnabled());
        newFont.setStrikeOut(dialog.strikethroughEnabled());
        newFont.setLetterSpacing(QFont::SpacingType::PercentageSpacing, (dialog.letterSpacing() + 1.0) * 100.0);

        QColor newFontColor = dialog.fontColor();
        QColor newFontBackgroundColor = dialog.fontBackgroundColor();

        titleFontButton->setGlyphFont(newFont);
        titleFontButton->setFontColor(newFontColor);
        titleFontButton->setFontBackgroundColor(newFontBackgroundColor);

        titleTextLineEdit->setFont(newFont);

        bool changeDetected = false;
        if (newFont != currentFont) {
            emit titleFontChanged(newFont);
            changeDetected = true;
        }

        if (newFontColor != fontColor) {
            emit titleFontColorChanged(newFontColor);
            changeDetected = true;
        }

        if (newFontBackgroundColor != fontBackgroundColor) {
            emit titleFontBackgroundColorChanged(newFontBackgroundColor);
            changeDetected = true;
        }

        if (changeDetected) {
            updateChangedStatus();
        }
    }

    dialog.saveSettings("CharacterFormatDialog");
}


void PlotAppearanceWidget::nowVisible() {
    unsigned buttonSide = static_cast<unsigned>(legendLocationComboBox->height());
    QSize    buttonSize(buttonSide, buttonSide);

    backgroundColorToolButton->setFixedSize(buttonSize);
    plotAreaBackgroundColorToolButton->setFixedSize(buttonSize);
    legendBackgroundColorToolButton->setFixedSize(buttonSize);
}







