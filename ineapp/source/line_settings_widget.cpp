/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LineSettingsWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

#include <util_hash_functions.h>

#include "eqt_line_sample_widget.h"

#include "line_settings_widget.h"

LineSettingsWidget::LineSettingsWidget(QWidget* parent):QWidget(parent) {
    configureWidget();
    setLineOrientation(LineOrientation::HORIZONTAL);
}


LineSettingsWidget::LineSettingsWidget(LineOrientation lineOrientation, QWidget* parent):QWidget(parent) {
    configureWidget();
    setLineOrientation(lineOrientation);
}


LineSettingsWidget::~LineSettingsWidget() {}


LineSettingsWidget::LineOrientation LineSettingsWidget::lineOrientation() const {
    return singleLineSampleWidget->orientation();
}


QSet<LineSettingsWidget::LineStyle> LineSettingsWidget::enabledLineStyles() const {
    QSet<LineStyle> enabledStyles;

    if (noLineRadioButton->isVisible()) {
        enabledStyles << LineStyle::NO_LINE;
    }

    if (singleLineRadioButton->isVisible()) {
        enabledStyles << LineStyle::SINGLE;
    }

    if (doubleLineRadioButton->isVisible()) {
        enabledStyles << LineStyle::DOUBLE;
    }

    return enabledStyles;
}


LineSettingsWidget::LineStyle LineSettingsWidget::lineStyle() const {
    LineStyle result;

    bool noLineStyle     = noLineRadioButton->isChecked();
    bool singleLineStyle = singleLineRadioButton->isChecked();
    bool doubleLineStyle = doubleLineRadioButton->isChecked();

    if        ( noLineStyle && !singleLineStyle && !doubleLineStyle) {
        result = LineStyle::NO_LINE;
    } else if (!noLineStyle &&  singleLineStyle && !doubleLineStyle) {
        result = LineStyle::SINGLE;
    } else if (!noLineStyle && !singleLineStyle &&  doubleLineStyle) {
        result = LineStyle::DOUBLE;
    } else {
        result = LineStyle::INVALID;
    }

    return result;
}


float LineSettingsWidget::lineWidth() const {
    float result = static_cast<float>(lineWidthSpinBox->value());
    return result;
}


void LineSettingsWidget::setLineOrientation(LineSettingsWidget::LineOrientation newLineOrientation) {
    singleLineSampleWidget->setOrientation(newLineOrientation);
    doubleLineSampleWidget->setOrientation(newLineOrientation);

    if (newLineOrientation == LineOrientation::HORIZONTAL) {
        noLineRadioButton->setToolTip(tr("The table will default to having no horizontal lines between rows."));
        noLineRadioButton->setWhatsThis(
            tr(
                "You can select this option to indicate that, by default, the table will have no horizontal lines "
                "between rows.  You can override this setting for individual row borders."
            )
        );

        singleLineRadioButton->setToolTip(
            tr("The table will default to having single horizontal lines between rows.")
        );
        singleLineRadioButton->setWhatsThis(
            tr(
                "You can select this option to indicate that, by default, the table will have single horizontal lines "
                "between rows.  You can override this setting for individual row borders."
            )
        );

        doubleLineRadioButton->setToolTip(
            tr("The table will default to having double horizontal lines between rows.")
        );
        doubleLineRadioButton->setWhatsThis(
            tr(
                "You can select this option to indicate that, by default, the table will have double horizontal lines "
                "between rows.  You can override this setting for individual row borders."
            )
        );
    } else {
        Q_ASSERT(newLineOrientation == LineOrientation::VERTICAL);

        noLineRadioButton->setToolTip(tr("The table will default to having no vertical lines between columns."));
        noLineRadioButton->setWhatsThis(
            tr(
                "You can select this option to indicate that, by default, the table will have no vertical lines "
                "between columns.  You can override this setting for individual row borders."
            )
        );

        singleLineRadioButton->setToolTip(
            tr("The table will default to having single vertical lines between columns.")
        );
        singleLineRadioButton->setWhatsThis(
            tr(
                "You can select this option to indicate that, by default, the table will have single vertical lines "
                "between columns.  You can override this setting for individual row borders."
            )
        );

        doubleLineRadioButton->setToolTip(
            tr("The table will default to having double vertical lines between columns.")
        );
        doubleLineRadioButton->setWhatsThis(
            tr(
                "You can select this option to indicate that, by default, the table will have double vertical lines "
                "between columns.  You can override this setting for individual row borders."
            )
        );
    }
}


void LineSettingsWidget::setEnabledLineStyles(const QSet<LineSettingsWidget::LineStyle>& newEnabledLineStyles) {
    bool showOptions              = newEnabledLineStyles.count() > 1;

    bool noLineSettingEnabled     = showOptions && newEnabledLineStyles.contains(LineStyle::NO_LINE);
    bool singleLineSettingEnabled = showOptions && newEnabledLineStyles.contains(LineStyle::SINGLE);
    bool doubleLineSettingEnabled = showOptions && newEnabledLineStyles.contains(LineStyle::DOUBLE);

    noLineRadioButton->setVisible(noLineSettingEnabled);

    singleLineSampleWidget->setVisible(singleLineSettingEnabled);
    singleLineRadioButton->setVisible(singleLineSettingEnabled);

    doubleLineSampleWidget->setVisible(doubleLineSettingEnabled);
    doubleLineRadioButton->setVisible(doubleLineSettingEnabled);
}


void LineSettingsWidget::setLineStyles(const QSet<LineSettingsWidget::LineStyle>& newLineStyles) {
    bool noLineSetting     = newLineStyles.contains(LineStyle::NO_LINE);
    bool singleLineSetting = newLineStyles.contains(LineStyle::SINGLE);
    bool doubleLineSetting = newLineStyles.contains(LineStyle::DOUBLE);

    noLineRadioButton->setChecked(noLineSetting);
    singleLineRadioButton->setChecked(singleLineSetting);
    doubleLineRadioButton->setChecked(doubleLineSetting);

    lineWidthSpinBox->setEnabled(singleLineRadioButton->isChecked() || doubleLineRadioButton->isChecked());
}


void LineSettingsWidget::setLineStyle(LineSettingsWidget::LineStyle newLineStyle) {
    QSet<LineStyle> lineStyle;
    lineStyle << newLineStyle;

    setLineStyles(lineStyle);
}


void LineSettingsWidget::setLineWidth(float newLineWidth) {
    if (newLineWidth <= 0) {
        setLineWidthInvalid();
    } else {
        lineWidthSpinBox->setValue(newLineWidth);
    }
}


void LineSettingsWidget::setLineWidthInvalid() {
    lineWidthSpinBox->clear();
}


void LineSettingsWidget::noLineRadioButtonClicked() {
    singleLineRadioButton->setChecked(false);
    doubleLineRadioButton->setChecked(false);

    lineWidthSpinBox->setEnabled(false);

    emit lineStyleChanged(LineStyle::NO_LINE);
}


void LineSettingsWidget::singleLineRadioButtonClicked() {
    noLineRadioButton->setChecked(false);
    doubleLineRadioButton->setChecked(false);

    lineWidthSpinBox->setEnabled(true);

    emit lineStyleChanged(LineStyle::SINGLE);
}


void LineSettingsWidget::doubleLineRadioButtonClicked() {
    noLineRadioButton->setChecked(false);
    singleLineRadioButton->setChecked(false);

    lineWidthSpinBox->setEnabled(true);

    emit lineStyleChanged(LineStyle::DOUBLE);
}


void LineSettingsWidget::lineWidthValueChanged(double newLineWidth) {
    singleLineSampleWidget->setLineWidth(newLineWidth);
    doubleLineSampleWidget->setLineWidth(newLineWidth);

    emit lineWidthChanged(newLineWidth);
}


void LineSettingsWidget::configureWidget() {
    QGridLayout* gridLayout = new QGridLayout;
    setLayout(gridLayout);

    noLineRadioButton = new QRadioButton(tr("No Line"));
    noLineRadioButton->setAutoExclusive(false);
    gridLayout->addWidget(noLineRadioButton, 0, 0);

    singleLineRadioButton = new QRadioButton(tr("Single Line"));
    singleLineRadioButton->setAutoExclusive(false);
    gridLayout->addWidget(singleLineRadioButton, 1, 0);

    singleLineSampleWidget = new EQt::LineSampleWidget;
    singleLineSampleWidget->setNumberLines(1);
    singleLineSampleWidget->setFixedSize(lineSampleWidgetWidth, lineSampleWidgetWidth);

    gridLayout->addWidget(singleLineSampleWidget, 1, 1);

    doubleLineRadioButton = new QRadioButton(tr("Double Line"));
    doubleLineRadioButton->setAutoExclusive(false);
    gridLayout->addWidget(doubleLineRadioButton, 2, 0);

    doubleLineSampleWidget = new EQt::LineSampleWidget;
    doubleLineSampleWidget->setNumberLines(2);
    doubleLineSampleWidget->setFixedSize(lineSampleWidgetWidth, lineSampleWidgetWidth);

    gridLayout->addWidget(doubleLineSampleWidget, 2, 1);

    QHBoxLayout* lineWidthLayout = new QHBoxLayout;
    gridLayout->addLayout(lineWidthLayout, 3, 0, 1, 2);

    lineWidthLayout->addStretch(1);

    QLabel* widthLabel = new QLabel(tr("Width: "));
    lineWidthLayout->addWidget(widthLabel);

    lineWidthSpinBox = new QDoubleSpinBox;
    lineWidthSpinBox->setRange(0.5, 5.0);
    lineWidthSpinBox->setSingleStep(0.5);
    lineWidthSpinBox->setCorrectionMode(QDoubleSpinBox::CorrectToNearestValue);
    lineWidthSpinBox->setDecimals(1);
    lineWidthSpinBox->setSuffix(tr(" pt."));
    lineWidthSpinBox->setToolTip(tr("The line width, in points."));
    lineWidthSpinBox->setWhatsThis(
        tr(
            "You can use this spin box to enter the desired default line width.  Double lines will employ this width "
            "for each line within the double line.  Note that you can override this setting for individual borders, "
            "if desired."
        )
    );

    lineWidthLayout->addWidget(lineWidthSpinBox);
    lineWidthLayout->addStretch(1);

    connect(noLineRadioButton,     &QRadioButton::clicked, this, &LineSettingsWidget::noLineRadioButtonClicked);
    connect(singleLineRadioButton, &QRadioButton::clicked, this, &LineSettingsWidget::singleLineRadioButtonClicked);
    connect(doubleLineRadioButton, &QRadioButton::clicked, this, &LineSettingsWidget::doubleLineRadioButtonClicked);

    connect(
        lineWidthSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &LineSettingsWidget::lineWidthValueChanged
    );
}

#if (!defined(Q_OS_WIN))

    unsigned qHash(LineSettingsWidget::LineStyle lineStyle, Util::HashSeed seed) {
        return qHash(static_cast<unsigned>(lineStyle), seed);
    }

#endif
