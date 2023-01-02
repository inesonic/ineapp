/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref LineStyleWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QColor>
#include <QHBoxLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTimer>
#include <QSize>
#include <QRect>
#include <QPoint>
#include <QPainter>
#include <QPen>
#include <QBrush>


#include <eqt_color_tool_button.h>
#include <eqt_color_dialog.h>

#include "application.h"
#include "metatypes.h"
#include "line_style_widget.h"

LineStyleWidget::LineStyleWidget(QWidget* parent):QWidget(parent) {
    configureWidget();
    lineWidthSpinBox->setValue(0);
    lineColorToolButton->setColor(QColor(Qt::GlobalColor::black));
    populateLineStyleComboBox(0, QColor(Qt::GlobalColor::black));
}


LineStyleWidget::LineStyleWidget(LineStyleWidget::LineStyle lineStyle, QWidget* parent):QWidget(parent) {
    configureWidget();
    setLineStyle(lineStyle);
}


LineStyleWidget::LineStyleWidget(
        LineStyleWidget::LineStyle lineStyle,
        float                      lineWidth,
        QWidget*                   parent
    ):QWidget(
        parent
    ) {
    configureWidget();

    lineWidthSpinBox->setValue(lineWidth);
    lineColorToolButton->setColor(Qt::GlobalColor::black);

    setLineStyle(lineStyle);
}


LineStyleWidget::LineStyleWidget(
        LineStyleWidget::LineStyle lineStyle,
        float                      lineWidth,
        const QColor&              lineColor,
        QWidget*                   parent
    ):QWidget(
        parent
    ) {
    configureWidget();

    lineWidthSpinBox->setValue(lineWidth);
    lineColorToolButton->setColor(lineColor);

    setLineStyle(lineStyle);
}


LineStyleWidget::~LineStyleWidget() {}


LineStyleWidget::LineStyle LineStyleWidget::lineStyle() const {
    return   lineStyleComboBox->currentIndex() >= 0
           ? lineStyleComboBox->currentData().value<LineStyle>()
           : LineStyle::NO_LINE;
}


float LineStyleWidget::lineWidth() const {
    return lineWidthSpinBox->value();
}


QColor LineStyleWidget::lineColor() const {
    return *lineColorToolButton->colors().begin();
}


void LineStyleWidget::setLineStyle(LineStyleWidget::LineStyle newLineStyle) {
    unsigned numberItems = static_cast<unsigned>(lineStyleComboBox->count());
    unsigned i           = 0;

    while (i < numberItems && lineStyleComboBox->itemData(i).value<LineStyle>() != newLineStyle) {
        ++i;
    }

    if (i < numberItems) {
        lineStyleComboBox->setCurrentIndex(i);
    }

    bool enableOtherControls = (newLineStyle != LineStyle::NO_LINE);

    lineWidthSpinBox->setEnabled(enableOtherControls);
    lineColorToolButton->setEnabled(enableOtherControls);
}


void LineStyleWidget::setLineWidth(float newLineWidth) {
    lineWidthSpinBox->setValue(newLineWidth);
    populateLineStyleComboBox(newLineWidth, lineColor());
}


void LineStyleWidget::setLineColor(const QColor& newLineColor) {
    lineColorToolButton->setColor(newLineColor);
    populateLineStyleComboBox(lineWidth(), newLineColor);
}


void LineStyleWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    showTimer->start(0);
}


void LineStyleWidget::lineStyleWasChanged(int index) {
    if (index >= 0) {
        LineStyle lineStyle = lineStyleComboBox->itemData(index).value<LineStyle>();
        emit lineStyleChanged(lineStyle);
        emit lineSettingsChanged(lineStyle, lineWidth(), lineColor());

        bool enableOtherControls = (lineStyle != LineStyle::NO_LINE);

        lineWidthSpinBox->setEnabled(enableOtherControls);
        lineColorToolButton->setEnabled(enableOtherControls);
    }
}


void LineStyleWidget::lineWidthWasChanged(double newLineWidth) {
    QColor currentLineColor = lineColor();
    populateLineStyleComboBox(newLineWidth, currentLineColor);

    emit lineWidthChanged(newLineWidth);
    emit lineSettingsChanged(lineStyle(), newLineWidth, currentLineColor);
}


void LineStyleWidget::lineColorButtonClicked() {
    EQt::ColorDialog dialog(lineColor(), this);
    dialog.loadSettings();

    int result = dialog.exec();
    if (result == EQt::ColorDialog::DialogCode::Accepted) {
        QColor newColor = dialog.currentColor();
        lineColorToolButton->setColor(newColor);
        populateLineStyleComboBox(lineWidth(), newColor);

        emit lineColorChanged(newColor);
        emit lineSettingsChanged(lineStyle(), lineWidth(), newColor);
    }

    dialog.saveSettings();
}


void LineStyleWidget::nowVisible() {
    unsigned colorButtonSide = lineStyleComboBox->height();
    lineColorToolButton->setFixedSize(QSize(colorButtonSide, colorButtonSide));
}


void LineStyleWidget::configureWidget() {
    QHBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    lineStyleComboBox = new QComboBox;
    lineStyleComboBox->setEditable(false);
    layout->addWidget(lineStyleComboBox);

    lineWidthSpinBox = new QDoubleSpinBox;
    lineWidthSpinBox->setMinimum(0);
    lineWidthSpinBox->setSingleStep(0.5);
    layout->addWidget(lineWidthSpinBox);

    lineColorToolButton = new EQt::ColorToolButton;
    layout->addWidget(lineColorToolButton);

    showTimer = new QTimer(this);
    showTimer->setSingleShot(true);

    connect(
        lineStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &LineStyleWidget::lineStyleWasChanged
    );

    connect(
        lineWidthSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &LineStyleWidget::lineWidthWasChanged
    );

    connect(
        lineColorToolButton,
        &EQt::ColorToolButton::clicked,
        this,
        &LineStyleWidget::lineColorButtonClicked
    );

    connect(showTimer, &QTimer::timeout, this, &LineStyleWidget::nowVisible);
}


void LineStyleWidget::populateLineStyleComboBox(float lineWidth, const QColor& lineColor) {
    QSize    physicalDpi      = Application::physicalDpi();
    float    widthScaleFactor = static_cast<float>(physicalDpi.width()) / static_cast<float>(physicalDpi.height());
    unsigned height           = QFontMetrics(Application::font()).height();
    unsigned width            = static_cast<unsigned>(2 * height * widthScaleFactor + 0.5);

    QSize  pixmapSize(width, height);
    QPoint leftCenter(0, height / 2.0);
    QPoint rightCenter(width, height / 2.0);

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

    unsigned  currentIndex = static_cast<unsigned>(std::max(0, lineStyleComboBox->currentIndex()));

    lineStyleComboBox->clear();

    for (unsigned i=0 ; i<numberLineStyles ; ++i) {
        const LineStyleData& lineStyleData = lineStyles[i];

        QPixmap pixmap(pixmapSize);
        pixmap.fill(QColor(Qt::GlobalColor::white));
        QPainter painter(&pixmap);

        QPen pen(QBrush(lineColor), lineWidth, static_cast<Qt::PenStyle>(lineStyleData.style));
        painter.setPen(pen);
        painter.setBrush(QBrush());

        painter.drawLine(leftCenter, rightCenter);
        lineStyleComboBox->addItem(QIcon(pixmap), lineStyleData.description, QVariant(static_cast<int>(lineStyleData.style)));
        lineStyleComboBox->setIconSize(pixmapSize);
    }

    lineStyleComboBox->setCurrentIndex(currentIndex);
}
