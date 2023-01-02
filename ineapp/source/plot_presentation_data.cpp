/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PlotPresentationData class.
***********************************************************************************************************************/

#include <QGraphicsItem>

#include <utility>
#include <cmath>

#include <eqt_graphics_item.h>

#include "plot_presentation_data.h"

/***********************************************************************************************************************
 * PlotPresentationData::SeriesMinMax
 */

void PlotPresentationData::SeriesMinMax::update(double candidateValue) {
    if (!std::isinf(candidateValue) && !std::isnan(candidateValue)) {
        if (candidateValue < currentMinimum) {
            currentMinimum = candidateValue;
        }

        if (candidateValue > currentMaximum) {
            currentMaximum = candidateValue;
        }
    }
}

void PlotPresentationData::SeriesMinMax::update(const PlotPresentationData::SeriesMinMax& other) {
    if (other.currentMinimum < currentMinimum) {
        currentMinimum = other.currentMinimum;
    }

    if (other.currentMaximum > currentMaximum) {
        currentMaximum = other.currentMaximum;
    }
}

/***********************************************************************************************************************
 * PlotPresentationData
 */

const unsigned PlotPresentationData::multiplierScaleFactors[] = {
       1,   2,   3,   5,
      10,  20,  30,  50,
     100, 200, 300, 500,
    1000,
    0
};

PlotPresentationData::PlotPresentationData(EQt::GraphicsItem* graphicsItem) {
    currentGraphicsItem = dynamic_cast<QGraphicsItem*>(graphicsItem);
}


PlotPresentationData::~PlotPresentationData() {
    if (currentGraphicsItem != Q_NULLPTR) {
        dynamic_cast<EQt::GraphicsItem*>(currentGraphicsItem)->deleteLater();
    }
}


void PlotPresentationData::setGraphicsItem(EQt::GraphicsItem* newGraphicsItem) {
    EQt::GraphicsItem* castGraphicsItem = dynamic_cast<EQt::GraphicsItem*>(currentGraphicsItem);
    if (currentGraphicsItem != Q_NULLPTR && newGraphicsItem != castGraphicsItem) {
        castGraphicsItem->deleteLater();
    }

    currentGraphicsItem = dynamic_cast<QGraphicsItem*>(newGraphicsItem);
}


QGraphicsItem* PlotPresentationData::graphicsItem() const {
    return currentGraphicsItem;
}


PlotPresentationData::SeriesMinMax PlotPresentationData::calculateNiceRange(
        PlotPresentationData::AxisLocation /* axisLocation */,
        double                             measuredMinimum,
        double                             measuredMaximum,
        PlotPresentationData::AxisScale    axisScale
    ) const {
    double recommendedMinimum;
    double recommendedMaximum;

    if (axisScale == AxisScale::LOG) {
        if (measuredMinimum <= 0) {
            recommendedMinimum = std::numeric_limits<double>::min();
        } else {
            double powerOf10 = std::floor(std::log10(measuredMinimum));
            recommendedMinimum = std::pow(10, powerOf10);
        }

        if (measuredMaximum <= 0) {
            recommendedMaximum = std::numeric_limits<double>::min();
        } else {
            double powerOf10 = std::ceil(std::log10(measuredMaximum));
            recommendedMaximum = std::pow(10, powerOf10);
        }
    } else {
        if (measuredMinimum == measuredMaximum) {
            recommendedMinimum = measuredMinimum - 0.5;
            recommendedMaximum = measuredMaximum + 0.5;
        } else {
            double span             = std::abs(measuredMaximum - measuredMinimum);
            double magnitudeMinimum = std::abs(measuredMinimum);
            double magnitudeMaximum = std::abs(measuredMaximum);

            double distance;
            if (20.0 * magnitudeMinimum < magnitudeMaximum) {
                distance = magnitudeMaximum;
            } else if (span < magnitudeMinimum) {
                distance = span;
            } else {
                distance = magnitudeMinimum;
            }

            double powerOf10  = std::floor(std::log10(distance)) - 1;
            double rounding1  = std::pow(10, powerOf10);
            double rounding2  = 2.0 * rounding1;
            double rounding5  = 5.0 * rounding1;
            double rounding10 = 10.0 * rounding1;
            double score1     = std::abs(distanceThreshold - (span / rounding1));
            double score2     = std::abs(distanceThreshold - (span / rounding1));
            double score5     = std::abs(distanceThreshold - (span / rounding1));
            double score10    = std::abs(distanceThreshold - (span / rounding10));

            double rounding;
            if (score1 < score2 && score1 < score5 && score1 < score10) {
                rounding = rounding1;
            } else if (score2 < score1 && score2 < score5 && score2 < score10) {
                rounding = rounding2;
            } else if (score5 < score1 && score5 < score2 && score5 < score10) {
                rounding = rounding5;
            } else {
                rounding = rounding10;
            }

            recommendedMinimum = rounding * std::floor(measuredMinimum / rounding);
            recommendedMaximum = rounding * std::ceil(measuredMaximum / rounding);

            if (axisScale == AxisScale::LINEAR) {
                if (std::isinf(recommendedMinimum)) {
                    if (recommendedMinimum < 0) {
                        recommendedMinimum = -std::numeric_limits<double>::max();
                    } else {
                        recommendedMinimum = +std::numeric_limits<double>::max();
                    }
                }

                if (std::isinf(recommendedMaximum)) {
                    if (recommendedMaximum < 0) {
                        recommendedMaximum = -std::numeric_limits<double>::max();
                    } else {
                        recommendedMaximum = +std::numeric_limits<double>::max();
                    }
                }
            } else {
                Q_ASSERT(axisScale == AxisScale::DB_10_LOG || axisScale == AxisScale::DB_20_LOG);

                if (recommendedMinimum < -200) {
                    recommendedMinimum = -200;
                }

                if (recommendedMaximum > 200) {
                    recommendedMaximum = 200;
                }
            }
        }
    }

    return SeriesMinMax(recommendedMinimum, recommendedMaximum);
}


PlotPresentationData::SeriesStepSizes PlotPresentationData::calculateNiceStepSizes(
        PlotPresentationData::AxisLocation axisLocation,
        double                             span,
        float                              maximumNumberTicks
    ) const {
    Q_ASSERT(span > 0);

    double   majorStepSize;
    double   minorStepSize;

    double   powerOf10       = std::floor(std::log10(span));
    double   baseScaleFactor = std::pow(10, 1 - powerOf10);
    unsigned maximumTicks    = static_cast<unsigned>(maximumNumberTicks);
    unsigned optimalTicks    = static_cast<unsigned>(0.5 + maximumNumberTicks / 2);
    unsigned maximumD        = std::min(optimalTicks , maximumTicks - optimalTicks);

    unsigned d;
    unsigned normalized;
    unsigned scaleFactor;

    const unsigned* multiplierScaleFactor = multiplierScaleFactors;
    do {
        scaleFactor = baseScaleFactor * (*multiplierScaleFactor);
        normalized  = static_cast<unsigned>(span * scaleFactor);
        d           = 0;

        while (d < maximumD && (normalized % (optimalTicks - d) != 0) && (normalized % (optimalTicks + d) != 0)) {
            ++d;
        }

        ++multiplierScaleFactor;
    } while (d >= maximumD && *multiplierScaleFactor != 0);

    unsigned numberMajorTicks;
    if (d < maximumD) {
        if (normalized % (optimalTicks - d) == 0) {
            numberMajorTicks = optimalTicks - d;
        } else {
            numberMajorTicks = optimalTicks + d;
        }
    } else {
        numberMajorTicks = optimalTicks;
    }

    majorStepSize = ((1.0 * normalized) / (1.0 * scaleFactor)) / numberMajorTicks;
    minorStepSize = calculateMinorStepSize(axisLocation, majorStepSize, numberMajorTicks, maximumTicks);

    return SeriesStepSizes(majorStepSize, minorStepSize);
}


double PlotPresentationData::calculateMinorStepSize(
        PlotPresentationData::AxisLocation /* axisLocation */,
        double                             majorStepSize,
        unsigned                           numberMajorTicks,
        unsigned                           maximumNumberTicks
    ) const {
    unsigned maximumMinorTicks    = 2 * maximumNumberTicks;
    unsigned maximumMinorPerMajor = static_cast<unsigned>(0.5 + (1.0 * maximumMinorTicks) / numberMajorTicks);

    unsigned minorPerMajor;
    if (maximumMinorTicks > 10) {
        minorPerMajor = 10;
    } else if (maximumMinorPerMajor > 5) {
        minorPerMajor = 5;
    } else if (maximumMinorPerMajor > 4) {
        minorPerMajor = 4;
    } else if (maximumMinorPerMajor > 2) {
        minorPerMajor = 2;
    } else {
        minorPerMajor = 1;
    }

    return majorStepSize / minorPerMajor;
}


std::pair<double, double> PlotPresentationData::calculateOLSLinearRegression(const QVector<QPointF>& points) {
    double sumX = 0;
    double sumY = 0;

    unsigned       numberPoints = static_cast<unsigned>(points.size());
    const QPointF* rawData      = points.data();
    for (unsigned i=0 ; i<numberPoints ; ++i) {
        sumX += rawData->x();
        sumY += rawData->y();
    }

    double meanX = sumX / numberPoints;
    double meanY = sumY / numberPoints;

    double covariance = 0; // Times number of samples
    double varianceX  = 0; // Times number of samples
    rawData = points.data();
    for (unsigned i=0 ; i<numberPoints ; ++i) {
        double dx = rawData->x() - meanX;
        double dy = rawData->y() - meanY;

        covariance += dx * dy;
        varianceX  += dx * dx;

        ++rawData;
    }

    double slope      = covariance / varianceX;
    double yIntercept = meanY - slope * meanX;

    return std::make_pair(slope, yIntercept);
}
