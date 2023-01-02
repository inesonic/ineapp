/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DonutChartPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DONUT_CHART_PRESENTATION_DATA_H
#define DONUT_CHART_PRESENTATION_DATA_H

#include <QObject>

#include "app_common.h"
#include "pie_chart_presentation_data.h"

/**
 * The \ref PlotPresentationData instance to use for donut charts.
 */
class APP_PUBLIC_API DonutChartPresentationData:public PieChartPresentationData {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The chart item containing the plot.
         */
        DonutChartPresentationData(EQt::ChartItem* graphicsItem);

        ~DonutChartPresentationData() override;

    protected:
        /**
         * Method that determines the diameter of a given pie or donut.
         *
         * \param[in] seriesIndex  The series index.  The outside pie will have a value of 0.
         *
         * \param[in] numberSeries The number of series.
         *
         * \return Returns the relative diameter of the pie.
         */
        double pieRelativeSize(unsigned seriesIndex, unsigned numberSeries) const override;

        /**
         * Method that determines the diameter of a hole within a given pie.
         *
         * \param[in] seriesIndex  The series index.  The outside pie will have a value of 0.
         *
         * \param[in] numberSeries The number of series.
         *
         * \return Returns the relative diameter of the hole in the center of the pie.
         */
        double holeRelativeSize(unsigned seriesIndex, unsigned numberSeries) const override;
};

#endif
