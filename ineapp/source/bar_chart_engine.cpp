/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BarChartEngine class.
***********************************************************************************************************************/

#include <QCoreApplication>
#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QStringList>
#include <QPen>
#include <QBrush>
#include <QAbstractAxis>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QXYSeries>
#include <QLineSeries>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QLegend>
#include <QPainter>
#include <QPainterPath>
#include <QImage>

#include <eqt_charts.h>
#include <eqt_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_plot_element.h>
#include <ld_chart_axis_format.h>
#include <ld_plot_series.h>
#include <ld_font_format.h>
#include <ld_plot_format.h>

#include "application.h"
#include "bar_chart_presentation_data.h"
#include "plot_engine.h"
#include "plot_2d_categorized_engine_base.h"
#include "bar_chart_engine.h"

const BarChartEngine BarChartEngine::instance;

BarChartEngine::BarChartEngine() {}


BarChartEngine::~BarChartEngine() {}


QString BarChartEngine::name() const {
    return QString("bar");
}


QString BarChartEngine::shortDescription() const {
    return tr("Bar Chart");
}


QString BarChartEngine::toolTipDescription() const {
    return tr("Bar chart style.");
}


QString BarChartEngine::whatsThisDescription() const {
    return tr("You can use this button to select a bar chart.");
}


QIcon BarChartEngine::icon() const {
    return QIcon(":plot_bar_chart_64.png");
}


BarChartEngine::AxisLocationStyle BarChartEngine::axisLocationStyle() const {
    return AxisLocationStyle::CATEGORIZED;
}


PlotPresentationData* BarChartEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    EQt::ChartItem*           chartItem        = new EQt::ChartItem;
    BarChartPresentationData* presentationData = new BarChartPresentationData(chartItem);

    QSizeF sizeSceneUnits = toScene(QSizeF(plotFormat->chartWidth(), plotFormat->chartHeight()));
    chartItem->setGeometry(0, 0, sizeSceneUnits.width(), sizeSceneUnits.height());

    dynamic_cast<EQt::GraphicsItemGroup*>(presentationData->graphicsItem())->setForcedGeometry(
        0, 0,
        sizeSceneUnits.width(), sizeSceneUnits.height()
    );

    configureChart(chartItem, presentationData, plotElement, plotFormat);

    return presentationData;
}


void BarChartEngine::configureChart(
        QChart*                         chartItem,
        BarChartPresentationData*       presentationData,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    Plot2DCategorizedEngineBase::configureChart(chartItem, plotElement, plotFormat);

    const Ld::ChartLineStyle activeAreaBorderLineStyle = plotFormat->drawingAreaOutlineStyle();

    AxisLocation        baselineAxisLocation = plotFormat->baselineAxis();
    QBarCategoryAxis*   baselineAxis         = new QBarCategoryAxis;
    Ld::ChartAxisFormat baselineAxisFormat   = plotFormat->axisFormat(baselineAxisLocation);

    configureAxisHelper(
        baselineAxis,
        baselineAxisFormat,
        plotElement->axisTitle(baselineAxisLocation),
        activeAreaBorderLineStyle
    );
    presentationData->setAxisFormat(baselineAxisLocation, baselineAxisFormat, baselineAxis);
    chartItem->addAxis(baselineAxis, qtAlignmentForAxis(baselineAxisLocation));

    presentationData->setBaselineAxisLocation(baselineAxisLocation);

    AxisLocation      scaleAxisLocation;
    Qt::AlignmentFlag scaleAxisAlignment;

    if (baselineAxisLocation == AxisLocation::BOTTOM_X_A_GM) {
        scaleAxisLocation  = AxisLocation::LEFT_Y_R_RC;
        scaleAxisAlignment = Qt::AlignmentFlag::AlignLeft;
    } else {
        Q_ASSERT(baselineAxisLocation == AxisLocation::LEFT_Y_R_RC);

        scaleAxisLocation  = AxisLocation::BOTTOM_X_A_GM;
        scaleAxisAlignment = Qt::AlignmentFlag::AlignBottom;
    }

    Ld::ChartAxisFormat scaleAxisFormat = plotFormat->axisFormat(scaleAxisLocation);
    QValueAxis*         scaleAxis       = new QValueAxis;

    configureAxisHelper(
        scaleAxis,
        scaleAxisFormat,
        plotElement->axisTitle(scaleAxisLocation),
        activeAreaBorderLineStyle
    );

    chartItem->addAxis(scaleAxis, scaleAxisAlignment);
    presentationData->setAxisFormat(scaleAxisLocation, scaleAxisFormat, scaleAxis);

    configureLegend(chartItem->legend(), *plotFormat, false);

    QString errorReason = configureSeries(*plotElement, *plotFormat, presentationData);

    if (!errorReason.isEmpty()) {
        presentationData->showErrorMessage(errorReason);
    } else {
        presentationData->clearErrorMessage();
    }
}


QString BarChartEngine::configureSeries(
        const Ld::PlotElement&    plotElement,
        const Ld::PlotFormat&     plotFormat,
        BarChartPresentationData* presentationData
    ) {
    Q_ASSERT(numberDataSourcesPerSeries == 1);

    QString  errorReason;

    unsigned numberDataSeries = plotElement.numberDataSeries();
    unsigned calculatedValueIndex;
    unsigned seriesIndex;

    presentationData->clearSeries();

    bool firstSeriesIsDataLabels = plotFormat.firstSeriesIsDataLabels();
    if (firstSeriesIsDataLabels) {
        presentationData->setLabelSeriesIndex(0);
        presentationData->setLabelsSeries(0, 0, plotElement.dataSource(0, 0));

        seriesIndex          = 1;
        calculatedValueIndex = 1;
    } else {
        presentationData->clearLabelSeriesIndex();

        seriesIndex          = 0;
        calculatedValueIndex = 0;
    }

    if (numberDataSeries <= seriesIndex) {
        errorReason = tr("Insufficient number data series for plot.");
    }

    while (errorReason.isEmpty() && seriesIndex < numberDataSeries) {
        AxisLocation          axisLocation      = plotElement.axisLocationForSource(seriesIndex, 0);
        Ld::VariableName      dataSourceName    = plotElement.dataSource(seriesIndex, 0);
        QString               seriesLegendTitle = plotElement.legendTitle(seriesIndex);
        const Ld::PlotSeries& plotSeries        = plotFormat.plotSeries(seriesIndex);
        AxisScale             axisScale         = plotFormat.axisFormat(axisLocation).axisScale();
        QColor                barColor          = validateForegroundColor(plotSeries.lineColor());

        errorReason = presentationData->setSeries(
            seriesIndex,
            calculatedValueIndex,
            dataSourceName,
            axisLocation,
            axisScale,
            seriesLegendTitle,
            barColor
        );

        ++seriesIndex;
        ++calculatedValueIndex;
    }

    return errorReason;
}
