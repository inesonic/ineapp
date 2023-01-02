/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref DonutChartPresentationData class.
***********************************************************************************************************************/

#include <QObject>

#include <eqt_chart_item.h>

#include <ld_variable_name.h>
#include <ld_calculated_value.h>
#include <ld_data_type.h>

#include "pie_chart_presentation_data.h"
#include "donut_chart_presentation_data.h"

DonutChartPresentationData::DonutChartPresentationData(
        EQt::ChartItem* chartItem
    ):PieChartPresentationData(
        chartItem
    ) {}


DonutChartPresentationData::~DonutChartPresentationData() {}


double DonutChartPresentationData::pieRelativeSize(unsigned seriesIndex, unsigned numberSeries) const {
    double result;

    if (numberSeries < 2) {
        result = 0.8;
    } else {
        result = 0.40 * (1 + (numberSeries - seriesIndex - 1.0) / (numberSeries - 1.0));
    }

    return result;
}


double DonutChartPresentationData::holeRelativeSize(unsigned seriesIndex, unsigned numberSeries) const {
    double result;

    if (numberSeries < 2) {
        result = 0.6;
    } else {
        result = 0.40 * (1 + (numberSeries - seriesIndex - 2.0) / (numberSeries - 1.0));
    }

    return result;
}
