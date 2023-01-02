/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref HeatChartPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef HEAT_CHART_PRESENTATION_DATA_H
#define HEAT_CHART_PRESENTATION_DATA_H

#include <QCoreApplication>
#include <QImage>

#include <eqt_charts.h>
#include <ld_plot_series.h>

#include "app_common.h"
#include "plot_wrapped_presentation_data.h"

namespace Model {
    class MatrixBoolean;
    class MatrixInteger;
    class MatrixReal;
    class MatrixComplex;
}

namespace EQt {
    class ChartItem;
    class GraphicsPixmapItem;
}

namespace Ld {
    class ChartAxisFormat;
}

/**
 * The \ref PlotPresentationData instance to use for heat charts.
 */
class APP_PUBLIC_API HeatChartPresentationData:public PlotWrappedPresentationData {
    Q_DECLARE_TR_FUNCTIONS(HeatChartPresentationData)

    public:
        /**
         * The series gradient.
         */
        typedef Ld::PlotSeries::GradientType GradientType;

        /**
         * Constructor
         *
         * \param[in] graphicsItem The chart item containing the plot.
         */
        HeatChartPresentationData(EQt::ChartItem* graphicsItem);

        ~HeatChartPresentationData() override;

        /**
         * Method you can call to report a new calculated value to the plot.
         *
         * \param[in] valueIndex      The index used to reference the calculated value.
         *
         * \param[in] calculatedValue The new calculated value.
         */
        void calculatedValueUpdated(unsigned valueIndex, const Ld::CalculatedValue& calculatedValue) override;

        /**
         * Method you can call to clear all the reported calculated values.
         */
        void calculatedValuesCleared() override;

        /**
         * Method you can use to set the series information for this plot.
         *
         * \param[in] variableName The name of the data source tied to the series.
         *
         * \param[in] seriesName   The label tied to the series.
         *
         * \param[in] seriesColor  The color to apply to the series.
         *
         * \param[in] gradientType The series gradient style.
         */
        void setSeries(
            const Ld::VariableName& variableName,
            const QString&          seriesName,
            const QColor&           seriesColor,
            GradientType            gradientType
        );

        /**
         * Method you can use to set the left axis format.
         *
         * \param[in] axis       The axis tied to this format.
         *
         * \param[in] axisFormat The new bottom axis format.
         */
        void setLeftAxisFormat(QValueAxis* axis, const Ld::ChartAxisFormat& axisFormat);

        /**
         * Method you can use to set the bottom axis format.
         *
         * \param[in] axis       The axis tied to this format.
         *
         * \param[in] axisFormat The new bottom axis format.
         */
        void setBottomAxisFormat(QValueAxis* axis, const Ld::ChartAxisFormat& axisFormat);

    private:
        /**
         * Method that is called to update the plot.
         */
        void updatePlot();

        /**
         * Method that is called to convert a boolean matrix to a heat map.
         *
         * \param[in] matrix       The matrix to be converted.
         *
         * \param[in] color        The series color.
         *
         * \param[in] gradientType The gradient type.
         *
         * \return Returns an image.
         */
        static QImage matrixToPixmap(
            const Model::MatrixBoolean& matrix,
            const QColor&               color,
            GradientType                gradientType
        );

        /**
         * Method that is called to convert an integer matrix to a heat map.
         *
         * \param[in] matrix       The matrix to be converted.
         *
         * \param[in] color        The series color.
         *
         * \param[in] gradientType The gradient type.
         *
         * \return Returns an image.
         */
        static QImage matrixToPixmap(
            const Model::MatrixInteger& matrix,
            const QColor&               color,
            GradientType                gradientType
        );

        /**
         * Method that is called to convert a real matrix to a heat map.
         *
         * \param[in] matrix       The matrix to be converted.
         *
         * \param[in] color        The series color.
         *
         * \param[in] gradientType The gradient type.
         *
         * \return Returns an image.
         */
        static QImage matrixToPixmap(
            const Model::MatrixReal& matrix,
            const QColor&            color,
            GradientType             gradientType
        );

        /**
         * Method that is called to convert a complex matrix to a heat map.
         *
         * \param[in] matrix       The matrix to be converted.
         *
         * \param[in] color        The series color.
         *
         * \param[in] gradientType The gradient type.
         *
         * \return Returns an image.
         */
        static QImage matrixToPixmap(
            const Model::MatrixComplex& matrix,
            const QColor&               color,
            GradientType                gradientType
        );

        /**
         * Method that called to configure an axis from an axis format.
         *
         * \param[in] axis         The axis to be formateed.
         *
         * \param[in] axisLocation The axis location.
         *
         * \param[in] axisFormat   The axis format used to format the axis.
         */
        void configureAxis(QValueAxis* axis, AxisLocation axisLocation, const Ld::ChartAxisFormat& axisFormat);

        /**
         * The current "dummy" data series.  We use this series to force axis labels to be drawn.
         */
        QScatterSeries* dummySeries;

        /**
         * The pixmap item.
         */
        EQt::GraphicsPixmapItem* currentPixmapItem;

        /**
         * The series variable name.  Mostly provided for debugging purposes.
         */
        Ld::VariableName currentSeriesVariableName;

        /**
         * The series label.
         */
        QString currentSeriesLabel;

        /**
         * The series color.
         */
        QColor currentSeriesColor;

        /**
         * The series gradient type.
         */
        GradientType currentGradientType;

        /**
         * The calculated value to generate the heat chart from.
         */
        Ld::CalculatedValue currentCalculatedValue;

        /**
         * The bottom axis.
         */
        QValueAxis* currentBottomAxis;

        /**
         * The bottom axis format.
         */
        Ld::ChartAxisFormat currentBottomAxisFormat;

        /**
         * The bottom axis.
         */
        QValueAxis* currentleftAxis;

        /**
         * The bottom axis format.
         */
        Ld::ChartAxisFormat currentLeftAxisFormat;
};

#endif
