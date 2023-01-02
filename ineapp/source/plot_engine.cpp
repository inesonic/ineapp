/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotEngine class.
***********************************************************************************************************************/

#include <QCoreApplication>
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QString>
#include <QIcon>

#include <limits>

#include <util_hash_functions.h>

#include <ld_plot_element.h>
#include <ld_plot_format.h>
#include <ld_calculated_value.h>

#include "scene_units.h"
#include "plot_presentation_data.h"
#include "plot_engine.h"

QMap<QString, const PlotEngine*> PlotEngine::plotEnginesByName;
QList<const PlotEngine*>         PlotEngine::plotEngines;

PlotEngine::PlotEngine() {}


PlotEngine::~PlotEngine() {}


bool PlotEngine::registerInstance(const PlotEngine* plotEngine) {
    bool result;

    if (!plotEnginesByName.contains(plotEngine->name())) {
        plotEnginesByName.insert(plotEngine->name(), plotEngine);
        plotEngines.append(plotEngine);

        result = true;
    } else {
        result = false;
    }

    return result;
}


const PlotEngine* PlotEngine::byName(const QString& plotStyle) {
    return plotEnginesByName.value(plotStyle, Q_NULLPTR);
}


const QList<const PlotEngine*>& PlotEngine::engines() {
    return plotEngines;
}


unsigned PlotEngine::minimumNumberAllowedDataSeries() const {
    return 1;
}


unsigned PlotEngine::maximumNumberAllowedDataSeries() const {
    return std::numeric_limits<int>::max();
}


bool PlotEngine::activeAreaBackgroundAdjustable() const {
    return true;
}


bool PlotEngine::supportsPlotLegend() const {
    return true;
}


bool PlotEngine::hasDistinctActiveArea() const {
    return true;
}


bool PlotEngine::supportsPlotTitle() const {
    return true;
}


bool PlotEngine::supportsControllableBackgroundArea() const {
    return true;
}


PlotPresentationData* PlotEngine::updatePresentationData(
        PlotPresentationData*           /* currentGraphicsItem */,
        QSharedPointer<Ld::PlotElement> plotElement,
        QSharedPointer<Ld::PlotFormat>  plotFormat
    ) const {
    PlotPresentationData* newPresentationData =  createPresentationData(plotElement, plotFormat);

    unsigned valueIndex                = 0;
    unsigned numberDataSeries          = plotElement->numberDataSeries();
    bool     hasInvalidCalculatedValue = false;
    unsigned seriesIndex               = 0;
    while (!hasInvalidCalculatedValue && seriesIndex < numberDataSeries) {
        unsigned numberSources = plotElement->numberDataSources(seriesIndex);
        unsigned sourceIndex   = 0;
        while (!hasInvalidCalculatedValue && sourceIndex < numberSources) {
            Ld::CalculatedValue calculatedValue = plotElement->calculatedValue(valueIndex);
            hasInvalidCalculatedValue = calculatedValue.isInvalid();
            ++valueIndex;
            ++sourceIndex;
        }

        ++seriesIndex;
    }

    if (!hasInvalidCalculatedValue) {
        valueIndex = 0;
        for (seriesIndex=0 ; seriesIndex<numberDataSeries ; ++seriesIndex) {
            unsigned numberSources = plotElement->numberDataSources(seriesIndex);
            for (unsigned sourceIndex=0 ; sourceIndex<numberSources ; ++sourceIndex) {
                Ld::CalculatedValue calculatedValue = plotElement->calculatedValue(valueIndex);
                newPresentationData->calculatedValueUpdated(valueIndex, calculatedValue);
                ++valueIndex;
            }
        }
    }

    return newPresentationData;
}


Util::HashResult qHash(PlotEngine::PlotSeriesMode value, Util::HashSeed seed) {
    return qHash(static_cast<int>(value), seed);
}
