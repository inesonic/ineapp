/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref XyPlotPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef XY_PLOT_PRESENTATION_DATA_H
#define XY_PLOT_PRESENTATION_DATA_H

#include <QObject>

#include "app_common.h"
#include "plot_2d_presentation_data.h"

namespace EQt {
    class ChartItem;
}

/**
 * The \ref PlotPresentationData instance to use for XY plots.
 */
class APP_PUBLIC_API XyPlotPresentationData:public Plot2DPresentationData {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] graphicsItem The chart item containing the plot.
         */
        XyPlotPresentationData(EQt::ChartItem* graphicsItem);

        ~XyPlotPresentationData() override;
};

#endif
