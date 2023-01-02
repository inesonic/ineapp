/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Polar2DPlotPresentationData class.
***********************************************************************************************************************/

#include <QObject>

#include <utility>
#include <cmath>

#include <eqt_polar_2d_chart_item.h>

#include "plot_2d_presentation_data.h"
#include "polar_2d_plot_presentation_data.h"

static const double pi = 4.0 * std::atan(1.0);

Polar2DPlotPresentationData::Polar2DPlotPresentationData(
        EQt::Polar2DChartItem* chartItem
    ):Plot2DPresentationData(
        chartItem
    ) {}


Polar2DPlotPresentationData::~Polar2DPlotPresentationData() {}


Polar2DPlotPresentationData::SeriesMinMax Polar2DPlotPresentationData::calculateNiceRange(
        Polar2DPlotPresentationData::AxisLocation axisLocation,
        double                                    measuredMinimum,
        double                                    measuredMaximum,
        Polar2DPlotPresentationData::AxisScale    axisScale
    ) const {
    SeriesMinMax result;

    if (axisLocation == AxisLocation::LEFT_Y_R_RC || axisLocation == AxisLocation::RIGHT_Y_R_RY) {
        result = Plot2DPresentationData::calculateNiceRange(axisLocation, measuredMinimum, measuredMaximum, axisScale);
    } else {
        double span = std::abs(measuredMaximum - measuredMinimum);
        if (span < pi) {
            result = Plot2DPresentationData::calculateNiceRange(
                 axisLocation,
                 measuredMinimum,
                 measuredMaximum,
                 AxisScale::LINEAR
             );
        } else if (span <= 2.0002 * pi) {
            if (measuredMinimum < -0.0001 * pi) {
                result = SeriesMinMax(-pi, +pi);
            } else {
                result = SeriesMinMax(0, 2.0 * pi);
            }
        } else if (span > 180.0 && span <= 360.036) {
            if (measuredMinimum < -0.018) {
                result = SeriesMinMax(-180, +180);
            } else {
                result = SeriesMinMax(0, 360.0);
            }
        } else {
            result = Plot2DPresentationData::calculateNiceRange(
                axisLocation,
                measuredMinimum,
                measuredMaximum,
                AxisScale::LINEAR
            );
        }
    }

    return result;
}


Polar2DPlotPresentationData::SeriesStepSizes Polar2DPlotPresentationData::calculateNiceStepSizes(
        Polar2DPlotPresentationData::AxisLocation axisLocation,
        double                                    span,
        float                                     maximumNumberTicks
    ) const {
    SeriesStepSizes result;

    if (axisLocation == AxisLocation::LEFT_Y_R_RC || axisLocation == AxisLocation::RIGHT_Y_R_RY) {
        result = Plot2DPresentationData::calculateNiceStepSizes(axisLocation, span, maximumNumberTicks);
    } else {
        float maximumNumberAngularTicks = 6.28 * maximumNumberTicks;
        if (std::abs(span - 2.0 * pi) < 0.0001 * pi || std::abs(span - 360.0) < 0.018) {
            if           (maximumNumberAngularTicks >= 96) {
                result = SeriesStepSizes(span / 48.0, span / 96.0);
            } else if (maximumNumberAngularTicks >= 80) {
                result = SeriesStepSizes(span / 40.0, span / 80.0);
            } else if (maximumNumberAngularTicks >= 72) {
                result = SeriesStepSizes(span / 36.0, span / 72.0);
            } else if (maximumNumberAngularTicks >= 48) {
                result = SeriesStepSizes(span / 24.0, span / 48.0);
            } else if (maximumNumberAngularTicks >= 40) {
                    result = SeriesStepSizes(span / 20.0, span / 40.0);
            } else if (maximumNumberAngularTicks >= 36) {
                result = SeriesStepSizes(span / 18.0, span / 36.0);
            } else if (maximumNumberAngularTicks >= 24) {
                result = SeriesStepSizes(span / 12.0, span / 24.0);
            } else if (maximumNumberAngularTicks >= 16) {
                result = SeriesStepSizes(span / 8.0, span / 16.0);
            } else if (maximumNumberAngularTicks >= 12) {
                result = SeriesStepSizes(span / 4.0, span / 12.0);
            }
        } else {
            result = Plot2DPresentationData::calculateNiceStepSizes(axisLocation, span, maximumNumberAngularTicks);
        }
    }

    return result;
}


bool Polar2DPlotPresentationData::isModuloAxis(AxisLocation axisLocation) const {
    return axisLocation == AxisLocation::BOTTOM_X_A_GM || axisLocation == AxisLocation::TOP_X_A_GM;
}
