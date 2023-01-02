/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PieChartEngine class.
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
#include "pie_chart_presentation_data.h"
#include "plot_engine.h"
#include "plot_2d_categorized_engine_base.h"
#include "pie_chart_engine.h"

const PieChartEngine PieChartEngine::instance;

PieChartEngine::PieChartEngine() {}


PieChartEngine::~PieChartEngine() {}


QString PieChartEngine::name() const {
    return QString("pie");
}


QString PieChartEngine::shortDescription() const {
    return tr("Pie Chart");
}


QString PieChartEngine::toolTipDescription() const {
    return tr("Pie chart style.");
}


QString PieChartEngine::whatsThisDescription() const {
    return tr("You can use this button to select a pie chart.");
}


QIcon PieChartEngine::icon() const {
    return QIcon(":plot_pie_chart_64.png");
}


PieChartEngine::AxisLocationStyle PieChartEngine::axisLocationStyle() const {
    return AxisLocationStyle::UNCATEGORIZED;
}


PlotPresentationData* PieChartEngine::createPresentationData(
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    EQt::ChartItem*           chartItem        = new EQt::ChartItem;
    PieChartPresentationData* presentationData = new PieChartPresentationData(chartItem);

    QSizeF sizeSceneUnits = toScene(QSizeF(plotFormat->chartWidth(), plotFormat->chartHeight()));
    chartItem->setGeometry(0, 0, sizeSceneUnits.width(), sizeSceneUnits.height());

    dynamic_cast<EQt::GraphicsItemGroup*>(presentationData->graphicsItem())->setForcedGeometry(
        0, 0,
        sizeSceneUnits.width(), sizeSceneUnits.height()
    );

    configureChart(chartItem, presentationData, plotElement, plotFormat);

    return presentationData;
}


void PieChartEngine::configureChart(
        QChart*                         chartItem,
        PieChartPresentationData*       presentationData,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    Plot2DCategorizedEngineBase::configureChart(chartItem, plotElement, plotFormat);
    configureLegend(chartItem->legend(), *plotFormat, false);

    QString errorReason = configureSeries(*plotElement, *plotFormat, presentationData);

    if (!errorReason.isEmpty()) {
        presentationData->showErrorMessage(errorReason);
    } else {
        presentationData->clearErrorMessage();
    }
}


QString PieChartEngine::configureSeries(
        const Ld::PlotElement&    plotElement,
        const Ld::PlotFormat&     plotFormat,
        PieChartPresentationData* presentationData
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
        Ld::VariableName      dataSourceName    = plotElement.dataSource(seriesIndex, 0);
        QString               seriesLegendTitle = plotElement.legendTitle(seriesIndex);
        const Ld::PlotSeries& plotSeries        = plotFormat.plotSeries(seriesIndex);
        QColor                sliceColor        = validateForegroundColor(plotSeries.lineColor());

        errorReason = presentationData->setSeries(
            seriesIndex,
            calculatedValueIndex,
            dataSourceName,
            seriesLegendTitle,
            sliceColor
        );

        ++seriesIndex;
        ++calculatedValueIndex;
    }

    return errorReason;
}
