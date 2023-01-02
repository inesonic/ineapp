/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotWrappedEngineBase class.
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
#include "plot_wrapped_presentation_data.h"
#include "plot_engine.h"
#include "plot_wrapped_engine_base.h"

const float PlotWrappedEngineBase::minimumMarkerSizePoints = 9.0F;

PlotWrappedEngineBase::PlotWrappedEngineBase() {}


PlotWrappedEngineBase::~PlotWrappedEngineBase() {}


void PlotWrappedEngineBase::configureChart(
        QChart*                         chartItem,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    const Ld::ChartLineStyle& borderLineStyle = plotFormat->borderLineStyle();
    QPen borderPen(
        QBrush(validateForegroundColor(borderLineStyle.lineColor())),
        borderLineStyle.lineWidth(),
        static_cast<Qt::PenStyle>(borderLineStyle.lineStyle())
    );
    chartItem->setBackgroundPen(borderPen);
    chartItem->setBackgroundRoundness(0); // For now, no round-ness.

    Ld::FontFormat titleFontFormat = plotFormat->titleFontFormat();
    QColor         titleFontColor  = validateForegroundColor(titleFontFormat.fontColor());

    float fontScaleFactor  = Application::fontScaleFactor();
    QFont titleFont        = titleFontFormat.toQFont();
    titleFont.setPointSizeF(titleFont.pointSizeF() * fontScaleFactor);

    chartItem->setTitle(plotElement->plotTitle());
    // NOTE: If we ever support sub-titles, we would set that here.
    chartItem->setTitleFont(titleFont);
    chartItem->setTitleBrush(QBrush(titleFontColor));

    chartItem->setDropShadowEnabled(false);
    chartItem->setBackgroundBrush(QBrush(validateBackgroundColor(plotFormat->plotAreaBackgroundColor())));
}


QAbstractAxis* PlotWrappedEngineBase::configureAxis(
        const Ld::ChartAxisFormat& axisFormat,
        const QString&             axisTitle,
        const Ld::ChartLineStyle&  plotAreaBorderLineStyle
    ) {
    QAbstractAxis* axis;
    if (axisFormat.axisScale() == Ld::ChartAxisFormat::AxisScale::LOG) {
        QLogValueAxis* logAxis = new QLogValueAxis;
        logAxis->setMinorTickCount(8);
        logAxis->setLabelFormat(tr("%5.0g"));
        axis = logAxis;
    } else {
        axis = new QValueAxis;
    }

    configureAxisHelper(axis, axisFormat, axisTitle, plotAreaBorderLineStyle);

    return axis;
}


void PlotWrappedEngineBase::configureAxisHelper(
        QAbstractAxis*             axis,
        const Ld::ChartAxisFormat& axisFormat,
        const QString&             axisTitle,
        const Ld::ChartLineStyle&  plotAreaBorderLineStyle
    ) {
    float fontScaleFactor  = Application::fontScaleFactor();

    Ld::FontFormat titleFontFormat  = axisFormat.titleFont();
    Ld::FontFormat numberFontFormat = axisFormat.numberFont();

    QFont  titleFont      = titleFontFormat.toQFont();
    QColor titleFontColor = titleFontFormat.fontColor();

    QFont  numberFont      = numberFontFormat.toQFont();
    QColor numberFontColor = numberFontFormat.fontColor();

    titleFont.setPointSizeF(titleFont.pointSizeF() * fontScaleFactor);
    numberFont.setPointSizeF(numberFont.pointSizeF() * fontScaleFactor);

    axis->setLabelsFont(numberFont);
    axis->setLabelsColor(numberFontColor);

    axis->setTitleFont(titleFont);
    axis->setTitleBrush(QBrush(titleFontColor));

    if (!axisTitle.isEmpty()) {
        axis->setTitleVisible(true);
        axis->setTitleText(axisTitle);
    } else {
        axis->setTitleVisible(false);
    }

    if (plotAreaBorderLineStyle.lineStyle() != Ld::ChartLineStyle::LineStyle::NO_LINE) {
        QPen borderLinePen(
            QBrush(plotAreaBorderLineStyle.lineColor()),
            plotAreaBorderLineStyle.lineWidth(),
            static_cast<Qt::PenStyle>(plotAreaBorderLineStyle.lineStyle())
        );

        axis->setLinePen(borderLinePen);
        axis->setLinePenColor(plotAreaBorderLineStyle.lineColor());
        axis->setLineVisible(true);
    } else {
        axis->setLineVisible(false);
    }

    Ld::ChartAxisFormat::TickStyle majorTickStyle = axisFormat.majorTickStyle();
    if (majorTickStyle == Ld::ChartAxisFormat::TickStyle::LINE          ||
        majorTickStyle == Ld::ChartAxisFormat::TickStyle::LINE_AND_TICK    ) {
        const Ld::ChartLineStyle& gridLineStyle = axisFormat.majorTickLineStyle();
        QPen gridPen(
            QBrush(gridLineStyle.lineColor()),
            gridLineStyle.lineWidth(),
            static_cast<Qt::PenStyle>(gridLineStyle.lineStyle())
        );
        axis->setGridLinePen(gridPen);
        axis->setGridLineColor(gridLineStyle.lineColor());
        axis->setGridLineVisible(true);
    } else {
        axis->setGridLineVisible(false);
    }

    Ld::ChartAxisFormat::TickStyle minorTickStyle = axisFormat.minorTickStyle();
    if (minorTickStyle == Ld::ChartAxisFormat::TickStyle::LINE          ||
        minorTickStyle == Ld::ChartAxisFormat::TickStyle::LINE_AND_TICK    ) {
        const Ld::ChartLineStyle& gridLineStyle = axisFormat.minorTickLineStyle();
        QPen gridPen(
            QBrush(gridLineStyle.lineColor()),
            gridLineStyle.lineWidth(),
            static_cast<Qt::PenStyle>(gridLineStyle.lineStyle())
        );
        axis->setMinorGridLinePen(gridPen);
        axis->setMinorGridLineColor(gridLineStyle.lineColor());
        axis->setMinorGridLineVisible(true);
    } else {
        axis->setMinorGridLineVisible(false);
    }
}


void PlotWrappedEngineBase::configureLegend(
        QLegend*              legend,
        const Ld::PlotFormat& plotFormat,
        bool                  honorMarkerShape
    ) {
    Ld::PlotFormat::LegendLocation legendLocation = plotFormat.legendLocation();

    if (legendLocation == Ld::PlotFormat::LegendLocation::NO_LEGEND) {
        legend->setVisible(false);
    } else {
        legend->setVisible(true);

        Qt::Alignment alignment;
        switch (legendLocation) {
            case Ld::PlotFormat::LegendLocation::NO_LEGEND: {
                Q_ASSERT(false);
                break;
            }

            case Ld::PlotFormat::LegendLocation::TOP:    { alignment = Qt::AlignmentFlag::AlignTop;      break; }
            case Ld::PlotFormat::LegendLocation::LEFT:   { alignment = Qt::AlignmentFlag::AlignLeft;     break; }
            case Ld::PlotFormat::LegendLocation::RIGHT:  { alignment = Qt::AlignmentFlag::AlignRight;    break; }
            case Ld::PlotFormat::LegendLocation::BOTTOM: { alignment = Qt::AlignmentFlag::AlignBottom;   break; }

            default: {
                Q_ASSERT(false);
                break;
            }
        }

        legend->setAlignment(alignment);

        if (honorMarkerShape) {
            legend->setMarkerShape(QLegend::MarkerShape::MarkerShapeFromSeries);
        }

        Ld::ChartLineStyle legendLineStyle = plotFormat.legendBorderlineStyle();
        QPen legendBorderPen(
            QBrush(validateForegroundColor(legendLineStyle.lineColor())),
            legendLineStyle.lineWidth(),
            static_cast<Qt::PenStyle>(legendLineStyle.lineStyle())
        );

        QColor         legendBackgroundColor = plotFormat.legendBackgroundColor();
        Ld::FontFormat legendFontFormat      = plotFormat.legendFontFormat();
        QFont          legendFont            = legendFontFormat.toQFont();

        float fontScaleFactor  = Application::fontScaleFactor();
        legendFont.setPointSizeF(legendFont.pointSizeF() * fontScaleFactor);

        if (legendBackgroundColor.isValid()) {
            legend->setColor(legendBackgroundColor);
            legend->setBackgroundVisible(true);
        } else {
            legend->setBackgroundVisible(false);
        }

        legend->setFont(legendFont);
        legend->setLabelBrush(QBrush(validateForegroundColor(legendFontFormat.fontColor())));
        legend->setPen(legendBorderPen);
        legend->setShowToolTips(true);
    }
}


Qt::Alignment PlotWrappedEngineBase::qtAlignmentForAxis(PlotWrappedEngineBase::AxisLocation axisLocation) const {
    Qt::Alignment result;
    switch (axisLocation) {
        case Ld::PlotFormat::AxisLocation::LEFT_Y_R_RC:   { result = Qt::AlignmentFlag::AlignLeft;     break; }
        case Ld::PlotFormat::AxisLocation::BOTTOM_X_A_GM: { result = Qt::AlignmentFlag::AlignBottom;   break; }
        case Ld::PlotFormat::AxisLocation::RIGHT_Y_R_RY:  { result = Qt::AlignmentFlag::AlignRight;    break; }
        case Ld::PlotFormat::AxisLocation::TOP_X_A_GM:    { result = Qt::AlignmentFlag::AlignTop;      break; }

        case Ld::PlotFormat::AxisLocation::Z_B_BK: {
            Q_ASSERT(false);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return result;
}


QColor PlotWrappedEngineBase::validateForegroundColor(const QColor& inputColor) {
    return inputColor.isValid() ? inputColor : QColor(Qt::GlobalColor::black);
}


QColor PlotWrappedEngineBase::validateBackgroundColor(const QColor& inputColor) {
    return inputColor.isValid() ? inputColor : QColor(Qt::GlobalColor::transparent);
}
