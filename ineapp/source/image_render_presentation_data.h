/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImageRenderPresentationData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMAGE_RENDER_PRESENTATION_DATA_H
#define IMAGE_RENDER_PRESENTATION_DATA_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QPen>
#include <QSizeF>
#include <QRectF>
#include <QPixmap>

#include <model_matrix_integer.h>
#include <model_matrix_real.h>

#include <ld_plot_series.h>

#include "app_common.h"
#include "scene_units.h"
#include "plot_wrapped_presentation_data.h"

namespace Model {
    class MatrixBoolean;
    class MatrixInteger;
    class MatrixReal;
    class MatrixComplex;
}

namespace EQt {
    class GraphicsMultiTextGroup;
    class GraphicsPixmapItem;
}

namespace Ld {
    class ChartAxisFormat;
}

/**
 * Pure virtual common base class for plot-like image rendering.
 */
class APP_PUBLIC_API ImageRenderPresentationData:public QObject, public PlotPresentationData, private SceneUnits {
    Q_OBJECT

    public:
        ImageRenderPresentationData();

        ~ImageRenderPresentationData() override;

        /**
         * Method you can call to configure this image render presentation data instance.
         *
         * \param[in] groupItem    The group item used to handle error messages and borders.  This class takes
         *                         ownership of the group item.
         *
         * \param[in] pixmapItem   The pixmap item used to render the image.  This class takes ownership of the
         *                         pixmap item.
         *
         * \param[in] sizeInPoints The size of the image, in scene units.
         *
         * \param[in] borderPen    The border pen.
         *
         * \param[in] defaultImage The default image to render.
         */
        void configure(
            EQt::GraphicsMultiTextGroup* groupItem,
            EQt::GraphicsPixmapItem*     pixmapItem,
            const QSizeF&                sizeInPoints,
            const QPen&                  borderPen,
            const QPixmap&               defaultImage
        );

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
         * Method you can use to obtain the bounding rectangle for this plot.
         *
         * \return Returns the bounding rectangle for this plot.  This version returns the bounding rectangle of the
         *         chart item.
         */
        QRectF boundingRectangle() const override;

        /**
         * Method you can use to obtain the scene bounding rectangle for this plot.
         *
         * \return Returns the scene bounding rectangle for this plot.  This version returns the scene bounding
         *         rectangle of the chart item.
         */
        QRectF sceneBoundingRectangle() const override;

        /**
         * Method you can use to set an error message.
         *
         * \param[in] errorMessage The error message to be displayed.
         */
        void showErrorMessage(const QString& errorMessage);

        /**
         * Method you can use to clear any reported error message.
         */
        void clearErrorMessage();

        /**
         * Method that indicates the number of series for this type of image.
         *
         * \return Returns the number of data series for this type of image.
         */
        virtual unsigned numberSeries() const = 0;

        /**
         * Method that renders an image using a collection of real matrices.
         *
         * \param[in]  matrixByAxisLocation A map of matrices based on axis location.
         *
         * \param[out] errorString          An error string that is populated if an error is found.
         *
         * \return Returns the rendered image.
         */
        virtual QImage render(
            const QMap<AxisLocation, Model::MatrixInteger>& matrixByAxisLocation,
            QString&                                        errorString
        ) const = 0;

        /**
         * Method that renders an image using a collection of real matrices.
         *
         * \param[in]  matrixByAxisLocation A map of matrices based on axis location.
         *
         * \param[out] errorString          An error string that is populated if an error is found.
         *
         * \return Returns the rendered image.
         */
        virtual QImage render(
            const QMap<AxisLocation, Model::MatrixReal>& matrixByAxisLocation,
            QString&                                     errorString
        ) const = 0;

        /**
         * Method you can use to set the series information for this plot.
         *
         * \param[in] seriesIndex  The zero based series index used to configure the series.
         *
         * \param[in] valueIndex   The value index tied to the series index.
         *
         * \param[in] axisLocation The axis location, indicates the color value.
         */
        void setSeries(unsigned seriesIndex, unsigned valueIndex, AxisLocation axisLocation);

    private slots:
        /**
         * Slot that is triggered to render the image.
         */
        void performUpdate();

    protected:
        /**
         * Class that holds information about a series.
         */
        class SeriesData {
            public:
                SeriesData();

                /**
                 * Constructor
                 *
                 * \param[in] axisLocation    The series axis location.  Indicates the color tied to this series.
                 *
                 * \param[in] seriesIndex     The series index.
                 *
                 * \param[in] valueIndex      The value index.
                 *
                 * \param[in] calculatedValue The calculated value for the series.
                 */
                SeriesData(
                    AxisLocation               axisLocation,
                    unsigned                   seriesIndex,
                    unsigned                   valueIndex,
                    const Ld::CalculatedValue& calculatedValue = Ld::CalculatedValue()
                );

                /**
                 * Copy constructor
                 *
                 * \param[in] other The instance to assign to this instance.
                 */
                SeriesData(const SeriesData& other);

                ~SeriesData();

                /**
                 * Method that provides the axis location (color) to use for this series.
                 *
                 * \return Returns the axis location (color) to use for this series.
                 */
                inline AxisLocation axisLocation() const {
                    return currentAxisLocation;
                }

                /**
                 * Method that provides the series index to use for this series.
                 *
                 * \return Returns the series index used for this series.
                 */
                inline unsigned seriesIndex() const {
                    return currentSeriesIndex;
                }

                /**
                 * Method that provides the value index to use for this series.
                 *
                 * \return Returns the value index to use for this series.
                 */
                inline unsigned valueIndex() const {
                    return currentValueIndex;
                }

                /**
                 * Method that assigns a new calculated value to this series.
                 *
                 * \param[in] newCalculatedValue The new calculated value.
                 */
                inline void setCalculatedValue(const Ld::CalculatedValue& newCalculatedValue) {
                    currentCalculatedValue = newCalculatedValue;
                }

                /**
                 * Method that provides the current calculated value for the series.
                 *
                 * \return Returns the calculated value for the series.
                 */
                inline const Ld::CalculatedValue& calculatedValue() const {
                    return currentCalculatedValue;
                }

                /**
                 * Assignment operator.
                 *
                 * \param[in] other The instance to assign to this instance.
                 *
                 * \return Returns a reference to this instance.
                 */
                SeriesData& operator=(const SeriesData& other);

            private:
                /**
                 * The current axis location.
                 */
                AxisLocation currentAxisLocation;

                /**
                 * The current series index.
                 */
                unsigned currentSeriesIndex;

                /**
                 * The current value index.
                 */
                unsigned currentValueIndex;

                /**
                 * The current calculated value.
                 */
                Ld::CalculatedValue currentCalculatedValue;
        };

    private:
        /**
         * Method that places the default image into the plot area.
         */
        void drawDefaultImage();

        /**
         * The current border pen.
         */
        QPen currentBorderPen;

        /**
         * The current pixmap item.
         */
        EQt::GraphicsPixmapItem* currentPixmapItem;

        /**
         * The required size for the pixmap item.
         */
        QSizeF currentSizeInPoints;

        /**
         * The default image to display when no data is present.
         */
        QPixmap currentDefaultPixmap;

        /**
         * The current series data.
         */
        QList<SeriesData> seriesDataByValueIndex;

        /**
         * Timer used to trigger deferred updates.
         */
        QTimer updateTimer;
};

#endif
