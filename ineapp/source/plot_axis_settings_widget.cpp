/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotAxisSettingsWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QFont>
#include <QColor>
#include <QVariant>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QFontMetrics>
#include <QShowEvent>
#include <QTimer>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QPen>
#include <QBrush>

#include <cmath>
#include <limits>

#include <eqt_color_tool_button.h>
#include <eqt_font_button.h>
#include <eqt_color_dialog.h>

#include "application.h"
#include "metatypes.h"
#include "double_automatic_line_edit.h"
#include "line_style_widget.h"
#include "character_format_dialog.h"
#include "plot_widget_base.h"
#include "plot_axis_settings_widget.h"

const double PlotAxisSettingsWidget::automaticScaling = std::numeric_limits<double>::infinity();

PlotAxisSettingsWidget::PlotAxisSettingsWidget(QWidget* parent):PlotWidgetBase(parent) {
    currentSupportedAxisScales << AxisScale::LINEAR << AxisScale::LOG << AxisScale::DB_10_LOG << AxisScale::DB_20_LOG;

    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    axisScalingComboBox = new QComboBox;
    axisScalingComboBox->setEditable(false);
    populateAxisScalingComboBox(axisScalingComboBox, currentSupportedAxisScales);

    QHBoxLayout* axisScalingHorizontalLayout = new QHBoxLayout;
    mainLayout->addLayout(axisScalingHorizontalLayout);

    axisScalingHorizontalLayout->addStretch(1);
    axisScalingHorizontalLayout->addWidget(axisScalingComboBox);
    axisScalingHorizontalLayout->addStretch(1);

    mainLayout->addSpacing(10);


    QFormLayout* formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    minimumValueDoubleAutomaticLineEditor = new DoubleAutomaticLineEdit;
    minimumValueDoubleAutomaticLineEditor->setAutomatic();

    minimumValueLabel = new QLabel(tr("Minimum: "));
    formLayout->addRow(minimumValueLabel, minimumValueDoubleAutomaticLineEditor);

    maximumValueDoubleAutomaticLineEditor = new DoubleAutomaticLineEdit;
    maximumValueDoubleAutomaticLineEditor->setAutomatic();

    maximumValueLabel = new QLabel(tr("Maximum: "));
    formLayout->addRow(maximumValueLabel, maximumValueDoubleAutomaticLineEditor);

    QHBoxLayout* majorTickMarkLayout = new QHBoxLayout;
    formLayout->addRow(tr("Major Tick Marks: "), majorTickMarkLayout);

    QHBoxLayout* minorTickMarkLayout = new QHBoxLayout;
    formLayout->addRow(tr("Minor Tick Marks: "), minorTickMarkLayout);


    majorTickMarkDoubleAutomaticLineEditor = new DoubleAutomaticLineEdit;
    majorTickMarkDoubleAutomaticLineEditor->setAutomatic();
    majorTickMarkLayout->addWidget(majorTickMarkDoubleAutomaticLineEditor);

    majorTickMarkStyleComboBox = new QComboBox;
    majorTickMarkStyleComboBox->setEditable(false);
    populateTickMarkStyleComboBox(majorTickMarkStyleComboBox);
    majorTickMarkLayout->addWidget(majorTickMarkStyleComboBox);

    majorTickMarkLayout->addSpacing(10);

    majorGridLineStyleWidget = new LineStyleWidget;
    majorTickMarkLayout->addWidget(majorGridLineStyleWidget);


    minorTickMarkDoubleAutomaticLineEditor = new DoubleAutomaticLineEdit;
    minorTickMarkDoubleAutomaticLineEditor->setAutomatic();
    minorTickMarkLayout->addWidget(minorTickMarkDoubleAutomaticLineEditor);

    minorTickMarkStyleComboBox = new QComboBox;
    minorTickMarkStyleComboBox->setEditable(false);
    populateTickMarkStyleComboBox(minorTickMarkStyleComboBox);
    minorTickMarkLayout->addWidget(minorTickMarkStyleComboBox);

    minorTickMarkLayout->addSpacing(10);

    minorGridLineStyleWidget = new LineStyleWidget;
    minorTickMarkLayout->addWidget(minorGridLineStyleWidget);

    mainLayout->addSpacing(10);
    mainLayout->addStretch(1);


    QHBoxLayout* fontTextButtonsLayout = new QHBoxLayout;
    mainLayout->addLayout(fontTextButtonsLayout);

    numberFontButton = new EQt::FontButton(tr("1.3E-5"), tr("Number Font"));
    fontTextButtonsLayout->addWidget(numberFontButton, 0, Qt::AlignmentFlag::AlignCenter);

    axisTitleGroupBox = new QGroupBox(tr("Axis Title"));
    fontTextButtonsLayout->addWidget(axisTitleGroupBox);

    QHBoxLayout* axisTitleLayout = new QHBoxLayout;
    axisTitleGroupBox->setLayout(axisTitleLayout);

    axisTitleLineEdit = new QLineEdit;
    axisTitleLayout->addWidget(axisTitleLineEdit);

    titleFontButton = new EQt::FontButton(tr("Abc"), QString());
    axisTitleLayout->addWidget(titleFontButton);

    majorGridLineStyleWidget->setVisible(false);
    minorGridLineStyleWidget->setVisible(false);

    mainLayout->addStretch(1);

    connect(
        axisScalingComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotAxisSettingsWidget::axisSettingChanged
    );
    connect(
        minimumValueDoubleAutomaticLineEditor,
        &DoubleAutomaticLineEdit::changed,
        this,
        static_cast<void (PlotAxisSettingsWidget::*)(double, bool)>(&PlotAxisSettingsWidget::minimumValueChanged)
    );
    connect(
        maximumValueDoubleAutomaticLineEditor,
        &DoubleAutomaticLineEdit::changed,
        this,
        static_cast<void (PlotAxisSettingsWidget::*)(double, bool)>(&PlotAxisSettingsWidget::maximumValueChanged)
    );
    connect(
        majorTickMarkDoubleAutomaticLineEditor,
        &DoubleAutomaticLineEdit::changed,
        this,
        &PlotAxisSettingsWidget::majorTickMarkValueChanged
    );
    connect(
        minorTickMarkDoubleAutomaticLineEditor,
        &DoubleAutomaticLineEdit::changed,
        this,
        &PlotAxisSettingsWidget::minorTickMarkValueChanged
    );
    connect(
        majorTickMarkStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotAxisSettingsWidget::majorTickMarkStyleChanged
    );
    connect(
        minorTickMarkStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotAxisSettingsWidget::minorTickMarkStyleChanged
    );
    connect(
        majorGridLineStyleWidget,
        &LineStyleWidget::lineStyleChanged,
        this,
        &PlotAxisSettingsWidget::majorGridLineStyleHasChanged
    );
    connect(
        minorGridLineStyleWidget,
        &LineStyleWidget::lineStyleChanged,
        this,
        &PlotAxisSettingsWidget::minorGridLineStyleHasChanged
    );
    connect(
        majorGridLineStyleWidget,
        &LineStyleWidget::lineWidthChanged,
        this,
        &PlotAxisSettingsWidget::majorGridLineWidthHasChanged
    );
    connect(
        minorGridLineStyleWidget,
        &LineStyleWidget::lineWidthChanged,
        this,
        &PlotAxisSettingsWidget::minorGridLineWidthHasChanged
    );
    connect(
        majorGridLineStyleWidget,
        &LineStyleWidget::lineColorChanged,
        this,
        &PlotAxisSettingsWidget::majorGridLineColorHasChanged
    );
    connect(
        minorGridLineStyleWidget,
        &LineStyleWidget::lineColorChanged,
        this,
        &PlotAxisSettingsWidget::minorGridLineColorHasChanged
    );
    connect(
        axisTitleLineEdit,
        &QLineEdit::textEdited,
        this,
        &PlotAxisSettingsWidget::axisTitleHasChanged
    );
    connect(
        titleFontButton,
        &EQt::FontButton::clicked,
        this,
        &PlotAxisSettingsWidget::titleFontButtonClicked
    );
    connect(
        numberFontButton,
        &EQt::FontButton::clicked,
        this,
        &PlotAxisSettingsWidget::numberFontButtonClicked
    );
}


PlotAxisSettingsWidget::~PlotAxisSettingsWidget() {}


bool PlotAxisSettingsWidget::isValid() const {
    double    currentMinimum = minimumValue();
    double    currentMaximum = maximumValue();

    bool valid = !std::isnan(currentMinimum) && !std::isnan(currentMaximum);
    if (valid) {
        AxisScale axisScale = PlotAxisSettingsWidget::axisScale();

        switch (axisScale) {
            case AxisScale::LINEAR: {
                valid = (
                       currentMinimum != currentMaximum
                    || (   currentMinimum == automaticScaling
                        && currentMaximum == automaticScaling
                       )
                );

                break;
            }

            case AxisScale::LOG: {
                if (currentMinimum != automaticScaling) {
                    if (currentMaximum == automaticScaling) {
                        valid = (currentMinimum > 0);
                    } else {
                        valid = (
                               currentMinimum > 0
                            && currentMaximum > 0
                            && currentMinimum < currentMaximum
                        );
                    }
                } else if (/* currentMinimum == automaticScaling && */ currentMaximum != automaticScaling) {
                    valid = (currentMaximum > 0);
                }

                break;
            }

            case AxisScale::DB_10_LOG:
            case AxisScale::DB_20_LOG: {
                if (currentMinimum != automaticScaling && currentMaximum != automaticScaling) {
                    valid = (currentMinimum < currentMaximum);
                }

                break;
            }
        }
    }

    if (valid) {
        double currentMajorStepSize = majorStepSize();
        double currentMinorStepSize = minorStepSize();

        valid = (
               !std::isnan(currentMajorStepSize)
            && !std::isnan(currentMinorStepSize)
            && currentMajorStepSize > 0
            && currentMinorStepSize > 0
        );
    }

    return valid;
}


QSet<PlotAxisSettingsWidget::AxisScale> PlotAxisSettingsWidget::supportedAxisScales() const {
    return currentSupportedAxisScales;
}


PlotAxisSettingsWidget::ScalingControlsMode PlotAxisSettingsWidget::scalingControlsMode() const {
    ScalingControlsMode result;

    bool comboBoxEnabled = axisScalingComboBox->isEnabled();
    if (!comboBoxEnabled) {
        result = ScalingControlsMode::NO_SCALING_CONTROLS;
    } else {
        bool automaticCheckBoxEnabled = minimumValueDoubleAutomaticLineEditor->automaticCheckboxEnabled();
        if (!automaticCheckBoxEnabled) {
            result = ScalingControlsMode::MANUAL_SCALING_CONTROLS;
        } else {
            result = ScalingControlsMode::ALL_SCALING_CONTROLS;
        }
    }

    return result;
}


PlotAxisSettingsWidget::AxisScale PlotAxisSettingsWidget::axisScale() const {
    AxisScale result;

    int currentIndex = axisScalingComboBox->currentIndex();
    if (currentIndex >= 0) {
        result = axisScalingComboBox->itemData(currentIndex).value<AxisScale>();
    } else {
        result = AxisScale::LINEAR;
    }

    return result;
}


double PlotAxisSettingsWidget::minimumValue() const {
    double result;

    if (minimumValueDoubleAutomaticLineEditor->automatic()) {
        result = automaticScaling;
    } else {
        result = minimumValueDoubleAutomaticLineEditor->value();
    }

    return result;
}


double PlotAxisSettingsWidget::maximumValue() const {
    double result;

    if (maximumValueDoubleAutomaticLineEditor->automatic()) {
        result = automaticScaling;
    } else {
        result = maximumValueDoubleAutomaticLineEditor->value();
    }

    return result;
}


double PlotAxisSettingsWidget::majorStepSize() const {
    double result;

    if (majorTickMarkDoubleAutomaticLineEditor->automatic()) {
        result = automaticScaling;
    } else {
        result = majorTickMarkDoubleAutomaticLineEditor->value();
    }

    return result;
}


double PlotAxisSettingsWidget::minorStepSize() const {
    double result;

    if (minorTickMarkDoubleAutomaticLineEditor->automatic()) {
        result = automaticScaling;
    } else {
        result = minorTickMarkDoubleAutomaticLineEditor->value();
    }

    return result;
}


PlotAxisSettingsWidget::TickStyle PlotAxisSettingsWidget::majorTickStyle() const {
    TickStyle result;

    int currentIndex = majorTickMarkStyleComboBox->currentIndex();
    if (currentIndex >= 0) {
        result = majorTickMarkStyleComboBox->itemData(currentIndex).value<TickStyle>();
    } else {
        result = TickStyle::NONE;
    }

    return result;
}


PlotAxisSettingsWidget::TickStyle PlotAxisSettingsWidget::minorTickStyle() const {
    TickStyle result;

    int currentIndex = minorTickMarkStyleComboBox->currentIndex();
    if (currentIndex >= 0) {
        result = minorTickMarkStyleComboBox->itemData(currentIndex).value<TickStyle>();
    } else {
        result = TickStyle::NONE;
    }

    return result;
}


PlotAxisSettingsWidget::LineStyle PlotAxisSettingsWidget::majorGridLineStyle() const {
    return majorGridLineStyleWidget->lineStyle();
}


float PlotAxisSettingsWidget::majorGridLineWidth() const {
    return majorGridLineStyleWidget->lineWidth();
}


QColor PlotAxisSettingsWidget::majorGridLineColor() const {
    return majorGridLineStyleWidget->lineColor();
}



PlotAxisSettingsWidget::LineStyle PlotAxisSettingsWidget::minorGridLineStyle() const {
    return minorGridLineStyleWidget->lineStyle();
}


float PlotAxisSettingsWidget::minorGridLineWidth() const {
    return minorGridLineStyleWidget->lineWidth();
}


QColor PlotAxisSettingsWidget::minorGridLineColor() const {
    return minorGridLineStyleWidget->lineColor();
}


bool PlotAxisSettingsWidget::axisTitleFieldVisible() const {
    return axisTitleGroupBox->isVisible();
}


bool PlotAxisSettingsWidget::axisTitleFieldHidden() const {
    return !axisTitleFieldVisible();
}


QString PlotAxisSettingsWidget::axisTitle() const {
    return axisTitleLineEdit->text();
}


QFont PlotAxisSettingsWidget::titleFont() const {
    return titleFontButton->glyphFont();
}


QColor PlotAxisSettingsWidget::titleFontColor() const {
    return titleFontButton->fontColor();
}


QColor PlotAxisSettingsWidget::titleFontBackgroundColor() const {
    return titleFontButton->fontBackgroundColor();
}


QFont PlotAxisSettingsWidget::numberFont() const {
    return numberFontButton->glyphFont();
}


QColor PlotAxisSettingsWidget::numberFontColor() const {
    return numberFontButton->fontColor();
}


QColor PlotAxisSettingsWidget::numberFontBackgroundColor() const {
    return numberFontButton->fontBackgroundColor();
}


void PlotAxisSettingsWidget::setSupportedAxisScales(
        const QSet<PlotAxisSettingsWidget::AxisScale>& newSupportedAxisScales
    ) {
    // Populating the axis scaling combo box triggers a number of spurious events that then create a loop.  To avoid
    // this, we temporarily disable processing of events until the updates are completed.

    disconnect(
        axisScalingComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotAxisSettingsWidget::axisSettingChanged
    );

    currentSupportedAxisScales = newSupportedAxisScales;
    populateAxisScalingComboBox(axisScalingComboBox, currentSupportedAxisScales);

    connect(
        axisScalingComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotAxisSettingsWidget::axisSettingChanged
    );
}


void PlotAxisSettingsWidget::setScalingControlsMode(
        PlotAxisSettingsWidget::ScalingControlsMode newScalingControlsMode
    ) {
    switch (newScalingControlsMode) {
        case ScalingControlsMode::NO_SCALING_CONTROLS: {
            axisScalingComboBox->setEnabled(false);
            axisScalingComboBox->setVisible(false);

            minimumValueLabel->setVisible(false);
            maximumValueLabel->setVisible(false);

            minimumValueDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();
            maximumValueDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();
            majorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();
            minorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();

            minimumValueDoubleAutomaticLineEditor->setLineEditorDisabled();
            maximumValueDoubleAutomaticLineEditor->setLineEditorDisabled();
            majorTickMarkDoubleAutomaticLineEditor->setLineEditorDisabled();
            minorTickMarkDoubleAutomaticLineEditor->setLineEditorDisabled();

            break;
        }

        case ScalingControlsMode::MANUAL_SCALING_CONTROLS: {
            axisScalingComboBox->setEnabled(true);
            axisScalingComboBox->setVisible(true);

            minimumValueLabel->setVisible(true);
            maximumValueLabel->setVisible(true);

            minimumValueDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();
            maximumValueDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();
//            majorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();
//            minorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxDisabled();
            majorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxEnabled();
            minorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxEnabled();

            minimumValueDoubleAutomaticLineEditor->setLineEditorEnabled();
            maximumValueDoubleAutomaticLineEditor->setLineEditorEnabled();
            majorTickMarkDoubleAutomaticLineEditor->setLineEditorEnabled();
            minorTickMarkDoubleAutomaticLineEditor->setLineEditorEnabled();

            break;
        }

        case ScalingControlsMode::ALL_SCALING_CONTROLS: {
            axisScalingComboBox->setEnabled(true);
            axisScalingComboBox->setVisible(true);

            minimumValueLabel->setVisible(true);
            maximumValueLabel->setVisible(true);

            minimumValueDoubleAutomaticLineEditor->setAutomaticCheckboxEnabled();
            maximumValueDoubleAutomaticLineEditor->setAutomaticCheckboxEnabled();
            majorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxEnabled();
            minorTickMarkDoubleAutomaticLineEditor->setAutomaticCheckboxEnabled();

            minimumValueDoubleAutomaticLineEditor->setLineEditorEnabled();
            maximumValueDoubleAutomaticLineEditor->setLineEditorEnabled();
            majorTickMarkDoubleAutomaticLineEditor->setLineEditorEnabled();
            minorTickMarkDoubleAutomaticLineEditor->setLineEditorEnabled();

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
 }


void PlotAxisSettingsWidget::setAxisScale(PlotAxisSettingsWidget::AxisScale newAxisScale) {
    unsigned numberItems = static_cast<unsigned>(axisScalingComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && axisScalingComboBox->itemData(i).value<AxisScale>() != newAxisScale) {
        ++i;
    }

    if (i < numberItems) {
        axisScalingComboBox->setCurrentIndex(i);
    }

    bool enableTickStepSizes = (newAxisScale != AxisScale::LOG);
    majorTickMarkDoubleAutomaticLineEditor->setEnabled(enableTickStepSizes);
    minorTickMarkDoubleAutomaticLineEditor->setEnabled(enableTickStepSizes);
}


void PlotAxisSettingsWidget::setMajorGridLineStyle(PlotAxisSettingsWidget::LineStyle newLineStyle) {
    majorGridLineStyleWidget->setLineStyle(newLineStyle);
}


void PlotAxisSettingsWidget::setMajorGridLineWidth(float newLineWidth) {
    majorGridLineStyleWidget->setLineWidth(newLineWidth);
}


void PlotAxisSettingsWidget::setMajorGridLineColor(const QColor& newLineColor) {
    majorGridLineStyleWidget->setLineColor(newLineColor);
}


void PlotAxisSettingsWidget::setMinorGridLineStyle(PlotAxisSettingsWidget::LineStyle newLineStyle) {
    minorGridLineStyleWidget->setLineStyle(newLineStyle);
}


void PlotAxisSettingsWidget::setMinorGridLineWidth(float newLineWidth) {
    minorGridLineStyleWidget->setLineWidth(newLineWidth);
}


void PlotAxisSettingsWidget::setMinorGridLineColor(const QColor& newLineColor) {
    minorGridLineStyleWidget->setLineColor(newLineColor);
}


void PlotAxisSettingsWidget::setMinimumValue(double newMinimumValue) {
    if (newMinimumValue == automaticScaling) {
        minimumValueDoubleAutomaticLineEditor->setAutomatic();
    } else {
        minimumValueDoubleAutomaticLineEditor->setValue(newMinimumValue);
    }
}


void PlotAxisSettingsWidget::setMaximumValue(double newMaximumValue) {
    if (newMaximumValue == automaticScaling) {
        maximumValueDoubleAutomaticLineEditor->setAutomatic();
    } else {
        maximumValueDoubleAutomaticLineEditor->setValue(newMaximumValue);
    }
}


void PlotAxisSettingsWidget::setMajorStepSize(double newMajorStepSize) {
    if (newMajorStepSize == automaticScaling) {
        majorTickMarkDoubleAutomaticLineEditor->setAutomatic();
    } else {
        majorTickMarkDoubleAutomaticLineEditor->setValue(newMajorStepSize);
    }
}


void PlotAxisSettingsWidget::setMinorStepSize(double newMinorStepSize) {
    if (newMinorStepSize == automaticScaling) {
        minorTickMarkDoubleAutomaticLineEditor->setAutomatic();
    } else {
        minorTickMarkDoubleAutomaticLineEditor->setValue(newMinorStepSize);
    }
}


void PlotAxisSettingsWidget::setMajorTickStyle(PlotAxisSettingsWidget::TickStyle newMajorTickStyle) {
    unsigned numberItems = static_cast<unsigned>(majorTickMarkStyleComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && majorTickMarkStyleComboBox->itemData(i).value<TickStyle>() != newMajorTickStyle) {
        ++i;
    }

    if (i < numberItems) {
        majorTickMarkStyleComboBox->setCurrentIndex(i);
    }
}


void PlotAxisSettingsWidget::setMinorTickStyle(PlotAxisSettingsWidget::TickStyle newMinorTickStyle) {
    unsigned numberItems = static_cast<unsigned>(minorTickMarkStyleComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && minorTickMarkStyleComboBox->itemData(i).value<TickStyle>() != newMinorTickStyle) {
        ++i;
    }

    if (i < numberItems) {
        minorTickMarkStyleComboBox->setCurrentIndex(i);
    }
}


void PlotAxisSettingsWidget::setAxisTitleVisible(bool nowVisible) {
    axisTitleGroupBox->setVisible(nowVisible);
}


void PlotAxisSettingsWidget::setAxisTitleHidden(bool nowHidden) {
    setAxisTitleVisible(!nowHidden);
}


void PlotAxisSettingsWidget::setAxisTitle(const QString& newAxisTitle) {
    axisTitleLineEdit->setText(newAxisTitle);
}


void PlotAxisSettingsWidget::setTitleFont(const QFont& newTitleFont) {
    titleFontButton->setGlyphFont(newTitleFont);
    axisTitleLineEdit->setFont(newTitleFont);
}


void PlotAxisSettingsWidget::setTitleFontColor(const QColor& newTitleFontColor) {
    titleFontButton->setFontColor(newTitleFontColor);
}


void PlotAxisSettingsWidget::setTitleFontBackgroundColor(const QColor& newTitleFontBackgroundColor) {
    return titleFontButton->setFontBackgroundColor(newTitleFontBackgroundColor);
}


void PlotAxisSettingsWidget::setNumberFont(const QFont& newNumberFont) {
    numberFontButton->setGlyphFont(newNumberFont);
}


void PlotAxisSettingsWidget::setNumberFontColor(const QColor& newNumberFontColor) const {
    numberFontButton->setFontColor(newNumberFontColor);
}


void PlotAxisSettingsWidget::setNumberFontBackgroundColor(const QColor& newNumberFontBackgroundColor) const {
    numberFontButton->setFontBackgroundColor(newNumberFontBackgroundColor);
}


void PlotAxisSettingsWidget::axisSettingChanged(int index) {
    AxisScale axisScale = axisScalingComboBox->itemData(index).value<AxisScale>();

    double newMinimum;
    double newMaximum;
    if (axisScale == AxisScale::LOG) {
        newMinimum = std::numeric_limits<double>::min();
        newMaximum = std::numeric_limits<double>::max();
    } else {
        newMinimum = std::numeric_limits<double>::lowest();
        newMaximum = std::numeric_limits<double>::max();
    }

    minimumValueDoubleAutomaticLineEditor->setMinimumValue(newMinimum);
    minimumValueDoubleAutomaticLineEditor->setMaximumValue(newMaximum);

    maximumValueDoubleAutomaticLineEditor->setMinimumValue(newMinimum);
    maximumValueDoubleAutomaticLineEditor->setMaximumValue(newMaximum);

    bool enableTickStepSizes = (axisScale != AxisScale::LOG);
    majorTickMarkDoubleAutomaticLineEditor->setEnabled(enableTickStepSizes);
    minorTickMarkDoubleAutomaticLineEditor->setEnabled(enableTickStepSizes);

    emit axisScaleChanged(axisScale);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::minimumValueChanged(double value, bool automatic) {
    emit minimumValueChanged(automatic ? automaticScaling : value);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::maximumValueChanged(double value, bool automatic) {
    emit maximumValueChanged(automatic ? automaticScaling : value);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::majorTickMarkValueChanged(double value, bool automatic) {
    emit majorStepSizeChanged(automatic ? automaticScaling : value);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::minorTickMarkValueChanged(double value, bool automatic) {
    emit minorStepSizeChanged(automatic ? automaticScaling : value);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::majorTickMarkStyleChanged(int index) {
    TickStyle tickStyle =   index >= 0
                          ? majorTickMarkStyleComboBox->itemData(index).value<TickStyle>()
                          : TickStyle::NONE;

    bool showGridControls = (tickStyle == TickStyle::LINE || tickStyle == TickStyle::LINE_AND_TICK);

    majorGridLineStyleWidget->setVisible(showGridControls);

    emit majorTickStyleChanged(tickStyle);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::minorTickMarkStyleChanged(int index) {
    TickStyle tickStyle =   index >= 0
                          ? minorTickMarkStyleComboBox->itemData(index).value<TickStyle>()
                          : TickStyle::NONE;

    bool showGridControls = (tickStyle == TickStyle::LINE || tickStyle == TickStyle::LINE_AND_TICK);

    minorGridLineStyleWidget->setVisible(showGridControls);

    emit minorTickStyleChanged(tickStyle);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::majorGridLineStyleHasChanged(PlotAxisSettingsWidget::LineStyle lineStyle) {
    emit majorGridLineStyleChanged(lineStyle);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::minorGridLineStyleHasChanged(PlotAxisSettingsWidget::LineStyle lineStyle) {
    emit minorGridLineStyleChanged(lineStyle);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::majorGridLineWidthHasChanged(double value) {
    emit majorGridLineWidthChanged(value);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::minorGridLineWidthHasChanged(double value) {
    emit minorGridLineWidthChanged(value);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::majorGridLineColorHasChanged(const QColor& newColor) {
    emit majorGridLineColorChanged(newColor);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::minorGridLineColorHasChanged(const QColor& newColor) {
    emit minorGridLineColorChanged(newColor);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::axisTitleHasChanged(const QString& newAxisTitle) {
    emit axisTitleChanged(newAxisTitle);
    updateChangedStatus();
}


void PlotAxisSettingsWidget::titleFontButtonClicked() {
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
    dialog.setLetterSpacing(currentFont.letterSpacing() / 100.0 - 1.0);
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

        setTitleFont(newFont);
        setTitleFontColor(newFontColor);
        setTitleFontBackgroundColor(newFontBackgroundColor);

        axisTitleLineEdit->setFont(newFont);

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


void PlotAxisSettingsWidget::numberFontButtonClicked() {
    CharacterFormatDialog dialog(this);

    dialog.loadSettings(QString("CharacterFormatDialog"));
    dialog.setAutoApplyDisabled();
    dialog.setPositionControlsDisabled();
    dialog.setFontBackgroundColorControlsDisabled();
    dialog.setApplyButtonHidden();

    QFont  currentFont         = numberFontButton->glyphFont();
    QColor fontColor           = numberFontButton->fontColor();
    QColor fontBackgroundColor = numberFontButton->fontBackgroundColor();

    dialog.setFontFamily(currentFont.family());
    dialog.setFontSize(currentFont.pointSizeF());
    dialog.setFontWeight(static_cast<Ld::FontFormat::Weight>(currentFont.weight()));
    dialog.setItalicsEnabled(currentFont.italic());
    dialog.setUnderlineEnabled(currentFont.underline());
    dialog.setOverlineEnabled(currentFont.overline());
    dialog.setStrikethroughEnabled(currentFont.strikeOut());
    dialog.setLetterSpacing(currentFont.letterSpacing() / 100.0 - 1.0);
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

        setNumberFont(newFont);
        setNumberFontColor(newFontColor);
        setNumberFontBackgroundColor(newFontBackgroundColor);

        bool changeDetected = false;
        if (newFont != currentFont) {
            emit numberFontChanged(newFont);
            changeDetected = true;
        }

        if (newFontColor != fontColor) {
            emit numberFontColorChanged(newFontColor);
            changeDetected = true;
        }

        if (newFontBackgroundColor != fontBackgroundColor) {
            emit numberFontBackgroundColorChanged(newFontBackgroundColor);
            changeDetected = true;
        }

        if (changeDetected) {
            updateChangedStatus();
        }
    }
}


void PlotAxisSettingsWidget::populateAxisScalingComboBox(
        QComboBox*                                     comboBox,
        const QSet<PlotAxisSettingsWidget::AxisScale>& supportedAxisScales
    ) {
    struct AxisScalingData {
        QString   text;
        AxisScale scaleMode;
    };

    const AxisScalingData axisScalingData[] = {
        {
            tr("Linear"),
            AxisScale::LINEAR
        },
        {
            tr("log%1%2 (Log Scale)").arg(QChar(0x2081)).arg(QChar(0x2080)),
            AxisScale::LOG
        },
        {
            tr("10%1log%2%3 (dB Scale)").arg(QChar(0x2009)).arg(QChar(0x2081)).arg(QChar(0x2080)),
            AxisScale::DB_10_LOG
        },
        {
            tr("20%1log%2%3 (dB Scale)").arg(QChar(0x2009)).arg(QChar(0x2081)).arg(QChar(0x2080)),
            AxisScale::DB_20_LOG
        },
    };
    unsigned numberScalingModes = sizeof(axisScalingData) / sizeof(AxisScalingData);

    AxisScale currentSelection = AxisScale::LINEAR;
    unsigned  currentIndex     = 0;

    if (comboBox->currentIndex() >= 0) {
        currentSelection = comboBox->currentData().value<AxisScale>();
    }

    comboBox->clear();

    for (unsigned i=0 ; i<numberScalingModes ; ++i) {
        const AxisScalingData& scalingData = axisScalingData[i];
        if (supportedAxisScales.contains(scalingData.scaleMode)) {
            comboBox->addItem(scalingData.text, QVariant::fromValue(scalingData.scaleMode));

            if (scalingData.scaleMode == currentSelection) {
                currentIndex = i;
            }
        }
    }

    comboBox->setCurrentIndex(currentIndex);
}


void PlotAxisSettingsWidget::populateTickMarkStyleComboBox(QComboBox* comboBox) {
    comboBox->addItem(tr("No Tick Marks or Grid Lines"), QVariant::fromValue(TickStyle::NONE));
    comboBox->addItem(tr("Outer Tick Mark"), QVariant::fromValue(TickStyle::OUTER));
//    comboBox->addItem(tr("Inner Tick Mark"), QVariant::fromValue(TickStyle::INNER));
//    comboBox->addItem(tr("Outer + Inner Tick Mark"), QVariant::fromValue(TickStyle::SPANNING));
//    comboBox->addItem(tr("Grid Lines"), QVariant::fromValue(TickStyle::LINE));
    comboBox->addItem(tr("Outer Tick Mark + Grid Lines"), QVariant::fromValue(TickStyle::LINE_AND_TICK));
}
