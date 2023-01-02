/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotWrappedPresentationData class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QGraphicsItem>
#include <QValueAxis>
#include <QLogValueAxis>
#include <QFontMetricsF>
#include <QRectF>

#include <cmath>

#include <eqt_charts.h>
#include <eqt_graphics_item.h>
#include <eqt_chart_item.h>
#include <eqt_graphics_multi_text_group.h>

#include <ld_calculated_value.h>

#include "application.h"
#include "presentation.h"
#include "plot_presentation_data.h"
#include "plot_wrapped_presentation_data.h"

/***********************************************************************************************************************
 * PlotWrappedPresentationData::AxisData
 */

PlotWrappedPresentationData::AxisData::AxisData() {
    currentAxisLocation = AxisLocation::LEFT_Y_R_RC;
    currentAxis         = Q_NULLPTR;
}


PlotWrappedPresentationData::AxisData::AxisData(
        PlotWrappedPresentationData::AxisLocation axisLocation,
        const Ld::ChartAxisFormat&                axisFormat,
        QAbstractAxis*                            axis
    ):currentAxisLocation(
        axisLocation
    ),currentAxisFormat(
        axisFormat
    ),currentAxis(
        axis
    ) {}


PlotWrappedPresentationData::AxisData::AxisData(
        const PlotWrappedPresentationData::AxisData& other
    ):currentAxisLocation(
        other.currentAxisLocation
    ),currentAxisFormat(
        other.currentAxisFormat
    ),currentAxis(
        other.currentAxis
    ) {}


PlotWrappedPresentationData::AxisData::~AxisData() {}


PlotWrappedPresentationData::AxisData& PlotWrappedPresentationData::AxisData::operator=(
        const PlotWrappedPresentationData::AxisData& other
    ) {
    currentAxisLocation = other.currentAxisLocation;
    currentAxisFormat   = other.currentAxisFormat;
    currentAxis         = other.currentAxis;

    return *this;
}

/***********************************************************************************************************************
 * PlotWrappedPresentationData
 */

PlotWrappedPresentationData::PlotWrappedPresentationData(EQt::GraphicsItem* chartItem) {
    EQt::GraphicsMultiTextGroup* group = new EQt::GraphicsMultiTextGroup;
    group->setBorderPen(QPen(Qt::PenStyle::NoPen));

    currentChartGraphicsItem = chartItem;
    currentChartItem         = dynamic_cast<QChart*>(chartItem);

    if (currentChartItem != Q_NULLPTR) {
        group->addToGroup(currentChartItem);
        currentChartItem->setPos(0, 0);
    }

    setGraphicsItem(group);
}


PlotWrappedPresentationData::~PlotWrappedPresentationData() {
    if (currentChartGraphicsItem != Q_NULLPTR) {
        currentChartGraphicsItem->deleteLater();
    }
}


QRectF PlotWrappedPresentationData::boundingRectangle() const {
    return chartItem()->boundingRect();
}


QRectF PlotWrappedPresentationData::sceneBoundingRectangle() const {
    return chartItem()->sceneBoundingRect();
}


void PlotWrappedPresentationData::showErrorMessage(const QString& errorMessage) {
    EQt::GraphicsMultiTextGroup* group = dynamic_cast<EQt::GraphicsMultiTextGroup*>(graphicsItem());

    QFont messageFont = Application::font();
    group->setTextPen(QPen(Presentation::errorBorderColor));
    group->setBorderPen(QPen(Presentation::errorBorderColor));

    messageFont.setPointSizeF((messageFont.pointSizeF() - 2) * Application::fontScaleFactor());
    QFontMetricsF messageFontMetrics(messageFont);
    float messageWidth  = messageFontMetrics.horizontalAdvance(errorMessage);
    float messageAscent = messageFontMetrics.ascent();
    float messageHeight = messageFontMetrics.height();

    QRectF  boundingRectangle = chartItem()->boundingRect();
    QPointF textLocation;
    if (boundingRectangle.isValid()) {
        textLocation = QPointF(
            std::max(0.0F, static_cast<float>(boundingRectangle.width() - messageWidth) / 2.0F),
            std::max(0.0F, static_cast<float>(boundingRectangle.height() - messageHeight) / 2.0F) + messageAscent
        );
    } else {
        textLocation = QPointF(0.0, messageAscent);
    }

    if (group->numberTextEntries() == 0) {
        group->append(errorMessage, messageFont, textLocation);
    } else {
        group->entry(0).setText(errorMessage);
    }
}


void PlotWrappedPresentationData::clearErrorMessage() {
    EQt::GraphicsMultiTextGroup* group = dynamic_cast<EQt::GraphicsMultiTextGroup*>(graphicsItem());
    group->clearText();
    group->setBorderPen(QPen(Qt::PenStyle::NoPen));
}


void PlotWrappedPresentationData::setChartItem(EQt::GraphicsItem* chartItem) {
    EQt::GraphicsMultiTextGroup* group = dynamic_cast<EQt::GraphicsMultiTextGroup*>(graphicsItem());

    if (currentChartItem != Q_NULLPTR) {
        group->removeFromGroup(currentChartItem);
    }

    if (currentChartGraphicsItem != Q_NULLPTR) {
        currentChartGraphicsItem->deleteLater();
    }

    currentChartGraphicsItem = chartItem;
    currentChartItem         = dynamic_cast<QChart*>(chartItem);

    if (currentChartItem != Q_NULLPTR) {
        group->addToGroup(currentChartItem);
        currentChartItem->setPos(0, 0);
    }
}


QChart* PlotWrappedPresentationData::chartItem() const {
    return currentChartItem;
}
