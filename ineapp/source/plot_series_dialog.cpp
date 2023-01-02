/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotSeriesDialog class.
***********************************************************************************************************************/

#include <QDialog>
#include <QSize>
#include <QRect>
#include <QColor>
#include <QVariant>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTimer>
#include <QPainter>

#include <eqt_color_dialog.h>
#include <eqt_color_tool_button.h>

#include "variable_name_fields_widget.h"
#include "variable_name_special_characters_widget.h"

#include "application.h"
#include "metatypes.h"
#include "plot_series_demo_widget.h"
#include "plot_series_dialog.h"

PlotSeriesDialog::PlotSeriesDialog(QWidget* parent):QDialog(parent) {
    configureWidget();
}


PlotSeriesDialog::~PlotSeriesDialog() {}


PlotSeriesDialog::PlotSeriesMode PlotSeriesDialog::plotSeriesMode() const {
    return currentPlotSeriesMode;
}


PlotSeriesDialog::AxisLocationStyle PlotSeriesDialog::axisLocationStyle() const {
    return currentAxisLocationStyle;
}


unsigned PlotSeriesDialog::numberDataSources() const {
    return currentNumberDataSources;
}


QSet<PlotSeriesDialog::SplineType> PlotSeriesDialog::supportedSplineTypes() const {
    return inputSupportedSplineTypes;
}


QString PlotSeriesDialog::sourceText1(unsigned sourceIndex) const {
    return sourceFieldsWidgets.at(sourceIndex)->text1();
}


QString PlotSeriesDialog::sourceText2(unsigned sourceIndex) const {
    return sourceFieldsWidgets.at(sourceIndex)->text2();
}


Ld::VariableName PlotSeriesDialog::sourceVariableName(unsigned sourceIndex) const {
    return sourceFieldsWidgets.at(sourceIndex)->variableName();
}


PlotSeriesDialog::AxisLocation PlotSeriesDialog::sourceAxisLocation(unsigned sourceIndex) const {
    return sourceAxisLocationComboBoxes.at(sourceIndex)->currentData().value<AxisLocation>();
}


QFont PlotSeriesDialog::variableNameFont() const {
    return sourceFieldsWidgets.first()->variableNameFont();
}


QString PlotSeriesDialog::label() const {
    return labelLineEdit->text();
}


PlotSeriesDialog::LineStyle PlotSeriesDialog::lineStyle() const {
    LineStyle result;

    if (lineStyleComboBox->currentIndex() >= 0) {
        result = static_cast<LineStyle>(lineStyleComboBox->currentData().toInt());
    } else {
        result = LineStyle::SOLID_LINE;
    }

    return result;
}


float PlotSeriesDialog::lineWidth() const {
    return lineWidthSpinBox->value();
}


QColor PlotSeriesDialog::lineColor() const {
    return *colorToolButton->colors().begin();
}


PlotSeriesDialog::MarkerStyle PlotSeriesDialog::markerStyle() const {
    MarkerStyle result;

    if (markerStyleComboBox->currentIndex() >= 0) {
        result = static_cast<MarkerStyle>(markerStyleComboBox->currentData().toInt());
    } else {
        result = MarkerStyle::CIRCLE;
    }

    return result;
}


PlotSeriesDialog::SplineType PlotSeriesDialog::splineType() const {
    SplineType result;

    if (splineTypeComboBox->currentIndex() >= 0) {
        result = static_cast<SplineType>(splineTypeComboBox->currentData().toInt());
    } else {
        result = SplineType::LINE;
    }

    return result;
}


PlotSeriesDialog::GradientType PlotSeriesDialog::gradientType() const {
    GradientType result;

    if (gradientTypeComboBox->currentIndex() >= 0) {
        result = static_cast<GradientType>(gradientTypeComboBox->currentData().toInt());
    } else {
        result = GradientType::NONE;
    }

    return result;
}


bool PlotSeriesDialog::axisLocationControlsEnabled() const {
    return sourceAxisLocationComboBoxes.first()->isEnabled();
}


bool PlotSeriesDialog::axisLocationControlsDisabled() const {
    return !axisLocationControlsEnabled();
}


bool PlotSeriesDialog::seriesLabelFieldEnabled() const {
    return labelLineEdit->isEnabled();
}


bool PlotSeriesDialog::seriesLabelFieldDisabled() const {
    return !seriesLabelFieldEnabled();
}


bool PlotSeriesDialog::seriesVisibleAttributesEnabled() const {
    return plotSeriesDemoWidget->isEnabled();
}


bool PlotSeriesDialog::seriesVisibleAttributesDisabled() const {
    return !seriesVisibleAttributesEnabled();
}


void PlotSeriesDialog::setPlotSeriesMode(PlotSeriesDialog::PlotSeriesMode newPlotSeriesMode) {
    switch (newPlotSeriesMode) {
        case PlotSeriesMode::LINES_AND_MARKERS: {
            lineStyleComboBox->setVisible(true);
            lineWidthSpinBox->setVisible(true);
            gradientTypeComboBox->setVisible(false);
            rightArrowLabel->setVisible(true);
            leftArrowLabel->setVisible(true);
            splineTypeComboBox->setVisible(true);
            upArrowLabel->setVisible(true);
            markerStyleComboBox->setVisible(true);
            plotSeriesDemoWidget->setVisible(true);

            break;
        }

        case PlotSeriesMode::LINES: {
            lineStyleComboBox->setVisible(true);
            lineWidthSpinBox->setVisible(true);
            gradientTypeComboBox->setVisible(false);
            rightArrowLabel->setVisible(true);
            leftArrowLabel->setVisible(true);
            splineTypeComboBox->setVisible(true);
            upArrowLabel->setVisible(false);
            markerStyleComboBox->setVisible(false);
            plotSeriesDemoWidget->setVisible(true);

            break;
        }

        case PlotSeriesMode::MARKERS: {
            lineStyleComboBox->setVisible(false);
            lineWidthSpinBox->setVisible(false);
            gradientTypeComboBox->setVisible(false);
            rightArrowLabel->setVisible(false);
            leftArrowLabel->setVisible(false);
            splineTypeComboBox->setVisible(false);
            upArrowLabel->setVisible(false);
            markerStyleComboBox->setVisible(true);
            plotSeriesDemoWidget->setVisible(true);

            break;
        }

        case PlotSeriesMode::COLOR_REGION: {
            lineStyleComboBox->setVisible(false);
            lineWidthSpinBox->setVisible(false);
            gradientTypeComboBox->setVisible(false);
            rightArrowLabel->setVisible(false);
            leftArrowLabel->setVisible(false);
            splineTypeComboBox->setVisible(false);
            upArrowLabel->setVisible(false);
            markerStyleComboBox->setVisible(false);
            plotSeriesDemoWidget->setVisible(false);

            break;
        }

        case PlotSeriesMode::GRADIENT: {
            lineStyleComboBox->setVisible(false);
            lineWidthSpinBox->setVisible(false);
            gradientTypeComboBox->setVisible(true);
            rightArrowLabel->setVisible(false);
            leftArrowLabel->setVisible(true);
            splineTypeComboBox->setVisible(false);
            upArrowLabel->setVisible(false);
            markerStyleComboBox->setVisible(false);
            plotSeriesDemoWidget->setVisible(true);

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    currentPlotSeriesMode = newPlotSeriesMode;
    updateDemoWidget();
}


void PlotSeriesDialog::setAxisLocationStyle(PlotSeriesDialog::AxisLocationStyle newAxisLocationStyle) {
    currentAxisLocationStyle = newAxisLocationStyle;
    populateAxisLocationComboBoxes();
}


void PlotSeriesDialog::setAllowedAxisLocations(const QSet<AxisLocation>& allowedAxisLocations) {
    currentAllowedAxisLocations = allowedAxisLocations;
    populateAxisLocationComboBoxes();
}


void PlotSeriesDialog::setNumberDataSources(unsigned newNumberDataSources) {
   for (unsigned sourceIndex=0 ; sourceIndex<maximumNumberDataSources ; ++sourceIndex) {
       dataSourceGroupBoxes[sourceIndex]->setVisible(sourceIndex < newNumberDataSources);
   }

   if (newNumberDataSources < 2) {
       dataSourceGroupBoxes[0]->setTitle(tr("Data Source"));
   } else {
       dataSourceGroupBoxes[0]->setTitle(tr("Data Source 1"));
   }

   currentNumberDataSources = newNumberDataSources;
}


void PlotSeriesDialog::setSupportedSplineTypes(const QSet<PlotSeriesDialog::SplineType>& supportedSplineTypes) {
    inputSupportedSplineTypes = supportedSplineTypes;
    populateSplineTypeComboBox();
}


void PlotSeriesDialog::setSourceAxisLocation(unsigned sourceIndex, PlotSeriesDialog::AxisLocation newAxisLocation) {
    inputSourceAxisLocations[sourceIndex] = newAxisLocation;

    QComboBox* comboBox = sourceAxisLocationComboBoxes.at(sourceIndex);
    unsigned numberItems = static_cast<unsigned>(comboBox->count());
    unsigned i           = 0;

    while (i < numberItems && comboBox->itemData(i).value<AxisLocation>() != newAxisLocation) {
        ++i;
    }

    if (i < numberItems) {
        comboBox->setCurrentIndex(i);
    }

    updateOkButton();
}


void PlotSeriesDialog::setSourceText1(unsigned sourceIndex, const QString& newText) {
    inputSourceVariableNames[sourceIndex].setText1(newText);
    sourceFieldsWidgets.at(sourceIndex)->setText1(newText);

    updateOkButton();
}


void PlotSeriesDialog::setSourceText2(unsigned sourceIndex, const QString& newText) {
    inputSourceVariableNames[sourceIndex].setText2(newText);
    sourceFieldsWidgets.at(sourceIndex)->setText2(newText);

    updateOkButton();
}


void PlotSeriesDialog::setSourceVariableName(unsigned sourceIndex, const Ld::VariableName& newVariableName) {
    inputSourceVariableNames[sourceIndex] = newVariableName;
    sourceFieldsWidgets.at(sourceIndex)->setVariableName(newVariableName);

    updateOkButton();
}


void PlotSeriesDialog::setVariableNameFont(const QFont& newFont) {
    for (  QList<VariableNameFieldsWidget*>::iterator it = sourceFieldsWidgets.begin(), end = sourceFieldsWidgets.end()
         ; it != end
         ; ++it
        ) {
        (*it)->setVariableNameFont(newFont);
    }
}


void PlotSeriesDialog::setSeriesLabel(const QString& newText) {
    inputSeriesLabel = newText;
    labelLineEdit->setText(newText);
}


void PlotSeriesDialog::setLineStyle(PlotSeriesDialog::LineStyle newLineStyle) {
    inputLineStyle = newLineStyle;

    unsigned numberItems = static_cast<unsigned>(lineStyleComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && lineStyleComboBox->itemData(i).value<LineStyle>() != newLineStyle) {
        ++i;
    }

    if (i < numberItems) {
        lineStyleComboBox->setCurrentIndex(i);
    }

    updateDemoWidget();
}


void PlotSeriesDialog::setLineWidth(float newLineWidth) {
    inputLineWidth = newLineWidth;
    lineWidthSpinBox->setValue(newLineWidth);
    updateDemoWidget();
}


void PlotSeriesDialog::setLineColor(const QColor& newColor) {
    inputLineColor = newColor;
    colorToolButton->setColor(newColor);
    updateDemoWidget();
}


void PlotSeriesDialog::setMarkerStyle(PlotSeriesDialog::MarkerStyle newMarkerStyle) {
    inputMarkerStyle = newMarkerStyle;

    unsigned numberItems = static_cast<unsigned>(markerStyleComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && markerStyleComboBox->itemData(i).value<MarkerStyle>() != newMarkerStyle) {
        ++i;
    }

    if (i < numberItems) {
        markerStyleComboBox->setCurrentIndex(i);
    }

    updateDemoWidget();
}


void PlotSeriesDialog::setSplineType(PlotSeriesDialog::SplineType newSplineType) {
    inputSplineType = newSplineType;

    unsigned numberItems = static_cast<unsigned>(splineTypeComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && splineTypeComboBox->itemData(i).value<SplineType>() != newSplineType) {
        ++i;
    }

    if (i < numberItems) {
        splineTypeComboBox->setCurrentIndex(i);
    }

    updateDemoWidget();
}


void PlotSeriesDialog::setGradientType(PlotSeriesDialog::GradientType newGradientType) {
    inputGradientType = newGradientType;

    unsigned numberItems = static_cast<unsigned>(gradientTypeComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && gradientTypeComboBox->itemData(i).value<GradientType>() != newGradientType) {
        ++i;
    }

    if (i < numberItems) {
        gradientTypeComboBox->setCurrentIndex(i);
    }

    updateDemoWidget();
}


void PlotSeriesDialog::setAxisLocationControlsEnabled(bool nowEnabled) {
    for (  QList<QComboBox*>::iterator it  = sourceAxisLocationComboBoxes.begin(),
                                       end = sourceAxisLocationComboBoxes.end()
         ; it!=end
         ; ++it
        ) {
        (*it)->setEnabled(nowEnabled);
        (*it)->setVisible(nowEnabled);
    }
}


void PlotSeriesDialog::setAxisLocationControlsDisabled(bool nowDisabled) {
    setAxisLocationControlsEnabled(!nowDisabled);
}


void PlotSeriesDialog::setSeriesLabelFieldEnabled(bool nowEnabled) {
    labelLineEdit->setEnabled(nowEnabled);
    labelLineEdit->setVisible(nowEnabled);
    labelLineEditLabel->setVisible(nowEnabled);
}


void PlotSeriesDialog::setSeriesLabelFieldDisabled(bool nowDisabled) {
    setSeriesLabelFieldEnabled(!nowDisabled);
}


void PlotSeriesDialog::setSeriesVisibleAttributesEnabled(bool nowEnabled) {
    lineStyleComboBox->setEnabled(nowEnabled);
    lineWidthSpinBox->setEnabled(nowEnabled);
    gradientTypeComboBox->setEnabled(nowEnabled);
    rightArrowLabel->setEnabled(nowEnabled);
    plotSeriesDemoWidget->setEnabled(nowEnabled);
    splineTypeComboBox->setEnabled(nowEnabled);
    upArrowLabel->setEnabled(nowEnabled);
    markerStyleComboBox->setEnabled(nowEnabled);
    colorToolButton->setEnabled(nowEnabled);

    if (nowEnabled) {
        setPlotSeriesMode(currentPlotSeriesMode);
    } else {
        lineStyleComboBox->setVisible(false);
        lineWidthSpinBox->setVisible(false);
        gradientTypeComboBox->setVisible(false);
        rightArrowLabel->setVisible(false);
        plotSeriesDemoWidget->setVisible(false);
        splineTypeComboBox->setVisible(false);
        upArrowLabel->setVisible(false);
        markerStyleComboBox->setVisible(false);
        colorToolButton->setVisible(false);
   }
}


void PlotSeriesDialog::setSeriesVisibleAttributesDisabled(bool nowDisabled) {
    setSeriesVisibleAttributesEnabled(!nowDisabled);
}


void PlotSeriesDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    showTimer->start(1);
}


void PlotSeriesDialog::specialCharacterButtonClicked(const QString& character) {
    unsigned i=0;
    while (i < maximumNumberDataSources && !sourceFieldsWidgets.at(i)->hasFocus()) {
        ++i;
    }

    if (i < maximumNumberDataSources) {
        sourceFieldsWidgets[i]->insertText(character);
    }
}


void PlotSeriesDialog::variableNameFieldFocusChanged() {
    unsigned i=0;
    while (i < maximumNumberDataSources && !sourceFieldsWidgets.at(i)->hasFocus()) {
        ++i;
    }

    bool enableSpecialCharactersButton = i < maximumNumberDataSources;
    specialCharactersWidget->setEnabled(enableSpecialCharactersButton);
}


void PlotSeriesDialog::axisLocationChanged(int /* index */) {
    updateOkButton();
}


void PlotSeriesDialog::variableNameChanged(const QString& /* text1 */, const QString& /* text2 */) {
    updateOkButton();
}


void PlotSeriesDialog::colorToolButtonClicked() {
    EQt::ColorDialog colorDialog(*colorToolButton->colors().begin(), this);
    colorDialog.loadSettings();

    int result = colorDialog.exec();
    if (result == QColorDialog::DialogCode::Accepted) {
        QColor color = colorDialog.currentColor();

        colorToolButton->setColor(color);
        lineColorChanged(color);
    }

    colorDialog.saveSettings();
}


void PlotSeriesDialog::lineStyleChanged(int /* index */) {
    populateSplineTypeComboBox();
    updateDemoWidget();
    updateOkButton();
}


void PlotSeriesDialog::lineWidthChanged(double /* newValue */) {
    populateLineStyleComboBox();
    populateSplineTypeComboBox();
    populateMarkerStyleComboBox();

    updateDemoWidget();
    updateOkButton();
}


void PlotSeriesDialog::lineColorChanged(const QColor& /* newColor */) {
    populateLineStyleComboBox();
    populateSplineTypeComboBox();
    populateMarkerStyleComboBox();
    populateGradientTypeComboBox();

    updateDemoWidget();
    updateOkButton();
}


void PlotSeriesDialog::markerStyleChanged(int /* index */) {
    updateDemoWidget();
    updateOkButton();
}


void PlotSeriesDialog::splineStyleChanged(int /* index */) {
    populateLineStyleComboBox();
    updateDemoWidget();
    updateOkButton();
}


void PlotSeriesDialog::gradientTypeChanged(int /* index */) {
    updateDemoWidget();
    updateOkButton();
}


void PlotSeriesDialog::nowVisible() {
    if (currentPlotSeriesMode == PlotSeriesMode::GRADIENT) {
        plotSeriesDemoWidget->setFixedSize(gradientTypeComboBox->size());
    } else {
        plotSeriesDemoWidget->setFixedSize(markerStyleComboBox->size());
    }

    sourceFieldsWidgets.first()->setFocus();
}


void PlotSeriesDialog::configureWidget() {
    setWindowTitle("Plot Series");
    setSizeGripEnabled(false);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QGroupBox* sourcesGroupBox = new QGroupBox(tr("Data Sources"));
    mainLayout->addWidget(sourcesGroupBox);

    QVBoxLayout* sourcesVerticalLayout = new QVBoxLayout;
    sourcesGroupBox->setLayout(sourcesVerticalLayout);

    QHBoxLayout* sourcesHorizontalLayout = Q_NULLPTR;
    for (unsigned sourceIndex=0 ; sourceIndex<maximumNumberDataSources ; ++sourceIndex) {
        if ((sourceIndex %2) == 0) {
            sourcesHorizontalLayout = new QHBoxLayout;
            sourcesVerticalLayout->addLayout(sourcesHorizontalLayout);
        }

        QGroupBox* dataSourceGroupBox = new QGroupBox(tr("Data Source %1").arg(sourceIndex + 1));
        sourcesHorizontalLayout->addWidget(dataSourceGroupBox);

        QVBoxLayout* sourceVerticalLayout = new QVBoxLayout;
        dataSourceGroupBox->setLayout(sourceVerticalLayout);

        QComboBox* sourceAxisLocationComboBox = new QComboBox;
        sourceAxisLocationComboBox->setEditable(false);
        sourceVerticalLayout->addWidget(sourceAxisLocationComboBox, 0, Qt::AlignmentFlag::AlignLeft);

        VariableNameFieldsWidget* sourceFieldsWidget = new VariableNameFieldsWidget;
        sourceVerticalLayout->addWidget(sourceFieldsWidget, 0, Qt::AlignmentFlag::AlignLeft);

        inputSourceVariableNames.append(Ld::VariableName());
        inputSourceAxisLocations.append(AxisLocation::LEFT_Y_R_RC);
        dataSourceGroupBoxes.append(dataSourceGroupBox);
        sourceAxisLocationComboBoxes.append(sourceAxisLocationComboBox);
        sourceFieldsWidgets.append(sourceFieldsWidget);
    }

    specialCharactersWidget = new VariableNameSpecialCharactersWidget;
    sourcesVerticalLayout->addWidget(specialCharactersWidget, 0, Qt::AlignmentFlag::AlignCenter);

    QLabel* latexCommandLabel = new QLabel(tr("<b>Note:</b> Some LaTeX commands also accepted."));
    sourcesVerticalLayout->addWidget(latexCommandLabel, 0, Qt::AlignmentFlag::AlignCenter);

    QFormLayout* labelFormLayout = new QFormLayout;
    mainLayout->addLayout(labelFormLayout);


    labelLineEdit      = new QLineEdit;
    labelLineEditLabel = new QLabel(tr("Series Label: "));
    labelFormLayout->addRow(labelLineEditLabel, labelLineEdit);


    QHBoxLayout* plotSeriesHorizontalLayout = new QHBoxLayout;
    mainLayout->addLayout(plotSeriesHorizontalLayout);

    plotSeriesHorizontalLayout->addStretch(1);

    QGridLayout* plotSeriesGridLayout = new QGridLayout;
    plotSeriesHorizontalLayout->addLayout(plotSeriesGridLayout);

    plotSeriesHorizontalLayout->addStretch(1);

    lineStyleComboBox = new QComboBox;
    lineStyleComboBox->setEditable(false);
    plotSeriesGridLayout->addWidget(
        lineStyleComboBox,
        0, 0,
        Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter
    );

    lineWidthSpinBox = new QDoubleSpinBox;
    lineWidthSpinBox->setMinimum(0);
    lineWidthSpinBox->setStepType(QDoubleSpinBox::StepType::DefaultStepType);
    lineWidthSpinBox->setSingleStep(0.5);
    lineWidthSpinBox->setDecimals(1);
    plotSeriesGridLayout->addWidget(lineWidthSpinBox, 0, 1, Qt::AlignmentFlag::AlignCenter);

    rightArrowLabel = new QLabel(tr("%1").arg(QChar(0x2192)));
    plotSeriesGridLayout->addWidget(rightArrowLabel, 0, 2, Qt::AlignmentFlag::AlignCenter);

    plotSeriesDemoWidget = new PlotSeriesDemoWidget;
    plotSeriesDemoWidget->setFixedSize(10, 10); // Make the width a non-zero size for now.
    plotSeriesGridLayout->addWidget(plotSeriesDemoWidget, 0, 3, Qt::AlignmentFlag::AlignCenter);

    leftArrowLabel = new QLabel(tr("%1").arg(QChar(0x2190)));
    plotSeriesGridLayout->addWidget(leftArrowLabel, 0, 4, Qt::AlignmentFlag::AlignCenter);

    QHBoxLayout* rightHorizontalLayout = new QHBoxLayout;
    plotSeriesGridLayout->addLayout(
        rightHorizontalLayout,
        0, 5,
        Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter
    );

    splineTypeComboBox = new QComboBox;
    splineTypeComboBox->setEditable(false);
    rightHorizontalLayout->addWidget(
        splineTypeComboBox,
        Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter
    );

    gradientTypeComboBox = new QComboBox;
    gradientTypeComboBox->setEditable(false);
    rightHorizontalLayout->addWidget(
        gradientTypeComboBox,
        Qt::AlignmentFlag::AlignLeading | Qt::AlignmentFlag::AlignVCenter
    );

    upArrowLabel = new QLabel(tr("%1").arg(QChar(0x2191)));
    plotSeriesGridLayout->addWidget(upArrowLabel, 1, 3, Qt::AlignmentFlag::AlignCenter);

    markerStyleComboBox = new QComboBox;
    markerStyleComboBox->setEditable(false);
    plotSeriesGridLayout->addWidget(
        markerStyleComboBox,
        2, 3,
        Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignTop
    );

    colorToolButton = new EQt::ColorToolButton;
    plotSeriesGridLayout->addWidget(
        colorToolButton,
        2, 5,
        Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter
    );

    buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel);
    mainLayout->addWidget(buttonBox);

    buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);

    colorToolButton->setColor(QColor(Qt::black));

    showTimer = new QTimer(this);
    showTimer->setSingleShot(true);

    currentPlotSeriesMode    = PlotSeriesMode::LINES_AND_MARKERS;
    currentAxisLocationStyle = AxisLocationStyle::PLOT_BORDERS;
    currentNumberDataSources = 2;

    setAxisLocationStyle(currentAxisLocationStyle);
    setNumberDataSources(currentNumberDataSources);

    populateMarkerStyleComboBox(false);
    populateLineStyleComboBox(false);
    populateSplineTypeComboBox(false);
    populateGradientTypeComboBox(false);

    setSourceAxisLocation(0, AxisLocation::BOTTOM_X_A_GM);
    setSourceAxisLocation(1, AxisLocation::LEFT_Y_R_RC);
    setSourceAxisLocation(2, AxisLocation::Z_B_BK);
    for (unsigned sourceIndex=3 ; sourceIndex<maximumNumberDataSources ; ++sourceIndex) {
        setSourceAxisLocation(sourceIndex, AxisLocation::LEFT_Y_R_RC);
    }

    setLineStyle(Ld::PlotSeries::defaultLineStyle);
    setLineWidth(Ld::PlotSeries::defaultLineWidth);
    setSplineType(Ld::PlotSeries::defaultSplineType);
    setMarkerStyle(Ld::PlotSeries::defaultMarkerStyle);

    setPlotSeriesMode(PlotSeriesMode::LINES_AND_MARKERS);

    connectAll();
}


void PlotSeriesDialog::connectAll() {
    for (unsigned sourceIndex=0 ; sourceIndex<maximumNumberDataSources ; ++sourceIndex) {
        connect(
            sourceFieldsWidgets[sourceIndex],
            &VariableNameFieldsWidget::changed,
            this,
            &PlotSeriesDialog::variableNameChanged
        );
        connect(
            sourceFieldsWidgets[sourceIndex],
            &VariableNameFieldsWidget::receivedFocus,
            this,
            &PlotSeriesDialog::variableNameFieldFocusChanged
        );
        connect(
            sourceFieldsWidgets[sourceIndex],
            &VariableNameFieldsWidget::lostFocus,
            this,
            &PlotSeriesDialog::variableNameFieldFocusChanged
        );

        connect(
            sourceAxisLocationComboBoxes[sourceIndex],
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &PlotSeriesDialog::axisLocationChanged
        );
    }

    connect(
        specialCharactersWidget,
        &VariableNameSpecialCharactersWidget::selected,
        this,
        &PlotSeriesDialog::specialCharacterButtonClicked
    );

    connect(
        lineStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::lineStyleChanged
    );
    connect(
        lineWidthSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &PlotSeriesDialog::lineWidthChanged
    );
    connect(
        splineTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::splineStyleChanged
    );
    connect(
        gradientTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::gradientTypeChanged
    );
    connect(
        markerStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::markerStyleChanged
    );
    connect(colorToolButton, &EQt::ColorToolButton::clicked, this, &PlotSeriesDialog::colorToolButtonClicked);

    connect(showTimer, &QTimer::timeout, this, &PlotSeriesDialog::nowVisible);

    connect(
        buttonBox->button(QDialogButtonBox::StandardButton::Ok),
        &QPushButton::clicked,
        this,
        &PlotSeriesDialog::accept
    );
    connect(
        buttonBox->button(QDialogButtonBox::StandardButton::Cancel),
        &QPushButton::clicked,
        this,
        &PlotSeriesDialog::reject
    );
}


void PlotSeriesDialog::disconnectAll() {
    for (unsigned sourceIndex=0 ; sourceIndex<maximumNumberDataSources ; ++sourceIndex) {
        disconnect(
            sourceFieldsWidgets[sourceIndex],
            &VariableNameFieldsWidget::changed,
            this,
            &PlotSeriesDialog::variableNameChanged
        );
        disconnect(
            sourceFieldsWidgets[sourceIndex],
            &VariableNameFieldsWidget::receivedFocus,
            this,
            &PlotSeriesDialog::variableNameFieldFocusChanged
        );
        disconnect(
            sourceFieldsWidgets[sourceIndex],
            &VariableNameFieldsWidget::lostFocus,
            this,
            &PlotSeriesDialog::variableNameFieldFocusChanged
        );

        disconnect(
            sourceAxisLocationComboBoxes[sourceIndex],
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            &PlotSeriesDialog::axisLocationChanged
        );
    }

    disconnect(
        specialCharactersWidget,
        &VariableNameSpecialCharactersWidget::selected,
        this,
        &PlotSeriesDialog::specialCharacterButtonClicked
    );

    disconnect(
        lineStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::lineStyleChanged
    );
    disconnect(
        lineWidthSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &PlotSeriesDialog::lineWidthChanged
    );
    disconnect(
        splineTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::splineStyleChanged
    );
    disconnect(
        gradientTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::gradientTypeChanged
    );
    disconnect(
        markerStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesDialog::markerStyleChanged
    );
    disconnect(colorToolButton, &EQt::ColorToolButton::clicked, this, &PlotSeriesDialog::colorToolButtonClicked);

    disconnect(showTimer, &QTimer::timeout, this, &PlotSeriesDialog::nowVisible);

    disconnect(
        buttonBox->button(QDialogButtonBox::StandardButton::Ok),
        &QPushButton::clicked,
        this,
        &PlotSeriesDialog::accept
    );
    disconnect(
        buttonBox->button(QDialogButtonBox::StandardButton::Cancel),
        &QPushButton::clicked,
        this,
        &PlotSeriesDialog::reject
    );
}


void PlotSeriesDialog::updateDemoWidget() {
    LineStyle    lineStyle   = LineStyle::NO_LINE;
    float        lineWidth   = 0;
    QColor       lineColor   = QColor(Qt::GlobalColor::black);
    SplineType   splineType  = SplineType::NONE;
    MarkerStyle  markerStyle = MarkerStyle::NONE;
    GradientType gradientType = GradientType::NONE;

    switch (currentPlotSeriesMode) {
        case PlotSeriesMode::LINES: {
            lineStyle    = PlotSeriesDialog::lineStyle();
            lineWidth    = PlotSeriesDialog::lineWidth();
            lineColor    = PlotSeriesDialog::lineColor();
            splineType   = PlotSeriesDialog::splineType();
            markerStyle  = MarkerStyle::NONE;
            gradientType = GradientType::NONE;

            break;
        }

        case PlotSeriesMode::MARKERS: {
            lineStyle    = LineStyle::NO_LINE;
            lineWidth    = 1.0F;
            lineColor    = PlotSeriesDialog::lineColor();
            splineType   = SplineType::NONE;
            markerStyle  = PlotSeriesDialog::markerStyle();
            gradientType = GradientType::NONE;

            break;
        }

        case PlotSeriesMode::LINES_AND_MARKERS: {
            lineStyle    = PlotSeriesDialog::lineStyle();
            lineWidth    = PlotSeriesDialog::lineWidth();
            lineColor    = PlotSeriesDialog::lineColor();
            splineType   = PlotSeriesDialog::splineType();
            markerStyle  = PlotSeriesDialog::markerStyle();
            gradientType = GradientType::NONE;

            break;
        }

        case PlotSeriesMode::COLOR_REGION: {
            lineStyle    = LineStyle::NO_LINE;
            lineWidth    = 0;
            lineColor    = PlotSeriesDialog::lineColor();
            splineType   = SplineType::NONE;
            markerStyle  = MarkerStyle::NONE;
            gradientType = GradientType::NONE;

            break;
        }

        case PlotSeriesMode::GRADIENT: {
            lineStyle    = LineStyle::NO_LINE;
            lineWidth    = 0;
            lineColor    = PlotSeriesDialog::lineColor();
            splineType   = SplineType::NONE;
            markerStyle  = MarkerStyle::NONE;
            gradientType = PlotSeriesDialog::gradientType();

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    plotSeriesDemoWidget->setLineStyle(lineStyle);
    plotSeriesDemoWidget->setLineWidth(lineWidth);
    plotSeriesDemoWidget->setLineColor(lineColor);
    plotSeriesDemoWidget->setSplineType(splineType);
    plotSeriesDemoWidget->setMarkerStyle(markerStyle);
    plotSeriesDemoWidget->setGradientType(gradientType);
}


QSet<PlotSeriesDialog::AxisLocation> PlotSeriesDialog::incompatibleAxis(
        PlotSeriesDialog::AxisLocationStyle axisLocationStyle,
        PlotSeriesDialog::AxisLocation      axisLocation
    ) {
    QSet<AxisLocation> result;

    switch (axisLocationStyle) {
        case AxisLocationStyle::CATEGORIZED:
        case AxisLocationStyle::PLOT_BORDERS: {
            switch (axisLocation) {
                case AxisLocation::TOP_X_A_GM: {
                    result << AxisLocation::BOTTOM_X_A_GM << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::BOTTOM_X_A_GM: {
                    result << AxisLocation::TOP_X_A_GM << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::LEFT_Y_R_RC: {
                    result << AxisLocation::RIGHT_Y_R_RY << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::RIGHT_Y_R_RY: {
                    result << AxisLocation::LEFT_Y_R_RC << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::Z_B_BK: {
                    result << AxisLocation::LEFT_Y_R_RC << AxisLocation::RIGHT_Y_R_RY
                           << AxisLocation::BOTTOM_X_A_GM << AxisLocation::TOP_X_A_GM;
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case AxisLocationStyle::UNCATEGORIZED:
        case AxisLocationStyle::FIXED: {
            // Uncategorized such as pie charts don't require axis locations.
            break;
        }

        case AxisLocationStyle::CARTESIAN_2D: {
            switch (axisLocation) {
                case AxisLocation::TOP_X_A_GM: {
                    result << AxisLocation::BOTTOM_X_A_GM << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::BOTTOM_X_A_GM: {
                    result << AxisLocation::TOP_X_A_GM << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::LEFT_Y_R_RC: {
                    result << AxisLocation::RIGHT_Y_R_RY << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::RIGHT_Y_R_RY: {
                    result << AxisLocation::LEFT_Y_R_RC << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::Z_B_BK: {
                    result << AxisLocation::LEFT_Y_R_RC << AxisLocation::RIGHT_Y_R_RY
                           << AxisLocation::BOTTOM_X_A_GM << AxisLocation::TOP_X_A_GM;
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case AxisLocationStyle::CARTESIAN_3D: {
            switch (axisLocation) {
                case AxisLocation::TOP_X_A_GM: {
                    result << AxisLocation::BOTTOM_X_A_GM;
                    break;
                }

                case AxisLocation::BOTTOM_X_A_GM: {
                    result << AxisLocation::TOP_X_A_GM;
                    break;
                }

                case AxisLocation::LEFT_Y_R_RC: {
                    result << AxisLocation::RIGHT_Y_R_RY;
                    break;
                }

                case AxisLocation::RIGHT_Y_R_RY: {
                    result << AxisLocation::LEFT_Y_R_RC;
                    break;
                }

                case AxisLocation::Z_B_BK: {
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case AxisLocationStyle::POLAR_2D: {
            switch (axisLocation) {
                case AxisLocation::TOP_X_A_GM: {
                    result << AxisLocation::BOTTOM_X_A_GM << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::BOTTOM_X_A_GM: {
                    result << AxisLocation::TOP_X_A_GM << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::LEFT_Y_R_RC: {
                    result << AxisLocation::RIGHT_Y_R_RY << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::RIGHT_Y_R_RY: {
                    result << AxisLocation::LEFT_Y_R_RC << AxisLocation::Z_B_BK;
                    break;
                }

                case AxisLocation::Z_B_BK: {
                    result << AxisLocation::LEFT_Y_R_RC << AxisLocation::RIGHT_Y_R_RY
                           << AxisLocation::BOTTOM_X_A_GM << AxisLocation::TOP_X_A_GM;
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case AxisLocationStyle::POLAR_3D: {
            switch (axisLocation) {
                case AxisLocation::TOP_X_A_GM: {
                    result << AxisLocation::BOTTOM_X_A_GM;
                    break;
                }

                case AxisLocation::BOTTOM_X_A_GM: {
                    result << AxisLocation::TOP_X_A_GM;
                    break;
                }

                case AxisLocation::LEFT_Y_R_RC: {
                    result << AxisLocation::RIGHT_Y_R_RY;
                    break;
                }

                case AxisLocation::RIGHT_Y_R_RY: {
                    result << AxisLocation::LEFT_Y_R_RC;
                    break;
                }

                case AxisLocation::Z_B_BK: {
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case AxisLocationStyle::RGB_COLOR_SPACE: {
            switch (axisLocation) {
                case AxisLocation::LEFT_Y_R_RC: {
                    result << AxisLocation::RIGHT_Y_R_RY;
                    break;
                }

                case AxisLocation::RIGHT_Y_R_RY: {
                    result << AxisLocation::LEFT_Y_R_RC;
                    break;
                }

                case AxisLocation::TOP_X_A_GM: {
                    result << AxisLocation::BOTTOM_X_A_GM;
                    break;
                }

                case AxisLocation::BOTTOM_X_A_GM: {
                    result << AxisLocation::TOP_X_A_GM;
                    break;
                }

                case AxisLocation::Z_B_BK: {
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case AxisLocationStyle::CMYK_COLOR_SPACE: {
            switch (axisLocation) {
                case AxisLocation::LEFT_Y_R_RC: {
                    break;
                }

                case AxisLocation::RIGHT_Y_R_RY: {
                    break;
                }

                case AxisLocation::TOP_X_A_GM: {
                    result << AxisLocation::BOTTOM_X_A_GM;
                    break;
                }

                case AxisLocation::BOTTOM_X_A_GM: {
                    result << AxisLocation::TOP_X_A_GM;
                    break;
                }

                case AxisLocation::Z_B_BK: {
                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            break;
        }

        case AxisLocationStyle::GRAYSCALE_COLOR_SPACE: {
            result << AxisLocation::LEFT_Y_R_RC
                   << AxisLocation::BOTTOM_X_A_GM
                   << AxisLocation::RIGHT_Y_R_RY
                   << AxisLocation::TOP_X_A_GM
                   << AxisLocation::Z_B_BK;

            result.remove(axisLocation);

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


void PlotSeriesDialog::populateAxisLocationComboBox(
        QComboBox*                                  comboBox,
        unsigned                                    sourceIndex,
        PlotSeriesDialog::AxisLocationStyle         axisLocationStyle,
        const QSet<PlotSeriesDialog::AxisLocation>& allowedAxisLocations
    ) {
    comboBox->clear();

    switch (axisLocationStyle) {
        case AxisLocationStyle::PLOT_BORDERS: {
            if ((sourceIndex %2) == 0) {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                    comboBox->addItem(tr("Bottom"), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
                }

                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::TOP_X_A_GM)) {
                    comboBox->addItem(tr("Top"),    QVariant::fromValue(AxisLocation::TOP_X_A_GM));
                }
            } else {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                    comboBox->addItem(tr("Left"),   QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
                }

                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::RIGHT_Y_R_RY)) {
                    comboBox->addItem(tr("Right"),  QVariant::fromValue(AxisLocation::RIGHT_Y_R_RY));
                }
            }

            break;
        }

        case AxisLocationStyle::CATEGORIZED: {
            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                comboBox->addItem(tr("Bottom"), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::TOP_X_A_GM)) {
                comboBox->addItem(tr("Top"),    QVariant::fromValue(AxisLocation::TOP_X_A_GM));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                comboBox->addItem(tr("Left"),   QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::RIGHT_Y_R_RY)) {
                comboBox->addItem(tr("Right"),  QVariant::fromValue(AxisLocation::RIGHT_Y_R_RY));
            }

            break;
        }

        case AxisLocationStyle::UNCATEGORIZED:
        case AxisLocationStyle::FIXED: {
            break;
        }

        case AxisLocationStyle::CARTESIAN_2D: {
            if ((sourceIndex %2) == 0) {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                    comboBox->addItem(tr("x"), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
                }
            } else {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                    comboBox->addItem(tr("y"), QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
                }
            }

            break;
        }

        case AxisLocationStyle::CARTESIAN_3D: {
            if ((sourceIndex % 3) == 0) {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                    comboBox->addItem(tr("x"), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
                }
            } else if ((sourceIndex %3) == 1) {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                    comboBox->addItem(tr("y"), QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
                }
            } else {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::Z_B_BK)) {
                    comboBox->addItem(tr("z"), QVariant::fromValue(AxisLocation::Z_B_BK));
                }
            }

            break;
        }

        case AxisLocationStyle::POLAR_2D: {
            if ((sourceIndex %2) == 0) {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                    comboBox->addItem(tr("%1").arg(QChar(0x03B8)), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
                }
            } else {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                    comboBox->addItem(tr("r"), QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
                }
            }

            break;
        }

        case AxisLocationStyle::POLAR_3D: {
            if ((sourceIndex % 3) == 0) {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                    comboBox->addItem(tr("r"), QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
                }
            } else if ((sourceIndex %3) == 1) {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                    comboBox->addItem(tr("%1").arg(QChar(0x03B8)), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
                }
            } else {
                if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::Z_B_BK)) {
                    comboBox->addItem(tr("%1").arg(QChar(0x03C6)), QVariant::fromValue(AxisLocation::Z_B_BK));
                }
            }

            break;
        }

        case AxisLocationStyle::RGB_COLOR_SPACE: {
            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                comboBox->addItem(tr("Red"), QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                comboBox->addItem(tr("Green"), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::Z_B_BK)) {
                comboBox->addItem(tr("Blue"),  QVariant::fromValue(AxisLocation::Z_B_BK));
            }

            break;
        }

        case AxisLocationStyle::CMYK_COLOR_SPACE: {
            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                comboBox->addItem(tr("Cyan"), QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::BOTTOM_X_A_GM)) {
                comboBox->addItem(tr("Magenta"), QVariant::fromValue(AxisLocation::BOTTOM_X_A_GM));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::RIGHT_Y_R_RY)) {
                comboBox->addItem(tr("Yellow"), QVariant::fromValue(AxisLocation::RIGHT_Y_R_RY));
            }

            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::Z_B_BK)) {
                comboBox->addItem(tr("Black"), QVariant::fromValue(AxisLocation::Z_B_BK));
            }

            break;
        }

        case AxisLocationStyle::GRAYSCALE_COLOR_SPACE: {
            if (allowedAxisLocations.isEmpty() || allowedAxisLocations.contains(AxisLocation::LEFT_Y_R_RC)) {
                comboBox->addItem(tr("Gray"), QVariant::fromValue(AxisLocation::LEFT_Y_R_RC));
            }

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


void PlotSeriesDialog::populateAxisLocationComboBoxes() {
    unsigned numberSourceAxisLocationComboBoxes = static_cast<unsigned>(sourceAxisLocationComboBoxes.size());
    for (unsigned sourceIndex=0 ; sourceIndex<numberSourceAxisLocationComboBoxes ; ++sourceIndex) {
        QComboBox* comboBox = sourceAxisLocationComboBoxes.at(sourceIndex);
        populateAxisLocationComboBox(comboBox, sourceIndex, currentAxisLocationStyle, currentAllowedAxisLocations);
    }
}


void PlotSeriesDialog::populateLineStyleComboBox(bool manageSignals) {
    if (manageSignals) {
        disconnectAll();
    }

    QSize    physicalDpi      = Application::physicalDpi();
    float    widthScaleFactor = static_cast<float>(physicalDpi.width()) / static_cast<float>(physicalDpi.height());
    unsigned height           = QFontMetrics(Application::font()).height();
    unsigned width            = static_cast<unsigned>(3 * height * widthScaleFactor + 0.5);

    struct LineStyleData {
        QString   description;
        LineStyle style;
    };

    const LineStyleData lineStyles[] = {
        { tr("No Line"),            LineStyle::NO_LINE                 },
        { tr("Solid"),              LineStyle::SOLID_LINE              },
        { tr("Dashed"),             LineStyle::DASHED_LINE             },
        { tr("Dotted"),             LineStyle::DOTTED_LINE             },
        { tr("Dash-Dotted"),        LineStyle::DASH_DOTTED_LINE        },
        { tr("Dash-Dotted-Dotted"), LineStyle::DASH_DOTTED_DOTTED_LINE }
    };
    const unsigned numberLineStyles = sizeof(lineStyles) / sizeof(LineStyleData);

    unsigned  currentIndex     = 0;
    LineStyle currentLineStyle = lineStyle();

    lineStyleComboBox->clear();

    for (unsigned i=0 ; i<numberLineStyles ; ++i) {
        const LineStyleData& lineStyleData = lineStyles[i];

        QRect    boundingRectangle(0, 0, width, height);
        QPixmap  pixmap(boundingRectangle.size());

        pixmap.fill(QColor(Qt::GlobalColor::white));

        QPainter painter(&pixmap);
        PlotSeriesDemoWidget::paint(
            &painter,
            boundingRectangle,
            lineStyleData.style,
            lineWidth(),
            lineColor(),
            MarkerStyle::NONE,
            splineType(),
            GradientType::NONE
        );

        lineStyleComboBox->addItem(
            QIcon(pixmap),
            lineStyleData.description,
            QVariant(static_cast<int>(lineStyleData.style))
        );

        if (currentLineStyle == lineStyleData.style) {
            currentIndex = i;
        }
    }

    lineStyleComboBox->setCurrentIndex(currentIndex);

    if (manageSignals) {
        connectAll();
    }
}


void PlotSeriesDialog::populateSplineTypeComboBox(bool manageSignals) {
    if (manageSignals) {
        disconnectAll();
    }

    QSize    physicalDpi      = Application::physicalDpi();
    float    widthScaleFactor = static_cast<float>(physicalDpi.width()) / static_cast<float>(physicalDpi.height());
    unsigned height           = QFontMetrics(Application::font()).height();
    unsigned width            = static_cast<unsigned>(3 * height * widthScaleFactor + 0.5);

    struct SplineStyleData {
        QString    description;
        SplineType style;
    };

    const SplineStyleData splineStyles[] = {
        { tr("No line"),            SplineType::NONE              },
        { tr("Straight Lines"),     SplineType::LINE              },
        { tr("Spline"),             SplineType::SPLINE            },
        { tr("Linear Regression"),  SplineType::LINEAR_REGRESSION }
    };
    const unsigned numberSplineStyles = sizeof(splineStyles) / sizeof(SplineStyleData);

    unsigned   currentIndex      = 0;
    SplineType currentSplineType = splineType();

    splineTypeComboBox->clear();

    for (unsigned i=0 ; i<numberSplineStyles ; ++i) {
        const SplineStyleData& splineStyleData = splineStyles[i];

        if (inputSupportedSplineTypes.contains(splineStyleData.style)) {
            QRect    boundingRectangle(0, 0, width, height);
            QPixmap  pixmap(boundingRectangle.size());

            pixmap.fill(QColor(Qt::GlobalColor::white));

            QPainter painter(&pixmap);
            PlotSeriesDemoWidget::paint(
                &painter,
                boundingRectangle,
                lineStyle(),
                lineWidth(),
                lineColor(),
                MarkerStyle::NONE,
                splineStyleData.style,
                GradientType::NONE
            );
            painter.end();

            splineTypeComboBox->addItem(
                QIcon(pixmap),
                splineStyleData.description,
                QVariant(static_cast<int>(splineStyleData.style))
            );

            if (currentSplineType == splineStyleData.style) {
                currentIndex = i;
            }
        }
    }

    splineTypeComboBox->setCurrentIndex(currentIndex);

    if (manageSignals) {
        connectAll();
    }
}


void PlotSeriesDialog::populateGradientTypeComboBox(bool manageSignals) {
    if (manageSignals) {
        disconnectAll();
    }

    QSize    physicalDpi      = Application::physicalDpi();
    float    widthScaleFactor = static_cast<float>(physicalDpi.width()) / static_cast<float>(physicalDpi.height());
    unsigned height           = QFontMetrics(Application::font()).height();
    unsigned width            = static_cast<unsigned>(height * widthScaleFactor + 0.5);

    struct GradientTypeData {
        QString      description;
        GradientType gradientType;
    };

    const GradientTypeData gradientTypes[] = {
        { tr("Intensity"),                      GradientType::INTENSITY },
        { tr("Hue + 60%1").arg(QChar(0x00BA)),  GradientType::HUE_POSITIVE_60 },
        { tr("Hue + 120%1").arg(QChar(0x00BA)), GradientType::HUE_POSITIVE_120 },
        { tr("Hue + 180%1").arg(QChar(0x00BA)), GradientType::HUE_POSITIVE_180 },
        { tr("Hue + 240%1").arg(QChar(0x00BA)), GradientType::HUE_POSITIVE_240 },
        { tr("Hue + 300%1").arg(QChar(0x00BA)), GradientType::HUE_POSITIVE_300 },
        { tr("Hue - 60%1").arg(QChar(0x00BA)),  GradientType::HUE_NEGATIVE_60 },
        { tr("Hue - 120%1").arg(QChar(0x00BA)), GradientType::HUE_NEGATIVE_120 },
        { tr("Hue - 180%1").arg(QChar(0x00BA)), GradientType::HUE_NEGATIVE_180 },
        { tr("Hue - 240%1").arg(QChar(0x00BA)), GradientType::HUE_NEGATIVE_240 },
        { tr("Hue - 300%1").arg(QChar(0x00BA)), GradientType::HUE_NEGATIVE_300 }
    };
    const unsigned numberGradientTypes = sizeof(gradientTypes) / sizeof(GradientTypeData);

    unsigned     currentIndex        = 0;
    GradientType currentGradientType = gradientType();

    gradientTypeComboBox->clear();

    for (unsigned i=0 ; i<numberGradientTypes ; ++i) {
        const GradientTypeData& gradientTypeData = gradientTypes[i];

        QRect    boundingRectangle(0, 0, width, height);
        QPixmap  pixmap(boundingRectangle.size());

        pixmap.fill(QColor(Qt::GlobalColor::white));

        QPainter painter(&pixmap);
        PlotSeriesDemoWidget::paint(
            &painter,
            boundingRectangle,
            LineStyle::NO_LINE,
            0,
            lineColor(),
            MarkerStyle::NONE,
            SplineType::NONE,
            gradientTypeData.gradientType
        );

        gradientTypeComboBox->addItem(
            QIcon(pixmap),
            gradientTypeData.description,
            QVariant(static_cast<int>(gradientTypeData.gradientType))
        );

        if (currentGradientType == gradientTypeData.gradientType) {
            currentIndex = i;
        }
    }

    gradientTypeComboBox->setCurrentIndex(currentIndex);

    if (manageSignals) {
        connectAll();
    }
}


void PlotSeriesDialog::populateMarkerStyleComboBox(bool manageSignals) {
    if (manageSignals) {
        disconnectAll();
    }

    QSize    physicalDpi      = Application::physicalDpi();
    float    widthScaleFactor = static_cast<float>(physicalDpi.width()) / static_cast<float>(physicalDpi.height());
    unsigned height           = QFontMetrics(Application::font()).height();
    unsigned width            = static_cast<unsigned>(height * widthScaleFactor + 0.5);

    struct MarkerStyleData {
        QString     description;
        MarkerStyle style;
    };

    const MarkerStyleData markerStyles[] = {
        { tr("No Marker"), MarkerStyle::NONE    },
        { tr("Dot"),       MarkerStyle::DOT     },
        { tr("Circle"),    MarkerStyle::CIRCLE  },
        { tr("Square"),    MarkerStyle::SQUARE  },
        { tr("Diamond"),   MarkerStyle::DIAMOND },
        { tr("Star"),      MarkerStyle::STAR    },
        { tr("X"),         MarkerStyle::X       }
    };
    const unsigned numberMarkerStyles = sizeof(markerStyles) / sizeof(MarkerStyleData);

    unsigned    currentIndex       = 0;
    MarkerStyle currentMarkerStyle = markerStyle();

    markerStyleComboBox->clear();

    for (unsigned i=0 ; i<numberMarkerStyles ; ++i) {
        const MarkerStyleData& markerStyleData = markerStyles[i];

        QRect    boundingRectangle(0, 0, width, height);
        QPixmap  pixmap(boundingRectangle.size());

        pixmap.fill(QColor(Qt::GlobalColor::white));

        QPainter painter(&pixmap);
        PlotSeriesDemoWidget::paint(
            &painter,
            boundingRectangle,
            LineStyle::NO_LINE,
            lineWidth(),
            lineColor(),
            markerStyleData.style,
            SplineType::NONE,
            GradientType::NONE
        );

        markerStyleComboBox->addItem(
            QIcon(pixmap),
            markerStyleData.description,
            QVariant(static_cast<int>(markerStyleData.style))
        );

        if (currentMarkerStyle == markerStyleData.style) {
            currentIndex = i;
        }
    }

    markerStyleComboBox->setCurrentIndex(currentIndex);

    if (manageSignals) {
        connectAll();
    }
}


void PlotSeriesDialog::updateOkButton() {
    bool               contentsOk   = true;
    bool               valueChanged = false;
    QSet<AxisLocation> usedAxisLocations;
    unsigned           sourceIndex = 0;

    QSet<AxisLocation> currentAxisLocations;
    while (sourceIndex < currentNumberDataSources && contentsOk) {
        AxisLocation axisLocation = sourceAxisLocation(sourceIndex);
        contentsOk = (
               !currentAxisLocations.contains(axisLocation)
            && !currentAxisLocations.intersects(incompatibleAxis(currentAxisLocationStyle, axisLocation))
            && sourceFieldsWidgets.at(sourceIndex)->isValid()
        );

        currentAxisLocations.insert(axisLocation);

        if (contentsOk && !valueChanged) {
            valueChanged = (
                   axisLocation != inputSourceAxisLocations.at(sourceIndex)
                || sourceFieldsWidgets.at(sourceIndex)->variableName() != inputSourceVariableNames.at(sourceIndex)
            );
        }

        ++sourceIndex;
    }

    if (contentsOk && !valueChanged) {
        valueChanged = (
               inputSeriesLabel != label()
            || inputLineStyle != lineStyle()
            || inputLineWidth != lineWidth()
            || inputLineColor != lineColor()
            || inputMarkerStyle != markerStyle()
            || inputSplineType != splineType()
            || inputGradientType != gradientType()
        );
    }

    buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(contentsOk && valueChanged);
}
