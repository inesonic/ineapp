/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotSeriesWidget class.
***********************************************************************************************************************/

#include <QWidget>
#include <QSize>
#include <QRect>
#include <QGridLayout>
#include <QSizePolicy>
#include <QTimer>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFontMetrics>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QColorDialog>

#include <eqt_color_tool_button.h>

#include "application.h"
#include "metatypes.h"
#include "plot_series_demo_widget.h"
#include "plot_series_widget.h"

PlotSeriesWidget::PlotSeriesWidget(QWidget* parent):QWidget(parent) {
    QGridLayout* layout = new QGridLayout;
    setLayout(layout);

    lineStyleComboBox = new QComboBox;
    lineStyleComboBox->setEditable(false);
    layout->addWidget(lineStyleComboBox, 0, 0, Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter);

    lineWidthSpinBox = new QDoubleSpinBox;
    lineWidthSpinBox->setMinimum(0);
    lineWidthSpinBox->setStepType(QDoubleSpinBox::StepType::DefaultStepType);
    lineWidthSpinBox->setSingleStep(0.5);
    lineWidthSpinBox->setDecimals(1);
    layout->addWidget(lineWidthSpinBox, 0, 1, Qt::AlignmentFlag::AlignCenter);

    rightArrowLabel = new QLabel(tr("%1").arg(QChar(0x2192)));
    layout->addWidget(rightArrowLabel, 0, 2, Qt::AlignmentFlag::AlignCenter);

    plotSeriesDemoWidget = new PlotSeriesDemoWidget;
    plotSeriesDemoWidget->setFixedSize(10, 10); // Make the width a non-zero size for now.
    layout->addWidget(plotSeriesDemoWidget, 0, 3, Qt::AlignmentFlag::AlignCenter);

    leftArrowLabel = new QLabel(tr("%1").arg(QChar(0x2190)));
    layout->addWidget(leftArrowLabel, 0, 4, Qt::AlignmentFlag::AlignCenter);

    QHBoxLayout* rightControlsLayout = new QHBoxLayout;
    layout->addLayout(rightControlsLayout, 0, 5, Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

    splineTypeComboBox = new QComboBox;
    splineTypeComboBox->setEditable(false);
    rightControlsLayout->addWidget(splineTypeComboBox, Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

    gradientTypeComboBox = new QComboBox;
    gradientTypeComboBox->setEditable(false);
    rightControlsLayout->addWidget(
        gradientTypeComboBox,
        Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter
    );

    upArrowLabel = new QLabel(tr("%1").arg(QChar(0x2191)));
    layout->addWidget(upArrowLabel, 1, 3, Qt::AlignmentFlag::AlignCenter);

    markerStyleComboBox = new QComboBox;
    markerStyleComboBox->setEditable(false);
    layout->addWidget(markerStyleComboBox, 2, 3, Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignTop);

    colorToolButton = new EQt::ColorToolButton;
    layout->addWidget(colorToolButton, 2, 5, Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter);

    colorToolButton->setColor(QColor(Qt::black));

    currentPlotSeriesMode = PlotSeriesMode::LINES_AND_MARKERS;

    populateMarkerStyleComboBox(false);
    populateLineStyleComboBox(false);
    populateSplineTypeComboBox(false);
    populateGradientTypeComboBox(false);

    setLineStyle(LineStyle::SOLID_LINE);
    setLineWidth(0);
    setSplineType(SplineType::LINE);
    setMarkerStyle(MarkerStyle::CIRCLE);

    setPlotSeriesMode(PlotSeriesMode::LINES_AND_MARKERS);

    showTimer = new QTimer(this);
    showTimer->setSingleShot(true);

    connectAll();
}


PlotSeriesWidget::~PlotSeriesWidget() {}


QRect PlotSeriesWidget::demoImageGeometry() const {
    return QRect(mapToGlobal(plotSeriesDemoWidget->pos()), plotSeriesDemoWidget->size());
}


PlotSeriesWidget::PlotSeriesMode PlotSeriesWidget::plotSeriesMode() const {
    return currentPlotSeriesMode;
}


PlotSeriesWidget::LineStyle PlotSeriesWidget::lineStyle() const {
    LineStyle result;

    if (lineStyleComboBox->currentIndex() >= 0) {
        result = static_cast<LineStyle>(lineStyleComboBox->currentData().toInt());
    } else {
        result = LineStyle::SOLID_LINE;
    }

    return result;
}


float PlotSeriesWidget::lineWidth() const {
    return lineWidthSpinBox->value();
}


QColor PlotSeriesWidget::lineColor() const {
    return *colorToolButton->colors().begin();
}


PlotSeriesWidget::MarkerStyle PlotSeriesWidget::markerStyle() const {
    MarkerStyle result;

    if (markerStyleComboBox->currentIndex() >= 0) {
        result = static_cast<MarkerStyle>(markerStyleComboBox->currentData().toInt());
    } else {
        result = MarkerStyle::CIRCLE;
    }

    return result;
}


PlotSeriesWidget::SplineType PlotSeriesWidget::splineType() const {
    SplineType result;

    if (splineTypeComboBox->currentIndex() >= 0) {
        result = static_cast<SplineType>(splineTypeComboBox->currentData().toInt());
    } else {
        result = SplineType::LINE;
    }

    return result;
}


PlotSeriesWidget::GradientType PlotSeriesWidget::gradientType() const {
    GradientType result;

    if (gradientTypeComboBox->currentIndex() >= 0) {
        result = static_cast<GradientType>(gradientTypeComboBox->currentData().toInt());
    } else {
        result = GradientType::NONE;
    }

    return result;
}


void PlotSeriesWidget::setDemoImageSize(const QSize& newDemoImagesize) {
    plotSeriesDemoWidget->setFixedSize(newDemoImagesize);
}


void PlotSeriesWidget::setPlotSeriesMode(PlotSeriesWidget::PlotSeriesMode newPlotSeriesMode) {
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
            rightArrowLabel->setVisible(true);
            leftArrowLabel->setVisible(false);
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


void PlotSeriesWidget::setLineStyle(LineStyle newLineStyle) {
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


void PlotSeriesWidget::setLineWidth(float newLineWidth) {
    lineWidthSpinBox->setValue(newLineWidth);
}


void PlotSeriesWidget::setLineColor(const QColor& newColor) {
    colorToolButton->setColor(newColor);
    updateDemoWidget();
}


void PlotSeriesWidget::setMarkerStyle(PlotSeriesWidget::MarkerStyle newMarkerStyle) {
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


void PlotSeriesWidget::setSplineType(PlotSeriesWidget::SplineType newSplineType) {
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


void PlotSeriesWidget::setGradientType(PlotSeriesWidget::GradientType newGradientType) {
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


void PlotSeriesWidget::showEvent(QShowEvent* /* event */) {
    showTimer->start(1);
}


void PlotSeriesWidget::colorToolButtonClicked() {
    QColorDialog colorDialog(*colorToolButton->colors().begin(), this);

    int result = colorDialog.exec();
    if (result == QColorDialog::DialogCode::Accepted) {
        QColor color = colorDialog.currentColor();

        colorToolButton->setColor(color);
        lineColorChanged(color);
    }
}


void PlotSeriesWidget::lineStyleChanged(int /* index */) {
    populateSplineTypeComboBox();
    updateDemoWidget();
}


void PlotSeriesWidget::lineWidthChanged(double /* newValue */) {
    populateLineStyleComboBox();
    populateSplineTypeComboBox();
    populateMarkerStyleComboBox();

    updateDemoWidget();
}


void PlotSeriesWidget::lineColorChanged(const QColor& /* newColor */) {
    populateLineStyleComboBox();
    populateSplineTypeComboBox();
    populateMarkerStyleComboBox();
    populateGradientTypeComboBox();

    updateDemoWidget();
}


void PlotSeriesWidget::markerStyleChanged(int /* index */) {
    updateDemoWidget();
}


void PlotSeriesWidget::splineStyleChanged(int /* index */) {
    populateLineStyleComboBox();
    updateDemoWidget();
}


void PlotSeriesWidget::gradientTypeChanged(int /* index */) {
    updateDemoWidget();
}


void PlotSeriesWidget::nowVisible() {
    lineStyleComboBox->setFocus();
}


void PlotSeriesWidget::updateDemoWidget() {
    LineStyle    lineStyle    = LineStyle::NO_LINE;
    float        lineWidth    = 0;
    QColor       lineColor    = QColor(Qt::GlobalColor::black);
    SplineType   splineType   = SplineType::NONE;
    MarkerStyle  markerStyle  = MarkerStyle::NONE;
    GradientType gradientType = GradientType::NONE;

    switch (currentPlotSeriesMode) {
        case PlotSeriesMode::LINES: {
            lineStyle    = PlotSeriesWidget::lineStyle();
            lineWidth    = PlotSeriesWidget::lineWidth();
            lineColor    = PlotSeriesWidget::lineColor();
            splineType   = PlotSeriesWidget::splineType();
            gradientType = GradientType::NONE;
            markerStyle  = MarkerStyle::NONE;

            break;
        }

        case PlotSeriesMode::MARKERS: {
            lineStyle    = LineStyle::NO_LINE;
            lineWidth    = 1.0F;
            lineColor    = PlotSeriesWidget::lineColor();
            splineType   = SplineType::NONE;
            gradientType = GradientType::NONE;
            markerStyle  = PlotSeriesWidget::markerStyle();

            break;
        }

        case PlotSeriesMode::LINES_AND_MARKERS: {
            lineStyle    = PlotSeriesWidget::lineStyle();
            lineWidth    = PlotSeriesWidget::lineWidth();
            lineColor    = PlotSeriesWidget::lineColor();
            splineType   = PlotSeriesWidget::splineType();
            gradientType = GradientType::NONE;
            markerStyle  = PlotSeriesWidget::markerStyle();

            break;
        }

        case PlotSeriesMode::COLOR_REGION: {
            lineStyle    = LineStyle::NO_LINE;
            lineWidth    = 0;
            lineColor    = PlotSeriesWidget::lineColor();
            splineType   = SplineType::NONE;
            gradientType = GradientType::NONE;
            markerStyle  = MarkerStyle::NONE;

            break;
        }


        case PlotSeriesMode::GRADIENT: {
            lineStyle    = LineStyle::NO_LINE;
            lineWidth    = 0;
            lineColor    = PlotSeriesWidget::lineColor();
            splineType   = SplineType::NONE;
            gradientType = PlotSeriesWidget::gradientType();
            markerStyle  = MarkerStyle::NONE;

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


void PlotSeriesWidget::connectAll() {
    connect(
        lineStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::lineStyleChanged
    );
    connect(
        lineWidthSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &PlotSeriesWidget::lineWidthChanged
    );
    connect(
        splineTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::splineStyleChanged
    );
    connect(
        gradientTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::gradientTypeChanged
    );
    connect(
        markerStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::markerStyleChanged
    );
    connect(colorToolButton, &EQt::ColorToolButton::clicked, this, &PlotSeriesWidget::colorToolButtonClicked);

    connect(showTimer, &QTimer::timeout, this, &PlotSeriesWidget::nowVisible);
}


void PlotSeriesWidget::disconnectAll() {
    disconnect(
        lineStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::lineStyleChanged
    );
    disconnect(
        lineWidthSpinBox,
        static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this,
        &PlotSeriesWidget::lineWidthChanged
    );
    disconnect(
        splineTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::splineStyleChanged
    );
    disconnect(
        gradientTypeComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::gradientTypeChanged
    );
    disconnect(
        markerStyleComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        &PlotSeriesWidget::markerStyleChanged
    );
    disconnect(colorToolButton, &EQt::ColorToolButton::clicked, this, &PlotSeriesWidget::colorToolButtonClicked);

    disconnect(showTimer, &QTimer::timeout, this, &PlotSeriesWidget::nowVisible);
}


void PlotSeriesWidget::populateLineStyleComboBox(bool manageSignals) {
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


void PlotSeriesWidget::populateSplineTypeComboBox(bool manageSignals) {
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
        { tr("Straignt Lines"),     SplineType::LINE              },
        { tr("Spline"),             SplineType::SPLINE            },
        { tr("Linear Regression"),  SplineType::LINEAR_REGRESSION }
    };
    const unsigned numberSplineStyles = sizeof(splineStyles) / sizeof(SplineStyleData);

    unsigned   currentIndex      = 0;
    SplineType currentSplineType = splineType();

    splineTypeComboBox->clear();

    for (unsigned i=0 ; i<numberSplineStyles ; ++i) {
        const SplineStyleData& splineStyleData = splineStyles[i];

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

    splineTypeComboBox->setCurrentIndex(currentIndex);

    if (manageSignals) {
        connectAll();
    }
}


void PlotSeriesWidget::populateGradientTypeComboBox(bool manageSignals) {
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


void PlotSeriesWidget::populateMarkerStyleComboBox(bool manageSignals) {
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


